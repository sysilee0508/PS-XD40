#include "common.h"

#define I2C_DELAY 3

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void I2C_Delay(unsigned int num)
{
    unsigned int i;
	for(i=0;i<num;i++);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void I2C_Start(void)
{
	I2C_SDA_HIGH;
	I2C_SCL_HIGH;       // Serial Clock & Serial Data is High
    I2C_Delay(I2C_DELAY);
    I2C_SDA_LOW;
    I2C_Delay(I2C_DELAY);
    I2C_SCL_LOW;
	I2C_Delay(I2C_DELAY);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void I2C_Stop(void)
{
	I2C_SDA_LOW; 
    I2C_Delay(I2C_DELAY);
    I2C_SCL_HIGH;
    I2C_Delay(I2C_DELAY);
    I2C_SDA_HIGH;
}    


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void I2C_P2S(BYTE Data)
{
   	BYTE i;

	for(i=0;i<8;i++)
	{
   		I2C_SCL_LOW;			//SCLK is Low

   		I2C_Delay(I2C_DELAY);

   		if((Data & 0x80) != 0)	//MSB First
   		{
        	I2C_SDA_HIGH;
   		}
       	else
       	{
            I2C_SDA_LOW;
       	}

       	I2C_Delay(I2C_DELAY); 
       	I2C_SCL_HIGH;			//SCLK is High

       	I2C_Delay(I2C_DELAY); 
       	I2C_SCL_LOW;			//SCLK is Low
		Data <<= 1;
	}

	I2C_Delay(I2C_DELAY);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void AckDetect(void)
{
	unsigned int i = 0;

    I2C_SDA_HIGH;

	I2C_SDA_INPUT_MODE;
	
    I2C_Delay(I2C_DELAY+2);

	while((I2C_SDA_INPUT) && (i<200))i++;

	I2C_SDA_OUTPUT_MODE;

    I2C_SDA_LOW;

    I2C_SCL_HIGH;
    I2C_Delay(I2C_DELAY);
    I2C_SCL_LOW;
    I2C_Delay(I2C_DELAY/2);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void AckSend(void)
{
    I2C_SDA_LOW;
    I2C_Delay(I2C_DELAY);

    I2C_SCL_HIGH;
    I2C_Delay(I2C_DELAY);
    I2C_SCL_LOW;
    I2C_Delay(I2C_DELAY);

    I2C_SDA_HIGH;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void NotAck(void)          
{
	I2C_SDA_HIGH;
    I2C_Delay(I2C_DELAY);
    I2C_SCL_HIGH;
    I2C_Delay(I2C_DELAY);
    I2C_SCL_LOW;
    I2C_Delay(I2C_DELAY);    
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
unsigned char I2C_S2P(void)
{
	BYTE i, data_d, data;
	data_d = 0;
	data = 0;

	//I2C_SDA_HIGH;
	I2C_SDA_LOW;

	I2C_SDA_INPUT_MODE;
	I2C_Delay(I2C_DELAY+2);

	for(i=0; i<8; i++)
	{
		I2C_SCL_HIGH;
		I2C_Delay(I2C_DELAY);
		if(I2C_SDA_INPUT)
		{
			//data_d = 0x01 << (7-i);
			data_d = 0x80 >> i;
			data |= data_d;
		}
		I2C_Delay(I2C_DELAY);
		I2C_SCL_LOW;
		I2C_Delay(I2C_DELAY);
	}

	I2C_SDA_OUTPUT_MODE;
	I2C_Delay(I2C_DELAY);

	return data;
}

unsigned char I2C_READ(unsigned char slaveaddr,unsigned char regaddr)
{
	unsigned char receive_data;

	I2C_Start();

	I2C_P2S(slaveaddr & 0xFE);
	AckDetect();

	I2C_P2S(regaddr);
	AckDetect();

	I2C_Stop();

	I2C_Start(); 
	I2C_P2S(slaveaddr | 0x01);
	AckDetect();

	receive_data = I2C_S2P();
	NotAck();

	I2C_Stop();

	return receive_data;

} 

void I2C_WRITE(unsigned char slaveaddr, unsigned char regaddr, unsigned char write_data)
{
	I2C_Start();

	I2C_P2S(slaveaddr & 0xFE);
	AckDetect();

	I2C_P2S(regaddr);
	AckDetect();

	I2C_P2S(write_data);
	AckDetect();
	
	I2C_Stop();	
}

