/*
 * soundSensor.h
 *
 * Created: 9-11-2020 17:35:58
 *  Author: Bram
 */ 


#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_

void initDistanceSensor(int *pDistance, int *pAngle);
float angleCalculator(void);



void sendTriggPulse(int device);
//void soundEchoCycle(void);

//pins for the sensor
#define TRIGG0 PORTD4
#define ECHO0 PORTD5
//if you use multiple sensors..
#define TRIGG1 PORTD6
#define ECHO1 PORTD7
#define distSensAmount 2 //the amount of distanceSensor devices

#define prescaleDiv 4 // number to divide by to get from uS to timer counts
// \/ \/ \/ the C makes clear that it is in clock pulses with a divider
#define triggPulsLengthC 10/prescaleDiv // minimal length of the trigger pulse // is 3 C
#define cycleLengthC 64000/prescaleDiv //uS  //normal 60000

#define distanceCal0 1.8; // distance calibration in cm for sensor 0
#define distanceCal1 1.8;
//to make it global
extern double echoDistance[distSensAmount];


#endif /* DISTANCESENSOR_H_ */
