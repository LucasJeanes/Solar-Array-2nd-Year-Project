#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <HardwareSerial.h>

//Custom libraries
#include "EqCalcTest.h"
#include "GPS_Module.h"
#include "InterruptSetup.h"
#include "MotorControl.h"

#define RXD2 16 //Connect TX pin from GPS to 16
#define TXD2 17 //Connect RX pin from GPS to 17
const char* ssid = "Homebrew Mobile";
const char* password = "lucas2009";
const char* apiKey = "0GV8ECT4EGN6VYGL";
unsigned long channelID = 2141777; 
const unsigned int fieldToUpdate = 2;
byte gpsTimeOutCount = 60; //Seconds before GPS connection times out and goes into manual mode
int gpsConnectAttempts = 0; //GPS connection attempts count
bool setupComplete = false; //Manual Setup flag
WiFiClient client;
    
void setup() {
    Serial.begin(115200);
    while(!Serial) {
      ; //Wait for serial port to connect
    }
    WiFi.mode(WIFI_STA);
    ThingSpeak.begin(client); //initialize ThingSpeak
    SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
    Motor_Setup();
    Motor_ResetPosition();
    delay(500);
    TimerSetup();
    gpsFlag = false;
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid,password);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected to ThingSpeak Server.");
  }
  while(gps_TimeOutFlag == false) { //While GPS connection has NOT timed out
    if(gpsFlag == false) {          //If no GPS connection has been made, attempt to connect
      GPS_FirstConnection();
      gpsConnectAttempts++;
    }
    while(gpsFlag == true) {  //If GPS connection successful, start tracking as normal
      while(SerialGPS.available() > 0) {
        if(gps.encode(SerialGPS.read())) {
          GPS_GetTime();
        }
      }    
      if(timeUpdated == true) {
        GPS_PrintReadout();
        if(motorUpdateTimer == true) {
          float timeAngle = HourAngle(gps_HourValue, gps_MinuteValue);
          float sunElevation = GetSunAngle(hemiCheck, gps_DayValue, gps_MonthValue, gps_HourValue, gps_MinuteValue, gps_GetLat);
          timeAngle *= (180/M_PI);
          timeAngle += 90;
          Motor_UpdatePosition(timeAngle, sunElevation);
          motorUpdateTimer = false;
          int x = ThingSpeak.writeField(channelID, fieldToUpdate, sunElevation, apiKey);
          if(x == 200) {
            Serial.println("Webserver Update successful.");
          } else {
            Serial.println("Problem updating channel. HTTP error code " + String(x));
          }
          Serial.printf("\nCurrent Sun angle: %.4f", sunElevation);
        }
        timeUpdated = false;
      }
    }
    //If GPS fails connection 3 times, connection times out and manual override kicks in
    if(gpsConnectAttempts >= 3) { 
      gps_TimeOutFlag = true;
    }
  }
  
  //GPS TIME OUT - MANUAL OVERRIDE CODE HERE
  while(gps_TimeOutFlag == true) {
    if(setupComplete == false) {  //First time manual setup
      ManualSetup();
      setupComplete = true;
    }
    
    if(timeUpdated == true) { //If time updated, print readout
      GPS_PrintReadout();
      if(motorUpdateTimer == true) {
          float timeAngle = HourAngle(gps_HourValue, gps_MinuteValue);
          float sunElevation = GetSunAngle(hemiCheck, gps_DayValue, gps_MonthValue, gps_HourValue, gps_MinuteValue, gps_GetLat);
          timeAngle *= (180/M_PI);
          timeAngle += 90;
          Motor_UpdatePosition(timeAngle, sunElevation);
          motorUpdateTimer = false;
          int x = ThingSpeak.writeField(channelID, fieldToUpdate, sunElevation, apiKey);
          if(x == 200) {
            Serial.println("Webserver Update successful.");
          } else {
            Serial.println("Problem updating channel. HTTP error code " + String(x));
          }
          Serial.printf("\nCurrent Sun angle: %.4f", sunElevation);
          Serial.printf("\nCurrent Hour angle: %.4f", timeAngle);
          //For simulation purposes
          //gps_HourValue++;
        }
      timeUpdated = false;
    }
    if(timeOutReset >= gpsTimeOutCount) { //If time until last check has passed, prompt GPS connection retry
    Serial.println("\nDo you want to try connecting to a satellite again? (Y/N)");
    while(!Serial.available()) {}
     char userInput = Serial.read();
     if(userInput == 'Y' || userInput == 'y'){
      gpsConnectAttempts = 0;
      timeOutReset = 0;
      gps_TimeOutFlag = false;
     }
     else {timeOutReset = 0;}
    }
  }
}
