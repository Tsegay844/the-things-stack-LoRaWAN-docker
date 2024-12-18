import time
import sys 
import logging
import paho.mqtt.client as mqtt  
import mqtt_client  
import config


#Main function initialize and connect the mqtt_client
def main():
    try:
        """
        #Create an MQTT client with a unique client ID of string "python-ttn-debug-"
        followed by the current timestamp. 
	"""
        client = mqtt.Client(client_id=f"python-ttn-debug-{int(time.time())}")
        # Set username and password of mqttclient
        client.username_pw_set(config.APPLICATION_ID, config.API_KEY)

        # callback functions for different mqtt events
        client.on_connect = mqtt_client.on_connect
        client.on_message = mqtt_client.on_message
        client.on_subscribe = mqtt_client.on_subscribe
        client.on_disconnect = mqtt_client.on_disconnect
        logging.info(f"Attempting to connect to {config.BROKER_ADDRESS}:{config.BROKER_PORT}")
        client.connect(config.BROKER_ADDRESS, config.BROKER_PORT)
        # loop
        client.loop_forever()

    except Exception as e:
        logging.error(f"MQTT Connection Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()  # Run main

