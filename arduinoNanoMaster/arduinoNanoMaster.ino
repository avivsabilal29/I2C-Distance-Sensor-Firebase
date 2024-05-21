#include <Wire.h>

// Alamat I2C slave
const int slaveAddress = 0x08;

// Pin untuk sensor ultrasonik
const int ECHOPIN = 6;
const int TRIGPIN = 7;

long timer;
int jarak;

void setup() {
  // Inisialisasi komunikasi I2C sebagai master
  Wire.begin();
  Serial.begin(9600);

  // Inisialisasi pin sensor ultrasonik
  setupSensor();
}

void loop() {
  // Baca jarak dari sensor
  readSensor();

  // Memulai transmisi ke slave dengan alamat tertentu
  Wire.beginTransmission(slaveAddress);

  // Mengirimkan data jarak
  Wire.write(jarak);

  // Mengakhiri transmisi
  Wire.endTransmission();

  // Tampilkan data yang dikirim di Serial Monitor
  Serial.print("Distance sent: ");
  Serial.println(jarak);

  // Tunggu 1 detik sebelum mengirimkan data lagi
  delay(1000);
}

void setupSensor() {
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
}

void readSensor() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  timer = pulseIn(ECHOPIN, HIGH);
  jarak = timer / 58;
  delay(1000);
}
