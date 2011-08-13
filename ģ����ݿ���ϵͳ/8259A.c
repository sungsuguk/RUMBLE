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
#include "8259A.H"
#include <absacc.h>
#include "CH452.H"
#include"LCD12864.H"

#ifndef STC
#include "STC12C5A56S2.H"
#endif
/*
unsigned char Key_Val_Tab[4][4] = { 0x0f, 0x0e, 0x0d, 0x0c,
						  			0x0b, 0x0a, 0x09, 0x08,
						  			0x07, 0x06, 0x05, 0x04,
						  			0x03, 0x02, 0x01, 0x00};//键值转换表
*/
//将读取到的键值转换为ascii码值

unsigned char Key_Val_Tab[4][4]=
{
	0x00,0x18,0x00,'3',
	0x19,0x18,0x00,'2',
	0x19,0x00,0x00,'1',
	0x00,0x00,0x00,'0'
};

/*
unsigned char Key_Val_Tab[4][4]=
{
	0x00,0xc5,0x00,'3',
	0xc6,0xc5,0x00,'2',
	0xc6,0x00,0x00,'1',
	0x00,0x00,0x00,'0'
};
*/

//extern unsigned char time[8];
#define LED_ADDR 0x4000	//led address
//extern unsigned char xdata *led_addr;

struct call_elevator
{
	uchar floor;
	uchar dir;
};

extern struct call_elevator command_list[];

unsigned char i=0;

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
	ADR_8259AO = ICW4;	//ICW4,	80/85 mode or 8x86 mode	
	ADR_8259AO = OCW1;	//IR0~IR2 is enable
	ADR_8259AE = OCW2;
}
void Inter8259a()  interrupt 2
{	
	Server_Key();		
}

void Server_key()
{
	unsigned char KeyValue;
	struct call_elevator *p;
	KeyValue = CH452_Read() - 0x40;
	p=command_list;
	while(p->dir!=0x00) p++;
	p->dir=Key_Val_Tab[3-KeyValue/8][3-KeyValue%4];
	p->floor=Key_Val_Tab[3-KeyValue/8][3];
	p++;
	p->dir=0x00;
}

/*
void Server_Key()
{	
	
	unsigned char KeyValue=0x00;
	unsigned char *p;

	
	KeyValue = CH452_Read() - 0x40;
	KeyValue = Key_Val_Tab[3-KeyValue/8][3-KeyValue%4];

	delayms(50);
	//接收按键输入，将第一个运算数存放到op1中
	if('0'<=KeyValue&&KeyValue<='9')
	{
		if(OP==0x00)
			p=op1;
		else
			p=op2;
		while(*p!=0x00) p++;
		*p=KeyValue;
		*(p+1)=0x00;//设置结束符
		//显示
		SdData(KeyValue);
	}
	//接收运算符
	if(KeyValue=='+'||KeyValue=='-'||KeyValue=='*'||KeyValue=='/')
	{
		OP=KeyValue;
		//进行显示，首先需要换行，然后显示这个运算符
		SdCmd(0xc0);
		SdData(KeyValue);
		SdData(' ');
	}
	//如果输入是'='，则置标志位，通知主程序进行计算
	if(KeyValue=='=')
	{
		SdData('=');
		flag=1;
	}
}
*/