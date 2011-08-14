//file name: 8259A.c
#include "8259A.H"
#include "CH452.H"
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

unsigned char Key_Val_Tab[4][4]={'7','8','9','+',
						  		 '4','5','6','-',
						  		 '1','2','3','*',
						  		 '0','.','=','/'};

extern uchar flag_disp,flag_10s;

//8259A�жϳ�ʼ��
void Init8259a()
{
	ADR_8259AE = ICW1;
	ADR_8259AO = ICW2;
	ADR_8259AO = ICW4;

	ADR_8259AO = OCW1;
	ADR_8259AE = OCW2;
}

//8259A�ж�ʶ��
void Inter8259a() interrupt 2
{	
	uchar int_numb;
	int_numb=ADR_8259AE;
	int_numb=ADR_8259AE;
	int_numb=int_numb&0x07;
	switch(int_numb)
	{
		case 2:Server_Key();
			break;
		default:
			break;
	}
	ADR_8259AE = OCW2;		
}

//�����жϷ������
void Server_Key()
{
	uchar key_value;
	key_value=CH452_Read()-0x40;
	key_value=Key_Val_Tab[3-key_value/8][3-key_value%4];
	
	if(key_value=='0') //����'0'�����л���ʾ��ʽ
	{
		flag_disp=!flag_disp; //������ʶ��ʾ��ʽ��0:�ַ���ʾ����0:������ʾ
		if(flag_disp==1) //��������ʾʱ
			flag_10s=1; //�˳������жϺ�������ʾ
	}
}

