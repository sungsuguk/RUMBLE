#include<ch452.h>
#include<pcf8591.h>
#include<absacc.h>
#define uchar unsigned char
#define uint unsigned int
#define oddaddr XBYTE[0x6001]//8259A��ַ����
#define evenaddr XBYTE[0x6000]
#define Vref 4.54//PCF8591�ο���ƽ����
uchar flag_ldo=0,flag_clear=1,flag_node=5;//���Ƹ��������ı�־λ
void delay(uint ms)//��ʱ����
{
uint i;
while(ms--)
for(i=0;i<1000;i++);

}
xdata uchar buffer[5];
/*
code uchar key_table[4][4]={{0x0f,0x20,0x0a,0x09},
                            {0x0b,0x08,0x7,0x06},
							{0x07,0x05,0x04,0x03},
							{0x21,0x02,0x01,0x00}};//��������ֵ
							*/
code unsigned char key_table[4][4]={{7,8,9,'+'},
                            {4,5,6,'-'},
							{1,2,3,'*'},
							{0,'.','=','/'}};//��������ֵ

const int DigCode[8]={CH452_DIG0,CH452_DIG1,CH452_DIG2,CH452_DIG3,
                       CH452_DIG4,CH452_DIG5,CH452_DIG6,CH452_DIG7};//����ܱ�

uint num[6]={0},n=0;
uint num1=0;//�����ѹֵ

/********�жϳ�ʼ��**********************/

void Int_init(void)
{
evenaddr=0x13;//ICW1
oddaddr=0x00; //ICW2
oddaddr=0x07;//ICW4
EX1=1;//��Ƭ���ⲿ�ж�1����
EA=1;

}
void CH452_Display_Init(void)//ch452��ʼ��   
{   
        CH452_SDA_SET;   
        CH452_SCL_SET;   
        CH452_Write(CH452_RESET);           //��λ��ʾ    
        delay(30);   
        CH452_Write(CH452_BCD);             //������ʾ��ʽ    
        CH452_Write(CH452_SYSON2);//����ʾ�ͼ���   
   
} 



/**************�����жϷ�����*****************************/
void server_2(void)
{uchar keycode,a,b,key_num;
uint m=1;
keycode=CH452_Read();
a=keycode>>3&0x07;
b=keycode&0x07;
key_num=key_table[3-a][3-b];
if(key_num>=0&&key_num<=9||key_num=='.')//��������ֵΪ0~9��ֵ��С����
{ if(key_num!='.')
  {
  num[n]=key_num;
  n++;
  num1=num1*10+key_num;
  }
  else {flag_node=n-1;}//���С����λ��
}
 

if(key_num=='=')//��ѹת��
{
	flag_ldo=5;
 	for(a=n-1;a>flag_node;a--)//����ֵת��С��ʱҪ���Ե���
 		m=m*10;
 	num1=num1*(1000/m);//��õ��ĵ�ѹֵ������1000��
// 	flag_clear=0;
}
if(key_num=='/')//����
	flag_clear=1;
}

/********************************/

void Int1(void)interrupt 2 
{
	uchar Int_num;
	EA=0;
	Int_num=oddaddr;//���жϺ�
	Int_num=oddaddr;
	Int_num=Int_num&0x07;

	switch(Int_num)
 	{
		//case 0x00:server_0();break;
  		//case 0x01:server_1();break;
  		case 0x02:server_2();break;//�����жϳ������
  		//case 0x03:server_3();break;
  		//case 0x04:server_4();break;
  	}
	EA=1;
}

/**********************************************/
void main()
{
	uchar temp,t,i;
	uint temp1;
	Int_init();//�жϳ�ʼ��
	CH452_Display_Init();//ch452��ʼ��
	DAC_PCF8591(0x40,75);//дһ��ֵ��8591
	temp=ADC_PCF8591(0x40);//��ȡ��ѹת����������Ϣ

	while(1)
	{
		if(flag_ldo) //������ѹ�任����                          
		{
			temp=ADC_PCF8591(0x40);//��ȡ�˿̵�ѹת����������Ϣ
			temp1=(Vref*temp*2*1000)/256;//����������Ϣ�õ���ѹֵ
			t=temp;

			while((temp1+4)<num1&&flag_clear==0)//��ʱ��ѹֵС������ĵ�ѹֵ
			{
				//if(flag_clear){break;}
				t++;//����ֵ��������
				DAC_PCF8591(0x40,t);//����������Ϣ
				temp=ADC_PCF8591(0x40);//���Ϸ�������
				temp1=(Vref*temp*2*1000)/256;
				CH452_Write(DigCode[0]|temp1%10);//��ʾ����
				CH452_Write(DigCode[1]|temp1%100/10);
				CH452_Write(DigCode[2]|temp1%1000/100);
				CH452_Write(DigCode[3]|temp1/1000|0x80);
			}

			while(temp1>(num1+8)&&flag_clear==0)//��ʱ��ѹֵ��������ĵ�ѹֵ
			{
				//if(flag_clear){break;}
				t--;//����ֵ���ϼ�С
				DAC_PCF8591(0x40,t);//����������Ϣ
				temp=ADC_PCF8591(0x40);//���Ϸ�������
				temp1=(Vref*temp*2*1000)/256;
				CH452_Write(DigCode[0]|temp1%10);
				CH452_Write(DigCode[1]|temp1%100/10);
				CH452_Write(DigCode[2]|temp1%1000/100);
				CH452_Write(DigCode[3]|temp1/1000|0x80);
			}

			flag_ldo--;
		}
		if(flag_clear==1)
		{
			for(i=0;i<n;i++)//����������ֵ������������
				num[i]=0;
  			n=0;
  			flag_node=5;
  			num1=0;
 			CH452_Write(CH452_NO_BCD);//ch452ת����NO_BCD��ʽ����������0�͵��ڿ����䲻��
 			for(i=0;i<8;i++)//�����Ϩ��
 				CH452_Write(DigCode[i]|0);
 			CH452_Write(CH452_BCD);
  			CH452_Write(DigCode[4]|0);
			flag_clear=0;
			flag_ldo=0;
 		}
		for(i=0;i<n;i++)//������ֵ��ʾ����
			if(flag_node==(n-1-i))//��С�����λ��
				CH452_Write(DigCode[i+4]|num[n-1-i]|0x80);
			else CH452_Write(DigCode[i+4]|num[n-1-i]);
}
	}

