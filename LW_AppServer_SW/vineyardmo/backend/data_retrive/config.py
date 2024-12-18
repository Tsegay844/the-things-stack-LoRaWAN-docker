import logging

# Configure logging for better debugging
logging.basicConfig(level=logging.DEBUG,  # Set to DEBUG for more detailed logs OR set to INFO for short logs
                    format='%(asctime)s - %(levelname)s: %(message)s') #logging format time, logging level name and then the log message.
                    

# MQTT connection
BROKER_ADDRESS = "eu1.cloud.thethings.network"
BROKER_PORT = 1883
APPLICATION_ID = "nucleo-wl55jc1-sht30@ttn"
API_KEY = "NNSXS.JOQUUABK2KEOMVCSQEUSL4E45JTRKJ7SMNQBWOQ.OOK2NH4SDHEFHHGBDE7WCK5ETBDN5PYOBFVGAQV6ZUQ652SAGF6Q"  
TOPIC = f"v3/{APPLICATION_ID}/devices/+/up"

# MongoDB connection 
MONGO_URI = "mongodb://db:27017/" 
DATABASE_NAME = "telemetry_data"
COLLECTION_SENSOR = "sensor_data"
COLLECTION_MQTT = "mqtt_data"

