#include <ESP32Servo.h>

void Motor_Setup();
void Motor_Test();
void Motor_UpdatePosition();
void Motor_ResetPosition();

byte motorAzimuth = 26, motorElevation = 25;
byte lastPosX = 0, lastPosY = 0;

Servo ServoAzimuth;
Servo ServoElevation;

void Motor_Setup() {
  ServoAzimuth.attach(motorAzimuth,544,2400);
  ServoElevation.attach(motorElevation,544,2400);
}

void Motor_Test() {  //Rotates both motors between 0-degrees and 180-degrees in 20-degree steps
  if(timeUpdated == true) {
    for(int i = 0; i <= 180; i+=20) {
      ServoAzimuth.write(i);
      ServoElevation.write(i);
    }
    for(int i = 180; i >= 0; i-=20) {
      ServoAzimuth.write(i);
      ServoElevation.write(i);
    }
  }
}
void Motor_UpdatePosition(int hourAngle, int sunAngle) {  //Motor position update function, takes in horizontal and vertical values from calculations
    if(sunAngle >= -5 && (lastPosX != hourAngle || lastPosY != sunAngle)) { //While Sun is visible & new position data is ready
      ServoElevation.write(sunAngle);
      ServoAzimuth.write(hourAngle);
      //Store current position values of servos for future check
      lastPosX = ServoAzimuth.read();
      lastPosY = ServoElevation.read();
      Serial.printf("\nMoving motors Azimuth to %d'. Elevation to %d'\n",hourAngle,sunAngle);
    } else {
      Serial.println("Sun too far below horizon. Resetting position for tomorrow...");
      Motor_ResetPosition();
    }
}

void Motor_ResetPosition() {   //Reset Azimuth(Horizontal) motor to 0-degree point and Elevation(Vertical) motor to 90-degree point.
  if(lastPosX != ServoAzimuth.read() || lastPosY != ServoElevation.read()) { 
    lastPosX = ServoAzimuth.read();
    lastPosY = ServoElevation.read();
    Serial.println("Resetting Motors...");
    ServoAzimuth.write(0);
    ServoElevation.write(90);
  } else {Serial.println("Waiting for morning...");}
}
