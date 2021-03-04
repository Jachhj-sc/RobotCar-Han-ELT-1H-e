/*
 * pwm.h
 *
 * Created: 28-1-2021 12:53:19
 *  Author: willi
 */ 


#ifndef PWM_H_
#define PWM_H_
#include <stdbool.h>


void initPWM();
void PWM_digPin9_init(bool invert_pwm9);
void PWM_digPin9_Output(uint8_t duty);
void PWM_digPin10_init(bool invert_pwm10);
void PWM_digPin10_Output(uint8_t duty);

void SetDriveForward();
void SetDriveBackward();
void SpinRight();
void SpinLeft();

#endif /* PWM_H_ */