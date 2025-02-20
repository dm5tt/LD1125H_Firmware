# What is this thing?

This application (cw_radar) sets up a basic CW radar using the LD1125H

# How?

1. Set the DAC to a static value
2. Sets a timer to 50kHz which triggers the ADC
3. The ADC reads out the Bang/Echo signal and writes it into a double buffer using DMA
4. The UART write these buffer to the interface using DMA

The entire implementation uses Byte-Stuffing and a small frame (FRAME_START + <Byte Stuffed Data> + CRC).

The RTOS is not (= only a vTaskDelay) being used here.

# And now?

You can use "fft_reader.py" to visualize the spectrum of the signal.

# How to start

1. Flash and run the firmware LD1125H

2. Execute the Python application

```
python3 fft_reader.py --help
usage: fft_reader.py [-h] [--port PORT] [--baudrate BAUDRATE] [--save-raw SAVE_RAW]

Real-time FFT visualization from serial data.

options:
  -h, --help           show this help message and exit
  --port PORT          Serial port to use (default: /dev/ttyUSB0)
  --baudrate BAUDRATE  Baud rate for serial communication (default: 4000000)
  --save-raw SAVE_RAW  Filename to save raw uint16_t samples (e.g., raw_samples.bin)
```