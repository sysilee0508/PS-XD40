#ifndef		__I2C_H__
#define		__I2C_H__

typedef enum _I2C_CH
{
	I2C_MAIN,
	I2C_SUB,
	I2C_MAX
} eI2C_CH_t;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
// i2c
void I2C_Start(eI2C_CH_t ch);
void I2C_Stop(eI2C_CH_t ch);
void I2C_P2S(eI2C_CH_t ch, unsigned char Data);
void AckDetect(eI2C_CH_t ch);
void AckSend(eI2C_CH_t ch);
void NotAck(eI2C_CH_t ch);
unsigned char I2C_S2P(eI2C_CH_t ch);
unsigned char I2C_READ(eI2C_CH_t ch, unsigned char slaveaddr,unsigned char regaddr);
void I2C_WRITE(eI2C_CH_t ch, unsigned char slaveaddr, unsigned char regaddr, unsigned char write_data);

#endif	
