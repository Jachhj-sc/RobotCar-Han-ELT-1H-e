/*

*/

#include <stdio.h>
#include <avr/io.h>
#include "USART_RS232_H_file.h"		/* include USART library */
#include <avr/interrupt.h>
#include <string.h>

#define LEDPORT PORTB					/* connected LED on PORT pin */
#define LED PORTB1


int main(void)
{
	initBluetooth();
	sei();
	
	while(1)
	{
		;
		
	}
	return 0;
}