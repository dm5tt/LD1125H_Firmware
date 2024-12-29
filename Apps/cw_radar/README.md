# What is this thing?

This application (cw_radar) set up a basic CW radar using the LD1125H

# How?

1. Set the DAC to a static value
2. Sets a timer to 50kHz which triggers the ADC
3. The ADC reads out the Bang/Echo signal and writes it into an A/B buffer using DMA
4. The UART write these buffer to the interface using DMA

The RTOS is not (= only a vTaskDelay) being used here.

# And now?

You can use "raw_reader.py" to visualize the spectrum of the signal.

Beware: no auto-scaling was implemented. You must use the Zoom button.

# How to start

1. Start the Python application first. It will wait for the preamble.

2. Flash and run the firmware LD1125H


# Limitations

Sometimes the Python script hickups and it losses the track of the raw frames.

Maybe someone should implement a small TLV protocol here to properly catch frame start and verify its CRC.
