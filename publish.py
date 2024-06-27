import subprocess
import boto3
import ssl
import json
import time as t
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient 

program = "./example_scan"
process = subprocess.Popen(program)
code = process.wait()


# subprocess.run(["./example_scan"], shell = "True")

# subprocess.call("./example_scan")

# AWS IoT Core endpoint
iot_endpoint = 'a1pt5dj68ngc41.iot.ap-southeast-1.amazonaws.com'

# AWS IoT credentials
cert_path = 'adappt-iotData.cert.pem'
key_path = 'adappt-iotData.private.key'
ca_path = 'root-CA.crt'
RANGE = 1

# MQTT topic to publish to
topic = 'HoneywellSensorData'

# Message payload

message = {
    'pandu': '90',
    'sumanth': '60',
    'arvind': '30'
}

# Init AWSIoTMQTTClient
client = AWSIoTMQTTClient("publisher")
client.configureEndpoint(iot_endpoint, 8883)
client.configureCredentials(ca_path, key_path, cert_path)

# Configure SSL/TLS
client.configureAutoReconnectBackoffTime(1, 32, 20)
client.configureOfflinePublishQueueing(-1)  # Infinite offline publish queueing
client.configureDrainingFrequency(2)  # Draining: 2 Hz
client.configureConnectDisconnectTimeout(10)  # 10 sec
client.configureMQTTOperationTimeout(5)  # 5 sec

# Connect to AWS IoT
client.connect()

# Publish message to the topic
with open('macids.txt', 'r') as f:
    for line in f:
        print(line)
        client.publish(topic, json.dumps(line.strip()), 1)
        print("I pushed you")
    # line = f.readline()
    # while line:
    #     print(line, end='')
    #     line = f.readline()
    #     client.publish(topic, json.dumps(line.strip()), 1)
    #     t.sleep(0.5)

# for i in range (RANGE):
#     client.publish(topic, json.dumps(message), 1)
#     print(f"Message published to {topic}")
#     t.sleep(0.1)



# Disconnect from AWS IoT
client.disconnect()

