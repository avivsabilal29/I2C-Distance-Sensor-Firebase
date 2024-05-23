import serial

# Inisialisasi port serial (sesuaikan dengan port yang digunakan)
wemos_port = "COM3"
baud = 115200

ser = serial.Serial(wemos_port, baud)
print("Connected to Wemos port " + wemos_port)

while True:
    try:
        # Baca data dari port serial
        data = ser.readline().decode().strip()
        if data:
            # Cek apakah baris mengandung data BPM (denyut jantung)
            if data.startswith("BPM="):
                bpm = float(data.split('=')[1])
                print("BPM:", bpm)
            # Cek apakah baris mengandung data suhu
            elif "Temperature:" in data:
                temperature = float(data.split(':')[1])
                print("Temperature:", temperature)
            # Cek apakah baris mengandung data IR
            elif data.startswith("IR="):
                ir_value = int(data.split('=')[1])
                print("IR Value:", ir_value)
    except UnicodeDecodeError:
        print("UnicodeDecodeError: Cannot decode data:", ser.readline())
        continue

# Tutup koneksi port serial
ser.close()
