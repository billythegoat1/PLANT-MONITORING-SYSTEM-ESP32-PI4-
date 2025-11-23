import paho.mqtt.client as mqtt
import sqlite3
import json
from datetime import datetime

#SETUP THE DATABASE

db = sqlite3.connect("plants.db")
cursor = db.cursor()

cursor.execute("""
               CREATE TABLE IF NOT EXISTS plant1 (
               id INTEGER PRIMARY KEY AUTOINCREMENT,
               air_temp REAL,
               air_humid REAL,
               light REAL,
               soil_moist REAL,
               timestamp TEXT
               
               )
               """)
db.commit()

client = mqtt.Client("mqtt_sqlite_script")


def on_connect(client, userdata, flags, rc):
    print(f"Client connected with rc: {rc}")
    client.subscribe("plant1/data")

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()

        print(f"Received: {payload}")

        data = json.load(payload)

        #Get json data to put in the database
        air_temp = data.get("ait_temp")
        air_humid = data.get("air_humid")
        light = data.get("light")
        soil_moist = data.get("soil_moist")
        timestamp = datetime.now().isoformat()

        cursor.execute("""
                       INSERT INTO plant1 (air_temp, air_humid, light, soil_moist, timestamp)
                       VALUES (?, ?, ?, ?, ?)
                       """, (air_temp, air_humid, light, soil_moist, timestamp))
        
        #Push to the database
        db.commit()

        print("Saved to DB")

    except Exception as e:
        print("Error processing message: ", e)

#MQTT setup
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883,60)
client.username_pw_set('billy','billyTHEmosquitto')

client.loop_forever()