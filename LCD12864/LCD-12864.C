//file name:LCD-12864.C
//date:2010-07-31

#include "LCD-12864.H"

#define WRITE_CMD_ADDR	0x1000
#define WRITE_DATA_ADDR	0x1100
#define READ_DATA_ADDR	0x1300
#define READ_BUSY_ADDR	0x1200


#define uchar unsigned char //  8bit, 0 ~           255
#define uint  unsigned int  // 16bit, 0 ~        65,535
#define ulong unsigned long // 32bit, 0 ~ 4,294,967,295


//delay ms
void delayms (unsigned int times)
{
	unsigned char onems = 5;
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
  uchar busy;               // 
    do
     {
     rcom_addr = READ_BUSY_ADDR;      // 读忙地址0x0200
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
     rcom_addr = READ_BUSY_ADDR;      // 读忙地址 0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);     // 判忙标志

    wdata_addr = WRITE_DATA_ADDR;     // 写数据地址 0x0100
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
   delayms(5);
//  RST=1;
   delayms(800);				// 延迟时间

   SdCmd(0x30);					// 设置工作方式(8位总线)
   SdCmd(0x06);					// 设置输入方式  
   SdCmd(0x0e);					// 设置显示方式,不闪烁
   SdCmd(0x01);					// 清屏、光标高归位  
   delayms(2);					// 为清屏指令延迟
}

// 2、清屏函数
void ClearRAM()
{
 uchar i,j;
// SdCmd(0x01);            // 清DDRAM
// delayms(2);
 SdCmd(0x36);              // 进入扩展指令集

 for(i=0;i<32;i++)          // 4行
  {
  SdCmd(i|0x80);           // 设置垂直地址
  SdCmd(0x80);             // 设置水平地址
  for(j=0;j<16;j++)        // 写64字（128字节）地址
     {
      SdData(0x00);        // 写0x00数据
      SdData(0x00);        // 写0x00数据
     }
  }
 SdCmd(0x30);              // 退出扩展指令集
}


// 2、写屏函数
void WriteRAMTest()
{
 uchar i,j;
// SdCmd(0x01);            // 清DDRAM
// delayms(2);
 SdCmd(0x36);              // 进入扩展指令集

 for(i=0;i<32;i++)          // 32行
  {
  SdCmd(i|0x80);           // 设置垂直地址
  SdCmd(0x80);             // 设置水平地址
  for(j=0;j<16;j++)        // 写16字（32字节）地址
     {
      SdData(0xFF);        // 写0xff数据
      SdData(0xFF);        // 写0xff数据
     }
  }

 SdCmd(0x30);              // 退出扩展指令集

}

