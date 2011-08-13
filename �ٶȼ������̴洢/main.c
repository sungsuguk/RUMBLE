//file name:main.c
#include <ch452.h>
#include <8259a.h>
#include <at24c02driver.h>
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

#define uint unsigned int
#define uchar unsigned char

uchar num0=0,num1=0,num2=0,num3=0,num4=0,num5=0,num6=0,num7=0;
extern uchar CarCircleNum,MileageNum;

void Delayms(unsigned char i);
void Clear_Dis();
void Init_EEPROM();
void Initialization();
void Speed_Computer();
void Speed_Dis();
void Mileage_Computer();
void Mileage_Read();
void Mileage_Dis();

void main()
{
	Delayms(50);
	Init_EEPROM();
	Initialization();
	while(1)
	{

	};
}

void Delayms(unsigned char i)
{	unsigned int	j;
	do{	for(j=0;j!=1000;j++)
		{;}
	}while(--i);
}

void Clear_Dis()
{
	CH452_Write(CH452_NO_BCD);
	CH452_Write(CH452_DIG0 | 0);
	CH452_Write(CH452_DIG1 | 0);
	CH452_Write(CH452_DIG2 | 0);
	CH452_Write(CH452_DIG3 | 0);
	CH452_Write(CH452_DIG4 | 0);
	CH452_Write(CH452_DIG5 | 0);
	CH452_Write(CH452_DIG6 | 0);
	CH452_Write(CH452_DIG7 | 0);
	CH452_Write(CH452_BCD);
}

void Init_EEPROM()
{
	Mileage_Read();
	if(num3<0|num3>9)
	{
		at24c02bytewrite(0x71,0);Delayms(2);
		at24c02bytewrite(0x72,0);Delayms(2);
		at24c02bytewrite(0x73,0);Delayms(2);
		at24c02bytewrite(0x74,0);Delayms(2);
		at24c02bytewrite(0x75,0);Delayms(2);
	}
}

void Initialization()
{
	CH452_Write(CH452_SYSON2);
	CH452_Write(CH452_BCD);
	Clear_Dis();
	Delayms(10);
	CH452_Write(CH452_DIG0 | 0);
	Init8259a ();
	TMOD=0x11;
	TH0=(65536-2000)/256;
	TL0=(65536-2000)%256;
	TH1=(65536-5000)/256;
	TL1=(65536-5000)%256;
	IT1=1;
	EX1=1;
	ET0=1;
	ET1=1;
	EA=1;
	TR0=1;
	TR1=1;
}

uint Cnt0;
void Timer0_IntSer_Speed(void) interrupt 1
{
	TH0=(65536-2000)/256;
	TL0=(65536-2000)%256;
	Cnt0++;
	if(Cnt0==1000)
	{
		Cnt0=0;
		Speed_Computer();
		Speed_Dis();
		CarCircleNum=0;
	}
}

uint Cnt1=1999;
void Timer1_IntSer_Mileage(void) interrupt 3
{
	TH1=(65536-5000)/256;
	TL1=(65536-5000)%256;
	Cnt1++;
	if(Cnt1==2000)
	{
		Cnt1=0;
		Mileage_Computer();
		Mileage_Read();
		Mileage_Dis();
		MileageNum=0;
	}
}

void Speed_Computer()
{
	uchar speed;
	speed=(3.14*0.6*CarCircleNum*3.6)/2;
	num0=speed%10;
	num1=speed/10%10;
}

void Speed_Dis()
{
	CH452_Write(CH452_DIG0 | num0);
	if(num1)
		CH452_Write(CH452_DIG1 | num1);
	else
	{
		CH452_Write(CH452_NO_BCD);
		CH452_Write(CH452_DIG1 | 0);
		CH452_Write(CH452_BCD);
	}
}

void Mileage_Computer()
{
	uint Mileage;
	if(MileageNum)
	{
		Mileage=3.14*0.6*MileageNum;
		Mileage=Mileage+num3;num3=Mileage%10;
		Mileage=Mileage/10+num4;num4=Mileage%10;
		Mileage=Mileage/10+num5;num5=Mileage%10;
		Mileage=Mileage/10+num6;num6=Mileage%10;
		Mileage=Mileage/10+num7;num7=Mileage%10;

		at24c02bytewrite(0x71,num3);Delayms(2);
		at24c02bytewrite(0x72,num4);Delayms(2);
		at24c02bytewrite(0x73,num5);Delayms(2);
		at24c02bytewrite(0x74,num6);Delayms(2);
		at24c02bytewrite(0x75,num7);Delayms(2);
	}
}

void Mileage_Read()
{
	num3=at24c02byteread(0x71);Delayms(1);
	num4=at24c02byteread(0x72);Delayms(1);
	num5=at24c02byteread(0x73);Delayms(1);
	num6=at24c02byteread(0x74);Delayms(1);
	num7=at24c02byteread(0x75);Delayms(1);
}

void Mileage_Dis()
{
	CH452_Write(CH452_DIG3 | num3);
	if(num4|num5|num6|num7)
	CH452_Write(CH452_DIG4 | num4);
	if(num5|num6|num7)
	CH452_Write(CH452_DIG5 | num5);
	if(num6|num7)
	CH452_Write(CH452_DIG6 | num6);
	if(num7)
	CH452_Write(CH452_DIG7 | num7);
}
