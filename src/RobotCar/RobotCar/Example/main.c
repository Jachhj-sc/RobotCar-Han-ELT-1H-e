/*
* soundsensors(v0.1).c
*
* Created: 6-11-2020 14:01:33
* Author : William
*/


#include <avr/io.h>
#include <math.h>
#include "distanceSensor/distanceSensor.h"



void soundEchoCycle(void);
void angleCalculator(void);


int main(void)
{
	//initialize button
	//DDRB &= ~(1<<PORTB7);

	initDistanceSensor();//the default pins are setup in soundsensor.h

	while (1)
	{
		soundEchoCycle();
		//the variable with distance info echoDistance[deviceNum] for example echoDistance[0];
		
		angleCalculator();
		
	}
}

//put this cycle in an ISR
int i = 0;
void soundEchoCycle(void){
	//add here the code for the cycle it needs to perform to trigger all the sound sensors
	if (TCNT1 >= cycleLengthC && i == 0){
		sendTriggPulse(TRIGG1);
		i = 1;
	}
	else if(TCNT1 >= cycleLengthC && i == 1){
		sendTriggPulse(TRIGG0);
		i = 0;
	}
}

