/*
* PWM_Robotcar_V1.2.c
*
* Created: 5-1-2021 11:01:42
* Author : 31613
*/

#include <avr/io.h>
#include "pwm.h"

#define true 1
#define false 0

void initPWM(){
	PWM_digPin9_init(0);
	SetDriveForward();
}

// - - - DIGITAL PIN 9 PWM (PB1-OC1A) - - - //
void PWM_digPin9_init(bool invert_pwm9)
{
	if (invert_pwm9 == 1)
	{
		TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM2A0)| (1 << COM2B1) | (1 << COM2B0);
	}

	else {
		TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM1B1);
	}

	TCCR2B |= (1 << CS22);
	DDRB |= (1 << PINB3);//PB3
	DDRD |= (1 << PIND3);//PD3
	DDRC |= (1 << PINC0) | (1 << PINC1);
	

}

void PWM_digPin9_Output(uint8_t duty)
{
	//0-255
	OCR2A = duty;
}
// - - - DIGITAL PIN 10 PWM (PB2-OC1B) - - - //
void PWM_digPin10_Output(uint8_t duty)
{
	//0-255
	OCR2B = duty;
}


void SetDriveForward(){
	PORTC |= (1 << PORTC0) | (1 << PORTC1);
}

void SetDriveBackward(){
	PORTC &= ~((1 << PORTC0) | (1 << PORTC1));
}

void SpinRight(){
	PORTC |=  (1 << PORTC0);
	PORTC &= ~(1 << PORTC1);
}

void SpinLeft(){
	PORTC |=  (1 << PORTC1);
	PORTC &= ~(1 << PORTC1);
}