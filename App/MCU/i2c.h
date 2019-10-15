#ifndef		__I2C_H__
#define		__I2C_H__

//#define SEPARATE_SDA

typedef enum _I2C_CH
{
	I2C_MAIN = 0,
	I2C_SUB,
	I2C_MAX
} eI2C_CH_t;

extern eI2C_CH_t i2c_ch;

#define I2C_SET_CHANNEL(ch)			i2c_ch = ch

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
// i2c
void I2C_Start(void);
void I2C_Stop(void);
void I2C_P2S(unsigned char Data);
void AckDetect(void);
void AckSend(void);
void NotAck(void);          
unsigned char I2C_S2P(void);
unsigned char I2C_READ(unsigned char slaveaddr,unsigned char regaddr);
void I2C_WRITE(unsigned char slaveaddr, unsigned char regaddr, unsigned char write_data);
void I2C_MultiWrite(unsigned char slaveaddr, unsigned char rAddr, unsigned char* pBuff, unsigned short bytes);
void I2C_MultiRead(unsigned char slaveaddr, unsigned char rAddr, unsigned char* pBuff, unsigned short bytes);

unsigned char I2C_Write16(unsigned char ID, unsigned char dAddr, unsigned char page, unsigned short rAddr, unsigned char* pBuff, unsigned short bytes);
unsigned char I2C_Read16(unsigned char ID, unsigned char dAddr, unsigned short	 rAddr, unsigned char* pBuff, unsigned short bytes);

#endif	
