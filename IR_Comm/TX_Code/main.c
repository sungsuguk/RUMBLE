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
uchar KeyPress=17;		  //�жϵõ���ֵ
uchar SendMark=0;
uchar Data_Num=0;		 //����λ��
uchar SendState=0,i=0,Data1;
uchar Data[4]={0x40,0xBD,0x00,0xff};

void IrDADecode();
uchar LeadCode=0;			  //����0 ����4.5���������� ����1�������� 
uchar DataNum=0;
uchar DataS=0x00;
uchar DataArray[8];
extern unsigned  char FirstReceive;


void main()
{  
	unsigned char i,j,k = 1;
		    
    CH452_Write(CH452_SYSON2);	//�����Ʒ�ʽ�����SDA���������ж��������ô����Ӧ��ΪCH452_SYSON2W(0x04,0x23)  
	                            //#define CH452_SYSON2W   0x0423  //������ʾ������, ����2�߽ӿ�
	CH452_Write(CH452_BCD);   	//BCD����,8�������
	Clear_Dis();				//����������ʾ����
	CH452_Write(CH452_DIG0 | 0);
	
//	Init8259a ();  
	
    EA=1;		  //���ⲿ�ж�  ���ռ�ֵ
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
	ChangeData();				//���ݰ�������ֵ�������
    SendMessage();			   	//������Ϣ
	CH452_Write(CH452_DIG0 | k++);
  }
/*  while(1){
	    //CH452_Write(CH452_DIG3 | KeyPress);  //��ӡ�����
		while(KeyPress==17);		//�ȴ�����
		EX1=0;					   	//�õ�����ֵ�ر��ⲿ�ж�
		ChangeData();				//���ݰ�������ֵ�������
	    SendMessage();			   	//������Ϣ
	//	Initialization();
	//	ReceiveData();             		//���ջ���
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
	TR1=1;		   //��ʼ��ʱ��T1��ʱ
	TR0=1;		   //������ʱ��T0��ʱ
	while(SkipWhile!=1); // ������� һ��32��BIT
}

void ReceiveData()
{
   while(FirstReceive==0);	//�ȴ���һ���������жϵĵ���
	while(DataNum<8)        //�ĳ�8��ʱ����ȷ�� �ٶ�����
	{		 
	}
	delay1(150);
	PrintfData(); 
}

void Initialization(void)
{
	IrDA=1;						 //�رշ����� �������
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



void IRDAStarTime(void) interrupt 1		//��ʱ��T0  ���Ʒ���38KHZ������ʱ��
{
   uchar DataTem;
   TR1=0;
   IrDA=1;			//Disable Ir TX;
   //TimeMark++;
   switch(TimeMark)
   {
     case 0:	  	//	4.5MS �رն�ʱ��T1		  4.52218MS	  	   
		TH0=0xee;	//(65536-4500)/256;
		TL0=0x6c;	//(65536-4500)%256;
		TR1=0;					  //ִ�йر�
		TimeMark++;
		SendMark++;
		IrDA=1;		//Disable Ir TX;
		break;
     case 1:	   //   ���ݴ���
   	    switch(SendMark)
		   {
		     case 1:	//����0.56MS�ĸߵ�ѹ
			   TH0=0xfd;//(65536-560)/256;		 546.546US
			   TL0=0xd0;//(65536-560)%256;
			   TR1=1;  		  //ִ�д�
			   SendMark++;
			   break;
			 case 2:		  //�������ݿ���ֹͣ����ʱ��
			 	
			   if(Data_Num==32)	//���ݷ������	ȫ����λ
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
				 if(Data_Num%8==0)   //��������
			      {				
					Data1=Data[i]; 
					 i++;
				  }
				DataTem=Data1&0x80;
				DataTem>>=7;
				Data1<<=1;
				switch(DataTem)		 //ֹͣ����ʱ��
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

void IRDASendTime(void) interrupt 3		 //��ʱ��T1   һֱ���� 38KHZ�ķ���	 28US
{
    TH1=0xff;
	TL1=0xf4;
	IrDA = ~IrDA;
/*    switch(SendState)
	{
	  case 0: 
	  	   IrDA=0;SendState++;break;
	  case 1:				  //��ʼ����  ������λ�����͵�ѹ
	       IrDA=1;SendState--;break;
	 } 
*/
}




void Clear_Dis() //����
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


void ReceiveMessage(void)		 //���ݻ�ú���б���
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
				else {DataArray[3]=High;DataArray[2]=Light;} 	 //ǰ��Ϊ��ַ�룡
				break;											 //����Ϊ�����룬ͬʱ���ݵķ���Ҳ��ȡ����
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
  
   TR0=0;					//���ȹر��ж� ���ڼ�ʱ
   
   switch(LeadCode)
   {		                //���û��������   ���ж�ȡ������
     case 0:
	      if(TH0==0x13)	   //4.5����� �͵�ѹ����
		  {
		    LeadCode++;
		    break;		   //��֤4.5����ͨ��
		 }
	 case 1:			   //ͨ������ִ�ж����ݽ���
       {
	   	 	
	    if(TH0>=0x03&&TH0<=0x05)       //���յ�0
	   	  {
		    DataS<<=1;DataNum++;		    
          }
		else if (TH0>=0x06&&TH0<=0x08)//���յ�1  
		   {
		    DataS<<=1;DataS|=0x01;DataNum++; 
			}

		else if(TH0>0x08){
		    DataNum=0;FirstReceive=0;IT1=0;LeadCode=0;		   //��֤4.5����ͨ��
		   }
	     ReceiveMessage();            //��������	 
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
		        CH452_Write(CH452_DIG7 | DataArray[1]);	 //ң�������̵�ַ  ��ң������40  
		        CH452_Write(CH452_DIG6 | DataArray[0]);
		        CH452_Write(CH452_DIG5 | DataArray[3]);	 //ң�������̵�ַ  ��ң������40BD  
		        CH452_Write(CH452_DIG4 | DataArray[2]);
				CH452_Write(CH452_DIG3 | DataArray[5]);	  //����λ
		        CH452_Write(CH452_DIG2 | DataArray[4]);
   		        CH452_Write(CH452_DIG1 | DataArray[7]);	  //���ݷ���
		        CH452_Write(CH452_DIG0 | DataArray[6]);  IT1=1;
}





