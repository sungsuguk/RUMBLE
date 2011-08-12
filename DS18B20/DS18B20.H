//file name:CH452.H
//date:2010-07-25
//By:bovey

void delay(unsigned char i);
void longDelay(unsigned char n);
unsigned char ow_reset(void);
unsigned char read_bit(void);
unsigned char read_byte(void);
void write_bit(char bitval);
void write_byte(char val);
void write_th(unsigned char TH);
void GetTemp ();