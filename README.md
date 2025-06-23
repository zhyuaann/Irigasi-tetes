Pada project sistem irigasi tetes ini, saya mengirimkan data dari ESP32 ke Firebase.
Flow datanya adalah sebagai berikut: ESP32 -> MQTT -> Firebase

Data dari sensor kelembapan tanah diterima oleh ESP32, kemudian dikirimkan ke MQTT dalam format JSON. Melalui MQTT broker, data kemudian dipush ke Firebase menggunakan Node.js
