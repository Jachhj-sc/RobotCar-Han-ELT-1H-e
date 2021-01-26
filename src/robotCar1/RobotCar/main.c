/*
* main.c
*
* Created: 22-10-2020 14:31:57
* Author: William
*
*/
#include <avr/interrupt.h>
#include <avr/io.h>

#include "screenController.h"

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
	
	sei();


	while(1){

		if(speed++ >= 255) {
			speed=0;
		}
		if(direction++ >= 255) {
			direction=0;
		}
		if(distance++ >= 255) {
			distance=0;
		}
		
		
		
		
		screenRoutine();//function for updating the display.
		
	}
}