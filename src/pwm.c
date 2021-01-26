/*
 * PWM_Robotcar_V1.2.c
 *
 * Created: 5-1-2021 11:01:42
 * Author : 31613
 */

#include <avr/io.h>
#include <stdbool.h>

#define true 1
#define false 0


// - - - DIGITAL PIN 9 PWM (PB1-OC1A) - - - //
void PWM_digPin9_init(bool invert_pwm9)
{
    if (invert_pwm9 == 1)
    {
        TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1A0);
    }

    else {
        TCCR1A |= (1 << WGM10) | (1 << COM1A1);
    }

    TCCR1B |= (1 << WGM12) | (1 << CS12) ;
    DDRB |= (1 << PINB1)    ;

}

void PWM_digPin9_Output(uint16_t duty)
{
    OCR1A = duty;
}

// - - - DIGITAL PIN 10 PWM (PB2-OC1B) - - - //
void PWM_digPin10_init(bool invert_pwm10)
{
    if (invert_pwm10 == 1)
    {
        TCCR1A |= (1 << WGM10) | (1 << COM1B1) | (1 << COM1B0);

    }

    else {
        TCCR1A |= (1 << WGM10) | (1 << COM1B1);
    }

    TCCR1B |= (1 << WGM12) | (1 << CS12) ;
    DDRB |= (1 << PINB2)  ;

}
void PWM_digPin10_Output(uint16_t duty)
{
    OCR1B = duty;
}

void PWM_digPin9_10_init(bool invert_pwm9, bool invert_pwm10)
{
    PWM_digPin9_init(invert_pwm9);
    PWM_digPin10_init(invert_pwm10);
}

int main(void) {
    PWM_digPin9_10_init(false, false);
    DDRB |= (1 << PINB0) | (1 << PINB4);

    while(1) {

        PWM_digPin9_Output(0xFF);        // PB1

        PWM_digPin10_Output(0x0F);        // PB2

    }
    return;
}
