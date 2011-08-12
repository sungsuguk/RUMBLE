//file name: 8259A.c
//date: 2010-06-29
//
/*******************************************************/
//In initinal program, Port1 is used for ICW1, OCW2 and OCW3. (Initialization Command Word - ICW)
//And ICW1 is need bit4 = 1, but OCW2 and OCW3 need bit4 = 0;
//在一个8259A芯片有如下几个内部寄存器：
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
#include "AT24C02Driver.h"

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif



unsigned char Key_Val_Tab[4][4] = { 0x0f, 0x0e, 0x0d, 0x0c,
						  			0x0b, 0x0a, 0x09, 0x08,
						  			0x07, 0x06, 0x05, 0x04,
						  			0x03, 0x02, 0x01, 0x00};//键值转换表
extern unsigned char time[8];

#define LED_ADDR 0x4000	//led address
extern unsigned char xdata *led_addr;

/*****************************************************/
//定义全局变量,中断向量号： Interrupt Vertor Number-IVN
/*****************************************************/
//unsigned char IVN;

/*****************************************************/
//定义全局变量,中断标志： Interrupt Flag - InterFlag
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
		case 0:
			CH452_Write(CH452_DIG6 | 0);//Server_Hall();			
			break;
		case 1:
			CH452_Write(CH452_DIG6 | 1);//Server_IR();
			break;
		case 2:
			CH452_Write(CH452_DIG6 | 2);Server_Key();
			break;
		default:
			CH452_Write(CH452_DIG6 | int_numb);//other Number
			break;
	}
	
	#ifdef MODE8085
	int_numb = ADR_8259AE;		//read the high 8 bits of interrupt vector
	#endif
	ADR_8259AE = OCW2;			//end of interrupt
}




void Server_Key()
{	
	unsigned char KeyValue;

	KeyValue = CH452_Read() - 0x40;
	KeyValue = Key_Val_Tab[KeyValue/8][KeyValue%4];
	at24c02bytewrite(1,KeyValue);
	//CH452_Write(CH452_DIG0 | KeyValue);
}

void Server_Hall()
{
	time[0] += 1;
	led_addr = LED_ADDR;
	*led_addr = (time[0]<<1) | 0xFD;
	CH452_Write(CH452_DIG0 | time[0]);	
}

void Server_IR()
{
	CH452_Write(CH452_DIG2 | 5);
}

