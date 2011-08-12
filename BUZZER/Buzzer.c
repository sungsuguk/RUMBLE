//file name:Buzzer.c
//date:2010-07-20
//要求：
//		能控制Buzzer发声，用两个不同频率
//		按下SW4响500Hz
//		按下SW5响2KHZ
//		蜂鸣器发声低电平有效，高电平停止发声
/////////////////////////////

//#include "reg52.h"
#include "STC11Fxx.H"

#define uchar unsigned char
#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

sbit BUZZER = P3^5;
sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;


#define SW4 KEY_0
#define SW5 KEY_1

void delay0r1ms (uchar times);
void delays (uchar times);

void main()
{
	while(1)
	{
		while(!SW4)
		{		
			delay0r1ms(10);		//500Hz
			BUZZER = 0;
			delay0r1ms(10);
			BUZZER = 1;
		}
		while(!SW5)
		{		
			delay0r1ms(2);		//2.5KHz
			BUZZER = 0;
			delay0r1ms(2);
			BUZZER = 1;
		}
		BUZZER = 1;
	}
}

void delay0r1ms (uchar times)	//delay times * 0.1ms
{
	uchar onems = CLK;
	uchar ms = 20;
	for (; times > 0; times --)
	{
		for (onems = CLK; onems > 0; onems --)
		{
			ms = 20;
			while (ms --);
		}
	}
}

void delays (uchar times)
{
	uchar onescd = 50;
	for (;times > 0; times --)			//Delay 'times' second.
	{
		for (onescd = 50;onescd > 0; onescd --)	//Delay one second.
		{
			delay0r1ms(200);
		}
	}
}