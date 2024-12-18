
import config
import json
import sys
import logging
import mongo_client
# Callback method to connect client to the mqtt broker
def on_connect(client, userdata, flags, rc):
    logging.info(f"Connected to broker with result code {rc}")
    if rc == 0: # rc means return code, and the value zero indicates a successful connection to the MQTT Broker, if other numbers(1-5) different level of connection failures.
        logging.info(f"Successfully connected. Subscribing to topic: {config.TOPIC}")
        client.subscribe(config.TOPIC)

        """
        # testing the mongodb
        data = {
            "appDevId": "appDevId",
            "dcInterval": "dcInt",
            "pktCounter": "pktCtr",
            "sht30Temp": "shtT"
        }
        # log the telemetry data
        logging.info(json.dumps(data, indent=2))

        #save data
        collection_mqtt, collection_sensor = mongo_client.get_mongodb_connection()
        if collection_mqtt is not None and collection_sensor is not None:
            mongo_client.save_to_mongodb(data, collection_sensor)
            mongo_client.save_to_mongodb(data, collection_mqtt)
        else:
            logging.error("failed connecting to Mongodb collections")
	"""
    else:
        logging.error(f"Connection failed with error code {rc}")
        sys.exit(1)

# Callback method to check subscription to TOPIC
def on_subscribe(client, userdata, mid, granted_qos):
    logging.info(f"Subscribed to topic {config.TOPIC}")

# Callback method to dibug disconnection
def on_disconnect(client, userdata, rc):
    logging.info(f"Disconnected from MQTT broker with result code: {rc}")


# Callback method to handle a message received from TOPIC
def on_message(client, userdata, msg):
    try:
        # Decode the message
        payload = json.loads(msg.payload.decode('utf-8'))
        logging.info("\n---- Payload Received -------------")
        #logging.info(json.dumps(payload, indent=2))
        
        # Extract telemetry data from the message
        if "uplink_message" in payload and "decoded_payload" in payload["uplink_message"]:
            decoded_payload = payload.get("uplink_message", {}).get("decoded_payload", {})
        #insert the telemetry data in to docuemnt object
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
            
            # Save the data to MongoDB
            collection_mqtt, collection_sensor = mongo_client.get_mongodb_connection()
            if collection_mqtt is not None and collection_sensor is not None:
                mongo_client.save_to_mongodb(document, collection_sensor)
                mongo_client.save_to_mongodb(payload, collection_mqtt)
            else:
                logging.error("Failed connecting to MongoDB collections")
        else:
            logging.error("message can not be decoded or is empty .")
    except Exception as e:
        logging.error(f"Error processing message: {e}")
        









