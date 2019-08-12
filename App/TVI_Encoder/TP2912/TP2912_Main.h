// *****************************************************
// Company : Techpoint Inc
// $Id: Main.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2013-10-17 18:08:40 -0800 (Thu, 17 Oct 2013) $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __MAIN__
#define __MAIN__


//----------- P0 Define ------------
#define MD0_0			P0_0
#define MD0_1			P0_1
#define MD0_2			P0_2
#define MD0_3			P0_3
#define MD0_4			P0_4
#define MD0_5			P0_5
#define MD0_6			P0_6
#define MD0_7			P0_7

//----------- P1 Define ------------
#define MD1_0			P1_0
#define MD1_1			P1_1
#define MD1_2			P1_2
#define MD1_3    		P1_3
#define MD1_4			P1_4
#define R60HZ_n50HZ 	P1_5
#define R6050f_n3025f  	P1_6
#define R1080P_n720P	P1_7

//----------- P3 Define ------------
#define MD3_2			P3_2
#define reset				P3_3
#define MD3_4			P3_4
#define MD3_5			P3_5
#define MD3_6			P3_6
#define MD3_7			P3_7

#define MD2_0			P2_0
#define MD2_1			P2_1
#define MD2_2			P2_2
#define MD2_3			P2_3
#define MD2_4			P2_4
#define MD2_5			P2_5
#define MD2_6			P2_6
#define MD2_7			P2_7

#define Test_Pat		P0_4
//#define	F37M125		P0_5
//#define CurDrive_MD			P0_5
#define CurDrive_MD		1		//Set Voltage Mode, 0=Current Mode
#define Mode_1920x532			P0_5

//#define AHD_nEN			P0_6
#define Diff_nEN			P0_6

#ifdef TP2915BD
#define nRGB			P0_7
#define Buff_nEN_2915	P2_0
#else
#define Buff_nEN		P0_7 
#endif

#define Test 	0
#define Live	1

#define ON	1
#define OFF 0

#define	TRUE	1
#define	FALSE	0

#define Manual  1
#define Auto_   0

void delay(BYTE cnt);

BYTE RS_ready(void);
BYTE RS_rx(void);
void RS_tx(BYTE tx_buf);
void NewLine(void);
void PutsP(PDATA_P BYTE *ptr);
void MonWriteI2C(BYTE addr, BYTE index, BYTE val);
BYTE MonReadI2C(BYTE addr, BYTE index);

void I2CDeviceSet_(BYTE addr, CODE_P BYTE *RegSet);

void I2CDeviceSet( CODE_P BYTE *RegSet); 
void Main_Initialize(void);

void Prompt(void);

BYTE Asc1Bin(BYTE asc);
BYTE Asc2Bin(PDATA_P BYTE *s);
#define DISPLAY_DEBUG_INFO
#endif	// __MAIN__

