#ifndef		__SPI_H__
#define		__SPI_H__



// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
// spi
DWORD SPI_Read(BYTE ch, WORD rAddr);
void SPI_Write(BYTE ch, WORD rAddr, WORD rData);

// GV7601
DWORD GV7601_RegRead(WORD rAddr);
void GV7601_RegWrite(WORD rAddr, WORD rData);

DWORD GV7601_2_RegRead(WORD rAddr);
void GV7601_2_RegWrite(WORD rAddr, WORD rData);

// GV7600
DWORD GV7600_RegRead(WORD rAddr);
void GV7600_RegWrite(WORD rAddr, WORD rData);



#endif
