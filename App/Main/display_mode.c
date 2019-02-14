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
static eDisplayMode_t systemDisplayMode = DISPLAY_MODE_FULL_CH1;
static eChannel_t systemDisplayChannel = CHANNEL1;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================

//static eVideoResolution_t Get_Current_Video_Resolution_Each_Channel(eChannel_t ch)
//{
//	BYTE inputVideoFormat =0x00;
//	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;
//
//	inputVideoFormat = NVP6158_Current_Video_Format_Check(ch);
//	switch(inputVideoFormat)
//	{
//		case AHD20_1080P_60P:
//		case AHD20_1080P_50P:
//		case AHD20_1080P_30P:
//		case AHD20_1080P_25P:
//		case TVI_FHD_30P:
//		case TVI_FHD_25P:
//			oCurVideoRes = VIDEO_RESOLUTION_1080P;
//			break;
//
//		case AHD20_720P_60P:
//		case AHD20_720P_50P:
//		case AHD20_720P_30P:
//		case AHD20_720P_25P:
//		case AHD20_720P_30P_EX:
//		case AHD20_720P_25P_EX:
//		case AHD20_720P_30P_EX_Btype:
//		case AHD20_720P_25P_EX_Btype:
//		case TVI_HD_60P:
//		case TVI_HD_50P:
//		case TVI_HD_30P:
//		case TVI_HD_25P:
//		case TVI_HD_30P_EX:
//		case TVI_HD_25P_EX:
//			oCurVideoRes = VIDEO_RESOLUTION_720P;
//			break;
//
//		default:
//			break;
//	}
//
//	return oCurVideoRes;
//}


static void Display_FullMode(void)
{
	Set_DisplayWindow(WINDOW_FULL);
}

static void Display_SplitA(void)
{
	Set_DisplayWindow(WINDOW_SPLIT_V);
}

static void Display_SplitB(void)
{
	Set_DisplayWindow(WINDOW_SPLIT_V);
	//crop
}

static void Display_SplitC(void)
{
	Set_DisplayWindow(WINDOW_SPLIT_H);
}

static void Display_SplitD(void)
{
	Set_DisplayWindow(WINDOW_SPLIT_H);
}

static void Display_SplitE(void)
{
	Set_DisplayWindow(WINDOW_SPLIT_V);
}

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

void DisplayScreen(eDisplayMode_t mode)
{
	switch(mode)
	{
		case DISPLAY_MODE_FULL_CH1:
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;
		case DISPLAY_MODE_FULL_CH2:
			SetInputSource(VIDEO_DIGITAL_NVP6158_B);
			break;

		case DISPLAY_MODE_SPLIT_A:
			SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
			break;
		case DISPLAY_MODE_SPLIT_B:
			SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
			break;
		case DISPLAY_MODE_SPLIT_C:
			SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
			break;
		case DISPLAY_MODE_SPLIT_D:
			SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
			break;
		case DISPLAY_MODE_SPLIT_E:
			SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
			break;

		case DISPLAY_MODE_PIP_A:
		case DISPLAY_MODE_PIP_B:
		case DISPLAY_MODE_PIP_C:
		case DISPLAY_MODE_PIP_D:
			SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
			break;
	}

	Set_DisplayWindow(mode);
	Write_NvItem_DisplayMode(mode);
}

void UpdateDisplayMode(void)
{
	if(Check_VideoFormat_Change() == TRUE)
	{
		InitInputSource();
		DisplayScreen(GetCurrentDisplayMode());
		Delay_ms(500);
	}
}

eDisplayMode_t GetCurrentDisplayMode(void)
{
	eDisplayMode_t displayMode;
	Read_NvItem_DisplayMode(&displayMode);
	return displayMode;
}

BYTE GetInputVideoFormat(eChannel_t channel)
{
	return NVP6158_Current_Video_Format_Check(channel);
}
