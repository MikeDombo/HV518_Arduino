#include "HV518.h"

// DIN, CLK, LATCH, Number of Digits, Total Shift Lines, left align display
// No dimming using the strobe line. This example shows dimming using anode PWM
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
int msInSecond = 1000;
uint8_t brightness = 255;

void loop() {
  while(Serial.available() > 0){
    char first = Serial.read();

    // Set time as tHH,MM,SS
    if(first == 't'){
      hours = Serial.parseInt();
      minutes = Serial.parseInt();
      seconds = Serial.parseInt();
      Serial.println("Time set to " + String(hours) + ":" + String(minutes) + ":" + String(seconds));
      if (Serial.read() == '\n') {
        timeSet = true;
      }
    }
    
    // Brighness
    else if(first == 'b'){
      brightness = Serial.parseInt();
      Serial.println("Brightness set to " + String(brightness) + " / 255");
      if (Serial.read() == '\n') {
        return;
      }
    }
    
    // Adjust tick time (milliseconds per second)
    else if(first == 'a'){
      msInSecond += Serial.parseInt();
      Serial.println("Tick adjusted to " + String(msInSecond) + " ms/s");
      if (Serial.read() == '\n') {
        return;
      }
    }
    
    // Print a string
    else if(first == 's'){
      timeSet = false;
      String str = Serial.readString();
      Serial.print("Displaying input string: " + str);
      disp.writeString(0, str);
      disp.displayWithAnodePWM(brightness, 1000);
    }
  }

  // Display the time
  if(timeSet){
    // Clear the display memory before writing into it
    disp.clearDisplayMemory();

    // Write time into display memory
    disp.writeNumber(0, hours, 2);
    disp.writeNumber(2, minutes, 2);
    disp.writeNumber(4, seconds, 2);
    
    // Alternate between colon line and dot
    if(seconds % 2 == 0){
      disp.setDigit(6, specialChars.colonDot);
    }
    else{
      disp.setDigit(6, specialChars.colonLine);
    }

    // Write to display
    disp.displayWithAnodePWM(brightness, msInSecond);

    seconds++; // Tick
    
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
