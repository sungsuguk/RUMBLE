//file name:CH452.H
//date:2010-06-28
//By:bovey

#include <intrins.h>
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif


//command define
/* 常量定义 */

/* CH451和CH452的常用命令码 */
#define CH452_NOP		0x0000					// 空操作
#define CH452_RESET     0x0201					// 复位
#define CH452_LEVEL		0x0100					// 加载光柱值,需另加7位数据
#define CH452_CLR_BIT	0x0180					// 段位清0,需另加6位数据
#define CH452_SET_BIT	0x01C0					// 段位置1,需另加6位数据
#define CH452_SLEEP		0x0202					// 进入睡眠状态
#define CH452_LEFTMOV   0x0300		            // 设置移动方式-左移
#define CH452_LEFTCYC   0x0301		            // 设置移动方式-左循环
#define CH452_RIGHTMOV  0x0302		            // 设置移动方式-右移
#define CH452_RIGHTCYC  0x0303		            // 设置移动方式-右循环	
#define CH452_SELF_BCD	0x0380					// 自定义BCD码,需另加7位数据
#define CH452_SYSOFF    0x0400					// 关闭显示、关闭键盘
#define CH452_SYSON1    0x0401					// 开启显示
#define CH452_SYSON2    0x0403					// 开启显示、键盘
#define CH452_SYSON2W   0x0423					// 开启显示、键盘, 真正2线接口
#define CH452_NO_BCD    0x0500					// 设置默认显示方式,可另加3位扫描极限
#define CH452_BCD       0x0580					// 设置BCD译码方式,可另加3位扫描极限
#define CH452_TWINKLE   0x0600		            // 设置闪烁控制,需另加8位数据
#define CH452_GET_KEY	0x0700					// 获取按键,返回按键代码
#define CH452_DIG0      0x0800					// 数码管位0显示,需另加8位数据
#define CH452_DIG1      0x0900		            // 数码管位1显示,需另加8位数据
#define CH452_DIG2      0x0a00		            // 数码管位2显示,需另加8位数据
#define CH452_DIG3      0x0b00		            // 数码管位3显示,需另加8位数据
#define CH452_DIG4      0x0c00		            // 数码管位4显示,需另加8位数据
#define CH452_DIG5      0x0d00					// 数码管位5显示,需另加8位数据
#define CH452_DIG6      0x0e00					// 数码管位6显示,需另加8位数据
#define CH452_DIG7      0x0f00		            // 数码管位7显示,需另加8位数据

// BCD译码方式下的特殊字符
#define		CH452_BCD_SPACE		0x10
#define		CH452_BCD_PLUS		0x11
#define		CH452_BCD_MINUS		0x12
#define		CH452_BCD_EQU		0x13
#define		CH452_BCD_LEFT		0x14
#define		CH452_BCD_RIGHT		0x15
#define		CH452_BCD_UNDER		0x16
#define		CH452_BCD_CH_H		0x17
#define		CH452_BCD_CH_L		0x18
#define		CH452_BCD_CH_P		0x19
#define		CH452_BCD_DOT		0x1A
#define		CH452_BCD_SELF		0x1E
#define		CH452_BCD_TEST		0x88
#define		CH452_BCD_DOT_X		0x80

// 有效按键代码
#define		CH452_KEY_MIN		0x40
#define		CH452_KEY_MAX		0x7F

// 2线接口的CH452定义
#define		CH452_I2C_ADDR0		0x40			// CH452的ADDR=0时的地址
#define		CH452_I2C_ADDR1		0x60			// CH452的ADDR=1时的地址,默认值
#define		CH452_I2C_MASK		0x3E			// CH452的2线接口高字节命令掩码





//hardware define
/* 2线接口的连接,与实际电路有关 */
sbit	CH452_SCL = P1^0;
sbit	CH452_SDA = P1^1;   // 如果使用真正的2线接口,那么SDA接中断引脚P3^3/INT1,用SDA直接做中断输出
// 		CH452_ADDR = 1;  // 实际电路应该硬件接GND或者VCC
//sbit	CH452_INT = P3^3;   // 标准的2线接口使用该引脚输出中断

/* 延时1uS子程序,主要用于2线接口,与单片机速度有关 */
//#define		DELAY_1US		{}				// MCS51<=10MHz
//#define		DELAY_1US		{_nop_();}		// MCS51<=20MHz
//STC 11F60XE 使用该条语句能够正常显示，使用晶振12MHz
#define		DELAY_1US		{_nop_();_nop_();_nop_();_nop_();}	// MCS51<=30MHz


/* 2线接口的位操作,与单片机有关 */
#define		CH452_SCL_SET		{CH452_SCL = 1;}
#define		CH452_SCL_CLR		{CH452_SCL = 0;}
#define		CH452_SCL_D_OUT		{}				// 设置SCL为输出方向,对于双向I/O需切换为输出
#define		CH452_SDA_SET		{CH452_SDA = 1;}
#define		CH452_SDA_CLR		{CH452_SDA = 0;}
#define		CH452_SDA_IN		(CH452_SDA)
#define		CH452_SDA_D_OUT		{}				// 设置SDA为输出方向,对于双向I/O需切换为输出
#define		CH452_SDA_D_IN		{CH452_SDA = 1;}	// 设置SDA为输入方向,对于双向I/O需切换为输入

//key interupt
/* 与单片机有关,与中断连接方式有关 */
//#define		USE_CH452_KEY		1			// 使用了CH452的按键中断
#ifdef USE_CH452_KEY
#define		DISABLE_KEY_INTERRUPT	{EX1 = 0;}
#define		ENABLE_KEY_INTERRUPT	{EX1 = 1;}
#define		CLEAR_KEY_INTER_FLAG	{IE1 = 0;}
#else
#define		DISABLE_KEY_INTERRUPT	{}
#define		ENABLE_KEY_INTERRUPT	{}
#define		CLEAR_KEY_INTER_FLAG	{}
#endif




//functions 
// 对外子程序
extern	unsigned char CH452_Read(void);				// 从CH452读取按键代码
extern  void CH452_Write(unsigned short cmd);	// 向CH452发出操作命令

unsigned char  CH452_I2c_RdByte(void);
void CH452_I2c_WrByte(unsigned char dat);
void CH452_I2c_Stop(void);
void CH452_I2c_Start(void);
