//file name:main.c
//date:2010-06-30
//function: test 8259A and CH452_key
//By:chenjian
//Last Changed: 2010-09-20
//By:Bovey
//1. changed while(1) {} 
// not use key, MCU will send number from 0 to 9 by itself
//2. void INTI(void)
//{
//  	P1M0 = P1M0 | 0x08;
//	P1M1 = P1M1 & ~0x08;
//	IrDA=1;
//3. void IRDASendTime(void) 
//  IrDA = ~IrDA; instead of switch(){}
//4. delete AT24c02Driver.h & .c
//5. delete 8259A.h & .c and disable Init8259a (); 

#include "CH452.H"
//#include "8259A.H"
#include "absacc.h"
//#include "AT24C02Driver.H"

#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif

sbit IrDA=P1^3;
sbit Key=P1^5;

#define uchar unsigned char

void SendMessage();
void delay1(uchar c);
void Clear_Dis();
void INTI(void);
void ChangeData();
void ReceiveData();
void PrintfData();
void Initialization(void);

uchar SkipWhile=0;
uchar TimeMark=0;
uchar KeyPress=17;		  //中断得到键值
uchar SendMark=0;
uchar Data_Num=0;		 //数据位数
uchar SendState=0,i=0,Data1;
uchar Data[4]={0x40,0xBD,0x00,0xff};

void IrDADecode();
uchar LeadCode=0;			  //等于0 等在4.5毫秒引导码 等于1接收数据 
uchar DataNum=0;
uchar DataS=0x00;
uchar DataArray[8];
extern unsigned  char FirstReceive;


void main()
{  
	unsigned char i,j,k = 1;
		    
    CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)  
	                            //#define CH452_SYSON2W   0x0423  //开启显示、键盘, 真正2线接口
	CH452_Write(CH452_BCD);   	//BCD译码,8个数码管
	Clear_Dis();				//清除数码管显示数字
	CH452_Write(CH452_DIG0 | 0);
	
//	Init8259a ();  
	
    EA=1;		  //打开外部中断  接收键值
	EX1=1;
	IT1=1;

  while(1)
  {
  	i = 2;
	while(i--)
	{
		for(j = 10; j > 0; j--)
			delay1(200);
	}
	k = k%10;					
	KeyPress = k;
	ChangeData();				//根据按键的数值传输编码
    SendMessage();			   	//发送信息
	CH452_Write(CH452_DIG0 | k++);
  }
/*  while(1){
	    //CH452_Write(CH452_DIG3 | KeyPress);  //打印发射键
		while(KeyPress==17);		//等待按键
		EX1=0;					   	//得到按键值关闭外部中断
		ChangeData();				//根据按键的数值传输编码
	    SendMessage();			   	//发送信息
	//	Initialization();
	//	ReceiveData();             		//接收回馈
		KeyPress = 17;
		EX1=1;
	}
*/
}



void ChangeData()
{   
   switch (KeyPress)
   {
     case 0x00:  Data[2]=0x90;Data[3]=~Data[2];break;
	 case 0x01:  Data[2]=0x00;Data[3]=~Data[2];break;
	 case 0x02:  Data[2]=0x80;Data[3]=~Data[2];break;
	 case 0x03:  Data[2]=0x40;Data[3]=~Data[2];break;
	 case 0x04:  Data[2]=0xc0;Data[3]=~Data[2];break;
	 case 0x05:  Data[2]=0x20;Data[3]=~Data[2];break;
	 case 0x06:  Data[2]=0xa0;Data[3]=~Data[2];break;
	 case 0x07:  Data[2]=0x60;Data[3]=~Data[2];break;
	 case 0x08:  Data[2]=0xe0;Data[3]=~Data[2];break;
	 case 0x09:  Data[2]=0x10;Data[3]=~Data[2];break;
	 default:    Data[2]=0xFF;Data[3]=~Data[2];break;
   }
}


void SendMessage()
{
  
    INTI();   
	delay1(200);
	delay1(200);
	TR1=1;		   //开始定时器T1计时
	TR0=1;		   //开启定时器T0计时
	while(SkipWhile!=1); // 发送完毕 一共32个BIT
}

void ReceiveData()
{
   while(FirstReceive==0);	//等待第一次上升沿中断的到来
	while(DataNum<8)        //改成8的时候正确率 速度提升
	{		 
	}
	delay1(150);
	PrintfData(); 
}

void Initialization(void)
{
	IrDA=1;						 //关闭发射器 避免过热
	TMOD=0x01;
	TH0=0x00;
	TL0=0x00;
	EA=1;
    EX1=1;
	IT1=1;    
}
void INTI(void)
{
//  	P1M0 = P1M0 | 0x08;
//	P1M1 = P1M1 & ~0x08;
	IrDA=1;
	TMOD=0X11;	

	ET0=1;
    ET1=1;
	TH1=0xff;
	TL1=0xf4;
	TH0=0xdc;//(65536-9000)/256;
	TL0=0xd8;//(65536-9000)%256;   8.97381MS	
}
void delay1(uchar c)			 //1ms
{
   uchar a,b;
   for(;c>0;c--)
     for(a=248;a>0;a--)
	   for(b=2;b>0;b--);
}



void IRDAStarTime(void) interrupt 1		//定时器T0  控制发射38KHZ方波的时间
{
   uchar DataTem;
   TR1=0;
   IrDA=1;			//Disable Ir TX;
   //TimeMark++;
   switch(TimeMark)
   {
     case 0:	  	//	4.5MS 关闭定时器T1		  4.52218MS	  	   
		TH0=0xee;	//(65536-4500)/256;
		TL0=0x6c;	//(65536-4500)%256;
		TR1=0;					  //执行关闭
		TimeMark++;
		SendMark++;
		IrDA=1;		//Disable Ir TX;
		break;
     case 1:	   //   数据传输
   	    switch(SendMark)
		   {
		     case 1:	//发射0.56MS的高电压
			   TH0=0xfd;//(65536-560)/256;		 546.546US
			   TL0=0xd0;//(65536-560)%256;
			   TR1=1;  		  //执行打开
			   SendMark++;
			   break;
			 case 2:		  //根据数据控制停止发射时间
			 	
			   if(Data_Num==32)	//数据发送完成	全部复位
			    {

				   SkipWhile=1;
				   TR0=0;
				   TR1=0;
				   Data_Num=0;
				   SendState=0;
				   i=0;
				   IrDA=1;		//Disable Ir TX;
				   TimeMark=0;
				   SendMark=0;	 
				   break;	
				}
				 if(Data_Num%8==0)   //更换数据
			      {				
					Data1=Data[i]; 
					 i++;
				  }
				DataTem=Data1&0x80;
				DataTem>>=7;
				Data1<<=1;
				switch(DataTem)		 //停止发射时间
				{
				   case 0: 	
				     TH0=0xfd;//(65536-560)/256;		  592.454
				     TL0=0xd0;//(65526-560)%256;
				     TR1=0;
					 IrDA=1;		//Disable Ir TX;
				     break;
				   case 1:	
					 TH0=(65536-1680)/256;		  //1.70955
					 TL0=(65536-1680)%256;		  //
					 TR1=0;
					 IrDA=1;		//Disable Ir TX;
					 break;			       
				}
				SendMark--;
				Data_Num++;	
				break;

		   }  
   }
}

void IRDASendTime(void) interrupt 3		 //定时器T1   一直发射 38KHZ的方波	 28US
{
    TH1=0xff;
	TL1=0xf4;
	IrDA = ~IrDA;
/*    switch(SendState)
	{
	  case 0: 
	  	   IrDA=0;SendState++;break;
	  case 1:				  //开始发送  单数据位后续低电压
	       IrDA=1;SendState--;break;
	 } 
*/
}




void Clear_Dis() //清屏
{
	CH452_Write(CH452_NO_BCD );
//	CH452_Write(CH452_SET_BIT | 0);
	CH452_Write(CH452_DIG7 | 0x00);
	CH452_Write(CH452_DIG6 | 0x00);
	CH452_Write(CH452_DIG5 | 0x00);
	CH452_Write(CH452_DIG4 | 0x00);
	CH452_Write(CH452_DIG3 | 0x00);
	CH452_Write(CH452_DIG2 | 0x00);
	CH452_Write(CH452_DIG1 | 0x00);
	CH452_Write(CH452_DIG0 | 0x00);
	CH452_Write(CH452_BCD); 
}


void ReceiveMessage(void)		 //数据获得后进行保存
{
  uchar High,Light;
  	switch (DataNum)		     
		{
		 case 8:
		   		Light=DataS&0x0F;
		        High=DataS>>4;
				if(Light!=0x00||High!=0x04)
				{DataNum=0;FirstReceive=0;IT1=0;LeadCode=0;}
				else {DataArray[1]=High;DataArray[0]=Light;}
				break;		
		 case 16:
		 
		        Light=DataS&0x0F;
		        High=DataS>>4;
				if(Light!=0x0d||High!=0x0b)
				{DataNum=0;FirstReceive=0;IT1=0;LeadCode=0;}
				else {DataArray[3]=High;DataArray[2]=Light;} 	 //前面为地址码！
				break;											 //后面为数据码，同时数据的反码也被取反了
	     case 24:
		 
		        DataArray[4]=DataS&0x0F;
		        DataArray[5]=DataS&0xF0;DataArray[5]>>=4; 
		   		break;		
		 case 32:
		       	Light=(~DataS)&0x0F;
		        High=(~DataS)>>4;
				if((DataArray[4]==Light)&&(DataArray[5]==High))
				{DataArray[7]=High;DataArray[6]=Light;DataNum=0;FirstReceive=0;IT1=0;LeadCode=0;}
				else  {DataNum=0;FirstReceive=0;IT1=0;LeadCode=0;}
		        break;  
		} 

}
 void IrDADecode() 
{
  
   TR0=0;					//首先关闭中断 利于计时
   
   switch(LeadCode)
   {		                //如果没有引导码   进行读取引导码
     case 0:
	      if(TH0==0x13)	   //4.5毫秒的 低电压过来
		  {
		    LeadCode++;
		    break;		   //验证4.5毫秒通过
		 }
	 case 1:			   //通过引导执行读数据解码
       {
	   	 	
	    if(TH0>=0x03&&TH0<=0x05)       //接收到0
	   	  {
		    DataS<<=1;DataNum++;		    
          }
		else if (TH0>=0x06&&TH0<=0x08)//接收到1  
		   {
		    DataS<<=1;DataS|=0x01;DataNum++; 
			}

		else if(TH0>0x08){
		    DataNum=0;FirstReceive=0;IT1=0;LeadCode=0;		   //验证4.5毫秒通过
		   }
	     ReceiveMessage();            //保存数据	 
	    }
		}
		if(DataNum==32)
		 {TR0=0;FirstReceive=0;IT1=0;LeadCode=0;}
	    TH0=0x00;
	    TL0=0x00;
	    TR0=1;
 }

void PrintfData()
{			
		        CH452_Write(CH452_DIG7 | DataArray[1]);	 //遥控器厂商地址  该遥控器是40  
		        CH452_Write(CH452_DIG6 | DataArray[0]);
		        CH452_Write(CH452_DIG5 | DataArray[3]);	 //遥控器厂商地址  该遥控器是40BD  
		        CH452_Write(CH452_DIG4 | DataArray[2]);
				CH452_Write(CH452_DIG3 | DataArray[5]);	  //数据位
		        CH452_Write(CH452_DIG2 | DataArray[4]);
   		        CH452_Write(CH452_DIG1 | DataArray[7]);	  //数据反码
		        CH452_Write(CH452_DIG0 | DataArray[6]);  IT1=1;
}





