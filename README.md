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

This projects uses 3 **Pushbuttons**:
- Open: for opening a cell that the player belives is emplty;
- Flag: for flagging a cell the player think contains a bomb;
- Joystick button: used in the **Menu interface and Pause interfeca**;

All three pushbuttons were debounced using **intterupts**.<br/>
When pressed, each of the 3 buttons change their coresponding variable's value to 1.<br/>
The **menu()** and **game_logic()** functions of the program use these variables to check is any of the 3 buttons was pressed.
```C
uint8_t joystick_btn = 0; // valoarea butonului de start 
uint8_t open_button = 0; //stocheaza valoare butonului open
uint8_t flag_button = 0; //stocheaza valoarea butonului flag
```
Here is the ISR function for the Joystick Button.The debounce time usde is 200 ms.<br/>
When the button is pressed, if 200ms passed from the last intterupt, meaning there was no electrical noise, then the ISR function changes the **joystic_btn** variable's value to 1.<br/>
The other 2 buttons' ISR functions follow the same pattern and are debounced in the same way.
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

### *Joystick usage example*
<details>
<summary>Click here</summary>
  
This code is part of the menu. When the player first starts the game he will be displayed the Main Menu.<br/>
He will have to chose from 2 options: 1.Start; 2.Top 10.<br/>
The player will use the joystick's movement to select an option. A red dot will visually indicate which option the player is currentlly selecting. The red dot selector will move **UP** and **DOWN** based on the command given by the joystick. When the joystock button is pressed, the option selected will be the one that has the red dot pointing to it. The Main Menu option selection runs in a loop. The loop ends when an the player choses an option (when the joysstick button is pressed). 
```C
 tft.fillRect(0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT,ILI9341_BLUE);
    tft.setCursor(70, 60);
    tft.setTextSize(3);
    tft.print("Minesweeper");

    tft.setCursor(49, 100);
    tft.setTextSize(2);
    tft.print("Apasa butonul pentru:");
    tft.setCursor(120, 140);
    tft.print("START");
    tft.setCursor(115, 170);
    tft.print("Top 10");
    tft.fillCircle(100, 145, 10, ILI9341_RED);

    //vom avea de ales intrea a incepe jocul si a vizualiza cele mai mairi 10 scoruri

    while(joystick_btn == 0){ //mentine meniul afisat pana se apasa o optiune de start joc sau top 10 times
      joystick_logic(); //interogam comenzile de la joystick
      switch(command){
        case COMMAND_DOWN:
          if(start_menu_option == 1){
            start_menu_option = 2;
          }
          else if(start_menu_option == 2){
            start_menu_option = 1;
          }
          break;
        case COMMAND_UP:
          if(start_menu_option == 1){
            start_menu_option = 2;
          }
          else if(start_menu_option == 2){
            start_menu_option = 1;
          }
          break;
        default:
          break;
      }

      tft.fillCircle(100, 145, 10, ILI9341_BLUE);
      tft.fillCircle(100, 175, 10, ILI9341_BLUE);

      if(start_menu_option == 1){
        tft.fillCircle(100, 145, 10, ILI9341_RED);
      }
      else if(start_menu_option == 2){
        tft.fillCircle(100, 175, 10, ILI9341_RED);
      }

      delay(50);
    }
```
</details>

### Function: game_logic()
The most important function is **game_logic()**. It constantly reads the values ​​given by the joystick, to update the cursor position. The cursor position identifies the cell on which we are acting. Then, this function checks the variables corresponding to the 3 buttons: open, flag and joystick and updates the state of the game and the screen depending on the player's input.

### Libraries
I used the following libraries for my project:
- [Adafruit_ILI9341](https://github.com/adafruit/Adafruit_ILI9341): This library is used to interface with displays based on the ILI9341 driver, like the one I used in my project.It simplifies the initialization and control of the screen, including displaying text, geometric shapes, and images. It ensures compatibility and optimized performance by using SPI for fast communication.;
- [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library): This is the core graphics library for all Adafruit displays, providing a common set of graphics primitives (points, lines, circles, etc.). It needs to be paired with a hardware-specific library like Adafuit_ILI9341 for each display device we carry (to handle the lower-level functions). For this library to work we also need Adafruit Bus IO Library;
- [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO): This is a helper library to abstract away I2C & SPI transactions and registers. It is useful for abstracting and simplifying interactions with hardware registers, reducing errors and making it easier to read and write data.;
- [SD](https://github.com/arduino-libraries/SD): The SD library allows for reading from and writing to SD cards. The SD library is used to access SD cards connected to the microcontroller via SPI. It is essential for managing files (e.g., images, data, settings) stored on SD cards.It allows easy reading and writing of data.;
- SPI: Serial Peripheral Interface is the primary protocol used for fast communication with ILI9341 and SD modules.
The standard Arduino SPI library provides an efficient implementation, reducing the complexity of developing a custom solution.;

### Laboratory functionalities and their usage in my project
My project uses the following functionalities, that were presented during the laboratories:
1. **SPI**: used for comunicating with both the TFT LCD ILI_9341 and the MicroSD Card. The project uses the VSPI interface of the microcontroller. The LCD and SD card share the MOSI, MISO and CLK lines while having separate CS lines. The following code snippets shows the TFT's SPI initialization and SD card initialization. The Adafruit_ILI9341 library simplifies the initialization process.
```C
// Pin definitions for the ESP32
#define TFT_CS    5      // Chip Select (CS)
#define TFT_DC    2      // Data/Command (DC) - GPIO2
#define TFT_RST   4      // Reset (RST) - GPIO4
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_CLK  18

#define SD_CS    16

// Create an instance of the display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
--------------------------------------------------------------------------
void setup(){
  tft.begin(); //Initialize the TFT
  tft.setRotation(3);
  tft.fillScreen(ILI9341_RED);
  tft.fillCircle(60, 105, 10, ILI9341_BLUE);
  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");
}
```
2. **Intterupts**: as I have already shown in the **Software design - Pushbuttons** section, all three pushbuttons that the project uses, are debounced using intterupts.
3. **ADC**: as I have already shown in the **Software design - Joystick Movement** section, the joystick makes use of the ADC function of the ESP32 through the **analogRead()** function.

## Obtained Results
The result of my implementation is a fully functional game, one that is very similar to the original game!!!<br/>
Here are some images of the setup:<br/>
<img src = "https://github.com/user-attachments/assets/d8d797e9-4d70-4e74-b86b-c28ca5fd85e7" width =680 height = 510>
<img src = "https://github.com/user-attachments/assets/8289a4b9-a2df-4246-9fee-4290f26c063a" width =680 height = 510>
<img src = "https://github.com/user-attachments/assets/0015ab13-fc40-44b2-973b-893f68b3db07"  width =680 height = 510>
<br/>And here are some images from different stages in the game:<br/>
<img src = "https://github.com/user-attachments/assets/36e432c9-e0d3-446b-887c-483bd38537a3" width = 300 height = 400>
<img src = "https://github.com/user-attachments/assets/1a5aaf5c-fb91-4b18-b858-ef0cbf28722f" width = 300 height = 400>
![Game_3](https://github.com/user-attachments/assets/7be293d4-241d-4214-9ac5-6e9a5414c018)
![Game_4](https://github.com/user-attachments/assets/855bca9d-6921-4ca5-90c5-e01bd7d0ce75)
![Game_5](https://github.com/user-attachments/assets/e8353232-a9f4-4332-abf3-18d2cc006b48)
![Game_6](https://github.com/user-attachments/assets/3d544157-03d0-42bc-bea0-03aed2131659)
![Game_7](https://github.com/user-attachments/assets/94d1b390-0384-4c11-a4ea-4d7441deba5e)

## Conclusions
### Final Conclusion
Implementing this prject was both fun and difficult. 
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
