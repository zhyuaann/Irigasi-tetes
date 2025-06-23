import mqtt from 'mqtt';
import { firebaseConfig } from './config';
import { client } from './config';
import { initializeApp } from 'firebase/app';
import { getDatabase, ref, set, push } from 'firebase/database';

// Init Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

client.on('connect', () => {
        console.log('Connected to MQTT');
        client.subscribe('sensor/soil', (err) => {
                if (err) {
                        console.error('❌ Gagal subscribe ke topic:', err);
                } else {
                        console.log('✅ Subscribed to topic sensor/suhu');
                }
        });

});

client.on('error', (error) => {
  console.error('❌ MQTT Error:', error);
});

client.on('message', (topic, message) => {
        const payload = message.toString();
        const timestamp = Date.now();

        console.log('Payload MQTT:', payload);

        console.log(`[${topic}] ${payload}`);

        const dataRef = push(ref(db, 'sensor/soil'));
        set(dataRef, {
                topic,
                value: payload,
                timestamp
        })
                .then(() => {
                        console.log('✅ Data pushed to Firebase');
                })
                .catch((error) => {
                        console.error('❌ Gagal push ke Firebase:', error);
                });
});

