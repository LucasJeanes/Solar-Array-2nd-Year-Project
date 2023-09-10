//NAME: Lucas Jeanes
//DATE: 17/12/2022
//DESC: Elevation Angle Calculator - Arduino Compatible

#include<stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>

float EquinoxCalc(char, byte, byte);  //hemiCheck, day, month
float HourAngle(byte, byte);          //hour, minute
float LatitudeAngle(float, char);   //Latitude Angle, hemiCheck
float SolarElevationCalc(float, float, float); //declinationAngle, LatAngle, hourAngle
float GetSunAngle(char, byte, byte, byte, byte, float);

float LatitudeAngle(float latInput, char hemiCheck) {
  float latOut = latInput * (M_PI / 180);
  //If latitude is in southern Hemisphere, flip sign to negative
  if (hemiCheck == 'S') {
    latOut *= -1;
    return latOut;
  }
  else {
    return latOut;
  }
}

float HourAngle(byte hourInput, byte minuteInput) {
//float HourAngle(int hourInput) {
  const float ONE_MINUTE_RAD = 0.004363323; //Radians per minute (Out of 15' per hour)
  float hourAngle;

  minuteInput *= ONE_MINUTE_RAD;
  hourAngle = M_PI / 12;
  hourAngle *= (hourInput - 12);
  hourAngle += minuteInput;

  return hourAngle;
  //[DEBUG PRINT] Serial.printf("\nHour angle rad: %.2f",hourAngle);
}

float EquinoxCalc(char hemiCheck, byte dayEntry, byte monthEntry) {
  //North Equinox 20 March = 78 days from Jan 1
  //South Equinox 23 Sept = 264 days ""   ""
  //Average days in a month = 30.437
  const float DAYS_MAX = 365.25, DAY_AVG = 30.437, DECL_ANGLE_CONSTANT = 23.45;
  const byte MONTH_MAX = 12, NORTH_EQ = 78;
  const int SOUTH_EQ = 264;
  short int preEqTest;
  short int dateFromEq = 0;

  //[DEBUG PRINT] Serial.print("\n\n----Date Calculator----\n");

  preEqTest = (dayEntry - 2) + ((monthEntry - 1) * DAY_AVG);
  //Pre equinox test finds the current day out of 365 to determine if year overflow is needed
  //dayEntry - 2: February has 2 days less than average day count per month (to make value more accurate)
  //monthEntry - 1: To remove current month from calculations as current day of the month is already included
  //          (otherwise value will be 30 days over true value)

  //[DEBUG PRINT] Serial.print("\nEquinox test: ");
  //[DEBUG PRINT] Serial.print(preEqTest);

  //If in the north, base calculations off March 20
  if (hemiCheck == 'N') {
    if (preEqTest < NORTH_EQ) {
      //If current date is before the equinox (Jan 1 to March 19) need to include 9 months overflow to wrap around the year
      monthEntry += 9;
      dayEntry += 11;   //Add 11 days to include missing days to make March = 31 days
      dateFromEq = dayEntry + ((monthEntry - 1) * DAY_AVG);
      //[DEBUG PRINT] Serial.print("\nDays since Equinox: ");
      //[DEBUG PRINT] Serial.print(dateFromEq);
    }
    //If current date is after Spring Equinox
    if (preEqTest > NORTH_EQ) {
      if (dayEntry < 20) {
        monthEntry -= 4;  //Set month 0 as March(3rd month) and remove current month from calculations
        dayEntry += 11;   //Add 11 days to include 11 missing days from March equinox to April (March 20 - 31st)
        dateFromEq = (monthEntry * DAY_AVG) + dayEntry;
      }
      else {  //If current day of the month is > 20
        monthEntry -= 3;    //Include current month + set March = 0 point
        dayEntry -= 19;     //Reduce total month days for March + current month by 10, totaling 20 days - current day
        dateFromEq = (monthEntry * DAY_AVG) + dayEntry;
      }
    }
    //If current date is the Spring Equinox
    if (preEqTest == NORTH_EQ) {
      dateFromEq = 1;
    }

    //[DEBUG PRINT] Serial.print("\nDays since Equinox: ");
    //[DEBUG PRINT] Serial.print(dateFromEq);
  }
  //If Southern Hemisphere, set equinox to September
  if (hemiCheck == 'S') {
    if (preEqTest < SOUTH_EQ) {
      monthEntry += 3;
      dayEntry += 7;
      dateFromEq = dayEntry + ((monthEntry - 1) * DAY_AVG);
      //[DEBUG PRINT] Serial.print("\nDays since Equinox: ");
      //[DEBUG PRINT] Serial.print(dateFromEq);
    }
    if (preEqTest > SOUTH_EQ) {
      if (dayEntry < 20) {
        monthEntry -= 1;
        dayEntry += 7;
        dateFromEq = (monthEntry * DAY_AVG) + dayEntry;
      }
      else {
        monthEntry -= 9;
        dayEntry -= 22;
        dateFromEq = (monthEntry * DAY_AVG) + dayEntry;
      }
    }
    if (preEqTest == SOUTH_EQ) {
      dateFromEq = 1;
    }

    //[DEBUG PRINT] Serial.print("\nDays since Equinox: ");
    //[DEBUG PRINT] Serial.print(dateFromEq);
  }

  //DECL_ANGLE_CONSTANT = 23.45;

  float sinValue = (360 * dateFromEq) / DAYS_MAX; //Get value of equinox * day modifier
  //[DEBUG PRINT] Serial.print("\nSin Multiplier degrees = %.3f", sinValue);

  sinValue *= (M_PI / 180); //Convert value to radians for Sin calculation
  //[DEBUG PRINT] Serial.print("\nSin Multiplier radians = %.3f", sinValue);

  sinValue = sin(sinValue);
  //[DEBUG PRINT] Serial.print("\nSin of Multiplier rad= %.3f", sinValue);

  float declAngle = DECL_ANGLE_CONSTANT * sinValue;
  declAngle *= (M_PI / 180);  //Convert final declination angle back to radians for Elevation formula
  //[DEBUG PRINT] Serial.print("\nDeclination Angle degrees: ");
  //[DEBUG PRINT] Serial.print(declAngle);
  //[DEBUG PRINT] Serial.print("\nDeclination Angle radians: ");
  //[DEBUG PRINT] Serial.print(declAngle);

  return declAngle;
}

float SolarElevationCalc(float declAngle, float latAngle, float hourAngle) {
  float elevationAngle;
  float declAngleSin = sin(declAngle);
  float declAngleCos = cos(declAngle);
  float latAngleSin = sin(latAngle);
  float latAngleCos = cos(latAngle);
  float hourAngleCos = cos(hourAngle);
  elevationAngle = asin((declAngleSin * latAngleSin) + (declAngleCos * latAngleCos * hourAngleCos));

  float elevationDeg = elevationAngle * (180 / M_PI);
  if (hourAngle < 0) {
    elevationAngle *= -1;
  }
  return elevationDeg;
  //[DEBUG PRINT] Serial.print("\nElevation angle in degrees: ");
  //[DEBUG PRINT] Serial.print(elevationDeg);
}

float GetSunAngle(char hemiCheck, byte gps_DayValue, byte gps_MonthValue, byte gps_HourValue, byte gps_MinuteValue, float gps_GetLat) {
  float declAngle = EquinoxCalc(hemiCheck, gps_DayValue, gps_MonthValue);
  float hourAngle = HourAngle(gps_HourValue, gps_MinuteValue);
  float latAngle = LatitudeAngle(gps_GetLat, hemiCheck);
  float sunAngle = SolarElevationCalc(declAngle, latAngle, hourAngle);

  return sunAngle;
}
