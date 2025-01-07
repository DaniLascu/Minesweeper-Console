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
### <ins>Algorithms and Important Code Parts</ins>
### Board Generation and fill Algorithm
Every Cell of the board is in fact a variable of the Struct type shown below. The struct has 4 attributes, that store the following: 
- if the cell contains a mine;
- number of neighbour mines;
- if the cell is flagged;
- if the cell is open;<br/>
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
### LCD display Functions & Cursor display logic
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
This function displays every cell base on the state of it's attributes:
- if the cell is opend the number of neighbouring mines will be displayed (if there are none, the cell will be black)
- if the cell is flagged, it will have a flag symbol drawn on it
- if the player opens a flagged cell, the cell will lose it's flag but not open
- if the player opens a cell that has a mine, the cell will be black with a red 'X' in the middle.<br/>
Every time a cell's state is changed: the player opens, or puts a flag on the cell; the cell_draw function is called for that specific cell. 
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
### Pushbuttons
All three pushbuttons were debounced using **intterupts**.<br/>
When pressed, each of the 3 buttons change it's coresponding variable's value to 1.<br/>
The **menu()** and **start_game()** functions of the program use these variables to check is any of the 3 buttons was pressed.
```C
uint8_t joystick_btn = 0; // valoarea butonului de start 
uint8_t open_button = 0; //stocheaza valoare butonului open
uint8_t flag_button = 0; //stocheaza valoarea butonului flag
```
The debounce time is 200 ms.
```C
//variabile pentru debouncing butoane in intreruperi
volatile unsigned long last_interrupt_time_start_btn = 0;
volatile unsigned long interrupt_time_start_btn;

volatile unsigned long last_interrupt_time_open_btn = 0;
volatile unsigned long interrupt_time_open_btn;

volatile unsigned long last_interrupt_time_flag_btn = 0;
volatile unsigned long interrupt_time_flag_btn = 0;

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

//ISR pentru apasarea butonului open
void IRAM_ATTR ISR_OPEN() { // se trimite o intrerupere de fiecare data cand este apasat butonul de start
  // Check status of switch
  // Toggle on variable if button pressed
  
  interrupt_time_open_btn = millis();//retine momentul la care s-a produs intreruperea curenta

  //Daca intreruperile vin mai repede de 200 de ms inseamna ca a fost zgomot electric
  if(interrupt_time_open_btn - last_interrupt_time_open_btn > 200){
    //daca butonul a fost apasat
    if (digitalRead(OPEN_BTN) == LOW) {
      // Switch was pressed
      // schimba starea variabilei open_button
      open_button = 1;
    }
  }

  last_interrupt_time_open_btn = interrupt_time_open_btn;
  
}

//ISR pentru apasarea butonului flag
void IRAM_ATTR ISR_FLAG() { // se trimite o intrerupere de fiecare data cand este apasat butonul de start
  // Check status of switch
  // Toggle on variable if button pressed
  
  interrupt_time_flag_btn = millis();//retine momentul la care s-a produs intreruperea curenta

  //Daca intreruperile vin mai repede de 200 de ms inseamna ca a fost zgomot electric
  if(interrupt_time_flag_btn - last_interrupt_time_flag_btn > 200){
    //daca butonul a fost apasat
    if (digitalRead(FLAG_BTN) == LOW) {
      // Switch was pressed
      // schimba starea variabilei flag_button
      flag_button = 1;
    }
  }

  last_interrupt_time_flag_btn = interrupt_time_flag_btn;
  
}
```
The intterupts are attached to their specific pins.
```C
  pinMode(SW_PIN, INPUT_PULLUP); //pushbutton-ul de pe joystick
  attachInterrupt(digitalPinToInterrupt(SW_PIN), ISR_START, FALLING); //se trimite o intrerupere de fiecare data cand este apasat butonul de start

  pinMode(OPEN_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(OPEN_BTN), ISR_OPEN, FALLING);

  pinMode(FLAG_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLAG_BTN), ISR_FLAG, FALLING);
```
### Joystick Movement
The joystick has 4 **COMMANDS**:
- Up
- Down
- Left
- Right
<br/>
The code snippet defines a set of threshold values and commands to interpret joystick movements based on ADC (Analog-to-Digital Converter) readings.<br/>
Thresholds are numeric boundaries used to determine the direction of joystick movement based on ADC readings.<br/>
The commands are symbolic constants used to represent joystick movements in code. Each command is assigned a unique hexadecimal value.<br/>
The joystick's position is captured by reading values from the ADC pin.
The ADC value is compared to the defined thresholds (LEFT_THRESHOLD, RIGHT_THRESHOLD, UP_THRESHOLD, and DOWN_THRESHOLD).
The command are then used in the game logic.

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
```

This is the function that is tasked with determining the appropriate joystick command.
```C
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
<!--This code snippet shows how the joystick is used to chose a difficulty option.<br/>
Depending on the command provided by the joystick, the TFT LCD displays a **selector** by the shape of a red dor near the currently selected option.<br/>
When the joystick is pressed the difficulty has been chosen.<br/>
The red circle is a clear visual cue to the player, showing which difficulty option is currently selected.
The use of the joystick to cycle through options is intuitive and allows for smooth interaction with the menu.
The code waits for a button press to start, then uses the joystick to cycle through difficulty options. The selected difficulty is confirmed with the joystick button press, and the game proceeds to the next stage. The display and user interaction are well-managed to ensure an easy and engaging experience.-->
This code snippet demonstrates how the joystick is used to select a difficulty level in the game. The user interacts with the menu by moving the joystick up and down to navigate between difficulty options ("Easy", "Medium", "Hard"), and presses the joystick button to confirm their selection. The currently selected option is highlighted by a red circle on the display.

Explanation:

Button Press to Start:

When the joystick button is pressed and the "Start" option (start_menu_option == 1) is selected, the game is launched (game_started = 1).
The screen is cleared (tft.fillRect) and a message asking the player to choose a difficulty is displayed. The options "Easy", "Medium", and "Hard" are shown at specific coordinates on the LCD.
Difficulty Selection:

A red circle (tft.fillCircle) acts as a selector, indicating the current difficulty option.
The program enters a while loop that keeps running until a difficulty is selected (difficulty == 0 means not selected yet).
Inside the loop, the joystick's logic (joystick_logic()) continuously checks for commands (up, down, or button press).
Joystick Movement:

If the joystick is moved down (COMMAND_DOWN), the cursor moves to the next option. If it's at the last option, it wraps around to the first one.
Similarly, moving the joystick up (COMMAND_UP) moves the cursor in reverse, wrapping around if necessary.
Updating the Selector:

As the user moves the joystick, the red circle follows the cursor to show the current selection.
All previous selectors (blue circles) are erased to avoid overlapping (tft.fillCircle(60, menu_cursor[0], 10, ILI9341_BLUE)), leaving only the red circle to show the active choice.
Joystick Button Press:

When the joystick button is pressed (joystick_btn), the selection is confirmed.
Depending on the position of the cursor (menu_joystick_cursor), the corresponding difficulty level is set (difficulty = MINES_EASY, MINES_MEDIUM, or MINES_HARD).
Starting the Game:

Once a difficulty is chosen, the program exits the selection loop (difficulty != 0), and the game initialization (start_game()) begins.

```C
 if(joystick_btn == 1 && start_menu_option == 1){ //daca butonul de pe joystick a fost apasat si optiunea = 1(START) dau drumul la joc
      game_started = 1; 
      joystick_btn = 0; //fac din nou valoarea joystick_btn 0, pentru a putea verifica din nou daca 
                        //butonul a fost apasat, deoarece a fost facuta 1 in intrerupere

      tft.fillRect(0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT,ILI9341_BLUE); //jucatorul este rugat sa aleaga dificultatea
      tft.setCursor(70, 60);
      tft.setTextSize(2);
      tft.print("Dificultate");

      tft.setCursor(80, 100);
      tft.print("Usor");
      tft.setCursor(80, 120);
      tft.print("Mediu");
      tft.setCursor(80, 140);
      tft.print("Greu");

      tft.fillCircle(60, menu_cursor[menu_joystick_cursor], 10, ILI9341_RED); //selectorul este un cerc rosu

      while(difficulty == 0){ //se selecteaza dificultatea cu ajutorul joystick-ului
        //cat timp difficulty = 0 inseamna ca nu s-a selectat inca dificultatea. se asteapta pana este selectata
        joystick_logic(); //se citeste in continuu comanda data de joystick

        if(command == COMMAND_DOWN){ //se schimba cursorul de pe ecran in functie de joystick
          if(menu_joystick_cursor == 2){
            menu_joystick_cursor = 0;
          }
          else{
            menu_joystick_cursor ++; 
          }
        }

        if(command == COMMAND_UP){
          if(menu_joystick_cursor == 0){
            menu_joystick_cursor = 2;
          }
          else{
            menu_joystick_cursor --; 
          }
        }
        tft.fillCircle(60, menu_cursor[0], 10, ILI9341_BLUE);
        tft.fillCircle(60, menu_cursor[1], 10, ILI9341_BLUE);
        tft.fillCircle(60, menu_cursor[2], 10, ILI9341_BLUE);
        tft.fillCircle(60, menu_cursor[menu_joystick_cursor], 10, ILI9341_RED);
        delay(50); //se afiseaza pe ecran selectorul la dificultatea aleasa cu ajutorul joystick-ului

        if(joystick_btn){ //daca a fost apasat butonul de pe joystick, s-a alaes dificultatea selectata
          joystick_btn = 0; //redevine 0 altfel la inceperea jocului intre in meniul de pauza
          switch(menu_joystick_cursor){ //in functie de dificulattea selectata, variabila difficulty va stoca nr de mine corespunzator
            case 0:
              difficulty = MINES_EASY;
              break;
            case 1:
              difficulty = MINES_MEDIUM;
              break;
            case 2:
              difficulty = MINES_HARD;
              break;
            default:
              break;
          }
        }
      }
    //dupa ce s-a ales dificultatea generez animatia de start a jocului si construiesc grid-ul
      if(game_started == 1){
        start_game();
      }
    }
```
## Obtained Results
## Conclusions
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
