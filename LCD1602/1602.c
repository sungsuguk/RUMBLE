//File Name: 1602.C
//Date: 2010-08-17
//From:Internet
//Changed By: Bovey
//


#include "1602.H"

#define WRITE_CMD_ADDR	0x1000
#define WRITE_DATA_ADDR	0x1100
#define READ_BUSY_ADDR	0x1200
#define READ_DATA_ADDR	0x1300


#define uchar unsigned char
#define uint unsigned int

void delayms (unsigned int times)
{
	unsigned char onems =5;
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
//    ��������
//-----------------------------------
//    ����Ѱַ��ʽ��������
//-----------------------------------

// 1��ָ��д�뺯��
void SdCmd(uchar Command)   //send command
{
  uchar xdata *wcom_addr;   // дָ��ָ��
  uchar xdata *rcom_addr;   // ��æָ��
  uchar busy;
     do
     {
     rcom_addr=READ_BUSY_ADDR;      // ��æ��ַ 0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);     // ��æ��־

    wcom_addr = WRITE_CMD_ADDR;       // дָ���ַ 0x0000
    *wcom_addr = Command;   // дָ��
//    delay100us();         // ������æ����
}
// 2������д����
void SdData(uchar DData)    //send display data
{
  uchar xdata *wdata_addr;  // д����ָ��
  uchar xdata *rcom_addr;   // ��æָ��
  uchar busy;
     do
     {
     rcom_addr=READ_BUSY_ADDR;      // ��æ��ַ 0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);     // ��æ��־

    wdata_addr =WRITE_DATA_ADDR;     // д���ݵ�ַ 0x0100
    *wdata_addr = DData;    // д����
//    delay100us();         // ������æ����
}
// 3�����ݶ�����
uchar RdData()
{
  uchar xdata *rcom_addr;		// ������ָ��
  uchar xdata *rdata_addr;		// ��æָ��
  uchar busy,DData;
     do
     {
	 rcom_addr = READ_BUSY_ADDR;		// ��æ��ַ0x0200
     busy = *rcom_addr&0x80;
     }
    while (busy==0x80);			// ��æ��־
	rdata_addr = READ_DATA_ADDR;		// �����ݵ�ַ 0x0300
    DData =*rdata_addr;			// ������
    return(DData);
}

//---------------------------------------
//  ���ܺ���
//---------------------------------------
// 1����ʼ������
void initLCM()
{
//   RST=0;						// Ӳ����λ
   delayms(10);
//   RST=1;
 // delayms(800);				// �ӳ�ʱ��

   SdCmd(0x38);	//delayms(1);				// ���ù�����ʽ(8λ����)  
   SdCmd(0x0e);	//delayms(1);			// ������ʾ��ʽ
   SdCmd(0x06);	//delayms(1);				// �������뷽ʽ 
   SdCmd(0x01);	//delayms(1);			// ���������߹�λ 
   delayms(2);					// Ϊ����ָ���ӳ�

}

// 2��д������
void WriteRAMTest()
{
 uchar j;
 SdCmd(0x01);            // ��DDRAM
 delayms(2);

  for(j=0;j<8;j++)        // д8�֣�16�ֽڣ���ַ
     {
      SdData(0xff);        // д0x00����
      SdData(0xff);        // д0x00����
     }
  SdCmd(0xc0);             // ����	
  for(j=0;j<8;j++)        // д8�֣�16�ֽڣ���ַ
     {
      SdData(0xff);        // д0x00����
      SdData(0xff);        // д0x00����
     }  
}


