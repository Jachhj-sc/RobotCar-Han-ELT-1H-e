/*
 * soundSensor.h
 *
 * Created: 9-11-2020 17:35:58
 *  Author: Bram
 */ 


#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_

void initDistanceSensor(void);
void angleCalculator(void);



void sendTriggPulse(int device);
//void soundEchoCycle(void);

//pins for the sensor
#define TRIGG0 PORTD2
#define ECHO0 PORTD3
//if you use multiple sensors..
#define TRIGG1 PORTD4
#define ECHO1 PORTD5
#define distSensAmount 2 //the amount of distanceSensor devices

#define prescaleDiv 4 // number to divide by to get from uS to timer counts
// \/ \/ \/ the C makes clear that it is in clock pulses with a divider
#define triggPulsLengthC 10/prescaleDiv // minimal length of the trigger pulse // is 3 C
#define cycleLengthC 60000/prescaleDiv //uS

#define distanceCal0 1.8; // distance calibration in cm for sensor 0
#define distanceCal1 1.8;
//to make it global
extern double echoDistance[distSensAmount];


#endif /* DISTANCESENSOR_H_ */
