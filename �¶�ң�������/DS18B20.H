#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

//引脚定义
sbit DQ=P3^4;

//函数声明
void delayus(unsigned int t);
bit OW_Init(void);
void Write_Bit(bit wbit);
bit Read_Bit(void);

void Write_Byte(unsigned char wbyte);
unsigned char Read_Byte(void);

