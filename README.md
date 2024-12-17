# Minesweeper-Console
## Introduction
  My project idea was to implement the well-known Minesweeper game using a microcontroller, a joystick and an LCD screen. I was motivated to choose this project topic because Minesweeper is a game that I have enjoyed playing since childhood. The purpose of the game is entertainment. The objective is to clear the board without detonating any mines in the grid, using clues about the number of neighboring mines in each cell.
  In my opinion, it is a good way to have fun and relieve stress.
## General Description
The Console is based on the ESP32-WROOM-32D microcontroller. La acesta este conectat prin interfata SPI (VSPI) un ecran LCD TFT de 2.2" 320x240px, cu controller ILI9341, care afiseaza jocul. Alimetarea microcontroller-ului se face printr-un cablu microUSB de la sursa de 5V DC. Jocul este controlat prin intermediul a 2 pushbuttons si al uni joystick. Joistick-ul este folosit pentru a naviga prin meniul jocului si pentru a naviga prin celulele de pe tabela de joc. Butoanele sunt folosite pentru a marca flag intr-o celula sau pentru a deschide o celula. Joistock-ul  are 5 pini: VCC conectat la 3V3, GND, VRX This pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in X-direction (axis), VRY This pin provides an analog output voltage from 0 volts to VCC according to the movement of Holder in Y- direction (axis) si SW. Pnii VRX si VRY sunt conectati la pinii de ADC SENSOR_VP si SENSOR_VN, care folosesc tensiunea de referinta 3V3. Pinul SW si celelalte 2 pushbuttons sunt conectate la GPIO-uri normale si folosesc pull-up resistors. Pentru a canta diverse inregistrari scurete salvate in memoria Flash, se foloseste un speaker de 8 Ohm, 0.5W, conectat la un modul cu amplificator LM386, care poate da 0.5W, cand e conectat la 5V. Amplificatorul este legat le pinul de DAC IO25, care da 3V3, si amplifica semnalul analogic primit de la aceta dandu-l apoi speaker-ului.
## Hardware Design
## Software Design
## Obtained Results
## Conclusions
## Source Code and other resources
## Journal
- The first problem I encountered was the possibility of not having enough memory to run the game, since the current state of the grid must be remembered, the previous state (many variables) and the animations on the LCD must be managed. So I decided to use the ESP32 microcontroller, which offers more RAM than the Arduino UNO, but whose pins operate at 3.3V.
## Bibliography/Resources
