/*
* Speedometer.c
*
* Created: 30-1-2021 21:04:24
*  Author: willi
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "distanceSensor.h"

uint32_t *spdo_Count;
int spdo_CountPrev = 0;
int spdo_CountpSec = 0;
int *spdo_speed;

//#define cycleLengthC 64000/prescaleDiv 
// #define TRIGG1 PORTB4
// #define TRIGG0 PORTB1


void initSpeedometer( uint32_t *SPDOCount, int *SPDOspeed){
	
	spdo_Count = SPDOCount;
	spdo_speed = SPDOspeed;
	DDRB &= ~(1<<PINB2);
	
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC00)/* | (1<<ISC01)*/; //0 run interrupt any logical change. 0 & 1 = only rising edge
	
	//rest of the timer is setup in pwm.c
	TIMSK2 |= (1<<TOIE2);
}

ISR(INT0_vect){//on pin change
	(*spdo_Count)++;
}

ISR(TIMER2_OVF_vect){//timer overflow
	static int count = 0;
	//static int prevcount = 0;
	if(count++ >= 976){//roughly every second
		*spdo_speed = 0;
		
		spdo_CountpSec = *spdo_Count - spdo_CountPrev;
		
		*spdo_speed = spdo_CountpSec / 2; //Meters and since its a time interval of 1 second you get meters per second
		*spdo_speed *= 3.6; //from m/s to km/h
		
		//prevcount = count;
		spdo_CountPrev = *spdo_Count;
		
		count = 0;
	}
	
}