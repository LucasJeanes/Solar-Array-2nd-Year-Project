//NAME: Lucas Jeanes
//DATE: 13/11/2022
//DESC: Elevation Angle Calculator

#include<stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>


float EquinoxCalc();
float HourAngle(int);
float LatitudeAngle(float);
float DeclinationCalc(float,float,float);

int main()
{
	float latInput, eqInput, hourInput;

	//while (1) {
		//Get latitude coordinate value
		printf("\nPlease input the Latitude coordinate in degrees: ");
		scanf("%f",&latInput);
		latInput = LatitudeAngle(latInput);

		eqInput = EquinoxCalc();

		//For loop to iterate elevation angle from 6am to 8pm (average max sunrise/sunset all year)
		for (int hourInput = 6;hourInput <= 20;hourInput++) {
			float hourValue = HourAngle(hourInput);
			printf("\nHour %i, Rad value: %.3f",hourInput,hourValue);
			DeclinationCalc(eqInput, latInput, hourValue);
		}	
}

float LatitudeAngle(float latInput) {
	char HemiCheck = 'H';
	printf("North or South? N/S: ");
	scanf(" %c", &HemiCheck);

	float latOut = latInput * (M_PI / 180);
	//If latitude is in southern Hemisphere, flip sign to negative
	if (HemiCheck == 'S') { 
		latOut *= -1;
		printf("Latitude angle is: %.2f'\nAngle in radians is: %.4f", latInput, latOut);
	}
	else {
		printf("Latitude angle is: %.2f'\nAngle in radians is: %.4f", latInput, latOut);
	}
	return latOut;

	/*
	float latOut = latInput * (M_PI / 180);
	printf("Latitude angle is: %.2f\nAngle in radians is: %.3f", latInput, latOut);
	return latOut;
	*/
}

float HourAngle(int hourInput) {
	float hourAngle;

	hourAngle = M_PI / 12;
	hourAngle *= (hourInput - 12);
	return hourAngle;
}

float EquinoxCalc() {
	//North Equinox = 78 days from Jan 1
	//South Equinox = 264 days ""   ""
	//Average days in a month = 30.437
	const float daysMax = 365.25, dayAvg = 30.437, declAngleConstant = 23.45;
	const int monthMax = 12, NorthEq = 78, SouthEq = 264;
	int dayEntry, monthEntry, preEqTest;
	int dateFromEq = 0;
	char HemiCheck = 'H';

	printf("\n\n----Date Calculator----\n");
	printf("North or South? N/S: ");
	scanf(" %c", &HemiCheck);
	printf("Enter the day (00): ");
	scanf("%i", &dayEntry);
	printf("Enter the month : ");
	scanf("%i", &monthEntry);

	printf("\nHemisphere entry: %c", HemiCheck);

	preEqTest = (dayEntry - 2) + ((monthEntry - 1) * 30.437);

	printf("\nEquinox test: %i", preEqTest);

	//If in the north, set equinox to March 23
	if (HemiCheck == 'N') {
		if (preEqTest < NorthEq) {
			monthEntry += 9;
			dayEntry += 10;
			dateFromEq = dayEntry + ((monthEntry - 1) * 30.437);
			printf("\nDays since Equinox = %i", dateFromEq);
		}
		//If current date is after Spring Equinox
		if (preEqTest > NorthEq) {
			if (dayEntry < 20) {
				monthEntry -= 4;
				dayEntry += 10;
				dateFromEq = (monthEntry * dayAvg) + dayEntry;
			}
			else {	//If current date is before Spring Equinox (Year turnover)
				monthEntry -= 3;
				dayEntry -= 20;
				dateFromEq = (monthEntry * dayAvg) + dayEntry;
			}
		}
		//If current date is the Spring Equinox
		if (preEqTest == NorthEq) {
			dateFromEq = 1;
		}

		printf("\nDays since Equinox = %i", dateFromEq);
	}
	//If Southern Hemisphere, set equinox to September
	if (HemiCheck == 'S') {
		if (preEqTest < SouthEq) {
			monthEntry += 3;
			dayEntry += 10;
			dateFromEq = dayEntry + ((monthEntry - 1) * 30.437);
			printf("\nDays since Equinox = %i", dateFromEq);
		}
		if (preEqTest > SouthEq) {
			if (dayEntry < 20) {
				monthEntry -= 1;
				dayEntry += 10;
				dateFromEq = (monthEntry * dayAvg) + dayEntry;
			}
			else {
				monthEntry -= 9;
				dayEntry -= 23;
				dateFromEq = (monthEntry * dayAvg) + dayEntry;
			}
		}
		if (preEqTest == SouthEq) {
			dateFromEq = 1;
		}

		printf("\nDays since Equinox = %i", dateFromEq);
	}

	//declAngleConstant = 23.45;

	float sinValue = (360 * dateFromEq) / daysMax;	//Get value of equinox * day modifier
//[DEBUG PRINT]	printf("\nSin Multiplier degrees = %.3f", sinValue);

	sinValue *= (M_PI / 180);	//Convert value to radians for Sin calculation
//[DEBUG PRINT] printf("\nSin Multiplier radians = %.3f", sinValue);

	sinValue = sin(sinValue);
//[DEBUG PRINT] printf("\nSin of Multiplier rad= %.3f", sinValue);

	float declAngle = declAngleConstant * sinValue;		
	printf("\nDeclination Angle degrees: %.3f", declAngle);
	declAngle *= (M_PI/ 180);	//Convert final declination angle back to radians for Elevation formula
	printf("\nDeclination Angle radians: %.3f", declAngle);

	return declAngle;
}

float DeclinationCalc(float declAngle, float latAngle, float hourAngle) {
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
	printf("\nElevation angle in degrees: %.2f\n",elevationDeg);
	return elevationDeg;
}
