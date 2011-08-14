//头文件包含
#include "DS18B20.H"
#include "CH452.H"
#include "LCD12864.H"
#include "AT24C02Driver.H"
#include "8259a.h"
#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

//硬件相关
sbit BUZZER=P3^5; //蜂鸣器的控制引脚

#define LED_ADDR 0x4000 //LED灯的地址
unsigned char xdata *led=LED_ADDR;

#define LS377_ADDR 0x2000 //LS377的地址
unsigned char xdata *ls377=LS377_ADDR;

//函数声明
void Sys_Init(); //系统初始化
void Temp_Read(); //读取温度并处理
void Disp_Char(); //字符显示
void Disp_Curve(); //曲线显示
void Draw_Axis(); //绘制数轴
void Buzz_and_Heat(); //蜂鸣和加热

//变量定义
uchar TimeCount_1s; //中断次数
uchar flag_1s; //已经定时1s的标志
uchar TimeCount_10s; //已经定时1s 的个数
uchar flag_10s; //已经定时10s的标志

uchar Temp_Int,Temp_Dec; //分别存放温度的整数部分和小数部分，其中Temp_Int用于温度的判断和存储
uchar Disp[6]={0x00}; //存放显示的温度值，4位数字+1位小数点+1个结束标志0x00

uchar flag_disp=0; //字符显示和曲线显示的区分标志，由按键中断服务程序修改

#define E2PROM_BEGIN 10 //存储区起始地址
#define E2PROM_END 119 //存储区结束地址，能够存储110次最新的温度值
uchar e2prom_add=E2PROM_BEGIN; //存储温度的存储单元的地址

//坐标原点
#define X0 16
#define Y0 50
//20℃对应的纵坐标
#define Y20 40

//主函数
void main()
{
	//系统初始化
	Sys_Init();
	while(1)
	{
		if(flag_1s==1) //已经定时 1s 了?
		{
			Temp_Read(); //读取温度
			if(flag_disp==0) //字符显示?
				Disp_Char(); //字符显示
			flag_1s=0; //等待下一个1s
			Buzz_and_Heat(); //蜂鸣和加热
		}
		if(flag_10s==1) //已经定时 10s 了?
		{
			e2prom_add++; //将要存储温度的存储单元的地址，从存储区的第二个存储单元开始存储
			if(e2prom_add==E2PROM_END+1) //如果存储区已经存储了一遍
				e2prom_add=E2PROM_BEGIN; //则从起始处覆盖存储
			at24c02bytewrite(e2prom_add,Temp_Int); //存储温度
			if(flag_disp==1) //曲线显示?
				Disp_Curve(); //曲线显示
			flag_10s=0; //等待下一个10s
		}
	}
}

//系统初始化
void Sys_Init()
{
	//DS18B20初始化
	while(OW_Init()); //查看DS18B20是否存在
	//LCD12864初始化
	initLCM();
	ClearRAM();
	//24C02初始化
	at24c02bytewrite(E2PROM_BEGIN,0x00); //第一个存储单元存储0x00，用于标识存储区没有存满
	//CH452初始化
	CH452_Write(CH452_SYSON2);
	CH452_Write(CH452_BCD);
	//8259A初始化
	Init8259a();
	//定时器1初始化
	TMOD=0x10; //定时器1，工作模式1
	TH1=(65536-50000)/256;
	TL1=(65536-50000)%256; //设置定时初值，每50ms中断一次
	//Buzzer初始化
	BUZZER=1; //关闭蜂鸣器
	//Relay初始化
	*ls377=0x80; //断开继电器
	//LED灯初始化
	*led=0xff; //熄灭LED

	EX1=1; //外部中断1使能
	ET1=1; //定时器中断1使能
	EA=1; //总中断使能
	TR1=1; //启动定时器1
}

//定时器1中断服务程序
void Timer1_ISR(void) interrupt 3
{
	TR1=0;
	TimeCount_1s=(TimeCount_1s+1)%20; //1s需要中断20次
	if(!TimeCount_1s) //定时1s了?
	{
		flag_1s=1; //定时1s标志置1
		TimeCount_10s=(TimeCount_10s+1)%10; //10=10*1s
		if(!TimeCount_10s) //定时10s了?
			flag_10s=1; //定时10s标志置1
   	}
	//重置定时初值
	TH1=(65536-50000)/256;
	TL1=(65536-50000)%256;
	TR1=1;
}

//读取温度
void Temp_Read()
{
	unsigned char lsb,msb; //直接接收DS18B20输出的温度的低字节、高字节
	//温度采集
	if(~OW_Init()) //初始化
	{
		Write_Byte(0xCC); //ROM命令，跳过ROM
		Write_Byte(0x44); //DS18B20功能命令，启动温度转换
		while(~Read_Bit()); //温度转换完毕后，DS18B20会发送应答位1
		if(~OW_Init()) //初始化
		{
			Write_Byte(0xCC); //ROM命令，跳过ROM
			Write_Byte(0xBE); //DS18B20功能命令，读RAM
			lsb=Read_Byte(); //读取温度值的低字节
			msb=Read_Byte(); //读取温度值的高字节
			while(OW_Init()); //初始化
		}
   	}
	//数据转换
	Temp_Int=(msb<<4|lsb>>4); //温度整数部分
	Temp_Dec=(lsb&0x0f)*0.0625*100; //温度小数部分，保留小数点后两位有效数字
}

//字符显示
void Disp_Char()
{
	//数据处理
	Disp[0]=Temp_Int/10+'0'; //十位
	Disp[1]=Temp_Int%10+'0'; //个位
	Disp[2]='.'; //小数点
	Disp[3]=Temp_Dec/10+'0'; //小数点后第一位
	Disp[4]=Temp_Dec%10+'0'; //小数点后第二位

	SdCmd(0x30);
	SdCmd(0x01);
	delayms(2);
	PrintGB(0,0,"当前温度:");
	PrintASCII(10,0,Disp);
	PrintGB(13,0,"℃");
}

//曲线显示
void Disp_Curve()
{
	uchar tp; //要读取的存储单元
	uchar x_add; //温度点的横坐标
	uchar temp_data,temp_data_pre; //本次读取的温度和上次读取的温度

	ClearRAM(); //清屏
	Draw_Axis(); //绘制坐标轴
	tp=e2prom_add; //从e2prom_add开始读取
	x_add=X0+1;
	temp_data=at24c02byteread(tp); //读取存储的温度
	tp--; //下一个要读取的存储单元
	while(tp!=E2PROM_BEGIN) //一直读到E2PROM_BEGIN之后
	{
		temp_data_pre=temp_data; //将上次读取的温度作为起点
		temp_data=at24c02byteread(tp); //本次读取的温度作为终点
		Draw_Line(x_add+1,Y20-3*(temp_data-20),x_add,Y20-3*(temp_data_pre-20)); //画线
		tp--; //下一个要读取的存储单元
		x_add++;
	}
	if(at24c02byteread(E2PROM_BEGIN)!=0x00) //如果E2PROM_BEGIN中存储了温度，则继续读取
	{
		temp_data_pre=temp_data;
		temp_data=at24c02byteread(tp);
		Draw_Line(x_add+1,Y20-3*(temp_data-20),x_add,Y20-3*(temp_data_pre-20));
		x_add++;

		tp=E2PROM_END; //继续从E2PROM_END读取
		while(tp!=e2prom_add) //一直读到e2prom_add之前
		{
			temp_data_pre=temp_data;
			temp_data=at24c02byteread(tp);
			Draw_Line(x_add+1,Y20-3*(temp_data-20),x_add,Y20-3*(temp_data_pre-20));
			tp--;
			x_add++;
		}
	}
}

//绘制坐标轴
void Draw_Axis()
{
	Draw_Line(X0,Y0,X0+110,Y0); //时间轴，110个像素点
	Draw_Line(X0,Y0,X0,Y0-50); //温度轴，50个像素点
	//6个温度刻度，刻度间相差6个像素点，表示2℃
	Draw_Dot(X0+1,Y20);
	Draw_Dot(X0+1,Y20-6);
	Draw_Dot(X0+1,Y20-12);
	Draw_Dot(X0+1,Y20-18);
	Draw_Dot(X0+1,Y20-24);
	Draw_Dot(X0+1,Y20-30);
	//5个时间刻度，刻度间相差20个像素点，每个像素点表示10s
	Draw_Dot(X0+20,Y0-1);
	Draw_Dot(X0+40,Y0-1);
	Draw_Dot(X0+60,Y0-1);
	Draw_Dot(X0+80,Y0-1);
	Draw_Dot(X0+100,Y0-1);
	//温度范围20~30
	PrintASCII(8,0,"20");
	PrintASCII(0,0,"30");
}

//蜂鸣和加热
void Buzz_and_Heat()
{
	unsigned char i;
	//当水温超过设定的范围时，使用蜂鸣器进行报警提示
	if(Temp_Int>=30||Temp_Int<20)
	{
		//蜂鸣
		i=100;
		while(i--)
		{
			BUZZER=0; //打开蜂鸣器
			delayus(100);
			BUZZER=1; //关闭蜂鸣器
			delayus(100);
		}
   	}
	//当水温超出一定的范围时，通过继电器的开关来对鱼缸的水进行加热，并用LED灯指示工作状态
	else if(Temp_Int<25)
	{
		//加热
		*ls377=0x00; //闭合继电器开关，加热
		*led=0xaa; //LED红灯点亮，表示正在加热
	}
	else
	{
		//停止加热
		*ls377=0x80; //断开继电器开关，停止加热
		*led=0x55; //LED蓝灯点亮，表示温度正常，没有加热
	}
}

