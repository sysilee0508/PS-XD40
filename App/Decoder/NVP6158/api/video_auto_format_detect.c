/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Auto format detection
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>

#include "video_auto_detect.h"
#include "video_fmt_info.h"
#include <raptor3_ioctl.h>
#include "video_output.h"
#include "video_input.h"
#include <raptor3_fmt.h>
#include <video_decoder.h>
#include "clock.h"
#include "_debug_macro.h"
#include "raptor3_api.h"

#include "hi_comm_vi.h"

extern char DebugMsgStr[4][256];
extern char DebugMsgNoVidStr[4][256];

unsigned char onvideo_check_cnt[4] = {0,};

/**************************************************************************************
* @desc
* 	A function that confirms that there is no video input signal.
*
* @param_in		(NC_VD_AUTO_VFC_STR *)pVFC				A structure parameter to check if the VFC value read from the decoder is correct.
* @param_in		(NC_VD_AUTO_DETECT_FLAG_STR *)pFlag		A structure parameter that stores necessary flags while Auto Format Detection is in progress.
*
* @return   	1		       							On Video Status
* @return   	0		       							Unknown Status
***************************************************************************************/
unsigned char NC_APP_VD_AUTO_ONVIDEO_Check(NC_VD_AUTO_VFC_STR *pVFC, NC_VD_AUTO_DETECT_FLAG_STR *pFlag)
{
	unsigned char rst;
	NC_VD_AUTO_ONVIDEO_CHECK_STR SW_Rst;

	SW_Rst.Info.Ch =  pVFC->Ch;
	SW_Rst.Info.Dev_Num =  0;
	SW_Rst.Info.Fmt_Def =  0;

	SW_Rst.VFC = pVFC->VFC;
	SW_Rst.SW_Rst = 0;

	/*
	if(onvideo_check_cnt[pVFC->Ch] < 1)
	{

	rst = NC_VD_AUTO_ONVIDEO_SW_RST_CHECK(&SW_Rst);
	if(rst)
	{
			DBG_INFO("[ch:%d]>>>>>>>>>>>>>>>>>>>>>>>> SWRST!!!!!!",SW_Rst.Info.Ch);
		NC_VD_AUTO_ONVIDEO_SW_RST(&SW_Rst.Info);
			onvideo_check_cnt[pVFC->Ch] = 1;
			DBG_INFO("[ch:%d] 0xF0 [%x]", SW_Rst.Info.Ch, SW_Rst.VFC );
		return 0;
	}
	else
	{
	return ((((((pVFC->VFC >> 4 ) & 0xF) != 0xF) && ((pVFC->VFC & 0x0F) != 0xF))
			&& pFlag->AutoDetect_Flag)
			&& (pFlag->NoVideo_Flag == FLAG_ON_VIDEO));
	}
	}
	else
	{
		return ((((((pVFC->VFC >> 4 ) & 0xF) != 0xF) && ((pVFC->VFC & 0x0F) != 0xF))
					&& pFlag->AutoDetect_Flag)
					&& (pFlag->NoVideo_Flag == FLAG_ON_VIDEO));
	}
*/

}


/**************************************************************************************
* @desc
* 	A function that confirms that there is no video input signal.
*
* @param_in		(NC_VD_AUTO_DETECT_FLAG_STR *)pFlag		A structure parameter that stores necessary flags while Auto Format Detection is in progress.
*
* @return   	1		       							No Video Status
* @return   	0		       							Unknown Status
***************************************************************************************/
unsigned char NC_APP_VD_AUTO_NOVIDEO_Check(NC_VD_AUTO_DETECT_FLAG_STR *pFlag)
{
	return ((pFlag->AutoDetect_Flag == OFF) && (pFlag->NoVideo_Flag == FLAG_NO_VIDEO));
	//return  (pFlag->NoVideo_Flag == FLAG_NO_VIDEO);
}

unsigned char s_Pre_VFC[4] = {0xFF, };
unsigned char s_NoVideo_SetFlag[4] = {0, };
unsigned char s_OnVideo_SetFlag[4] = {0, };
/**************************************************************************************
* @desc
* 	Function to obtain VFC value during Auto Detection process.
*
* @param_in		(NC_VD_AUTO_DETECT_FLAG_STR *)pFlag		Parameter structure for an ongoing process that gets the value of VFC.
* @param_out	(NC_VD_AUTO_VFC_STR *)pVFC				A structure parameter that stores the VFC value read from RAPTOR3.
*
* @return   	void		       						None
***************************************************************************************/
void NC_APP_VD_AUTO_VFC_Get(NC_VD_AUTO_VFC_STR* pVFC, NC_VD_AUTO_DETECT_FLAG_STR* pFlag)
{
#if !NO_POWER_OFF_CAMERA_TEST
	if(pFlag->AutoDetect_Flag)				// AutoDetectFlag 상태 변화는 NC_VD_VI_AT_NOVIDEO_Set 부분에서 변경
	{
		NC_VD_AUTO_VFC_Get(pVFC);	// 채널에 해당하는 VFC (Video Format Classified) Value Read

		if(arrVfcName[pVFC->VFC] == 0)
			sprintf(DebugMsgStr[pVFC->Ch],"UNKNOWN");
		else
		{
			sprintf(DebugMsgStr[pVFC->Ch],"%s", arrVfcName[pVFC->VFC]);
		}
	}
	else
	{


	}

#else
	unsigned char VFC_History[10];
	NC_VD_AUTO_NOVIDEO_STR NoVideo;
	NoVideo.Ch = pVFC->Ch;
	NoVideo.Dev_Num = 0;
	NoVideo.NoVid = 0;
	unsigned char History_Cnt = 0;
	unsigned char Check_Cnt = 0;
	unsigned char NoVideo_Flag;
	unsigned char Unstable_Cnt = 0;

	while(History_Cnt < 10)
	{
		usleep(1000);

		NC_VD_AUTO_VFC_Get(pVFC);	// 채널에 해당하는 VFC (Video Format Classified) Value Read
		pFlag->NoVideo_Flag = NC_APP_VD_AUTO_NOVIDEO_Get(pVFC->Ch, &NoVideo);
		VFC_History[History_Cnt] = pVFC->VFC;

		if(arrVfcName[pVFC->VFC] == 0) sprintf(DebugMsgStr[pVFC->Ch],"UNKNOWN");
		else sprintf(DebugMsgStr[pVFC->Ch],"%s", arrVfcName[pVFC->VFC]);

		if(History_Cnt >= 1)
		{
			// Previous VFC Current VFC Same?
			if(VFC_History[History_Cnt-1] == VFC_History[History_Cnt]) Check_Cnt += 1; // YES
			else
			{
				Check_Cnt = 0; // NO
				Unstable_Cnt++;
			}

			if((Check_Cnt > 5))
			{
				pFlag->AutoDetect_Flag = ON;
				if((s_Pre_VFC[pVFC->Ch] != pVFC->VFC) && (pVFC->VFC != 0xFF))
				{
					DBG_INFO("CH:[%d] Auto Format Detection Success ... [%s]", pVFC->Ch, arrVfcName[pVFC->VFC]);
					s_Pre_VFC[pVFC->Ch] = pVFC->VFC;
					s_NoVideo_SetFlag[pVFC->Ch] = 0;
					return;
				}
		else
		{
					//DBG_INFO("CH:[%d] Previous VFC & Current VFC Same!!!", pVFC->Ch);
					pFlag->NoVideo_Flag = FLAG_NO_VIDEO;
					return;
				}
			}
			else if(Unstable_Cnt > 3)
			{
				DBG_ERR("CH:[%d] Unstable_Cnt [%d]Times Over .. Forces NoVideo Set", pVFC->Ch, Unstable_Cnt);
				s_Pre_VFC[pVFC->Ch] = 0xFF; // On-V.deo -> No Video
				pFlag->AutoDetect_Flag = OFF;
				pFlag->NoVideo_Flag = FLAG_NO_VIDEO;
				s_NoVideo_SetFlag[pVFC->Ch] = 1;
				return;
		}
	}

		//if((pFlag->NoVideo_Flag == FLAG_NO_VIDEO) && (pVFC->VFC == 0xFF))	// NoVideo Status
		if((pFlag->NoVideo_Flag == FLAG_NO_VIDEO))	// NoVideo Status
		{
		//	NC_VD_VO_Auto_Data_Mode_Set(pVFC->Ch, OUT_MODE_OFF);	// Video Out Display On
			if(s_NoVideo_SetFlag[pVFC->Ch])
			{
				pFlag->AutoDetect_Flag = ON; // Previous No Video Status and Current No Video
				return;
			}
	else
	{
				s_Pre_VFC[pVFC->Ch] = 0xFF; // On-V.deo -> No Video
				pFlag->AutoDetect_Flag = OFF;
			}
			s_NoVideo_SetFlag[pVFC->Ch] = 1;
			return;
		}
		else History_Cnt++;
	}

	DBG_ERR("CH:[%d] Auto Format Detection Error ... ", pVFC->Ch);
	pFlag->AutoDetect_Flag = ON;
	pFlag->NoVideo_Flag = FLAG_NO_VIDEO;
	return;
#endif
}

/**************************************************************************************
* @desc
* 	During the Auto Format Detection process, the default video format setting(CVI HD 30P EX) of the decoder is set and the Flag value is updated when the video is in the no video state.
*
* @param_in		(unsigned char)Ch										Video Channel
* @param_out	(RAPTOR3_FMT_DETECT_RESULT_S *)pDetectResult	Structure in which the default format (CVI HD 30P EX) setting value is saved when in no video state.
* @param_out	(NC_VD_AUTO_DETECT_FLAG_STR *)pFlag				A structure parameter to update the Flag when no video settings are complete.
*
* @return   	void		       								None
***************************************************************************************/
void NC_APP_VD_AUTO_NOVIDEO_Set(unsigned char Ch , RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult, NC_VD_AUTO_DETECT_FLAG_STR *pFlag )
{
	DBG_INFO("[%d CH] No Video Set\n", Ch);
	int port = Ch;

	unsigned char oChannel = Ch % 4;
	unsigned char oDevNum = Ch / 4;

	VDEC_CH_CFG_S ch_cfg[ RAPTOR3_MAX_CH_PER_CHIP ] = {0,};
	VDEC_PORT_CFG_S port_cfg[ RAPTOR3_MAX_PORT_PER_CHIP ] = {0,};

	ch_cfg[Ch].FmtDef = CVI_HD_30P;

	port_cfg[port].output_interface = VI_INPUT_MODE_BT656;
	port_cfg[port].mux_mode = VI_WORK_MODE_1Multiplex;
	port_cfg[port].edge = VI_CLK_EDGE_SINGLE_UP;
	port_cfg[port].SEQ[0] = Ch;
	port_cfg[port].SEQ[1] = Ch;
	port_cfg[port].SEQ[2] = Ch;
	port_cfg[port].SEQ[3] = Ch;
	//FIXME KWANHO Ch : Port 할당 테스트
	if(pDetectResult != 0)
	{
		pDetectResult->FormatChanged = 1;
		pDetectResult->FmtDef = ch_cfg[Ch].FmtDef;
		memcpy( &pDetectResult->ch_cfg, &ch_cfg[Ch], sizeof( VDEC_CH_CFG_S ) );
		memcpy( &pDetectResult->port_cfg, &port_cfg[port], sizeof( VDEC_PORT_CFG_S ) );
	}
	else
	{
		DBG_ERR("pDetectResult NULL!!");
	}

	NC_VD_AUTO_NoVideo_Set(oChannel, oDevNum);
	NC_VO_PORT_FMT_S *pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( ch_cfg[Ch].FmtDef); // TODO : unnecessary arguments
	NC_VD_VO_Mode_Set( oDevNum, port, pPortFmt, port_cfg[port].mux_mode, port_cfg[port].output_interface,
			port_cfg[port].edge, port_cfg[port].SEQ[0], port_cfg[port].SEQ[1], port_cfg[port].SEQ[2], port_cfg[port].SEQ[3]);

	_PRINT_NOVIDEO_INFO_2_GUI(Ch);

#if !NO_POWER_OFF_CAMERA_TEST
	 pFlag->AutoDetect_Flag = ON;
#endif
}

/**************************************************************************************
* @desc
* 	Function to set the setting value of Detected Format when Auto Format Detection is done properly.
*
* @param_in		(unsigned char)Ch										Video Channel
* @param_out	(RAPTOR3_FMT_DETECT_RESULT_S *)pDetectResult	A Structure Parameter for storing the setting value of Detected Format.
* @param_in		(NC_VD_AUTO_VFC_STR *)pVFC						Structure parameters for Decoder and SoC Setting according to VFC value(Format).
*
* @return   	void		       								None
***************************************************************************************/
void NC_APP_VD_AUTO_ONVIDEO_Set(unsigned char Ch, RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult , NC_VD_AUTO_VFC_STR *pVFC)
{
	DBG_INFO("CH:[%d] On Video Set\n", Ch);
	int port = Ch;
	int devnum = Ch / 4;

	VDEC_CH_CFG_S ch_cfg[ RAPTOR3_MAX_CH_PER_CHIP ] = {0,};
	VDEC_PORT_CFG_S port_cfg[ RAPTOR3_MAX_PORT_PER_CHIP ] = {0,};
	NC_VIVO_CH_FORMATDEF FmtDef = arrVfcType[pVFC->VFC];

	/* (Only 1Mux)
	 * Ch  0 ~ 3  : PortA
	 * Ch  4 ~ 7  : PortB
	 * Ch  8 ~ 11 : PortC
	 * Ch 12 ~ 15 : PortD
	 */

	if(FmtDef == AHD30_4M_30P || \
			FmtDef == AHD30_4M_25P || \
			FmtDef == AHD30_3M_30P || \
			FmtDef == AHD30_3M_25P || \
			FmtDef == AHD30_5M_20P || \
			FmtDef == AHD30_5_3M_20P || \
			FmtDef == AHD30_6M_18P || \
			FmtDef == AHD30_6M_20P || \
			FmtDef == AHD30_8M_15P || \
			FmtDef == AHD30_8M_12_5P || \
			FmtDef == CVI_4M_25P || \
			FmtDef == CVI_4M_30P || \
			FmtDef == CVI_8M_12_5P || \
			FmtDef == CVI_8M_15P || \
			FmtDef == TVI_4M_25P || \
			FmtDef == TVI_4M_30P
			)
		port_cfg[port].edge = VI_CLK_EDGE_DOUBLE;
	else
		port_cfg[port].edge = VI_CLK_EDGE_SINGLE_UP;

	port_cfg[port].output_interface = VI_MODE_BT656;
	port_cfg[port].mux_mode = VI_WORK_MODE_1Multiplex;

	port_cfg[port].SEQ[CH1] = Ch;
	port_cfg[port].SEQ[CH2] = Ch;
	port_cfg[port].SEQ[CH3] = Ch;
	port_cfg[port].SEQ[CH4] = Ch;

	ch_cfg[Ch].FmtDef = FmtDef;

	if(pDetectResult != 0)
	{
		pDetectResult->FormatChanged = 1;
		pDetectResult->FmtDef = FmtDef;
		memcpy( &pDetectResult->ch_cfg, &ch_cfg[Ch], sizeof( VDEC_CH_CFG_S ) );
		memcpy( &pDetectResult->port_cfg, &port_cfg[port], sizeof( VDEC_PORT_CFG_S ) );
	}
	else
	{
		DBG_ERR("pDetectResult is NULL !!");
	}

	VDEC_PORT_CFG_S *pPortCfg = &port_cfg[port];
	VDEC_CH_CFG_S *pChCfg = &ch_cfg[Ch];
	NC_VO_PORT_FMT_S *pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( pChCfg->FmtDef ); // TODO : unnecessary arguments

	NC_VI_CH_FMT_S *pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( pChCfg->FmtDef );

	NC_VD_VI_H_TIMING_STR VinHTiming;
	NC_VD_VI_FB_YDELAY_STR VinYDelay;
	NC_VD_VI_FB_CDELAY_STR VinCDelay;
	// SW Reset Func ++++++
	NC_VD_DEV_CH_INFO_STR DEV_CH_INFO;
	DEV_CH_INFO.Ch = Ch;
	DEV_CH_INFO.Dev_Num = 0;
	DEV_CH_INFO.Fmt_Def = NC_HI_VI_FindFormatDef(pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );
	NC_VD_AUTO_ONVIDEO_SET(&DEV_CH_INFO);


	NC_VD_VI_H_Timing_Set(&DEV_CH_INFO);

	NC_VD_VI_COLOR_Set(&DEV_CH_INFO);

	NC_VD_VI_VAFE_Init(&DEV_CH_INFO);

	NC_VD_VO_Mode_Set( devnum, port, pPortFmt, pPortCfg->mux_mode, pPortCfg->output_interface,
					pPortCfg->edge, pPortCfg->SEQ[0], pPortCfg->SEQ[1], pPortCfg->SEQ[2], pPortCfg->SEQ[3]);
}

/**************************************************************************************
* @desc
* 	A function for reading the no-video state value from the decoder.
*
* @param_in		(unsigned char) Ch								Video Channel
* @param_out	(NC_VD_AUTO_NOVIDEO_STR *)pNoVideo		A structure parameter to store the no-video state value read from the decoder.
*
* @return   	(AUTO_VIDEO_STATUS) 1					No Video Status
* @return   	0										Unknown video status(But it is not a no-video status)
***************************************************************************************/
AUTO_VIDEO_STATUS NC_APP_VD_AUTO_NOVIDEO_Get(unsigned char Ch,NC_VD_AUTO_NOVIDEO_STR *pNoVideo)
{
	NC_VD_AUTO_NOVIDEO_Get(pNoVideo);														// Video Status Read (OnVideo or NoVideo)

	sprintf(DebugMsgNoVidStr[Ch],"NoVideo : %x", pNoVideo->NoVid);				// GUI
	return pNoVideo->NoVid ?  FLAG_NO_VIDEO : FLAG_ON_VIDEO;
}


/********************************************
 *											*
 *											*
 *     Not Use Function ... 20170627		*
 *  										*
 *											*
 ********************************************/
