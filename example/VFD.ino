#include "HV518.h"

const uint8_t brightnessMax = 255;
const uint8_t brightnessMin = 33;
int msInSecond = 1000;

// DIN, CLK, LATCH, Number of Digits, Total Shift Lines, left align display
HV518 disp = HV518(5, 6, 7, 6, 64, true);

void setup() {
  Serial.begin(9600);
  disp.setAllHigh();
  disp.updateDisplay();
}

uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;
boolean timeSet = false;
uint8_t brightness = 255;

void loop() {
  // Input time as HH,MM,SS
  while(Serial.available() > 0){
    char first = Serial.read();
    if(first == 't'){
      hours = Serial.parseInt();
      minutes = Serial.parseInt();
      seconds = Serial.parseInt();
      if (Serial.read() == '\n') {
        timeSet = true;
      }
    }
    else if(first == 'b'){
      brightness = Serial.parseInt();
    }
    else if(first == 'a'){
      msInSecond += Serial.parseInt();
      Serial.println(msInSecond);
    }
    else if(first == 's'){
      timeSet = false;
      String str = Serial.readString();
      Serial.print(str);
      disp.writeString(0, str);
      disp.displayWithAnodePWM(brightness, 1000);
    }
  }

  if(timeSet){
    disp.clearDisplayMemory();
    disp.writeNumber(0, hours, 2);
    disp.writeNumber(2, minutes, 2);
    disp.writeNumber(4, seconds, 2);
    if(seconds % 2 == 0){
      disp.setDigit(6, disp.specialChars.colonDot);
    }
    else{
      disp.setDigit(6, disp.specialChars.colonLine);
    }
    disp.displayWithAnodePWM(brightness, msInSecond);

    seconds++;
    // Carry-over
    if(seconds == 60){
      seconds = 0;
      minutes++;
    }
    if(minutes == 60){
      minutes = 0;
      hours++;
    }
    if(hours == 24){
      hours = 0;
    }
  }
}

