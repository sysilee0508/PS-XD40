#include "..\main\common.h"

//#define SPI_DELAY 2
#define SPI_DELAY 5

#define GV7600	0
#define GV7601	1

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SPI_Delay(unsigned int num)
{
    unsigned int i;

	for(i=0;i<num;i++);
}


//---------------------------------------------------------------------------------------------
DWORD SPI_Read(BYTE kind, WORD rAddr)
{
	DWORD rData;		 
	WORD shift_tmp;
	BYTE i;
	
	SPI_SCK_LOW;
	SPI_Delay(SPI_DELAY);
	if(kind == GV7600) SPI_GV7600_CS_LOW;
	else if(kind == GV7601) SPI_GV7601_CS_LOW; 
	SPI_Delay(SPI_DELAY);

	shift_tmp = rAddr|0x8000;
	for(i=0;i<16;i++)			// R/W, RSV, RSV, AIC, A11-A0
	{
		if(shift_tmp & 0x8000) SPI_SDO_HIGH;
		else SPI_SDO_LOW;

		SPI_Delay(SPI_DELAY);
		SPI_SCK_HIGH;
		SPI_Delay(SPI_DELAY);
		SPI_SCK_LOW;
		SPI_Delay(SPI_DELAY);

		shift_tmp <<= 1;
	}

	SPI_Delay(SPI_DELAY*5);

	SPI_SDO_LOW;
	rData = 0;
	for(i=0;i<16;i++)			// received duplicated SDOUT
	{
		rData <<= 1;

		SPI_SCK_HIGH;
		SPI_Delay(SPI_DELAY);

		if(SPI_SDI_INPUT) rData |= 0x0001;
		else rData &= 0xfffe; 
			
		SPI_SCK_LOW;
		SPI_Delay(SPI_DELAY);
	}

	if(kind == GV7600) SPI_GV7600_CS_HIGH;
	else if(kind == GV7601) SPI_GV7601_CS_HIGH; 

	//printf("SPI Read : rAddr[%x], rData[%x] \n",rAddr,rData);		

	return(rData);
}


//---------------------------------------------------------------------------------------------
void SPI_Write(BYTE kind, WORD rAddr, WORD rData)
{
	BYTE i;
	WORD shift_tmp;
	
	//printf("SPI Write : rAddr[%x], rData[%x] \n",rAddr,rData);		

	SPI_SCK_LOW;
	SPI_Delay(SPI_DELAY);
	if(kind == GV7600) SPI_GV7600_CS_LOW;
	else if(kind == GV7601) SPI_GV7601_CS_LOW; 
	SPI_Delay(SPI_DELAY);

	shift_tmp = rAddr;
	for(i=0;i<16;i++)			// R/W, RSV, RSV, AIC, A11-A0
	{
		if(shift_tmp & 0x8000) SPI_SDO_HIGH;
		else SPI_SDO_LOW;

		SPI_Delay(SPI_DELAY);
		SPI_SCK_HIGH;
		SPI_Delay(SPI_DELAY);
		SPI_SCK_LOW;
		SPI_Delay(SPI_DELAY);

		shift_tmp <<= 1;
	}

	SPI_Delay(SPI_DELAY*5);
	
	shift_tmp = rData;
	for(i=0;i<16;i++)
	{
		if(shift_tmp & 0x8000) SPI_SDO_HIGH;
		else SPI_SDO_LOW;

		SPI_Delay(SPI_DELAY);
		SPI_SCK_HIGH;
		SPI_Delay(SPI_DELAY);
		SPI_SCK_LOW;
		SPI_Delay(SPI_DELAY);

		shift_tmp <<= 1;
	}

	if(kind == GV7600) SPI_GV7600_CS_HIGH;
	else if(kind == GV7601) SPI_GV7601_CS_HIGH; 
}	


//---------------------------------------------------------------------------------------------
DWORD GV7601_RegRead(WORD rAddr)
{
	DWORD rData;

	rData = SPI_Read(GV7601,rAddr);
	
	return(rData);
}

//---------------------------------------------------------------------------------------------
void GV7601_RegWrite(WORD rAddr, WORD rData)
{
	 SPI_Write(GV7601, rAddr, rData);	   // dAddr = 0x00;
}	



//---------------------------------------------------------------------------------------------
DWORD GV7600_RegRead(WORD rAddr)
{
	DWORD rData;

	rData = SPI_Read(GV7600, rAddr);
	
	return(rData);
}

//---------------------------------------------------------------------------------------------
void GV7600_RegWrite(WORD rAddr, WORD rData)
{
	 SPI_Write(GV7600, rAddr, rData);	   // dAddr = 0x01;
}
			 
//---------------------------------------------------------------------------------------------



