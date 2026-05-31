# STM-Hero
Wanted to make my own version of the classic rhythm game Guitar Hero on my STM32 Nucleo board (also inspiration from @hoff._world on YouTube). Below are some features, hardware, a brief overview of the code, and things I learned along the way. A python script was initially used to convert the MIDI song file to timestamped note events for gameplay timings. The rest of the project was done in STM32CubeIDE using HAL drivers. Right now the game requires you to start the song file (in .WAV format) on another device at the same time as you start the song on the STM32. My future plans are to add some sort of DAC and output audio directly from the MCU. First, here are a few pictures of the project and videos showing a brief portion of the gameplay:

<img width="288" height="383" alt="PXL_20260531_144535761" src="https://github.com/user-attachments/assets/79ac0094-9254-43d3-9005-47d812a2ec0a" />
<img width="288" height="383" alt="PXL_20260531_144605324" src="https://github.com/user-attachments/assets/a12e2619-1e6e-4222-aeac-593678bc0c99" />
<img width="288" height="383" alt="PXL_20260531_144614059" src="https://github.com/user-attachments/assets/807b3365-f435-4339-a39e-ef681937673b" />
<img width="288" height="383" alt="PXL_20260531_144818203 MP" src="https://github.com/user-attachments/assets/459cecd3-b3e7-4f41-9ffb-cbd3a40872e2" />
https://github.com/user-attachments/assets/9f9db17e-bbb0-4606-9cbf-cb2c49498580
<img width="288" height="383" alt="PXL_20260531_144802651" src="https://github.com/user-attachments/assets/f7392152-ddac-4868-b186-da7925c25c5b" />






## Features
- OLED menu with song selection and status display (I2C)
- time rhythm gameplay
- song notes spawned from MIDI file timing
- 4-lane note system on daisy-chained LED matrices (SPI)
- timestamped input processing driven by interrupt system
- non-blocking updates on peripherals

## Hardware
- STM32 Nucleo-F446RE
- Displays
  - 4x MAX7219 8x8 LED matrix modules
    - SPI interface
    - software framebuffer
    - real time gameplay rendering
  - SSD1306 OLED display
    - I2C interface
    - menu system with gameplay states
- Inputs
  - 1 enter button and 4 note buttons
  - EXTI interrupt driven
  - timestamped event queue
- Timing
  - TIM2 configured as free running 32-bit timer
    - Prescaler: 84 - 1
    - Frequency: 1 MHz
    - used for gameplay and input processing

## Things I Learned

Here are some things I learned about embedded development while creating this game:

- deterministic scheduling and real time gameplay behavior
- SyStick and how interrupts affect timing, free running timer to prevent stale counter
- interrupt-driven input handling, EXTI
  - ISR should be minimal, non-blocking
- embedded graphics, framebuffers, rendering
- I2C
- SPI
- UART + debugging
- DMA peripheral communication
- embedded C fundamentals of volatile, pointers, arrays, buffers, structs, bit manipulation
- constraints on storage sizes
  - how memory works, flash vs RAM, what goes where including consts, globals, locals

## Overview of Main Loop and Gameplay Systems

### Here is the main loop:

```c
while (1) {
    uint32_t now = micros();
    uint32_t song_time = now - song_start_time;

    GameUpdate(song_time);
    GameRender(song_time);
    ProcessInputs(song_time);

    LED_Build_Frame();
}
```

``` micros() ``` returns the current value of TIM2 in microseconds and is used by main loop to update current song time.

``` GameUpdate(song_time) ``` checks the current game state, updates active song notes based on current song time, and renders the OLED menu.

``` GameRender(song_time) ``` updates the LED framebuffer based on the active notes array.

``` ProcessInputs(song_time) ``` checks timestamped inputs created from ISRs, judges the accuracy of input "hits" and updates the total game scores.

``` LED_Build_Frame() ``` uses the LED framebuffer to prepare a memory frame and transmit frame used to send to the LED matrices.

### Here is the Game State System:

```c
typedef enum
{
  GAME_STATE_MENU,
  GAME_STATE_RUNNING,
  GAME_STATE_RESULTS
} GameState;
```

The current state of the game is used to control when the song starts, gameplay, and proper menu screens for the OLED.

### Song Format

Songs are represented as time stamped note events. Here is what each note looks like:

```c
typedef struct
{
  uint32_t time;
  uint8_t lanes;
} Note;
```

Here is an example of a song as an array of Notes:

```c
const Note song_Tetris[] = {
		{5000000, 0x40},
		{5250000, 0x10},
		{5500000, 0x20},
		{5750000, 0x20},
		{6000000, 0x40},
		{6250000, 0x10},
};
```

Each bit in ``` lanes ``` corresponds to one of the 8 columns of the LED matrices (4 playable lanes each 2 columns wide).

### Active Note System

Runtime note objects are stored separately from song data:

```c
typedef struct {
	uint32_t hit_time;
	uint8_t lane;
	uint8_t active;
	uint8_t hit;
} ActiveNote;
```

The active_notes array tracks spawned notes from the song data, handles note expiration, and supports hit detection from interrupt driven inputs.

### Input System

Inputs are timestamped when they occur and stored in a ring buffer.

``` PushInputEvent(lane, time) ``` is used inside the ISR to timestamp the lane being hit. An InputEvent is pushed into the input_queue ring buffer.

```c
typedef struct
{
  uint8_t lane;
  uint32_t timestamp;
} InputEvent;
```

``` ProcessInput() ``` is then used to traverse the ring buffer (tail catching up to head) and judge each lane hit to add to total scores.
