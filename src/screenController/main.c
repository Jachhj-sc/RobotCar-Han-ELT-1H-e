/*
* main.c
*
* Created: 22-10-2020 14:31:57
* Author: William
*	this code is made for the Atmega328P
*/

#define F_CPU 16000000UL
#include "U8G/u8g.h"
//#include "distanceSensor/distanceSensor"

#if defined(__AVR__)
#include <avr/interrupt.h>
#include <avr/io.h>
#endif

#include <string.h>
#include <util/delay.h>
#include <stdlib.h>

#define screenWidth 128
#define screenHeight 64
#define widthCurChoice 128
#define widthChoice 128/2
#define Margin 2

#define MENU 1


#define SELECT 7
#define BACK 6
#define UP 5
#define DOWN 4

#define DebounceTime 50 //ms

#define menuChoices 4
char *menustrings[menuChoices] = {"Data", "Modes", "Something", "Settings"};

#define MenuAnim 1 // select menu animation
int framesNum = 2; // number of frames in the animation for selecting a new choice value higher than 0

#define MenuRounding 2

u8g_t u8g;

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

void u8g_setup(void)
{
	/*
	SCL: Port C, Bit 5
	SDA: Port C, Bit 4
	*/
	/* activate pull-up, set ports to output, init U8glib */
	u8g_SetPinInput(PN(2,5)); u8g_SetPinLevel(PN(2,5), 1); u8g_SetPinOutput(PN(2,5));
	u8g_SetPinInput(PN(2,4)); u8g_SetPinLevel(PN(2,4), 1); u8g_SetPinOutput(PN(2,4));
	u8g_InitI2C(&u8g, &u8g_dev_ssd1306_128x64_i2c, U8G_I2C_OPT_NONE);

	/* assign default color value */
	if ( u8g_GetMode(&u8g) == U8G_MODE_R3G3B2 )
	u8g_SetColorIndex(&u8g, 255);     /* white */
	else if ( u8g_GetMode(&u8g) == U8G_MODE_GRAY2BIT )
	u8g_SetColorIndex(&u8g, 3);         /* max intensity */
	else if ( u8g_GetMode(&u8g) == U8G_MODE_BW )
	u8g_SetColorIndex(&u8g, 1);         /* pixel on */
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
	sei();//set enable interrupts
}

_Bool reDrawRequired = 1;
int main()
{
	//screen
	u8g_setup();
	sys_init();
	but_init();
	u8g_Begin(&u8g);
	
	u8g_SetFont(&u8g, u8g_font_5x7r);
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
		currentPage = MENU;
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
		currentPage = 1;
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
	if(currentPage == MENU){
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
	}
}

void MenuAnim0(void){
	//add scroll instead off a proportional menu
	
	// menu
	for (int currentFrame = 0; currentFrame <= framesNum; currentFrame++){
		u8g_FirstPage(&u8g);
		do{
			for (int i = 0; i < menuChoices; i++){
				if(currentChoice == i){
					//x and the y of the boxes
					int width = (currentFrame *((widthCurChoice - widthChoice)/framesNum))+widthChoice;
					int height = (screenHeight / menuChoices) - Margin;
					
					int x = (screenWidth/2) - width / 2;
					int y = (i) * (screenHeight / menuChoices);
					
					u8g_SetDefaultForegroundColor(&u8g);
					u8g_DrawRBox(&u8g, x, y, width, height, MenuRounding);
					
					
					u8g_SetDefaultBackgroundColor(&u8g);
					u8g_DrawStr(&u8g, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+(height/2)+3, menustrings[i]);
					
					}else{
					//x and the y of the boxes
					int x = (screenWidth/2)-widthChoice/2;
					int y = (i) * (screenHeight / menuChoices);
					int width = widthChoice;
					int height = (screenHeight / menuChoices) - Margin;
					
					u8g_SetDefaultForegroundColor(&u8g);
					u8g_DrawRFrame(&u8g, x, y, width, height, MenuRounding);
					
					u8g_DrawStr(&u8g, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+(height/2)+3, menustrings[i]);
				}
			}
		}while(u8g_NextPage(&u8g));
	}
	reDrawRequired = 0;
}

void MenuAnim1(void){
	u8g_FirstPage(&u8g);
	do{
		for (int i = 0; i < menuChoices; i++){
			if(currentChoice == i){
				//x and the y of the boxes
				int width = screenWidth;
				int height = (screenHeight / menuChoices) - Margin;
				
				int x = (screenWidth/2) - width / 2;
				int y = (i) * (screenHeight / menuChoices);
				
				u8g_SetDefaultForegroundColor(&u8g);
				u8g_DrawRBox(&u8g, x, y, width, height, MenuRounding);
				
				
				u8g_SetDefaultBackgroundColor(&u8g);
				u8g_DrawStr(&u8g, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+(height/2)+3, menustrings[i]);
				
				}else{
				//x and the y of the boxes
				int width = screenWidth;
				int height = (screenHeight / menuChoices) - Margin;
				int x = (screenWidth/2) - width / 2;
				int y = (i) * (screenHeight / menuChoices);
				
				
				u8g_SetDefaultForegroundColor(&u8g);
				u8g_DrawRFrame(&u8g, x, y, width, height, MenuRounding);
				
				u8g_DrawStr(&u8g, x + (width/2)-(5*(strlen(menustrings[i])/2)+1), y+(height/2)+3, menustrings[i]);
			}
		}
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}

void StartAnim(void){
	textHeight = u8g_font_GetFontAscent(u8g_font_5x7r)+2;
	u8g_FirstPage(&u8g);
	do{
		u8g_DrawStr(&u8g, 0, textHeight, "SPECIAL THANKS TO :");
		u8g_DrawStr(&u8g, 0, textHeight*2, "Bram , William, Antonis,");
		u8g_DrawStr(&u8g, 0, textHeight*3, "Corne, Yasmine, Adil");
		u8g_DrawStr(&u8g, screenWidth/4, screenHeight-textHeight*2, "Press \"Select\"");
		u8g_DrawStr(&u8g, screenWidth/4, screenHeight-textHeight, " to continue!");
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}

void nopage(void){
	u8g_FirstPage(&u8g);
	do{
		u8g_DrawStr(&u8g, 0, textHeight, "This Page has no content!");
		
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}

void page_2(void){
	#define textnum 4
	#define diglength 3
	char *text[textnum] = {
		" ",
		"Speed:>  ",
		"Direction:> ",
		"objectDistance:> "
	};
	int speed = 50;
	char Spd[diglength];
	itoa(speed, Spd, 10);
	
	int direction = 180;
	char Dir[diglength];
	itoa(direction, Dir, 10);
	
	volatile char _void[] = "";//this is here to show the library that the previous array stops
	
	int distance = 25;
	char Dist[diglength];
	itoa(distance, Dist, 10);
	
	#define xDat 80
	u8g_FirstPage(&u8g);
	do{
		//drawstring x strlen(text[1])*5
		//title
		u8g_DrawStr(&u8g, 0, textHeight, menustrings[currentChoice]);
		//speed
		u8g_DrawStr(&u8g, 0, textHeight*3, text[1]);
		u8g_DrawStr(&u8g, xDat, textHeight*3,Spd);
		u8g_DrawStr(&u8g,xDat + diglength*5 + 1, textHeight*3, "km/h");
		//direction
		u8g_DrawStr(&u8g, 0, textHeight*4, text[2]);
		u8g_DrawStr(&u8g, xDat, textHeight*4, Dir);
		u8g_DrawStr(&u8g, xDat + diglength*5 + 1, textHeight*4, "deg");
		//distance
		u8g_DrawStr(&u8g, 0, textHeight*5, text[3]);
		u8g_DrawStr(&u8g, xDat, textHeight*5, Dist);
		u8g_DrawStr(&u8g, xDat + diglength*5 + 1, textHeight*5, "cm");
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}

void page_3(void){
	u8g_FirstPage(&u8g);
	do{
		u8g_DrawStr(&u8g, 0, textHeight, menustrings[currentChoice]);
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}

void page_4(void){
	u8g_FirstPage(&u8g);
	do{
		u8g_DrawStr(&u8g, 0, textHeight, menustrings[currentChoice]);
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}

void page_5(void){
	u8g_FirstPage(&u8g);
	do{
		u8g_DrawStr(&u8g, 0, textHeight, menustrings[currentChoice]);
		u8g_DrawStr(&u8g, 0, textHeight*2, "test");
	}while(u8g_NextPage(&u8g));
	reDrawRequired = 0;
}