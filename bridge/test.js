import { initializeApp } from 'firebase/app';
import { getDatabase, ref, push, set } from 'firebase/database';

const firebaseConfig = {
        apiKey: "AIzaSyDpBDNP91IEwZpFqBPinS5MC8eFYjehow0",
        authDomain: "tetes-ddb26.firebaseapp.com",
        databaseURL: "https://tetes-ddb26-default-rtdb.asia-southeast1.firebasedatabase.app",
        projectId: "tetes-ddb26",
        storageBucket: "tetes-ddb26.appspot.com",
        messagingSenderId: "184194950887",
        appId: "1:184194950887:web:4194cedda26fb982656442"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

const dataRef = push(ref(db, 'test'));
set(dataRef, {
  message: 'Hello Firebase',
  timestamp: Date.now()
}).then(() => {
  console.log('Test data pushed successfully');
}).catch((e) => {
  console.error('Test push error:', e);
});
