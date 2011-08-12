sbit SCL = P1^0;
sbit SDA = P1^1;
#define SCL_0 SCL=0
#define SCL_1 SCL=1
#define SDA_0 SDA=0
#define SDA_1 SDA=1


extern unsigned char at24c02byteread(unsigned char address);
extern void at24c02bytewrite(unsigned char address,unsigned char writedata);