//file name: 8259A.c
#include "8259A.H"
//#include <absacc.h>
#include "CH452.H"
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

#define uchar unsigned char

uchar CarCircleNum=0,MileageNum=0;

void AddCarCircle();

void Init8259a ()
{
	ADR_8259AE=ICW1;
	ADR_8259AO=ICW2;
	ADR_8259AO=ICW4;

	ADR_8259AO=OCW1;
	ADR_8259AE=OCW2;
}

void Inter8259a() interrupt 2
{
	uchar int_numb;
	int_numb = ADR_8259AE;
	int_numb = ADR_8259AE;
	#ifdef MODE8085
	int_numb = ADR_8259AE;
	#endif

	int_numb = int_numb & 0x07;
	switch(int_numb)
	{	case 0:
			AddCarCircle();
			break;
		case 7:break;
		default:
			break;
	}
	ADR_8259AE = OCW2;
}

void AddCarCircle()
{
	CarCircleNum++;
	MileageNum++;
}