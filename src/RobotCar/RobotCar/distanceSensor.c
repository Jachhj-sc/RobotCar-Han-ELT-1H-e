/*
* distanceSensor.c
*
* Created: 9-11-2020 17:36:26
*  Author: William
*/
#define F_CPU 16000000UL
#include "distanceSensor.h"
#include <avr/interrupt.h>
#include <math.h>

int risingEdgeTimeC = 0;
int fallingEdgeTimeC = 0;


unsigned long echoHighLevelTimeC = 0;
unsigned int echoHighLevelTimeuS = 0;

#define sensorseperate 6.6

double length;
double differenceLength;
float angleradian;
float angledegree;

//double echoDistance[DevAmount];

double echoDistance[distSensAmount];

float angledegree;


int *int_angledegree;
int *int_Distance;


void initDistanceSensor(int *pDistance, int *pAngle){
	//initialize ultrasonic sensor
	int_angledegree = pAngle;
	int_Distance = pDistance;
		
	//setup the DDR register and the pin change mask
	
	DDRD |= (1<<TRIGG0);
	DDRD &= ~(1<<ECHO0);
	DDRD |= (1<<TRIGG1);
	DDRD &= ~(1<<ECHO1);

	PCMSK2 |= (1<<ECHO0) | (1<<ECHO1);;
	
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

float angleCalculator(void){
	if (echoDistance[0] <= 20 && echoDistance[1] >= echoDistance[0])
		{
			DDRB |= (1<<PORTB5);
			PORTB = (1<<PORTB5);
			length = (echoDistance[1] - echoDistance[0]);
			differenceLength = length / sensorseperate; //calculate the number that needs to be put in the arctan.
			angleradian = atan(differenceLength); //calculate the angle from the wall in radian.
			angledegree = angleradian * 180 / 3.14; //calculate the angle from the wall in degrees.
		}
		else if (echoDistance[1] <= 20 && echoDistance[0] >= echoDistance[1])
		{
			DDRB |= (1<<PORTB4);
			PORTB = (1<<PORTB4);
			length = (echoDistance[0] - echoDistance[1]);
			differenceLength = length / sensorseperate; //calculate the number that needs to be put in the arctan.
			angleradian = atan(differenceLength); //calculate the angle from the wall in radian.
			angledegree = angleradian * 180 / 3.14; //calculate the angle from the wall in degrees.
		}
		else{
			PORTB &= ~(1<<PORTB5);
			PORTB &= ~(1<<PORTB4);
		}
		
		return angledegree; 
}


ISR(PCINT2_vect){//PD
	static int runcount2 = 0;
	static int runcount1 = 0;
	
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
		case (1 << ECHO1):
		case 1:
		if (runcount1 == 0)
		{
			prevDevice = 1;
			TCNT1 = 0;
			risingEdgeTimeC = TCNT1;

			runcount1++;
		}
		else
		{
			prevDevice = -2;
			fallingEdgeTimeC = TCNT1;

			echoHighLevelTimeC = fallingEdgeTimeC - risingEdgeTimeC;
			echoHighLevelTimeuS = echoHighLevelTimeC * prescaleDiv;
			echoDistance[device] = echoHighLevelTimeuS/58.0 + distanceCal1;// in cm

			runcount1 = 0;
		}
		break;
	}
	
	*int_Distance =(int) (echoDistance[0] + echoDistance[1]) / 2;
}
