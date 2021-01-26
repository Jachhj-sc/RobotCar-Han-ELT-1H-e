/*
 * Project 1a IR sensor for black line V1.c
 *
 * Created: 10/21/2020 5:29:40 PM
 * Author : cyant
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>

#define F_CPU 16000000UL
#define LED1_ON PORTB |= (1 << PORTB2)
#define LED1_OFF PORTB &= ~(1 << PORTB2)
#define LED2_ON PORTB |= (1 << PORTB1)
#define LED2_OFF PORTB &= ~(1 << PORTB1)
#define LED3_ON PORTB |= (1 << PORTB0)
#define LED3_OFF PORTB &= ~(1 << PORTB0)






int main(void)
{
	//LED outputs to show if the sensor is sensing black or not
	DDRB |= (1 << DDB0);
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB4);
	
	//IR sensors as inputs
	DDRC &= ~(1<<DDC0); 
	DDRC &= ~(1<<DDC1); 
	DDRC &= ~(1<<DDC2); 
	DDRC &= ~(1<<DDC3);
	
	//setting pull-up for the iputs
	PORTC |= (1 << PORTC0);
	PORTC |= (1 << PORTC1);
	PORTC |= (1 << PORTC2);
	PORTC |= (1 << PORTC3);
	
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8);
	PCMSK1 |= (1 << PCINT9);
	PCMSK1 |= (1 << PCINT10);
	PCMSK1 |= (1 << PCINT11);
	
	sei();
	
   while (1) 
    {
		;
    }
}



ISR(PCINT1_vect)
{
	if(!(PINC & (1<<PINC0)) && (PINC & (1<<PINC1)) && (PINC & (1<<PINC2)) && (PINC & (1<<PINC3)))
	LED3_ON;
	else
	{
		LED3_OFF;
	}
	if(!(PINC & (1<<PINC0)) && !(PINC & (1<<PINC1)) && (PINC & (1<<PINC2)) && (PINC & (1<<PINC3)))
	LED3_ON;
	else
	{
		LED3_OFF;
	}
	
	if((PINC & (1<<PINC0)) && !(PINC & (1<<PINC1)) && !(PINC & (1<<PINC2)) && (PINC & (1<<PINC3)))
	LED2_ON;
	else
	{
		LED2_OFF;
	}
	if((PINC & (1<<PINC0)) && (PINC & (1<<PINC1)) && !(PINC & (1<<PINC2)) && !(PINC & (1<<PINC3)))
	LED1_ON;
	else
	{
		LED1_OFF;
	}
	if((PINC & (1<<PINC0)) && (PINC & (1<<PINC1)) && (PINC & (1<<PINC2)) && !(PINC & (1<<PINC3)))
	LED1_ON;
	else
	{
		LED1_OFF;
	}
}