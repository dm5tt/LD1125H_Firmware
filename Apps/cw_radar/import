import serial
import struct
import time

def main():
    # Konfiguration der seriellen Schnittstelle
    port = '/dev/ttyUSB0'
    baudrate = 2000000
    preamble = b'XXXXXXX'  # Erwartetes Preamble
    
    try:
        # Öffne die serielle Verbindung
        with serial.Serial(port, baudrate, timeout=None) as ser:  # Kein Timeout
            print(f"Warte auf Preamble '{preamble.decode('utf-8')}'...")
            
            buffer = b''
            while True:
                # Lese Daten byteweise
                buffer += ser.read(1)

                # Prüfe, ob das Preamble erkannt wurde
                if preamble in buffer:
                    print("Preamble erkannt!")
                    buffer = buffer[buffer.index(preamble) + len(preamble):]

                # Empfange uint16_t-Werte, wenn genügend Daten vorhanden sind
                while len(buffer) >= 2:
                    data = buffer[:2]
                    buffer = buffer[2:]
                    value = struct.unpack('<H', data)[0]  # Little Endian uint16_t
                    timestamp = int(time.time())  # Unix-Timestamp
                    print(f"[{timestamp}] {value}")

    except serial.SerialException as e:
        print(f"Fehler beim Zugriff auf die serielle Schnittstelle: {e}")
    except KeyboardInterrupt:
        print("Programm beendet.")

if __name__ == "__main__":
    main()

