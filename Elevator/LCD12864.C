//file name:LCD.H
//date:2010-07-08

#include "LCD12864.H"
#include "Ch452.h"

#define WRITE_CMD_ADDR	0x1000
#define WRITE_DATA_ADDR 0x1100
#define READ_DATA_ADDR	0x1300
#define READ_BUSY_ADDR	0x1200


#define uchar unsigned char //  8bit, 0 ~           255
#define uint  unsigned int  // 16bit, 0 ~        65,535
#define ulong unsigned long // 32bit, 0 ~ 4,294,967,295

/*-----------建立自定义演示用字符库-----------*/
uchar code CCTAB[]={
/*--  文字:  拓  --*/
/*--  仿宋_GB231212;  此字体下对应的点阵为：宽x高=16x16   --*/
0x08,0x00,0x08,0x00,0x08,0x3C,0x0B,0xC0,0x0C,0x40,0x78,0x80,0x08,0x80,0x0D,0x38,
0x39,0xC8,0xCB,0x08,0x0D,0x08,0x09,0x08,0x29,0x38,0x19,0xC0,0x09,0x00,0x00,0x00,

/*--  文字:  普  --*/
/*--  仿宋_GB231212;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x20,0x04,0x40,0x02,0x70,0x1F,0x80,0x02,0x90,0x12,0x90,0x0A,0xA0,0x03,0xFE,
0x7C,0x00,0x01,0xE0,0x0E,0x20,0x09,0xA0,0x0E,0x20,0x08,0xE0,0x0F,0x20,0x00,0x00,

/*--  文字:  微  --*/
/*--  仿宋_GB231212;  此字体下对应的点阵为：宽x高=16x16   --*/
0x01,0x10,0x11,0x50,0x15,0x50,0x25,0x50,0x55,0xE6,0x16,0x38,0x21,0xC8,0x66,0x48,
0xA0,0x28,0x23,0x28,0x25,0x50,0x25,0x90,0x29,0x28,0x30,0x48,0x21,0x86,0x00,0x00
};

//-----------------------------------
// 延迟函数
//-----------------------------------
void delay100us(void)		// 延迟100us,基于MPU CLK=12MHz
{
	uchar data i;
	for(i=0;i<=10;i++)
	{
		_nop_();
		_nop_();
	}
}

void delayms( uint m )        // 延迟ms,基于MPU CLK=12MHz
{
    uint j;
    uint i;
    
    for(i=0; i<m; i++)
        for(j=0; j<109; j++)
            _nop_();
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
		rcom_addr=READ_BUSY_ADDR;      // 读忙地址0x0200
		busy = *rcom_addr&0x80;
	}while (busy==0x80);     // 判忙标志

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
	}while (busy==0x80);     // 判忙标志

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
	}while (busy==0x80);			// 判忙标志
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
//	delayms(5);
//   RST=1;
	delayms(800);				// 延迟时间
//	
	SdCmd(0x30);// 设置工作方式(8位总线)
	
	SdCmd(0x06);					// 设置输入方式  
	SdCmd(0x0c);					// 设置显示方式
	SdCmd(0x01);					// 清屏、光标高归位  
	delayms(2);					// 为清屏指令延迟
}
/*
// 2、清屏函数
void ClearRAM()
{
	uchar i,j;
	// SdCmd(0x01);            // 清DDRAM
	// delayms(2);
	SdCmd(0x34);              // 进入扩展指令集
	for(i=0;i<32;i++)         // 32行
	{
		SdCmd(i|0x80);           // 设置垂直地址
		SdCmd(0x80);             // 设置水平地址
		for(j=0;j<16;j++)        // 写16字（32字节）地址
		{
 			SdData(0x00);        // 写0x00数据
 			SdData(0x00);        // 写0x00数据
 		}
	}
	SdCmd(0x30);              // 退出扩展指令集
}

// 3、自建字库函数
void Set_CGRAM(uint num,uchar *pstr)
// num 汉字个数；pstr：汉字数组名
{
  uchar i;
  SdCmd(0x40);  
  for(i=0;i<num*32;i++)// 计算数据量
     {
      SdData(*pstr++);
     }
}

// 4、西文写入函数
void PrintASCII(uint x,y,uchar *pstr) //通用函数
// (x,y) 字符显示位置坐标
//x以字为单位（2个字节），y以字符行为单位
// pstr：字符数组
{
	uint addr;
	y=y<<4;
	addr=y|0x80+x;            // 计算写入地址
	SdCmd(addr);              // 设置写入地址
	while(*pstr>0)
	{
		SdData(*pstr++);          // 写入字符代码
	}     
}

// 5、中文写入函数
void PrintGB(uchar x,y,uchar *pstr)
// (x,y) 字符显示位置坐标，
//x以字为单位（2个字节），y以字符行为单位 
//pstr：字符数组
{
	uint addr;
	y=y<<4;
	addr=y|0x80+x;            // 计算写入地址
	SdCmd(addr);              // 设置写入地址
	while(*pstr>0)
	{
		SdData(*pstr++);          // 写入字符代码
	}
}


// 6、 画图函数
void ShowBmp(uchar x,y, width,high,uchar *bmp)
// (x,y) 图画其始位置坐标，x以字为单位（2个字节）；y以像素行为单位
// width 为图画宽度，以字为单位（2个字节）；high 为图画高度，以像素行为单位
// bmp 图画数组
{
 uchar i,j;

 SdCmd(0x34);               // 进入扩展指令集
 SdCmd(0x36);               // 开图形显示

 for(i=y;i<(y+high);i++)    // 循环写入行数
  {
  if (i<32)                 // 修改行地址
    {
    SdCmd(0x80|i);          // 小于32行，垂直地址为80H~9FH
    SdCmd(0x80|x);          // 水平地址0~7H
	}
    else
    {
	SdCmd(0x80|(i-32));     // 大等于32行（<63行），垂直地址为80H~9FH
    SdCmd(0x80|(x+8));      // 水平地址地址8~FH
	}
  for(j=0;j<width;j++)      // 循环写入图画字数据（2个字节）
    {
     SdData(*bmp++);        // 写入数据
     SdData(*bmp++);	    // 写入数据
    }
  }
 SdCmd(0x30);               // 退出扩展指令集
}

// 7、画点函数
void Draw_Dot(uint x,y)
//(x,y) 绘点位置坐标，x,y均以像素点为单位
{
	uchar m,n;
	uint k;

	SdCmd(0x34);
	SdCmd(0x36);
	
	if(y>=32)                  // 如果y>32，则修改地址，注意x范围0~255,y范围0~31
	{
		y=y-32;
		x=x+128;
	}
	
	SdCmd(y|0x80);            // 设置垂直地址，|0x80表示，设置GDRAM地址(扩展指令集)
	k=x/16;                   // 计算x的地址（字单位）//范围0~15，用四位表示
	SdCmd(k|0x80);            // 设置水平地址

	k=0x8000;
	k=k>>x%16;		//%(4)优先级高于>>(6),计算出来K的值表示在2字节的数据中，哪一位要点亮，其他位不变
	
	m=RdData();               // 空读
	m=RdData()|k>>8;          // 读第一字节，D15~D8
	n=RdData()|k;             // 读第二字节，D7~D0
	k=x/16;
	
	SdCmd(y|0x80);            // 设置垂直地址
	SdCmd(k|0x80);            // 设置水平地址
	SdData(m);                // 写入第一字节
	SdData(n);                // 写入第二字节
}

// 8、画线函数
void Draw_Line(uint x1,y1,x2,y2)
//(x1,y1) 线段起始坐标，(x2,y2) 线段终止坐标，x,y均以像素点为单位
{
	uint temp;
	int dalt_x,dalt_y,err=0;
	if (y1>y2)
	{
		temp=x1;
		x1=x2;
		x2=temp;
		temp=y1;
		y1=y2;
		y2=temp;
	}
	Draw_Dot(x1,y1);
	dalt_x=x2-x1;
	dalt_y=y2-y1;
	if(dalt_x>=0)
	{
		if(dalt_y>dalt_x)//k>1
		{
			while(y1<y2)
			{
				if(err<0)
				{
					x1=x1+1;
					y1=y1+1;
					err=err+dalt_y-dalt_x;
				}
				else
				{
					y1=y1+1;
					err=err-dalt_x;
				}
				Draw_Dot(x1,y1);
			}
		}
		else  // 0<=k=<1
		{
			if (dalt_y==0)
				y1=y1-1;
			while(x1<x2)
			{
				if(err<0)
				{
					x1=x1+1;
					err=err+dalt_y;
				}
				else
				{
					y1=y1+1;
					x1=x1+1;
					err=err+dalt_y-dalt_x;
				}
				Draw_Dot(x1,y1);
			}
		}
	}
	else
	{
		dalt_x=x1-x2;
		if(dalt_y>dalt_x)//k<-1
		{
			while(y1<y2)
			{
				if(err<0)
				{
					x1=x1-1;
					y1=y1+1;
					err=err+dalt_y-dalt_x;
				}
				else
				{
					y1=y1+1;
					err=err-dalt_x;
				}
				Draw_Dot(x1,y1);
			}
		}
		else       //0>k>=-1
		{
			if (dalt_y==0)
				y1=y1-1; 
			while(x1>x2)
			{
				if(err<0)
				{
					x1=x1-1;
					err=err+dalt_y;
				}
				else
				{
					x1=x1-1;
					y1=y1+1;
					err=err+dalt_y-dalt_x;
				}
				Draw_Dot(x1,y1);
			}
		} 
	}
     
}
*/
// end of program
