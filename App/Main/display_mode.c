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
#define UpdatedChannel(a,b)				(a & (0x01 << b))?TRUE:FALSE
//=============================================================================
//  Static Variable Declaration
//=============================================================================
//static eDisplayMode_t systemDisplayMode = DISPLAY_MODE_FULL_CH1;
//static eChannel_t systemDisplayChannel = CHANNEL1;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================

static BOOL Check_VideoFormat_Change(void)
{
	eChannel_t channel;
	static BYTE oPreVideofmt[NUM_OF_CHANNEL] = {0,};
	BYTE changedChannel = 0x00;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	BOOL changed = FALSE;

	for(channel = CHANNEL1; channel <NUM_OF_CHANNEL; channel++)
	{
		if(NVP6158_Current_Video_Format_Check(channel) != oPreVideofmt[channel])
		{
			changedChannel |= (0x01 << channel);
			oPreVideofmt[channel] = NVP6158_Current_Video_Format_Check(channel);
		}
	}

	// any channel is changed
	if(changedChannel != 0x00)
	{
		switch(displayMode)
		{
			case DISPLAY_MODE_FULL_CH1:
				changed = UpdatedChannel(changedChannel, CHANNEL1);
				break;

			case DISPLAY_MODE_FULL_CH2:
				changed = UpdatedChannel(changedChannel, CHANNEL2);
				break;

			case DISPLAY_MODE_FULL_CH3:
				changed = UpdatedChannel(changedChannel, CHANNEL3);
				break;

			case DISPLAY_MODE_FULL_CH4:
				changed = UpdatedChannel(changedChannel, CHANNEL4);
				break;

			case DISPLAY_MODE_2SPLIT_HSCALE_A:
			case DISPLAY_MODE_2SPLIT_HCROP_A:
			case DISPLAY_MODE_2SPLIT_VSCALE_A:
			case DISPLAY_MODE_2SPLIT_VCROP_A:
			case DISPLAY_MODE_PIP_A2:
			case DISPLAY_MODE_PIP_B2:
			case DISPLAY_MODE_PIP_C2:
			case DISPLAY_MODE_PIP_D2:
				changed = UpdatedChannel(changedChannel, CHANNEL1) | UpdatedChannel(changedChannel, CHANNEL2);
				break;

			case DISPLAY_MODE_2SPLIT_HSCALE_B:
			case DISPLAY_MODE_2SPLIT_HCROP_B:
			case DISPLAY_MODE_2SPLIT_VSCALE_B:
			case DISPLAY_MODE_2SPLIT_VCROP_B:
				changed = UpdatedChannel(changedChannel, CHANNEL3) | UpdatedChannel(changedChannel, CHANNEL4);
				break;

			case DISPLAY_MODE_4SPLIT_QUAD:
			case DISPLAY_MODE_4SPLIT_R3SCALE:
			case DISPLAY_MODE_4SPLIT_R3CROP:
			case DISPLAY_MODE_4SPLIT_L3SCALE:
			case DISPLAY_MODE_4SPLIT_L3CROP:
			case DISPLAY_MODE_4SPLIT_D3SCALE:
			case DISPLAY_MODE_4SPLIT_D3CROP:
			case DISPLAY_MODE_4SPLIT_U3SCALE:
			case DISPLAY_MODE_4SPLIT_U3CROP:
				changed = UpdatedChannel(changedChannel, CHANNEL1) | 		\
							UpdatedChannel(changedChannel, CHANNEL2) |		\
							UpdatedChannel(changedChannel, CHANNEL3) | 		\
							UpdatedChannel(changedChannel, CHANNEL4);
				break;

			case DISPLAY_MODE_PIP_A3:
			case DISPLAY_MODE_PIP_B3:
			case DISPLAY_MODE_PIP_C3:
			case DISPLAY_MODE_PIP_D3:
				changed = UpdatedChannel(changedChannel, CHANNEL1) | UpdatedChannel(changedChannel, CHANNEL3);
				break;

			case DISPLAY_MODE_PIP_A4:
			case DISPLAY_MODE_PIP_B4:
			case DISPLAY_MODE_PIP_C4:
			case DISPLAY_MODE_PIP_D4:
				changed = UpdatedChannel(changedChannel, CHANNEL1) | UpdatedChannel(changedChannel, CHANNEL4);
				break;

		}
	}

	return changed;
}


void DisplayScreen(eDisplayMode_t mode)
{
	switch(mode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;

		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			SetInputSource(VIDEO_DIGITAL_NVP6158_C);
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_D2:
			SetInputSource(VIDEO_DIGITAL_NVP6158_AB);
			break;

		case DISPLAY_MODE_4SPLIT_QUAD:
		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
			SetInputSource(VIDEO_DIGITAL_NVP6158_ABCD);
			break;

		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_D3:
			SetInputSource(VIDEO_DIGITAL_NVP6158_AC);
			break;

		case DISPLAY_MODE_PIP_A4:
		case DISPLAY_MODE_PIP_B4:
		case DISPLAY_MODE_PIP_C4:
		case DISPLAY_MODE_PIP_D4:
			SetInputSource(VIDEO_DIGITAL_NVP6158_AD);
			break;
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
	return NVP6158_Current_Video_Format_Check(channel);
}
