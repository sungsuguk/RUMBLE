#include<reg51.h>
#include<absacc.h>
#include<intrins.h>
#include"LCD_1602.h"
#define led XBYTE[0x4000]
#define WC XBYTE[0x0400]
#define WD XBYTE[0x0500]
#define RD XBYTE[0x0700]
#define RB XBYTE[0x0600]

void delay_1ms(void)                 //1ms延时函数 
  { 
   unsigned int i; 
   for (i=0;i<1356;i++); 
  } 
   
void delay_nms(unsigned int n)       //N ms延时函数 
  { 
   unsigned int i=0; 
   for (i=0;i<n;i++) 
   delay_1ms(); 
  } 
void write_CMD(char CMD,char buzy)
{char temp;
if(buzy)
 {P0=0xff;
 temp=RB;
 _nop_();
 while(temp&0x80);
 }
WC=CMD;
delay_nms(2);
}
void write_Data(char Data)
{WD=Data;
delay_nms(2);
}
void LCD1602_init(void)
{//unsigned char temp; 
delay_nms(15);
write_CMD(FUNCTION_SET,0);
write_CMD(FUNCTION_SET,0);
write_CMD(FUNCTION_SET,0);
write_CMD(FUNCTION_SET,1);
write_CMD(DISPLAY_SET,1);//显示控制
write_CMD(CLR_SCREEN,1);

}

