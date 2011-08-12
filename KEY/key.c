//file name:key.c
//date:2010-07-20
//独立式按键
//要求：
//三个按键SW4, SW5, SW6 对应LED*8 的其中3 个LED ，
//支持不同键同时按下


#include "reg52.h"
#include "absacc.h"

#define LED_ADDR 0x4000
#define LED_AD XBYTE[0x2fff]
#define uchar unsigned char
#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;
sbit KEY_2 = P1^7;

#define SW4 KEY_0
#define SW5 KEY_1
#define SW6 KEY_2

unsigned char xdata *led_addr;

void delayms (unsigned int times);
void delays (unsigned char times);

void main ()
{
	unsigned char temp = 0xef;

	led_addr = LED_ADDR;
	*led_addr = 0xef;

	while(1)
	{

/*
//这个程序段实现的是按下一个键，对应的一个位亮灭状态改变
		if (!SW4)
		{
		delayms(20);
		while (!SW4);
		delayms(20);
		temp = (temp & ~(1 << 1)) | ((~ temp) & 1 << 1);	//第二位取反
		*led_addr = temp;
		}
		if (!SW5)
		{
		delayms(20);
		while (!SW5);
		delayms(10);
		temp = (temp & ~(1 << 2)) | ((~ temp) & 1 << 2);	//第四位取反
		*led_addr = temp;
		}		
		if (!SW6)
		{
		delayms(20);
		while (!SW6);
		delayms(10);
		temp = (temp & ~(1 << 3)) | ((~ temp) & 1 << 3);	//第三位取反
		*led_addr = temp;
		}
*/

//按键检测，有键按下则点亮对应的灯

		if (SW4)
			temp = temp | 1 << 1;
		else 
			temp = temp & ~(1 << 1);
		if (SW5)
			temp = temp | 1 << 2;
		else 
			temp = temp & ~(1 << 2);
		if (SW6)
			temp = temp | 1 << 3;
		else 
			temp = temp & ~(1 << 3);
		*led_addr = temp;
	}	
	
}

//delay nms
void delayms (unsigned int times)
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
			delayms(200);
		}
	}
}
