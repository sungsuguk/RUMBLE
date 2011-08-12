//File name:CH452.C
//Date:2010-06-28
//From: Internet
//Changed By:Bovey

#include "CH452.H"


void CH452_I2c_Start(void)  // 操作起始
{
	DISABLE_KEY_INTERRUPT;  //禁止键盘中断,防止开始时被CH452中断而进入中断服务程序中的START
	//SCL = 1, SDA = 1
	CH452_SDA_SET;   /*发送起始条件的数据信号*/
	CH452_SDA_D_OUT;   /* 设置SDA为输出方向 */	
	CH452_SCL_SET;
	CH452_SCL_D_OUT;   /* 设置SCL为输出方向 */

	DELAY_1US;
	CH452_SDA_CLR;   /*发送起始信号*/
	DELAY_1US;      
	CH452_SCL_CLR;   /*钳住I2C总线，准备发送或接收数据 */
	DELAY_1US;
}

void CH452_I2c_Stop(void)  // 操作结束
{
	CH452_SDA_CLR;
	CH452_SDA_D_OUT;   /* 设置SDA为输出方向 */
	DELAY_1US;
	CH452_SCL_SET;
	DELAY_1US;
	CH452_SDA_SET;  /*发送I2C总线结束信号*/
	DELAY_1US;
	CH452_SDA_D_IN;   /* 设置SDA为输入方向 */
	DELAY_1US;
  	ENABLE_KEY_INTERRUPT;
}

void CH452_I2c_WrByte(unsigned char dat)	//写一个字节数据
{
	unsigned char i;
	CH452_SDA_D_OUT;   /* 设置SDA为输出方向 */
	for(i=0;i!=8;i++)  // 输出8位数据
	{
		if(dat&0x80) {CH452_SDA_SET;}
		else {CH452_SDA_CLR;}
		DELAY_1US;
		CH452_SCL_SET;
		dat<<=1;
		DELAY_1US;
		DELAY_1US;
		CH452_SCL_CLR;
		DELAY_1US;
	}
	CH452_SDA_D_IN;   /* 设置SDA为输入方向 */
	CH452_SDA_SET;
	DELAY_1US;
	CH452_SCL_SET;  // 接收应答
	DELAY_1US;
	DELAY_1US;
	CH452_SCL_CLR;
	DELAY_1US;
}

unsigned char  CH452_I2c_RdByte(void)		//读一个字节数据
{
	unsigned char dat,i;
	CH452_SDA_SET;
	CH452_SDA_D_IN;   /* 设置SDA为输入方向 */
	dat=0;
	for(i=0;i!=8;i++)  // 输入8位数据
	{
		CH452_SCL_SET;
		DELAY_1US;
		DELAY_1US;
		dat<<=1;
		if(CH452_SDA_IN) dat++;  // 输入1位
		CH452_SCL_CLR;
		DELAY_1US;
		DELAY_1US;
	}
	CH452_SDA_SET;
	DELAY_1US;
	CH452_SCL_SET;  // 发出无效应答
	DELAY_1US;
	DELAY_1US;
	CH452_SCL_CLR;
	DELAY_1US;
	return(dat);
}

void CH452_Write(unsigned short cmd)	//写命令
{
	CH452_I2c_Start();               /*启动总线*/
#ifdef	ENABLE_2_CH452				// 若有两个CH452并连
   	CH452_I2c_WrByte((unsigned char)(cmd>>7)&CH452_I2C_MASK|CH452_I2C_ADDR0);  // CH452的ADDR=0时
#else 
   	CH452_I2c_WrByte((unsigned char)(cmd>>7)&CH452_I2C_MASK|CH452_I2C_ADDR1);  // CH452的ADDR=1时(默认)
#endif
   	CH452_I2c_WrByte((unsigned char)cmd);     /*发送数据*/
  	CH452_I2c_Stop();                 /*结束总线*/ 
}

unsigned char CH452_Read(void)		//读取按键
{
	unsigned char keycode;
   	CH452_I2c_Start();               /*启动总线*/
   	CH452_I2c_WrByte((unsigned char)(CH452_GET_KEY>>7)&CH452_I2C_MASK|0x01|CH452_I2C_ADDR1);    // 若有两个CH452并连,当ADDR=0时,需修改为CH452_I2C_ADDR0
	keycode=CH452_I2c_RdByte();      /*读取数据*/
	CH452_I2c_Stop();                /*结束总线*/ 
	return(keycode);
}

