#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HX711_ADC.h>

// WiFi Credentials
const char* ssid = "nice";
const char* password = "12345678";

// Endpoint Server Flask
const char* serverUrl = "http://192.168.155.106:5000/api/regist_vehicle"; // ganti sesuai IP Flask-mu

// HX711
const int HX711_dout = 5;
const int HX711_sck = 4;
HX711_ADC LoadCell(HX711_dout, HX711_sck);

// RFID RC522
#define RST_PIN 22
#define SS_PIN 21
MFRC522 rfid(SS_PIN, RST_PIN);

String lastUID = "";

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTersambung ke WiFi");

  SPI.begin(18, 19, 23, SS_PIN); // SCK, MISO, MOSI, SS
  rfid.PCD_Init();

  LoadCell.begin();
  LoadCell.start(2000, true);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("HX711 tidak terbaca!");
  } else {
    LoadCell.setCalFactor(-119.79); // sesuaikan hasil kalibrasi kamu
    Serial.println("LoadCell siap");
  }
}

void loop() {
  LoadCell.update();

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uidStr = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidStr += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      uidStr += String(rfid.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase();

    if (uidStr != lastUID) {
      lastUID = uidStr;

      float berat = LoadCell.getData() + 100;
      Serial.println("UID: " + uidStr);
      Serial.println("Berat: " + String(berat, 2));

      if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{\"rfid_tag\":\"" + uidStr + "\", \"weight\":" + String(berat, 2) + "}";
        int code = http.POST(jsonPayload);

        Serial.print("HTTP Code: ");
        Serial.println(code);
        if (code > 0) {
          String response = http.getString();
          Serial.println("Response: " + response);
        } else {
          Serial.println("Gagal kirim data");
        }
        http.end();
      }

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }

  delay(1000);
}