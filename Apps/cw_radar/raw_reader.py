import serial
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from threading import Thread, Event
from queue import Queue

def serial_reader(port, baudrate, preamble, samples_per_frame, data_queue, stop_event):
    """Liest Daten von der seriellen Schnittstelle und füllt die Queue."""
    try:
        with serial.Serial(port, baudrate, timeout=None) as ser:
            print(f"Warte auf Preamble '{preamble.decode('utf-8')}'...")
            buffer = b''
            # Warte auf das Preamble
            while not stop_event.is_set():
                buffer += ser.read(1)
                if preamble in buffer:
                    print("Preamble erkannt!")
                    buffer = buffer[buffer.index(preamble) + len(preamble):]
                    break

            # Daten kontinuierlich lesen
            while not stop_event.is_set():
                frame_data = []
                for _ in range(samples_per_frame):
                    data = ser.read(2)
                    if len(data) == 2:
                        value = struct.unpack('<H', data)[0]
                        frame_data.append(value)
                    else:
                        print("Warnung: Unvollständige Daten empfangen")
                        frame_data.append(0)
                data_queue.put(frame_data)
    except serial.SerialException as e:
        print(f"Fehler beim Zugriff auf die serielle Schnittstelle: {e}")

def main():
    # Konfiguration
    port = '/dev/ttyUSB0'
    baudrate = 2000000
    preamble = b'XXXXXXX'
    sampling_rate = 50000  # Abtastfrequenz
    fps = 10               # Aktualisierungsrate
    samples_per_frame = sampling_rate // fps

    # Daten-Queue und Stop-Event
    data_queue = Queue()
    stop_event = Event()

    # Start der Thread zum Lesen der seriellen Daten
    reader_thread = Thread(target=serial_reader, args=(port, baudrate, preamble, samples_per_frame, data_queue, stop_event))
    reader_thread.start()

    # Vorbereitung der Anzeige
    fig, ax = plt.subplots()
    x_data = np.fft.rfftfreq(samples_per_frame, d=1/sampling_rate)
    y_data = np.zeros(len(x_data))
    line, = ax.plot(x_data, y_data)
    ax.set_xlim(0, sampling_rate / 2)
    ax.set_ylim(0, 1)
    ax.set_xlabel("Frequenz (Hz)")
    ax.set_ylabel("Amplitude")
    ax.set_title("Echtzeit-FFT")

    def update(frame):
        if not data_queue.empty():
            frame_data = np.array(data_queue.get()) - np.mean(data_queue.get())  # Gleichstromanteil entfernen
            fft_result = np.abs(np.fft.rfft(frame_data)) / samples_per_frame
            line.set_ydata(fft_result)
        return line,

    ani = FuncAnimation(fig, update, interval=1000/fps, blit=True)
    try:
        plt.show()
    finally:
        stop_event.set()
        reader_thread.join()

if __name__ == "__main__":
    main()
