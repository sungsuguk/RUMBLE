//file name: 8259A.c
//date: 2010-06-29
//
/*******************************************************/
//In initinal program, Port1 is used for ICW1, OCW2 and OCW3. (Initialization Command Word - ICW)
//And ICW1 is need bit4 = 1, but OCW2 and OCW3 need bit4 = 0;
//��һ��8259AоƬ�����¼����ڲ��Ĵ�����
//Interrupt Mask Register (IMR)
//Interrupt Request Register (IRR)
//In Sevice Register (ISR)
/*******************************************************/


//#include "time.h"		//delay time sub_codes
//#include "CH452.H"
#include "8259A.H"
//#include "HardWare.H"
//#include "Interrupt.h"
#include <absacc.h>
#include "CH452.H"

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

void AddCarCircle();
void DisplayNum();

extern unsigned char time[8];
unsigned char FirstReceive=0;

#define LED_ADDR 0x4000	//led address
extern unsigned char xdata *led_addr;

/*****************************************************/
//����ȫ�ֱ���,�ж������ţ� Interrupt Vertor Number-IVN
/*****************************************************/
//unsigned char IVN;

/*****************************************************/
//����ȫ�ֱ���,�жϱ�־�� Interrupt Flag - InterFlag
/*****************************************************/
//unsigned char InterFlag;

//Init 8259a
//ICW3,ICW4 is disable,and Interrupt mode is level mode
void Init8259a ()
{
	ADR_8259AE = ICW1;	//ICW1
	ADR_8259AO = ICW2;	//ICW2
//	ADR_8259AO = ICW3;	//ICW3, No slaves
	ADR_8259AO = ICW4;	//ICW4,	80/85 mode or 8x86 mode	

	ADR_8259AO = OCW1;	//IR0~IR2 is enable
	ADR_8259AE = OCW2;
//	ADR_8259AE = OCW3;
}



void Inter8259a()  interrupt 2
{
	uchar int_numb;			//default value :int_numb >> 3 = 0x08

	int_numb = ADR_8259AE;	//first INTA pulse
	int_numb = ADR_8259AE;	//second INTA pulse, and read the low 8 bits of interrupt vector
	int_numb = int_numb & 0x07; 
	switch(int_numb)
	{
		case 1:	
			if(FirstReceive==0)	 //��һ�ν��յ�������   ������ʱ�ж�
			{
			   ET0=1;TR0=1;	 
			   FirstReceive=1;
			   TH0=0x00;
	           TL0=0x00;
			}
			else				//����ʱ����ж�
			{
			  IrDADecode();		  //�����ⲿ�������ж�
			  }    		
			break;
	
	}
	
	#ifdef MODE8085
	int_numb = ADR_8259AE;		//read the high 8 bits of interrupt vector
	#endif
	ADR_8259AE = OCW2;			//end of interrupt
}


