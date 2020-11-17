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
#define _HIGH PORTB |= (1<<PORTB5)
#define _LOW PORTB &= ~(1<<PORTB5)
char TIME_1 = 0;
char freq = 50;
char devide = 0;

ISR(TIMER1_COMPA_vect)
{
	TIME_1 += 1;
}
void Timer_Frequency(uint8_t freq)
{
	// Initialize Timer 1:
	// - 128 prescaler
	// - CTC mode of operation
	TCCR1B |= (1<<CS11) | (1<<WGM12);

	// Enable output compare match interrupt for channel A
	TIMSK1 |= (1<<OCIE1A);

	// Set output compare value for channel A
	// OCRnA = (F_CPU / (frequency * 2 * N)) - 1
	OCR1A = (F_CPU / (freq * 2 * 256)) - 1;
}
int main(void)
{
	DDRB |= (1<<DDB5);
	DDRB &= ~(1<<DDB7);
	Timer_Frequency(62);
	
	sei();

	while(1)
	{
		if (TIME_1 == (50 - freq))
		{
			_HIGH;
		}
		if (TIME_1 == freq)
		{
			_LOW;
			TIME_1 = 0;
			devide++;
		}
		if (devide == 50)
		{
			freq--;
			devide = 0;
		}
		if (freq == 5) freq = 50;
	}
}

