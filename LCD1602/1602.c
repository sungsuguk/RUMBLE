//File Name: 1602.C
//Date: 2010-08-17
//From:Internet
//Changed By: Bovey
//


#include "1602.H"

#define WRITE_CMD_ADDR	0x1000
#define WRITE_DATA_ADDR	0x1100
#define READ_BUSY_ADDR	0x1200
#define READ_DATA_ADDR	0x1300


#define uchar unsigned char
#define uint unsigned int

void delayms (unsigned int times)
{
	unsigned char onems =5;
	unsigned char ms = 200;
	for (; times > 0; times --)					//Delay nms, n = times.
	{
		for (onems = 5; onems > 0; onems --)	//Delay 1ms
		{
			ms = 200;
			while (ms --);
		}
	}
}

//-----------------------------------
//    驱动函数
//-----------------------------------
//    总线寻址方式驱动程序
//-----------------------------------

// 1、指令写入函数
void SdCmd(uchar Command)   //send command
{
  uchar xdata *wcom_addr;   // 写指令指针
  uchar xdata *rcom_addr;   // 读忙指针
  uchar busy;
     do
     {
     rcom_addr=READ_BUSY_ADDR;      // 读忙地址 0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);     // 判忙标志

    wcom_addr = WRITE_CMD_ADDR;       // 写指令地址 0x0000
    *wcom_addr = Command;   // 写指令
//    delay100us();         // 不做读忙是用
}
// 2、数据写函数
void SdData(uchar DData)    //send display data
{
  uchar xdata *wdata_addr;  // 写数据指针
  uchar xdata *rcom_addr;   // 读忙指针
  uchar busy;
     do
     {
     rcom_addr=READ_BUSY_ADDR;      // 读忙地址 0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);     // 判忙标志

    wdata_addr =WRITE_DATA_ADDR;     // 写数据地址 0x0100
    *wdata_addr = DData;    // 写数据
//    delay100us();         // 不做读忙是用
}
// 3、数据读函数
uchar RdData()
{
  uchar xdata *rcom_addr;		// 读数据指针
  uchar xdata *rdata_addr;		// 读忙指针
  uchar busy,DData;
     do
     {
	 rcom_addr = READ_BUSY_ADDR;		// 读忙地址0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);			// 判忙标志
	rdata_addr = READ_DATA_ADDR;		// 读数据地址 0x0300
    DData =*rdata_addr;			// 读数据
    return(DData);
}

//---------------------------------------
//  功能函数
//---------------------------------------
// 1、初始化函数
void initLCM()
{
//   RST=0;						// 硬件复位
   delayms(10);
//   RST=1;
 // delayms(800);				// 延迟时间

   SdCmd(0x38);	//delayms(1);				// 设置工作方式(8位总线)  
   SdCmd(0x0e);	//delayms(1);			// 设置显示方式
   SdCmd(0x06);	//delayms(1);				// 设置输入方式 
   SdCmd(0x01);	//delayms(1);			// 清屏、光标高归位 
   delayms(2);					// 为清屏指令延迟

}

// 2、写屏函数
void WriteRAMTest()
{
 uchar j;
 SdCmd(0x01);            // 清DDRAM
 delayms(2);

  for(j=0;j<8;j++)        // 写8字（16字节）地址
     {
      SdData(0xff);        // 写0x00数据
      SdData(0xff);        // 写0x00数据
     }
  SdCmd(0xc0);             // 换行	
  for(j=0;j<8;j++)        // 写8字（16字节）地址
     {
      SdData(0xff);        // 写0x00数据
      SdData(0xff);        // 写0x00数据
     }  
}


