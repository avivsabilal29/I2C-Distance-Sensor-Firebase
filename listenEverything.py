import serial

# Inisialisasi port serial (sesuaikan dengan port yang digunakan)
wemos_port = "COM9"
baud = 9600

ser = serial.Serial(wemos_port, baud)
print("Connected to Wemos port " + wemos_port)

try:
    while True:
        try:
            # Baca data dari port serial
            data = ser.readline().decode().strip()
            if data:
                print("Data received:", data)
        except UnicodeDecodeError:
            print("UnicodeDecodeError: Cannot decode data:", ser.readline())
            continue
except KeyboardInterrupt:
    print("Interrupted by user")

# Tutup koneksi port serial
ser.close()
