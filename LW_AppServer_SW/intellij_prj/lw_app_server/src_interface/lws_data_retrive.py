import paho.mqtt.client as mqtt
import json
import time
import sys
import logging

# Configure logging for better debugging
logging.basicConfig(level=logging.DEBUG,  # Set to DEBUG for more detailed logs OR set to INFO for short logs
                    format='%(asctime)s - %(levelname)s: %(message)s') #logging format time, logging level name and then the log message.

# MQTT connection
BROKER_ADDRESS = "eu1.cloud.thethings.network"
BROKER_PORT = 1883
APPLICATION_ID = "nucleo-wl55jc1-sht30@ttn"
API_KEY = "NNSXS.JOQUUABK2KEOMVCSQEUSL4E45JTRKJ7SMNQBWOQ.OOK2NH4SDHEFHHGBDE7WCK5ETBDN5PYOBFVGAQV6ZUQ652SAGF6Q"  # The API key for authentication
TOPIC = f"v3/{APPLICATION_ID}/devices/+/up"

# Callback method to connect client to the MQTT broker
def on_connect(client, userdata, flags, rc):
    logging.info(f"Connected to broker with result code {rc}")
    if rc == 0: # rc means return code, and the value zero indicates a successful connection to the MQTT Broker, if other numbers(1-5) different level of connection failures.
        logging.info(f"Successfully connected. Subscribing to topic: {TOPIC}")
        client.subscribe(TOPIC)
    else:
        logging.error(f"Connection failed with error code {rc}")
        sys.exit(1)

# Callback method to check subscription to TOPIC
def on_subscribe(client, userdata, mid, granted_qos):
    logging.info(f"Subscribed to topic {TOPIC}")

# Callback method to dibug disconnection
def on_disconnect(client, userdata, rc):
    logging.info(f"Disconnected from MQTT broker with result code: {rc}")


# Callback method to handle a message received from TOPIC
def on_message(client, userdata, msg):
    try:
        # Decode the payload of the message
        payload = json.loads(msg.payload.decode('utf-8'))
        logging.info("\n---- Payload Received -------------")
        #logging.info(json.dumps(payload, indent=2))
        # Extract telemetry data from the message
        decoded_payload = payload.get("uplink_message", {}).get("decoded_payload", {})
        #put the telemetry data in to docuemnt object
        if decoded_payload:
            document = {
                "appDevId": decoded_payload.get("appDevId"),
                "dcInterval": decoded_payload.get("dcInt"),
                "pktCounter": decoded_payload.get("pktCtr"),
                "sht30Temp": decoded_payload.get("shtT"),
                "sht30Hum": decoded_payload.get("shtRH"),
                "bmp280Temp": decoded_payload.get("bmpT"),
                "bmp280Pres": decoded_payload.get("bmpP"),
                "batteryLevel": decoded_payload.get("batLv"),
            }
            #log the telemetry data
            logging.info(json.dumps(document, indent=2))
        else:
            logging.error("message can not be decoded or is empty .")
    except Exception as e:
        logging.error(f"Error processing message: {e}")


#Main function to initialize and connect the MQTT client
def main():
    try:
        # Create an MQTT client
        client = mqtt.Client(client_id=f"python-ttn-debug-{int(time.time())}")
        # Set the username and password for the MQTT client
        client.username_pw_set(APPLICATION_ID, API_KEY)
        # Set the callback functions for different MQTT events
        client.on_connect = on_connect
        client.on_message = on_message
        client.on_subscribe = on_subscribe
        client.on_disconnect = on_disconnect
        logging.info(f"Attempting to connect to {BROKER_ADDRESS}:{BROKER_PORT}")
        client.connect(BROKER_ADDRESS, BROKER_PORT,)
        # Start the MQTT network loop
        client.loop_forever()

    except Exception as e:
        logging.error(f"MQTT Connection Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()  # Run the main function to start the MQTT client
#Bruno's code
"""from requests.auth import HTTPBasicAuth
import requests


def retrieve_stored_uplinks(key_id, api_key, limit=10, after="2024-07-02T00:00:00Z"):
    url = f"https://eu1.cloud.thethings.network/api/v3/as/applications/nucleo-wl55jc1-sht30/packages/storage/uplink_message"
    headers = {
        "Accept": "text/event-stream"
    }
    params = {
        "limit": limit,
        "after": after
    }
    auth = HTTPBasicAuth(key_id, api_key)
    response = requests.get(url, headers=headers, auth=auth, params=params)

    if response.status_code == 200:
        return response.text
    else:
        response.raise_for_status()


api_key = "NNSXS.XTHLU7MNY7KOYUBN7AKPIOLXLVJSVZRAGISGZHQ.HFAO56MQ34N4AEXPWH5OG7UAFVP3Q5F65K7NORKLCDOHEL4P3IDA"
key_id = "XTHLU7MNY7KOYUBN7AKPIOLXLVJSVZRAGISGZHQ"
uplinks = retrieve_stored_uplinks(key_id, api_key)
print(uplinks)"""
