#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Synapsis.id Head Office"
#define WIFI_PASSWORD "synaps1sjkt23@"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBTPoD5A-8KDFBV0ycFKhJGHXyQxcUiMOU"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "parking@gmail.com"
#define USER_PASSWORD "haikalMurjaLGay"

// Insert Firebase Realtime Database URL
#define DATABASE_URL "https://parking-ezz-default-rtdb.asia-southeast1.firebasedatabase.app/"  // Replace with your database URL

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Alamat I2C slave
const int slaveAddress = 0x08;

// Variabel untuk menyimpan data yang diterima
int receivedDistance = 0;

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  initWiFi();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Inisialisasi komunikasi I2C sebagai slave dengan alamat tertentu
  Wire.begin(slaveAddress);

  // Menetapkan fungsi callback untuk menerima data
  Wire.onReceive(receiveData);
}

void loop() {
  // Cek apakah token Firebase sudah kedaluwarsa
  if (Firebase.isTokenExpired()) {
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }
}

void receiveData(int byteCount) {
  byte data[2];
  int index = 0;

  // Membaca data dari I2C buffer
  while (Wire.available() && index < 2) {
    data[index] = Wire.read();
    index++;
  }

  if (index == 2) {
    // Mengkonversi data dari byte array menjadi integer
    receivedDistance = (data[0] << 8) | data[1];

    // Tampilkan data yang diterima di Serial Monitor
    Serial.print("Distance received: ");
    Serial.println(receivedDistance);

    // Kirim data ke Firebase
    if (Firebase.RTDB.setInt(&fbdo, "users/" + uid + "/distance", receivedDistance)) {
      Serial.println("Data sent to Firebase successfully");
    } else {
      Serial.print("Failed to send data to Firebase: ");
      Serial.println(fbdo.errorReason());
    }
  } else {
    Serial.println("Failed to receive complete data.");
  }
}
