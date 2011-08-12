//File name:DS1302.C
//Date:2009-07-28
//Changed By:Bovey

//#include "reg52.h"
#include "ch452.h"


#define uint unsigned int
#define uchar unsigned char
sbit  DS1302_CLK = P1^0;              //实时时钟时钟线引脚 
sbit  DS1302_IO  = P1^1;              //实时时钟数据线引脚 
sbit  DS1302_RST = P1^4;  
//实时时钟复位线引脚
sbit  ACC0 = ACC^0;
sbit  ACC7 = ACC^7;

sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;
sbit KEY_2 = P1^7;

#define SW4 KEY_0
#define SW5 KEY_1
#define SW6 KEY_2

void Display(unsigned char * DisDate);
void delays (uchar times);
void delayms (unsigned int times);

//char hide_sec,hide_min,hide_hour,hide_day,hide_week,hide_month,hide_year;  //秒,分,时到日,月,年位闪的计数

/***********DS1302时钟部分子程序******************/
typedef struct __SYSTEMTIME__
{
	unsigned char Second;
	unsigned char Minute;
	unsigned char Hour;
	unsigned char Week;
	unsigned char Day;
	unsigned char Month;
	unsigned char  Year;
//	unsigned char DateString[11];
//	unsigned char TimeString[9];
}SYSTEMTIME ;	//定义的时间类型
SYSTEMTIME CurrentTime;


#define AM(X)	X
#define PM(X)	(X+12)            	  // 转成24小时制
#define DS1302_SECOND	0x80          //时钟芯片的寄存器位置,存放时间
#define DS1302_MINUTE	0x82
#define DS1302_HOUR		0x84 
#define DS1302_WEEK		0x8A
#define DS1302_DAY		0x86
#define DS1302_MONTH	0x88
#define DS1302_YEAR		0x8C 

void DS1302InputByte(unsigned char d) 	//实时时钟写入一字节(内部函数)
{ 
    unsigned char i;
    ACC = d;
    for(i=8; i>0; i--)
    {
        DS1302_IO = ACC0;           	//相当于汇编中的 RRC
        DS1302_CLK = 1;
		DELAY_1US
        DS1302_CLK = 0;
		DELAY_1US
        ACC = ACC >> 1; 
    } 
}

unsigned char DS1302OutputByte(void) 	//实时时钟读取一字节(内部函数)
{ 
    unsigned char i;
    for(i=8; i>0; i--)
    {
        ACC = ACC >>1;         			//相当于汇编中的 RRC 
        ACC7 = DS1302_IO;
        DS1302_CLK = 1;	
		DELAY_1US
        DS1302_CLK = 0;
		DELAY_1US
    } 
    return(ACC); 
}

void Write1302(unsigned char ucAddr, unsigned char ucDa)	//ucAddr: DS1302地址, ucData: 要写的数据
{
    DS1302_RST = 0;
	DELAY_1US;
    DS1302_CLK = 0;
	DELAY_1US;
    DS1302_RST = 1;
	DELAY_1US;
    DS1302InputByte(ucAddr);       	// 地址，命令 
    DS1302InputByte(ucDa);       	// 写1Byte数据
    DS1302_CLK = 1;
	DELAY_1US;
    DS1302_RST = 0;
	DELAY_1US;
} 

unsigned char Read1302(unsigned char ucAddr)	//读取DS1302某地址的数据
{
    unsigned char ucData;
    DS1302_RST = 0;
	DELAY_1US;
    DS1302_CLK = 0;
	DELAY_1US;
    DS1302_RST = 1;
	DELAY_1US;
    DS1302InputByte(ucAddr|0x01);        // 地址，命令 
    ucData = DS1302OutputByte();         // 读1Byte数据
    DS1302_CLK = 1;
	DELAY_1US;
    DS1302_RST = 0;
	DELAY_1US;
    return(ucData);
}
void Initial_DS1302(void)   //时钟芯片初始化
{   
	unsigned char Second=Read1302(DS1302_SECOND);

	if(Second&0x80)	      //判断时钟芯片是否关闭	  
    {
	Write1302(0x8e,0x00); //写入允许
	Write1302(0x8c,0x10); //以下写入初始化时间 日期:10/07/28.星期: 3. 时间: 23:59:55
	Write1302(0x88,0x07);
	Write1302(0x86,0x28);
	Write1302(0x8a,0x03); 	
	Write1302(0x84,0x22);
	Write1302(0x82,0x30);
	Write1302(0x80,0x50);
	Write1302(0x8e,0x80); //禁止写入
	}

}


void DS1302_GetTime(SYSTEMTIME *Time)  //获取时钟芯片的时钟数据到自定义的结构型数组
{
	unsigned char ReadValue;
	ReadValue = Read1302(DS1302_SECOND);
	Time->Second = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_MINUTE);
	Time->Minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_HOUR);
	Time->Hour = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_DAY);
	Time->Day = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);	
	ReadValue = Read1302(DS1302_WEEK);
	Time->Week = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_MONTH);
	Time->Month = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_YEAR);
	Time->Year = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);	
}

void main()
{
	int i;
	unsigned char temp = 0xff;
	unsigned char times[] = {1};
	unsigned char *DispDate;
	unsigned char CurrentTime[7];
	unsigned char ReadValue;
	unsigned char Seconds = Read1302(DS1302_SECOND);

	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	//BCD译码,8个数码管
	Display(times);
	
	DispDate = CurrentTime;
	Initial_DS1302();

	while (1)
	{	
	for(i = 0; i < 200; i++)
	{ 
		if (!SW4)
		{
		delayms(5);
		while (!SW4);
		delayms(5);
		temp = ~temp;
		}
		delayms(2);
	}
	if(temp)
	{
	ReadValue = Read1302(DS1302_SECOND);
	DispDate[0] = ReadValue&0x0f;
	DispDate[1] = ReadValue>>4;
	ReadValue = Read1302(DS1302_MINUTE);
	DispDate[2] = (ReadValue&0x0f)|0x80;
	DispDate[3] = ReadValue>>4;;
	ReadValue = Read1302(DS1302_HOUR);
	DispDate[4] = (ReadValue&0x0f)|0x80;
	DispDate[5] = ReadValue>>4;
	ReadValue = Read1302(DS1302_WEEK);
	DispDate[6] = 0x12;
	DispDate[7] = ReadValue&0x0f;

	}
	else
	{
	ReadValue = Read1302(DS1302_DAY);
	DispDate[0] = (ReadValue&0x0f)|0x00;
	DispDate[1] = ReadValue>>4;	
	ReadValue = Read1302(DS1302_MONTH);
	DispDate[2] = (ReadValue&0x0f)|0x80;
	DispDate[3] = ReadValue>>4;
	ReadValue = Read1302(DS1302_YEAR);
	DispDate[4] = (ReadValue&0x0f)|0x80;
	DispDate[5] = ReadValue>>4;
	DispDate[6] = 0;
	DispDate[7] = 2;
	}
	Display(DispDate);

	}
   
}

void Display(unsigned char * DisDate)
{
	CH452_Write(CH452_DIG7 | DisDate[7]);
	CH452_Write(CH452_DIG6 | DisDate[6]);
	CH452_Write(CH452_DIG5 | DisDate[5]);
	CH452_Write(CH452_DIG4 | DisDate[4]);
	CH452_Write(CH452_DIG3 | DisDate[3]);
	CH452_Write(CH452_DIG2 | DisDate[2]);
	CH452_Write(CH452_DIG1 | DisDate[1]);
	CH452_Write(CH452_DIG0 | DisDate[0]);
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
