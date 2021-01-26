/*
 * blinkLedControl.c
 *
 * Created: 26-1-2021 13:12:56
 * Author : William
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "blinker.h"
#include <avr/interrupt.h>

int main(void)
{
	initBlinker();
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		blinkerController(Left);
		_delay_ms(5000);
		blinkerController(Right);
		_delay_ms(5000);
		blinkerController(VOID);
		_delay_ms(5000);
    }
}



