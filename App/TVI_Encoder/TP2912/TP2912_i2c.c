// *****************************************************
// Company : Techpoint Inc
// $Date: 2013-09-17 
// $Log:  $
// ******************************************************

//#include "Config.h"
//#include "reg.h"
#include "TP2912_Typedefs.h"
#include "TP2912_i2c.h"
#include "I2C.h"

#ifdef TP2912_I2C_FUNC

#define I2C_FAST


#ifdef I2C_FAST
#define I2Cdelay()
#else
 void I2Cdelay(int cnt)
 {
	int i;
	for (i=0;i<cnt;i++) {
		};
}

#endif

void I2C_CntDelay(int cnt){
	while(cnt){
		I2Cdelay()
		cnt--;
	}
}

#endif //TP2912_I2C_FUNC



void TP2912_WriteI2C(BYTE addr, BYTE index, BYTE val)
{
	I2C_SET_CHANNEL(I2C_SUB);
	I2C_WRITE(addr, index, val);
}

void TP2912_WriteI2C16(BYTE addr, BYTE indexH, BYTE indexL, BYTE valH, BYTE valL)
{
	BYTE writeBuf[4];
	unsigned char cnt = 0;

	I2C_SET_CHANNEL(I2C_SUB);

	writeBuf[0] = indexH;
	writeBuf[1] = indexL;
	writeBuf[2] = valH;
	writeBuf[3] = valL;

//	I2C_MultiWrite(addr, writeBuf, 4);
	I2C_Start();

	I2C_P2S(addr & 0xFE);
	AckDetect();
	
	for(cnt = 0; cnt < 4; cnt++)
	{
		I2C_P2S(writeBuf[cnt]);
		AckDetect();
	}
	
	I2C_Stop();	
}

BYTE TP2912_ReadI2C(BYTE addr, BYTE index)
{
#ifdef TP2912_I2C_FUNC
	BYTE val;

	I2C_Start();
	TP2912_I2CWriteData(addr);
	TP2912_I2CWriteData(index);
	I2C_Start();
	TP2912_I2CWriteData(addr | 0x01);
	val = TP2912_I2CReadData();
	I2C_Stop();
	
	return val;
#else
	I2C_SET_CHANNEL(I2C_SUB);
	return I2C_READ(addr, index);
#endif

}

WORD TP2912_ReadI2C16(BYTE addr, BYTE indexH, BYTE indexL)
{
	WORD val;

#ifdef TP2912_I2C_FUNC
	I2C_Start();
	TP2912_I2CWriteData(addr);
	TP2912_I2CWriteData(indexH);
	TP2912_I2CWriteData(indexL);
	I2C_Stop();
	I2C_Start();
	TP2912_I2CWriteData(addr | 0x01);
	val =(WORD) TP2912_I2CReadDataWithACK();
	val <<=8;
	val |=(WORD) TP2912_I2CReadDataWithNAK();
	I2C_Stop();
#else
	I2C_SET_CHANNEL(I2C_SUB);
	
	I2C_Start();

	I2C_P2S(addr & 0xFE);
	AckDetect();

	I2C_P2S(indexH);
	AckDetect();
	I2C_P2S(indexL);
	AckDetect();
	I2C_Stop();

	I2C_Start(); 
	I2C_P2S(addr | 0x01);
	AckDetect();

	val = I2C_S2P();
	val << 8;
	AckSend();
	val |= I2C_S2P();
	NotAck();

	I2C_Stop();
#endif
	return val;
}

#ifdef TP2912_I2C_FUNC
BYTE TP2912_I2CReadDataWithNAK(void)
{
//	return I2CReadData();
	int i;
	BYTE value=0;

	I2C_SDA = 1;	I2Cdelay(5);
	I2C_SCL = 0;

	for(i=0; i<8; i++) {
		I2C_SCL = 1;
		value <<= 1;
		if(I2C_SDA) value |= 1;
		I2C_SCL = 0;
	}
	I2C_SDA = 1;	I2Cdelay(5); 	//nck
	I2C_SCL = 1;	I2Cdelay(5);
	I2C_SCL = 0;

	return value;
}

void TP2912_I2CWriteData(BYTE value)
{
	int i;
	I2C_SCL = 0;
	for(i=0; i<8; i++){	
//		I2Cdelay(5);
		if((value & 0x80) != 0) 
			I2C_SDA = 1;
		else 			 
			I2C_SDA = 0;
		I2Cdelay();
//		I2C_CntDelay(0);
		I2C_SCL = 1;
		I2Cdelay();
		value<<=1;
		I2C_SCL = 0;
	}
	I2C_SCL = 1;		
	I2C_CntDelay(0);
	I2C_SCL = 0;		
}

BYTE TP2912_I2CReadData()
{
	int i;
	BYTE value=0;

	
	for(i=0; i<8; i++) {
		I2C_SCL = 0;
		I2Cdelay(10);
		I2C_SCL = 1;
		value <<= 1;
		if(I2C_SDA) value |= 1;
		I2Cdelay(10);
		I2C_SCL = 0;
		I2Cdelay(10);
	}

	I2C_SCL = 0;	I2Cdelay(10);
	I2C_SCL = 1;	I2Cdelay(10);
	I2C_SCL = 0;	I2Cdelay(10);

	return value;

}

BYTE TP2912_I2CReadDataWithACK()
{
	int i;
	BYTE value=0;

	I2C_SDA = 1;	I2Cdelay(10);
	I2C_SCL = 0;

	for(i=0; i<8; i++) {
		I2C_SCL = 1;
		value <<= 1;
		if(I2C_SDA) value |= 1;
		I2C_SCL = 0;
	}
	I2C_SDA = 0;	I2Cdelay(10);		//ack
	I2C_SCL = 1;	I2Cdelay(10);
	I2C_SCL = 0;

	return value;
}

#endif
