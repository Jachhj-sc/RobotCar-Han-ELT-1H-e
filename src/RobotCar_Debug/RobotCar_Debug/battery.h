/*
 * battery.h
 *
 * Created: 30-1-2021 15:25:29
 *  Author: willi
 */ 


#ifndef BATTERY_H_
#define BATTERY_H_

//in mV
#define BATTERY0 6550
#define BATTERY5 7220
#define BATTERY10 7370
#define BATTERY15 7410
#define BATTERY20 7450
#define BATTERY25 7490
#define BATTERY30 7530
#define BATTERY35 7570
#define BATTERY40 7590
#define BATTERY45 7630
#define BATTERY50 7670
#define BATTERY55 7710
#define BATTERY60 7750
#define BATTERY65 7830
#define BATTERY70 7910
#define BATTERY75 7970
#define BATTERY80 8050
#define BATTERY85 8160
#define BATTERY90 8220
#define BATTERY95 8300
#define BATTERY100 8400

int getBatteryADC();
void ADCinit(void);
//float battery_read();

#endif /* BATTERY_H_ */