 /*
 * motortjen20.c
 *
 * Created: 20-10-2020 16:26:15
 * Author : 31613
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL // 16 MHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define MOTORS02_ON PORTB |= (5<<PORTB0)  // Output for driving forwards SET HIGH
#define MOTORS13_ON PORTB |= (10<<PORTB0) // Output for driving backwards SET HIGH
#define MOTOR0_OFF PORTB &= ~(1<<PORTB0)  // SET LOW
#define MOTOR1_OFF PORTB &= ~(1<<PORTB1)  // SET LOW
#define MOTOR2_OFF PORTB &= ~(1<<PORTB2)  // SET LOW
#define MOTOR3_OFF PORTB &= ~(1<<PORTB3)  // SET LOW
char TIME_0 = 0;    // enter a value between 0 and 255
char TIME_1 = 255;  // enter a value between 0 and 255
char TIME_2 = 0;    // enter a value between 0 and 255
char TIME_3 = 0;    // enter a value between 0 and 255
unsigned char counter = 0;
unsigned char DIRECTION = 0; // Choose forward or backwards

ISR(TIMER1_COMPA_vect)
{
	counter += 1;
}
void Timer_Frequency(uint8_t freq)
{
	// Initialize Timer 1:
	// - 63 prescaler
	// - CTC mode of operation
	TCCR1B |= (1<<CS10) | (1<<WGM12);

	// Enable output compare match interrupt for channel A
	TIMSK1 |= (1<<OCIE1A);

	// Set output compare value for channel A
	// OCRnA = (F_CPU / (frequency * 2 * N)) - 1
	OCR1A = (F_CPU / (freq * 2 * 256)) - 1;
}
int main(void)
{
	// Set in and outputs
	DDRB |= (15<<DDB0); // Set pinB0 to pinB3 as an output
//	DDRB &= ~(1<<DDB7); // Set pinB7 as an input
	Timer_Frequency(63); // frequency is 128Hz
	
	sei();

	while(1)
	{
		//Set the time HIGH for forward motor
		if ((counter == 0) && (DIRECTION == 1))
		{
			MOTORS02_ON;
			if (TIME_0 == 0) MOTOR0_OFF;
			if (TIME_2 == 0) MOTOR2_OFF;
		}
		
		//Set the time HIGH for backwards motor
		if ((counter == 0) && (DIRECTION == 0))
		{
			MOTORS13_ON;
			if (TIME_1 == 0) MOTOR1_OFF;
			if (TIME_3 == 0) MOTOR3_OFF;						
		}
		
		//Set the time low for motor0
		if (TIME_0 <= counter)
		{
			MOTOR0_OFF;
		}
		
		//Set the time low for motor1
		if (TIME_1 <= counter)
		{
			MOTOR1_OFF;
		}
		
		//Set the time low for motor2
		if (TIME_2 <= counter)
		{
			MOTOR2_OFF;
		}
		
		//Set the time low for motor3
		if (TIME_3 <= counter)
		{
			MOTOR3_OFF;
		}	
			
		// limitation of the variable counter
		if (counter == 255){
			counter = 0;
		}
	}
}
