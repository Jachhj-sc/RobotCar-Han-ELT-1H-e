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
<<<<<<< Updated upstream
<<<<<<< Updated upstream

#include "screenController.h"
=======
=======
>>>>>>> Stashed changes
#include "screenController.h"
#include "pwm.h"
#include "USART_RS232_H_file.h"		/* include USART library */
#include <util/delay.h>
//#include <math.h>
#include "distanceSensor.h"

#include "interrupts.h"


void soundEchoCycle(void);
void angleCalculator(void);
void pwmUpdate();
<<<<<<< Updated upstream
>>>>>>> Stashed changes

#define MinSpeed 130
//all data variables.
<<<<<<< Updated upstream

int speed = 5;
int direction = 22;
=======
int speed = 0; //value between -255 and 255
int steer = 0; //value between -255 and 255
//int direction = 0;
>>>>>>> Stashed changes
=======

#define MinSpeed 130
//all data variables.
int speed = 0; //value between -255 and 255
int steer = 0; //value between -255 and 255
//int direction = 0;
>>>>>>> Stashed changes
int distance = 10;

//default settings
<<<<<<< Updated upstream
<<<<<<< Updated upstream
int Settings[5] = {
	0, //mode
	2, //dummy
	5, //dummy
	7, //dummy
=======
=======
>>>>>>> Stashed changes
int Settings[settings] = {
	0, //mode
	255, //speed from 0 - 255
	50, //counter delay auto mode
	120, //Minspeed
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
	8  //dummy
	};

int8_t *main_currentPage = 0;

unsigned int Time_ms = 0;//accuracy is fluctuating
unsigned int Time_min = 0;//accurate

void setup_display(void){
	//screen initialization
<<<<<<< Updated upstream
<<<<<<< Updated upstream
	main_currentPage = u8g2_setup(Settings, &speed, &direction, &distance); //save the address of the currentPage
=======
	main_currentPage = u8g2_setup(Settings, &Settings[1], &steer, &distance, &angle_deg); //save the address of the currentPage
>>>>>>> Stashed changes
=======
	main_currentPage = u8g2_setup(Settings, &Settings[1], &steer, &distance, &angle_deg); //save the address of the currentPage
>>>>>>> Stashed changes
	sys_init();
	but_init();
	timeInit(&Time_min, &Time_ms);
	ADCinit();
}


int main()
{
<<<<<<< Updated upstream
	setup_display();
	
=======
	
	
	initBluetooth(Settings, &speed, &steer);
	
<<<<<<< Updated upstream
=======
	initBluetooth(Settings, &speed, &steer);
	
>>>>>>> Stashed changes
	setup_display();
	
	initDistanceSensor(&distance, &angle_deg);//the default pins are setup in soundsensor.h
	
	initPWM();
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
	sei();


	while(1){
		screenRoutine();//function for updating the display.
		
<<<<<<< Updated upstream
=======
		
		soundEchoCycle();
		//the variable with distance info echoDistance[deviceNum] for example echoDistance[0];


		//run Autonomous Test Mode
		if(Settings[0] == 1){
			_delay_ms(5000);
			
			
			speed = Settings[3];
			pwmUpdate();
			_delay_ms(1500);
			
			speed = 0;
			steer = Settings[3]+40;
			pwmUpdate();
			_delay_ms(1700);
			
			speed = -Settings[3];
			pwmUpdate();
			_delay_ms(1000);
			
			speed  = 0;
			steer = Settings[3]+40;
			pwmUpdate();
			_delay_ms(1000);

			speed = Settings[3];
			pwmUpdate();
			_delay_ms(1500);
			
			Settings[0] = 0;
			
			}else if(Settings[0] == 2){
			
			speed = Settings[3];
			pwmUpdate();
			_delay_ms(2000);
			
			speed = -Settings[3];
			pwmUpdate();
			
			Settings[0] = 0;
			_delay_ms(2000);
			}else{
			pwmUpdate();//change the speed of the motors.
		}
		// 		PWM_digPin9_Output(125);
		// 		PWM_digPin10_Output(125);
	}
}


void pwmUpdate(){
	//maybe add calibration or offset
	static unsigned short rightwheelSpeed = 0; //0 -255
	static unsigned short leftwheelSpeed = 0; //0-255
	
	//standard mode

	if(speed >= 0){//drive forwards
		SetDriveForward();
		leftwheelSpeed = speed + steer;
		rightwheelSpeed = speed - steer;
		
		if (speed - steer < 0){
			rightwheelSpeed = 0;
			}else if(speed - steer >= 255){
			rightwheelSpeed = 255;
		}
		if (speed + steer < 0){
			leftwheelSpeed = 0;
			}else if(speed + steer >= 255){
			leftwheelSpeed = 255;
		}
		
		if (leftwheelSpeed > 10 && leftwheelSpeed < Settings[3]){
			leftwheelSpeed = Settings[3];
		}
		if (rightwheelSpeed > 10 && rightwheelSpeed < Settings[3]){
			rightwheelSpeed = Settings[3];
		}
<<<<<<< Updated upstream
		
		}else{//drive backwards
		SetDriveBackward();
		
		//change speed to a positive variable again.
		//speed *= -1;
		
=======
		
		}else{//drive backwards
		SetDriveBackward();
		
		//change speed to a positive variable again.
		//speed *= -1;
		
>>>>>>> Stashed changes
		leftwheelSpeed = -speed + steer;
		rightwheelSpeed = -speed - steer;
		
		if (-speed - steer < 0){
			rightwheelSpeed = 0;
			}else if(-speed - steer >= 255){
			rightwheelSpeed = 255;
		}
		if (-speed + steer < 0){
			leftwheelSpeed = 0;
			}else if(-speed + steer >= 255){
			leftwheelSpeed = 255;
		}
		if (leftwheelSpeed > 10 && leftwheelSpeed < Settings[3]){
			leftwheelSpeed = Settings[3];
		}
		if (rightwheelSpeed > 10 && rightwheelSpeed < Settings[3]){
			rightwheelSpeed = Settings[3];
		}
	}
	//speed *= -1; // revert changes so the value stays the same
	PWM_digPin9_Output(leftwheelSpeed);//left
	PWM_digPin10_Output(rightwheelSpeed);//right
	
	




}



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
>>>>>>> Stashed changes
	}
}