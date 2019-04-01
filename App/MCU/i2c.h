#ifndef		__I2C_H__
#define		__I2C_H__

typedef enum _I2C_CH
{
	I2C_MAIN,
	I2C_SUB,
	I2C_MAX
} I2C_CH;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
// i2c
void I2C_Start(I2C_CH ch);
void I2C_Stop(I2C_CH ch);
void I2C_P2S(I2C_CH ch, unsigned char Data);
void AckDetect(I2C_CH ch);
void AckSend(I2C_CH ch);
void NotAck(I2C_CH ch);
unsigned char I2C_S2P(I2C_CH ch);
unsigned char I2C_READ(I2C_CH ch, unsigned char slaveaddr,unsigned char regaddr);
void I2C_WRITE(I2C_CH ch, unsigned char slaveaddr, unsigned char regaddr, unsigned char write_data);

#endif	
