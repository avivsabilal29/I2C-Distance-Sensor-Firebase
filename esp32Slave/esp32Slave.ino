#include <Wire.h>

const int slaveAddress = 0x08;

// Variabel untuk menyimpan data yang diterima
int receivedDistance;

void setup() {
  // Inisialisasi komunikasi I2C sebagai slave dengan alamat tertentu
  Wire.begin(slaveAddress);

  // Menetapkan fungsi callback untuk menerima data
  Wire.onReceive(receiveData);

  Serial.begin(9600);
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop utama untuk slave
  // Data diterima di fungsi callback
}

void receiveData(int byteCount) {
  while (Wire.available()) {
    receivedDistance = Wire.read();
  }

  // Tampilkan data yang diterima di Serial Monitor
  Serial.print("Distance received: ");
  Serial.println(receivedDistance);
}
