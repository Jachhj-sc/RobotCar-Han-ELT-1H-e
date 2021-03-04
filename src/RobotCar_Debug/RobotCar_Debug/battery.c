/*
* battery.c
*
* Created: 30-1-2021 15:25:19
*  Author: willi
*/
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>


int getBatteryADC(){
	volatile long Voltage = 0;
	
	Voltage = (long) ADC;    // defined as channel 5 on my board

	Voltage = (Voltage * 500)/1024;
	Voltage = Voltage * 20;        //using a resistor divider
	return (int)Voltage;
}


//Battery_____________________________________________________________________________________________________________________________________________
void ADCinit(void){
	ADMUX |= (1<<REFS0) | (1<<MUX1);//enable pin PC2
	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	//sei
	
}