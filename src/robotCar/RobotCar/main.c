/*
* main.c
*
* Created: 22-10-2020 14:31:57
* Author: William
*
*/
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include "screenController.h"

//#include <math.h>
#include "distanceSensor.h"

void soundEchoCycle(void);
void angleCalculator(void);

//all data variables.
int speed = 5;
int direction = 22;
int distance = 10;

//default settings
int Settings[5] = {
	0, //mode
	2, //dummy
	5, //dummy
	7, //dummy
	8  //dummy
};

int8_t *main_currentPage = 0;

unsigned int Time_ms = 0;//accuracy is fluctuating
unsigned int Time_min = 0;//accurate

void setup_display(void){
	//screen initialization
	main_currentPage = u8g2_setup(Settings, &speed, &direction, &distance); //save the address of the currentPage
	sys_init();
	but_init();
	timeInit(&Time_min, &Time_ms);
	ADCinit();
}

int main()
{
	setup_display();
	
	initDistanceSensor(&distance);//the default pins are setup in soundsensor.h
	
	sei();


	while(1){
		screenRoutine();//function for updating the display.
		
		
		soundEchoCycle();
		//the variable with distance info echoDistance[deviceNum] for example echoDistance[0];
		
		
	}
}

//put this cycle in an ISR

void soundEchoCycle(void){
	
	static int count = 0;
	//add here the code for the cycle it needs to perform to trigger all the sound sensors
	if (TCNT1 >= cycleLengthC && count == 0){
		sendTriggPulse(TRIGG1);
		count = 1;
	}
	else if(TCNT1 >= cycleLengthC && count == 1){
		sendTriggPulse(TRIGG0);
		count = 0;
	}
	angleCalculator();
}