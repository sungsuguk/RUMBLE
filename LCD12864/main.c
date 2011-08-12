/////////////////////////////////////////////
//file name:main.c
//LCD-12864
//date: 2010-07-31
//////////////////////////////////////////////


#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
#include "absacc.h"
#include "LCD-12864.H"


#define uchar unsigned char
#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz


void delayxms (unsigned int times);
void delays (unsigned char times);


#define WRITE_CMD_ADDR	0x1000
#define WRITE_DATA_ADDR	0x1100
#define READ_BUSY_ADDR	0x1200
#define READ_DATA_ADDR	0x1300
void main ()
{

	delayms(100);

	initLCM();

	while(1)
	{
	initLCM();
	WriteRAMTest();	
	delays(4);
	ClearRAM();
	SdCmd(0x08);
	delays(4);
	}	
	
}

//delay xms
void delayxms (unsigned int times)
{
	unsigned char onems = CLK;
	unsigned char ms = 200;
	for (; times > 0; times --)					//Delay nms, n = times.
	{
		for (onems = CLK; onems > 0; onems --)	//Delay 1ms
		{
			ms = 200;
			while (ms --);
		}
	}
}

//delay ns
void delays (unsigned char times)
{
	uchar onescd = 5;
	for (;times > 0; times --)					//Delay ns, n = times.
	{
		for (onescd = 5;onescd > 0; onescd --)	//Delay 1s.
		{
			delayxms(200);
		}
	}
}
