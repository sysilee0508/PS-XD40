//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2010  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	MDINI2C.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"mdin3xx.h"
#include 	"..\main\common.h"

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------
#define		I2C_OK				0
#define		I2C_NOT_FREE		1
#define		I2C_HOST_NACK		2
#define		I2C_TIME_OUT		3

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
static WORD PageID = 0;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// You must make functions which is defined below.
static BYTE MDINI2C_Write(MDIN_CHIP_ID_t chipId, BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes);
static BYTE MDINI2C_Read(MDIN_CHIP_ID_t chipId, BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes);

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE MDINI2C_SetPage(BYTE nID, WORD page)
BYTE MDINI2C_SetPage(MDIN_CHIP_ID_t chipId, BYTE nID, WORD page)
{
#if	defined(SYSTEM_USE_MDIN380)&&defined(SYSTEM_USE_BUS_HIF)
	MDINBUS_SetPageID(page);	// set pageID to BUS-IF
#endif

	if (page==PageID) return I2C_OK;	PageID = page;
	return MDINI2C_Write(chipId, nID, 0x400, (PBYTE)&page, 2);	// write page
}

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE MHOST_I2CWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
BYTE MHOST_I2CWrite(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(chipId, MDIN_HOST_ID, 0x0000);	// write host page
	if (err) return err;

	err = MDINI2C_Write(chipId, MDIN_HOST_ID, rAddr, pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE MHOST_I2CRead(WORD rAddr, PBYTE pBuff, WORD bytes)
BYTE MHOST_I2CRead(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(chipId, MDIN_HOST_ID, 0x0000);	// write host page
	if (err) return err;

	err = MDINI2C_Read(chipId, MDIN_HOST_ID, rAddr, pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE LOCAL_I2CWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
BYTE LOCAL_I2CWrite(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(chipId, MDIN_LOCAL_ID, 0x0101);	// write local page
	if (err) return err;

	err = MDINI2C_Write(chipId, MDIN_LOCAL_ID, rAddr, pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE LOCAL_I2CRead(WORD rAddr, PBYTE pBuff, WORD bytes)
BYTE LOCAL_I2CRead(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	WORD RegOEN, err = I2C_OK;

	if		(rAddr>=0x030&&rAddr<0x036)	RegOEN = 0x04;	// mfc-size
	else if (rAddr>=0x043&&rAddr<0x045)	RegOEN = 0x09;	// out-ptrn
	else if (rAddr>=0x062&&rAddr<0x083)	RegOEN = 0x09;	// enhance
	else if (rAddr>=0x088&&rAddr<0x092)	RegOEN = 0x09;	// out-sync
	else if (rAddr>=0x094&&rAddr<0x097)	RegOEN = 0x09;	// out-sync
	else if (rAddr>=0x09a&&rAddr<0x09c)	RegOEN = 0x09;	// bg-color
	else if (rAddr>=0x0a0&&rAddr<0x0d0)	RegOEN = 0x09;	// out-ctrl
	else if (              rAddr<0x100)	RegOEN = 0x01;	// in-sync
	else if (rAddr>=0x100&&rAddr<0x140)	RegOEN = 0x05;	// main-fc
	else if (rAddr>=0x140&&rAddr<0x1a0)	RegOEN = 0x07;	// aux
	else if (rAddr>=0x1a0&&rAddr<0x1c0)	RegOEN = 0x03;	// arbiter
	else if (rAddr>=0x1c0&&rAddr<0x1e0)	RegOEN = 0x02;	// fc-mc
	else if (rAddr>=0x1e0&&rAddr<0x1f8)	RegOEN = 0x08;	// encoder
	else if (rAddr>=0x1f8&&rAddr<0x200)	RegOEN = 0x0a;	// audio
	else if (rAddr>=0x200&&rAddr<0x280)	RegOEN = 0x04;	// ipc
	else if (rAddr>=0x2a0&&rAddr<0x300)	RegOEN = 0x07;	// aux-osd
	else if (rAddr>=0x300&&rAddr<0x380)	RegOEN = 0x06;	// osd
	else if (rAddr>=0x3c0&&rAddr<0x3f8)	RegOEN = 0x09;	// enhance
	else								RegOEN = 0x00;	// host state

	err = LOCAL_I2CWrite(chipId, 0x3ff, (PBYTE)&RegOEN, 2);	// write reg_oen
	if (err) return err;

	err = MDINI2C_Read(chipId, MDIN_LOCAL_ID, rAddr, pBuff, bytes);
	return err;
}

#if defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN380)
//--------------------------------------------------------------------------------------------------------------------------
//static BYTE MHDMI_I2CWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
BYTE MHDMI_I2CWrite(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(chipId, MDIN_HDMI_ID, 0x0202);	// write hdmi page
	if (err) return err;

	err = MDINI2C_Write(chipId, MDIN_HDMI_ID, rAddr/2, (PBYTE)pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHDMI_GetWriteDone(MDIN_CHIP_ID_t chipId)//(void)
{
	WORD rVal = 0, count = 100, err = I2C_OK;

	while (count&&(rVal==0)) {
		err = MDINI2C_Read(chipId, MDIN_HDMI_ID, 0x027, (PBYTE)&rVal, 2);
		if (err) return err;	rVal &= 0x04;	count--;
	}
	return (count)? I2C_OK : I2C_TIME_OUT;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHDMI_HOSTRead(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff)
{
	WORD rData, err = I2C_OK;

	err = MDINI2C_SetPage(chipId, MDIN_HOST_ID, 0x0000);	// write host page
	if (err) return err;

	err = MDINI2C_Write(chipId, MDIN_HOST_ID, 0x025, (PBYTE)&rAddr, 2);
	if (err) return err;	rData = 0x0003;
	err = MDINI2C_Write(chipId, MDIN_HOST_ID, 0x027, (PBYTE)&rData, 2);
	if (err) return err;	rData = 0x0002;
	err = MDINI2C_Write(chipId, MDIN_HOST_ID, 0x027, (PBYTE)&rData, 2);
	if (err) return err;

	// check done flag
	err = MHDMI_GetWriteDone(chipId); if (err) {mdinERR = 4; return err;}
	
	err = MDINI2C_Read(chipId, MDIN_HOST_ID, 0x026, (PBYTE)pBuff, 2);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE MHDMI_I2CRead(WORD rAddr, PBYTE pBuff, WORD bytes)
BYTE MHDMI_I2CRead(MDIN_CHIP_ID_t chipId, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	// DDC_STATUS, DDC_FIFOCNT
	if (rAddr==0x0f2||rAddr==0x0f5) return MHDMI_HOSTRead(chipId, rAddr, pBuff);

	err = MDINI2C_SetPage(chipId, MDIN_HDMI_ID, 0x0202);	// write hdmi page
	if (err) return err;

	err = MDINI2C_Read(chipId, MDIN_HDMI_ID, rAddr/2, (PBYTE)pBuff, bytes);
	return err;
}
#endif	/* defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN380) */

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE SDRAM_I2CWrite(DWORD rAddr, PBYTE pBuff, DWORD bytes)
BYTE SDRAM_I2CWrite(MDIN_CHIP_ID_t chipId, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	WORD row, len, idx, unit, err = I2C_OK;

	err = MDINI2C_RegRead(chipId, MDIN_HOST_ID, 0x005, &unit);	if (err) return err;
	unit = (unit&0x0100)? 4096 : 2048;

	while (bytes>0) {
		row = ADDR2ROW(rAddr, unit);	// get row
		idx = ADDR2COL(rAddr, unit);	// get col
		len = MIN((unit/2)-(rAddr%(unit/2)), bytes);

		err = MDINI2C_RegWrite(chipId, MDIN_HOST_ID, 0x003, row); if (err) return err;	// host access
		err = MDINI2C_SetPage(chipId, MDIN_HOST_ID, 0x0303); if (err) return err;	// write sdram page
		err = MDINI2C_Write(chipId, MDIN_SDRAM_ID, idx/2, (PBYTE)pBuff, len); if (err) return err;
		bytes-=len; rAddr+=len; pBuff+=len;
	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
//static BYTE SDRAM_I2CRead(DWORD rAddr, PBYTE pBuff, DWORD bytes)
BYTE SDRAM_I2CRead(MDIN_CHIP_ID_t chipId, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	WORD row, len, idx, unit, err = I2C_OK;

	err = MDINI2C_RegRead(chipId, MDIN_HOST_ID, 0x005, &unit);	if (err) return err;
	unit = (unit&0x0100)? 4096 : 2048;

	while (bytes>0) {
		row = ADDR2ROW(rAddr, unit);	// get row
		idx = ADDR2COL(rAddr, unit);	// get col
		len = MIN((unit/2)-(rAddr%(unit/2)), bytes);

		err = MDINI2C_RegWrite(chipId, MDIN_HOST_ID, 0x003, row); if (err) return err;	// host access
		err = MDINI2C_SetPage(chipId, MDIN_HOST_ID, 0x0303); if (err) return err;	// write sdram page
		err = MDINI2C_Read(chipId, MDIN_SDRAM_ID, idx/2, (PBYTE)pBuff, len); if (err) return err;
		bytes-=len; rAddr+=len; pBuff+=len;
	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_WriteByID(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	BYTE err = I2C_OK;

	switch (nID&0xfe) {
		case MDIN_HOST_ID:	err = MHOST_I2CWrite(chipId, rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_LOCAL_ID:	err = LOCAL_I2CWrite(chipId, rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_SDRAM_ID:	err = SDRAM_I2CWrite(chipId, rAddr, (PBYTE)pBuff, bytes); break;

	#if defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN380)
		case MDIN_HDMI_ID:	err = MHDMI_I2CWrite(chipId, rAddr, (PBYTE)pBuff, bytes); break;
	#endif

	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_ReadByID(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	BYTE err = I2C_OK;

	switch (nID&0xfe) {
		case MDIN_HOST_ID:	err = MHOST_I2CRead(chipId, rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_LOCAL_ID:	err = LOCAL_I2CRead(chipId, rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_SDRAM_ID:	err = SDRAM_I2CRead(chipId, rAddr, (PBYTE)pBuff, bytes); break;

	#if defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN380)
		case MDIN_HDMI_ID:	err = MHDMI_I2CRead(chipId, rAddr, (PBYTE)pBuff, bytes); break;
	#endif

	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
#if	defined(SYSTEM_USE_MDIN380)&&defined(SYSTEM_USE_BUS_HIF)
MDIN_ERROR_t MDINI2C_SetPageID(WORD nID)
{
	PageID = nID;
	return MDIN_NO_ERROR;
}
#endif

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_MultiWrite(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	return (I2C_WriteByID(chipId, nID, rAddr, (PBYTE)pBuff, bytes))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_RegWrite(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, WORD wData)
{
	return (MDINI2C_MultiWrite(chipId, nID, rAddr, (PBYTE)&wData, 2))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_MultiRead(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	return (I2C_ReadByID(chipId, nID, rAddr, (PBYTE)pBuff, bytes))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_RegRead(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, PWORD rData)
{
	return (MDINI2C_MultiRead(chipId, nID, rAddr, (PBYTE)rData, 2))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
// bPos ��ŭ �������� ��Ʈ�� �̵��� ��ġ�� bCnt��ŭ�� ��Ʈ��(bData)�� �� �ִ´�.     
//-------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_RegField(MDIN_CHIP_ID_t chipId, BYTE nID, DWORD rAddr, WORD bPos, WORD bCnt, WORD bData)
{
	WORD temp;

	if (bPos>15||bCnt==0||bCnt>16||(bPos+bCnt)>16) return MDIN_INVALID_PARAM;
	if (MDINI2C_RegRead(chipId, nID, rAddr, &temp)) return MDIN_I2C_ERROR;
	bCnt = ~(0xffff<<bCnt);		  //bCnt = b00001111 , bCnt = 2, bPos = 1.
	temp &= ~(bCnt<<bPos);		  //temp &= b11100001
	temp |= ((bData&bCnt)<<bPos);
	return (MDINI2C_RegWrite(chipId, nID, rAddr, temp))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

#ifdef MDIN_MULTI_DEVICE
static void MIDNI2C_FindI2CInfo(MDIN_CHIP_ID_t chipId, BYTE* slave_addr, eI2C_CH_t* i2c)
{
	switch(chipId)
	{
		case MDIN_CHIP_ID_A:
			*slave_addr = I2C_MDIN3xx_ADDR_AC;
			*i2c = I2C_MAIN;
			break;
		case MDIN_CHIP_ID_B:
			*slave_addr = I2C_MDIN3xx_ADDR_BD;
			*i2c = I2C_MAIN;
			break;
		case MDIN_CHIP_ID_C:
			*slave_addr = I2C_MDIN3xx_ADDR_AC;
			*i2c = I2C_SUB;
			break;
		case MDIN_CHIP_ID_D:
			*slave_addr = I2C_MDIN3xx_ADDR_BD;
			*i2c = I2C_SUB;
			break;
	}
}
#endif

//--------------------------------------------------------------------------------------------------------------------------
// Drive Function for I2C read & I2C write
// You must make functions which is defined below.
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#ifdef MDIN_MULTI_DEVICE
static BYTE MDINI2C_Write(MDIN_CHIP_ID_t chipId, BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes)
#else
static BYTE MDINI2C_Write(BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes)
#endif
{
	WORD i;
	BYTE slave_addr;
#ifdef MDIN_MULTI_DEVICE
	eI2C_CH_t i2c_ch;

	MIDNI2C_FindI2CInfo(chipId, &slave_addr, &i2c_ch);
#else
	slave_addr = I2C_MDIN3xx_ADDR
#endif

	I2C_Start(i2c_ch);
	I2C_P2S(i2c_ch, slave_addr&0xFE); AckDetect(i2c_ch);

	I2C_P2S(i2c_ch, (BYTE)(rAddr >> 8));   AckDetect(i2c_ch);
	I2C_P2S(i2c_ch, (BYTE)(rAddr & 0xFF)); AckDetect(i2c_ch);
	
	I2C_Start(i2c_ch);
	I2C_P2S(i2c_ch, slave_addr&0xFE); AckDetect(i2c_ch);
	
	for (i=0; i<bytes/2-1; i++) 
	{
		I2C_P2S(i2c_ch, (BYTE)(HIBYTE(((PWORD)pBuff)[i]))); AckDetect(i2c_ch);
		I2C_P2S(i2c_ch, (BYTE)(LOBYTE(((PWORD)pBuff)[i]))); AckDetect(i2c_ch);
		//I2C_P2S(pBuff[i+1]);AckDetect();  		
		//I2C_P2S(pBuff[i]);	AckDetect();  	
	}

	I2C_P2S(i2c_ch, (BYTE)(HIBYTE(((PWORD)pBuff)[i]))); AckDetect(i2c_ch);
	I2C_P2S(i2c_ch, (BYTE)(LOBYTE(((PWORD)pBuff)[i]))); NotAck(i2c_ch);//AckDetect();
	//I2C_P2S(pBuff[i+1]); AckDetect();  		
	//I2C_P2S(pBuff[i]);   NotAck();//AckDetect();  	


	I2C_Stop(i2c_ch);

	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
#ifdef MDIN_MULTI_DEVICE
static BYTE MDINI2C_Read(MDIN_CHIP_ID_t chipId, BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes)
#else
static BYTE MDINI2C_Read(BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes)
#endif
{
	WORD i;	
	BYTE slave_addr;
#ifdef MDIN_MULTI_DEVICE
	eI2C_CH_t i2c_ch;

	MIDNI2C_FindI2CInfo(chipId, &slave_addr, &i2c_ch);
#else
	slave_addr = I2C_MDIN3xx_ADDR
#endif

	I2C_Start(i2c_ch);
	I2C_P2S(i2c_ch, slave_addr&0xFE); AckDetect(i2c_ch);

	I2C_P2S((BYTE)(rAddr >> 8));   AckDetect(i2c_ch);
	I2C_P2S((BYTE)(rAddr & 0xFF)); AckDetect(i2c_ch);

	I2C_Start(); 
	I2C_P2S(slave_addr|0x01); AckDetect(i2c_ch);

	for (i=0; i<bytes/2-1; i++) 
	{
		((PWORD)pBuff)[i]  = ((WORD)I2C_S2P(i2c_ch))<<8; AckSend(i2c_ch);	// Receive a buffer data
		((PWORD)pBuff)[i] |= ((WORD)I2C_S2P(i2c_ch));	   AckSend(i2c_ch);	// Receive a buffer data
		//pBuff[i+1] = I2C_S2P(); AckSend();	// Receive a buffer data
		//pBuff[i]   = I2C_S2P(); AckSend();	// Receive a buffer data
	}

	((PWORD)pBuff)[i]  = ((WORD)I2C_S2P(i2c_ch))<<8; AckSend(i2c_ch);		// Receive a buffer data
	((PWORD)pBuff)[i] |= ((WORD)I2C_S2P(i2c_ch));	   NotAck(i2c_ch);		// Receive a buffer data
	//pBuff[i+1] = I2C_S2P();	 AckSend();		// Receive a buffer data
	//pBuff[i]   = I2C_S2P();	 NotAck();		// Receive a buffer data


	I2C_Stop(i2c_ch);

	//printf("[I2C_R] nID:%02X, rAddr:%04X, pBuff:%04X, bytes:%04X\n", nID, rAddr, *((PWORD)pBuff), bytes);

	return I2C_OK;
}

/*  FILE_END_HERE */
