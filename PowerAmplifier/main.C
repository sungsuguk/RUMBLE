//File Name: main.C
//Date: 2010-08-10
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
#define VCC 4700
#define Visb VCC/256

#define uchar unsigned char
//sbit SDA=P1^1;                          // 将p1.1口模拟数据口
//sbit SCL=P1^0;                          // 将p1.01口模拟时钟口
//#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};   
#define delayNOP();            
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
unsigned char code sinTab[]={  //正弦波代码数组
0x14,0x17,0x19,0x1B,0x1E,0x20,0x22,0x23,0x25,0x26,
0x27,0x28,0x28,0x28,0x28,0x27,0x26,0x25,0x23,0x22,
0x20,0x1E,0x1B,0x19,0x17,0x14,0x11,0x0F,0x0D,0x0A,
0x08,0x06,0x05,0x03,0x02,0x01,0x00,0x00,0x00,0x00,
0x01,0x02,0x03,0x05,0x06,0x08,0x0A,0x0D,0x0F,0x11
};
//0-49


void delayms (unsigned int times);
void Display(unsigned char * DisDate);
	
void main()
{
//	unsigned char *DisDate;
	unsigned char date[8] = {0};
	unsigned char i,n = 1;
	unsigned char delayus;
	unsigned int Voltage = 0;
	delayms(50);	
//	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
//	CH452_Write(CH452_BCD);   	//BCD译码,8个数码管
//	DisDate = date;
//	Display(DisDate);				//清除数码管显示数字

	while(1)
	{
		for (i = 0; i < 50; )
		{
			if (!SW5)
			{
				delayms(3);
				while (!SW5);
				delayms(3);
				n += 1;
				if(n > 5) n = 1;
			}

			DAC_PCF8591(0x40,sinTab[i]); 	//output

			if (!SW4)	//stop ,start
			{
				delayms(3);
				while (!SW4);
				delayms(3);
				while (SW4);
				delayms(3);
				while (!SW4);
				delayms(3);
			}
			i += n;
		}
	}
		
}
void Display(unsigned char * DisDate)
{
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
//0.1ms * times
void delayms (unsigned int times)
{
	uchar onems = CLK;
	uchar ms = 100;
	for (; times > 0; times --)
	{
		for (onems = CLK; onems > 0; onems --)
		{
			ms = 100;
			while (ms --);
		}
	}
}

