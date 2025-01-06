# Minesweeper-Console
## Introduction
  My project idea was to implement the well-known Minesweeper game using a microcontroller, a joystick and an LCD screen. I was motivated to choose this project topic because Minesweeper is a game that I have enjoyed playing since childhood. The purpose of the game is entertainment. The objective is to clear the board without detonating any mines in the grid, using clues about the number of neighboring mines in each cell.
  In my opinion, it is a good way to have fun and relieve stress.
## General Description
The Console is based on the ESP32-WROOM-32D microcontroller. A 2.2" 320x240px TFT LCD screen, with ILI9341 controller, is connected to the microcontroller through the SPI (VSPI) interface, which displays the game. The microcontroller is powered via a microUSB cable from a 5V DC source. The game is controlled via 2 pushbuttons and a joystick. The joystick is used to navigate through the game menu and to navigate through the cells on the game board. The buttons are used to mark a flag in a cell or to open a cell. The joystock has 5 pins: VCC connected to 3V3, GND, VRX - this pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in X-direction (axis), VRY - this pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in Y- direction (axis) and SW. The VRX and VRY pins are connected to the ADC pins SENSOR_VP and SENSOR_VN, which use the 3V3 reference voltage and capture the movemnt of the joystick. The SW pin and the other 2 pushbuttons are connected to normal GPIOs and use pull-up resistors. <!--To play various short recordings saved in Flash memory, an 8 Ohm, 0.5W speaker is used , connected to an LM386 amplifier module, which can output 0.5W, when connected to 5V. The amplifier is connected to the DAC pin IO25, which outputs 3V3, and amplifies the analog signal received from it and then feeds it to the speaker.-->I also added a MicroSD Card to the project, in order to be able to save the Top 10 lowest solving times of the game. The MicroSD Adapter is connected to the microcontroller via the VSPI interface.
### Block Diagram
![Schema_bloc_v2](https://github.com/user-attachments/assets/10698412-0361-49e7-96c8-973664d9bffa)

## Hardware Design
### Electrical Diagram
![Schema_electrica_v2](https://github.com/user-attachments/assets/76d80705-9cd3-4a37-bac9-5f7e8fd9adaf)
### Bill Of Materials
|Nr.|Component|Quantity|Datasheet|
|:---:|:---|:---:|:---:|
|1.|[ESP32 Development Board](https://www.optimusdigital.ro/ro/placi-cu-bluetooth/4371-placa-de-dezvoltare-esp32-cu-wifi-i-bluetooth-42.html)|1|[link](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)|
|2.|[TFT LCD 2.2" 320x240 px with SPI, ILI9341](https://www.optimusdigital.ro/en/lcds/1260-lcd-spi-22-240x320-px.html?srsltid=AfmBOoomwC89ioOqQza2nfAfHylgQTiiy7VlUNdnHcffcxHRP7E-kHHq)|1|[link](https://www.hpinfotech.ro/ILI9341.pdf)|
|3.|[MicroSD Card Adapter](https://www.electronicmarket.ro/ro/product/mini-modul-card-sd?gad_source=1&gclid=CjwKCAiAm-67BhBlEiwAEVftNrN8-f85QN9-n_2M5kz5bxjl8-hzS39afzuyPs_KuE6gahEWGtsnBBoCS30QAvD_BwE)|1|N/A|
|4.|[MicroSD Card 32GB, FAT32](https://altex.ro/card-de-memorie-hama-124151-microsdhc-32gb-80mb-s-clasa-10-u1-v10-uhs-i-adaptor/cpd/CRD124151/)|1|N/A|
|5.|330 Ohm Resistor|2|N/A|
|6.|Pushbuttons|2|N/A|
|7.|Analog Joystick|1|[link](https://components101.com/modules/joystick-module)|
|8.|Wires|too many|N/A|
<!--|3.|[LM386 Audio Amplifier Module](https://www.sigmanortec.ro/Modul-amplificator-audio-LM386-p136254255?fast_search=fs)|1|[link](https://www.ti.com/lit/ds/symlink/lm386.pdf)|-->
<!--|4.|[8 Ohm 0.5W Speaker](https://www.conexelectronic.ro/difuzoare/6157-DIFUZOR-0-5-W-8-OHMI-66-MM.html)|1|N/A|-->
### Pin Configuration
- I connected the TFT LCD via one of the ESP32's SPI interfaces - VSPI -. The pins I used are: IO23 = MOSI, IO19 = MISO, IO18 = CLK, IO5 = CS, D/C = IO2, RESET = IO4 and LED = 3V3.
- The MicroSD Adapter Module is connected via SPI interface to the microcontroller - VSPI. I used  the same pins for MOSI, MISO, CLK as for the TFT screen, but I used a different pin IO16, for CS.
- The joystick's Ox and Oy axis movement are translated to Voltage, then transformed into digital values by the ADC pins SENSOR_VP and SENSOR_VN, that use a refference voltage of 3V3. The software uses those values and some thresholds to decide if the movent of the joystick is Left, Right, Up or Down.
- The Joystick pushbutton is connected to GPIO32
- The other 2 pushbuttons are connected to GPIO27 and GPIO33. I use 330 Ohm resistors for both the puthbuttons to use the input-pullup mode.
  |Component|ESP32 Pins|Description|
  |:---:|:---|:---|
  |Buttons|IO27, IO33|use 330 Ohm pullup-resistors|
  |Joystick(VRY,VRX,SW)|SENSOR_VP, SENSOR_VN, IO32|Analog inputs for the horizontal and vertical axis mvement; Uses ADC to convert analogue values into digital ones|
  |TFT Display|IO23 = MOSI, IO19 = MISO, IO18 = CLK, IO5 = CS, IO2 = D/C, IO4 = RESET, 3V3 = LED|VSPI interface|
  |MicroSD Card Adapter|IO23 = MOSI, IO19 = MISO, IO18 = CLK, IO16 = CS, 3V3 = VCC, GND|VSPI interface|
## Software Design
### Development Environment
I used PlatformIO IDE for VSCode.
### Game Flow
The player starts the game off in the Main Menu. There, he has 2 options: 1.Start Game; 2.Top 10 Times. The player uses the joystick in order to chose one option. If the player choses Top 10 Times, he will be displayed the Top 10 lowest game solve times. If the player presses the joystick button again he will be brought back to the Main Menu. If the player choses Start Game, he will be then asked to chose the difficulty. The difficulty is selected using the joystick and the joystick button. After the difficulty is selected the game starts. To the left of the screen the time that has passed is displayed, being incremented every second. If the player presses the joystick button again during the play time, the game will be paused. The player will be prompted to chose to Resume or to Quit the Game. If he choses to quit he will be brought back to the Main Menu. If he choses Resume, he will be brought back to the game. The time counter pauses during pause time. The player uses other 2 buttons during the game, one to open a cell that he belives is empty and the other to flag a cell he thinks has a mine inside. If the player opens a cell that has a mine, he will lose the game and will be brought back to the main menu. If the player celars all mines, he WON! If he has a time in Top 10 Times, he will be prompted for his name. Then he is brought back to the Main Menu.
![Game_Flow_Diagram](https://github.com/user-attachments/assets/a3c4e45e-c1b3-49ce-b804-bfb6f594c72c)
### <ins>Algorithms and Important Code Parts</ins>
### Board Generation and fill Algorithm
Every cell of the board is in fact a variable of the Struct type shown below. The struct has 4 attributes, that store the following: 
- if the cell contains a mine;
- number of neighbour mines;
- if the cell is flagged;
- if the cell is open;
```C
// struct ce reprezinta o celula din grid
struct cell{
  bool mine; //1 daca o bomba se afla in celula; 0 daca nu 
  uint8_t mines_num; //nr de bombe adiacente
  bool flaged; //1 daca player-ul a marcat celula ca bomba; 0 altfel
  bool open; //1 daca player-ul a deschis celula considerand ca nu exista bombe; 0 daca celula esta inca inchisa
};

struct cell bomb_grid[ROWS][COLUMNS]; 
```
## Obtained Results
## Conclusions
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
