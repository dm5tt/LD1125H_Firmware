# What is this thing?

This is a attempt of writing an alternative firmware for the LD1125H FMCW radar to extract raw ADC values from the Echo/Bang signal via high-speed UART. 

The hardware reverse engineering part was done in [this article](https://www.dm5tt.de/2024/12/14/ld1125H-reverse-engineering/).

# How to compile

First prepare CMake

```
# mkdir build
# cd build
# cmake ..
```

Then you can build targets

```
# make hello_world
```

which will spit out .elf, .bin and .hex files.


# Build Targets

### hello_world ✓

This target is a basic test to verify if the clock configuration, RTOS and UART is working correctly.

Connect a UART to PA10 (TX) and PA9 (RX) using a FTDI like FT232H at a baudrate of 4MBaud to see a text being printed out every second.
 
### cw_radar ✓

 - Set the DAC to a static value
 - Sample the Bang signal at 50kHz with ADC DMA into an A/B buffer
 - Output it to the serial port buffer using  UART DMA TX
 - Visualize in Python (FFT)

![vlcsnap-2024-12-28-21h09m07s086](https://github.com/user-attachments/assets/9ca926fc-e758-4770-bf42-3fb6cb9ba38e)


[LD1125H shows the FFT of the Bang signal of a spinning fan](Docs/LD1125H_CW_Radar_FAN.mp4)


# Credits

Template heavily adapted from https://github.com/djix123/GD32F303CG_CMake

Linker Scripts and Startup from https://github.com/cjacker/gd32f30x_firmware_library_gcc_makefile

OpenOCD script https://github.com/gd32-rs/gd32-openocd
