/////////////////////////////////////////////
//file name:Relay.c
//date:2010-08-11
//////////////////////////////////////////////

#include "reg52.h"
#include "absacc.h"

#define MOTOR_ADDR 0x2000

#define uchar unsigned char
#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;
sbit KEY_2 = P1^7;

#define SW4 KEY_0
#define SW5 KEY_1
#define SW6 KEY_2

unsigned char xdata *addr;


void delay100us (unsigned int times);
//void delays (unsigned char times);

void main ()
{

	addr = MOTOR_ADDR;

	while(1)
	{
		if (!SW4)
		{
			delay100us(10);
			while (!SW4);
			delay100us(10);
			*addr = 0x80;
			
		}
		if (!SW6)
		{
			delay100us(10);
			while (!SW6);
			delay100us(10);
			*addr = 0x00;		
		}
	}
	
}

//delay n*100us
void delay100us (unsigned int times)
{
	unsigned char onems = CLK;
	unsigned char ms = 20;
	for (; times > 0; times --)					//Delay nms, n = times.
	{
		for (onems = CLK; onems > 0; onems --)	//Delay 1ms
		{
			ms = 20;
			while (ms --);
		}
	}
}

