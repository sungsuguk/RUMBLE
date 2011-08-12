/////////////////////////////////////////////
//file name:ledx8_test.c
//LED * 8 test code 
//Lighten the led from left to right, 
//and then close the LED from left to right.
//date: 2010-07-19
//要求：
//		依次点亮，每次多一个，亮度一致
//////////////////////////////////////////////

#include "reg52.h"
#include "absacc.h"

#define LED_ADDR 0x4000
#define LED_AD XBYTE[0x2fff]
#define uchar unsigned char
#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

unsigned char xdata *led_addr;

void delayms (unsigned int times);
void delays (unsigned char times);

void main ()
{
	unsigned char temp;
	unsigned char times;

	led_addr = LED_ADDR;
	*led_addr = 0xfe;

	while(1)
	{
		temp = 0xfe;
		
		for (times = 0; times < 8; times ++)
		{	
			*led_addr = temp;
			delays(1);
			temp = temp << 1;
		}
		
		temp = 0;
		for (times = 0; times < 8; times ++)
		{
			*led_addr = temp;
			delays(1);
			temp = (temp << 1) + 1;			
		}

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
