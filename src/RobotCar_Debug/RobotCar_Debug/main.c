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
#include <util/delay.h>

#include "screenController.h"
#include "pwm.h"
#include "USART_RS232.h"		/* include USART library */
#include "distanceSensor.h"
#include "speedometer.h"
#include "interrupts.h"
#include "battery.h"

void soundEchoCycle(void);
void angleCalculator(void);
void pwmUpdate();
void ModePicker();
void driveBackward(int cm);
void driveForward(int cm);

#define MinSpeed 130
//all data variables.

int angle_deg = 0;
int speed = 0; //value between -255 and 255
int steer = 0; //value between -255 and 255
//int direction = 0;

int distance = 10;

uint32_t SpeedoCountMain = 0;
uint32_t SpeedoDistanceCmMain = 0;
int SpeedoSpeed = 0;


//default settings
int Settings[settings] = {
	0, //mode
	255, //speed from 0 - 255
	50, //counter delay auto mode
	100, //Minspeed
	0  //distance sensor enabled or disabled
};

int8_t *main_currentPage = 0;

unsigned int Time_ms = 0;//accuracy is fluctuating
unsigned int Time_min = 0;//accurate

void setup_display(void){
	//screen initialization
	main_currentPage = u8g2_setup(Settings, &Settings[1], &steer, &distance, &angle_deg, &SpeedoSpeed); //save the address of the currentPage

	sys_init();
	but_init();
	timeInit(&Time_min, &Time_ms);
	ADCinit();
}


int main()
{
	setup_display();
	initBluetooth(Settings, &speed, &steer);
	initSpeedometer(&SpeedoCountMain, &SpeedoSpeed);
	
	initDistanceSensor(&distance, &angle_deg);//the default pins are setup in soundsensor.h
	
	initPWM();

	sei();


	while(1){
		SpeedoDistanceCmMain = SpeedoCountMain / 2;//centimeters
		screenRoutine();//function for updating the display.

// 		if (Settings[4] != 0)
// 		{
// 			//soundEchoCycle(); //moved to speedometer
// 		}
		
		

		//the variable with distance info echoDistance[deviceNum] for example echoDistance[0];

		ModePicker();
		// 		PWM_digPin9_Output(125);
		// 		PWM_digPin10_Output(125);
	}
}

void soundEchoCycle(){
	//soundecho routine
	static int count1 = 0;
	//add here the code for the cycle it needs to perform to trigger all the sound sensors
	if (TCNT1 >= cycleLengthC && count1 == 0){
		sendTriggPulse(TRIGG1);
		count1 = 1;
	}
	else if(TCNT1 >= cycleLengthC && count1 == 1){
		sendTriggPulse(TRIGG0);
		count1 = 0;
	}
}

void ModePicker(){
	static int count3 =0;

	switch (Settings[0])
	{
		case 0://manual mode
		pwmUpdate();//change the speed of the motors.
		break;
		
		case 1: //auto mode 1
		_delay_ms(3000);
		if(Settings[0] != 1){
			break;
		}
		speed = Settings[3];
		pwmUpdate();
		_delay_ms(1500);

		speed = 0;
		steer = Settings[3]+50;
		pwmUpdate();
		_delay_ms(1700);

		speed = -Settings[3];
		steer = 0;
		pwmUpdate();
		_delay_ms(1000);

		speed  = 0;
		steer = Settings[3]+50;
		pwmUpdate();
		_delay_ms(1000);

		speed = Settings[3];
		steer = 0;
		pwmUpdate();
		_delay_ms(1500);

		Settings[0] = 0;
		speed = 0;
		steer = 0;
		pwmUpdate();
		break;
		
		case 2://auto mode 2
		_delay_ms(3000);
		if(Settings[0] != 1){
			break;
		}
		
		
		break;
		
		case 3://auto mode 3
		if (count3 == 0)
		{
			_delay_ms(2000);
			if(Settings[0] != 3){
				break;
			}
			
			driveForward(200);//in cm
			speed = 0;
			steer = 0;
			pwmUpdate();
		}else if (count3 == 1)
		{
			driveForward(100);//in cm
			
			Settings[0] = 0;
			speed = 0;
			steer = 0;
			count3 = 0;
			pwmUpdate();
		}

		
		//driveBackward(50);

		count3++;

		//_delay_ms(2000);
		break;
	}
	

}

void driveForward(int cm){
	uint32_t PrevDistanceVal = 0;
	int countSTEER = 0;
	PrevDistanceVal = SpeedoDistanceCmMain;
	
	//speed = 101;
	while (SpeedoDistanceCmMain - PrevDistanceVal < cm)
	{
		if(Settings[0] == 0){
			speed = 0;
			steer = 0;
			pwmUpdate();
			break;
		}
		if (distance < 30 && countSTEER < 1){
			//steer around object sequence
			speed = 0;
			steer = -150;
			pwmUpdate();
			_delay_ms(600);
			steer = 0;
			pwmUpdate();
			
			//drive forward.
			
			PrevDistanceVal = SpeedoCountMain / 2;
			SpeedoDistanceCmMain = SpeedoCountMain / 2;
			while (SpeedoDistanceCmMain - PrevDistanceVal < 40)
			{
				speed = 140;
				pwmUpdate();
				SpeedoDistanceCmMain = SpeedoCountMain / 2;//centimeters
			}
			speed = 0;
			steer = 150;
			pwmUpdate();
			_delay_ms(600);
			
			steer = 0;
			pwmUpdate();
			countSTEER++;
		}
		
		
		//ramp up speed
		if (speed < Settings[1])
		{
			speed++;
			pwmUpdate();
		}
		//ramp down speed
		if (speed > Settings[1])
		{
			speed--;
			pwmUpdate();
		}
		_delay_ms(4);
		SpeedoDistanceCmMain = SpeedoCountMain / 2;//centimeters
		
	}
	speed = 0;
	steer = 0;
	pwmUpdate();
}


void driveBackward(int cm){
	uint32_t PrevDistanceVal = 0;
	
	PrevDistanceVal = SpeedoDistanceCmMain;
	//speed = -101;
	while (SpeedoDistanceCmMain - PrevDistanceVal < 40)
	{
		if(Settings[0] == 0){
			Settings[0] = 0;
			speed = 0;
			steer = 0;
			pwmUpdate();
			break;
		}
		
		//ramp up speed
		
		if (speed > -Settings[1])
		{
			speed--;
			pwmUpdate();
		}
		//ramp down speed
		if (speed < -Settings[1])
		{
			speed++;
			pwmUpdate();
		}
		
		//speed = -Settings[3];
		pwmUpdate();
		_delay_ms(4);
		
		SpeedoDistanceCmMain = SpeedoCountMain / 2;//centimeters
	}
	speed = 0;
	steer = 0;
	pwmUpdate();
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
		
		}else{//drive backwards
		SetDriveBackward();
		
		//change speed to a positive variable again.
		//speed *= -1;
		
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



