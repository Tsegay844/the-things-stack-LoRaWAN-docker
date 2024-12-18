
import logging
from pymongo import MongoClient
import config


def get_mongodb_connection():
    """
    Establishing mongodb connection
    """
    try:
        client = MongoClient(config.MONGO_URI)
        db = client[config.DATABASE_NAME]
        collection_mqtt = db[config.COLLECTION_MQTT]
        collection_sensor = db[config.COLLECTION_SENSOR]
        return collection_mqtt, collection_sensor
    except Exception as e:
        logging.error(f"connecting mongodb failed!: {e}")
        return None, None

def save_to_mongodb(data, collection):
    """
    Saving data into mongodb
    
    :param data: Document to be inserted
    :param collection: Target collection in the collection
    """
    try:
        collection.insert_one(data)
        logging.info("Document inserted into MongoDB")
    except Exception as e:
        logging.error(f"Documetn is not inserted into MongoDB: {e}")


