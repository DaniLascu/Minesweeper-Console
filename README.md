# Minesweeper-Console
## Introduction
  My project idea was to implement the well-known Minesweeper game using a microcontroller, a joystick and an LCD screen. I was motivated to choose this project topic because Minesweeper is a game that I have enjoyed playing since childhood. The purpose of the game is entertainment. The objective is to clear the board without detonating any mines in the grid, using clues about the number of neighboring mines in each cell.
  In my opinion, it is a good way to have fun and relieve stress.
## General Description
The Console is based on the ESP32-WROOM-32D microcontroller. A 2.2" 320x240px TFT LCD screen, with ILI9341 controller, is connected to the microcontroller through the SPI (VSPI) interface, which displays the game. The microcontroller is powered via a microUSB cable from a 5V DC source. The game is controlled via 2 pushbuttons and a joystick. The joystick is used to navigate through the game menu and to navigate through the cells on the game board. The buttons are used to mark a flag in a cell or to open a cell. The joystock has 5 pins: VCC connected to 3V3, GND, VRX - this pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in X-direction (axis), VRY - this pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in Y- direction (axis) and SW. The VRX and VRY pins are connected to the ADC pins SENSOR_VP and SENSOR_VN, which use the 3V3 reference voltage and capture the movemnt of the joystick. The SW pin and the other 2 pushbuttons are connected to normal GPIOs and use pull-up resistors. To play various short recordings saved in Flash memory, an 8 Ohm, 0.5W speaker is used , connected to an LM386 amplifier module, which can output 0.5W, when connected to 5V. The amplifier is connected to the DAC pin IO25, which outputs 3V3, and amplifies the analog signal received from it and then feeds it to the speaker.
### Block Diagram
![Schema_bloc](https://github.com/user-attachments/assets/877daeec-10f9-4f07-90f1-089c6d5fcd59)

## Hardware Design
### Electrical Diagram
![Schema_electrica](https://github.com/user-attachments/assets/c217cd65-7bc2-4756-9e7f-01a78813f73f)
### Bill Of Materials
|Nr.|Component|Quantity|Datasheet|
|:---:|:---|:---:|:---:|
|1.|[ESP32 Development Board](https://www.optimusdigital.ro/ro/placi-cu-bluetooth/4371-placa-de-dezvoltare-esp32-cu-wifi-i-bluetooth-42.html)|1|[link](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)|
|2.|[TFT LCD 2.2" 320x240 px with SPI](https://www.optimusdigital.ro/en/lcds/1260-lcd-spi-22-240x320-px.html?srsltid=AfmBOoomwC89ioOqQza2nfAfHylgQTiiy7VlUNdnHcffcxHRP7E-kHHq)|1|[link](https://www.hpinfotech.ro/ILI9341.pdf)|
|3.|[LM386 Audio Amplifier Module](https://www.sigmanortec.ro/Modul-amplificator-audio-LM386-p136254255?fast_search=fs)|1|[link](https://www.ti.com/lit/ds/symlink/lm386.pdf)|
|4.|[8 Ohm 0.5W Speaker](https://www.conexelectronic.ro/difuzoare/6157-DIFUZOR-0-5-W-8-OHMI-66-MM.html)|1|N/A|
|5.|330 Ohm Resistor|2|N/A|
|6.|Pushbuttons|2|N/A|
|7.|Analog Joystick|1|[link](https://components101.com/modules/joystick-module)|
|8.|Wires|too many|N/A|
### Pin Configuration
- I connected the TFT LCD via one of the ESP32's SPI interfaces - VSPI -. The pins I used are: IO23 = MOSI, IO19 = MISO, IO18 = CLK, IO5 = CS, D/C = IO2, RESET = IO4 and LED = 3V3.
- The input analog signal of the speaker is given through the DAC1 - IO25 pin and is first passed to the LM386 amplifier module that raises it to 0.5 W.
- The joystick's Ox and Oy axis movement are translated to Voltage by the ADC pins SENSOR_VP and SENSOR_VN, that use a refference voltage of 3V3. The software uses those values and some thresholds to decide if the movent of the joystick is Left, Right, Up or Down.
- The Joystick pushbutton is connected to GPIO32
- The other 2 pushbuttons are connected to GPIO27 and GPIO33. I use 330 Ohm resistors for both the puthbuttons to use the input-pullup mode.
  |Component|ESP32 Pins|Description|
  |:---:|:---|:---|
  |Buttons|IO27, IO33|use 330 Ohm pullup-resistors|
  |Joystick(VRY,VRX,SW)|SENSOR_VP, SENSOR_VN, IO32|Analog inputs for the horizontal and vertical axis mvement|
  |TFT Display|IO23 = MOSI, IO19 = MISO, IO18 = CLK, IO5 = CS, D/C = IO2, RESET = IO4, LED = 3V3|VSPI interface|
  |LM386 Amplifier Module|IO25|DAC pin|
## Software Design
## Obtained Results
## Conclusions
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
