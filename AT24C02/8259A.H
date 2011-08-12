//file name:8259A.H
//date:2010-06-29
//

#include <ABSACC.H>


#define uchar unsigned char
#define uint unsigned int


#ifndef _ADR_8259A_
#define ADR_8259AO XBYTE[0x6001]	//odd address of TMD8259A; A0 = 1
#define ADR_8259AE XBYTE[0x6000]	//even address of TMD8259A; A0 = 0
#define _ADR_8259A_ 1

//定义工作模式为8086模式或80/85模式，前者需要2个INTA脉冲，后者需要3个INTA脉冲
#define MODE8x86 1		
//#define MODE8085 1

//8259A的命令码
/*****************************************************/
//ICW1
//ICWl主要用来设定中断请求的触发方式，是否级联，初始化过程中用不用ICW4等
//A0 = 0
//ICW1:	|A0	|D7	|D6	|D5	|D4	|D3	|D2	|D1	|D0	|
//		|0	|0	|0	|0	|1	|0	|0	|1	|1	|
//D7~D5:A7~A5
//*****
//D4 = 1 & A0 = 0 ,Interpreted as Initalization Command Word 1(ICW1)
//*****
//D3:LTIM(Level Interrupt Mode) 1, level;0,edge 
//D2:ADI,1,Interval = 4; 0,Interval = 8
//D1:bit1 = 1-->NO ICW3;bit1 = 0-->ICW3 Used
//D0:ICW4,If ICW4 is not needed,set ICW4 = 0
//
//ICW1 = 0x0001 0011 
//
/******************************************************/
//Interval = 8
//读到的中断向量号bit0~bit3 = 0, bit4~it8 = 0~7
//脉冲触发
//使用ICW4
#define ICW1 0x13		

/*****************************************************/
//ICW2
//ICW2用来设置中断向量
//A0 = 1
//ICW2被用作指定本8259A中的中断请求的起始中断向量
//bit0:3必须被设为0, 所以，其起始中断向量必须是8的倍数
//我们设计成0x00
/*****************************************************/
#define ICW2 0x08

/*****************************************************/
//ICW3
//ICW3仅在8259A级联时才需要
//A0 = 1
//bitx = 0 means UR input does not have a slave
//bitx = 1 means IR input has a slave
/*****************************************************/
#define ICW3 0x00

/*****************************************************/
//ICW4
//用来设置是否为特殊全嵌套方式，是否为缓冲器方式，
//是否为自动中断结束中断方式，是否为8086／8088系统等
//A0 = 1
//bit[7:5] = 0
//bit4 SFNM (嵌套方式)， 
//bit4 = 0 一般全嵌套方式; 
//bit4 = 1 特殊全嵌套
//bit3 BUF  bit2 M/S (缓冲方式)
//0 x	非缓冲方式
//1 0	缓冲方式/从片
//0 1	缓冲方式/主片
//bit1 AEOI (Auto EOI); 
//bit1 = 0 Normal EOI
//bit1 = 1 Auto EOI
//bit0 Mode
//bit0 = 0 MCS-80/85 MODE
//bit0 = 1 8086/8088 MODE
//不使用8259A的特殊功能，因此将bit1:4都设为0
//即默认的Full Nested Mode，不使用Buffer，以及手动EOI模式
//将bit0设为1，8086模式；将bit0设为0，80/85模式
/*****************************************************/
//使用一般全嵌套方式
/*****************************************************/
//一般全嵌套方式处理过程
//在ISR寄存器中有一个8-bit的字节，范围为bit[0,7]；每一个
//bit对应一个IRQ（IRQ0-IRQ7对应bit[0,7]）。当一个IRQ被提
//交给CPU之后（收到来自于CPU的第一个INTA信号之后），其对
//应的bit会被设置为1。比如IRQ6被提交给CPU之后，IS Register
//的bit-6会被设置为1。当此8259A收到一个EOI之后（对于手动
//模式，这意味着一个优先级别最高的中断请求被处理结束），
//会将IS Register中被设置的最高优先级IRQ的对应的bit清为0。
//比如在收到一个EOI时，发现IS Register的bit-3, bit-5, 
//bit-6被设置，那么被清除的则是bit-3（越小优先级别越高）。
//在清除优先级最高的bit之后，8259A会到IRR中察看是否有优先
//级别高于当前正在处理的IRQ中优先级别最高的IRQ，如果有，
//则将此IRQ提交给CPU处理，同时设置相应的bit。还以上面的例
//子为例，当bit-3被清除之后，如果发现在IRR中有一个IRQ4等待
//被处理，则将其提交给CPU，在收到来自于CPU的第一个INTA信号
//之后，则将IS Register的bit-4置为1.
/*****************************************************/
#ifdef MODE8x86
#define ICW4 0x01
#endif

#ifdef MODE8085
#define ICW4 0x00
#endif

/*****************************************************/
//OCW1
//A0 = 1
//OCW1是用来做中断请求屏蔽用的操作控制字
//bit0-IRQ0, bit1-IRQ1, bit2-IRQ2, ..., bit7-IRQ7
//bitx = 1, 屏蔽IRQx
/*****************************************************/
#define OCW1 0x00	//开IR2

/*****************************************************/
//OCW2
//OCW2用来设置中断结束方式和优先级循环方式
//A0 = 0
//Bit(s) 	Function
//bit7:5
//000 		Rotate in Auto EOI Mode (Clear)
//001 		Non Specific EOI
//010 		Reserved
//011 		Specific EOI
//100 		Rotate in Auto EOI Mode (Set)
//101 		Rotate on Non-Specific EOI
//110 		Set Priority Command (Use Bits 2:0)
//111 		Rotate on Specific EOI (Use Bits 2:0)
//bit4 		Must be set to 0
//bit3 		Must be set to 0
//bit2:0 
//000 		Act on IRQ 0 or 8
//001 		Act on IRQ 1 or 9
//010 		Act on IRQ 2 or 10
//011 		Act on IRQ 3 or 11
//100 		Act on IRQ 4 or 12
//101 		Act on IRQ 5 or 13
//110 		Act on IRQ 6 or 14
//111 		Act on IRQ 7 or 15
//将bit3:4设置为0，以说明这是一个OCW2
//bit-6被设为1，则bit0:2有效,其操作则是面向某个IRQ的
//否则将bit0:2设为0，其操作是面向整个8259A的所有IRQ的
//因为我们在初始化8259A时，制定的EOI(End Of Interrupt)
//Mode为手动模式，
//所以当每次对应某个8259A芯片的IRQ的中断服务程序ISR执
//行结束后，都需要向8259A发送一个EOI，
//其对应的OCW2的值为0x20
//它需要在8259A的中断的服务程序ISR末尾处被调用
/*****************************************************/ 
#define OCW2 0x20	//bit6 = 0, 操作是面向整个8259A的所有IRQ,固定优先级

/*****************************************************/
//OCW3
//A0 = 0
//Bit(s) Function
//bit7 	Must be set to 0
//bit6:5 
//00 	Reserved
//01 	Reserved
//10 	Reset Special Mask
//11 	Set Special Mask
//bit4 	Must be set to 0
//bit3 	Must be set to 1
//bit2 
//1 	Poll Command
//0 	No Poll Command
//bit1:0 
//00 	Reserved
//01 	Reserved
//10 	Next Read Returns Interrupt Request Register
//11 	Next Read Returns In-Service Register
//将Bit-3设为1，Bit-4设为0，以让8259A知道这是一个OCW3
/*****************************************************/	
//用来读IRR或ISR
#define OCW3 0x0b

#endif

void Init8259a();
void Inter8259a();
void Server_Key();
void Server_Hall();
void Server_IR();
