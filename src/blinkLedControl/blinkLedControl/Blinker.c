/*
 * Blinker.c
 *
 * Created: 26-1-2021 13:13:31
 *  Author: willi
 */ 
#define F_CPU 16000000UL
#include "blinker.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LEFT_LED_ON PORTB |= (1<<PORTB5)
#define LEFT_LED_OFF PORTB &= ~(1<<PORTB5)
#define LEFT_LED_TOGGLE PORTB ^= (1<<PORTB5)

#define RIGHT_LED_ON PORTB |= (1<<PORTB4)
#define RIGHT_LED_OFF PORTB &= ~(1<<PORTB4)
#define RIGHT_LED_TOGGLE PORTB ^= (1<<PORTB4)


char heading = ' ';

ISR(TIMER1_OVF_vect){
	switch(heading){
		case Left:
		//code for editing the blinker  light
		LEFT_LED_TOGGLE;
		RIGHT_LED_OFF;
		break;
		
		case Right:
		RIGHT_LED_TOGGLE;
		LEFT_LED_OFF;
		break;
		
		default:
		LEFT_LED_OFF;
		RIGHT_LED_OFF;
		break;
		
	}
}

void initBlinker(){
	DDRB |= ((1<< DDB5) | (1<<DDB4) | (1<<DDB3));
	//PORTB |= ();
	
	TCCR1B |= (1<<CS12);
	TIMSK1 |= (1<<TOIE1);
}

void blinkerController(char LR){
	switch(LR){
		case Left:
		//code for editing the blinker  light
		heading  = 'l';
		break;
		
		case Right:
		heading = 'r';
		break;
		
		default:
		heading = ' ';
		break;
		
	}
	
}