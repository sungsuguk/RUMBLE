//file name:CH452_DisplayTest_Main.c
//date:2010-06-28
//note:CH452 is 2 wires mode,the circuit has SDA,SCL
//要求：
//		第一位a~h段依次点亮，最终显示'8.'
//		全部显示'8.'	
//		在2V0版本的物料中焊接后的显示情况
//		用段寻址方式现象和原来的不一致，
//		为从第一位逐位点亮a 段，之后逐位显示'8.'
//		分析原因可能是内部段地址编码方式不同造成
//////////////////////////////////////////

//#include "time.h"		//delay time sub_codes
#include "CH452.H"
/*#ifndef STC
#include "STC12C5A56S2.H"
#endif*/
#include "reg52.h"

#define uchar unsigned char
#define uint unsigned int


sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;
sbit KEY_2 = P1^7;

#define SW4 KEY_0
#define SW5 KEY_1
#define SW6 KEY_2

void Clear_Dis();
void delays (uchar times);
void delayms (unsigned int times);

void main()
{
	delayms(50);	
	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	//BCD译码,8个数码管
	Clear_Dis();				//清楚数码管显示数字


	while (1)
	{	
	Clear_Dis();				//清楚数码管显示数字
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x00);	//段寻址,DIG8的a~h
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x08);
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x10);
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x18);
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x20);
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x28);
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x30);
	delays(1);
	CH452_Write(CH452_SET_BIT | 0x38);
	delays(1);	
	CH452_Write(CH452_DIG0 | 0x88);		//BCD码位输入，显示字符8
	delays(1);
	CH452_Write(CH452_DIG1 | 0x88);
	delays(1);
	CH452_Write(CH452_DIG2 | 0x88);
	delays(1);
	CH452_Write(CH452_DIG3 | 0x88);
	delays(1);
	CH452_Write(CH452_DIG4 | 0x88);
	delays(1);
	CH452_Write(CH452_DIG5 | 0x88);
	delays(1);
	CH452_Write(CH452_DIG6 | 0x88);
	delays(1);
	CH452_Write(CH452_DIG7 | 0x88);  
	delays(1);
	while(1)
	{
		if (!SW4)
		{
		delayms(20);
		while (!SW4);
		delayms(20);
		CH452_Write(CH452_DIG0 | 0);
		}
	}
	}	
}



#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

//times is from 0 to 65535
void delayms (unsigned int times)
{
	uchar onems = CLK;
	uchar ms = 200;
	for (; times > 0; times --)
	{
		for (onems = CLK; onems > 0; onems --)
		{
			ms = 200;
			while (ms --);
		}
	}
}


//times is from 0 to 255
void delays (uchar times)
{
	uchar onescd = 5;
	for (;times > 0; times --)			//Delay 'times' second.
	{
		for (onescd = 5;onescd > 0; onescd --)	//Delay one second.
		{
			delayms(200);
		}
	}
}

void Clear_Dis()
{
	CH452_Write(CH452_NO_BCD );
//	CH452_Write(CH452_SET_BIT | 0);
	CH452_Write(CH452_DIG7 | 0x00);
	CH452_Write(CH452_DIG6 | 0x00);
	CH452_Write(CH452_DIG5 | 0x00);
	CH452_Write(CH452_DIG4 | 0x00);
	CH452_Write(CH452_DIG3 | 0x00);
	CH452_Write(CH452_DIG2 | 0x00);
	CH452_Write(CH452_DIG1 | 0x00);
	CH452_Write(CH452_DIG0 | 0x00);
	CH452_Write(CH452_BCD); 
}
