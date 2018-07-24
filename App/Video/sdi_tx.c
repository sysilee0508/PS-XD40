//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2012  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	SDI_Tx.C
// Description 		:	Gennum SDI Tx(GV7600) 
// Ref. Docment		: 
// Revision History 	: v0.1(2012.03.16)

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"..\main\common.h"

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
BYTE SDITX_OutFrmt = 0xff;		   //by hungry 2012.02.12


// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
extern DWORD GV7600_RegRead(WORD rAddr);		//by hungry 2012.02.16
// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

static void SDITX_SetOutVideoFrmt(PMDIN_VIDEO_INFO pINFO)
{
	BYTE frmt = pINFO->stOUT_m.frmt;

	switch(frmt)
	{
		case VIDOUT_720x480i60:	case VIDOUT_720x576i50:
 			//SDI_RATE_SEL0(HIGH);	//SDI_RATE_SEL1(Don't care); //SD 20bit(PCKL@13.5Mhz)
			break;

		case VIDOUT_1280x720p60:  case VIDOUT_1280x720p59:  case VIDOUT_1280x720p50:
		case VIDOUT_1280x720p30: case VIDOUT_1280x720p25: 									//2012.03.19
		case VIDOUT_1920x1080i60: case VIDOUT_1920x1080i59:	case VIDOUT_1920x1080i50:
		case VIDOUT_1920x1080p30: case VIDOUT_1920x1080p25: case VIDOUT_1920x1080p24:	   //by hungry 2012.03.16
			//SDI_RATE_SEL0(LOW);	SDI_RATE_SEL1(LOW);	 			//HD 20bit(PCKL@74.25Mhz)
			break;

		case VIDOUT_1920x1080p60: case VIDOUT_1920x1080p59:	case VIDOUT_1920x1080p50:
			//SDI_RATE_SEL0(LOW);	SDI_RATE_SEL1(HIGH);	 //Full-HD 20bit(PCKL@148.5Mhz)
			break;
	}
}

static void SDI_Tx_ErrPrt(DWORD temp)
{
	if((temp >> 6 ) & 0x01)		//[6]:TRS_PERR(High=Errors in TRS)
		printf("SDI-Tx : Errors in TRS. \n");

	if((temp >> 5 ) & 0x01)		//[5]:Y1_EDH_CS_ERR
		printf("SDI-Tx : Y1 EDH CS ERR, packet being inspected is an EDH packet. \n");

	if((temp >> 4 ) & 0x01)		//[4]:CS_ERR
		printf("SDI-Tx : CS ERR, checksum error is detected. \n");

	if(temp & 0x01)	   			//[0]LOCK_ERR
		printf("SDI-Tx : PLL lock error indication ! \n");
}


//---------------------------------------------------------------------------------------------
void SDITX_ShowStatus(void)
{

	printf("==============================================\n");
	printf("==  SDI Tx(GV7600) status check.. \n");
	printf("----------------------------------------------\n");

  	if((GV7600_RegRead(0x004) >> 2) & 0x01)	   		// standard lock indication
	{
		printf("SDI-Tx : Detected video standard = [%x] \n", ((GV7600_RegRead(0x004) >> 5) & 0x1f));	 //by hungry 2012.03.07
	}
	else
		printf("SDI-Tx : Standard un-lock indication. \n");
 

  	if((GV7600_RegRead(0x004) >> 4) & 0x01)	   		// INT/PROG
 		printf("SDI-Tx : Interlaced signal \n");
	else
		printf("SDI-Tx : Progressive signal \n");
 

  	if((GV7600_RegRead(0x004) >> 1) & 0x01)	   		// Vertical lock indication.
 		printf("SDI-Tx : Vertical lock indication. \n");
	else
		printf("SDI-Tx : Vertical un-lock indication. \n");

 
   	if((GV7600_RegRead(0x004) >> 0) & 0x01)	   		// Horizontal lock indication.
 		printf("SDI-Tx : Horizontal lock indication. \n");
	else
		printf("SDI-Tx : Horizontal un-lock indication. \n");

	printf("SDI-Tx : H-Active[%4d], H-Total[%4d], V-Active[%4d], V-Total[%4d] \n",
			(GV7600_RegRead(0x014) & 0x1fff), (GV7600_RegRead(0x013) & 0x3fff),
			(GV7600_RegRead(0x015) & 0x07ff), (GV7600_RegRead(0x012) & 0x07ff));


	SDI_Tx_ErrPrt(GV7600_RegRead(0x001));					   //SDI input Error mode check.	


	//audio
	if((GV7600_RegRead(0x800) >> 8) & 0x01)	   		// Primary Group asynchronous mode.([8]ASXA)
 		printf("SDI-Tx_audio : Primary Group asynchrous mode, \t");
	else
		printf("SDI-Tx_audio : Primary Group synchrous mode, \t");

   	if((GV7600_RegRead(0x803) >> 2) & 0x01)	   	// [2]ADPG1_DET.
 		printf("Group 1 audio data packets are detected. \n");
	else
		printf("Group 2,3,4 audio data packets are detected. \n");

/*
   	if( !(GV7600_RegRead(0x800) & 0x01) )	   		// Primary Group to embed.([1:0]IDA)
 		printf("SDI-Tx_audio : Primary Group to embeded(Audio group #1) \n");
	else
		printf("SDI-Tx_audio : Primary Group to embeded(Audio group #2 ~ 4) \n");

  	if((GV7600_RegRead(0x803) >> 6) & 0x01)	   	// [6]ACPG1_DET.
 		printf("SDI-Tx_audio : Group 1 audio control packets are detected. \n");
	else
		printf("SDI-Tx_audio : Group 2~4 audio control packets are detected. \n");

   	if((GV7600_RegRead(0x80e) >> 8) & 0x01)	   	// [8]LSB_FIRSTA.
 		printf("SDI-Tx_audio : stereo pair serial input format to use LSB first. \n");
	else
		printf("SDI-Tx_audio : stereo pair serial input format to use MSB first. \n");
*/
}
								   
//---------------------------------------------------------------------------------------------
void SDITX_CtrlHandler(PMDIN_VIDEO_INFO pINFO)
{

	if (SDITX_OutFrmt!= pINFO->stOUT_m.frmt)
	{
		SDITX_SetOutVideoFrmt(pINFO);
		SDITX_OutFrmt = pINFO->stOUT_m.frmt;
	}
}


/*  FILE_END_HERE */
