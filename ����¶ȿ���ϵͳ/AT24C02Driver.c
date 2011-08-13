//#include <REG52.H>
#include <INTRINS.H>
#include "AT24C02Driver.h"

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

void delay(void);
void i_start(void);
void i_stop(void);
bit i_clock(void);
bit i_send(unsigned char i_data);
unsigned char i_receive(void);

void delay(void)
{
  _nop_();
  _nop_();
  _nop_();
  _nop_();
  _nop_();
  _nop_();
}

void i_start(void)
{
  SCL_1;
  delay();
  SDA_0;
  delay();
  SCL_0;
  delay();
}

void i_stop(void)
{
  SDA_0;
  delay();
  SCL_1;
  delay();
  SDA_1;
  delay();
  SCL_0;
  delay();
}

bit i_clock(void)
{
  bit sample;

  SCL_1;
  delay();
  sample=SDA;
  _nop_();
  _nop_();
  SCL_0;
  delay();
  return(sample);
}


bit i_send(unsigned char i_data)
{
  unsigned char i;

  for(i=8;i>0;i--)
    {
      SDA=(bit)(i_data & 0x80);
      i_data=i_data<<1;
      i_clock();
    }
  SDA_1;
  return(~i_clock());
}

unsigned char i_receive(void)
{
  unsigned char i_data=0;
  unsigned char i;

  for(i=8;i>0;i--)
    {
      i_data*=2;
      if(i_clock()) i_data++;
    }
  return(i_data);
}



unsigned char at24c02byteread(unsigned char address)
{
  unsigned char  retdata;

  i_start();//Æô¶¯I2C×ÜÏß
  if(i_send(0xa0))
    {
      if(i_send(address))
        {
          i_start();
          if(i_send(0xa1))
            {
              retdata=i_receive();
              i_stop();
              delay();
			        return(retdata);
            }
        }
    }
}


void at24c02bytewrite(unsigned char address,unsigned char writedata)
{
  i_start();
  if(i_send(0xa0))
    {
      if(i_send(address))
        {
          if(i_send(writedata))
            {
              i_stop();
              delay();
            }
        }
    }
}
