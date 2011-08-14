//ͷ�ļ�����
#include "DS18B20.H"
#include "CH452.H"
#include "LCD12864.H"
#include "AT24C02Driver.H"
#include "8259a.h"
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

//Ӳ�����
sbit BUZZER=P3^5; //�������Ŀ�������

#define LED_ADDR 0x4000 //LED�Ƶĵ�ַ
unsigned char xdata *led=LED_ADDR;

#define LS377_ADDR 0x2000 //LS377�ĵ�ַ
unsigned char xdata *ls377=LS377_ADDR;

//��������
void Sys_Init(); //ϵͳ��ʼ��
void Temp_Read(); //��ȡ�¶Ȳ�����
void Disp_Char(); //�ַ���ʾ
void Disp_Curve(); //������ʾ
void Draw_Axis(); //��������
void Buzz_and_Heat(); //�����ͼ���

//��������
uchar TimeCount_1s; //�жϴ���
uchar flag_1s; //�Ѿ���ʱ1s�ı�־
uchar TimeCount_10s; //�Ѿ���ʱ1s �ĸ���
uchar flag_10s; //�Ѿ���ʱ10s�ı�־

uchar Temp_Int,Temp_Dec; //�ֱ����¶ȵ��������ֺ�С�����֣�����Temp_Int�����¶ȵ��жϺʹ洢
uchar Disp[6]={0x00}; //�����ʾ���¶�ֵ��4λ����+1λС����+1��������־0x00

uchar flag_disp=0; //�ַ���ʾ��������ʾ�����ֱ�־���ɰ����жϷ�������޸�

#define E2PROM_BEGIN 10 //�洢����ʼ��ַ
#define E2PROM_END 119 //�洢��������ַ���ܹ��洢110�����µ��¶�ֵ
uchar e2prom_add=E2PROM_BEGIN; //�洢�¶ȵĴ洢��Ԫ�ĵ�ַ

//����ԭ��
#define X0 16
#define Y0 50
//20���Ӧ��������
#define Y20 40

//������
void main()
{
	//ϵͳ��ʼ��
	Sys_Init();
	while(1)
	{
		if(flag_1s==1) //�Ѿ���ʱ 1s ��?
		{
			Temp_Read(); //��ȡ�¶�
			if(flag_disp==0) //�ַ���ʾ?
				Disp_Char(); //�ַ���ʾ
			flag_1s=0; //�ȴ���һ��1s
			Buzz_and_Heat(); //�����ͼ���
		}
		if(flag_10s==1) //�Ѿ���ʱ 10s ��?
		{
			e2prom_add++; //��Ҫ�洢�¶ȵĴ洢��Ԫ�ĵ�ַ���Ӵ洢���ĵڶ����洢��Ԫ��ʼ�洢
			if(e2prom_add==E2PROM_END+1) //����洢���Ѿ��洢��һ��
				e2prom_add=E2PROM_BEGIN; //�����ʼ�����Ǵ洢
			at24c02bytewrite(e2prom_add,Temp_Int); //�洢�¶�
			if(flag_disp==1) //������ʾ?
				Disp_Curve(); //������ʾ
			flag_10s=0; //�ȴ���һ��10s
		}
	}
}

//ϵͳ��ʼ��
void Sys_Init()
{
	//DS18B20��ʼ��
	while(OW_Init()); //�鿴DS18B20�Ƿ����
	//LCD12864��ʼ��
	initLCM();
	ClearRAM();
	//24C02��ʼ��
	at24c02bytewrite(E2PROM_BEGIN,0x00); //��һ���洢��Ԫ�洢0x00�����ڱ�ʶ�洢��û�д���
	//CH452��ʼ��
	CH452_Write(CH452_SYSON2);
	CH452_Write(CH452_BCD);
	//8259A��ʼ��
	Init8259a();
	//��ʱ��1��ʼ��
	TMOD=0x10; //��ʱ��1������ģʽ1
	TH1=(65536-50000)/256;
	TL1=(65536-50000)%256; //���ö�ʱ��ֵ��ÿ50ms�ж�һ��
	//Buzzer��ʼ��
	BUZZER=1; //�رշ�����
	//Relay��ʼ��
	*ls377=0x80; //�Ͽ��̵���
	//LED�Ƴ�ʼ��
	*led=0xff; //Ϩ��LED

	EX1=1; //�ⲿ�ж�1ʹ��
	ET1=1; //��ʱ���ж�1ʹ��
	EA=1; //���ж�ʹ��
	TR1=1; //������ʱ��1
}

//��ʱ��1�жϷ������
void Timer1_ISR(void) interrupt 3
{
	TR1=0;
	TimeCount_1s=(TimeCount_1s+1)%20; //1s��Ҫ�ж�20��
	if(!TimeCount_1s) //��ʱ1s��?
	{
		flag_1s=1; //��ʱ1s��־��1
		TimeCount_10s=(TimeCount_10s+1)%10; //10=10*1s
		if(!TimeCount_10s) //��ʱ10s��?
			flag_10s=1; //��ʱ10s��־��1
   	}
	//���ö�ʱ��ֵ
	TH1=(65536-50000)/256;
	TL1=(65536-50000)%256;
	TR1=1;
}

//��ȡ�¶�
void Temp_Read()
{
	unsigned char lsb,msb; //ֱ�ӽ���DS18B20������¶ȵĵ��ֽڡ����ֽ�
	//�¶Ȳɼ�
	if(~OW_Init()) //��ʼ��
	{
		Write_Byte(0xCC); //ROM�������ROM
		Write_Byte(0x44); //DS18B20������������¶�ת��
		while(~Read_Bit()); //�¶�ת����Ϻ�DS18B20�ᷢ��Ӧ��λ1
		if(~OW_Init()) //��ʼ��
		{
			Write_Byte(0xCC); //ROM�������ROM
			Write_Byte(0xBE); //DS18B20���������RAM
			lsb=Read_Byte(); //��ȡ�¶�ֵ�ĵ��ֽ�
			msb=Read_Byte(); //��ȡ�¶�ֵ�ĸ��ֽ�
			while(OW_Init()); //��ʼ��
		}
   	}
	//����ת��
	Temp_Int=(msb<<4|lsb>>4); //�¶���������
	Temp_Dec=(lsb&0x0f)*0.0625*100; //�¶�С�����֣�����С�������λ��Ч����
}

//�ַ���ʾ
void Disp_Char()
{
	//���ݴ���
	Disp[0]=Temp_Int/10+'0'; //ʮλ
	Disp[1]=Temp_Int%10+'0'; //��λ
	Disp[2]='.'; //С����
	Disp[3]=Temp_Dec/10+'0'; //С������һλ
	Disp[4]=Temp_Dec%10+'0'; //С�����ڶ�λ

	SdCmd(0x30);
	SdCmd(0x01);
	delayms(2);
	PrintGB(0,0,"��ǰ�¶�:");
	PrintASCII(10,0,Disp);
	PrintGB(13,0,"��");
}

//������ʾ
void Disp_Curve()
{
	uchar tp; //Ҫ��ȡ�Ĵ洢��Ԫ
	uchar x_add; //�¶ȵ�ĺ�����
	uchar temp_data,temp_data_pre; //���ζ�ȡ���¶Ⱥ��ϴζ�ȡ���¶�

	ClearRAM(); //����
	Draw_Axis(); //����������
	tp=e2prom_add; //��e2prom_add��ʼ��ȡ
	x_add=X0+1;
	temp_data=at24c02byteread(tp); //��ȡ�洢���¶�
	tp--; //��һ��Ҫ��ȡ�Ĵ洢��Ԫ
	while(tp!=E2PROM_BEGIN) //һֱ����E2PROM_BEGIN֮��
	{
		temp_data_pre=temp_data; //���ϴζ�ȡ���¶���Ϊ���
		temp_data=at24c02byteread(tp); //���ζ�ȡ���¶���Ϊ�յ�
		Draw_Line(x_add+1,Y20-3*(temp_data-20),x_add,Y20-3*(temp_data_pre-20)); //����
		tp--; //��һ��Ҫ��ȡ�Ĵ洢��Ԫ
		x_add++;
	}
	if(at24c02byteread(E2PROM_BEGIN)!=0x00) //���E2PROM_BEGIN�д洢���¶ȣ��������ȡ
	{
		temp_data_pre=temp_data;
		temp_data=at24c02byteread(tp);
		Draw_Line(x_add+1,Y20-3*(temp_data-20),x_add,Y20-3*(temp_data_pre-20));
		x_add++;

		tp=E2PROM_END; //������E2PROM_END��ȡ
		while(tp!=e2prom_add) //һֱ����e2prom_add֮ǰ
		{
			temp_data_pre=temp_data;
			temp_data=at24c02byteread(tp);
			Draw_Line(x_add+1,Y20-3*(temp_data-20),x_add,Y20-3*(temp_data_pre-20));
			tp--;
			x_add++;
		}
	}
}

//����������
void Draw_Axis()
{
	Draw_Line(X0,Y0,X0+110,Y0); //ʱ���ᣬ110�����ص�
	Draw_Line(X0,Y0,X0,Y0-50); //�¶��ᣬ50�����ص�
	//6���¶ȿ̶ȣ��̶ȼ����6�����ص㣬��ʾ2��
	Draw_Dot(X0+1,Y20);
	Draw_Dot(X0+1,Y20-6);
	Draw_Dot(X0+1,Y20-12);
	Draw_Dot(X0+1,Y20-18);
	Draw_Dot(X0+1,Y20-24);
	Draw_Dot(X0+1,Y20-30);
	//5��ʱ��̶ȣ��̶ȼ����20�����ص㣬ÿ�����ص��ʾ10s
	Draw_Dot(X0+20,Y0-1);
	Draw_Dot(X0+40,Y0-1);
	Draw_Dot(X0+60,Y0-1);
	Draw_Dot(X0+80,Y0-1);
	Draw_Dot(X0+100,Y0-1);
	//�¶ȷ�Χ20~30
	PrintASCII(8,0,"20");
	PrintASCII(0,0,"30");
}

//�����ͼ���
void Buzz_and_Heat()
{
	unsigned char i;
	//��ˮ�³����趨�ķ�Χʱ��ʹ�÷��������б�����ʾ
	if(Temp_Int>=30||Temp_Int<20)
	{
		//����
		i=100;
		while(i--)
		{
			BUZZER=0; //�򿪷�����
			delayus(100);
			BUZZER=1; //�رշ�����
			delayus(100);
		}
   	}
	//��ˮ�³���һ���ķ�Χʱ��ͨ���̵����Ŀ���������׵�ˮ���м��ȣ�����LED��ָʾ����״̬
	else if(Temp_Int<25)
	{
		//����
		*ls377=0x00; //�պϼ̵������أ�����
		*led=0xaa; //LED��Ƶ�������ʾ���ڼ���
	}
	else
	{
		//ֹͣ����
		*ls377=0x80; //�Ͽ��̵������أ�ֹͣ����
		*led=0x55; //LED���Ƶ�������ʾ�¶�������û�м���
	}
}

