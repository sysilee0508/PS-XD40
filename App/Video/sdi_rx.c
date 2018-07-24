//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2012  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	SDI_Rx.C
// Description 		:	Gennum SDI Rx(GV7601)
// Ref. Docment		: 
// Revision History 	: v0.1(2012.03.16)

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"..\main\common.h"

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------
//002h
#define		SDI_RX_VD_STD_ERR		10		// Video Standard Error indication.
#define		SDI_RX_FF_CRC_ERR		9		// EDH Full Frame Error indication.
#define		SDI_RX_AP_CRC_ERR		8		// EDH Active Picture CRC error indication.
#define		SDI_RX_CCS_ERR			6		// Chroma ancillary data checksum error indication.
#define		SDI_RX_YCS_ERR			5		// Luma ancillary data checksum error indication.
#define		SDI_RX_CCRC_ERR			4		// Chroma CRC error indication.
#define		SDI_RX_YCRC_ERR			3		// Luma CRC error indication.
#define		SDI_RX_LNUM_ERR			2		// Line number error indication.
#define		SDI_RX_SAV_ERR			1		// SAV error indication.
#define		SDI_RX_EAV_ERR			0		// EAV error indication.

//006h
#define		SDI_RX_VD_STD			8		//13-8:Detected Video Standard.

//022h
#define		SDI_RX_STD_LOCK				12	//Video standard lock.
#define		SDI_RX_INT_PROG				11	//Interlaced or progressive
#define		SDI_RX_ACTLINE_PER_FIELD	10	//10-0 Active lines per frame.
 
//023h
#define		SDI_RX_V_LOCK				1	//indicates that the timing signal generator is locked to vertical timing.
#define		SDI_RX_H_LOCK				0	//indicates that the timing signal generator is locked to horizontal timing.


//024h
#define		SDI_RX_AUTO_MAN				2	//Detect data rate automatically or program manually

//025h
#define		SDI_RX_FORMAT_ERR			6	//Indicates standard is nor recognized for CEA 861 conversion.

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
extern DWORD GV7601_RegRead(WORD rAddr);				 //by hungry 2012.02.10
//extern void GV7601_RegWrite(WORD rAddr, WORD rData);	 //by hungry 2012.02.27
// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
//static void SDIRX_SetInitial(void)		 //moved on 'video.c'

static void SDI_Rx_ErrPrt(DWORD temp)
{
	if((temp >> SDI_RX_VD_STD_ERR) & 0x01)
		printf("SDI-Rx : Video Standard Error indicated \n");

	if((temp >> SDI_RX_FF_CRC_ERR) & 0x01)
		printf("SDI-Rx : EDH Full Frame Error indicated \n");

	if((temp >> SDI_RX_AP_CRC_ERR) & 0x01)
		printf("SDI-Rx : EDH Active Picture CRC error indicated \n");

	if((temp >> SDI_RX_CCS_ERR) & 0x01)
		printf("SDI-Rx : Chroma ancillary data checksum error indicated \n");

	if((temp >> SDI_RX_YCS_ERR) & 0x01)
		printf("SDI-Rx : Luma ancillary data checksum error indicated \n");

	if((temp >> SDI_RX_CCRC_ERR) & 0x01)
		printf("SDI-Rx : Chroma CRC error indication \n");

	if((temp >> SDI_RX_YCRC_ERR) & 0x01)
		printf("SDI-Rx : Luma CRC error indicated \n");

	if((temp >> SDI_RX_LNUM_ERR) & 0x01)
		printf("SDI-Rx : Line number error indicated \n");

	if((temp >> SDI_RX_SAV_ERR) & 0x01)
		printf("SDI-Rx : SAV error indicated \n");

	if((temp >> SDI_RX_EAV_ERR) & 0x01)
		printf("SDI-Rx : EAV error indicated \n");
}

//---------------------------------------------------------------------------------------------
void SDIRX_Input_Resolution(void)	  //by flcl 2012.12.06
{
	DWORD SDI_VD_STD = 0x0;

	SDI_VD_STD = ((GV7601_RegRead(0x006) >> SDI_RX_VD_STD) & 0x03f);	   // Get video std info.

	switch(SDI_VD_STD)			   
	{
		case 0x16: case 0x17: case 0x19: case 0x1b:
			Video1_In_Res_Val = 1;
			break;

		case 0x18: case 0x1a:
			Video1_In_Res_Val = 2;
			break;

		case 0x20: case 0x00:
			Video1_In_Res_Val = 3;
			break;

		case 0x24: case 0x04:
			Video1_In_Res_Val = 4;
			break;

		case 0x2a: case 0x0a:
			Video1_In_Res_Val = 5;
			break;

		case 0x2c: case 0x0c:
			Video1_In_Res_Val = 6;
			break;

		case 0x2b: case 0x0b:
			Video1_In_Res_Val = 7;
			break;

		case 0x2d: case 0x0d:
			Video1_In_Res_Val = 8;
			break;

		case 0x30: case 0x10:
			Video1_In_Res_Val = 9;
			break;

		case 0x02:
			Video1_In_Res_Val = 10;
			break;

		case 0x06:
			Video1_In_Res_Val = 11;
			break;

		case 0x08:
			Video1_In_Res_Val = 12;
			break;

		case 0x1e:							 //720x480i60hz for MDIN380 656 output //by hungry 2012.03.07
			if( ((GV7601_RegRead(0x020) & 0x3fff) == 1716) &&		 //H-Total = 1716?		
				((GV7601_RegRead(0x01f) & 0x3fff) == 1440) && 		 //H-active = 1440?
				((GV7601_RegRead(0x021) & 0x07ff) == 525) &&		 //V-Total = 525 ?
				((GV7601_RegRead(0x022) & 0x07ff) == 240) )			 //V-active = 240?
			{
				Video1_In_Res_Val = 13;
			}
			break;

		default:
			break;
	}
}


static void SDIRX_PrtVideoSystem(void)	  //by hungry 2012.03.06
{
	DWORD SDI_VD_STD = 0x0;

	SDI_VD_STD = ((GV7601_RegRead(0x006) >> SDI_RX_VD_STD) & 0x03f);	   // Get video std info.

	switch(SDI_VD_STD)			   
	{
		case 0x16: case 0x17: case 0x19: case 0x1b:
			printf("SDI-Rx : CEA-861(6&7), 720(1440) x 480i @ 59.94/60Hz detected(VIDSRC_720x480i60), VD_STD[5:0] = [%x] ! \n", SDI_VD_STD);	   //by hungry 2012.03.07
			break;

		case 0x18: case 0x1a:
			printf("SDI-Rx : CEA-861(21&22), 720(1440) x 576i @ 50Hz detected(VIDSRC_720x576i50), VD_STD[5:0] = [%x] ! \n", SDI_VD_STD);		   //by hungry 2012.03.07
			break;

		case 0x20: case 0x00:
			printf("SDI-Rx : CEA-861(4), 1280 x 720p @ 59.94/60Hz detected(VIDSRC_1280x720p60) ! \n");
			break;

		case 0x24: case 0x04:
			printf("SDI-Rx : CEA-861(19), 1280 x 720p @ 50Hz detected(VIDSRC_1280x720p50) ! \n");
			break;

		case 0x2a: case 0x0a:
			printf("SDI-Rx : CEA-861(5), 1920 x 1080i @ 59Hz detected(VIDSRC_1920x1080i60) ! \n");
			break;

		case 0x2c: case 0x0c:
			printf("SDI-Rx : CEA-861(20), 1920 x 1080i @ 50Hz detected(VIDSRC_1920x1080i50) ! \n");
			break;

		case 0x2b: case 0x0b:
			printf("SDI-Rx : CEA-861(34), 1920 x 1080p @ 29.97/30(59.94/60)Hz detected(VIDSRC_1920x1080p60) \n");
			break;

		case 0x2d: case 0x0d:
			printf("SDI-Rx : CEA-861(33), 1920 x 1080p @ 25(50)Hz detected(VIDSRC_1920x1080p50) ! \n");
			break;

		case 0x30: case 0x10:
			printf("SDI-Rx : CEA-861(32), 1920 x 1080p @ 23.98/24Hz detected ! \n");
			break;

		case 0x02:
			printf("SDI-Rx : CEA-861(-), 1280 x 720p @ 30Hz detected(VIDSRC_1280x720p60) ! \n");	  //by hungry 2012.03.07
			break;

		case 0x06:
			printf("SDI-Rx : CEA-861(-), 1280 x 720p @ 25Hz detected(VIDSRC_1280x720p60) ! \n");	  //by hungry 2012.03.07
			break;

		case 0x08:
			printf("SDI-Rx : CEA-861(-), 1280 x 720p @ 24Hz detected(VIDSRC_1280x720p60) ! \n");	  //by hungry 2012.03.06
			break;

		case 0x1e:							 //720x480i60hz for MDIN380 656 output //by hungry 2012.03.07
			if( ((GV7601_RegRead(0x020) & 0x3fff) == 1716) &&		 //H-Total = 1716?		
				((GV7601_RegRead(0x01f) & 0x3fff) == 1440) && 		 //H-active = 1440?
				((GV7601_RegRead(0x021) & 0x07ff) == 525) &&		 //V-Total = 525 ?
				((GV7601_RegRead(0x022) & 0x07ff) == 240) )			 //V-active = 240?
			{
				printf("SDI-Rx : CEA-861(-), 720 x 480i @ 60Hz detected(VIDSRC_720x480i60) ! \n");		 
			}
			break;

		default:
			printf("SDI-Rx : CEA-861(-), Non standard format detected !.. VD_STD[5:0] = [%x] \n", SDI_VD_STD);
			break;
	}
}


//---------------------------------------------------------------------------------------------
static void SDIRX_GetAudioSystem(void)
{
	DWORD rVal = 0x0;

//HD Audio Core Registers	  0x200h~
	printf("--------------[HD audio]----------------------\n");

	rVal = 0;
	rVal = GV7601_RegRead(0x200);

	if((rVal >> 13) & 0x01)				 //[13]MUTE_ALL
		printf("SDI-Rx_audio_HD : [MUTE : ON], \t");

	printf("[%d bit], format[I2S:3/%d], extract group [#%d], ",	
				(24 - (((rVal >> 8) & 0x03) * 4)),		//[9:8]ASWLA
 				((rVal >> 4) & 0x03), 					//[5:4]AMA
				(((rVal >> 0) &0x03) + 1));				//[1:0]IDA

	printf("freq = [%d] \n",
				((GV7601_RegRead(0x221) >> 1) & 0x07));	 //[3:1]RATEA

	rVal = 0;
	rVal = GV7601_RegRead(0x201);

	if((rVal >> 6) & 0x01)   //[6]DBNA_ERR
		printf("SDI-Rx_audio : Primary group audio Data 'block number sequence' is discontinuous. \n");

	if((rVal >> 4) & 0x01)   //[4]CTRA_DET
		printf("SDI-Rx_audio : Primary group 'audio control packet' is detected. \n");

	if((rVal >> 0) & 0x01)   //[0]ACS_DET1_2A
		printf("SDI-Rx_audio : Primary group 'audio status' detected for ch1 and ch2. \n");

	if((GV7601_RegRead(0x202) >> 1) & 0x01)   //[1]ADPG1_DET
		printf("SDI-Rx_audio : Group 1 audio data packets are detected. \n");

 	if((GV7601_RegRead(0x203) >> 1) & 0x01)   //[1]ECCA_ERROR
		printf("SDI-Rx_audio : Primary group audio data packet error detected \n");

 	if((GV7601_RegRead(0x206) >> 0) & 0x01)   //[0]CH1_VALIDA
		printf("SDI-Rx_audio : Primary group channel 1 sample validity flag. \n");

 	if((GV7601_RegRead(0x209) >> 2) & 0x01)   //[2]MISSING_PHASE
		printf("SDI-Rx_audio : Embedded phase info for chosen group missing or incorrect. \n");

 	if((GV7601_RegRead(0x209) >> 0) & 0x01)   //[0]NO_PHASEA_DATA
		printf("SDI-Rx_audio : Primary group has invalid embedded clock information. \n");

	if((GV7601_RegRead(0x208) >> 8) & 0x01)   //[8]LSB_FIRSTA
		printf("SDI-Rx_audio : Primary group serial output formats to use LSB first \n");
	else
		printf("SDI-Rx_audio : Primary group serial output formats to use MSB first \n");

	printf("SDI-Rx_audio_HD : OUT CH1 src = [#%d] \n",
					(((GV7601_RegRead(0x20A) >> 0) & 0x07) +1));   //[2:0]OP1_SRC



//SD Audio Core Registers	  0x400h~					   //by hungry 2012.03.09
	printf("--------------[SD audio]----------------------\n");

	rVal = 0;
	rVal = GV7601_RegRead(0x400);

	if((rVal >> 12) & 0x01)	   //[12]MUTE_ALL
		printf("SDI-Rx_audio_SD : all output muted \n");

	if((rVal >> 4) & 0x01)			//[4]LSB_FIRSTA
		printf("SDI-Rx_audio_SD : CH1/2 output format to use LSB first \n");
	else
		printf("SDI-Rx_audio_SD : CH1/2 output format to use MSB first \n");

	if((rVal >> 0) & 0x03)			//[1:0]IDA
		printf("SDI-Rx_audio_SD : Primary audio group to extract [#%d] \n", ((rVal & 0x03) +1));

	rVal = 0;
	rVal = GV7601_RegRead(0x401);
	if((rVal >> 0) & 0x01)	 //[0]ACS_DET1_2A
		printf("SDI-Rx_audio_SD : Primary group audio status detected for CH1/2 \n");

	if((rVal >> 4) & 0x01)	//[4]CTRA_DET
		printf("SDI-Rx_audio_SD : Primary group audio control packet is detected \n");

	if((rVal >> 6) & 0x01)	//[6]SAMP_DBNA_ERR
		printf("SDI-Rx_audio_SD : Primary group data packet Data Block Number sequence is discontinous \n");

	if((rVal >> 8) & 0x01)	//[8]EXT_DBNA_ERR
		printf("SDI-Rx_audio_SD : Primary group extended data packet DataBlockNumber sequence is discontinuous \n");

	if((rVal >> 10) & 0x01)	//[10]CTL_DBNA_ERR
		printf("SDI-Rx_audio_SD : Primary group control packet Data Block Number sequence is discontinuous \n");

	if((rVal >> 12) & 0x01)	//[12]EXT_DET1_2A
		printf("SDI-Rx_audio_SD : Primary group CH1/2 have extended data \n");

	if(((GV7601_RegRead(0x403) >> 4) & 0x01))   //[4]ADPG1_DET
		printf("SDI-Rx_audio_SD : Group 1 audio data packets are detected. \n");

	if(((GV7601_RegRead(0x404) >> 0) & 0x01))   //[0]CSUM_ERROR
	{
		printf("SDI-Rx_audio_SD : Embeded packet checksum error detected. \n");
	}

 
	printf("SDI-Rx_audio_SD : Word Length[%d], frequency[%d], format[I2S:3/%d] \n",
				(24 - (((GV7601_RegRead(0x408) >> 8) & 0x03) *4)),	  //[9:8]ASWLA
				((GV7601_RegRead(0x422) >> 1) & 0x07),	  //[3:1]RATE1_2A
				((GV7601_RegRead(0x408) >> 0) & 0x03));	  //[1:0]AMA
}

//---------------------------------------------------------------------------------------------
void SDIRX_ShowStatus(void)	   // by hungry 2012.02.16
{
	printf("==============================================\n");
	printf("==  SDI Rx(GV7601) status check.. \n");
	printf("----------------------------------------------\n");

	//SDI Rx error mode check.
	SDI_Rx_ErrPrt(GV7601_RegRead(0x002));					   //SDI input Error mode check.

	//SDI Rx information check.
	if((GV7601_RegRead(0x024) >> SDI_RX_AUTO_MAN) & 0x01)	   // Detect mode check.
		printf("SDI-Rx : Detect data rate automatically ! \n");
	else
		printf("SDI-Rx : Detect data rate program manually ! \n");
 

 	if((GV7601_RegRead(0x025) >> SDI_RX_FORMAT_ERR) & 0x01)   //Format rec. error check.
		printf("SDI-Rx : Input stream is not recognized	for CEA-861 conversion. \n");

	if((GV7601_RegRead(0x022) >> SDI_RX_STD_LOCK) & 0x01)		// Video standard lock check.
		printf("SDI-Rx : Video standard locked. \n");
	else											   //by hungry 2012.03.07
		printf("SDI-Rx : Video standard un-locked. \n");

	printf("SDI-Rx : H-Active[%4d], H-Total[%4d], V-Active[%4d], V-Total[%4d] \n",
				(GV7601_RegRead(0x01f) & 0x3fff), (GV7601_RegRead(0x020) & 0x3fff),
				(GV7601_RegRead(0x022) & 0x07ff), (GV7601_RegRead(0x021) & 0x07ff));
	
	if((GV7601_RegRead(0x022) >> SDI_RX_INT_PROG) & 0x01)	// Interlace/Progressive check.
		printf("SDI-Rx : Interlace video detected ! \n");
	else
		printf("SDI-Rx : Progressive video detected ! \n");

	//SDI Rx video format check.
	SDIRX_PrtVideoSystem();

	SDIRX_GetAudioSystem();		   

	printf("==============================================\n");
}

u8 SDIRX_change_flag = 0;
//---------------------------------------------------------------------------------------------
BYTE SDIRX_GetVideoSystem(void)
{
	DWORD SDI_VD_STD = 0x0;
	BYTE	rVal = 0xff;
	static BYTE	Old_rVal = VIDSRC_1920x1080p60;
	static BYTE	Old_rVal2 = VIDSRC_1920x1080p60;

	static WORD csc_f_coef0_backup = 0;
	static WORD csc_f_coef4_backup = 0;
	static WORD csc_f_coef8_backup = 0;
	static BYTE rVal_status_flag = 5;


	SDI_VD_STD = ((GV7601_RegRead(0x006) >> SDI_RX_VD_STD) & 0x03f);	   // Get video std info.

	switch(SDI_VD_STD)
	{
		case 0x16: case 0x17: case 0x19: case 0x1b:	   //1440x487/480i60hz
			rVal = VIDSRC_720x480i60;
			break;

		case 0x18: case 0x1a:						   //1440x576i50hz
			rVal = VIDSRC_720x576i50;
			break;

		case 0x20: case 0x00:						   //720p60hz
			rVal = VIDSRC_1280x720p60; 
			break;

		case 0x24: case 0x04:						   //720p50hz
			rVal = VIDSRC_1280x720p50;
			break;

		case 0x2a: case 0x0a:						   //1080i60hz
			rVal = VIDSRC_1920x1080i60;
			break;

		case 0x2c: case 0x0c:						   //1080i50hz
			rVal = VIDSRC_1920x1080i50;
			break;

		case 0x2b: case 0x0b:						   //1080p60/30hz
			rVal = VIDSRC_1920x1080p60;
			break;

		case 0x2d: case 0x0d:						   //1080p50/25hz
			rVal = VIDSRC_1920x1080p50;
			break;

		case 0x30: case 0x10:						   //1080p24hz
			rVal = VIDSRC_1920x1080p60;			   //by hungry 2012.03.20
			break;

		case 0x02:
			rVal = VIDSRC_1280x720p60;		 //1280x720p30hz 
			break;

		case 0x06:
			rVal = VIDSRC_1280x720p60;		 //1280x720p25hz 
			break;

		case 0x08:
			rVal = VIDSRC_1280x720p60;		 //1280x720p24hz 
			break;

		case 0x1e:							 //for 1440x480i60hz non-standard //by hungry 2012.03.07
			if( ((GV7601_RegRead(0x020) & 0x3fff) == 1716) &&		 //H-Total = 1716?		
				((GV7601_RegRead(0x01f) & 0x3fff) == 1440) && 		 //H-active = 1440?
				((GV7601_RegRead(0x021) & 0x07ff) == 525) &&		 //V-Total = 525 ?
				((GV7601_RegRead(0x022) & 0x07ff) == 240) )			 //V-active = 240?
					rVal = VIDSRC_720x480i60;		 
			break;

		default:
			break;
	}

#ifdef __9CH__
	if(rVal == 0xff)
	{
		ch9_loss = 1;
		
		if(rVal_status_flag != 0)
		{
			if(rVal_status_flag == 5)
			{
				MDINHIF_RegRead(MDIN_LOCAL_ID ,0x181, &csc_f_coef0_backup);	
				MDINHIF_RegRead(MDIN_LOCAL_ID ,0x185, &csc_f_coef4_backup);	
				MDINHIF_RegRead(MDIN_LOCAL_ID ,0x189, &csc_f_coef8_backup);	
			}

			rVal_status_flag--;

			MDINHIF_RegWrite(MDIN_LOCAL_ID ,0x181, 0);	
			MDINHIF_RegWrite(MDIN_LOCAL_ID ,0x185, 0);	
			MDINHIF_RegWrite(MDIN_LOCAL_ID ,0x189, 0);	

			if (MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 0, 1, 0)) return MDIN_I2C_ERROR;
		}
	}
	else 
	{
		ch9_loss = 0;
		
		if(rVal_status_flag < 5)
		{
			rVal_status_flag = 5;

			MDINHIF_RegWrite(MDIN_LOCAL_ID ,0x181, csc_f_coef0_backup);	
			MDINHIF_RegWrite(MDIN_LOCAL_ID ,0x185, csc_f_coef4_backup);	
			MDINHIF_RegWrite(MDIN_LOCAL_ID ,0x189, csc_f_coef8_backup);	

			if(aux_display_flag)
			{
				if (MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 0, 1, 1)) return MDIN_I2C_ERROR;
			}	
		}
	}
#endif

	if(rVal == 0xff) rVal = Old_rVal;
	else Old_rVal = rVal;

//	printf("CH2 GV7601 rVal = 0x%02x \n", rVal);


	if(Old_rVal2 != rVal)
	{
		SDIRX_change_flag = 1;
	}

	Old_rVal2 = rVal; 
		
	return rVal;
}




//--------------------------------------------------------------------------------------------------
void MDIN_Status(void)		 //by hungry 2012.03.16		   //for test..
{
 	WORD temp=0;

		temp = 0x0000;		
 		MDINDLY_mSec(10);	// delay 10ms

		printf("==============================================\n");
		printf("==  MDIN status check.. \n");
		printf("----------------------------------------------\n");

		//Read 'main_video_mode_control_reg'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x040, &temp))		 
			printf("MDIN : 0x040, I2C read error \n");
		else
		{
			printf("MDIN-In : video_display_en[1/%x], \t",((temp >> 5) & 0x01));
			printf("main_a_sel[1/%x], \t",((temp >> 3) & 0x01));
			printf("main_freeze[0/%x] \n",((temp >> 1) & 0x01));
		}
		temp = 0x0000;

		//read 'in_data_map_mode(in_format_control)'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x000, &temp))		   
			printf("MDIN : 0x000, I2C read error \n");
		else
		{
			printf("MDIN-In : in_data_map_mode[0/%x], \t",((temp >> 4) & 0x0003));  //[5:4]:in_data_map_mode
			printf("cbcr_swap_a[1/%x] \n", ((temp >> 3) & 0x01));	//[3]cbcr_swap_a //by hungry 2012.02.22

		}
		temp = 0x0000;

		//Read 'in_mode_a(in_sync_ctrl)'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x002, &temp))		  
			printf("MDIN : 0x002, I2C read error \n");
		else
			printf("MDIN-In : in_mode_a[1/%x], \t",((temp >> 4) & 0x0003));   //[5:4]:in_mode_a, [13:12]:in_mode_b 
			printf("positive_h/vsync_a[3/%x] \n",((temp >> 1) & 0x03));   //[2:1]:positive_h/vsync_a 	//by hungry 2012.03.05

		temp = 0x0000;

		//Read 'in_size_h_a'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x006, &temp))		 
			printf("MDIN : 0x006, I2C read error \n");
		else
			printf("MDIN-In : in_size_h_a[%4d], \t",temp);			//[12:0]:in_size_h_a

		temp = 0x0000;

		//Read 'in_size_v_a'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x007, &temp))		
			printf("MDIN : 0x007, I2C read error \n");
		else
			printf("in_size_v_a[%4d] \n",temp);			  //[12:0]:in_size_v_a

		temp = 0x0000;

		//Read 'aux_input_ctrl
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x140, &temp))		
			printf("MDIN : 0x140, I2C read error \n");
		else
			printf("MDIN-In : aux_in_cbcr_swap[1/%x], \t", ((temp >> 4) & 0x0001));	//[4]aux_in_cbcr_swap //by hungry 2012.02.22		  //[12:0]:in_size_v_a

		temp = 0x0000;

		//Read 'aux_out_ctrl
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x145, &temp))		
			printf("MDIN : 0x145, I2C read error \n");
		else
			printf("MDIN-In : aux_out_cbcr_swap[0/%x] \n", ((temp >> 6) & 0x0001));	//[6]aux_out_cbcr_swap //by hungry 2012.02.22		  //[12:0]:in_size_v_a

		temp = 0x0000;

		//Read 'out_data_mode(out_mux_ctrl)'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x0A5, &temp))	 
			printf("MDIN : 0x0A5, I2C read error \n");
		else
			printf("MDIN-out : out_data_mode = [14/%d], \n",(temp & 0x0f));			 //[3:0]:out_data_mode

		temp = 0x0000;

		//Read 'digital_out_format & digital_8bit_out_en(out_control)'
		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x0A2, &temp))	
			printf("MDIN : 0x0A2, I2C read error \n");
		else
		{
			printf("MDIN-out : [9:8]dig_clip_mode[0/%x], \t", ((temp >> 8)	& 0x03));	//[9:8]:digital_clip_mode
			printf("[5]dig_sync_en[1/%x], \t", ((temp >> 5)&0x01));	//[5]:digital_sync_en
			printf("[4]dig_cbcr_swap[0/%x] \n", ((temp >> 5)&0x01));	//[4]dig_cbcr_swap //by hungry 2012.02.22
			printf("MDIN-out : [2]dig_8bit_out_en = [0/%x], \t",((temp >>2) & 0x01));	//[2]:digital_8bit_out_en
			printf("[1:0]dig_out_format = [1/%x] \n",(temp & 0x03));		//[1:0]:digital_out_format
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x0A0, &temp))	 
			printf("MDIN : 0x0A0, I2C read error \n");
		else
		{
			printf("MDIN-out : invert_hsync[0/%x], \t ",((temp >>5) & 0x01));			 //[3:0]:out_data_mode
			printf(" invert_vsync[0/%x], \t",((temp >> 4) & 0x01));			 //[3:0]:out_data_mode
			printf(" de_out_mode[2/%x], \n",((temp >> 8) & 0x03));			 //[9:8]:de_out_mode, 2:FIELD_ID, 3:HACTIVE
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_LOCAL_ID, 0x100, &temp))	 
			printf("MDIN : 0x100, I2C read error \n");
		else
		{
			printf("MDIN : mfc_interlaced_input[1/%x], \t ",((temp >>3) & 0x01));			 //[3:mfc_interlaced_input
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_HOST_ID, 0x024, &temp))	 		 //by hungry 2012.03.13
			printf("MDIN : 0x024, I2C read error \n");
		else
		{
			printf("vclk_out_inverse[1/%x], \n ",((temp >>0) & 0x07));			 //[2]:vclk_out_inverse
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_HOST_ID, 0x042, &temp))	  //vclk_src_ctrl
			printf("MDIN : 0x042, I2C read error \n");
		else
		{
			printf("MDIN : ");
			switch((temp >> 4) & 0x01)		//vclk_bypass_sel
			{
				case 0:	printf("VCLK : PLL ");	break;
				case 1: printf("VCLK : VCLK A/B ");	break;
			}
			
			switch((temp >> 0) & 0x03)		//vclk_out_sel
			{
				case 0: printf("/1, \t");	break;
				case 1: printf("/2, \t");	break;
				case 2: printf("/4, \t");	break;
				case 3: printf("/8, \t");	break;
			}
		}
		temp = 0x0000;


		if(MDINI2C_RegRead(MDIN_HOST_ID, 0x020, &temp))	  //pll_dis
			printf("MDIN : 0x020, I2C read error \n");
		else
		{
			printf(",vclk_pll_dis[%x] ", ((temp >>0) & 0x01));
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_HOST_ID, 0x046, &temp))	  //clk_aux_ctrl
			printf("MDIN : 0x046, I2C read error \n");
		else
		{
			printf(",pll_sel_vclk_hclk[%x] ", ((temp >>10) & 0x01));
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_HOST_ID, 0x02a, &temp))	  //vclk_pll_src_sel
			printf("MDIN : 0x02a, I2C read error \n");
		else
		{
			printf(",pll_sel_vclk_xtal[%x] ", ((temp >>0) & 0x01));
		}
		temp = 0x0000;

		if(MDINI2C_RegRead(MDIN_HOST_ID, 0x03a, &temp))	  //vclk_pll_src_sel
			printf("MDIN : 0x03a, I2C read error \n");
		else
		{
			printf(",pll_sel_vclk_hact[%x] \n", ((temp >>0) & 0x01));
		}
		temp = 0x0000;


		printf("----------------------------------------------\n");
}

/*  FILE_END_HERE */
