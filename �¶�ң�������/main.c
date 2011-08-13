//file name:Main.c
//ͷ�ļ�����
#include "DS18B20.H"
#include "CH452.H"
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
 
//���Ŷ���
sbit BUZZER=P3^5;
//LED�Ƶĵ�ַ
#define LED_ADDR 0x4000
unsigned char xdata *led=LED_ADDR;
//LS377�ĵ�ַ
#define LS377_ADDR 0x2000
unsigned char xdata *ls377=LS377_ADDR;
 
//��������
void Sysrem_Init();
void Temp_Read();
void DIG_Disp();
void Buzz_and_Heat();
void Clear_Dis();
 
//��������
unsigned char Temp_Int,Temp_Dec;
unsigned char Disp[4]={0x00};
 
//������
void main(void)
{
	//ϵͳ��ʼ��
	Sysrem_Init();
	while(1)
	{
		//��ȡ�¶Ȳ�����
		Temp_Read();
		//�������ʾ
		DIG_Disp();
		//�����ͼ���
		Buzz_and_Heat();
	}
}
 
//ϵͳ��ʼ��
void Sysrem_Init()
{
	//DS18B20��ʼ��
	while(OW_Init());
 
	//CH452��ʼ��
	CH452_Write(CH452_SYSON1);
	CH452_Write(CH452_BCD);
	Clear_Dis();
	CH452_Write(CH452_DIG0|0|0x80);
 
	//Buzzer��ʼ��
	BUZZER=1;
 
	//Relay��ʼ��
	*ls377=0x80;
 
	//LED�Ƴ�ʼ��
	*led=0xff;
}
 
//��ȡ�¶Ȳ�����
void Temp_Read()
{
	unsigned char lsb,msb;
	//�¶Ȳɼ�
	if(~OW_Init())
	{
		Write_Byte(0xCC);
		Write_Byte(0x44);
		while(~Read_Bit());
		if(~OW_Init())
		{
			Write_Byte(0xCC);
			Write_Byte(0xBE);
			lsb=Read_Byte();
			msb=Read_Byte();
			while(OW_Init());
		}
   	}
	//���ݴ���
	Temp_Int=(msb<<4|lsb>>4);
	Temp_Dec=(lsb&0x0f)*0.0625*100;
	Disp[3]=Temp_Int/10;
	Disp[2]=Temp_Int%10;
	Disp[1]=Temp_Dec/10;
	Disp[0]=Temp_Dec%10;
}
 
//�������ʾ
void DIG_Disp()
{
	//��ʾ�¶�
	CH452_Write(CH452_DIG3|Disp[3]);
	CH452_Write(CH452_DIG2|Disp[2]|0x80);
	CH452_Write(CH452_DIG1|Disp[1]);
	CH452_Write(CH452_DIG0|Disp[0]);
}
 
//��������
void Buzz_and_Heat()
{
	unsigned char i;
	//���¶ȳ����趨�ķ�Χʱ��ʹ�÷�����������ʾ
	if(Temp_Int>=30||Temp_Int<20)
	{
		//����
		i=100;
		while(i--)
		{
			BUZZER=0;
			delayus(100);
			BUZZER=1;
			delayus(100);
		}
   	}
	//���¶ȳ���һ���ķ�Χʱ��ͨ���̵����Ŀ��ؽ��м��ȣ�����LED��ָʾ����״̬
	else if(Temp_Int<25)
	{
		//����
		*ls377=0x00;
		*led=0xaa;
	}
	else
	{
		//ֹͣ����
		*ls377=0x80;
		*led=0x55;
	}
}
 
//����������ʾ
void Clear_Dis()
{
	CH452_Write(CH452_NO_BCD);
	CH452_Write(CH452_DIG0|0);
	CH452_Write(CH452_DIG1|0);
	CH452_Write(CH452_DIG2|0);
	CH452_Write(CH452_DIG3|0);
	CH452_Write(CH452_DIG4|0);
	CH452_Write(CH452_DIG5|0);
	CH452_Write(CH452_DIG6|0);
	CH452_Write(CH452_DIG7|0);
	CH452_Write(CH452_BCD);
}
