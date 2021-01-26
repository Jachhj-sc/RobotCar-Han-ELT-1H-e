/*
* main.c
*
* Created: 22-10-2020 14:31:57
* Author: William
* this code is made for the Atmega328P and 128 * 64 0,96 in. i2c screen.
*/
//OLED u8g2 library
#include "U8G2/u8g2.h"
#include "U8G2/u8x8.h"
#include "U8Gi2cLib/u8x8_avr.h"

//#include "distanceSensor/distanceSensor"


#if defined(__AVR__)
#include <avr/interrupt.h>
#include <avr/io.h>
#endif

#include <string.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>

// #define F_CPU 16000000UL
// #define AVR_USE_HW_I2C // this lets the OLED library know what type of functions to use.
//please define them in project property objects

#define SSD1306_ADDR  0x78

#define LED_TOGGLE PORTB ^= (1<<PORTB5)

#define TopMargin 9
#define screenWidth 128
#define realScreenHeight 64
#define screenHeight realScreenHeight - TopMargin
#define widthCurChoice 128
#define widthChoice 128/2
#define Margin 2

#define MENUPAGE 1



#define SELECT 7
#define BACK 6
#define UP 5
#define DOWN 4

#define DebounceTime 50 //ms

#define menuChoicesVis 3
#define menuChoices 5
char *menustrings[menuChoices] = {"Data", "Modes", "Compass", "Settings", "yeahh test"};

short int MenuAnim = 1; // select menu animation
#define framesNum 3 -1 // number of frames in the animation for selecting a new choice value higher than 0
#define MenuBoxRounding 3
int menuScroll = 0;

short Mode = 0;//setting for the mode of the car.

u8g2_t u8g2;

void draw(void);
void update(void);
void menuChoiceAd(int adder);
void pageSel();
void MenuAnim0(void);
void MenuAnim1(void);
void StartAnim(void);

void nopage(void);
void ScreenOverlay(void);
void page_2(void);
void page_3(void);
void page_4(void);
void page_5(void);

void getTime(char *s, int choice);
/*
Write the time as a string to the given string pointer
*/

int currentChoice = 0;
unsigned int currentFrame = 0;
int keyPressed;
int currentPage = 0;



unsigned short textHeight;
unsigned short maxCharWidth;
double screenDiv = screenHeight;
double screenDivholder = menuChoicesVis;

typedef u8g2_uint_t u8g_uint_t;

unsigned int Time_ms = 0;
unsigned int Time_min = 0;
#define Time_cal 305 //ms calibration

void u8g2_setup(void)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_avr_delay);
	u8g2_SetI2CAddress(&u8g2, SSD1306_ADDR);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	
	screenDiv = screenDiv / screenDivholder;//for better resolution

	u8g2_SetFontDirection(&u8g2, 0);//0: 0deg 1:90deg 2: 180deg 3: 270deg
}

void sys_init(void)
{
	#if defined(__AVR__)
	/* select minimal prescaler (max system speed) */
	CLKPR = 0x80;
	CLKPR = 0x00;
	#endif
}

//initialize and setup the buttons / pins used for them
void but_init(void){
	DDRB &= ~((1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK));     // set pinD 4 5 6 and 7 as an input
	// is now an input
	PORTD |= (1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK);    // turn On the Pull-up on all those pins
	// is now an input with pull-up enabled

	//interrupt setup
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK);//enable the pin change interrupts on pinD 4,5,6,7
	_delay_ms(200);//to prevent accidental interrupt triggering
	
}

_Bool reDrawRequiredExt = 0;
ISR(TIMER0_COMPB_vect){
	static int rCount = 0;
	
	if (Time_ms >= 60000){
		Time_ms = Time_cal-117;//Formula for the minute CAl (Time_cal-c): c = 0.615384615*Time_cal
		Time_min++;
		rCount = 0;
		reDrawRequiredExt = 1; //update the screen so the overlay shows the right minutes.
	}else if (rCount++ >= 13000)
	{
		Time_ms = Time_ms + Time_cal;
		rCount = 0;
	}

	Time_ms++;
	
}

void timerInit(void){
	
	TCCR0B |= (1<<CS00) | (1<<CS01);
	TIMSK0 |= (1<<OCIE0B);
	OCR0B = 250;//to attain 1 ms for every overflow.
}

_Bool reDrawRequired = 1;
int main()
{
	//screen
	u8g2_setup();
	sys_init();
	but_init();
	timerInit();
	sei();//set enable interrupts
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
	textHeight = u8g2_GetMaxCharHeight(&u8g2);
	maxCharWidth = u8g2_GetMaxCharWidth(&u8g2);
	//counter_init();
	//StartAnim();

	while(1){
		if (reDrawRequired || reDrawRequiredExt)
		{
			reDrawRequired = 1;
			u8g2_ClearBuffer(&u8g2);
			draw();
			u8g2_SendBuffer(&u8g2);
			reDrawRequiredExt = 0;
		}
		update();
	}
}

void draw(void){
	switch(currentPage){
		case 0:// page 0 start
		StartAnim();
		break;//end page0
		
		case 1://page 1 menu
		ScreenOverlay();//remove this if you don't want the overlay
		
		switch (MenuAnim)//for switching between menu animations
		{
			case 0://animation 0
			MenuAnim0();
			break;//end animation 0
			
			case 1://animation 1
			MenuAnim1();
			break;//end animation 1
		}
		break;//end page 1
		
		case 2:
		ScreenOverlay();
		page_2();
		break;

		case 3:
		ScreenOverlay();
		page_3();
		break;
		
		case 4:
		ScreenOverlay();
		page_4();
		break;
		
		case 5:
		ScreenOverlay();
		page_5();
		break;
		
		default:
		nopage();
		break;
		
	}
}

//the routine that gets run when the pin change interrupt gets triggered on portD for the enabled pins
ISR (PCINT2_vect)
{
	if ( (PIND & (1<<PIND4)) == 0 )
	keyPressed = 4;
	else
	if ( (PIND & (1<<PIND5)) == 0 )
	keyPressed = 5;
	else
	if ( (PIND & (1<<PIND6)) == 0 )
	keyPressed = 6;
	else
	if ( (PIND & (1<<PIND7)) == 0 )
	keyPressed = 7;

}

void update(void){
	switch (keyPressed)
	{
		case BACK:
		currentPage = MENUPAGE;
		reDrawRequired = 1;
		keyPressed = 0;//reset key pressed
		break;
		
		case SELECT:
		pageSel();
		reDrawRequired = 1;
		keyPressed = 0;
		break;
		
		case UP:
		menuChoiceAd(-1);
		reDrawRequired = 1;
		keyPressed = 0;
		break;
		
		case DOWN:
		menuChoiceAd(+1);
		reDrawRequired = 1;
		keyPressed = 0;
		break;
	}
}

void menuChoiceAd(int addValue){ //change the selection in the menu
	if(currentPage == MENUPAGE){
		currentChoice += addValue;
		
		if (currentChoice > menuChoices-1)
		{
			currentChoice = 0;
			menuScroll = 0;
		}
		if (currentChoice < 0)
		{
			currentChoice = menuChoices-1;
			menuScroll = menuChoicesVis - menuChoices;
		}


		if(currentChoice >= menuChoicesVis - menuScroll){
			menuScroll += -addValue;
		}
		if (currentChoice < -menuScroll){
			menuScroll += -addValue;
		}
	}
}

void pageSel(void){
	#define firstPage 2
	//code for managing the page
	if (currentPage == 1){
		currentPage = currentChoice+firstPage; //increment with two because the menu choice pages start at 2
		}else if(currentPage == 0){
		currentPage = MENUPAGE;
	}
}


void MenuAnim0(void){
	//int yStat = (y+(height/2)+3);
	int height;
	int width;
	int x;
	int y;
	
	// menu
	for (int i = 0; i < menuChoices+1; i++){
		if(i == currentChoice){
			//x and the y of the boxes
			width = ((currentFrame+1) *(widthCurChoice - widthChoice))/framesNum+widthChoice;
			height = screenDiv - Margin;
			
			x = (screenWidth/2) - width / 2;
			y = i * screenDiv + TopMargin + (menuScroll * screenDiv);
			if (y >= TopMargin)
			{
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRBox(&u8g2, x, y, width, height, MenuBoxRounding);

				u8g2_SetDrawColor(&u8g2, 0);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menustrings[i]) / 2 , y+(height/2)+3, menustrings[i]);
			}
			
			}else{
			//x and the y of the boxes
			
			width = widthChoice;
			height = screenDiv - Margin;
			x = (screenWidth/2)-widthChoice/2;
			y = i * screenDiv + TopMargin + (menuScroll * screenDiv);
			if (y >= TopMargin)
			{
				//u8g2_SetDefaultForegroundColor(&u8g2);
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRFrame(&u8g2, x, y, width, height, MenuBoxRounding);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menustrings[i]) / 2, y+(height/2)+3, menustrings[i]);
			}
		}
	}
	
	//stop drawing when the animation is done
	if (currentFrame++ >= framesNum)
	{
		currentFrame = 0;
		reDrawRequired = 0;
	}
	

}

void MenuAnim1(void){
	int width = screenWidth;
	int height = screenDiv;
	
	int x = ((screenWidth/2) - width / 2);
	int y;
	int yStat = (height/2)+3;
	height = height - Margin;
	
	for (int i = 0; i < menuChoices+1; i++){//do one extra to dirty fix lib error. make one menu extra outside scope.
		if(currentChoice == i){
			//x and the y of the boxes
			//width = screenWidth;
			//height = screenDiv - Margin;
			
			// 				x = (screenWidth/2) - width / 2;
			y = i * screenDiv + TopMargin + (menuScroll * screenDiv);
		/*	if (menuScroll == -1){
				y -= 2;
			}*/
			
			if (y >= TopMargin)
			{
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRBox(&u8g2, x, y, width, height, MenuBoxRounding);

				u8g2_SetDrawColor(&u8g2, 0);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menustrings[i]) / 2, y+yStat, menustrings[i]);
			}
			}else{

			y = i * screenDiv + TopMargin + (menuScroll * screenDiv);
/*
			if (menuScroll == -1){
				y -= 2;
			}*/
			if (y >= TopMargin)
			{
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRFrame(&u8g2, x, y, width, height, MenuBoxRounding);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menustrings[i]) / 2, y+yStat, menustrings[i]);
			}
		}
	}
	reDrawRequired = 0;
}

void StartAnim(void){
	#define lineNum 4
	char *text[lineNum] = {"SPECIAL THANKS TO :", "Bram , William, ", "Antonis, Corne, ", "Yasmine, Adil"};
	
	for (int i = 0; i < lineNum; i++)
	{
		u8g2_DrawStr(&u8g2, 0, textHeight * (i + 1), text[i]);
	}
	
	char *text2[2] = {
		"Press \"Select\"",
		" to continue!"
	};
	
	u8g2_DrawStr(&u8g2, screenWidth/2 - u8g2_GetStrWidth(&u8g2, text2[0]) / 2, realScreenHeight-textHeight, text2[0]);
	u8g2_DrawStr(&u8g2, screenWidth/2 - u8g2_GetStrWidth(&u8g2, text2[1]) / 2, realScreenHeight, text2[1]);
	reDrawRequired = 0;
}

void nopage(void){
	u8g2_DrawStr(&u8g2, 0, textHeight + TopMargin, "No content yet!");
	
	reDrawRequired = 0;
}

void ScreenOverlay(void){
	//this is for screen overlay
	
	//battery icon
	int height1 = TopMargin-1;
	int width1 = 15;
	int x1 = screenWidth/2 - width1/2; //use this for the place
	int y1 = 0;
	
	int height2 = height1/2;
	int width2 = 3;
	int x2 = x1 + width1 - 1;
	int y2 = height2/2;
	
	int width3 = width1/3 - 2;
	int height3 = height1 - 4;
	
	u8g2_DrawFrame(&u8g2, x1, y1, width1, height1);
	u8g2_DrawFrame(&u8g2, x2, y2, width2, height2);
	
	int chargeBars = 3;
	for (int i = 0 ; i < chargeBars; i++)
	{
		u8g2_DrawBox(&u8g2, x1 + 2 + (width3+1) * i, y1 + 2, width3, height3);
	}
	
	
	
	//Mode:
	char modeTxt[7] = "Mode: ";
	char modeTxthold[1];
	itoa(Mode, modeTxthold, 10);
	strcat(modeTxt, modeTxthold);
	
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	u8g2_DrawStr(&u8g2, screenWidth - u8g2_GetStrWidth(&u8g2, modeTxt), textHeight-3, modeTxt);
	
	
	//Time
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	char timeTxt[7];
	getTime(timeTxt, 0);
	
	u8g2_DrawStr(&u8g2, 0, textHeight-3, timeTxt);
	
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);//reset font to the main font.
}

void page_2(void){
	#define Lines 4
	#define digAmount 8
	
	#define startLine_Data 3

	char *text2[Lines] = {
		"Speed:>  ",
		"Direction:> ",
		"Distance:> ",
		"runTime:> "
	};
	
	int maxPrefixWidth = u8g2_GetStrWidth(&u8g2, text2[1]);
	
	char *postFixes[Lines] = {
		"km/h",
		"deg",
		"cm",
		""
	};
	
	//placeholders
	char d1[digAmount];
	char d2[digAmount];
	char d3[digAmount];
	char d4[digAmount];
	char *Data[Lines] = { d1, d2, d3, d4};
	
	int speed = 50;
	itoa(speed, Data[0], 10);
	
	int direction = 180;
	itoa(direction, Data[1], 10);
	
	int distance = 25;
	itoa(distance, Data[2], 10);
	
	//little bit of code to make a typical digital watch look in the time String
	getTime(Data[3], 1);
	
	
	
	//title
	u8g2_DrawStr(&u8g2, 0, textHeight + TopMargin, menustrings[currentChoice]);

	for (int i = 0; i < Lines; i++)
	{
		int dataWidth = u8g2_GetStrWidth(&u8g2, Data[i]);
		u8g2_DrawStr(&u8g2, 0, textHeight*(i+startLine_Data), text2[i]);
		u8g2_DrawStr(&u8g2, maxPrefixWidth, textHeight*(i+startLine_Data), Data[i]);
		u8g2_DrawStr(&u8g2, maxPrefixWidth + dataWidth + maxCharWidth/2, textHeight*(i+startLine_Data), postFixes[i]);
	}
	
	reDrawRequired = 1;
}

void page_3(void){
	u8g2_DrawStr(&u8g2, 0, textHeight+ TopMargin, menustrings[currentChoice]);
	reDrawRequired = 0;
}

void page_4(void){
	u8g2_DrawStr(&u8g2, 0, textHeight+ TopMargin, menustrings[currentChoice]);

	reDrawRequired = 0;
}

void page_5(void){
	u8g2_DrawStr(&u8g2, 0, textHeight+ TopMargin, menustrings[currentChoice]);
	
	reDrawRequired = 0;
}

void getTime(char *s, int choice){
	/*choice */
	static char Time_s_hold[3] = "00";
	itoa(Time_min, s, 10);
	itoa((Time_ms/1000), Time_s_hold, 10);
	strcat(s, ":");
	
	switch (choice){
		case 1:
		strcat(s, Time_s_hold);
		break;
	}
	
}