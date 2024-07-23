#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <Preferences.h>
#include "FS.h"
#include <SPIFFS.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define API_KEY "AIzaSyBTPoD5A-8KDFBV0ycFKhJGHXyQxcUiMOU"
#define USER_EMAIL "parking@gmail.com"
#define USER_PASSWORD "haikalMurjaLGay"
#define DATABASE_URL "https://parking-ezz-default-rtdb.asia-southeast1.firebasedatabase.app/"

Preferences preferences;
DNSServer dnsServer;
AsyncWebServer server(80);
IPAddress apIP(8, 8, 4, 4);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid, databasePath, parentPath, ssid, password, device;
bool is_setup_done = false; bool valid_ssid_received = false; bool valid_password_received = false; bool valid_device_received = false; bool wifi_timeout = false;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600; const int slaveAddress = 0x08;
unsigned long sendDataPrevMillis = 0; unsigned long receiveDataPrevMillis = 0; unsigned long timerDelay = 5000; unsigned long receiveDataInterval = 5000;
String distancePath = "/distance"; String timePath = "/timestamp"; String statusParkPath = "/status"; String deviceNamePath = "/name";
int timestamp; int statusPark = 0; int receivedDistance = 0;
FirebaseJson json;

void setup() {
  Serial.begin(115200);
  setupCaptivePortal();
  setupServerFunction();
  Wire.begin(slaveAddress);
  Wire.onReceive(receiveData);
}

void loop() {
  serverHandleFunction();
}
