# STM-Hero
The classic rhythm game Guitar Hero made with an STM32 Nucleo board.

## Features
-OLED menu with song selection and status display (I2C)
-real-time rhythm gameplay
-song notes spawned from MIDI file timing
-4-lane note system on daisy-chained LED matrixes (SPI)
-timestamped input processing driven by interrupt system
-non-blocking updates on peripherals

## Hardware
-STM32 Nucleo-F446RE
-Diplays
  -4x MAX7219 8x8 LED matrix modules
    -SPI interface
    -software framebuffer
    -real time gameplay rendering
  -OLED
  
