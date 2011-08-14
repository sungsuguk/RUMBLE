//file name:Main.c
//头文件包含
#include "DS18B20.H"
#include "CH452.H"
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
 
//引脚定义
sbit BUZZER=P3^5;
//LED灯的地址
#define LED_ADDR 0x4000
unsigned char xdata *led=LED_ADDR;
//LS377的地址
#define LS377_ADDR 0x2000
unsigned char xdata *ls377=LS377_ADDR;
 
//函数声明
void Sysrem_Init();
void Temp_Read();
void DIG_Disp();
void Buzz_and_Heat();
void Clear_Dis();
 
//变量定义
unsigned char Temp_Int,Temp_Dec;
unsigned char Disp[4]={0x00};
 
//主函数
void main(void)
{
	//系统初始化
	Sysrem_Init();
	while(1)
	{
		//读取温度并处理
		Temp_Read();
		//数码管显示
		DIG_Disp();
		//蜂鸣和加热
		Buzz_and_Heat();
	}
}
 
//系统初始化
void Sysrem_Init()
{
	//DS18B20初始化
	while(OW_Init());
 
	//CH452初始化
	CH452_Write(CH452_SYSON1);
	CH452_Write(CH452_BCD);
	Clear_Dis();
	CH452_Write(CH452_DIG0|0|0x80);
 
	//Buzzer初始化
	BUZZER=1;
 
	//Relay初始化
	*ls377=0x80;
 
	//LED灯初始化
	*led=0xff;
}
 
//读取温度并处理
void Temp_Read()
{
	unsigned char lsb,msb;
	//温度采集
	if(~OW_Init())
	{
		Write_Byte(0xCC);
		Write_Byte(0x44);
		while(~Read_Bit());
		if(~OW_Init())
		{
			Write_Byte(0xCC);
			Write_Byte(0xBE);
			lsb=Read_Byte();
			msb=Read_Byte();
			while(OW_Init());
		}
   	}
	//数据处理
	Temp_Int=(msb<<4|lsb>>4);
	Temp_Dec=(lsb&0x0f)*0.0625*100;
	Disp[3]=Temp_Int/10;
	Disp[2]=Temp_Int%10;
	Disp[1]=Temp_Dec/10;
	Disp[0]=Temp_Dec%10;
}
 
//数码管显示
void DIG_Disp()
{
	//显示温度
	CH452_Write(CH452_DIG3|Disp[3]);
	CH452_Write(CH452_DIG2|Disp[2]|0x80);
	CH452_Write(CH452_DIG1|Disp[1]);
	CH452_Write(CH452_DIG0|Disp[0]);
}
 
//蜂鸣加热
void Buzz_and_Heat()
{
	unsigned char i;
	//当温度超过设定的范围时，使用蜂鸣器进行提示
	if(Temp_Int>=30||Temp_Int<20)
	{
		//蜂鸣
		i=100;
		while(i--)
		{
			BUZZER=0;
			delayus(100);
			BUZZER=1;
			delayus(100);
		}
   	}
	//当温度超出一定的范围时，通过继电器的开关进行加热，并用LED灯指示工作状态
	else if(Temp_Int<25)
	{
		//加热
		*ls377=0x00;
		*led=0xaa;
	}
	else
	{
		//停止加热
		*ls377=0x80;
		*led=0x55;
	}
}
 
//清除数码管显示
void Clear_Dis()
{
	CH452_Write(CH452_NO_BCD);
	CH452_Write(CH452_DIG0|0);
	CH452_Write(CH452_DIG1|0);
	CH452_Write(CH452_DIG2|0);
	CH452_Write(CH452_DIG3|0);
	CH452_Write(CH452_DIG4|0);
	CH452_Write(CH452_DIG5|0);
	CH452_Write(CH452_DIG6|0);
	CH452_Write(CH452_DIG7|0);
	CH452_Write(CH452_BCD);
}
