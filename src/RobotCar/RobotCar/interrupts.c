/*
 * interrupts.c
 *
 * Created: 27-1-2021 17:44:53
 *  Author: willi
 */ 
#include "avr/interrupt.h"
#include "avr/io.h"
#include "screencontroller.h"
#include "USART_RS232_H_file.h"

ISR(PCINT2_vect)
{
	if( !(PIND & (1<<PCINT16) )){
	char receivedStr[BL_BUFFERSIZE];
	USART_ReceiveString(receivedStr);
	USART_BluetoothChanger(receivedStr);
	}else{
		if ( (BUTTON_PIN & (1<<DOWN)) == 0 )
		keyPressed = DOWN;
		else
		if ( (BUTTON_PIN & (1<<UP)) == 0 )
		keyPressed = UP;
		else
		if ( (BUTTON_PIN & (1<<BACK)) == 0 )
		keyPressed = BACK;
		else
		if ( (BUTTON_PIN & (1<<SELECT)) == 0 )
		keyPressed = SELECT;
	}
}