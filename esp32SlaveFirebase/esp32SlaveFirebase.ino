#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Synapsis.id Head Office"
#define WIFI_PASSWORD "synaps1sjkt23@"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// Insert Firebase project API Key
#define API_KEY "AIzaSyBTPoD5A-8KDFBV0ycFKhJGHXyQxcUiMOU"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "parking@gmail.com"
#define USER_PASSWORD "haikalMurjaLGay"

// Insert RTDB URL
#define DATABASE_URL "https://parking-ezz-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String distancePath = "/distance";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 5000;

// Variables for I2C
const int slaveAddress = 0x08;
int receivedDistance = 0;

// Timer for I2C
unsigned long receiveDataPrevMillis = 0;
unsigned long receiveDataInterval = 5000; // Interval untuk menerima data dari I2C setiap 5 detik

// Initialize WiFi
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

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  configTime(0, 0, ntpServer);

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

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

  // Initialize I2C as slave
  Wire.begin(slaveAddress);
  Wire.onReceive(receiveData);
}

void loop() {
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Get current timestamp
    timestamp = getTime();
    Serial.print("time: ");
    Serial.println(timestamp);

    parentPath = databasePath;
    json.set(distancePath, String(receivedDistance)); // Include the received distance data
    json.set(timePath, String(timestamp));
    printLocalTime();
    Serial.println(timeInfo);
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }

  // Receive data from I2C
  if (millis() - receiveDataPrevMillis > receiveDataInterval) {
    receiveDataPrevMillis = millis();
    // Data reception is handled in the receiveData function called by Wire.onReceive
  }
}

void receiveData(int byteCount) {
  while (Wire.available()) {
    receivedDistance = Wire.read();
  }

  // Tampilkan data yang diterima di Serial Monitor
  Serial.print("Distance received: ");
  Serial.println(receivedDistance);
}


// Function that gets current epoch time
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);


  // Menambahkan angka 7 ke timeHour
  int hourOffset = 7;
  int currentHour = atoi(timeHour);
  currentHour = (currentHour + hourOffset) % 24; // Menghindari nilai yang melebihi 24 jam
  sprintf(timeHour, "%02d", currentHour);


  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  char timeYear[6];
  strftime(timeYear, 10, "%Y", &timeinfo);
  Serial.println(timeYear);
  char timeMonth[12];
  strftime(timeMonth, 12, "%B", &timeinfo);
  Serial.println(timeMonth);
  char timeDate[3];
  strftime(timeDate, 3, "%d", &timeinfo);
  Serial.println(timeDate);
  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  Serial.println(timeMinute);
  Serial.println();
  String timeInfo = String(timeWeekDay) + ", " + String(timeDate) + " " + String(timeMonth) + " " + String(timeYear) + ", " + String(timeHour) + ":" + String(timeMinute);

  Serial.print("Time Info : ");
  Serial.println(timeInfo);
}
