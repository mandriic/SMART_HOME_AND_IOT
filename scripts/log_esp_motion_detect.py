import paho.mqtt.client as mqtt
from datetime import datetime

def on_message(client, userdata, message):
    time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    payload = message.payload.decode("utf-8")
    log_entry = f"[{time_str}] Topic: {message.topic} | Status: {payload}\n"
    
    with open("motion_log.txt", "a") as f:
        f.write(log_entry)
    print(log_entry.strip())

client = mqtt.Client()
client.on_message = on_message
client.connect("localhost", 1883)
client.subscribe("home/livingroom/motion")
client.loop_forever()