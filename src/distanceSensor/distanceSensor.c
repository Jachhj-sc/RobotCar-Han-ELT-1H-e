/*
* distanceSensor.c
*
* Created: 9-11-2020 17:36:26
*  Author: William
*/
#include "distanceSensor.h"
#include <avr/interrupt.h>

int risingEdgeTimeC = 0;
int fallingEdgeTimeC = 0;

unsigned long echoHighLevelTimeC = 0;
unsigned int echoHighLevelTimeuS = 0;
//double echoDistance[DevAmount];

double echoDistance[distSensAmount];

void initDistanceSensor(void){
	//initialize ultrasonic sensor
	
	//setup the DDR register and the pin change mask
	
	DDRD |= (1<<TRIGG0);
	DDRD &= ~(1<<ECHO0);

	PCMSK2 |= (1<<ECHO0);
	PCICR |= (1<<PCIE2);

	sei();//opposite of cli();

	//Timer
	TCCR1B |= (1<<CS11)|(1<<CS10);//counter 1 in register TCNT1  - 4 uS/c clk/64 prescaler
}

void sendTriggPulse(int device){
	TCNT1 = 0;
	int pulseStartTimeC = TCNT1;
	PORTD |= (1<<device);
	
	while (TCNT1 < pulseStartTimeC + triggPulsLengthC);//wait the right amount of time
	
	PORTD &= ~(1<<device);
}

int runcount2 = 0;
ISR(PCINT2_vect){//PD
	//check which device it is and keep that device for the falling edge
	static int prevDevice;
	static int device;
	if ((device = PIND) == 0)
	{
		device = prevDevice;//save the current device
	}
	
	switch(device){
		case (1<<ECHO0):
		case 0:
		//if device is device 0 or if the pin ECHO0 gave a high
		if (runcount2 == 0)
		{
			prevDevice = 0;
			TCNT1 = 0;
			risingEdgeTimeC = TCNT1;
			
			runcount2++;
		}
		else
		{
			prevDevice = -1;
			fallingEdgeTimeC = TCNT1;
			
			echoHighLevelTimeC = fallingEdgeTimeC - risingEdgeTimeC;
			echoHighLevelTimeuS = echoHighLevelTimeC * prescaleDiv;
			echoDistance[device] = echoHighLevelTimeuS/58.0 + distanceCal0;// in cm
			
			runcount2 = 0;
		}
		break;
		
		//use for multiple sound sensors
// 		case (1 << ECHO1):
// 		case 1:
// 		if (runcount2 == 0)
// 		{
// 			prevDevice = 0;
// 			TCNT1 = 0;
// 			risingEdgeTimeC = TCNT1;
// 			
// 			runcount2++;
// 		}
// 		else
// 		{
// 			prevDevice = -1;
// 			fallingEdgeTimeC = TCNT1;
// 			
// 			echoHighLevelTimeC = fallingEdgeTimeC - risingEdgeTimeC;
// 			echoHighLevelTimeuS = echoHighLevelTimeC * prescaleDiv;
// 			echoDistance[device] = echoHighLevelTimeuS/58.0 + distanceCal1;// in cm
// 			
// 			runcount2 = 0;
// 		}	
// 		break;		
	}
}