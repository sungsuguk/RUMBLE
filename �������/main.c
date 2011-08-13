//#include<intrins.h>
#include "CH452.H" 
#include<absacc.h>
#define oddaddr XBYTE[0x6001]
#define evenaddr XBYTE[0x6000]

#define uchar unsigned char 
#define uint unsigned int
int flag=0;
//sbit sw4=P1^5;
//sbit sw5=P1^6;
//sbit sw6=P1^7;

const uint DigCode[8]={CH452_DIG0,CH452_DIG1,CH452_DIG2,CH452_DIG3,
                       CH452_DIG4,CH452_DIG5,CH452_DIG6,CH452_DIG7};

uchar table[8]={0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00};

code uchar key_table[4][4]={{0x15,0x14,0x13,0x12},//键值编码
                            {0x11,0x10,0x9,0x08},
							{0x07,0x06,0x05,0x04},
							{0x03,0x02,0x01,0x00}};

void delay_us(int n)
{
	int i,j;
	for(j=0;j<10;j++)
		for(i=0;i<n;i++)_nop_();
}


void Int_Init()//中断初始化
{
	evenaddr=0x13;//ICW1
	oddaddr=0x00;//ICW2 
	oddaddr=0x07;//ICW4
	TMOD=0x02;//T0初始化
	TH0=6;
	TL0=6;
	IE=0x86;
	//TR0=1;
}

/**********键盘中断服务程序***********/
void server_2(void)
{
	static uchar flag1=0;
	uchar keycode,num,i;

	keycode=CH452_Read();//获取键盘码
	num=key_table[keycode>>3&0x07][keycode&0x07];//查表获取键值

	switch(num)
	{
		case 0x01: //实现秒表的停与跑
			flag1++;
			if(flag1==1) TR0=1;
			if(flag1==2) {TR0=0; flag1=0;}
			break;

		case 0x00: //秒表清0
			TR0=0;
			for(i=0;i<8;i++) table[i]=table[i]&0x80;
			flag1=0;
			break;
	} 
}

/*****单片机下8259中断*************/
void Int1(void)interrupt 2 using 2
{
	uchar Int_num;//中断号

	EA=0;
	ACC=oddaddr;
	ACC=oddaddr;//第二个机器周期送中断向量号
	Int_num=ACC;

	switch(Int_num)
	{
//		case 0x00:server_0();break;
//		case 0x01:server_1();break;
		case 0x02:server_2();break;
//		case 0x03:server_3();break;
//		case 0x04:server_4();break;
  }

	EA=1;
}

/**************ch452初始化***************/ 
void CH452_Display_Init()   
{   
	CH452_SDA_SET;   
	CH452_SCL_SET;   
	CH452_Write(CH452_RESET);           //复位显示
	delay_us(30);
	CH452_Write(CH452_BCD);             //设置显示方式
	CH452_Write(CH452_SYSON2);          //打开显示和键盘
}

/*******************************************************/
void main()
{
	int i;
	Int_Init();
	CH452_Display_Init();

	while(1)
	{
		for(i=0;i<8;i++)
			CH452_Write(DigCode[i]|table[i]);//数码管显示
	}
}

void T0_Int(void)interrupt 1 using 1//定时器0，实现秒表
{
	int i;

	flag++;
	if(flag==40)
	{
		flag=0;//1
		table[0]++;
		if(table[0]>9)
		{
			table[1]++;//2
			table[0]=0;
			if(table[1]>9)
			{
				table[2]++;//3
				table[1]=0;
				if((table[2]&0x7f)>9)
				{
					table[3]++;//4
					table[2]=0x80;
					if(table[3]>5)
					{
						table[4]++;//5
						table[3]=0;
						if((table[4]&0x7f)>9)
						{
							table[5]++;//6
							table[4]=0x80;
							if(table[5]>5)
							{
								table[6]++;//7
								table[5]=0;
								if((table[6]&0x7f)>9)
								{
									table[7]++;//8
									table[6]=0x80;
									if(table[7]==2&&table[6]&0x7f==4)
										for(i=0;i<8;i++)
											table[i]=table[i]&0x80;
								}//8
							}//7
						}//6
					}//5
				}//4
			}//3
		}//2
	}//1
}

























