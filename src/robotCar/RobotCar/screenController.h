/*
 * screenController.h
 *
 * Created: 16-12-2020 11:19:42
 *  Author: william
 
	this code is made for the Atmega328P and 128 * 64 0,96 in. i2c screen.
 */ 

// #define F_CPU 16000000UL
// #define AVR_USE_HW_I2C // this lets the OLED library know what type of functions to use.
//please define them in project property objects

#ifndef SCREENCONTROLLER_H_
#define SCREENCONTROLLER_H_

#include "U8G2/u8g2.h"
#include "U8G2/u8x8.h"
#include "U8G2/u8x8_avr.h"

void sys_init(void);
int8_t * u8g2_setup(int *pSettings, int *pSpeed, int *pDirection, int *pDistance);
void screenRoutine(void);
void choiceAdder(int8_t addValue);
void pageSel(void);
void draw(void);
//Buttons-----------------------------------------------------------------
void but_init(void);
void updateButtons(void);
#define BUTTON_PC_VECT PCINT0_vect
#define BUTTON_PIN	PINB
#define BUTTON_PORT PORTB
#define BUTTON_REG	DDRB

#define SELECT PINB3
#define BACK PINB2
#define UP PINB1
#define DOWN PINB0

#define DebounceTime 50 //ms

int keyPressed;

//Pages-------------------------------------------------------------------
void MenuStyle0(void);
void MenuStyle1(void);

void StartPage(void);

void nopage(void);
void ScreenOverlay(void);
void Page_1(void);
void Page_2(void);
void Page_3(void);
void Page_4(void);
void Page_5(void);

//Time________________________________________________________________________________________________________________________________________________

void getTimeToChar(char *s, int choice);
void timeInit(unsigned int *ptime_min, unsigned int *ptime_ms);

//Battery_____________________________________________________________________________________________________________________________________________

int updateBatterySens();
void ADCinit(void);
#endif /* SCREENCONTROLLER_H_ */