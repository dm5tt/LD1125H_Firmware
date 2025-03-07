import serial
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from threading import Thread, Event
from queue import Queue
import time
import sys
import argparse
import os
from datetime import datetime

# Define reserved symbols and XOR value for byte-stuffing
FRAME_START  = 0x7E
FRAME_STOP   = 0x7F
FRAME_ESCAPE = 0x7D
XOR_VALUE    = 0x20

def read_frame(ser):
    """
    Reads one complete frame from the serial port with debytestuffing.
    Returns:
      A bytearray containing the de-stuffed frame data (payload + checksum),
      or an empty bytearray if a frame isn’t successfully read.
    """
    while True:
        b = ser.read(1)
        if not b:
            continue
        if b[0] == FRAME_START:
            break

    frame_data = bytearray()
    while True:
        b = ser.read(1)
        if not b:
            continue
        byte = b[0]

        if byte == FRAME_ESCAPE:
            b = ser.read(1)
            if not b:
                continue  # Incomplete escape sequence
            byte = b[0] ^ XOR_VALUE

        if byte == FRAME_STOP:
            break

        frame_data.append(byte)

    return frame_data

def rotate_file(raw_data_file, base_filename, rotation_count):
    """Rotates the file by closing the current one and opening a new one with an incremented suffix."""
    raw_data_file.close()
    new_filename = f"{base_filename}_{rotation_count}.bin"
    raw_data_file = open(new_filename, 'wb')
    print(f"Rotated to new file: {new_filename}")
    return raw_data_file, rotation_count + 1

def serial_reader(port, baudrate, samples_per_frame, data_queue, stop_event, stats_event, stats, save_raw_data, raw_data_file, base_filename, rotate_time, rotate_size, skip_crc_errors):
    """Reads framed and debytestuffed data from the serial port, accumulating samples if necessary."""
    try:
        with serial.Serial(port, baudrate, timeout=None) as ser:
            print("Waiting for frames...")

            accumulated_samples = []
            rotation_count = 0
            start_time = time.time()
            bytes_written = 0

            if save_raw_data:
                if raw_data_file is None:
                    print("Error: Raw data file is None, cannot save data!")
                    return
                print(f"Saving raw data to: {raw_data_file.name}")

            while not stop_event.is_set():
                frame_data = read_frame(ser)
                if not frame_data:
                    continue

                stats["frames_processed"] += 1

                if len(frame_data) < 2:
                    continue

                payload, received_checksum = frame_data[:-1], frame_data[-1]
                computed_checksum = sum(payload) & 0xFF

                if computed_checksum != received_checksum:
                    stats["crc_errors"] += 1
                    if skip_crc_errors:
                        continue  # Skip this frame if CRC error is detected and skipping is enabled

                if len(payload) % 2 != 0:
                    payload = payload[:-1]

                sample_values = [
                    struct.unpack('<H', payload[i:i+2])[0] & 0x0FFF
                    for i in range(0, len(payload), 2)
                ]
                
                accumulated_samples.extend(sample_values)

                while len(accumulated_samples) >= samples_per_frame:
                    frame_to_send = accumulated_samples[:samples_per_frame]
                    accumulated_samples = accumulated_samples[samples_per_frame:]
                    data_queue.put(frame_to_send)
                    stats["samples_processed"] += samples_per_frame

                    if save_raw_data:
                        raw_data = np.array(frame_to_send, dtype=np.uint16).tobytes()
                        raw_data_file.write(raw_data)
                        raw_data_file.flush()  # Flush immediately after writing
                        bytes_written += len(raw_data)

                        # Rotate file based on time or size
                        if (rotate_time > 0 and time.time() - start_time >= rotate_time) or \
                           (rotate_size > 0 and bytes_written >= rotate_size * 1024 * 1024):
                            raw_data_file, rotation_count = rotate_file(raw_data_file, base_filename, rotation_count)
                            start_time = time.time()
                            bytes_written = 0

                if time.time() - stats["last_report_time"] >= 1.0:
                    stats_event.set()
                    stats["last_report_time"] = time.time()

    except serial.SerialException as e:
        print(f"Serial error: {e}")

def stats_printer(stats_event, stats, stop_event):
    """Prints the number of samples, frames, and CRC errors per second."""
    while not stop_event.is_set():
        stats_event.wait()
        stats_event.clear()

        sys.stdout.write(
            f"\rSamples/s: {stats['samples_processed']} | Frames/s: {stats['frames_processed']} | CRC Errors/s: {stats['crc_errors']}  "
        )
        sys.stdout.flush()

        stats["samples_processed"] = 0
        stats["frames_processed"] = 0
        stats["crc_errors"] = 0

def main():
    parser = argparse.ArgumentParser(description="Real-time FFT visualization from serial data.")
    parser.add_argument('--port', type=str, default='/dev/ttyUSB0', help='Serial port to use (default: /dev/ttyUSB0)')
    parser.add_argument('--baudrate', type=int, default=4000000, help='Baud rate for serial communication (default: 4000000)')
    parser.add_argument('--save-raw', type=str, help='Filename to save raw uint16_t samples (e.g., raw_samples.bin)')
    parser.add_argument('--rotate-time', type=int, default=0, help='Rotate file after X seconds (default: 0, no rotation)')
    parser.add_argument('--rotate-size', type=int, default=0, help='Rotate file after X MB (default: 0, no rotation)')
    parser.add_argument('--skip-crc-errors', action='store_true', help='Skip frames with CRC errors (default: False)')
    args = parser.parse_args()

    sampling_rate = 50000  # Hz
    fps = 10
    samples_per_frame = sampling_rate // fps

    data_queue = Queue()
    stop_event = Event()
    stats_event = Event()

    stats = {
        "samples_processed": 0,
        "frames_processed": 0,
        "crc_errors": 0,
        "last_report_time": time.time(),
    }

    raw_data_file = None
    base_filename = None
    if args.save_raw:
        try:
            base_filename = os.path.splitext(args.save_raw)[0]
            raw_data_file = open(args.save_raw, 'wb')
            print(f"Opened raw data file: {args.save_raw}")
        except Exception as e:
            print(f"Error opening file {args.save_raw}: {e}")
            return

    reader_thread = Thread(target=serial_reader, args=(args.port, args.baudrate, samples_per_frame, data_queue, stop_event, stats_event, stats, bool(args.save_raw), raw_data_file, base_filename, args.rotate_time, args.rotate_size, args.skip_crc_errors))
    reader_thread.start()

    stats_thread = Thread(target=stats_printer, args=(stats_event, stats, stop_event))
    stats_thread.start()

    fig, ax = plt.subplots()
    x_data = np.fft.rfftfreq(samples_per_frame, d=1/sampling_rate)
    y_data = np.zeros(len(x_data))
    line, = ax.plot(x_data, y_data)

    ax.set_xlim(0, sampling_rate / 2)
    ax.set_ylim(0, 1)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Amplitude")
    ax.set_title("Real-time FFT")

    def update(frame):
        if not data_queue.empty():
            frame_data = np.array(data_queue.get(), dtype=np.float64)
            frame_data -= np.mean(frame_data)
            frame_data /= 4095.0

            fft_result = np.abs(np.fft.rfft(frame_data)) / len(frame_data)
            
            if len(fft_result) == len(x_data):
                line.set_ydata(fft_result)
            else:
                print(f"FFT length mismatch: {len(fft_result)} vs {len(x_data)}")

        return line,

    ani = FuncAnimation(fig, update, interval=1000/fps, blit=False, save_count=100)

    try:
        plt.show()
    finally:
        stop_event.set()
        reader_thread.join()
        stats_thread.join()
        if raw_data_file:
            raw_data_file.close()
            print("Raw data file closed.")

if __name__ == "__main__":
    main()
