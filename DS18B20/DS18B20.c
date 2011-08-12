//File Name:DS18B20.C
//Date :2010-07-25
//By:Bovey



#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

#include "DS18B20.H"

sbit clk=P1^0; //p1.0 connect to clk
sbit dat=P1^1; //p1.1 connect to dat
sbit DQ = P3^4;

#define FALSE 0
#define TRUE 1

#define uchar unsigned char
#define uint  unsigned int

////////////////////////////////////////////////////////////////////////////
///////////////////// GLOBAL VARIABLES /////////////////////////////////////

unsigned char Temp_Integer;
unsigned char Temp_Decimal;
unsigned char Temp_Flag = 0;



// DELAY 
void delay(uchar i)
{
	 while(--i);
}

//delay a long time
void longDelay(uchar n)
{
	for (;n > 0;n--) 
	{
		delay(0);
	}
}


//////////////////////////////////////////////////////////////////////////////
// OW_RESET 
unsigned char ow_reset(void)
{
        unsigned char presence;
		DQ = 1;
		;
		;
   		DQ = 0;//pull DQ line low
		;
		;
		delay(250);// leave it low for 600us
		delay(100);
		DQ = 1;       // allow line to return high
		delay(30); // wait for presence40us
		presence = DQ;// get presence signal
		delay(100);// wait for end of timeslot 200us
   		return presence;// presence signal returned
}                            //     0=presence, 1 = no part

// READ_BIT 
unsigned char read_bit(void)
{
	unsigned char i;
	DQ = 1;
	;
	;
	DQ = 0;// pull DQ low to start timeslot
	;
	;
	DQ = 1;// then return high
	delay(1);//delay 7us
	i = DQ;
	return(i);// return value of DQ line
}

// READ_BYTE 
unsigned char read_byte(void)
{
	unsigned char i;
	unsigned char value = 0;
	for (i = 0; i < 8; i++)
	{
		if(read_bit()) value|= 0x01 << i;    // reads byte in, one byte at a time and then
//shifts it left
		delay(20);// wait for rest of timeslot delay45us
	}
	return(value);
}

// WRITE_BIT 
void write_bit(char bitval)
{
	DQ = 1;
	;
	;
	DQ = 0;// pull DQ low to start timeslot
	delay(2);
	if(bitval == 1) DQ = 1;// return DQ high if write 1
	delay(30);// hold value for remainder of timeslot 65us
	DQ = 1;
}// Delay provides 16us per loop, plus 24us. Therefore delay(5) = 104us

// WRITE_BYTE 
void write_byte(char val)
{
	unsigned char i;
	unsigned char temp;
	for (i = 0; i < 8; i++)            // writes byte, one bit at a time
	{
		temp = val >> i;          // shifts val right 'i' spaces
		temp &= 0x01;              // copy that bit to temp
		write_bit(temp);           // write bit in temp into
	}
	delay(30);
}

/*
//////////////////////read four ds18b20s' TH value///////
void read_th()
{
	write_byte(0xcc);	
	write_byte(0xbe);//write scratchpad command
	read_byte();
	read_byte();
	TH = read_byte();		
}
*/

////////////////////////////////////////////////////
/////////////////write TH value to ds18b20//////////
////////////////////////////////////////////////////
void write_th(unsigned char TH)
{	
	write_byte(0xcc);
 	write_byte(0x4e);//write scratchpad command
	write_byte(TH);
	ow_reset();
  	write_byte(0xcc);
    write_byte(0x48);//copy scratchpad command
    longDelay(20);//write TH's value to 18b20
}


void GetTemp ()
{
 	unsigned char temp_lsb,temp_msb;
  	Temp_Flag = 0;		//标识温度的正负

	ow_reset();
	write_byte(0xcc);  //跳过ROM
  	write_byte(0xbe);
  	temp_lsb=read_byte();
  	temp_msb=read_byte();
  	//TH[nums]=read_byte(); //读温度上限值
  	ow_reset();

  	if(temp_msb > 0x7f) //0x7f
  	{
   	temp_lsb =~ temp_lsb+1; 
   	temp_msb =~ temp_msb;       
   	Temp_Flag= 1;            
  	}
//温度小数部分
  	Temp_Decimal = (temp_lsb&0x0f)*6;

//温度整数部分
  	temp_lsb = temp_lsb >> 4;
  	temp_msb = temp_msb << 4;
  	Temp_Integer = temp_msb|temp_lsb; 
}


