#include "8259a.h"
void Init8259a ()
{
	ADR_8259AE = ICW1;	//ICW1
	ADR_8259AO = ICW2;	//ICW2
	//ADR_8259AO = ICW3;	//ICW3, No slaves
	ADR_8259AO = ICW4;	//ICW4,	80/85 mode or 8x86 mode	

	//ADR_8259AO = OCW1;	//IR0~IR2 is enable
	//ADR_8259AE = OCW2;
    //ADR_8259AE = OCW3;
}

