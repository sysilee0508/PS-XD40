// *****************************************************
// Company : Techpoint Inc
// $Id: Tp2801.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2014-03-10 $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __TP2801_H__
#define __TP2801_H__



enum{
		NTSC=0,
		PAL,
		R1080P30,
		R1080P25,
		R720P60,
		R720P50,
		R720P30_TVI20,
		R720P25_TVI20,
//		R720P30,
//		R720P25,
		R1080P60,
		R1080P50,
		RQHD30,
		RQHD25,
		R5M20,
		R8M15,
		R8M125,
		AHD720P30_36M=0x10,
		AHD720P25_36M,
		AHD1080P30=0x12,
		AHD1080P25,
		AHD720P60,
		AHD720P50,
		AHD720P30,
		AHD720P25,
		AHD_QHD30,
		AHD_QHD25,
		AHD5M20,
		AHD8M15=0x1c,
		AHD8M125,
		NR1920x532=0x20,
		NoDet
};

enum{
	TP2801A=0,
	TP2801B,
	TP2803,
	TP2910,
	TP2912,
	TP2912B,
	TP2915
};

typedef struct TVITx_Dn_cmd
{
	BYTE Head;
	BYTE data1;
	BYTE data2;
	BYTE data3;
	BYTE data4;
	
}_TVITx_Dn_cmd;

enum{
	NoCmd = 0,
	ACK
};


CODE BYTE TP2910_AHD1080_UpData_DataSet[];

void Get_AHD_RxD();
void Set_PLL();

void Init_UpData_Comm();
void Get_Interrupt();
void Clear_Interrupt();
void TVI_Init();
void AHD_Init();
void Set_OutputMode();

void Init_TVITx_RegSet();
BYTE Det_TVI_addr();




#endif

