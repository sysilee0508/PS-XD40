/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Auto detection
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>

#include <raptor3_ioctl.h>
#include <raptor3_fmt.h>
#include "video_fmt_info.h"
#include "raptor3_api.h"
#include "video_output.h"
#include "video_input.h"
#include "clock.h"
#include "_debug_macro.h"
#include "video_auto_detect.h"

#include "hi_comm_vi.h"

char DebugMsgStr[4][256]={"VFC CH1","VFC CH2","VFC CH3","VFC CH4"};
char DebugMsgNoVidStr[4][256] = {"NOVIDEO CH1", "NOVIDEO CH2","NOVIDEO CH3","NOVIDEO CH4"};
char DebugMsgCabDistStr[4][256] = {"DIST CH1","DIST CH2","DIST CH3","DIST CH4",};
char DebugMsgSAMStr[4][256] = {"SAM CH1","SAM CH2","SAM CH3","SAM CH4",};
char DebugMsgHsyncAccumVal1Str[4][256] = {"Hsync1 CH1","Hsync1 CH2","Hsync1 CH3","Hsync1 CH4"};
char DebugMsgHsyncAccumVal2Str[4][256] = {"Hsync2 CH1","Hsync2 CH2","Hsync2 CH3","Hsync2 CH4"};
char DebugMsgHsyncAccumResultStr[4][256] = {"Hsync3 CH1","Hsync3 CH2","Hsync3 CH3","Hsync3 CH4"};
char DebugMsgAGCStr[4][256] = {"AGC CH1","AGC CH2","AGC CH3","AGC CH4"};
char DebugMsgFSCVal1Str[4][256] = {"FSC1 CH1", "FSC1 CH2", "FSC1 CH3","FSC1 CH4"};
char DebugMsgFSCVal2Str[4][256] = {"FSC2 CH1", "FSC2 CH2", "FSC2 CH3","FSC2 CH4"};
char DebugMsgFSCFinalStr[4][256] = {"FSC3 CH1", "FSC3 CH2", "FSC3 CH3","FSC3 CH4"};
char DebugMsgACCGainStr[4][256] = {"ACC CH1", "ACC CH2", "ACC CH3","ACC CH4"};


NC_VIVO_CH_FORMATDEF arrVfcType[0x100] = {
	/*  0x00 */	AHD20_SD_H960_2EX_Btype_NT,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x10 */ AHD20_SD_H960_2EX_Btype_PAL,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x20 */	AHD20_720P_30P_EX_Btype, AHD20_720P_25P_EX_Btype, AHD20_720P_60P, AHD20_720P_50P,
				0,
	/*  0x25 */ TVI_HD_30P_EX, TVI_HD_25P_EX, TVI_HD_60P, TVI_HD_50P, TVI_HD_B_30P_EX, TVI_HD_B_25P_EX,
	/*  0x2B */	CVI_HD_30P_EX, CVI_HD_25P_EX, CVI_HD_60P, CVI_HD_50P,
				0,
	/*  0x30 */	AHD20_1080P_30P, AHD20_1080P_25P,
				0,
	/*	0x33 */ TVI_FHD_30P, TVI_FHD_25P,
	/*  0x35 */	CVI_FHD_30P, CVI_FHD_25P,
				0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x40 */	AHD30_3M_30P, AHD30_3M_25P, AHD30_3M_18P,
				0,
	/*  0x44 */ TVI_3M_18P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x50 */	AHD30_4M_30P, AHD30_4M_25P, AHD30_4M_15P,
				0,
	/*  0x54 */	CVI_4M_30P, CVI_4M_25P,
				0,
	/*  0x57 */ TVI_4M_30P, TVI_4M_25P, TVI_4M_15P,
				0, 0, 0, 0, 0, 0,
	/*  0x60 */	AHD30_8M_X_30P, AHD30_8M_X_25P, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x70 */	AHD30_5M_20P, AHD30_5M_12_5P, AHD30_5_3M_20P, TVI_5M_12_5P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x80 */	AHD30_8M_15P, AHD30_8M_7_5P, AHD30_8M_12_5P, CVI_8M_15P, CVI_8M_12_5P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x90 */	AHD30_6M_18P, AHD30_6M_20P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0xA0 */
};

unsigned char *arrVfcName[0x100] = {
	/*  0x00 */	"NTSC",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x10 */ "PAL",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x20 */	"AHD 720P 30P", "AHD 720P 25P", "AHD 720P 60P", "AHD 720P 50P",
				0,
	/*  0x25 */ "TVI HD 30P EX", "TVI HD 25P EX", "TVI HD 60P", "TVI HD 50P", "TVI HD 30P B", "TVI HD 25P B",
	/*  0x2B */	"CVI HD 30P EX", "CVI HD 25P EX", "CVI HD 60P", "CVI HD 50P",
				0,
	/*  0x30 */	"AHD 1080P 30P", "AHD 1080P 25P",
				0,
	/*	0x33 */ "TVI FHD 30P", "TVI FHD 25P",
	/*  0x35 */	"CVI FHD 30P", "CVI FHD 25P",
				0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x40 */	"AHD 3M 30P", "AHD 3M 25P", "AHD 3M 18P",
				0,
	/*  0x44 */ "TVI 3M 18P",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x50 */	"AHD 4M 30P", "AHD 4M 25P", "AHD 4M 15P",
				0,
	/*  0x54 */	"CVI 4M 30P", "CVI 4M 25P",
				0,
	/*  0x57 */	"TVI 4M 30P", "TVI 4M 25P", "TVI 4M 15P",
				0, 0, 0, 0, 0, 0,
	/*  0x60 */	"AHD 8M 30P X", "AHD 8M 25P X", 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x70 */	"AHD 5M 20P", "AHD 5M 12.5P", "AHD 5.3M 20P", "TVI 5M 12.5P",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x80 */	"AHD 8M 15P", "AHD 8M 7.5P", "AHD 8M 12.5P", "CVI 8M 15P", "CVI 8M 12.5P",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x90 */	"AHD 6M 18P", "AHD30 6M 20P",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0xA0 */
};


unsigned int Distance[0xFF] = {2, 100, 200, 300, 400, 500, 0, };
unsigned int AEQ[0xFF] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0F, };
unsigned int DEQ[0xFF] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0F, };

void _PRINT_NOVIDEO_INFO_2_GUI(unsigned char Ch)
{
	sprintf(DebugMsgStr[Ch],"NO VIDEO");
	sprintf(DebugMsgNoVidStr[Ch],"NO VIDEO");
	sprintf(DebugMsgCabDistStr[Ch],"NO VIDEO");
	sprintf(DebugMsgSAMStr[Ch],"NO VIDEO");
	sprintf(DebugMsgHsyncAccumVal1Str[Ch],"NO VIDEO");
	sprintf(DebugMsgHsyncAccumVal2Str[Ch],"NO VIDEO");
	sprintf(DebugMsgHsyncAccumResultStr[Ch],"NO VIDEO");
	sprintf(DebugMsgAGCStr[Ch],"NO VIDEO");
	sprintf(DebugMsgFSCVal1Str[Ch],"NO VIDEO");
	sprintf(DebugMsgFSCVal2Str[Ch],"NO VIDEO");
	sprintf(DebugMsgFSCFinalStr[Ch],"NO VIDEO");
	sprintf(DebugMsgACCGainStr[Ch],"NO VIDEO");
}

void _PRINT_MANUALMODE_INFO_2_GUI(unsigned char Ch)
{
	sprintf(DebugMsgStr[Ch],"MANUAL");
	sprintf(DebugMsgNoVidStr[Ch],"NoVideo");
	sprintf(DebugMsgCabDistStr[Ch],"CableDist");
	sprintf(DebugMsgSAMStr[Ch],"SAM");
	sprintf(DebugMsgHsyncAccumVal1Str[Ch],"Hsync_Val1");
	sprintf(DebugMsgHsyncAccumVal2Str[Ch],"Hsync_Val2");
	sprintf(DebugMsgHsyncAccumResultStr[Ch],"Hsync_Result");
	sprintf(DebugMsgAGCStr[Ch],"AGC");
	sprintf(DebugMsgFSCVal1Str[Ch],"FSC_Val1");
	sprintf(DebugMsgFSCVal2Str[Ch],"FSC_Val2");
	sprintf(DebugMsgFSCFinalStr[Ch],"FSC_Final");
	sprintf(DebugMsgACCGainStr[Ch],"ACC_Gain");
}

NC_VD_AUTO_VFC_NOVIDEO_STR		g_VFC_NOVIDEO[RAPTOR3_MAX_CH_PER_CHIP];
NC_VD_AUTO_STATUS_INFO_STR		g_AT_STATUS[RAPTOR3_MAX_CH_PER_CHIP];
NC_VD_AUTO_NOVIDEO_FLAG_STR 	g_AT_NOVIDEO_INFO[RAPTOR3_MAX_CH_PER_CHIP];

VERIFICATION_FLAG 					g_FLAG[RAPTOR3_MAX_CH_PER_CHIP];
VERIFICATION_VAL 					g_VAL[RAPTOR3_MAX_CH_PER_CHIP];

CABLE_DISTANCE NC_VD_AUTO_CABLE_DISTANCE_Get (unsigned char Ch, NC_VD_AUTO_CABLE_DIST_STR *pDistance)
{
	unsigned char tmp_cnt = 0;

	while(1)
	{
		usleep(10);
		NC_VD_AUTO_CABLE_DISTANCE_Read(pDistance);
		tmp_cnt = (pDistance->Dist == 0xF) ? tmp_cnt + 1 : 0;

		if(tmp_cnt == 0) break;
		else if(tmp_cnt >= 10)
		{
			DBG_ERR("[%d CH] Cable Distance Get Error\n", Ch);
			break;
		}
	}

	if(pDistance->Dist == 0xF)
		sprintf(DebugMsgCabDistStr[Ch],"Dist : ERROR");
	else
		sprintf(DebugMsgCabDistStr[Ch],"Dist : %3dM", Distance[pDistance->Dist]);

	return (CABLE_DISTANCE)pDistance->Dist;
}

/*******************************************************************************
*	Description		: get eq stage(manual)
*	Argurments		: Ch(channel), pDistance(distance structure)
*	Return value	: distance( eq stage)
*	Modify			:
*	warning			:
*******************************************************************************/
CABLE_DISTANCE NC_VD_MANUAL_CABLE_DISTANCE_Get (unsigned char Ch, NC_VD_MANUAL_CABLE_DIST_STR *pDistance)
{
	unsigned char sGetDistCnt = 0;
	unsigned char sGetDist[10] = {0, };
	unsigned char sMaxGetDistVal;
	unsigned char sMaxDistVal;
	unsigned char ii;

	/* Get Distance 10 Times */
	while(sGetDistCnt < 10)
	{
		usleep(10);

		NC_VD_MANUAL_CABLE_DISTANCE_Read(pDistance);

		sGetDist[ pDistance->Dist ]++;

		sGetDistCnt++;
	}

	sMaxDistVal = sGetDist[0];
	sMaxGetDistVal = 0;

	for(ii = 1; ii < 6; ii++)
		{
		if( sMaxDistVal < sGetDist[ii] )
		{
			sMaxDistVal = sGetDist[ii];
			sMaxGetDistVal = ii;
		}
	}


	DBG_INFO("TESTING... Get Distance Value : ");
	for(ii = 0; ii < 6; ii++)
		DBG_INFO("[ stage: %d _ get_value: %d ]\n", ii, sGetDist[ii]);

	DBG_INFO(" Distance distinguish result : [%d]\n", sMaxGetDistVal);
//	return (CABLE_DISTANCE)pDistance->Dist;
	return sMaxGetDistVal;
}

// 170207 SAM register read
void NC_VD_AUTO_SAM_Get (unsigned char Ch, NC_VD_AUTO_SAM_STR *pSAM)
{
	// (Hsync Accumulation 1 - Hsync Accumulation 2) / AGC(Auto Gain Control) Value Read
	NC_VD_VI_SAM_VAL_Read(pSAM);
																								
	if(pSAM->SAMval != 0)
	{
		sprintf(DebugMsgSAMStr[Ch],"SAM : %3x", pSAM->SAMval);

		DBG_INFO("[%d CH] SAM : %3x\n",Ch, pSAM->SAMval);
	}
	else
	{
		DBG_ERR("[%d CH] SAM : Error [%d]\n",Ch, pSAM->SAMval);
	}
}

// 170207 Hsync Accumulation register read
void NC_VD_AUTO_HSYNC_Get (unsigned char Ch, NC_VD_AUTO_HSYNC_STR* pHsync)
{
	NC_VD_VI_HSYNC_ACCUM_Read(pHsync);
	
	if(pHsync->Hsync_Accum_Result != 0 && pHsync->h_lock)
	{
		sprintf(DebugMsgHsyncAccumVal1Str[Ch],"%08x", pHsync->Hsync_Accum_Val1);
		sprintf(DebugMsgHsyncAccumVal2Str[Ch],"%08x", pHsync->Hsync_Accum_Val2);
		sprintf(DebugMsgHsyncAccumResultStr[Ch],"%08x", pHsync->Hsync_Accum_Result);

		sprintf(DebugMsgFSCVal1Str[Ch],"h_lock:%x", pHsync->h_lock);

		DBG_INFO("[%d CH] Hsync 1 : %08x\n", Ch, pHsync->Hsync_Accum_Val1);
		DBG_INFO("[%d CH] Hsync 2 : %08x\n", Ch, pHsync->Hsync_Accum_Val2);
		DBG_INFO("[%d CH] Hsync Result : %08x\n", Ch, pHsync->Hsync_Accum_Result);
	}
	else
	{
		sprintf(DebugMsgFSCVal1Str[Ch],"h_lock:%x", pHsync->h_lock);
	}

}

// 170207 AGC register read
void NC_VD_AUTO_AGC_Get (unsigned char Ch, NC_VD_AUTO_AGC_STR* pAGC)
{
	NC_VD_AUTO_AGC_Read(pAGC);									// Auto Gain Control Value Read

	if(pAGC->AGCval != 0 && pAGC->agc_lock)
	{
		sprintf(DebugMsgAGCStr[Ch],"AGC : %2x", pAGC->AGCval);
		sprintf(DebugMsgFSCVal2Str[Ch],"AGClock : %x", pAGC->agc_lock);
	}
	else
	{
	DBG_INFO("[%d CH] AGC : %x\n",Ch, pAGC->AGCval);
	sprintf(DebugMsgAGCStr[Ch],"AGC : %2x", pAGC->AGCval);
		sprintf(DebugMsgFSCVal2Str[Ch],"AGClock : %x", pAGC->agc_lock);
	}

}

void NC_VD_AUTO_FLAG_OFF_SET(NC_VD_AUTO_DETECT_INFO_STR* pDetectData)
{
	pDetectData->Flag.AutoDetect_Flag = OFF;
	pDetectData->Flag.NoVideo_Flag = FLAG_NO_VIDEO;

	pDetectData->AutoInfo.p_ThreadOn = OFF;
}

void NC_VD_AUTO_FLAG_ON_SET(NC_VD_AUTO_DETECT_INFO_STR* pDetectData)
{
	pDetectData->VFC.VFC = 0x0;
	pDetectData->NoVideo.NoVid = FLAG_NO_VIDEO;

	pDetectData->Flag.AutoDetect_Flag = OFF;
	pDetectData->Flag.NoVideo_Flag = FLAG_NO_VIDEO;

	pDetectData->AutoInfo.p_ThreadOn = ON;
}

unsigned char NC_APP_VD_AGC_STABLE_Check(NC_VD_AUTO_HSYNC_STR *pHsync,NC_VD_AUTO_AGC_STR* pAGC, NC_VD_AUTO_SAM_STR *pSAM)
{
	//return ((pAGC->agc_lock) && (pHsync->h_lock) && (pHsync->Hsync_Accum_Result != 0) && (pSAM->SAMval != 0));
	return ((pHsync->Hsync_Accum_Result != 0) && (pSAM->SAMval != 0));
}

void NC_VD_VO_Auto_Data_Mode_Set(unsigned char ch, unsigned char devnum, unsigned char SetVal)
{
	NC_VD_AUTO_DATA_OUT_MODE_STR DataOutMode;

	DataOutMode.Ch = ch%4;
	DataOutMode.devnum = devnum;
	DataOutMode.SetVal = SetVal;

	NC_VD_AT_DAT_OUT_MODE_Set(&DataOutMode);
}

void NC_VD_AUTO_NoVideo_Set(unsigned char ch, unsigned char devnum)
{	
	NC_VD_AUTO_NOVIDEO_REG_STR AutoNoVidDet;

	AutoNoVidDet.Ch = ch%4;
	AutoNoVidDet.devnum = devnum;
	NC_VD_AUTO_NOVIDEO_Set(&AutoNoVidDet);
};

void NC_VD_AUTO_AutoMode_Set(unsigned char ch, unsigned char DevNum)
{
	unsigned char oChannel = ch%4;
	unsigned char oDevNum = DevNum;
	NC_VD_VI_AUTO_DETECT_REG_STR VinAutoDet;

	VinAutoDet.Ch = oChannel;
	VinAutoDet.DevNum = oDevNum;

	NC_VD_AUTO_Auto_Detection_Set(&VinAutoDet);
}
//*******************************************************************************//
//
//				Verification Code
//
//
//*******************************************************************************//
void NC_VD_AUTO_ACC_GAIN_Get(unsigned char Ch, ACC_DEBUG FuncSel)
{
	NC_VD_AUTO_ACC_GAIN_STR ACC;



	ACC.Ch = Ch % 4;
	ACC.devnum = Ch / 4;
	ACC.FuncSel = FuncSel;

	// DEBUG
	NC_VD_AUTO_ACC_GAIN_Read(&ACC);
	sprintf(DebugMsgACCGainStr[Ch],"SAM ACC : %3x",ACC.AccGain);

	if(FuncSel == ACC_GAIN_NORMAL)		// B5 0xE2 [10:8] 0xE3[7:0] ACC Gain Register
		DBG_INFO("CH:[%d] ACC GAIN : %3x\n", Ch, ACC.AccGain);
	else if(FuncSel == ACC_GAIN_DEBUG)	// B13 0xC6 [9:8] 0xC7[7:0] ACC Gain Debug Register
		DBG_INFO("CH:[%d] SAM ACC GAIN : %3x\n", Ch, ACC.AccGain);
	else
		DBG_INFO("CH:[%d] ACC GAIN : %3x\n", Ch, ACC.AccGain);
}

void NC_VD_AUTO_FSC_Get (unsigned char Ch, NC_VD_AUTO_FSC_STR* pFSC)
{
	NC_VD_VI_FSC_VAL_Read(pFSC);

	sprintf(DebugMsgFSCVal1Str[Ch],"%08x", pFSC->FSC_Val1);
	sprintf(DebugMsgFSCVal2Str[Ch],"%08x", pFSC->FSC_Val2);
	sprintf(DebugMsgFSCFinalStr[Ch],"%08x", pFSC->FSC_Final);
}

NC_VIVO_CH_FORMATDEF NC_VD_AUTO_VFCtoFMTDEF(unsigned char VFC)
{
	return arrVfcType[VFC];
}



