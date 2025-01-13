#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BusIO_Register.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

// Pin definitions for the ESP32
#define TFT_CS    5      // Chip Select (CS)
#define TFT_DC    2      // Data/Command (DC) - GPIO2
#define TFT_RST   4      // Reset (RST) - GPIO4
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_CLK  18

//Chip Select for SD card
#define SD_CS    16
#define SPEAKER_PIN 25 //DAC output pin for audio

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240
#define TFT_WIDTH  320
#define TFT_HEIGHT 240

// Create an instance of the display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);


//-----------------------------------Joystick si butoane------------------------------------------
#define VRX_PIN  39 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define VRY_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRY pin
#define SW_PIN   32 // ESP32 pin GPIO32 connected to SW  pin; butonul de start stop 

#define OPEN_BTN 27 //butonul care deschide o celula 
#define FLAG_BTN 33 //butonul care pune flag pe o celula

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
uint8_t joystick_btn = 0; // valoarea butonului de start 
uint8_t open_button = 0; //stocheaza valoare butonului open
uint8_t flag_button = 0; //stocheaza valoarea butonului flag

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

//----------------------------Logica Grid Mine----------------------------
#define ROWS 10
#define COLUMNS 10

#define MINES_EASY 8
#define MINES_MEDIUM 10
#define MINES_HARD 15

// struct ce reprezinta o celula din grid
struct cell{
  bool mine; //1 daca o bomba se afla in celula; 0 daca nu 
  uint8_t mines_num; //nr de bombe adiacente
  bool flaged; //1 daca player-ul a marcat celula ca bomba; 0 altfel
  bool open; //1 daca player-ul a deschis celula considerand ca nu exista bombe; 0 daca celula esta inca inchisa
};

struct cell bomb_grid[ROWS][COLUMNS];



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

// void open_adjacent_cells(int poz_i, int poz_j){
//   if()
// }


//----------------------------- LCD functions & Game Logic-------------------------------------

uint8_t game_started = 0; //variabila care mentine starea jocului. 0 daca nu a inceput is 1 daca a inceput

uint8_t difficulty = 0; //variabila in care se vor stoca nr de mine in functie de dificultatea aleasa

/*pozitia selectorului controlat de joystick pe ecran, pentru a alege dificultatea: 0 - easy; 1 - medium; 2 - hard*/
uint8_t menu_joystick_cursor = 0; 

uint8_t menu_cursor[3] = {105, 125, 145}; //liniile la care va aparea selectorul pe ecran: easy - medium - hard

//poitia curenta si anterioara a cursorului
uint8_t prev_cursor_x = 0;
uint8_t prev_cursor_y = 0;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

void grid(){
  tft.fillRect(0,0,240,240,ILI9341_WHITE);
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLUMNS; j++){
      tft.drawRect(i * 24, j * 24, 24, 24, ILI9341_BLACK);
    }
  }
}

void draw_cursor(){
  tft.drawRect(prev_cursor_x * 24, prev_cursor_y * 24, 24, 24, ILI9341_BLACK); //face marginile vechii casete negre
  tft.drawRect(cursor_x * 24, cursor_y * 24, 24, 24, ILI9341_RED); //face marginile noii casete selectate rosii
}

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


void open_adjacent_block(uint8_t pos_x, uint8_t pos_y, struct cell *block){
  /*
  * function that is run when a block is opend
  * The program will run this function for the 8 adjacent blocks to the block just opend.
  * this function will try to open the adjacent block that was given as input and will
  * manage it only the block is not already opend, is not flagged or does not have a bomb in it.
  * The function will check first if the block we want to open is in the bounds of the grid
  */
  int random_number = random(1,3);
  if(valid_position(pos_x, pos_y) && block->flaged == false && block->open == false && block->mine == false && random_number == 1){ 
    /*
    * if the block is in the boundries of the grid
    * and is not flagged and is not previously opend and doesn't have a mine in it
    * we will first open that block
    * so the cell_draw function can display the block properlly
    * I also added some randomnes, because i dont want all the 8 block to always open
    */
    block->open = true;
    cell_draw(pos_x, pos_y, bomb_grid[pos_x][pos_y]);
  }
}

void timer_display(unsigned long start_time){
  /*
  * functie care afiseaza timpul scurs de la inceputul jocului la orice moment
  */

  tft.fillRect(240, 0, 80, 240, ILI9341_LIGHTGREY);

  unsigned long time_ms = millis() - start_time;

  unsigned long time_sec = time_ms / 1000;
  int seconds = time_sec % 60;
  int minutes = time_sec / 60;

  if(start_time != 0){ 
    //daca s-a apasat un buton i.e. start_time != 0,  incepe cronometrarea
    //altfel nu se afiseaza cronometrul pana la apasarea unui buton
    tft.setCursor(260, 50);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_BLACK);
    tft.print("Min:");
    tft.setCursor(270, 80);
    if(minutes < 10){
      tft.print("0");
      tft.print(minutes);
    }
    else{
      tft.print(minutes);
    }

    tft.setCursor(260, 120);
    tft.print("Sec:");
    tft.setCursor(270, 150);
    if(seconds < 10){
      tft.print("0");
      tft.print(seconds);
    }
    else{
      tft.print(seconds);
    }
  }
}


//--------------------------- SD card Functions------------------

/*
* Daca un player va avea un timp de rezolvare al jocului in top 10 timpi,
* acesta va fi rugat sa introduca un nume de 3 litere.
* Cei mai buni 10 timpi de rezolvare vor fi satlavi in fisierul times.txt
* de pe cardul SD, in formatul: ABC,10000000
*/

char new_name[3]; //variabila in care se stocheaza numele introdus

struct Record {
  char name[4];  // 3 letters + null terminator
  int time;      // Solve time in milliseconds
};

Record records[10] = {{"---", 1200000}, {"---", 1200000}, {"---", 1200000}, {"---", 1200000}, {"---", 1200000},
                      {"---", 1200000}, {"---", 1200000}, {"---", 1200000}, {"---", 1200000}, {"---", 1200000}};


void top_ten_name_request(){
  /*
  *Functie prin care jucatorul isi alege numele la sfarsitul jocului,
  *daca a obtinut un timp in top 10
  */

  tft.fillScreen(ILI9341_BLUE);
  tft.setCursor(40, 40);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Ai obtinut un scor in");
  tft.setCursor(100, 70);
  tft.setTextSize(3);
  tft.print("TOP 10!!");

  tft.setCursor(85, 110);
  tft.setTextSize(2);
  tft.print("Alege un nume:");

  char a = 'A';
  char b = 'A';
  char c = 'A';

  tft.setCursor(130, 160);
  tft.setTextSize(3);
  tft.print(a);
  tft.print(b);
  tft.print(c);

  int i = 0; //indice care indica a cata litera din nume se alege

  while(i < 3){ //cat timp nu s-a ales numele
    joystick_logic();

    if(i == 0){ //prima litera
      if(joystick_btn == 1){
        // daca s-a ales o litera se trece la urmatoarea 
        joystick_btn = 0;
        i++;
      }

      switch(command){
        case COMMAND_DOWN:
          if(a > 'A'){
            a--;
          }
          else if(a == 'A'){
            a = 'Z';
          }
          break;
        
        case COMMAND_UP:
          if(a == 'Z'){
            a = 'A';
          }
          else if(a < 'Z'){
            a++;
          }
          break;

        default:
          break;
      }
    }
    else if(i == 1){
      if(joystick_btn == 1){
        // daca s-a ales o litera se trece la urmatoarea 
        joystick_btn = 0;
        i++;
      }

      switch(command){
        case COMMAND_DOWN:
          if(b > 'A'){
            b--;
          }
          else if(b == 'A'){
            b = 'Z';
          }
          break;
        
        case COMMAND_UP:
          if(b == 'Z'){
            b = 'A';
          }
          else if(b < 'Z'){
            b++;
          }
          break;

        default:
          break;
      }
    }
    else if(i == 2){
      if(joystick_btn == 1){
        // daca s-a ales o litera se trece la urmatoarea 
        joystick_btn = 0;
        i++;
      }

      switch(command){
        case COMMAND_DOWN:
          if(c > 'A'){
            c--;
          }
          else if(c == 'A'){
            c = 'Z';
          }
          break;
        
        case COMMAND_UP:
          if(c == 'Z'){
            c = 'A';
          }
          else if(c < 'Z'){
            c++;
          }
          break;

        default:
          break;
      }
    }

    tft.setCursor(130, 160);
    tft.fillRect(130, 160, 150, 180, ILI9341_BLUE);
    tft.print(a);
    tft.print(b);
    tft.print(c);

    delay(50);
  }

  new_name[0] = a;
  new_name[1] = b;
  new_name[2] = c;
}


void read_solve_times() {
  File file = SD.open("/times.txt");
  
  if (file) {
    int index = 0;
    while (file.available() && index < 10) {
      String line = file.readStringUntil('\n');
      int comma_index = line.indexOf(','); // pozitia in care se afla virgula in linie
      if (comma_index != -1) {
        String name = line.substring(0, comma_index); //ce este pana in virgula e numele
        int time = line.substring(comma_index + 1).toInt(); //ce este dupa este timpul, se converteste timpul la int
        
        strncpy(records[index].name, name.c_str(), 3);  // Copiere nume in records
        records[index].name[3] = '\0';                 // Se adauga '\0'
        records[index].time = time;                    // Se salveaza si timpul
        index++;
      }
    }
    file.close();
  } else {
    Serial.println("No previous records file found. Creating new file.");
  }
}


void save_solve_time(const char* new_name, int new_time) {
  // Insereaza noua inregistrare la pozitia corecta in records
  for (int i = 0; i < 10; i++) {
    if (new_time < records[i].time) {
      // Shift down inregistrarile ramase
      for (int j = 9; j > i; j--) {
        records[j] = records[j-1];
      }
      // Inseram noua inregistrare in records
      strncpy(records[i].name, new_name, 3);
      records[i].name[3] = '\0'; // Null-terminator
      records[i].time = new_time;
      break;
    }
  }

  //Salvare timpi updatati pe cardul SD
  File file = SD.open("/times.txt", FILE_WRITE);
  if (file) {
    for (int i = 0; i < 10; i++) {
      file.print(records[i].name);
      file.print(",");
      file.println(records[i].time);
    }
    file.close();
    Serial.println("Top records saved.");
  } else {
    Serial.println("Error saving top records.");
  }
}


void check_top_time(int new_time){
  /*
  *functie care verifica daca scorul este in top 10
  */
  int ok = 0; //0 daca timpul nu e in top 10; 1 daca timpul e in top 10

  read_solve_times(); //citim timpii din fisier

  for (int i = 0; i < 10; i++) {
    if (new_time < records[i].time){ //daca timpul obtinut se afla in top 10
      ok = 1;
      break;
    }
  }

  if(ok == 1){ // daca avem un timp in top 10
    top_ten_name_request(); //cerem palyer-ului sa introduca un nume
    save_solve_time(new_name, new_time);
  }

}


void display_top_times(){ //afisarea primilor 10 timpi
  read_solve_times(); //se citesc timpii

  tft.fillScreen(ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  
  int pos = 1;
  int nr = 1;

  for(int i = 0; i < 10; i++){
    if(records[i].time != 1200000){ //daca nu este un timp default
      int time_sec = records[i].time / 1000;
      int time_min = time_sec / 60;
      time_sec %= 60;

      tft.setCursor(30, pos * 10);
      tft.print(nr);
      tft.print(". ");
      tft.print(records[i].name);
      tft.print(';');
      tft.print(" min: ");
      tft.print(time_min);
      tft.print(" sec: ");
      tft.print(time_sec);

      pos+=2;
      nr++;
    }
  }
}



//---------------------------Game Logic--------------------------

void start_game();

uint8_t start_menu_option = 1; 
/*
* in meniul de start voi avea optiunile:
* 1. de a incepe jocul
* 2. de a vizualiza top 10 timpi de rezolvare ai jocului
*/
void menu(){
  if(game_started == 0){ //daca jocul nu a inceput inca
    //meniul de la inceputul jocului difera de cel din timpul jocului
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
    else if(joystick_btn == 1 && start_menu_option == 2){
      /*
      * daca butonul de pe joystick a fost apasat si optiunea este 2(top 10 times)
      */
      joystick_btn = 0;
      display_top_times();
      while(joystick_btn == 0){ //se afiseaza top 10 times pana se apasa din nou butonul
        delay(1);
      }
      if(joystick_btn == 1){
        joystick_btn = 0;
      }
    }
  }
}

void start_animation(){ //animatie de incepere a jocului
  /*
  *Numara in sens invers de la 3 la 1
  *apoi afiseaza un mesaj de start
  */
  tft.fillRect(0,0,320,240, ILI9341_BLUE);
  tft.setTextSize(4);

  tft.setCursor(DISPLAY_WIDTH / 2 - 10, DISPLAY_HEIGHT / 2 - 20);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("3");
  delay(1000);

  tft.setCursor(DISPLAY_WIDTH / 2 - 10, DISPLAY_HEIGHT / 2 - 20);
  tft.fillRect(DISPLAY_WIDTH / 2 - 10, DISPLAY_HEIGHT / 2 - 20, 50, 50, ILI9341_BLUE);
  tft.setTextColor(ILI9341_RED);
  tft.print("2");
  delay(1000);

  tft.setCursor(DISPLAY_WIDTH / 2 - 10, DISPLAY_HEIGHT / 2 - 20);
  tft.fillRect(DISPLAY_WIDTH / 2 - 10, DISPLAY_HEIGHT / 2 - 20, 50, 50, ILI9341_BLUE);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("1");
  delay(1000);

  tft.setCursor(DISPLAY_WIDTH / 2 - 80, DISPLAY_HEIGHT / 2 - 20);
  tft.fillRect(DISPLAY_WIDTH / 2 - 10, DISPLAY_HEIGHT / 2 - 20, 50, 50, ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("START!!!");
  delay(2000);
}

void start_game(){
  /*
  * functie care porneste animatia de inceput construieste grid-ul, genereaza minele si vecinii
  * pune cursorul in pozitia 0 0
  */
  start_animation();
  grid();
  generate_bombs(difficulty);
  calculate_number_of_neighbour_bombs();
  cursor_x = 0;
  cursor_y = 0;
  prev_cursor_x = 0;
  prev_cursor_y = 0;
  draw_cursor();
}


uint8_t discoverd_bombs = 0; //ne de bombe descoperite
//cand nr de bombe descoperite == nr de bombe din difficulty, se incheie jocul

void end_animation(uint8_t minutes, uint8_t seconds){
  /*
  *sunt 2 animatii pentru sfarsitul jocului
  *1. Daca s-au gasit toate minele se va afisa timpul de rezolvare al jocukui
  *2. Daca s-a deschis o celula care continea o mina inseamna ca jucatorul a pierdut
  */
  if(discoverd_bombs == difficulty){ //daca s-au gasit toate minele
    tft.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_BLUE);
    tft.setCursor(DISPLAY_WIDTH / 2- 100, DISPLAY_HEIGHT / 2 - 50);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.print("WELL DONE!!!");

    tft.setTextSize(2);
    tft.setCursor(DISPLAY_WIDTH / 2 - 120, DISPLAY_HEIGHT / 2 - 10);
    tft.print("Ai resolvat jocul in:");
    tft.setCursor(DISPLAY_WIDTH / 2 - 115, DISPLAY_HEIGHT / 2 + 20);
    tft.print(minutes);
    tft.print(" minute; ");
    tft.print(seconds);
    tft.print(" secunde");
    delay(5000);
  }
  else{ //daca o mina a explodat
    tft.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_BLUE);
    tft.setCursor(DISPLAY_WIDTH / 2- 70, DISPLAY_HEIGHT / 2 - 50);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.print("SORRY!!!");

    tft.setTextSize(2);
    tft.setCursor(DISPLAY_WIDTH / 2 - 120, DISPLAY_HEIGHT / 2 - 10);
    tft.print("O mina a explodat :((");
    delay(5000);
  }
}

unsigned long start_time = 0;
unsigned long end_time = 0;
unsigned long game_length;
uint8_t game_length_seconds;
uint8_t game_lenght_minutes;

unsigned long wait_start_time = 0; 
unsigned long wait_end_time = 0;
//in timpul jocului se poate intra in meniu
//timpul de stat in meniu trebuie extras din timpul jocului

unsigned long last_timer_refresh = 0; //timer-ul isi da refresh o data la 0.5 sec
unsigned long timer_wait_time = 500; //ms

void game_logic(){
  if(millis() - last_timer_refresh >= timer_wait_time){
    timer_display(start_time); //se actualizeaza constant timer-ul din 0.5 in 0.5 sec
    last_timer_refresh = millis();
  }
  joystick_logic(); //se citeste constant inputul analogic de la joystick

  if(discoverd_bombs == difficulty){ //daca s-au descoperit toate bombele se incheie jocul
    end_time = millis(); //se retine momentul opririi jocului
    game_length = end_time - start_time; //durata jocului

    game_length_seconds = game_length / 1000;
    game_lenght_minutes = game_length_seconds / 60;
    game_length_seconds = game_length_seconds % 60;
    end_animation(game_lenght_minutes, game_length_seconds);

    check_top_time(game_length); //se verifica daca timpul intra in top 10
    game_started = 0; //se opreste jocul
  }

  switch (command){ //logica de miscare a cursorului
    case COMMAND_DOWN:
      if(valid_position(cursor_x, cursor_y + 1)){ //daca se poate muta cursorul in jos
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y; //se aduce prev cursor pe pozitia curenta a cursorului
        cursor_y++; //se schimba pozitia curenta a cursorului

        draw_cursor();
      }
      break;
    
    case COMMAND_UP:
      if(valid_position(cursor_x, cursor_y - 1)){ //daca cursorul se poate muta in sus
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y; //se aduce cursorul anterior in poz curenta
        cursor_y--; //se schimba pozitia curenta a cursorului
        
        draw_cursor();
      }
      break;

    case COMMAND_LEFT:
      if(valid_position(cursor_x - 1, cursor_y)){ //daca cursorul se poate muta in stanga
        prev_cursor_x = cursor_x; //se aduce cursorul anterior in poz curenta
        prev_cursor_y = cursor_y;
        cursor_x--; //se schimba pozitia curenta a cursorului

        draw_cursor();
      }
      break;

    case COMMAND_RIGHT:
      if(valid_position(cursor_x + 1, cursor_y)){ //daca cursorul se poate muta in dreapta
        prev_cursor_x = cursor_x; //se aduce cursorul anterior in poz curenta
        prev_cursor_y = cursor_y;
        cursor_x++; //se schimba pozitia curenta a cursorului

        draw_cursor();
      }
      break;

    default:
      break;
  }

  if(open_button == 1){
    //daca a fost apasat butonul de open, pentru a deschide o celula
    open_button = 0;

    if(start_time == 0){ //la prima apasare a unui buton incepe cronometrul
      start_time = millis();
    }

    if(bomb_grid[cursor_x][cursor_y].mine == false && bomb_grid[cursor_x][cursor_y].flaged == false && bomb_grid[cursor_x][cursor_y].open == false){
      //daca nu se afli nicio mina si celula nu e flagged - si celula nu a mai fost deschisa deja
      //se afiseaza nr de mine invecinate

      bomb_grid[cursor_x][cursor_y].open = true;
      cell_draw(cursor_x, cursor_y, bomb_grid[cursor_x][cursor_y]);

      //new part - open 8 adjacent cells
      open_adjacent_block(cursor_x - 1, cursor_y - 1, &bomb_grid[cursor_x - 1][cursor_y - 1]);
      open_adjacent_block(cursor_x - 1, cursor_y, &bomb_grid[cursor_x - 1][cursor_y]);
      open_adjacent_block(cursor_x - 1, cursor_y + 1, &bomb_grid[cursor_x - 1][cursor_y + 1]);
      open_adjacent_block(cursor_x + 1, cursor_y - 1, &bomb_grid[cursor_x + 1][cursor_y - 1]);
      open_adjacent_block(cursor_x + 1, cursor_y, &bomb_grid[cursor_x + 1][cursor_y]);
      open_adjacent_block(cursor_x + 1, cursor_y + 1, &bomb_grid[cursor_x + 1][cursor_y + 1]);
      open_adjacent_block(cursor_x, cursor_y - 1, &bomb_grid[cursor_x][cursor_y - 1]);
      open_adjacent_block(cursor_x, cursor_y + 1, &bomb_grid[cursor_x][cursor_y + 1]);
    }
    else if(bomb_grid[cursor_x][cursor_y].flaged){
      // daca celula era flagged
      //se scoate flag-ul
      bomb_grid[cursor_x][cursor_y].flaged = false;
      cell_draw(cursor_x, cursor_y, bomb_grid[cursor_x][cursor_y]);
      if(bomb_grid[cursor_x][cursor_y].mine == true){ //daca chiar se afla o bomba acolo se va decrementa discoverd_bombs
        discoverd_bombs--;
      }
    }
    else if(bomb_grid[cursor_x][cursor_y].mine == true){
      //daca se afla o mina in acea celula si se incearca sa se deschida

      game_started = 0; //se opreste jocul
      //difficulty = 0;
      end_time = millis(); //se retine timpul de stop
      for(int i = 0; i < ROWS; i++){ // se afiseaza toate celulele
        for(int j = 0; j < COLUMNS; j++){
          bomb_grid[i][j].open = true; //deschidem celula
          cell_draw(i, j, bomb_grid[i][j]);
        }
      }

      delay(5000);

      end_animation(game_lenght_minutes, game_length_seconds); //se afiseaza animatia de sf
      discoverd_bombs = 0;
      difficulty = 0;
    }
  }

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

  if(joystick_btn == 1){ 
    /*
    *daca se apasa butonul de pe joystick se intra in meniul de pauza
    */
    joystick_btn = 0;
    wait_start_time = millis(); // retine timpul la care a inceput pauza

    //tft
    uint8_t option = 1;

    tft.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_BLUE);
    tft.setCursor(70, 60);
    tft.setTextSize(3);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("Pause");

    tft.setTextSize(2);
    tft.setCursor(80, 100);
    tft.print("Resume");
    tft.setCursor(80, 120);
    tft.print("Quit");

    tft.fillCircle(60, 105, 10, ILI9341_RED);
    
    while(joystick_btn == 0){ //se asteapta pana se alege o optiune
      joystick_logic(); //se citesc comenzile de la joystick
      switch(command){
        case COMMAND_DOWN:
          if(option == 1){
            option = 2;
          }
          else if(option == 2){
            option = 1;
          }
          break;
        
        case COMMAND_UP:
          if(option == 2){
            option = 1;
          }
          else if(option == 1){
            option = 2;
          }
          break;
      }

      tft.fillCircle(60, 105, 10, ILI9341_BLUE);
      tft.fillCircle(60, 125, 10, ILI9341_BLUE);
      if(option == 1){
        tft.fillCircle(60, 105, 10, ILI9341_RED);
      }
      else{
        tft.fillCircle(60, 125, 10, ILI9341_RED);
      }

      delay(50);
    }

    if(joystick_btn == 1){ //daca s-a apasat butonul si s-a ales o optiune
      joystick_btn = 0;
      wait_end_time = millis(); //se retine momentul in care am revenit din pauza
       
      /*
      * daca jocul a inceput i.e. s-a apasat butonul de open sau de flag, atunci cronometrarea a inceput
      * adica start_time != 0, atunci ->
      * se adauga la momentul inceperii jocului timpul petrecut intr-o pauza
      * pentru ca la final end_time - start_time sa nu includa timpii petrecuti in pauza
      * 
      * daca jocul nu a inceput inca i.e. start_time == 0 nu se adauga nimic la start_time pentru a nu initializa
      * gresit momentul de inceput al jocului (acesta trebuir initializat cu millis() in momentul inceperii jocului)
      */
      if(start_time != 0){
        start_time += (wait_end_time - wait_start_time);
      }

      if(option == 1){
        /*
        * se revine la joc
        * se afiseaza din nou gridul
        */
        grid(); // afiseaza grid-ul
        for(int i = 0; i < ROWS; i++){ // se redeseneaza celulele deschise sau cu flag
          for(int j = 0; j < COLUMNS; j++){
            cell_draw(i,j,bomb_grid[i][j]);
          }
        }
        draw_cursor(); // se redeseneaza cursorul
      }
      else if(option == 2){
        /*
        * se incheie jocul
        */
       game_started = 0;
      }
    }
  }

}


void setup() {
  init();
  Serial.begin(115200); // Set baud rate to 115200
  
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // Set the ADC attenuation to 11 dB (up to ~3.3V input)

  pinMode(SW_PIN, INPUT_PULLUP); //pushbutton-ul de pe joystick
  attachInterrupt(digitalPinToInterrupt(SW_PIN), ISR_START, FALLING); //se trimite o intrerupere de fiecare data cand este apasat butonul de start

  pinMode(OPEN_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(OPEN_BTN), ISR_OPEN, FALLING);

  pinMode(FLAG_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLAG_BTN), ISR_FLAG, FALLING);

  Serial.println("Initializing TFT...");
  
  tft.begin(); //Initialize the TFT

  Serial.println("TFT initialized!");
  
  tft.setRotation(3); // 0, 1, 2, 3 for different orientations


  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  randomSeed(analogRead(0));

  menu();

}

void loop() {
  if(game_started == 1){// daca jocul a inceput se apeleaza functia game_logic
    game_logic();
  }
  else{
    exit(0);
  }
}
