//File name:main.c
//Date:2010-06-30
//By:Bovey
//Function: test 8259A , CH452_key and Hall sensor
//现象: 上电后数码管的最低位显示0，
//其他位熄灭, 有按键按下在最后一位显示键值
//第七位显示中断号，用磁铁在hall传感器附近
//移动会在最低位显示计数值，高位显示中断号0
//测试中会偶尔出现高位为0的情况，为红外接
//收的干扰。
//Last Changed:2010-09-20

//#include "time.h"		//delay time sub_codes
#include "CH452.H"
#include "8259A.H"
#include "HardWare.H"
//#include "Interrupt.h"
#include "absacc.h"
//#include "ABSACC.H"

#ifndef STC
#include "STC12C5A56S2.H"
#endif

sbit KEY0 = P1^5;
sbit KEY1 = P1^6;
sbit KEY2 = P1^7;

#define SW4 KEY0
#define SW5 KEY1
#define SW6 KEY2

#define uchar unsigned char

#define LED_ADDR 0x4000	//for test
		//for test
unsigned char xdata *led_addr; //for test

unsigned char time[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //Default
extern unsigned char IVN;
extern unsigned char InterFlag;

void init();
void Write_Disp();

void main()
{
	unsigned char KeyValue = 0;

	init();

	led_addr = LED_ADDR;	//for test
	*led_addr = 0x7f;
	CH452_Write(CH452_DIG0 | 0);

	while (1)
	{
	;
	/*	if(InterFlag > 0)
		{
			InterruptServer();
		}*/
	}
}

void init()
{
//	TMOD=0x21;
//  TH0=0x3c;
//  TL0=0xb0;	//延时50ms 
//  TMOD=0x01;	//定时器模式0，16位
//  ET0=1; 		//允许定时器0中断
	EX1 = 1;	//允许外中断1中断
//  TR0=1;		//开启定时器0
//	TI=0;

	CLK_DIV = 0x01;
	Init8259a();

///////////////////////////		//init CH452
	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	// BCD译码,8个数码管
//	Write_Disp();

	EA=1;		//开启总中断	
}

void Write_Disp()
{
	CH452_Write(CH452_DIG0 | time[0]);
	CH452_Write(CH452_DIG1 | time[1]);	
	CH452_Write(CH452_DIG2 | time[2]);	
	CH452_Write(CH452_DIG3 | time[3]);	
	CH452_Write(CH452_DIG4 | time[4]);	
	CH452_Write(CH452_DIG5 | time[5]);	
	CH452_Write(CH452_DIG6 | time[6]);	
	CH452_Write(CH452_DIG7 | time[7]);  
}
