//file name: HardWare.h
//date : 2010-06-29
//
#include <absacc.h>

//External Data Address
#ifndef _ADR_8259A_ 
#define ADR_8259AO 	XBYTE [0x6001]	//odd address of TMD8259A
#define ADR_8259AE 	XBYTE [0x6000]	//even address of TMD8259A
#define _ADR_8259A_ 1
#endif

#define ADR_LS377DW	XBYTE [0x2000]	//address of 74LS377WD
#define ADR_LCD		XBYTE [0x0000]	//address of LCD
#define ADR_LED		XBYTE [0x4000]	//address of LED, 74HC573's address

//Interface defined as the schematic discripted
#define IIC_SCL		P1^0
#define IIC_SDA 	P1^1
#define IR_IN		P1^2
#define IR_OUT		P1^3
#define TIME_RST	P1^4
#define KEY_0		P1^5
#define KEY_1		P1^6
#define KEY_2		P1^7

#define RX			P3^0
#define TX			P3^1
#define DQ			P3^4
#define BUZZER 		P3^5
#define WR 			P3^6
#define RD 			P3^7




