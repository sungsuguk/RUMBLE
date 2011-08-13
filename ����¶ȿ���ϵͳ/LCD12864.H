//file name:LCD.H

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
#include <intrins.h>
#include <string.h>
#include <stdio.h>

#define uchar unsigned char
#define uint  unsigned int
//延迟函数
void delay100us(void);
void delayms(unsigned int m);

//驱动函数
unsigned char RdStatus(void); //read BF and AC from BF and AC
void SdCmd(unsigned char Cmd); //write instruction to IR
void SdData(unsigned char Data); //write data to DR
unsigned char RdData(); //read data from DR

//功能函数
void initLCM();	//初始化函数
void ClearRAM(); //清屏函数
void Set_CGRAM(unsigned int num,unsigned char *pstr); //自定义字符函数
//num:字符个数,pstr:指向字符的字模数组
void PrintASCII(uint x,y,uchar *pstr); //ASCII字符写入函数
//(x,y):字符的写入坐标，x以字为单位（2个字节），y以字符行为单位 pstr:指向字符数组
void PrintGB(uchar x,y,uchar *pstr); //中文写入函数
//(x,y):字符的写入坐标，x以字为单位（2个字节），y以字符行为单位 pstr:指向字符数组
void ShowBmp(uchar x,y, width,high,uchar *bmp); //画图函数
//(x,y):图形起始坐标，x以字为单位（2个字节），y以像素行为单位
//width:图形宽度，以字为单位（2个字节） high:图形高度，以像素行为单位
//bmp图形点阵数组
void Draw_Dot(uint x,y); //画点函数
//(x,y):画点坐标，x,y均以像素点为单位
void Draw_Line(uint x1,y1,x2,y2); //画线函数
//(x1,y1):线段起始坐标，(x2,y2):线段终止坐标，均以像素点为单位

