// *****************************************************
// Company : Techpoint Inc
// $Id: I2c.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2013-10-17 18:08:40 -0800 (Thu, 17 Oct 2013) $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef	__TP2912_I2C_H__
#define	__TP2912_I2C_H__

#define TP2912_ADDR				0x88 // 0x8A

BYTE TP2912_ReadI2C(BYTE addr, BYTE index);
void TP2912_WriteI2C(BYTE addr, BYTE index, BYTE val);
WORD TP2912_ReadI2C16(BYTE addr, BYTE indexH, BYTE indexL);
void TP2912_WriteI2C16(BYTE addr, BYTE indexH, BYTE indexL, BYTE valH, BYTE valL);

#endif	/* __TP2912_I2C_H__ */
