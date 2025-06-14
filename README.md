# 🌡️ Smart Fish Tank Sensor System

Sistem ini merupakan proyek **IoT** berbasis **ESP32** yang berfungsi untuk membaca data sensor suhu air, TDS (Total Dissolved Solids), dan pH, kemudian mengirimkan datanya ke **Firebase Realtime Database** secara periodik. Cocok digunakan untuk monitoring kualitas air di akuarium atau kolam ikan secara real-time.

---

## 🚀 Fitur Utama

- Membaca suhu air menggunakan sensor **DS18B20**
- Membaca nilai **TDS** menggunakan sensor analog (EC/TDS sensor)
- Membaca nilai **pH** menggunakan sensor analog
- Kalibrasi pH manual (dengan referensi tegangan pH 4 dan pH 7)
- Kirim data ke **Firebase Realtime Database**
- Tampilkan data ke **Serial Monitor**

---

## 🧰 Library yang Digunakan

| Library | Fungsi |
|--------|--------|
| `WiFi.h` | Koneksi ESP32 ke jaringan WiFi |
| `Firebase_ESP_Client.h` | Komunikasi ke Firebase Realtime Database |
| `OneWire.h` | Komunikasi dengan sensor suhu DS18B20 |
| `DallasTemperature.h` | Pembacaan suhu dari sensor DS18B20 |

---

## 📦 Struktur Firebase

Data dikirim ke path:  
```
/sensors/
    ├── temperature : float (°C)
    ├── tds         : float (ppm)
    └── ph          : float
```

Contoh:
```json
"sensors": {
  "temperature": 27.62,
  "tds": 391.24,
  "ph": 6.95
}
```

---

## ⚙️ Konfigurasi

### WiFi dan Firebase

Ubah bagian berikut sesuai dengan jaringan dan akun Firebase kamu:
```cpp
#define WIFI_SSID     "HAYOLO"
#define WIFI_PASSWORD "12345678"

#define API_KEY       "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL  "https://your-project.firebaseio.com/"
#define USER_EMAIL    "your-user@gmail.com"
#define USER_PASSWORD "your-password"
```

---

## 🧪 Kalibrasi pH

```cpp
#define PH4 3.1  // Tegangan ADC saat larutan pH 4
#define PH7 2.6  // Tegangan ADC saat larutan pH 7
```

Pastikan kamu melakukan kalibrasi berdasarkan tegangan aktual yang terbaca saat mencelupkan sensor ke larutan buffer pH 4 dan pH 7.

---

## 📷 Tampilan Serial Monitor (Contoh Output)

```
Temperature: 28.50 ºC
TDS Value: 445 ppm
ADC pH: 2330
Tegangan pH: 2.65
Nilai pH: 6.95
------------------------
```

---

## 🕒 Interval Pengiriman

Data dikirim ke Firebase setiap **5 detik**:
```cpp
delay(5000);
```
Kamu bisa mengatur ulang sesuai kebutuhan.

---

## 📌 Schematic (Umum)

| Komponen       | Pin ESP32     |
|----------------|---------------|
| TDS Sensor     | GPIO 33 (Analog) |
| pH Sensor      | GPIO 32 (Analog) |
| DS18B20        | GPIO 25 (Digital) + Pull-up 4.7kΩ |
