import json
import urllib3
import paho.mqtt.client as mqtt
import time

# Telegram Bot Token zur Authentifizierung mit der Telegram API
BOT_TOKEN = "7737824358:AAFBF7DU55csaZ-x-eLx2VHqrTwExDRmpXo"

# MQTT-Broker Informationen (IP-Adresse und Port)
mqtt_broker = "54.205.109.222"
mqtt_port = 1883

# MQTT-Themen zum Veröffentlichen von Befehlen und Empfangen von Antworten
mqtt_publish_topic = "test/topic"  # Hierhin werden Befehle gesendet
mqtt_reply_topic = "temp/topic"    # Hier werden Antworten vom Gerät erwartet

def sendReply(chat_id, message):
    """
    Sendet eine Nachricht an einen Telegram-Chat über die Telegram-Bot-API.
    
    Parameter:
    chat_id (int): Die ID des Chats, an den die Nachricht gesendet wird.
    message (str): Der zu sendende Nachrichtentext.
    """
    reply = {
        "chat_id": chat_id,
        "text": message
    }
    
    # Erstelle eine Verbindung zur Telegram-API
    http = urllib3.PoolManager()
    url = f"https://api.telegram.org/bot{BOT_TOKEN}/sendMessage"
    encoded_data = json.dumps(reply).encode('utf-8')
    
    # Sende eine HTTP-POST-Anfrage an die Telegram-API
    response = http.request(
        'POST', url, body=encoded_data, headers={'Content-Type': 'application/json'}
    )
    
    # Ausgabe der gesendeten Nachricht und des API-Response-Status
    print(f"*** Reply Sent: {encoded_data}, Response Status: {response.status}")

def on_message(client, userdata, msg):
    """
    Callback-Funktion, die aufgerufen wird, wenn eine MQTT-Nachricht empfangen wird.
    
    Parameter:
    client (mqtt.Client): Das MQTT-Client-Objekt.
    userdata (dict): Benutzerdaten, die zum Speichern der empfangenen Nachricht genutzt werden.
    msg (mqtt.MQTTMessage): Die empfangene MQTT-Nachricht.
    """
    userdata['message'] = msg.payload.decode()
    print(f"*** MQTT message received on {msg.topic}: {userdata['message']}")

def on_connect(client, userdata, flags, rc):
    """
    Callback-Funktion, die beim erfolgreichen Verbinden mit dem MQTT-Broker aufgerufen wird.
    
    Parameter:
    client (mqtt.Client): Das MQTT-Client-Objekt.
    userdata (dict): Benutzerdaten.
    flags (dict): Statusflags der Verbindung.
    rc (int): Rückgabecode des Verbindungsversuchs.
    """
    print(f"*** Connected to MQTT broker with result code {rc}")
    client.subscribe(mqtt_reply_topic)  # Abonniere das Antwort-Thema, um Nachrichten zu empfangen

def lambda_handler(event, context):
    """
    AWS Lambda-Handler-Funktion für eingehende Telegram-Bot-Nachrichten.
    
    Parameter:
    event (dict): Das eingehende Ereignis mit Telegram-Daten.
    context (object): AWS Lambda-Kontextobjekt.
    
    Rückgabewert:
    dict: Eine HTTP-Response mit Statuscode und Nachricht.
    """
    try:
        # Extrahiere den Body des eingehenden Webhook-Events
        body = json.loads(event['body'])
        
        # Falls keine Nachricht vorhanden ist, wird ein Fehler ausgegeben
        if 'message' not in body:
            print("*** No 'message' field in the body")
            return {
                'statusCode': 200,
                'body': json.dumps('No message to process')
            }
        
        # Extrahiere Chat-ID, Benutzername und Nachrichtentext aus dem Event
        chat_id = body['message']['chat']['id'] 
        user_name = body['message']['from'].get('username', 'unknown')
        message_text = body['message'].get('text', '')
        
        print(f"*** chat id: {chat_id}")
        print(f"*** user name: {user_name}")
        print(f"*** message text: {message_text}")
        
        # Verarbeitung verschiedener Befehle
        if message_text == "/start":
            sendReply(chat_id, "This is a Test of Group 1")
        elif message_text == "/temp":
            print("*** Processing '/temp' command")
            
            # Initialisiere MQTT-Client mit einer Zustandsvariable zur Speicherung der Antwort
            state = {'message': None}
            client = mqtt.Client(userdata=state)
            client.on_connect = on_connect
            client.on_message = on_message
            
            try:
                print(f"*** Connecting to MQTT broker {mqtt_broker}:{mqtt_port}")
                client.connect(mqtt_broker, mqtt_port, 60)
                client.loop_start()
                
                # Sende das '/temp'-Kommando an das IoT-Gerät
                temp_message = "/temp"
                print(f"*** Publishing '/temp' message to topic {mqtt_publish_topic}")
                client.publish(mqtt_publish_topic, temp_message)
                
                # Warte auf die Antwort mit einem Timeout
                timeout = 15
                start_time = time.time()
                while state['message'] is None and time.time() - start_time < timeout:
                    time.sleep(0.1)
                
                # Falls eine Antwort empfangen wurde, sende diese an den Benutzer
                if state['message']:
                    print(f"*** MQTT message received: {state['message']}")
                    sendReply(chat_id, state['message'])
                else:
                    print("*** No MQTT message received within timeout")
                    sendReply(chat_id, "No response from IoT device in time.")
            except Exception as mqtt_error:
                print(f"*** MQTT error: {mqtt_error}")
                sendReply(chat_id, "Failed to connect to IoT device.")
            finally:
                client.loop_stop()
                client.disconnect()
        else:
            sendReply(chat_id, "Unknown Command")
        
        return {
            'statusCode': 200,
            'body': json.dumps('Message processed successfully')
        }
    except Exception as e:
        print(f"*** Error: {e}")
        return {
            'statusCode': 500,
            'body': json.dumps(f"Internal server error: {str(e)}")
        }
