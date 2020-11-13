/*
* soundsensors(v0.1).c
*
* Created: 6-11-2020 14:01:33
* Author : William
*/


#include <avr/io.h>
#include "SoundSensor/soundSensor.h"


void soundEchoCycle(void);


int main(void)
{
	//initialize button
	//DDRB &= ~(1<<PORTB7);

	initSoundSensor();//the default pins are setup in soundsensor.h

	while (1)
	{
		soundEchoCycle();
		//the variable with distance info echoDistance[deviceNum] for example echoDistance[0];
	}
}

//put this cycle in an ISR
void soundEchoCycle(void){
	//add here the code for the cycle it needs to perform to trigger all the sound sensors
	if (TCNT1 >= cycleLengthC){
		sendTriggPulse(TRIGG0);
	}
}

