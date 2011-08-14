//sbit SCL = P1^0;
//sbit SDA = P1^1;



extern unsigned char at24c02byteread(unsigned char address);
extern void at24c02bytewrite(unsigned char address,unsigned char writedata);