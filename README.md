# What is this thing?

This is a attempt to write an alternative firmware for the LD1125H FMCW radar to extract raw ADC values from the Echo/Bang signal via high-speed UART.

The hardware reverse engineering part was done in [this article](https://www.dm5tt.de/2024/12/14/ld1125H-reverse-engineering/).


# Build Targets

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
