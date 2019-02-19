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
	if(mode == DISPLAY_MODE_FULL_CH1)
	{
		SetInputSource(VIDEO_DIGITAL_NVP6158_A);
	}
	else if(mode == DISPLAY_MODE_FULL_CH1)
	{
		SetInputSource(VIDEO_DIGITAL_NVP6158_B);
	}
	else
	{
		SetInputSource(VIDEO_DIGITAL_NVP6158_2CH);
	}
	//Set_DisplayWindow(mode);
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
