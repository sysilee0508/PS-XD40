//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "NVP6158.h"
#include "common.h"
#include "display_mode.h"

extern NC_VIVO_CH_FORMATDEF NVP6158_Current_Video_Format_Check(unsigned char oLogicalChannel);

//=============================================================================
//  Define & MACRO
//=============================================================================

//=============================================================================
//  Static Variable Declaration
//=============================================================================
//static eDisplayMode_t systemDisplayMode = DISPLAY_MODE_SPLIT;
//static eChannel_t systemDisplayChannel = CHANNEL_SPLIT;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================


static BYTE Check_VideoFormat_Change(void)
{
	eChannel_t channel;
	BYTE oCurVideofmt;
	static BYTE oPreVideofmt[NUM_OF_CHANNEL] = {0,};
	BYTE videoFormatChanged;

	for(channel = CHANNEL1; channel <NUM_OF_CHANNEL; channel++)
	{
		oCurVideofmt = NVP6158_Current_Video_Format_Check(channel);
		if(oCurVideofmt != oPreVideofmt[channel])
		{
			oPreVideofmt[channel] = oCurVideofmt;
			videoFormatChanged = TRUE;
		}
	}

	return videoFormatChanged;	
}

void Set_DisplayoutputMode_table(void)
{
//	eDisplayMode_t displayMode = Get_SystemDisplayMode();

//	if(Check_VideoFormat_Change() == TRUE)
//	{
//		if(displayMode == DISPLAY_MODE_FULL)
//		{
//			Display_FullMode(Get_SystemDisplayChannel());
//		}
//		else
//		{
//			Display_SplitMode(Get_SystemSplitMode());
//		}
//		Delay_ms(500);
//	}
}

void DisplayMode_FullScreen(eChannel_t ch)
{
}

//void DisplayMode_SplitScreen(eSplitMode_t splitMode)
//{
//}

eInputVideoMode_t Get_InputVideoMode(eChannel_t channel)
{
	eInputVideoMode_t videoMode;

	switch(NVP6158_Current_Video_Format_Check(channel))
	{
		case AHD20_1080P_60P:
		case AHD20_1080P_30P:
		case TVI_FHD_30P:
			videoMode = INPUT_VIDEO_1080P30;
			break;

		case AHD20_1080P_50P:
		case AHD20_1080P_25P:
		case TVI_FHD_25P:
			videoMode = INPUT_VIDEO_1080P25;
			break;

		case AHD20_720P_60P:
		case AHD20_720P_30P:
		case AHD20_720P_30P_EX:
		case AHD20_720P_30P_EX_Btype:
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
			videoMode = INPUT_VIDEO_720P30;
			break;

		case AHD20_720P_50P:
		case AHD20_720P_25P:
		case AHD20_720P_25P_EX:
		case AHD20_720P_25P_EX_Btype:
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
			videoMode = INPUT_VIDEO_720P25;
			break;

		case AHD20_SD_H960_NT:
		case AHD20_SD_SH720_NT:
		case AHD20_SD_H1280_NT:
		case AHD20_SD_H1440_NT:
		case AHD20_SD_H960_EX_NT:
		case AHD20_SD_H960_2EX_NT:
		case AHD20_SD_H960_2EX_Btype_NT:
			videoMode = INPUT_VIDEO_CVBS_NTSC;
			break;
			
		case AHD20_SD_H960_PAL:
		case AHD20_SD_SH720_PAL:
		case AHD20_SD_H1280_PAL:
		case AHD20_SD_H1440_PAL:
		case AHD20_SD_H960_EX_PAL:
		case AHD20_SD_H960_2EX_PAL:
		case AHD20_SD_H960_2EX_Btype_PAL:
			videoMode = INPUT_VIDEO_CVBS_PAL;
			break;

		default:
			videoMode = INPUT_VIDEO_MAX;
			break;
	}
	return videoMode;
}
