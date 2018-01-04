#include "HV518.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h> // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> // Local WebServer used to serve the configuration portal
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <NTPtimeESP.h>
#include <DoubleResetDetector.h>  //https://github.com/datacute/DoubleResetDetector
#define DRD_TIMEOUT 10 // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use

/*
* User Settable Constants
*/
const float TIME_ZONE = -5.0;
const int US_EUROPE = 2; // 2 for US, 1 for Europe, 0 for no DST

const char* NTP_SERVER_ADDRESS = "time.nist.gov";
const int NTP_UPDATE_FREQUENCY = 60; // Update frequency in minutes

uint8_t brightness = 255;
const uint8_t numRegisters = 8;

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
NTPtime NTPch(NTP_SERVER_ADDRESS);

// DIN, CLK, LATCH, Number of Digits, Total Shift Lines, left align display
// No dimming using the strobe line. This example shows dimming using anode PWM
HV518 disp = HV518(D5, D6, D7, 6, 64, true);

os_timer_t secondTimer;
os_timer_t ntpUpdateTimer;
unsigned long lastTime = 0;
bool secondTick;
bool ntpTick;
strDateTime dateTime;
bool firstTime = true;

void secondTimerCallback(void *pArg) {
  lastTime++;
  if(!ntpTick){
    secondTick = true;
  }
}

void ntpTimerCallback(void *pArg) {
  ntpTick = true;
}

void timerInit() {
  os_timer_setfn(&secondTimer, secondTimerCallback, NULL);
  os_timer_arm(&secondTimer, 1000, true);

  os_timer_setfn(&ntpUpdateTimer, ntpTimerCallback, NULL);
  os_timer_arm(&ntpUpdateTimer, NTP_UPDATE_FREQUENCY * 60 * 1000, true);
}

void setupWiFi();

void setup() {
  Serial.begin(115200);
  Serial.println();

  disp.setAllHigh();
  disp.updateDisplay();

  setupWiFi();

  secondTick = false;
  ntpTick = true;

  timerInit();
}

void loop() {
  if(secondTick){
    secondTick = false;
    printUTCTime(lastTime);
  }
  else if(ntpTick){
    Serial.print("NTP update started: ");
    Serial.println(lastTime);
    printUTCTime(lastTime);
    disp.updateDisplay();
    ntpTick = false;

    dateTime = NTPch.getNTPtime(TIME_ZONE, US_EUROPE);
    if(dateTime.valid){
      lastTime = dateTime.epochTime;
      Serial.print("NTP time updated, new time: ");
      Serial.println(lastTime);
      firstTime = false;
    }
    else {
      Serial.println("NTP retrying");
      ntpTick = true;
      secondTick = false;
      delay(500);
      if(!firstTime){
        delay(500);
      }
    }
  }

  drd.loop();

  while(Serial.available() > 0){
    char first = Serial.read();    
    // Brighness
    if(first == 'b'){
      brightness = Serial.parseInt();
      Serial.println("Brightness set to " + String(brightness) + " / 255");
      if (Serial.read() == '\n') {
        return;
      }
    }
  }  
}

bool secondPassed(){
  return !ntpTick && !secondTick;
}

void printUTCTime(unsigned long epoch){
  unsigned int seconds = epoch % 60;
  disp.clearDisplayMemory();
  disp.writeNumber(0, (epoch  % 86400L) / 3600, 2); // print the hour
  disp.writeNumber(2, (epoch  % 3600) / 60, 2); // print the minute
  disp.writeNumber(4, seconds, 2); // print the second

  // Alternate between colon line and dot
  if(seconds % 2 == 0){
    disp.setDigit(6, specialChars.colonDot);
  }
  else{
    disp.setDigit(6, specialChars.colonLine);
  }

  // Write to display
  disp.displayWithAnodePWM(brightness, secondPassed);
}

void setupWiFi(){
  if (WiFi.SSID() == "" || drd.detectDoubleReset()){
    WiFi.disconnect();
    Serial.println("Starting configuration portal.");
    WiFiManager wifiManager;
    if (!wifiManager.startConfigPortal()) {
      Serial.println("Not connected to WiFi but continuing anyway.");
    }
    else {
      Serial.println("Connected!");
    }

    ESP.restart();
    delay(5000);
  }

  WiFi.mode(WIFI_STA); // Force to station mode because if device was switched off while in access point mode it will start up next time in access point mode.
  Serial.println("Done setting up wifi");
}

