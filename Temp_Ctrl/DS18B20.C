#include "DS18B20.H"

void delayus(unsigned int t)
{
	while(t--);
}

bit OW_Init()
{
	bit presence;
	DQ=1;
	delayus(2);
	DQ=0;
	delayus(600);
	DQ=1;
	delayus(60);
	presence=DQ;
	delayus(500);
	return(presence);
}

void Write_Bit(bit wbit)
{
	DQ=1;
	delayus(2);
	DQ=0;
	delayus(2);
	if(wbit==1) DQ=wbit;
	delayus(90);
	DQ=1;
	delayus(2);
}

void Write_Byte(unsigned char wbyte)
{
	unsigned char i,wbit;
	for(i=0;i<8;i++)
	{
		wbit=(bit)(wbyte&0x01);
		Write_Bit(wbit);
		wbyte>>=1;
	}
}

bit Read_Bit(void)
{
	bit rbit;
	DQ=1;
	delayus(2);
	DQ=0;
	delayus(2);
	DQ=1;
	delayus(2);
	rbit=DQ;
	delayus(90);
	DQ=1;
	delayus(2);
	return(rbit);
}

unsigned char Read_Byte(void)
{
	unsigned char rbyte=0,i;
	for(i=0;i<8;i++)
	{
		rbyte>>=1;
		if(Read_Bit())
		rbyte|=0x80;
	}
	return(rbyte);
}