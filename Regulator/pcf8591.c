#include<pcf8591.h>
#include<ch452.h>
/******************************************/
void delay()               
   {
     unsigned int i,j;
	  for(i=0;i<10;i++)
	    for(j=0;j<1000;j++)
	    	 ;
   }




unsigned char  PCF8591_I2c_RdByte(void) //读一个字节数据
{
unsigned char dat,i;
CH452_SDA_SET;
dat=0;
for(i=0;i!=8;i++)  // 输入8位数据
{
CH452_SCL_SET;
DELAY_1US;
DELAY_1US;
dat<<=1;
if(CH452_SDA_IN) dat++;  // 输入1位
CH452_SCL_CLR;
DELAY_1US;

}
 
return(dat);
}

void DAC_PCF8591(unsigned char controlbyte,unsigned char wdata)
{
  CH452_I2c_Start();
  CH452_I2c_WrByte(PCF851_WIRTE);
  CH452_I2c_WrByte(controlbyte&0x77);
  CH452_I2c_WrByte(wdata);
  CH452_I2c_Stop();
}
unsigned char ADC_PCF8591(unsigned char controlbyte)//字节读函数
{unsigned char Data;
 CH452_I2c_Start();
//if(CH452_I2c_WrByte(controlbyte&0x77))//delay();delay();
 //CH452_I2c_Start();
//
 CH452_I2c_WrByte(PCF851_READ);
 Data=CH452_I2c_RdByte();

 CH452_I2c_Stop();
 delay();
//
return(Data);
}
