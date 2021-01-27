/*
* screenController.c
*
* Created: 16-12-2020 14:28:37
*  Author: william
*/
#define F_CPU 16000000UL

#include "screenController.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


#define SSD1306_ADDR  0x78
u8g2_t u8g2;
typedef u8g2_uint_t u8g_uint_t;
_Bool reDrawRequired = 1;
_Bool reDrawRequiredTime = 0;

#define MENUPAGE 1
#define menuChoices 4
#define menuChoicesVis 3

#define SETTINGPAGE 5
#define settings 5
#define settingsVis 5

#define PAGES SETTINGPAGE + settings //assuming that settingPage is the last page plus the settings in the settingspage

int8_t currentPage = 0;
int8_t currentChoice[PAGES+settings];//the settings use the selections as an increment
int8_t Scroll[PAGES];
int8_t selections[PAGES+settings]; // menu choices , settings
int8_t selectionsVis[PAGES];

int *speedHold;
int *directionHold;
int *distanceHold;
int *angleHold;

//defined in the init
int8_t pageHistory(_Bool GetPrevpage, int pageNum);
_Bool selectEnable;


//amount of menu choices on one page. this directly affects the size of the option bars.

char *menuStrings[menuChoices] = {
	"Data",
	"Angle",
	"Compass",
	"Settings"
};

//#define settingsVis 2//amount of settings on one page. this directly affects the size of the option bars.

char *settingStrings[settings] = {
	"Mode:>      ",
	"Something:> ",
	"test1:>     ",
	"test2:>     ",
	"test3:>     "
};
//variable for storing the settings.
int *settingVal;//mode and something. pointer holder for the settings array in main.c

unsigned int currentFrame = 0;

uint8_t MenuAnim = 1; // select menu animation
#define framesNum 3 -1 // number of frames in the animation for selecting a new choice animation; value higher than 0
#define MenuBoxRounding 3



#define TopMargin 9
#define screenWidth 128
#define realScreenHeight 64
#define screenHeight realScreenHeight - TopMargin
#define widthCurChoice 128
#define widthChoice 128/2
#define Margin 2

uint8_t textHeight;//unsigned short
uint8_t maxCharWidth;
double screenDiv = screenHeight;
double screenDivholder = menuChoicesVis;

int batteryBars = 0;


//Time
unsigned int _Time_ms;
unsigned int _Time_min;

unsigned int *pTime_ms;
unsigned int *pTime_min;

#define Time_cal 305 //ms calibration

int8_t * u8g2_setup(int pSettings[settings], int *pSpeed, int *pDirection, int *pDistance, int *pangdeg)
{
	settingVal = &pSettings[0];
	angleHold = pangdeg;
	speedHold = pSpeed;
	directionHold = pDirection;
	distanceHold = pDistance;
	
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_avr_delay);
	u8g2_SetI2CAddress(&u8g2, SSD1306_ADDR);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	
	screenDiv = screenDiv / screenDivholder;//for better resolution

	u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
	textHeight = u8g2_GetMaxCharHeight(&u8g2);
	maxCharWidth = u8g2_GetMaxCharWidth(&u8g2);
	
	u8g2_SetFontDirection(&u8g2, 0);//0: 0deg 1:90deg 2: 180deg 3: 270deg
	
	selections[MENUPAGE] = menuChoices;
	selections[SETTINGPAGE] = settings;
	selectionsVis[MENUPAGE] = menuChoicesVis;
	selectionsVis[SETTINGPAGE] = settingsVis;
	return &currentPage;
}

void sys_init(void)
{
	#if defined(__AVR__)
	/* select minimal prescaler (max system speed) */
	CLKPR = 0x80;
	CLKPR = 0x00;
	#endif
}

//Time________________________________________________________________________________________________________________________________________________

void timeInit(unsigned int *ptime_min, unsigned int *ptime_ms){
	pTime_min = ptime_min;
	pTime_ms = ptime_ms;
	TCCR0B |= (1<<CS00) | (1<<CS01);
	TIMSK0 |= (1<<OCIE0B);
	OCR0B = 250;//to attain roughly 1 ms for every overflow. this roughness gets calibrated further in the code.
}



ISR(TIMER0_COMPB_vect){
	//this routine updates the time variables.
	static int rCount = 0;
	
	if (_Time_ms >= 60000){
		_Time_ms = Time_cal-117;//Formula for the minutes. pTime_ms = (Time_cal-c): c = 0.615384615*Time_cal
		_Time_min++;
		rCount = 0;
		reDrawRequiredTime = 1; //update the screen so the overlay shows the right minutes.
	}else if (rCount++ >= 13000)
	{
		_Time_ms = _Time_ms + Time_cal;
		rCount = 0;
	}

	_Time_ms++;
	
	*pTime_ms = _Time_ms;
	*pTime_min = _Time_min;
}

void getTimeToChar(char *s, int TemplateOption){
	//pTime_min and pTime_ms are global variables.
	strcpy(s, ""); // empty string s
	char Buff[3] = {0, 0, 0};
	
	itoa(_Time_min, Buff, 10);
	
	if(_Time_min < 10){
		strcat(s, "0");
	}
	strcat(s, Buff);
	
	itoa((_Time_ms/1000), Buff, 10);
	strcat(s, ":");
	
	switch (TemplateOption){
		case 0:
		//only have the minutes:> 00:
		break;
		
		case 1:
		//include minutes and seconds:> 00:00
		if(_Time_ms/1000 < 10){
			strcat(s, "0");
		}
		strcat(s, Buff);
		break;
	}
}


//Battery_____________________________________________________________________________________________________________________________________________
void ADCinit(void){
	ADMUX |= (1<<REFS0) | (1<<MUX1);
	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	//sei
	
}

int updateBatterySens(){
	static int ADCval = 0;
	ADCval = ADC;
	//mimicking a battery voltage by a pot meter.

	return ADCval;
}
//--------------------------------------
void screenRoutine(void){
	updateButtons();
	updateBatterySens();
	if (reDrawRequired || reDrawRequiredTime)
	{
		reDrawRequired = 1;
		
		u8g2_ClearBuffer(&u8g2);
		draw();
		u8g2_SendBuffer(&u8g2);
		
		reDrawRequiredTime = 0;
	}
}

void choiceAdder(int8_t addValue){ //change the selection in the menu
	currentChoice[currentPage] += addValue;
	
	if(selectEnable == 1){
		if (currentChoice[currentPage] > selections[currentPage]-1)
		{
			currentChoice[currentPage] = 0;
			Scroll[currentPage] = 0;
		}
		if (currentChoice[currentPage] < 0)
		{
			currentChoice[currentPage] = selections[currentPage]-1;
			Scroll[currentPage] = selectionsVis[currentPage] - selections[currentPage];
		}
		if(currentChoice[currentPage] >= selectionsVis[currentPage] - Scroll[currentPage]){
			Scroll[currentPage] += -addValue;
		}
		if (currentChoice[currentPage] < -Scroll[currentPage]){
			Scroll[currentPage] += -addValue;
		}
	}
}

void pageSel(void){
	//code for managing the pageNumbers
	#define firstPage 2
	if (selectEnable){
		if (currentPage == MENUPAGE){
			currentPage = currentChoice[MENUPAGE] + firstPage; //increment with two because the menu choice pages start at 2
			}else if(currentPage == 0){
			currentPage = MENUPAGE;
			}else{
			if(currentPage == 5){
				currentPage = currentChoice[5]+ 1 + 5;
			}
			
		}
		pageHistory(0, currentPage);//update page history
	}
}

int8_t pageHistory(_Bool GetPrevpage, int pageNum){
	#define historyBuff 4
	static short history[historyBuff];//initialize first page in case you don't use the start up screen.
	static short i = -1;
	
	if (!GetPrevpage)
	{
		if (i < historyBuff-1){
			history[++i] = pageNum;
		}
		return -1;
		}else{
		
		if (i <= 0)
		{
			return history[i];
			}else{
			return history[--i];
		}
	}
}

void draw(void){
	switch(currentPage){
		case 0:// page 0 start
		StartPage();
		break;//end page0
		
		case 1://page 1 menu
		ScreenOverlay();//remove this if you don't want the overlay
		
		switch (MenuAnim)//for switching between menu animations
		{
			case 0://animation 0
			MenuStyle0();
			break;//end animation 0
			
			case 1://animation 1
			MenuStyle1();
			break;//end animation 1
		}
		break;//end page 1
		
		case 2:
		ScreenOverlay();
		Page_2();
		break;

		case 3:
		ScreenOverlay();
		Page_3();
		break;
		
		case 4:
		ScreenOverlay();
		Page_4();
		break;
		
		default:
		if (currentPage >= SETTINGPAGE && currentPage <= SETTINGPAGE + settings){
			ScreenOverlay();
			Page_5();
			}else{
			nopage();
		}
		break;
		
	}
}
//Buttons_____________________________________________________________________________________________________________________________________________

//the routine that gets run when the pin change interrupt gets triggered on portD for the enabled pins


void but_init(void){
	BUTTON_REG &= ~((1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK));     // set pinD 4 5 6 and 7 as an input
	// is now an input
	BUTTON_PORT |= (1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK);    // turn On the Pull-up on all those pins
	// is now an input with pull-up enabled

	if (BUTTON_PORT == PORTB){
		//interrupt setup
		PCICR |= (1<<PCIE0);
		PCMSK0 |= (1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK);//enable the pin change interrupts on pinD 4,5,6,7
		
		}else if (BUTTON_PORT == PORTC){
		//interrupt setup
		PCICR |= (1<<PCIE1);
		PCMSK1 |= (1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK);//enable the pin change interrupts on pinD 4,5,6,7
		
		}else if (BUTTON_PORT == PORTD){
		//interrupt setup
		PCICR |= (1<<PCIE2);
		PCMSK2 |= (1 << DOWN)|(1 << UP)|(1 << SELECT)|(1 << BACK);//enable the pin change interrupts on pinD 4,5,6,7
	}

	_delay_ms(200);//to prevent accidental interrupt triggering/ dirty fix
}

void updateButtons(void){
	switch (keyPressed)
	{
		case BACK:
		currentPage = pageHistory(1, 0);
		reDrawRequired = 1;
		keyPressed = 0;//reset key pressed
		break;
		
		case SELECT:
		pageSel();
		reDrawRequired = 1;
		keyPressed = 0;
		break;
		
		case UP:
		choiceAdder(-1);
		reDrawRequired = 1;
		keyPressed = 0;
		break;
		
		case DOWN:
		choiceAdder(+1);
		reDrawRequired = 1;
		keyPressed = 0;
		break;
	}
}


//pages_______________________________________________________________________________________________________________________________________________

void MenuStyle0(void){
	//int yStat = (y+(height/2)+3);
	selectEnable = 1;
	int height;
	int width;
	int x;
	int y;
	
	// menu
	for (int i = 0; i < menuChoices+1; i++){
		if(i == currentChoice[currentPage]){
			//x and the y of the boxes
			width = ((currentFrame+1) *(widthCurChoice - widthChoice))/framesNum+widthChoice;
			height = screenDiv - Margin;
			
			x = (screenWidth/2) - width / 2;
			y = i * screenDiv + TopMargin + (Scroll[MENUPAGE] * screenDiv);
			if (y >= TopMargin)
			{
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRBox(&u8g2, x, y, width, height, MenuBoxRounding);

				u8g2_SetDrawColor(&u8g2, 0);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menuStrings[i]) / 2 , y+(height/2)+3, menuStrings[i]);
			}
			
			}else{
			//x and the y of the boxes
			
			width = widthChoice;
			height = screenDiv - Margin;
			x = (screenWidth/2)-widthChoice/2;
			y = i * screenDiv + TopMargin + (Scroll[MENUPAGE] * screenDiv);
			if (y >= TopMargin)
			{
				//u8g2_SetDefaultForegroundColor(&u8g2);
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRFrame(&u8g2, x, y, width, height, MenuBoxRounding);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menuStrings[i]) / 2, y+(height/2)+3, menuStrings[i]);
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

void MenuStyle1(void){
	selectEnable = 1;
	int width = screenWidth;
	int height = screenDiv;
	
	int x = ((screenWidth/2) - width / 2);
	int y;
	int yStat = (height/2)+3;
	height = height - Margin;
	
	for (int i = 0; i < menuChoices+1; i++){//do one extra to dirty fix lib error. make one menu extra outside scope.
		if(currentChoice[currentPage] == i){
			//x and the y of the boxes
			//width = screenWidth;
			//height = screenDiv - Margin;
			
			// 				x = (screenWidth/2) - width / 2;
			y = i * screenDiv + TopMargin + (Scroll[MENUPAGE] * screenDiv);
			/*	if (Scroll[MENUPAGE] == -1){
			y -= 2;
			}*/
			
			if (y >= TopMargin)
			{
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRBox(&u8g2, x, y, width, height, MenuBoxRounding);

				u8g2_SetDrawColor(&u8g2, 0);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menuStrings[i]) / 2, y+yStat, menuStrings[i]);
			}
			}else{

			y = i * screenDiv + TopMargin + (Scroll[MENUPAGE] * screenDiv);
			/*
			if (Scroll[MENUPAGE] == -1){
			y -= 2;
			}*/
			if (y >= TopMargin)
			{
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRFrame(&u8g2, x, y, width, height, MenuBoxRounding);
				u8g2_DrawStr(&u8g2, x + (width/2) - u8g2_GetStrWidth(&u8g2, menuStrings[i]) / 2, y+yStat, menuStrings[i]);
			}
		}
	}
	reDrawRequired = 0;
}

void StartPage(void){
	selectEnable = 1;
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
	selectEnable = 0;
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
	
	unsigned int ADCval = updateBatterySens();
	if (ADCval<=256){
		batteryBars = 0;
		}else if (ADCval <= 512){
		batteryBars = 1;
		}else if (ADCval <= 786){
		batteryBars = 2;
		}else if (ADCval <= 1024){
		batteryBars = 3;
	}
	
	for (int i = 0 ; i < batteryBars; i++)//batteryBars is a global variable used to measure the amount of bars that need to be drawn to represent the battery's charge
	{
		u8g2_DrawBox(&u8g2, x1 + 2 + (width3+1) * i, y1 + 2, width3, height3);
	}
	
	
	
	//Mode:
	char modeTxt[7] = "Mode: ";
	char modeTxthold[1];
	itoa(*(settingVal+0), modeTxthold, 10);
	strcat(modeTxt, modeTxthold);
	
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	u8g2_DrawStr(&u8g2, screenWidth - u8g2_GetStrWidth(&u8g2, modeTxt), textHeight-3, modeTxt);
	
	
	//Time
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	static char timeTxt[7] = {0,0,0,0,0,0,0};
	getTimeToChar(timeTxt, 0);
	
	u8g2_DrawStr(&u8g2, 0, textHeight-3, timeTxt);
	
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);//reset font to the main font.
}

void Page_2(void){
	#define Lines 4
	#define digAmount 8
	
	#define startLine_Data 3
	selectEnable = 0;

	static char *text2[Lines] = {
		"Speed:>  ",
		"Direction:> ",
		"Distance:> ",
		"runTime:> "
	};
	
	int maxPrefixWidth = u8g2_GetStrWidth(&u8g2, text2[1]);
	
	static char *postFixes[Lines] = {
		"km/h",
		"deg",
		"cm",
		""
	};
	
	//placeholders for the data
	char d1[digAmount];
	char d2[digAmount];
	char d3[digAmount];
	char d4[digAmount];
	char *Data[Lines] = { d1, d2, d3, d4};
		
	itoa(*speedHold, Data[0], 10);
	itoa(*directionHold, Data[1], 10);
	itoa(*distanceHold, Data[2], 10);
	
	//little bit of code to make a typical digital watch look for the time String
	getTimeToChar(Data[3], 1);
	
	
	
	//title
	u8g2_DrawStr(&u8g2, 0, textHeight + TopMargin, menuStrings[currentChoice[MENUPAGE]]);

	for (int i = 0; i < Lines; i++)
	{
		int dataWidth = u8g2_GetStrWidth(&u8g2, Data[i]);
		u8g2_DrawStr(&u8g2, 0, textHeight*(i+startLine_Data), text2[i]);
		u8g2_DrawStr(&u8g2, maxPrefixWidth, textHeight*(i+startLine_Data), Data[i]);
		u8g2_DrawStr(&u8g2, maxPrefixWidth + dataWidth + maxCharWidth/2, textHeight*(i+startLine_Data), postFixes[i]);
	}
	
	reDrawRequired = 1;
}

void Page_3(void){
	selectEnable = 0;
	
	char angleHolder[8];
	
	itoa(*angleHold, angleHolder, 10);
	u8g2_DrawStr(&u8g2, 0, textHeight+ TopMargin, menuStrings[currentChoice[MENUPAGE]]);//the titles needs to be the same as the titles of the choices in the menu
	u8g2_DrawStr(&u8g2, 0, textHeight*2 + TopMargin, angleHolder);
	reDrawRequired = 1;
}

void Page_4(void){
	selectEnable = 0;
	u8g2_DrawStr(&u8g2, 0, textHeight+ TopMargin, menuStrings[currentChoice[MENUPAGE]]);

	reDrawRequired = 0;
}

void Page_5(void){//settings
	selectEnable = 1;
	//code for settings, its the core of the menu code..
	//title
	//u8g2_DrawStr(&u8g2, 0, textHeight+ TopMargin, menuStrings[currentChoice[MENUPAGE]]);
	#define selectorWidth 3*maxCharWidth
	#define selector "-> "
	

	int height = screenDiv;
	
	int x = selectorWidth;
	int y;
	int yStat = (height/2)+3;
	height = height - Margin;
	_Bool highlightChoice = 0;
	
	if (currentPage > 5 && currentPage <= 5 + settings){//code for enabling the settings to be changed
		selectEnable = 0;
		//you selected a setting
		*(settingVal + currentChoice[5]) -= currentChoice[currentPage];
		currentChoice[currentPage] = 0;
		highlightChoice = 1;
	}
	
	for (int i = 0; i < settings; i++){
		
		y = TopMargin + i * textHeight/* +  (Scroll[currentPage] * textHeight)*/;

		if(currentChoice[5] == i){
			
			u8g2_SetDrawColor(&u8g2, 1);
			
			u8g2_DrawStr(&u8g2, 0, y+yStat, selector);
			if(highlightChoice){
				u8g2_SetDrawColor(&u8g2, 1);
				u8g2_DrawRBox(&u8g2, x-2, y+yStat - textHeight+1, u8g2_GetStrWidth(&u8g2, settingStrings[i]), textHeight + 1, 2);
				u8g2_SetDrawColor(&u8g2, 0);
				u8g2_DrawStr(&u8g2, x, y+yStat, settingStrings[i]);
				u8g2_SetDrawColor(&u8g2, 1);
				}else{
				
				u8g2_DrawStr(&u8g2, x, y+yStat, settingStrings[i]);
			}
			
			
			char tmptxt[4];
			itoa(*(settingVal + i), tmptxt, 10);
			u8g2_DrawStr(&u8g2, x + u8g2_GetStrWidth(&u8g2, settingStrings[i]), y+yStat, tmptxt);
			}else{
			u8g2_DrawStr(&u8g2, x, y+yStat, settingStrings[i]);
			char tmptxt[4];
			itoa(*(settingVal + i), tmptxt, 10);
			u8g2_DrawStr(&u8g2, x + u8g2_GetStrWidth(&u8g2, settingStrings[i]), y+yStat, tmptxt);
		}
	}


	reDrawRequired = 1;
}
