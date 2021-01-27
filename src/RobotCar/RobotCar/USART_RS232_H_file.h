/*
 */ 


#ifndef USART_RS232_H_FILE_H_				/* Define library H file if not defined */
#define USART_RS232_H_FILE_H_

#define F_CPU 16000000UL						/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>							/* Include AVR std. library file */
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)	/* Define prescale value */
#define settings 5
#define BL_BUFFERSIZE 10

void USART_Init(unsigned long);				/* USART initialize function */
char USART_RxChar();						/* Data receiving function */
void USART_TxChar(char);					/* Data transmitting function */
void USART_SendString(char*);				/* Send string of USART data function */
void USART_BluetoothChanger(char Data_in[BL_BUFFERSIZE]);			/* Makes the micro controller send values */
void USART_ReceiveString(char *str);
void initBluetooth(int SettingsHold[settings]);

#endif