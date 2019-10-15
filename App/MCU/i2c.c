#include "common.h"

#define I2C_DELAY 3

eI2C_CH_t i2c_ch = I2C_MAIN;
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
	I2C_SDA_HIGH(i2c_ch);
	I2C_SCL_HIGH(i2c_ch);       // Serial Clock & Serial Data is High
	I2C_Delay(I2C_DELAY);
	I2C_SDA_LOW(i2c_ch);
	I2C_Delay(I2C_DELAY);
	I2C_SCL_LOW(i2c_ch);
	I2C_Delay(I2C_DELAY);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void I2C_Stop(void)
{
	I2C_SDA_LOW(i2c_ch); 
	I2C_Delay(I2C_DELAY);
	I2C_SCL_HIGH(i2c_ch);
	I2C_Delay(I2C_DELAY);
	I2C_SDA_HIGH(i2c_ch);
}    


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void I2C_P2S(BYTE Data)
{
	BYTE i;

	for(i=0;i<8;i++)
	{
		I2C_SCL_LOW(i2c_ch);			//SCLK is Low

		I2C_Delay(I2C_DELAY);

		if((Data & 0x80) != 0)	//MSB First
		{
			I2C_SDA_HIGH(i2c_ch);
		}
		else
		{
			I2C_SDA_LOW(i2c_ch);
		}

		I2C_Delay(I2C_DELAY); 
		I2C_SCL_HIGH(i2c_ch);			//SCLK is High

		I2C_Delay(I2C_DELAY); 
		I2C_SCL_LOW(i2c_ch);			//SCLK is Low
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

	I2C_SDA_HIGH(i2c_ch);

	I2C_SDA_INPUT_MODE;

	I2C_Delay(I2C_DELAY+2);

	while((I2C_SDA_INPUT(i2c_ch)) && (i<200))i++;

	I2C_SDA_OUTPUT_MODE;

	I2C_SDA_LOW(i2c_ch);

	I2C_SCL_HIGH(i2c_ch);
	I2C_Delay(I2C_DELAY);
	I2C_SCL_LOW(i2c_ch);
	I2C_Delay(I2C_DELAY/2);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void AckSend(void)
{
	I2C_SDA_LOW(i2c_ch);
	I2C_Delay(I2C_DELAY);

	I2C_SCL_HIGH(i2c_ch);
	I2C_Delay(I2C_DELAY);
	I2C_SCL_LOW(i2c_ch);
	I2C_Delay(I2C_DELAY);

	I2C_SDA_HIGH(i2c_ch);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void NotAck(void)          
{
	I2C_SDA_HIGH(i2c_ch);
	I2C_Delay(I2C_DELAY);
	I2C_SCL_HIGH(i2c_ch);
	I2C_Delay(I2C_DELAY);
	I2C_SCL_LOW(i2c_ch);
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
	I2C_SDA_LOW(i2c_ch);

	I2C_SDA_INPUT_MODE;
	I2C_Delay(I2C_DELAY+2);

	for(i=0; i<8; i++)
	{
		I2C_SCL_HIGH(i2c_ch);
		I2C_Delay(I2C_DELAY);
		if(I2C_SDA_INPUT(i2c_ch))
		{
			//data_d = 0x01 << (7-i);
			data_d = 0x80 >> i;
			data |= data_d;
		}
		I2C_Delay(I2C_DELAY);
		I2C_SCL_LOW(i2c_ch);
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

//--------------------------------------------------------------------------------------------------------------------------
void I2C_MultiWrite(unsigned char slaveaddr, unsigned char rAddr, unsigned char* pBuff, unsigned short bytes)		// i2c 8bit multiwrite
{
	unsigned short i;	

	//printf("[I2C_W] nID:%02X, rAddr:%04X, pBuff:%04X, bytes:%04X\n", nID, rAddr,  *((PWORD)pBuff), bytes);

	I2C_Start();
	I2C_P2S(slaveaddr&0xFE); AckDetect();

	I2C_P2S(rAddr);   AckDetect();		
	
	for (i=0; i<bytes; i++) 
	{
		I2C_P2S(pBuff[i]); AckDetect();  		
		//I2C_P2S(pBuff[i+1]);AckDetect();  		
		//I2C_P2S(pBuff[i]);	AckDetect();  	
	}
	//I2C_P2S(pBuff[i+1]); AckDetect();  		
	I2C_P2S(pBuff[i]);   NotAck();//AckDetect();  	

	I2C_Stop();		

//	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
void I2C_MultiRead(unsigned char slaveaddr, unsigned char rAddr, unsigned char* pBuff, unsigned short bytes)		// i2c 8bit multiread
{
	unsigned short i;	

	I2C_Start();
	I2C_P2S(slaveaddr&0xFE); AckDetect();

	I2C_P2S(rAddr & 0xFF); AckDetect();

	I2C_Stop();

	I2C_Start(); 
	I2C_P2S(slaveaddr|0x01); AckDetect();

	for (i=0; i<bytes; i++) 
	{
		//pBuff[i+1] = I2C_S2P(); AckSend();	// Receive a buffer data
		pBuff[i]   = I2C_S2P(); AckSend();	// Receive a buffer data
	}

	//pBuff[i+1] = I2C_S2P();	 AckSend();		// Receive a buffer data
	pBuff[i]   = I2C_S2P();	 NotAck();		// Receive a buffer data


	I2C_Stop();												

	//printf("[I2C_R] nID:%02X, rAddr:%04X, pBuff:%04X, bytes:%04X\n", nID, rAddr, *((PWORD)pBuff), bytes);

//	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
unsigned char I2C_Write16(unsigned char ID, unsigned char dAddr, unsigned char page, unsigned short rAddr, unsigned char* pBuff, unsigned short bytes)	// i2c 16bit multiwrite
{
	unsigned short i;

	i2c_ch = ID;
//	BYTE slaveAddr = I2C_MDIN3xx_ADDR(selectedMDIN);
//	printf("[I2C_W] nID:%02X, rAddr:%04X, pBuff:%04X, bytes:%04X\n", nID, rAddr,  *((PWORD)pBuff), bytes);

	I2C_Start();
	I2C_P2S(dAddr&0xFE); AckDetect();

	I2C_P2S((BYTE)(rAddr >> 8));   AckDetect();		
	I2C_P2S((BYTE)(rAddr & 0xFF)); AckDetect();		
	
	I2C_Start(); 
	I2C_P2S(dAddr&0xFE); AckDetect();
	
	for (i=0; i<bytes/2-1; i++) 
	{
		I2C_P2S((BYTE)(HIBYTE(((PWORD)pBuff)[i]))); AckDetect();  		
		I2C_P2S((BYTE)(LOBYTE(((PWORD)pBuff)[i]))); AckDetect();  	
		//I2C_P2S(pBuff[i+1]);AckDetect();  		
		//I2C_P2S(pBuff[i]);	AckDetect();
		if (page==MDIN_HOST_ID) MDINDLY_10uSec(5);	// for stability of font osd display on 190906
	}

	I2C_P2S((BYTE)(HIBYTE(((PWORD)pBuff)[i]))); AckDetect();  		
	I2C_P2S((BYTE)(LOBYTE(((PWORD)pBuff)[i]))); NotAck();//AckDetect();  	
	//I2C_P2S(pBuff[i+1]); AckDetect();  		
	//I2C_P2S(pBuff[i]);   NotAck();//AckDetect();  	


	I2C_Stop();		

	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
unsigned char I2C_Read16(unsigned char ID, unsigned char dAddr, unsigned short	 rAddr, unsigned char* pBuff, unsigned short bytes)		// i2c 16bit multiread
{
	unsigned short i;

	i2c_ch = ID;
		
//	BYTE slaveAddr = I2C_MDIN3xx_ADDR(selectedMDIN);

	I2C_Start();
	I2C_P2S(dAddr&0xFE); AckDetect();

	I2C_P2S((BYTE)(rAddr >> 8));   AckDetect();		
	I2C_P2S((BYTE)(rAddr & 0xFF)); AckDetect();		

	I2C_Start(); 
	I2C_P2S(dAddr|0x01); AckDetect();

	for (i=0; i<bytes/2-1; i++) 
	{
		((PWORD)pBuff)[i]  = ((WORD)I2C_S2P())<<8; AckSend();	// Receive a buffer data
		((PWORD)pBuff)[i] |= ((WORD)I2C_S2P());	   AckSend();	// Receive a buffer data
		//pBuff[i+1] = I2C_S2P(); AckSend();	// Receive a buffer data
		//pBuff[i]   = I2C_S2P(); AckSend();	// Receive a buffer data
	}

	((PWORD)pBuff)[i]  = ((WORD)I2C_S2P())<<8; AckSend();		// Receive a buffer data
	((PWORD)pBuff)[i] |= ((WORD)I2C_S2P());	   NotAck();		// Receive a buffer data
	//pBuff[i+1] = I2C_S2P();	 AckSend();		// Receive a buffer data
	//pBuff[i]   = I2C_S2P();	 NotAck();		// Receive a buffer data

	I2C_Stop();												

	//printf("[I2C_R] nID:%02X, rAddr:%04X, pBuff:%04X, bytes:%04X\n", nID, rAddr, *((PWORD)pBuff), bytes);

	return I2C_OK;
}

