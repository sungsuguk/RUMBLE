//file name:LCD.H
//date:2010-07-08
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
#include <intrins.h>
#include <string.h>
#include <stdio.h>

#define uchar unsigned char
#define uint  unsigned int 

void delay100us(void);
void delayms( unsigned int m );
void SdCmd(unsigned char Command);	//send command
void SdData(unsigned char DData);	//send display data
unsigned char RdData();				//read data
void initLCM();						//Init fuction
void ClearRAM();					//clear screen
void Set_CGRAM(unsigned int num,unsigned char *pstr);	//自建字库函数
void PrintASCII(uint x,y,uchar *pstr); //通用函数,西文写入函数
// (x,y) 字符显示位置坐标，x以字为单位（2个字节），y以字符行为单位 pstr：字符数组
void PrintGB(uchar x,y,uchar *pstr); //中文写入函数
// (x,y) 字符显示位置坐标，x以字为单位（2个字节），y以字符行为单位 pstr：字符数组
void ShowBmp(uchar x,y, width,high,uchar *bmp); //画图函数
// (x,y) 图画其始位置坐标，x以字为单位（2个字节）；y以像素行为单位
// width 为图画宽度，以字为单位（2个字节）；high 为图画高度，以像素行为单位
// bmp 图画数组
void Draw_Dot(uint x,y); //画点函数
//(x,y) 绘点位置坐标，x,y均以像素点为单位
void Draw_Line(uint x1,y1,x2,y2); //画线函数
//(x1,y1) 线段起始坐标，(x2,y2) 线段终止坐标，x,y均以像素点为单位