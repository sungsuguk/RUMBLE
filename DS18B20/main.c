//file name:main.c
//date:2010-07-25
//By: Bovey
//function:要求：
//		能检测温度值，并将结果在数码管中显示，给传感器加热，观察温度值的变化是否 
//只在最低四位显示温度值，测试程序只能检测0度以上的温度
//无传感器，则会显示00.06

#include "CH452.H"
#include "DS18B20.H"

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

sbit IROUT = P1^3;

unsigned char time[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88}; //Default
extern unsigned char Temp_Integer;
extern unsigned char Temp_Decimal;

#define CLK 5
void delays (unsigned char times);
void delayms (unsigned char times);
void Write_Disp();

#define LED_ADDR 0x4000	//
#define ADDR 0x2000
unsigned char xdata *led_addr; //

void main()
{
	ow_reset();
  	write_byte(0xcc);  //跳过ROM
    write_byte(0x44);  //启动温度转换指令

	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	// BCD译码,8个数码管
	Write_Disp();
	
	IROUT = 0;
	CLK_DIV = 0x02;

	while(1)
	{
		delays(0);
		ow_reset();
		write_byte(0xcc);  //跳过ROM
  		write_byte(0x44);
		delayms(50);
		GetTemp();
		time[0] = Temp_Decimal % 10;
		time[1] = Temp_Decimal/10;
		time[2] = Temp_Integer%10 | 0x80;
		time[3] = Temp_Integer/10;

		Write_Disp();
		ow_reset();
	}
}

void Write_Disp()
{
//	CH452_Write(CH452_DIG0 | time[0]);
	CH452_Write(CH452_DIG0 | time[0]);
	CH452_Write(CH452_DIG1 | time[1]);	
	CH452_Write(CH452_DIG2 | time[2]);	
	CH452_Write(CH452_DIG3 | time[3]); 	
//	CH452_Write(CH452_DIG4 | time[4]);	
//	CH452_Write(CH452_DIG5 | time[5]);	
//	CH452_Write(CH452_DIG6 | time[6]);	
//	CH452_Write(CH452_DIG7 | time[7]);  
}

void delayms (unsigned char times)
{
	unsigned char onems = CLK;
	unsigned char ms = 200;
	for (; times > 0; times --)
	{
		for (onems = CLK; onems > 0; onems --)
		{
			ms = 200;
			while (ms --);
		}
	}
}

void delays (unsigned char times)
{
	unsigned char onescd = 5;
	for (;times > 0; times --)			//Delay 'times' second.
	{
		for (onescd = 5;onescd > 0; onescd --)	//Delay one second.
		{
			delayms(200);
		}
	}
}