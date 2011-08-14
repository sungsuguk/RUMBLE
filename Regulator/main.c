#include<ch452.h>
#include<pcf8591.h>
#include<absacc.h>
#define uchar unsigned char
#define uint unsigned int
#define oddaddr XBYTE[0x6001]//8259A地址定义
#define evenaddr XBYTE[0x6000]
#define Vref 4.54//PCF8591参考电平定义
uchar flag_ldo=0,flag_clear=1,flag_node=5;//控制各个函数的标志位
void delay(uint ms)//延时函数
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
							{0x21,0x02,0x01,0x00}};//按键编码值
							*/
code unsigned char key_table[4][4]={{7,8,9,'+'},
                            {4,5,6,'-'},
							{1,2,3,'*'},
							{0,'.','=','/'}};//按键编码值

const int DigCode[8]={CH452_DIG0,CH452_DIG1,CH452_DIG2,CH452_DIG3,
                       CH452_DIG4,CH452_DIG5,CH452_DIG6,CH452_DIG7};//数码管表

uint num[6]={0},n=0;
uint num1=0;//输出电压值

/********中断初始化**********************/

void Int_init(void)
{
evenaddr=0x13;//ICW1
oddaddr=0x00; //ICW2
oddaddr=0x07;//ICW4
EX1=1;//单片机外部中断1开启
EA=1;

}
void CH452_Display_Init(void)//ch452初始化   
{   
        CH452_SDA_SET;   
        CH452_SCL_SET;   
        CH452_Write(CH452_RESET);           //复位显示    
        delay(30);   
        CH452_Write(CH452_BCD);             //设置显示方式    
        CH452_Write(CH452_SYSON2);//打开显示和键盘   
   
} 



/**************按键中断服务函数*****************************/
void server_2(void)
{uchar keycode,a,b,key_num;
uint m=1;
keycode=CH452_Read();
a=keycode>>3&0x07;
b=keycode&0x07;
key_num=key_table[3-a][3-b];
if(key_num>=0&&key_num<=9||key_num=='.')//按键编码值为0~9数值或小数点
{ if(key_num!='.')
  {
  num[n]=key_num;
  n++;
  num1=num1*10+key_num;
  }
  else {flag_node=n-1;}//标记小数点位置
}
 

if(key_num=='=')//电压转换
{
	flag_ldo=5;
 	for(a=n-1;a>flag_node;a--)//求数值转换小数时要除以的数
 		m=m*10;
 	num1=num1*(1000/m);//想得到的电压值并扩大1000倍
// 	flag_clear=0;
}
if(key_num=='/')//清零
	flag_clear=1;
}

/********************************/

void Int1(void)interrupt 2 
{
	uchar Int_num;
	EA=0;
	Int_num=oddaddr;//读中断号
	Int_num=oddaddr;
	Int_num=Int_num&0x07;

	switch(Int_num)
 	{
		//case 0x00:server_0();break;
  		//case 0x01:server_1();break;
  		case 0x02:server_2();break;//按键中断程序控制
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
	Int_init();//中断初始化
	CH452_Display_Init();//ch452初始化
	DAC_PCF8591(0x40,75);//写一初值给8591
	temp=ADC_PCF8591(0x40);//读取电压转换的数字信息

	while(1)
	{
		if(flag_ldo) //控制稳压变换处理                          
		{
			temp=ADC_PCF8591(0x40);//读取此刻电压转换的数字信息
			temp1=(Vref*temp*2*1000)/256;//根据数字信息得到电压值
			t=temp;

			while((temp1+4)<num1&&flag_clear==0)//此时电压值小于输入的电压值
			{
				//if(flag_clear){break;}
				t++;//数字值不断增加
				DAC_PCF8591(0x40,t);//输入数字信息
				temp=ADC_PCF8591(0x40);//不断反馈调整
				temp1=(Vref*temp*2*1000)/256;
				CH452_Write(DigCode[0]|temp1%10);//显示控制
				CH452_Write(DigCode[1]|temp1%100/10);
				CH452_Write(DigCode[2]|temp1%1000/100);
				CH452_Write(DigCode[3]|temp1/1000|0x80);
			}

			while(temp1>(num1+8)&&flag_clear==0)//此时电压值大于输入的电压值
			{
				//if(flag_clear){break;}
				t--;//数字值不断减小
				DAC_PCF8591(0x40,t);//输入数字信息
				temp=ADC_PCF8591(0x40);//不断反馈调整
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
			for(i=0;i<n;i++)//按键编码数值接收数组清零
				num[i]=0;
  			n=0;
  			flag_node=5;
  			num1=0;
 			CH452_Write(CH452_NO_BCD);//ch452转换成NO_BCD形式，这样输入0就等于控制其不亮
 			for(i=0;i<8;i++)//数码管熄灭
 				CH452_Write(DigCode[i]|0);
 			CH452_Write(CH452_BCD);
  			CH452_Write(DigCode[4]|0);
			flag_clear=0;
			flag_ldo=0;
 		}
		for(i=0;i<n;i++)//输入数值显示控制
			if(flag_node==(n-1-i))//有小数点的位置
				CH452_Write(DigCode[i+4]|num[n-1-i]|0x80);
			else CH452_Write(DigCode[i+4]|num[n-1-i]);
}
	}

