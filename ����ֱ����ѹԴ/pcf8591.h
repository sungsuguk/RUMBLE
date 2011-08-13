#define PCF851_WIRTE 0x90
#define PCF851_READ 0x91
void DAC_PCF8591(unsigned char controlbyte,unsigned char wdata);
unsigned char ADC_PCF8591(unsigned char controlbyte);
