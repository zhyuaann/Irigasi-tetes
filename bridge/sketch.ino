#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Pin Setup
#define SOIL_PIN_1 35
#define SOIL_PIN_2 34
#define RELAY_PIN 25

// Wifi Setup
const char* ssid = "";
const char* password = "";

// MQTT Setup
const char* mqtt_server = "";
const int mqttPort = ;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqtt_topic = "";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Waktu Pengukuran
const unsigned long MEASUREMENT_INTERVAL = 30UL * 60UL * 1000UL;
const unsigned long MEASUREMENT_DURATION = 2UL * 60UL * 1000UL;
const unsigned long DATA_SEND_INTERVAL = 5000;

unsigned long lastMeasurementTime = 0;
unsigned long measurementStartTime = 0;
unsigned long lastDataSendTime = 0;
bool isMeasuring = false;

// Kontrol Pompa Pakai Relay
void controlPump(bool enable) {
  digitalWrite(RELAY_PIN, enable ? LOW : HIGH); // Aktif LOW
  Serial.println(enable ? "Pompa ON (relay aktif)" : "Pompa OFF (relay mati)");
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(SOIL_PIN_1, INPUT);
  pinMode(SOIL_PIN_2, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  controlPump(false);

  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqttPort);
}

void setup_wifi() {
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terkoneksi");
  Serial.println(WiFi.localIP());
}

// Koneksi MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Terhubung ke MQTT broker");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void loop() {
  unsigned long currentTime = millis();

  if (!isMeasuring && (currentTime - lastMeasurementTime >= MEASUREMENT_INTERVAL)) {
    isMeasuring = true;
    measurementStartTime = currentTime;
    lastMeasurementTime = currentTime;
    Serial.println("=== MULAI PENGUKURAN ===");
  }

  if (isMeasuring) {
    if (currentTime - measurementStartTime >= MEASUREMENT_DURATION) {
      isMeasuring = false;
      controlPump(false);
      Serial.println("=== PENGUKURAN SELESAI ===");
      return;
    }
    performMeasurementAndSend();
  } else {
    static unsigned long lastStatusTime = 0;
    if (currentTime - lastStatusTime >= 60000) {
      unsigned long timeToNext = MEASUREMENT_INTERVAL - (currentTime - lastMeasurementTime);
      Serial.print("Menunggu sesi berikutnya dalam ");
      Serial.print(timeToNext / 60000);
      Serial.println(" menit");
      lastStatusTime = currentTime;
    }
  }

  delay(1000);
}

// Baca dan kirim data
void performMeasurementAndSend() {
  unsigned long currentTime = millis();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (currentTime - lastDataSendTime >= DATA_SEND_INTERVAL) {
    lastDataSendTime = currentTime;

    int soil1 = analogRead(SOIL_PIN_1);
    int soil2 = analogRead(SOIL_PIN_2);

    Serial.print("Sensor 1: "); Serial.println(soil1);
    Serial.print("Sensor 2: "); Serial.println(soil2);

    const int threshold = 1100;
    bool pumpStatus = false;

    if (soil1 < threshold || soil2 < threshold) {
      controlPump(true);
      pumpStatus = true;
    } else {
      controlPump(false);
    }

    unsigned long remaining = MEASUREMENT_DURATION - (currentTime - measurementStartTime);

    String payload = "{\"soil1\": " + String(soil1) +
                     ", \"soil2\": " + String(soil2) +
                     ", \"pump_status\": \"" + (pumpStatus ? "ON" : "OFF") + "\"" +
                     ", \"remaining_time\": " + String(remaining / 1000) +
                     ", \"timestamp\": " + String(currentTime) + "}";

    client.publish(mqtt_topic, payload.c_str());

    Serial.print("Data MQTT dikirim. Sisa waktu: ");
    Serial.print(remaining / 1000);
    Serial.println(" detik");
  }
}
