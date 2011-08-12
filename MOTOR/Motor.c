/////////////////////////////////////////////
//file name:Motor.c
//date:2010-08-01
//////////////////////////////////////////////

#include "reg52.h"
#include "absacc.h"

#define MOTOR_ADDR 0x2000
#define LED_ADDR 0x4000
#define uchar unsigned char
#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

#define COUNT_TIME 1000

sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;
sbit KEY_2 = P1^7;

#define SW4 KEY_0
#define SW5 KEY_1
#define SW6 KEY_2

#define PH 8 //8相
#define PH8 1	//8相
//#define PH 4 //4相

#define AC 1 //步进电机

unsigned char xdata *addr;
unsigned char xdata *addr1;
unsigned char tab_motor[] = {0x07, 0x03, 0x0b, 0x09, 0x0d, 0x0c, 0x0e, 0x06};
unsigned char tab_motor0[] = {0x08, 0x0a, 0x02, 0x06, 0x04, 0x05, 0x0a, 0x09};
char SystemTime;
char times = 0;
#ifdef AC
	unsigned char t = 10;	//ac
#else  
	unsigned char t = 100; //dc
#endif
	unsigned char forword = 0;

void delay100us (unsigned int times);
//void delays (unsigned char times);

void main ()
{

	unsigned int x = 0;

    TMOD &= 0xF0;
    TMOD |= 0x01; 	//TMOD的值表示定时器工作方式1,16位计数
	TH0=(65536-COUNT_TIME)/256; 
	TL0=(65536-COUNT_TIME)%256; 
    ET0 = 1; 		//允许定时器0中断
    EA=1;   		//开总中断
    TR0 = 1; 		//开始计数		
	addr1 = LED_ADDR;
	addr = MOTOR_ADDR;
	SystemTime = t;

	*addr1 = 0x00;
	while(1)
	{
		if (!SW4)
		{
			delay100us(10);
			while (!SW4);
			delay100us(10);
			t ++;
			if(t > 20) t = 20;		
		}
		if (!SW6)
		{
			delay100us(10);
			while (!SW6);
			delay100us(10);
			t --;
			if(t < 3) t = 3;		
		}
		if (!SW5)
		{
			delay100us(10);
			while (!SW5);
			delay100us(10);
			forword = ~forword;	t = 10;	
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

void timer0(void) interrupt 1 using 1 //中断部分代码
{
	TH0=(65536-COUNT_TIME)/256; 
	TL0=(65536-COUNT_TIME)%256;

    SystemTime--;

	if(SystemTime == 1)
	{	SystemTime = t;	
		*addr = (tab_motor[times] << 3);
		if (forword)
		{
			times ++; 
			if( times > PH)
			 {times = 0;}
		}
		else
		{
			times --;
			if( times < 0){ times = PH - 1;}
		}
		
	}
}
