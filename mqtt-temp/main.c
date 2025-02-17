#include <stdio.h> 
#include <math.h>
#include <string.h>
#include "timex.h"
#include "ztimer.h"
#include "paho_mqtt.h"
#include "MQTTClient.h"
#include "saul_reg.h"
#include "saul.h"
#include "thread.h"
#include <sys/select.h>
#include <unistd.h>
#include "msg.h"

// Puffergröße für MQTT-Nachrichten
#define BUF_SIZE 1024
// MQTT-Protokollversion
#define MQTT_VERSION_v311 4
// Timeout für MQTT-Befehle in Millisekunden
#define COMMAND_TIMEOUT_MS 4000
// Standard-IP-Adresse des MQTT-Brokers
#define DEFAULT_MQTT_BROKER_IP "2600:1f18:2a2e:423c:ef89:ece:f856:46db"
// Standard Client-ID für die Verbindung zum Broker
#define DEFAULT_CLIENT_ID "RIOTClient"
// Standard-Port des MQTT-Brokers
#define MQTT_PORT 1883
// MQTT-Thema für eingehende Nachrichten
#define MQTT_TOPIC "test/topic"
// MQTT-Thema für Antworten (z.B. Temperatur)
#define RESPONSE_TOPIC "temp/topic"
// Zeitintervall für das Senden von Keep-Alive-Nachrichten an den Broker (in Sekunden)
#define KEEP_ALIVE_SEC 10
// Gibt an, ob eine neue Sitzung bei jeder Verbindung erstellt werden soll
#define IS_CLEAN_SESSION 1
// Gibt an, ob veröffentlichte Nachrichten gespeichert werden sollen
#define IS_RETAINED_MSG 0
// Maximale Anzahl an Verbindungsversuchen
#define RETRY_LIMIT 3

// MQTT-Client und Netzwerkverbindung
static MQTTClient client;
static Network network;
// Speicherpuffer für das Senden und Empfangen von MQTT-Nachrichten
static unsigned char buf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];
// IP-Adresse des Brokers und Client-ID, die zur Laufzeit geändert werden können
static char mqtt_broker_ip[256] = DEFAULT_MQTT_BROKER_IP;
static char client_id[256] = DEFAULT_CLIENT_ID;

// Stack und Thread-ID für den MQTT-Thread
static char mqtt_thread_stack[MQTT_THREAD_STACKSIZE];
static kernel_pid_t mqtt_pid;

// Liest die Temperatur vom NRF-Sensor aus
static int read_temperature(float *temperature) {
    // Sucht nach einem Sensor mit dem Namen "NRF_TEMP"
    saul_reg_t *sensor = saul_reg_find_name("NRF_TEMP");
    if (sensor == NULL) {
        printf("Sensor not found\n");
        return -1;
    }
    phydat_t temp_value;
    // Liest die Temperaturwerte aus dem Sensor
    if (sensor->driver->read(sensor, &temp_value) < 0) {
        printf("Failed to read temperature\n");
        return -1;
    }
    // Berechnet die Temperatur basierend auf Skalenfaktor
    *temperature = temp_value.val[0] * pow(10, temp_value.scale);
    return 0;
}

// Callback-Funktion, die bei empfangenen MQTT-Nachrichten aufgerufen wird
static void _on_msg_received(MessageData *data) {
    printf("Message received on topic %.*s: %.*s\n", 
           (int)data->topicName->lenstring.len,
           data->topicName->lenstring.data,
           (int)data->message->payloadlen,
           (char *)data->message->payload);
    
    // Prüft, ob die Nachricht den Befehl "/temp" enthält
    if (strncmp((char *)data->message->payload, "/temp", data->message->payloadlen) == 0) {
        float temperature = 0.0;
        // Liest die Temperatur aus und sendet eine Antwort
        if (read_temperature(&temperature) < 0) {
            return;
        }
        char temp_response[64];
        snprintf(temp_response, sizeof(temp_response), "Temperature: %d °C", (int)temperature);
        
        MQTTMessage message;
        message.qos = QOS0; // QoS 0: Keine Zustellgarantie
        message.retained = IS_RETAINED_MSG;
        message.payload = temp_response;
        message.payloadlen = strlen(temp_response);
        
        int rc = MQTTPublish(&client, RESPONSE_TOPIC, &message);
        if (rc < 0) {
            printf("Failed to publish temperature response (%d)\n", rc);
        } else {
            printf("Published temperature response: %.2f°C\n", temperature);
        }
    }
}

// Verbindet sich mit dem MQTT-Broker
static int mqtt_connect(void) {
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = MQTT_VERSION_v311;
    data.clientID.cstring = client_id;
    data.username.cstring = "";
    data.password.cstring = "";
    data.keepAliveInterval = KEEP_ALIVE_SEC;
    data.cleansession = IS_CLEAN_SESSION;

    int attempt = 0;
    int ret;
    // Wiederholt die Verbindung bis zum Erreichen des Limits
    while (attempt < RETRY_LIMIT) {
        printf("Attempting to connect to broker (%d/%d)...\n", attempt + 1, RETRY_LIMIT);
        ret = NetworkConnect(&network, mqtt_broker_ip, MQTT_PORT);
        if (ret == 0) {
            ret = MQTTConnect(&client, &data);
            if (ret == 0) {
                printf("Connected to MQTT broker at %s:%d\n", mqtt_broker_ip, MQTT_PORT);
                return 0;
            }
        }
        printf("Connection failed (%d). Retrying...\n", ret);
        NetworkDisconnect(&network);
        attempt++;
        ztimer_sleep(ZTIMER_SEC, 2);
    }
    printf("Failed to connect to MQTT broker after %d attempts.\n", RETRY_LIMIT);
    return -1;
}

//Wiederaufbau der verloren gegangenen Verbindung
static int mqtt_reconnect(void) {
    printf("Attempting to fully reset and reconnect...\n");

    //Entfernt fehlgeschlagene Verbindung
    MQTTDisconnect(&client);
    NetworkDisconnect(&network);
    
    // Cleared die buffer
    memset(buf, 0, BUF_SIZE);
    memset(readbuf, 0, BUF_SIZE);

    // Warten vor reinitialisierung
    ztimer_sleep(ZTIMER_SEC, 2);

    // Reinitialisierunf des Netzwerkes und des Clients
    NetworkInit(&network);
    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);
    
    return mqtt_connect();
}

//Abbonieren des test/topic
static int mqtt_subscribe(void) {
    int ret = MQTTSubscribe(&client, MQTT_TOPIC, QOS0, _on_msg_received);
    if (ret < 0) {
        printf("Failed to subscribe to topic %s\n", MQTT_TOPIC);
        return ret;
    }
    printf("Subscribed to topic: %s\n", MQTT_TOPIC);
    return 0;
}

//Verarbeitet Eingaben des Nutzers
static void handle_user_input(void) {
    // Buffer zur Speicherung der Benutzereingabe
    char input[128];

    // Endlosschleife zur kontinuierlichen Verarbeitung von Benutzereingaben
    while (1) {
        // Liest eine Zeile von der Standardeingabe (stdin) und speichert sie in 'input'
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Entfernt das abschließende Newline-Zeichen ('\n'), falls vorhanden
            input[strcspn(input, "\n")] = 0;

            // Überprüfung, ob der Befehl mit "setIP " beginnt
            if (strncmp(input, "setIP ", 6) == 0) {
                char new_ip[256]; // Buffer für die neue IP-Adresse
                strncpy(new_ip, input + 6, sizeof(new_ip)); // Extrahiert die neue IP-Adresse aus der Eingabe
                new_ip[sizeof(new_ip) - 1] = '\0'; // Stellt sicher, dass der String nullterminiert ist

                // Prüft, ob sich die neue IP von der aktuellen MQTT-Broker-IP unterscheidet
                if (strcmp(mqtt_broker_ip, new_ip) != 0) {
                    printf("Changing MQTT Broker IP from %s to %s\n", mqtt_broker_ip, new_ip);
                    strncpy(mqtt_broker_ip, new_ip, sizeof(mqtt_broker_ip)); // Aktualisiert die gespeicherte IP-Adresse

                    // Trennt die bestehende MQTT-Verbindung
                    NetworkDisconnect(&network);
                    MQTTDisconnect(&client);

                    // Initialisiert das Netzwerk und den MQTT-Client neu
                    NetworkInit(&network);
                    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);

                    // Versucht, sich mit dem neuen MQTT-Broker zu verbinden
                    if (mqtt_reconnect() == 0) {
                        mqtt_subscribe(); // Abonniert wieder die relevanten MQTT-Topics
                    } else {
                        printf("Failed to reconnect to the new MQTT broker.\n");
                    }
                } else {
                    printf("Already connected to this IP. No reconnection needed.\n");
                }
            }
            // Überprüfung, ob der Befehl mit "setClientID " beginnt
            else if (strncmp(input, "setClientID ", 12) == 0) {
                char new_client_id[256]; // Buffer für die neue Client-ID
                strncpy(new_client_id, input + 12, sizeof(new_client_id)); // Extrahiert die neue Client-ID
                new_client_id[sizeof(new_client_id) - 1] = '\0'; // Stellt sicher, dass der String nullterminiert ist

                // Prüft, ob sich die neue Client-ID von der aktuellen unterscheidet
                if (strcmp(client_id, new_client_id) != 0) {
                    printf("Changing MQTT Client ID from %s to %s\n", client_id, new_client_id);
                    strncpy(client_id, new_client_id, sizeof(client_id)); // Aktualisiert die gespeicherte Client-ID

                    // Trennt die bestehende MQTT-Verbindung
                    NetworkDisconnect(&network);
                    MQTTDisconnect(&client);

                    // Initialisiert das Netzwerk und den MQTT-Client neu
                    NetworkInit(&network);
                    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);

                    // Versucht, sich mit dem neuen Client-ID zu verbinden
                    if (mqtt_connect() == 0) {
                        mqtt_subscribe(); // Abonniert wieder die relevanten MQTT-Themen
                    } else {
                        printf("Failed to reconnect with the new Client ID.\n");
                    }
                } else {
                    printf("Client ID is already set. No reconnection needed.\n");
                }
            }
            // Überprüfung, ob der Benutzer "exit" eingegeben hat
            else if (strcmp(input, "exit") == 0) {
                printf("Exiting...\n");
                break; // Beendet die Schleife und damit die Funktion
            }
            // Unbekannter Befehl
            else {
                printf("Unknown command. Try 'setIP', 'setClientID', or 'exit'.\n");
            }
        }
        // Wartet 100 Millisekunden, um CPU-Auslastung zu reduzieren
        ztimer_sleep(ZTIMER_MSEC, 100);
    }
}

// Thread, der die MQTT-Verbindung überwacht und verlorene Verbindungen erneut aufbaut
static void *mqtt_thread(void *arg) {
    (void)arg;
    int retry_delay = 1;

    while (1) {
        if (!MQTTIsConnected(&client)) {
            printf("MQTT connection lost. Restarting connection...\n");
            while (mqtt_connect() != 0) {
                printf("Reconnect failed. Retrying in %d seconds...\n", retry_delay);
                ztimer_sleep(ZTIMER_SEC, retry_delay);
                retry_delay = (retry_delay < 10) ? retry_delay * 2 : 10;
            }
            mqtt_subscribe();
            printf("Reconnected successfully!\n");
            retry_delay = 1;  // Zurücksetzen des retry delay
        }
        // Wartet kurz, um Systemressourcen zu schonen
        MQTTYield(&client, 50);
        ztimer_sleep(ZTIMER_MSEC, 15);
    }
    return NULL;
}

// Hauptfunktion, die den MQTT-Client startet und den Thread für das Wiederverbinden aufruft
int main(void) {
    NetworkInit(&network);
    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);
    printf("MQTT client initialized. Default Broker IP: %s, Client ID: %s\n",
           mqtt_broker_ip, client_id);
    
    if (mqtt_connect() < 0) {
        return 1;
    }
    if (mqtt_subscribe() < 0) {
        return 1;
    }
    
    mqtt_pid = thread_create(mqtt_thread_stack, sizeof(mqtt_thread_stack),
                              THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                              mqtt_thread, NULL, "MQTT thread");
    
    if (mqtt_pid <= KERNEL_PID_UNDEF) {
        printf("Failed to create MQTT thread\n");
        return 1;
    }
    
    handle_user_input();
    printf("Exiting main thread...\n");
    return 0;
}
