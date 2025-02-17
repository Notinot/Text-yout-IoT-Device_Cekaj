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

#define BUF_SIZE 1024
#define MQTT_VERSION_v311 4
#define COMMAND_TIMEOUT_MS 4000
#define DEFAULT_MQTT_BROKER_IP "2600:1f18:2a2e:423c:ef89:ece:f856:46db"
#define DEFAULT_CLIENT_ID "RIOTClient"
#define MQTT_PORT 1883
#define MQTT_TOPIC "test/topic"
#define RESPONSE_TOPIC "temp/topic"
#define KEEP_ALIVE_SEC 10
#define IS_CLEAN_SESSION 1
#define IS_RETAINED_MSG 0
#define RETRY_LIMIT 3

static MQTTClient client;
static Network network;
static unsigned char buf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];
static char mqtt_broker_ip[256] = DEFAULT_MQTT_BROKER_IP;
static char client_id[256] = DEFAULT_CLIENT_ID;


//#define MQTT_THREAD_STACKSIZE (THREAD_STACKSIZE_DEFAULT)
static char mqtt_thread_stack[MQTT_THREAD_STACKSIZE];
static kernel_pid_t mqtt_pid;

static int read_temperature(float *temperature) {
    saul_reg_t *sensor = saul_reg_find_name("NRF_TEMP");
    if (sensor == NULL) {
        printf("Sensor not found\n");
        return -1;
    }
    phydat_t temp_value;
    if (sensor->driver->read(sensor, &temp_value) < 0) {
        printf("Failed to read temperature\n");
        return -1;
    }
    *temperature = temp_value.val[0] * pow(10,temp_value.scale) ;
    
    return 0;
}

static void _on_msg_received(MessageData *data) {
    printf("Message received on topic %.*s: %.*s\n",
           (int)data->topicName->lenstring.len,
           data->topicName->lenstring.data, (int)data->message->payloadlen,
           (char *)data->message->payload);

    if (strncmp((char *)data->message->payload, "/temp", data->message->payloadlen) == 0) {
        float temperature = 0.0;
        if (read_temperature(&temperature) < 0) {
            return;
        }
        char temp_response[64];
        snprintf(temp_response, sizeof(temp_response), "Temperature: %d °C", (int)temperature);
        MQTTMessage message;
        message.qos = QOS0;
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

static int mqtt_reconnect(void) {
    printf("Attempting to fully reset and reconnect...\n");
    
    MQTTDisconnect(&client);
    NetworkDisconnect(&network);
    
    // Clear buffers
    memset(buf, 0, BUF_SIZE);
    memset(readbuf, 0, BUF_SIZE);

    // Wait a bit before reinitializing
    ztimer_sleep(ZTIMER_SEC, 2);

    // Reinitialize network and client
    NetworkInit(&network);
    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);
    
    return mqtt_connect();
}

static int mqtt_subscribe(void) {
    int ret = MQTTSubscribe(&client, MQTT_TOPIC, QOS0, _on_msg_received);
    if (ret < 0) {
        printf("Failed to subscribe to topic %s\n", MQTT_TOPIC);
        return ret;
    }
    printf("Subscribed to topic: %s\n", MQTT_TOPIC);
    return 0;
}


static void handle_user_input(void) {
    char input[128];

    while (1) {
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;  

            if (strncmp(input, "setIP ", 6) == 0) {
                char new_ip[256];
                strncpy(new_ip, input + 6, sizeof(new_ip));
                new_ip[sizeof(new_ip) - 1] = '\0';

                
                if (strcmp(mqtt_broker_ip, new_ip) != 0) {
                    printf("Changing MQTT Broker IP from %s to %s\n", mqtt_broker_ip, new_ip);
                    strncpy(mqtt_broker_ip, new_ip, sizeof(mqtt_broker_ip));

                    
                    NetworkDisconnect(&network);
                    MQTTDisconnect(&client);

                    NetworkInit(&network);
                    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);

                    if (mqtt_reconnect() == 0) {
                        mqtt_subscribe();
                    } else {
                        printf("Failed to reconnect to the new MQTT broker.\n");
                    }
                } else {
                    printf("Already connected to this IP. No reconnection needed.\n");
                }
            } else if (strncmp(input, "setClientID ", 12) == 0) {
                char new_client_id[256];
                strncpy(new_client_id, input + 12, sizeof(new_client_id));
                new_client_id[sizeof(new_client_id) - 1] = '\0';

                
                if (strcmp(client_id, new_client_id) != 0) {
                    printf("Changing MQTT Client ID from %s to %s\n", client_id, new_client_id);
                    strncpy(client_id, new_client_id, sizeof(client_id));

                    
                    NetworkDisconnect(&network);
                    MQTTDisconnect(&client);

                    NetworkInit(&network);
                    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);

                    if (mqtt_connect() == 0) {
                        mqtt_subscribe();
                    } else {
                        printf("Failed to reconnect with the new Client ID.\n");
                    }
                } else {
                    printf("Client ID is already set. No reconnection needed.\n");
                }
            } else if (strcmp(input, "exit") == 0) {
                printf("Exiting...\n");
                break;
            } else {
                printf("Unknown command. Try 'setIP', 'setClientID', or 'exit'.\n");
            }
        }
        ztimer_sleep(ZTIMER_MSEC, 100);
    }
}

/*static void *mqtt_thread(void *arg) {
    (void)arg;

    while (1) {
        MQTTYield(&client, 100);
        ztimer_sleep(ZTIMER_MSEC, 100);
    }

    return NULL;
}*/

static void *mqtt_thread(void *arg) {
    (void)arg;
    int retry_delay = 1;

    while (1) {
        if (!MQTTIsConnected(&client)) {
            printf("MQTT connection lost. Restarting connection...\n");

            while (mqtt_reconnect() != 0) {
                printf("Reconnect failed. Retrying in %d seconds...\n", retry_delay);
                ztimer_sleep(ZTIMER_SEC, retry_delay);
                retry_delay = (retry_delay < 10) ? retry_delay * 2 : 10;
            }

            mqtt_subscribe();
            printf("Reconnected successfully!\n");
            retry_delay = 1;  // Reset retry delay
        }

        MQTTYield(&client, 50);
        ztimer_sleep(ZTIMER_MSEC, 15);
    }

    return NULL;
}

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
