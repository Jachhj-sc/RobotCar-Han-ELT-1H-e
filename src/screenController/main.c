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

// #define F_CPU 16000000UL
// #define AVR_USE_HW_I2C // this lets the OLED library know what type of functions to use.
//please define them in project property objects

#define SSD1306_ADDR  0x78

#define LED_TOGGLE PORTB ^= (1<<PORTB5)

#define screenWidth 128
#define screenHeight 64
#define widthCurChoice 128
#define widthChoice 128/2
#define Margin 2

#define MENUPAGE 1



#define SELECT 7
#define BACK 6
#define UP 5
#define DOWN 4

#define DebounceTime 50 //ms

#define menuChoices 4
char *menustrings[menuChoices] = {"Data", "Modes", "Compass", "Settings"};

short int MenuAnim = 1; // select menu animation
int framesNum = 2; // number of frames in the animation for selecting a new choice value higher than 0

#define MenuBoxRounding 3

u8g2_t u8g2;

void draw(void);
void update(void);
void menuChoiceAd(int adder);
void pageSel();
void MenuAnim0(void);
void MenuAnim1(void);
void StartAnim(void);

void nopage(void);
void page_2(void);
void page_3(void);
void page_4(void);
void page_5(void);

int currentChoice = 0;
unsigned int currentFrame = 0;
int keyPressed;
int currentPage = 0;
unsigned long prevTime;

int textHeight;
double screenDiv = screenHeight;
double screenDivholder = menuChoices;

typedef u8g2_uint_t u8g_uint_t;

void u8g2_setup(void)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_2(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_avr_delay);
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

int reDrawRequired = 1;
int main()
{
	//screen
	u8g2_setup();
	sys_init();
	but_init();
	//u8g2_Begin(&u8g2);
	
	sei();//set enable interrupts
	
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	//counter_init();
	//StartAnim();

	while(1){
		if (reDrawRequired)
		{
			draw();
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
		page_2();
		break;

	    case 3:
		page_3();
		break;
		
		case 4:
		page_4();
		break;
		
		case 5:
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
		keyPressed = 0;
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
		}
		if (currentChoice < 0)
		{
			currentChoice = menuChoices-1;
		}
	}
}

void pageSel(void){
	//code for managing the page
	if (currentPage == 1){
		currentPage = currentChoice+2; //increment with two because the menu choice pages start at 2
		}else if(currentPage == 0){
		currentPage = MENUPAGE;
	}
}

// int width = screenWidth;
// int height = screenDiv;
// 
// int x = ((screenWidth/2) - width / 2);
// int y;
// int yStat = (y+(height/2)+3);

void MenuAnim0(void){
	//add scroll instead off a proportional menu
	int width = screenWidth;
	int height = screenDiv;

	int x = ((screenWidth/2) - width / 2);
	int y;
	//int yStat = (y+(height/2)+3);
	height = height - Margin;
	// menu
	for (int currentFrame = 0; currentFrame <= framesNum; currentFrame++){
		u8g2_FirstPage(&u8g2);
		do{
			for (int i = 0; i < menuChoices+1; i++){
				if(i == currentChoice){
					//x and the y of the boxes
					width = (currentFrame *((widthCurChoice - widthChoice)/framesNum))+widthChoice;
					height = screenDiv - Margin;
					
					x = (screenWidth/2) - width / 2;
					y = i * screenDiv;
					
					u8g2_SetDrawColor(&u8g2, 1);
					u8g2_DrawRBox(&u8g2, x, y, width, height, MenuBoxRounding);

					u8g2_SetDrawColor(&u8g2, 0);
					u8g2_DrawStr(&u8g2, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+(height/2)+3, menustrings[i]);
					}else{
					//x and the y of the boxes
					x = (screenWidth/2)-widthChoice/2;
					y = i * screenDiv;
					width = widthChoice;
					height = screenDiv - Margin;
					
					//u8g2_SetDefaultForegroundColor(&u8g2);
					u8g2_SetDrawColor(&u8g2, 1);
					u8g2_DrawRFrame(&u8g2, x, y, width, height, MenuBoxRounding);
					u8g2_DrawStr(&u8g2, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+(height/2)+3, menustrings[i]);
				}
			}
		}while(u8g2_NextPage(&u8g2));
	}
	reDrawRequired = 0;
}

void MenuAnim1(void){
	int width = screenWidth;
	int height = screenDiv;

	int x = ((screenWidth/2) - width / 2);
	int y;
	int yStat = (height/2)+3;
	height = height - Margin;
	
	u8g2_FirstPage(&u8g2);
	do{
		for (int i = 0; i < menuChoices+1; i++){//do one extra to dirty fix lib error.
			if(currentChoice == i){
				//x and the y of the boxes
				//width = screenWidth;
				//height = screenDiv - Margin;
				
// 				x = (screenWidth/2) - width / 2;
 				y = i * screenDiv;

				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRBox(&u8g2, x, y, width, height, MenuBoxRounding);

				u8g2_SetDrawColor(&u8g2, 0);
				u8g2_DrawStr(&u8g2, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+yStat, menustrings[i]);
				}else{

// 				width = screenWidth;
// 				height = screenDiv - Margin;
// 				x = (screenWidth/2) - width / 2;
 				y = i * screenDiv;

				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRFrame(&u8g2, x, y, width, height, MenuBoxRounding);
				u8g2_DrawStr(&u8g2, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+yStat, menustrings[i]);
			}
		}
	}while(u8g2_NextPage(&u8g2));
	reDrawRequired = 0;
}

void StartAnim(void){
	
	textHeight = 6+2;
	
	u8g2_FirstPage(&u8g2);
	do{
		u8g2_DrawStr(&u8g2, 0, textHeight, "SPECIAL THANKS TO :");
		u8g2_DrawStr(&u8g2, 0, textHeight*2, "Bram , William, Antonis,");
		u8g2_DrawStr(&u8g2, 0, textHeight*3, "Corne, Yasmine, Adil");
		u8g2_DrawStr(&u8g2, screenWidth/4, screenHeight-textHeight*2, "Press \"Select\"");
		u8g2_DrawStr(&u8g2, screenWidth/4, screenHeight-textHeight, " to continue!");
	}while(u8g2_NextPage(&u8g2));
	reDrawRequired = 0;
}

void nopage(void){
	u8g2_FirstPage(&u8g2);
	do{
		u8g2_DrawStr(&u8g2, 0, textHeight, "This Page has no content!");
		
	}while(u8g2_NextPage(&u8g2));
	reDrawRequired = 0;
}

void page_2(void){
	#define Lines 4
	#define digAmount 4
	#define xDat (12 * 5)
	#define yDat 3
	
	static char *text2[Lines] = {
		"Speed:>  ",
		"Direction:> ",
		"Distance:> ",
		"runTime:> "
	};
	
	static char *postFixes[Lines] = {
		"km/h",
		"deg",
		"cm",
		"s"
	};
	
	//placeholders
	static char d1[digAmount];
	static char d2[digAmount];
	static char d3[digAmount];
	static char d4[digAmount];
	static char *Data[Lines] = { d1, d2, d3, d4};
	
	static int speed = 50;
	itoa(speed, Data[0], 10);
	
	static int direction = 180;
	itoa(direction, Data[1], 10);
	
	static int distance = 25;
	itoa(distance, Data[2], 10);
	
	int runTime = 250;
	itoa(runTime, Data[3], 10);
	
	u8g2_FirstPage(&u8g2);
	do{
		//title
		u8g2_DrawStr(&u8g2, 0, textHeight, menustrings[currentChoice]);

		for (int i = 0; i < Lines; i++)
		{
			u8g2_DrawStr(&u8g2, 0,                      textHeight*(i+yDat), text2[i]);
			u8g2_DrawStr(&u8g2, xDat,                   textHeight*(i+yDat), Data[i]);
			u8g2_DrawStr(&u8g2, xDat + digAmount*5 + 1, textHeight*(i+yDat), postFixes[i]);
		}
		
	}while(u8g2_NextPage(&u8g2));
	
	reDrawRequired = 0;
}

void page_3(void){
	u8g2_FirstPage(&u8g2);
	do{
		u8g2_DrawStr(&u8g2, 0, textHeight, menustrings[currentChoice]);
	}while(u8g2_NextPage(&u8g2));
	reDrawRequired = 0;
}

void page_4(void){
	u8g2_FirstPage(&u8g2);
	do{
		u8g2_DrawStr(&u8g2, 0, textHeight, menustrings[currentChoice]);
	}while(u8g2_NextPage(&u8g2));
	reDrawRequired = 0;
}

void page_5(void){
	u8g2_FirstPage(&u8g2);
	do{
		u8g2_DrawStr(&u8g2, 0, textHeight, menustrings[currentChoice]);
		u8g2_DrawStr(&u8g2, 0, textHeight*2, "test");
	}while(u8g2_NextPage(&u8g2));
	reDrawRequired = 0;
}

//can i add two ISR for the same vector?
