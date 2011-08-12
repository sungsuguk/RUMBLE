//File Name: main.C
//Date: 2010-08-07
//Changed By: Bovey
//



//-----------------------函数声明，变量定义--------------------------------------------------------
#include <reg52.h>
#include <intrins.h>
#include "ch452.h"
#include "IIC.H"

sbit KEY_0 = P1^5;
sbit KEY_1 = P1^6;
sbit KEY_2 = P1^7;

#define SW4 KEY_0
#define SW5 KEY_1
#define SW6 KEY_2
#define VREF 4.1
#define VOLT 1.601

#define uchar unsigned char
//sbit SDA=P1^1;                          // 将p1.1口模拟数据口
//sbit SCL=P1^0;                          // 将p1.01口模拟时钟口
#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};                  
bit   bdata SystemError;                // 从机错误标志位
//-----------------------PCF8591专用变量定义--------------------------------------------------------
#define PCF8591_WRITE 0x90
#define PCF8591_READ  0x91
#define  NUM  4                      // 接收和发送缓存区的深度
unsigned char idata receivebuf[NUM];    // 数据接收缓冲区

//--------------------------------------------------------------------------------------------------
// 函数名称： DAC_PCF8591
// 入口参数： controlbyte控制字,wdata待发送的数据
// 函数功能： 向D/A发送一个数据
//--------------------------------------------------------------------------------------------------
void DAC_PCF8591(unsigned char controlbyte,unsigned char wdata)
{    

  i_start();
  if(i_send(PCF8591_WRITE))
    {
      if(i_send(controlbyte&0x77))
        {
          if(i_send(wdata))
            {
              i_stop();         // 全部发完则停止
 			  delayNOP();
 			  delayNOP();
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
// 函数名称： ADC_PCF8591
// 入口参数： controlbyte控制字
// 函数功能： 读入A/D转换结果到receive_da
//--------------------------------------------------------------------------------------------------
unsigned char ADC_PCF8591(unsigned char controlbyte)
{ 
    unsigned char receive_da = 0,i=0;


   i_start();
   if(i_send(PCF8591_WRITE))		//控制字
   {
      if(i_send(controlbyte))		//控制字
      {
		  i_start();
          if(i_send(PCF8591_READ))
          {
          		delay();delay();
  			   	receive_da = i_receive();
				i_slave_NOACK();    // 收到一个字节后发送一个应答位
			  			  
			  	i_stop();
				return receive_da;
          }
      }
   }
	return receive_da;
}

//--------------------------------------------------------------------------------------------------
// 函数名称： Auto_ADC_PCF8591
// 入口参数： controlbyte控制字
// 函数功能： 连续读入4路通道的A/D转换结果到receivebuf
//--------------------------------------------------------------------------------------------------
void Auto_ADC_PCF8591(unsigned char controlbyte)
{ 
    unsigned char receive_da = 0,i=0;


   i_start();
   if(i_send(PCF8591_WRITE))		//控制字
   {
      if(i_send(controlbyte))		//控制字
      {
		  i_start();
          if(i_send(PCF8591_READ))
          {
			while(i<4)
			{
          		delay();delay();
  			   	receivebuf[i++] = i_receive();
				i_slave_NOACK();    // 收到一个字节后发送一个应答位
			}  			  
			  	i_stop();
          }
      }
   }
}

//--------------------------------------------------------------------------------------------------
// 函数名称： main
// 函数功能： 主程序
//--------------------------------------------------------------------------------------------------
void delays (unsigned char times);
void delayms (unsigned int times);
void Display(unsigned char * DisDate);	
void main()
{
	unsigned char *DisDate;
	unsigned char date[8] = {0,0,0,0x1E,0x1E,0x1E,0x1E,0x1E};
	unsigned char key_val = 0;
	unsigned char key_val_old = 0;
	unsigned char Vol_ain[5] = 0;
	unsigned char Vol_out = 0;
	unsigned char i,j = 0;
	unsigned int value;



	delayms(50);	
	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	//BCD译码,8个数码管
	DisDate = date;
	Display(DisDate);				//清除数码管显示数字

	while(1)
	{
	//	Vol_out = key_val;
		for(i = 0; i < 200; i++)
		{ 
			key_val_old = key_val;
			if (!SW6)
			{
			delayms(10);
			while (!SW6);
			delayms(10);
			key_val -= 5;
			if(key_val > 150) 
				key_val = 0;
			}
	
			if (!SW4)
			{
				delayms(10);
				while (!SW4);
				delayms(10);
				key_val += 5;
				if(key_val > 150) 
				key_val = 150;
			}
			
			if(key_val != key_val_old)
			{

				Vol_out = key_val;
				i = 201;						
			}

			delayms(10);
		}

		DAC_PCF8591(0x40,Vol_out); 	//output
		delayms(100);
		Vol_ain[0] = ADC_PCF8591(0x40);	//input
		delayms(100);
		Vol_ain[1] = ADC_PCF8591(0x40);	//input
		delayms(100);
		Vol_ain[2] = ADC_PCF8591(0x40);	//input
		delayms(100); 
		Vol_ain[3] = ADC_PCF8591(0x40);	//input
		delayms(100);

		Vol_ain[4] = (Vol_ain[0] + Vol_ain[1] + Vol_ain[2] + Vol_ain[3]) >> 2;

		Vol_ain[4] = Vol_ain[4]*2;
   		DisDate[4]	= Vol_ain[4]%10;
		DisDate[5]	= (Vol_ain[4]/10)%10;
		DisDate[6]	= (Vol_ain[4]/100);
  		
		value = DisDate[6]*16039 + DisDate[5]*1603 + DisDate[4]*160;

	//	value = value * 2;

		DisDate[0]	= (unsigned char)(value/10)%10;
		DisDate[1]	= (unsigned char)((value/100)%10);
		DisDate[2]	= (unsigned char)((value/1000)%10);
		DisDate[3]	= (unsigned char)(value/10000)|0x80;
		
		Display(DisDate);	//display
		

	}
		
}
void Display(unsigned char * DisDate)
{
//	CH452_Write(CH452_DIG0 | DisDate[0]);
	CH452_Write(CH452_DIG0 | DisDate[0]);
	CH452_Write(CH452_DIG1 | DisDate[1]);
	CH452_Write(CH452_DIG2 | DisDate[2]);
	CH452_Write(CH452_DIG3 | DisDate[3]);
	CH452_Write(CH452_DIG4 | DisDate[4]);
	CH452_Write(CH452_DIG5 | DisDate[5]);
	CH452_Write(CH452_DIG6 | DisDate[6]);
	CH452_Write(CH452_DIG7 | DisDate[7]);
}



#define CLK	5 	//if the clock is 12MHz
//#define CLK 10 //if the clock is 6MHz

//times is from 0 to 65535
void delayms (unsigned int times)
{
	uchar onems = CLK;
	uchar ms = 200;
	for (; times > 0; times --)
	{
		for (onems = CLK; onems > 0; onems --)
		{
			ms = 200;
			while (ms --);
		}
	}
}


//times is from 0 to 255
void delays (uchar times)
{
	uchar onescd = 5;
	for (;times > 0; times --)			//Delay 'times' second.
	{
		for (onescd = 5;onescd > 0; onescd --)	//Delay one second.
		{
			delayms(200);
		}
	}
}
