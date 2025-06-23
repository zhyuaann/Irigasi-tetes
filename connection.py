import paho.mqtt.client as mqtt
import pymysql
import json

# Setup koneksi database MySQL
db = pymysql.connect(host='localhost', user='root', password='admin', database='tetes')
cursor = db.cursor()

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker with result code "+str(rc))
    client.subscribe("sensor/soil")

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        data = json.loads(payload)
        soil1 = data['soil1']
        soil2 = data['soil2']

        sql = "INSERT INTO soil_data (soil1, soil2) VALUES (%s, %s)"
        cursor.execute(sql, (soil1, soil2))
        db.commit()

        print(f"Data inserted: soil1={soil1}, soil2={soil2}")

    except Exception as e:
        print("Error processing message:", e)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

mqtt_broker = "YOUR_MQTT_BROKER"  # contoh: "broker.hivemq.com"
mqtt_port = 1883                  # atau 8883 untuk TLS, sesuaikan
mqtt_user = "MQTT_USERNAME"       # kosongkan jika broker tidak perlu
mqtt_pass = "MQTT_PASSWORD"

client.username_pw_set(mqtt_user, mqtt_pass)
client.connect(mqtt_broker, mqtt_port, 60)

client.loop_forever()
