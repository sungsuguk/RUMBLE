//file name:main.c
//date:2010-06-30
//By:Bovey

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
#include "AT24C02Driver.h"
#include "ch452.h"
#include "8259A.h"

#define uchar unsigned char 

#define LED_ADDR 0x4000

unsigned char code displaycode[]={
                                   0x3f,0x06,0x5b,0x4f,
                                   0x66,0x6d,0x7d,0x07,
                                   0x7f,0x6f,0x77,0x7c,
                                   0x39,0x5e,0x79,0x71,
                                   0x00,
                                 };


unsigned int sendcount;
unsigned char readpointer;

unsigned char displaycount;
unsigned char displaybuffer[8];

unsigned char xdata *led_addr;

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


void init();


void main(void)
{

  unsigned char read_value;

	led_addr = LED_ADDR;
	*led_addr = 0xfe;

	init();

  while(1)
    {
  		read_value = at24c02byteread(1);
		CH452_Write(CH452_DIG0 | read_value%10);	
		CH452_Write(CH452_DIG1 | (read_value/10)%10);
		CH452_Write(CH452_DIG2 | (read_value/100));
//		*led_addr = read_value;
		delays(1);

    }
}



void init()
{
//	TMOD=0x21;
//  TH0=0x3c;
//  TL0=0xb0;	//��ʱ50ms 
//  TMOD=0x01;	//��ʱ��ģʽ0��16λ
//  ET0=1; 		//����ʱ��0�ж�
	EX1 = 1;	//�������ж�1�ж�
//  TR0=1;		//������ʱ��0
//	TI=0;

	CLK_DIV = 0x01;
	Init8259a();

///////////////////////////		//init CH452
	CH452_Write(CH452_SYSON2);	//�����Ʒ�ʽ�����SDA���������ж��������ô����Ӧ��ΪCH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	// BCD����,8�������
//	Write_Disp();

	EA=1;		//�������ж�	
}

