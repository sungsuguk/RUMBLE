#include "LCD12864.H"
#include "8259a.h"
#include "ch452.h"

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

#define uchar unsigned char 
#define MOTOR_ADDR 0x2000
#define LED_ADDR 0x4000
#define T0_COUNT 10000

struct call_elevator
{
	uchar floor;
	uchar dir;
};

struct call_elevator command_list[4];
struct call_elevator *cp;

uchar elevator_floor=1;//�����������¥�㣬Ĭ����1¥

//parameter of motor
uchar motor_dir=1;// 1��ʾ������0��ʾ�½�
uchar motor_floors=0;//���еĲ���
uchar xdata *bus_addr;
uchar xdata *bus_addr2;
uchar tab_motor[] = {0x07, 0x03, 0x0b, 0x09, 0x0d, 0x0c, 0x0e, 0x06};

uchar motor_phase=0;

uint timecount=4096;


void System_Init();
void Motor_Control();
void Motor_Run();
void Command_Schedule();
void ElevatorState_Disp();

void main()
{
	
	System_Init();
	while(1)
	{
		if(cp->dir!=0x00)
		{
			//����elevatorĿǰ��״̬����motor�������趨
			Motor_Control();
			
			//motor ����
			Motor_Run();

			//command_list ����
			Command_Schedule();
		}
	}
}

void System_Init()
{
	//LCD12864
	initLCM();
	SdData('1');
	SdData(' ');
	SdCmd(0x8f);
	SdData(0x08);
	//motor
	bus_addr=MOTOR_ADDR;
	*bus_addr=0x80;
	bus_addr2=LED_ADDR;
	*bus_addr2=0xe0;
	//8259a
	Init8259a();
	//ch452
	CH452_Write(CH452_SYSON2);//��������ɨ�蹦��
	//MCU
	EA=1;
	EX1=1;
	ET0=1;
	//T0
	TH0=(65536-T0_COUNT)/256;
	TL0=(65536-T0_COUNT)%256;
	//command
	cp=command_list;
	cp->dir=0x00;
}

void Motor_Control()
{
	//elevator_floor�����������¥��
	//cp->floor�б�����е��ݵ�¥����߽�Ҫǰ����¥��
	uchar cp_floor;
	cp_floor=cp->floor-'0';
	if(cp_floor==elevator_floor)
	{
		motor_floors=0;
	}
	else
	{
		if(cp_floor>elevator_floor)
		{
			motor_floors=cp_floor-elevator_floor;
			motor_dir=1;//����
		}
		else
		{
			motor_floors=elevator_floor-cp_floor;
			motor_dir=0;//�½�
		}
	}	
}

void Motor_Run()
{
	uchar i;
	uint j;
	//��ʾ����״̬:����or�½�
	if(motor_floors!=0)
	{
		if(motor_dir==0)
		{
			SdCmd(0x81);SdData(0x19);//�½�
		}
		else
		{
			SdCmd(0x81);SdData(0x18);//����
		}
	}
	//motor_dir && motor_floors
	for(i=0;i<motor_floors;i++)
	{
		timecount=4096;//�����źŵĸ���
		TR0=1;
		while(timecount!=0);
		TR0=0;
		*bus_addr=0x07;//�ر�motor����led
		//����һ��forѭ��������¥����Ϣ�ı�һ��
		if(motor_dir==0)
			elevator_floor--;
		else
			elevator_floor++;
		//����״̬��ʾ:¥�㡢����
		ElevatorState_Disp();
	}
	motor_floors=0;
}

void timer0(void) interrupt 1
{
	TH0=(65536-T0_COUNT)/256;
	TL0=(65536-T0_COUNT)%256;
	TR0=0;
	if(timecount!=0)
	{
		*bus_addr=~tab_motor[motor_phase]<<3;
		//flag_rotation==1��ת
		if(motor_dir==1) 
			motor_phase=(motor_phase+1)%8;
		//flag_rotation==0��ת
		if(motor_dir==0) 
			if(motor_phase==0)
				motor_phase=7;
			else
				motor_phase--;
		timecount--;
	}
	
	TR0=1;
}

void Command_Schedule()
{
	//ɾ���Ѿ���ɵ�����
	do
	{
		cp->dir=(cp+1)->dir;
		cp->floor=(cp+1)->floor;
		cp++;
	}
	while(cp->dir!=0x00);
	//ȡ��һ������
	cp=command_list;
}

void ElevatorState_Disp()
{
	//elevator_floor;motor_dir;
	SdCmd(0x01);
	SdData(elevator_floor+'0');//¥��
	SdData(' ');

	if(elevator_floor!=(cp->floor-'0'))
	{
		if(motor_dir==0)
			SdData(0x19);//�½�0x19
		else
			SdData(0x18);//����0x18
	}

	switch(elevator_floor)
	{
		case 1:
			SdCmd(0x8f);
			break;
		case 2:
			SdCmd(0x97);
			break;
		case 3:
			SdCmd(0x87);
			break;
		default:
			break;
	}
	SdData(0x08);
}