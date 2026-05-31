# STM-Hero
The classic rhythm game Guitar Hero made with an STM32 Nucleo board.

## Features
- OLED menu with song selection and status display (I2C)
- -time rhythm gameplay
- song notes spawned from MIDI file timing
- 4-lane note system on daisy-chained LED matrixes (SPI)
- timestamped input processing driven by interrupt system
- non-blocking updates on peripherals

## Hardware
- STM32 Nucleo-F446RE
- Displays
  - 4x MAX7219 8x8 LED matrix modules
    - SPI interface
    - software framebuffer
    - real time gameplay rendering
  - OLED
    - SSD1306 OLED display
    - I2C interface
    - menu system with gameplay states
- Inputs
  - Enter button and 4 note buttons
  - EXTI interrupt driven
  - timestamped event queue
- Timing
  - TIM2 configured as free running 32-bit timer
    - Prescaler: 84 - 1
    - Frequency: 1 MHz
    - used for gameplay and input processing

## Things I Learned

Here are some things I learned about embedded development while creating this game:


## Overview of Main Loop and Gameplay Systems

Here is the main loop:

> while (1) { 
> uint32_t now = micros(); 
> uint32_t song_time = now - song_start_time; 
> GameUpdate(song_time); 
> GameRender(song_time); 
> ProcessInputs(song_time); 
> LED_Build_Frame(); 
> }
    
  
