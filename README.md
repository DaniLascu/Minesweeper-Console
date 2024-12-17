# Minesweeper-Console
## Introduction
  My project idea was to implement the well-known Minesweeper game using a microcontroller, a joystick and an LCD screen. I was motivated to choose this project topic because Minesweeper is a game that I have enjoyed playing since childhood. The purpose of the game is entertainment. The objective is to clear the board without detonating any mines in the grid, using clues about the number of neighboring mines in each cell.
  In my opinion, it is a good way to have fun and relieve stress.
## General Description
The Console is based on the ESP32-WROOM-32D microcontroller. A 2.2" 320x240px TFT LCD screen, with ILI9341 controller, is connected to the microcontroller through the SPI (VSPI) interface, which displays the game. The microcontroller is powered via a microUSB cable from a 5V DC source. The game is controlled via 2 pushbuttons and a joystick. The joystick is used to navigate through the game menu and to navigate through the cells on the game board. The buttons are used to mark a flag in a cell or to open a cell. The joystock has 5 pins: VCC connected to 3V3, GND, VRX - this pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in X-direction (axis), VRY - this pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in Y- direction (axis) and SW. The VRX and VRY pins are connected to the ADC pins SENSOR_VP and SENSOR_VN, which use the 3V3 reference voltage and capture the movemnt of the joystick. The SW pin and the other 2 pushbuttons are connected to normal GPIOs and use pull-up resistors. To play various short recordings saved in Flash memory, an 8 Ohm, 0.5W speaker is used , connected to an LM386 amplifier module, which can output 0.5W, when connected to 5V. The amplifier is connected to the DAC pin IO25, which outputs 3V3, and amplifies the analog signal received from it and then feeds it to the speaker.
### Block Diagram
![Schema_bloc](https://github.com/user-attachments/assets/877daeec-10f9-4f07-90f1-089c6d5fcd59)

## Hardware Design
## Software Design
## Obtained Results
## Conclusions
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
