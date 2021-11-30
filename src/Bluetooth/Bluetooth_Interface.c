/*
   
 */ 

#include <stdio.h>
#include <avr/io.h>
#include "USART_RS232_H_file.h"		/* include USART library */
#include <avr/interrupt.h>
#define LED PORTB					/* connected LED on PORT pin */


ISR(PCINT2_vect)
{
	char Data_in;
	Data_in = USART_RxChar();
	USART_BluetoothChanger(Data_in);
}


int main(void)
{
	//enable pin change interrupt on rc pin
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT16);
	sei();
    DDRB = 0xff;							/* make PORT as output port */
	USART_Init(9600);						/* initialize USART with 9600 baud rate */
	LED = 0;
	
	
	while(1)
	{
		
			
	}		
	return 0;
}