/*
*/

#include "USART_RS232_H_file.h"						/* Include USART header file */
#include <string.h>
#include <avr/interrupt.h>

#define BL_LED_ON "ON"
#define BL_LED_OFF "OFF"
#define BL_FORWARD "FORWARD"
#define BL_BACKWARD "BACKWARD"
#define BL_LEFT "LEFT"
#define BL_RIGHT "RIGHT"
#define BL_ModeDown "MODEDOWN"
#define BL_ModeUp "MODEUP"

#define settings 5


int *Settings;


int result;
void initBluetooth(int *SettingsHold[settings]){
	Settings = &SettingsHold[0];
	
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT16);
	
	USART_Init(9600);						/* initialize USART with 9600 baud rate */
}

void USART_Init(unsigned long BAUDRATE)				/* USART initialize function */
{
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);				/* Enable USART transmitter and receiver */
	UCSR0C |= (1 << UCPOL0)| (1 << UCSZ00) | (1 << UCSZ01);	/* Write USCRC for 8 bit data and 1 stop bit */
	UBRR0L = BAUD_PRESCALE;							/* Load UBRRL with lower 8 bit of prescale value */
	UBRR0H = (BAUD_PRESCALE >> 8);					/* Load UBRRH with upper 8 bit of prescale value */
}

char USART_RxChar()									/* Data receiving function */
{
	while (!(UCSR0A & (1 << RXC0)));					/* Wait until new data receive */
	return(UDR0);									/* Get and return received data */
}

void USART_TxChar(char data)						/* Data transmitting function */
{
	UDR0 = data;										/* Write data to be transmitting in UDR */
	while (!(UCSR0A & (1<<UDRE0)));					/* Wait until data transmit and buffer get empty */
}

/*
* This function gets a string of characters from the USART.
* The string is placed in the array pointed to by str.
*
* - This function uses the function ReceiveByte() to get a byte
* from the UART.
* - If the received byte is equal to '\n' (Line Feed),
* the function returns.
* - The array is terminated with ?\0?.
*/

void USART_ReceiveString(char *str)
{
	uint8_t t = 0;

	while ((str[t] = USART_RxChar()) != '\n')
	{
		t++;
	}
	str[t] = '\0';
}

void USART_SendString(char *str)									/* Send string of USART data function */
{
	int i=0;
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);										/* Send each char of string till the NULL */
		i++;
	}
}

void USART_BluetoothChanger(char Data_in[BL_BUFFERSIZE]){			/* When certain values are received the car does something*/
	
	if(strcmp(Data_in, BL_LED_ON) == 0)
	{
		/* Turn ON LED */
		USART_SendString("LE4D_ON");								/* send status of LED i.e. LED ON */
		
	}
	else if(strcmp(Data_in, BL_LED_OFF) == 0)
	{
		/* Turn OFF LED */
		USART_SendString("LED4_OFF"); 								/* send status of LED i.e. LED OFF */
	}
	else if(strcmp(Data_in, BL_FORWARD) == 0)
	{
		/* Turn OFF LED */
		USART_SendString("Driving forwards"); 						/* send status of LED i.e. LED OFF */
	}
	else if(strcmp(Data_in, BL_BACKWARD) == 0)
	{
		USART_SendString("Driving backwards"); 						/* send status of LED i.e. LED OFF */
	}
	else if(strcmp(Data_in, BL_LEFT) == 0)
	{
		USART_SendString("Steering Left"); 							/* send status of LED i.e. LED OFF */
	}
	else if(strcmp(Data_in, BL_RIGHT) == 0)
	{
		USART_SendString("Steering Right"); 						/* send status of LED i.e. LED OFF */
	}
	else if(strcmp(Data_in, BL_ModeUp) == 0)
	{
		USART_SendString("Mode Up"); 						/* send status of LED i.e. LED OFF */
		*(Settings + 0) += 1;
	}
	else if(strcmp(Data_in, BL_ModeDown) == 0)
	{
		USART_SendString("Mode down"); 						/* send status of LED i.e. LED OFF */
		*(Settings + 0) -= 1;
	}
	else{
		char buffer_tmp[BL_BUFFERSIZE] = "RX:> ";
		strcat(buffer_tmp, Data_in);
		strcat(buffer_tmp, "\n");
		USART_SendString(buffer_tmp);
		strcpy(buffer_tmp, "");
	}
	
}