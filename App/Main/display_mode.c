//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
#include "NVP6168.h"
#endif

//extern NC_VIVO_CH_FORMATDEF NVP6158_Current_Video_Format_Check(unsigned char oLogicalChannel);

//=============================================================================
//  Define & MACRO
//=============================================================================

//=============================================================================
//  Static Variable Declaration
//=============================================================================

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================
static BYTE Get_CurrentVideoFormat(eChannel_t channel)
{
#if BD_NVP == NVP6158
	return NVP6158_Current_Video_Format_Check(channel);
#elif BD_NVP == NVP6168
	return NVP6168_Current_VideoFormat_Get(channel);
#endif
}

static BOOL Check_VideoFormat_Change(void)
{
	eChannel_t channel;
	static BYTE oPreVideofmt[NUM_OF_CHANNEL] = {0,};
	BYTE videoFormatChanged = FALSE;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();

	if(displayMode == DISPLAY_MODE_FULL_CH1)
	{
		if(Get_CurrentVideoFormat(CHANNEL1) != oPreVideofmt[CHANNEL1])
		{
			oPreVideofmt[CHANNEL1] = Get_CurrentVideoFormat(CHANNEL1);
			videoFormatChanged = TRUE;
		}
	}
	else if(displayMode == DISPLAY_MODE_FULL_CH2)
	{
		if(Get_CurrentVideoFormat(CHANNEL2) != oPreVideofmt[CHANNEL2])
		{
			oPreVideofmt[CHANNEL2] = Get_CurrentVideoFormat(CHANNEL2);
			videoFormatChanged = TRUE;
		}
	}
	else
	{
		for(channel = CHANNEL1; channel <NUM_OF_CHANNEL; channel++)
		{
			if(Get_CurrentVideoFormat(channel) != oPreVideofmt[channel])
			{
				oPreVideofmt[channel] = Get_CurrentVideoFormat(channel);
				videoFormatChanged = TRUE;
				break;
			}
		}
	}

	return videoFormatChanged;	
}

void DisplayScreen(eDisplayMode_t mode)
{
	if((mode == DISPLAY_MODE_FULL_CH1) || (mode == DISPLAY_MODE_FULL_CH2))
	{
		SetInputSource(VIDEO_DIGITAL_NVP6158_A);
	}
	else
	{
		SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
	}
	Write_NvItem_DisplayMode(mode);
}

void UpdateDisplayMode(void)
{
	if(Check_VideoFormat_Change() == TRUE)
	{
		OSD_RefreshScreen();
		SetInputChanged();
		InitInputSource();
		DisplayScreen(GetCurrentDisplayMode());
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
	Get_CurrentVideoFormat(channel);
}
