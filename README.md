# What is this thing?

This is a attempt to write an alternative firmware for the LD1125H FMCW radar to extract raw ADC values from the Echo/Bang signal via high-speed UART.

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

### hello_world

This target is a basic test to verify if the clock configuration, RTOS and UART is working correctly.

Connect a UART to PA10 (TX) and PA9 (RX) using a FTDI like FT232H at a baudrate of 4MBaud to see a text being printed out every second.

### remote_control

Will be the first throw of implementing a easy and cheap TLV command parser. 


### radar_cw

Todo

### radar_fmcw_ramp

Todo

### radar_fmcw_sinus

Todo

# How to build?

```
# mkdir build
# cd build
# cmake ..
# make hello_world
```

# Credits

Template heavily adapted from https://github.com/djix123/GD32F303CG_CMake

Linker Scripts and Startup from https://github.com/cjacker/gd32f30x_firmware_library_gcc_makefile

OpenOCD script https://github.com/gd32-rs/gd32-openocd
