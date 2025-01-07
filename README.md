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
The player starts the game off in the Main Menu. There, he has 2 options: 1.Start Game; 2.Top 10 Times. The player uses the joystick in order to chose one option. If the player choses Top 10 Times, he will be displayed the Top 10 lowest game solve times. If the player presses the joystick button again he will be brought back to the Main Menu. If the player choses Start Game, he will be then asked to chose the difficulty. The difficulty is selected using the joystick and the joystick button. After the difficulty is selected the game starts. To the left of the screen the time that has passed is displayed, being incremented every second. If the player presses the joystick button again during the play time, the game will be paused. The player will be prompted to chose to Resume or to Quit the Game. If he choses to quit he will be brought back to the Main Menu. If he choses Resume, he will be brought back to the game. The time counter pauses during pause time. The player uses other 2 buttons during the game, one to open a cell that he belives is empty and the other to flag a cell he thinks has a mine inside. If the player opens a cell that contains a mine, he will lose the game and will be brought back to the main menu. If the player celars all mines, he WON! If he has a time in Top 10 Times, he will be prompted for his name. Then he is brought back to the Main Menu.
![Game_Flow_Diagram](https://github.com/user-attachments/assets/a3c4e45e-c1b3-49ce-b804-bfb6f594c72c)
### <ins>Algorithms and Implemented Functions</ins>
### *Board Generation and fill Algorithm*  
<details>
<summary>Click here</summary>
Every Cell of the board is in fact a variable of the Struct type shown below. The struct has 4 attributes, that store the following:

- if the cell contains a mine;
- number of neighbour mines;
- if the cell is flagged;
- if the cell is open;
  
The Cell Grid is a **10x10 matrix of "struct cell"** variables.

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
Every time the **Game** is **STARTED**, the program must generate the **Mine Grid**. As it is shown at first, every cell is closed, unflagged, and has 0 mines.
The flaged and open attributes are used when displaying the grid. Then this function randomly generates and places mines. The number of mines depends on the difficulty that was selected.
The function also checks that a mine is not already present in the cell it wants to place the new mine in.
```C
#define MINES_EASY 8
#define MINES_MEDIUM 10
#define MINES_HARD 15

void generate_bombs(int difficulty){
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLUMNS; j++){
      bomb_grid[i][j].mine = false;
      bomb_grid[i][j].mines_num = 0;
      bomb_grid[i][j].flaged = false;
      bomb_grid[i][j].open = false;
    }
  }

  srand(time(0)); //use current time for seed

  for(int i = 0; i < difficulty; i++){
    int poz_i = rand() % ROWS;
    int poz_j = rand() % COLUMNS;

    if(bomb_grid[poz_i][poz_j].mine == true){//daca o bomba e deja pe acea pozitie mai generez o data pozitiile
      i--;
    }
    else{
      bomb_grid[poz_i][poz_j].mine = true; //altfel se pune o mina in acea celula
    }
  }
}
```
The first function checks if the current position, the one I am checking at the moment is inside the **boundaries** of the **board matrix**.
The second function calculates the number of neighbouring mines of every cell. If the current cell has a mine in it, that that calculation doesn't take place.
Every time the game starts, after the mines are randomly generated and placed, the second function from below is called.
```C
bool valid_position(int poz_i, int poz_j){ //verifica deca elementul pe care il verific nu a iesit in afara matricei
  return (poz_i >= 0 && poz_i < ROWS) && (poz_j >= 0 && poz_j < COLUMNS);
}


void calculate_number_of_neighbour_bombs(){
  int nr = 0; //nr de bombe care invecineaza o celula
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLUMNS; j++){
      nr = 0;
      if(bomb_grid[i][j].mine == false){ //daca nu se afla o mina in acea celula
        if(valid_position(i-1,j-1) && bomb_grid[i-1][j-1].mine == true)
            nr++;
        if(valid_position(i-1,j) && bomb_grid[i-1][j].mine == true)
            nr++;
        if(valid_position(i-1,j+1) && bomb_grid[i-1][j+1].mine == true)
            nr++;
        if(valid_position(i+1,j-1) && bomb_grid[i+1][j-1].mine == true)
            nr++;
        if(valid_position(i+1,j) && bomb_grid[i+1][j].mine == true)
            nr++;
        if(valid_position(i+1,j+1) && bomb_grid[i+1][j+1].mine == true)
            nr++;
        if(valid_position(i,j-1) && bomb_grid[i][j-1].mine == true)
            nr++;
        if(valid_position(i,j+1) && bomb_grid[i][j+1].mine == true)
            nr++;

        if(nr != 0){
          bomb_grid[i][j].mines_num = nr; //scriu nr de bombe adiacente celului curente
        }
      }
    }
  }
}
```
</details>

### *LCD display Functions & Cursor display logic*
<details>

<summary>Click here</summary>

The grid consists of a 10x10 cell matrix. The following function displays the initial board,100 white cells, divided by a black grid.<br/>
The grid function fills a 240x240 px rectangle with the white color.<br/>
The rectangle is then divided by a grid where each cell is 24x24 px.<br/>
The function below is called every time the games starts or the player resumes the game from Pause.
```C
void grid(){
  tft.fillRect(0,0,240,240,ILI9341_WHITE);
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLUMNS; j++){
      tft.drawRect(i * 24, j * 24, 24, 24, ILI9341_BLACK);
    }
  }
}
```
These variables are used to keep the position of the cursor on the board.
```C
uint8_t prev_cursor_x = 0;
uint8_t prev_cursor_y = 0;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
```
The cell that the cursor points to has **RED** margins instead of black ones.<br/>
When the cursor is moved to a new position, the old position of the cursor is erased.<br/>
The margins of the cell that the cursor previously pointed at become black again.
```C
void draw_cursor(){
  tft.drawRect(prev_cursor_x * 24, prev_cursor_y * 24, 24, 24, ILI9341_BLACK); //face marginile vechii casete negre
  tft.drawRect(cursor_x * 24, cursor_y * 24, 24, 24, ILI9341_RED); //face marginile noii casete selectate rosii
}
```
Every time a cell's state is changed: the player opens, or puts a flag on the cell; the cell_draw function is called for that specific cell. <br/>
This function displays every cell based on the state of it's attributes:
- if the cell is opend the number of neighbouring mines will be displayed (if there are none, the cell will be black)
- if the cell is flagged, it will have a flag symbol drawn on it
- if the player opens a flagged cell, the cell will lose it's flag but not open
- if the player opens a cell that has a mine, the cell will be black with a red 'X' in the middle.
```C
void cell_draw(uint8_t pos_x, uint8_t pos_y, struct cell block){
  /*
  * functie care afiseaza fiecare celula in functie de proprietatile acesteia
  *daca celula nu a fost deschisa se afiseaza blanc
  *daca player-ul a considerat ca este o bomba si a pus un flag pe acea celula
  *se va afisa un '!'
  *daca celula a fost deschisa, iar acolo nu se afla nici o mina, celula va fii neagra, daca nu se invecineaza cu vreo mina,
  *altfel va afisa nr de mine adiacente 
  *daca player-ul a deschis celula, iar acolo se afla o mina se va afisa 'X'
  */
  
  if(block.open == false && block.flaged == false){
    //daca celula nu este nici deschisa si nu are nici flag: se lasa alb
    tft.fillRect(pos_x * 24 + 1, pos_y * 24 + 1, 22, 22, ILI9341_WHITE);
  }
  
  if(block.open == false && block.flaged == true){
    //daca player_ul a apus un flag pe acea celula
    tft.fillRect(pos_x * 24 + 1, pos_y * 24 + 1, 22, 22, ILI9341_WHITE);
    tft.setCursor(24 * pos_x + 1, 24 * pos_y + 1);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_BLACK, ILI9341_RED);
    tft.print((char)33);
  }

  if(block.open == true && block.mine == false){ //daca player a deschis o celula care nu are mina
    if(block.mines_num == 0){ //daca nu are mine adiacente
      tft.fillRect(pos_x * 24 + 1, pos_y * 24 + 1, 22, 22, ILI9341_BLACK);
    }
    else{
      tft.fillRect(pos_x * 24 + 1, pos_y * 24 + 1, 22, 22, ILI9341_BLACK);
      tft.setCursor(24 * pos_x + 1, 24 * pos_y + 1);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.print(String(block.mines_num));
    }
  }

  if(block.open == true && block.mine == true){ //daca player a deschis o celula care avea o mina
    tft.fillRect(pos_x * 24 + 1, pos_y * 24 + 1, 22, 22, ILI9341_BLACK);
    tft.setCursor(24 * pos_x + 1, 24 * pos_y + 1);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print((char)88);
  }
}
```

</details>

### *Pushbuttons*

<details>

<summary>Click here</summary>

All three pushbuttons were debounced using **intterupts**.<br/>
When pressed, each of the 3 buttons change it's coresponding variable's value to 1.<br/>
The **menu()** and **game_logic()** functions of the program use these variables to check is any of the 3 buttons was pressed.
```C
uint8_t joystick_btn = 0; // valoarea butonului de start 
uint8_t open_button = 0; //stocheaza valoare butonului open
uint8_t flag_button = 0; //stocheaza valoarea butonului flag
```
Here is the ISR function for the Joystick Button.The debounce time usde is 200 ms.<br/>
The other 2 buttons' ISR functions follow the same pattern and are debounced in tha same way.
```C
//variabile pentru debouncing butoane in intreruperi
volatile unsigned long last_interrupt_time_start_btn = 0;
volatile unsigned long interrupt_time_start_btn;

//ISR pentru apasarea butonului de pe joystick
void IRAM_ATTR ISR_START() { // se trimite o intrerupere de fiecare data cand este apasat butonul de start
  // Check status of switch
  // Toggle on variable if button pressed
  
  interrupt_time_start_btn = millis();//retine momentul la care s-a produs intreruperea curenta

  //Daca intreruperile vin mai repede de 200 de ms inseamna ca a fost zgomot electric
  if(interrupt_time_start_btn - last_interrupt_time_start_btn > 200){
    //daca butonul a fost apasat
    if (digitalRead(SW_PIN) == LOW) {
      // Switch was pressed
      // schimba starea variabilei joystick_btn
      joystick_btn = 1;
    }
  }

  last_interrupt_time_start_btn = interrupt_time_start_btn;
  
}
```
Each ISR function is atached to it's specific pins.
```C
  pinMode(SW_PIN, INPUT_PULLUP); //pushbutton-ul de pe joystick
  attachInterrupt(digitalPinToInterrupt(SW_PIN), ISR_START, FALLING); //se trimite o intrerupere de fiecare data cand este apasat butonul de start

  pinMode(OPEN_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(OPEN_BTN), ISR_OPEN, FALLING);

  pinMode(FLAG_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLAG_BTN), ISR_FLAG, FALLING);
```
This is an example of how the game_logic function uses the **flag button**. If the button was pressed during the game, an intterupt is send, which changes the value of **flag_button** to 1.<br/>
The game_logic checks if that variable is 1 and if that is the case it puts 0 in the flag_button variable, again. Then, if the button was pressed, the currently selected cell is flagged. The flag attribute of that cell becomes **true** and the **cell_draw** function is called for that cell. 
```C
if(flag_button == 1){ //daca s-a apasat butonul de flag
    flag_button = 0;

    if(start_time == 0){ //cronometrarea timpului incepe la prima apasare a unui buton
      start_time = millis();
    }

    bomb_grid[cursor_x][cursor_y].flaged = true; //se pune flag-ul pe acea celula

    cell_draw(cursor_x, cursor_y, bomb_grid[cursor_x][cursor_y]);

    if(bomb_grid[cursor_x][cursor_y].mine == true){ //daca chiar se afla o bomba acolo
      discoverd_bombs++; //incerementez variabila discoverd_bombs
    }
  }
```

</details>

### *Joystick Movement*

<details>

<summary>Click here</summary>

The joystick generates analog signals based on its position along the X and Y axes. These signals are converted into digital values (ranging from 0 to 4095) using the ESP32's ADC (Analog-to-Digital Converter) pins. The thresholds (LEFT_THRESHOLD, RIGHT_THRESHOLD, UP_THRESHOLD, and DOWN_THRESHOLD) are used to interpret these values and determine the joystick's direction.<br/>
**Key Steps in Joystick Logic**<br/>
1.Reading Joystick Values<br/>
The joystick's X and Y positions are read using analogRead:
```C
value_x = analogRead(VRX_PIN);
value_y = analogRead(VRY_PIN);
```
These values represent the position of the joystick along the X and Y axes. The neutral (centered) position usually gives values near the middle of the range (around 2048).<br/>
2.Threshold Comparison<br/>
The program compares value_x and value_y to predefined thresholds:<br/>

- If value_x < LEFT_THRESHOLD, the joystick is pushed left.
- If value_x > RIGHT_THRESHOLD, the joystick is pushed right.
- If value_y < UP_THRESHOLD, the joystick is pushed up.
- If value_y > DOWN_THRESHOLD, the joystick is pushed down.
<br/>
3.Command Assignment<br/>
Based on the thresholds, the program assigns specific commands using bitwise OR operations:

```C
if (value_x < LEFT_THRESHOLD){
      command = command | COMMAND_LEFT;
    }else if (value_x > RIGHT_THRESHOLD){
      command = command | COMMAND_RIGHT;
    }

    // check up/down commands
    if (value_y < UP_THRESHOLD){
      command = command | COMMAND_UP;
    }else if (value_y > DOWN_THRESHOLD){
      command = command | COMMAND_DOWN;
    }
```
Here is the whole joystick logic:
```C
//treshold-uri pentru mutarea pozitiei la stanga, la dreapta, sus si jos cu joystick-ul
#define LEFT_THRESHOLD  1000 
#define RIGHT_THRESHOLD 3000
#define UP_THRESHOLD    1000
#define DOWN_THRESHOLD  3000

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

#define JOYSTICK_READ_PERIOD 150 //valorile de pe joystick se citesc la o anumita perioada


int value_x = 0; // to store the X-axis value
int value_y = 0; // to store the Y-axis value

uint8_t command = COMMAND_NO; //comanda venita de la joystick

unsigned long last_joystick_read = 0;
unsigned long joystick_read;

void joystick_logic(){

  joystick_read = millis();
  command = COMMAND_NO;

  if(joystick_read - last_joystick_read > JOYSTICK_READ_PERIOD){
    value_x = analogRead(VRX_PIN);
    value_y = analogRead(VRY_PIN);
   

    // Serial.print("Value X: ");
    // Serial.print(value_x);
    // Serial.print(" | Value Y: ");
    // Serial.println(value_y);

    // check left/right commands
    if (value_x < LEFT_THRESHOLD){
      command = command | COMMAND_LEFT;
    }else if (value_x > RIGHT_THRESHOLD){
      command = command | COMMAND_RIGHT;
    }

    // check up/down commands
    if (value_y < UP_THRESHOLD){
      command = command | COMMAND_UP;
    }else if (value_y > DOWN_THRESHOLD){
      command = command | COMMAND_DOWN;
    }
    
    
    last_joystick_read = joystick_read;
  }
}
```

</details>

## Obtained Results
## Conclusions
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
