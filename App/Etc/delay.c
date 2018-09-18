#include "common.h"


void Delay_us(unsigned long time_us)			//time delay for us in 64MHz
{
    unsigned int i;

    for(i = 0; i < time_us; i++)
    { 
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();
    }
}

void Delay_ms(unsigned long time_ms)			//time delay for ms in 64MHz
{
    unsigned int i;

    for(i = 0; i < time_ms; i++)
        Delay_us(1000);
}
