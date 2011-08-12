//File name:main.c
//Date:2010-06-30
//By:Bovey
//Function: test 8259A , CH452_key and Hall sensor
//����: �ϵ������ܵ����λ��ʾ0��
//����λϨ��, �а������������һλ��ʾ��ֵ
//����λ��ʾ�жϺţ��ô�����hall����������
//�ƶ��������λ��ʾ����ֵ����λ��ʾ�жϺ�0
//�����л�ż�����ָ�λΪ0�������Ϊ�����
//�յĸ��š�
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
