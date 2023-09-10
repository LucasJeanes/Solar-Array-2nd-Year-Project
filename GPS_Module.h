#include <TinyGPSPlus.h>

TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

void GPS_GetTime();
void TimeReadout();
void GPS_GetDate();
byte GPS_GetSatelliteCount();
void GPS_GetLocation();
void GPS_PrintReadout();
void GPS_FirstConnection();
void ManualSetup();

float gps_GetLat = 0, gps_GetLong = 0;
byte gps_HourValue = 0, gps_MinuteValue = 0, gps_SecondValue = 0, gps_DayValue = 0, gps_MonthValue = 0;
int gps_YearValue = 0;
char hemiCheck = 'F';
bool gpsFlag = false;

void GPS_FirstConnection() {
  if(gpsFlag == false) {
    Serial.println("\nLooking for satellites...");
      while(SerialGPS.available() > 0) {
        if(gps.encode(SerialGPS.read())) {
          GPS_GetTime();
          byte activeSats = GPS_GetSatelliteCount();
          GPS_GetLocation();
          GPS_GetDate();
          if(activeSats > 0) {
            gpsFlag = true;
            Serial.println("\nConnected to satellites!");
            break;
          }
        }
     }
     delay(2000);
  }
}

void GPS_GetTime() {
    if (gps.time.isValid()) {
        gps_HourValue = gps.time.hour();
        gps_MinuteValue = gps.time.minute();
        gps_SecondValue = gps.time.second();
    }
}

byte GPS_GetSatelliteCount() {
    byte activeSats = 0;
    if (gps.satellites.isValid()) {
        activeSats = gps.satellites.value();
    }
    return activeSats;
}

void GPS_GetLocation() {
    if (gps.location.isValid()) {
        gps_GetLat = (gps.location.lat());
        gps_GetLong = (gps.location.lng());
        if (gps_GetLat > 0) {
            hemiCheck = 'N';
        }
        else if(gps_GetLat < 0){
            hemiCheck = 'S';
        }
    }
}

void GPS_GetDate() {
    if (gps.date.isValid()) {
        gps_DayValue = gps.date.day();
        gps_MonthValue = gps.date.month();
        gps_YearValue = gps.date.year();
    }
}

void TimeReadout() {
    Serial.printf("\nTime: %02d:%02d:%02d",gps_HourValue, gps_MinuteValue, gps_SecondValue);
}

void GPS_PrintReadout() {
    Serial.println("\n+------GPS DATA------+");
    Serial.printf("Time: %02d:%02d:%02d",gps_HourValue, gps_MinuteValue, gps_SecondValue);
    Serial.printf("\nDate: %02d/%02d/%d",gps_DayValue,gps_MonthValue,gps_YearValue);
    Serial.printf("\nNo. of Satellites: %02d",GPS_GetSatelliteCount());
    Serial.printf("\nLatitude: %.4f",gps_GetLat);
    Serial.println("\n+------GPS DATA------+");
}
void ManualSetup() {
  Serial.println("\n[ERROR]: GPS CONNECTION TIMED OUT");
  Serial.println("Starting Manual Procedure....");
  Serial.print("\nInsert manual latitude coordinate for target location: ");
  
  while (!Serial.available()) {}
  gps_GetLat = Serial.parseFloat();
  Serial.printf("%.4f",gps_GetLat);
  if (gps_GetLat > 0) {
    hemiCheck = 'N';
  }
  else if(gps_GetLat < 0){
    hemiCheck = 'S';
  }
  Serial.print("\nInput the current Year(eg. 2023): ");
  while(!Serial.available()) {}
  gps_YearValue = Serial.parseInt();
  Serial.print(gps_YearValue);
  
  Serial.print("\nInput the current Month(1-12): ");
  while(!Serial.available()) {}
  gps_MonthValue = Serial.parseInt();
  Serial.printf("%02d",gps_MonthValue);
  
  Serial.print("\nInput the current Day(1-31): ");
  while (!Serial.available()) {}
  gps_DayValue = Serial.parseInt();
  Serial.printf("%02d",gps_DayValue);
 
  Serial.print("\nInput the current hour in GMT-0 (0-23): ");
  while (!Serial.available()) {}
  gps_HourValue = Serial.parseInt();
  Serial.printf("%02d",gps_HourValue);
  
  Serial.print("\nInput the current minute (1-59): ");
  while (!Serial.available()) {}
  gps_MinuteValue = Serial.parseInt();
  Serial.printf("%02d",gps_MinuteValue);
}
