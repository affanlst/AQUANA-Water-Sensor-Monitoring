#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define WIFI_SSID "HAYOLO"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyCmmHx7BdW0dSfvxsoL4WIvVIWOj7F-jIU"
#define DATABASE_URL "https://smart-fish-tank-10016-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define USER_EMAIL "espsensorsmartfishtank@gmail.com"
#define USER_PASSWORD "12345678"


// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


// -------------------- Konfigurasi Pin dan Konstanta --------------------
#define TdsSensorPin 33
#define PH_Pin 32
#define VREF 3.3
#define SCOUNT 50

// -------------------- Variabel TDS --------------------
int analogBuffer[SCOUNT];     
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
float averageVoltage = 0;
float tdsValue = 0;
float temperature = 5;

// -------------------- DS18B20 Setup --------------------
const int oneWireBus = 25;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// -------------------- Variabel PH --------------------
float PH4 = 3.1;
float PH7 = 2.6;
float PH_step;
float Po;
double TeganganPh;

// -------------------- Median Filter --------------------
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++) bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i]; bTab[i] = bTab[i + 1]; bTab[i + 1] = bTemp;
      }
    }
  }
  return (iFilterLen & 1) ? bTab[(iFilterLen - 1) / 2] : 
         (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
}


// -------------------- Setup --------------------
void setup() {
 Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(TdsSensorPin, INPUT);
  pinMode(PH_Pin, INPUT);
  sensors.begin();
}

// -------------------- Loop --------------------
void loop() {
  // -------------------- Suhu --------------------
  sensors.requestTemperatures(); 
  temperature = sensors.getTempCByIndex(0);

  // -------------------- TDS --------------------
  for (int i = 0; i < SCOUNT; i++) {
    analogBuffer[i] = analogRead(TdsSensorPin);
    delay(5); // sampling pelan biar stabil
  }
  averageVoltage = getMedianNum(analogBuffer, SCOUNT) * (float)VREF / 4095.0;
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVoltage = averageVoltage / compensationCoefficient;
  tdsValue = (133.42 * pow(compensationVoltage, 3) -
              255.86 * pow(compensationVoltage, 2) +
              857.39 * compensationVoltage) * 0.5;

  

  Serial.print("Compensation Voltage: ");
  Serial.println(compensationVoltage, 3);

  // -------------------- jeda dulu biar ADC stabil --------------------
  delay(300);

  // -------------------- pH --------------------
  // Flushing nilai ADC sebelum pengambilan data
  for (int i = 0; i < 10; i++) {
    analogRead(PH_Pin);
    delay(5);
  }

  int totalPhAnalog = 0;
  for (int i = 0; i < 10; i++) {
    totalPhAnalog += analogRead(PH_Pin);
    delay(5);
  }
  int nilai_analog_PH = totalPhAnalog / 10;

  TeganganPh = 3.3 / 4095.0 * nilai_analog_PH;
  PH_step = (PH4 - PH7) / 3;
  Po = 7.00 + ((PH7 - TeganganPh) / PH_step);

  // -------------------- Output ke Serial --------------------
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" ºC");

  Serial.print("TDS Value: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");

  Serial.print("ADC pH: ");
  Serial.println(nilai_analog_PH);

  Serial.print("Tegangan pH: ");
  Serial.println(TeganganPh, 3);

  Serial.print("Nilai pH: ");
  Serial.println(Po, 2);

  Serial.println("------------------------");
  if (Firebase.ready()) {
      String path = "/sensors"; // bisa juga disertai timestamp jika mau
      Firebase.RTDB.setFloat(&fbdo, path + "/temperature", temperature);
      Firebase.RTDB.setFloat(&fbdo, path + "/tds", tdsValue);
      Firebase.RTDB.setFloat(&fbdo, path + "/ph", Po);
    }

    delay(5000);// tunggu sebelum loop berikutnya
}
