//File Name: main.C
//Date: 2010-07-26
//Changed By: Bovey
//
//
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

#define VOLT 1.607

#define uchar unsigned char
//sbit SDA=P1^1;                          // 将p1.1口模拟数据口
//sbit SCL=P1^0;                          // 将p1.0口模拟时钟口
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
// 函数名称： ADC_PCF8591
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
	unsigned char date[8] = {0};
	unsigned char key_val = 0;
	unsigned char i;
	unsigned char volt;
	float value;

	delayms(50);	
	CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
	CH452_Write(CH452_BCD);   	//BCD译码,8个数码管
	DisDate = date;
	Display(DisDate);				//清数码管显示数字

	while(1)
	{

	for(i = 0; i < 150; i++)
	{ 
		if (!SW4)
		{
		delayms(10);
		while (!SW4);
		delayms(10);
		key_val++;
		if(key_val > 4) key_val = 0;
		}
		delayms(10);
	}
	receivebuf[0] = ADC_PCF8591(0x40);
	receivebuf[0] = ADC_PCF8591(0x40);
	receivebuf[1] = ADC_PCF8591(0x41);
	receivebuf[1] = ADC_PCF8591(0x41);
	receivebuf[2] = ADC_PCF8591(0x42);
	receivebuf[2] = ADC_PCF8591(0x42);
	receivebuf[3] = ADC_PCF8591(0x43);
	receivebuf[3] = ADC_PCF8591(0x43);
//	Auto_ADC_PCF8591(0x44);
//	 receivebuf[3] =receivebuf[2];
//	value = receivebuf[3]*1.607;
//	receivebuf[3] = (unsigned char) value;
			  
	if(key_val == 0)
	{
		value = receivebuf[0]*VOLT;
		receivebuf[0] = (unsigned char) value;
		volt = receivebuf[0];
		DisDate[3]	= 0;
	}else
	if(key_val == 1)
	{
		value = receivebuf[1]*VOLT;
		receivebuf[1] = (unsigned char) value;
		volt = receivebuf[1];
		DisDate[3]	= 1;
	}else
	if(key_val == 2)
	{
		value = receivebuf[2]*VOLT;
		receivebuf[2] = (unsigned char) value;
		volt = receivebuf[2];
		DisDate[3]	= 2;
	}else
	if(key_val == 3)
	{
		value = receivebuf[3]*VOLT;
		receivebuf[3] = (unsigned char) value;
		volt = receivebuf[3];
		DisDate[3]	= 3;
	}
		DisDate[0]	= volt%10;
		DisDate[1]	= (volt/10)%10;
		DisDate[2]	= (volt/100)|0x80;
	
	if(key_val == 4)
	{
		DisDate[3]	= 4;
		DisDate[0] = 0;	
		DisDate[1] = 0;
		DisDate[2] = 0;
		DAC_PCF8591(0x40,0); 	//output 0V
		Display(DisDate);
		while(SW4);
		delayms(5);
		DAC_PCF8591(0x40,65); 	//output 1V
		DisDate[0] = 1;
		Display(DisDate);
		while(!SW4);
		delayms(5);
		while(SW4);
		delayms(5);
		DAC_PCF8591(0x40,127); 	//output 2V
		DisDate[0] = 2;
		Display(DisDate);
		while(!SW4);
		delayms(5);
		while(SW4);
		delayms(5);
		DAC_PCF8591(0x40,254);	//output 4V
		DisDate[0] = 4;
		Display(DisDate);
		while(!SW4);
		delayms(5);
		while(SW4);
		delayms(5);
		while(!SW4);
		delayms(5);
		key_val = 0;
	}

	Display(DisDate);
	}
	
}
void Display(unsigned char * DisDate)
{
	CH452_Write(CH452_DIG7 | DisDate[7]);
	CH452_Write(CH452_DIG6 | DisDate[6]);
	CH452_Write(CH452_DIG5 | DisDate[5]);
	CH452_Write(CH452_DIG4 | DisDate[4]);
	CH452_Write(CH452_DIG3 | DisDate[3]);
	CH452_Write(CH452_DIG2 | DisDate[2]);
	CH452_Write(CH452_DIG1 | DisDate[1]);
	CH452_Write(CH452_DIG0 | DisDate[0]);
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
