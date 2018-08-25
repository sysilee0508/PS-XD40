#ifndef		__I2C_H__
#define		__I2C_H__


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

#endif	
