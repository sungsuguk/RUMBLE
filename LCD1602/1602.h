#ifndef STC
#include "STC12C5A56S2.H"
#define STC 1
#endif
#include <intrins.h>
//#include <string.h>
//#include <stdio.h>

void delay100us(void);
void delayms( unsigned int m );
void SdCmd(unsigned char Command);	//send command
void SdData(unsigned char DData);	//send display data
unsigned char RdData();				//read data
void initLCM();						//Init fuction
void WriteRAMTest();				//test
