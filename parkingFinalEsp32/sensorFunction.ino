void receiveData(int byteCount) {
  while (Wire.available()) {
    receivedDistance = Wire.read();
  }

  // Tampilkan data yang diterima di Serial Monitor
  Serial.print("Distance received: ");
  Serial.println(receivedDistance);
}
