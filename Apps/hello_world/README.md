# What is this thing?

This application (hello_world) sets up the clock tree, UART and FreeRTOS to verify its functionality

# How?

Connect a UART (FT232 or whatever) to PA10 (TX) and PA9 (RX).

# And now?

Open picocom on your FT232 at 4MBaud:

`# picocom -b 4000000 /dev/ttyUSB0` 

And you will see a piece of text every second spit out by a FreeRTOS task.
