//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "NVP6158.h"
#include "common.h"

extern NC_VIVO_CH_FORMATDEF NVP6158_Current_Video_Format_Check(unsigned char oLogicalChannel);

//=============================================================================
//  Define & MACRO
//=============================================================================
#define UpdatedChannel(a,b)				(a & (0x01 << b))?TRUE:FALSE
//=============================================================================
//  Static Variable Declaration
//=============================================================================
eDisplayMode_t prevDisplayMode = DISPLAY_MODE_MAX;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================

static BOOL Check_VideoFormat_Change(void)
{
	eChannel_t channel;
	static BYTE oPreVideofmt[NUM_OF_CHANNEL] = {0xFF, 0xFF, 0xFF, 0xFF};
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

			default:
				changed = (changedChannel != 0x00)?TRUE:FALSE;
				break;

		}
	}

	return changed;
}


void DisplayScreen(eDisplayMode_t mode)
{
	prevDisplayMode = GetCurrentDisplayMode();
	switch(mode)
	{
		case DISPLAY_MODE_FULL_CH1:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;

		case DISPLAY_MODE_FULL_CH2:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;

		case DISPLAY_MODE_FULL_CH3:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_B);
			break;

		case DISPLAY_MODE_FULL_CH4:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_B);
			break;
		
		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_D2:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_B);
			break;

		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
		case DISPLAY_MODE_4SPLIT_H:
		case DISPLAY_MODE_4SPLIT_X:
			NVP6158_Set_VportMap(VPORT_MAP0);
			SetInputSource(VIDEO_DIGITAL_NVP6158_AB);
			break;

		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_D3:
			NVP6158_Set_VportMap(VPORT_MAP4);
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;

		case DISPLAY_MODE_PIP_A4:
		case DISPLAY_MODE_PIP_B4:
		case DISPLAY_MODE_PIP_C4:
		case DISPLAY_MODE_PIP_D4:
			NVP6158_Set_VportMap(VPORT_MAP5);
			SetInputSource(VIDEO_DIGITAL_NVP6158_A);
			break;

		case DISPLAY_MODE_4SPLIT_QUAD:
			NVP6158_Set_VportMap(VPORT_MAP4);
			SetInputSource(VIDEO_DIGITAL_NVP6158_AB);
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

eDisplayMode_t GetPrevDisplayMode(void)
{
	return prevDisplayMode;
}

eDisplayMode_t GetCurrentDisplayMode(void)
{
	eDisplayMode_t displayMode;
	Read_NvItem_DisplayMode(&displayMode);
	return displayMode;
}

eDisplayMode_t GetSystemSplitMode(void)
{
	eDisplayMode_t split;

	Read_NvItem_SplitMode(&split);
	if((split < DISPLAY_MODE_2SPLIT_HSCALE_A) || (split >= DISPLAY_MODE_MAX))
	{
		split = DISPLAY_MODE_2SPLIT_HSCALE_A;
		Write_NvItem_SplitMode(split);
	}
	
	return split;
}

void SetSystemSplitMode(eDisplayMode_t split)
{
	Write_NvItem_SplitMode(split);
}

BYTE GetInputVideoFormat(eChannel_t channel)
{
	return NVP6158_Current_Video_Format_Check(channel);
}

// return main channel for split or pip mode
eChannel_t ConvertDisplayMode2Channel(eDisplayMode_t displayMode)
{
	eChannel_t channel;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH2:
			channel = CHANNEL2;
			break;

		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			channel = CHANNEL3;
			break;

		case DISPLAY_MODE_FULL_CH4:
			channel = CHANNEL4;
			break;

		default:
			channel = CHANNEL1;
			break;
	}

	return channel;
}

eChannel_t FindMainChannel(eDisplayMode_t displayMode, MDIN_CHIP_ID_t mdin)
{
	eChannel_t main= CHANNEL1;

	if(mdin == MDIN_ID_A)
	{
		if(displayMode == DISPLAY_MODE_FULL_CH2)
		{
			main = CHANNEL2;
		}
	}
	else if(mdin == MDIN_ID_B)
	{
		if(displayMode == DISPLAY_MODE_FULL_CH4)
		{
			main = CHANNEL4;
		}
		else if(displayMode == DISPLAY_MODE_4SPLIT_QUAD)
		{
			main = CHANNEL2;
		}
		else
		{
			main = CHANNEL3;
		}
	}

	return main;
}

eChannel_t FindAuxChannel(eDisplayMode_t displayMode, MDIN_CHIP_ID_t mdin)
{
	eChannel_t aux = CHANNEL4;

	if(mdin == MDIN_ID_A)
	{
		if((displayMode == DISPLAY_MODE_PIP_A3) || (displayMode == DISPLAY_MODE_PIP_B3) ||
			(displayMode == DISPLAY_MODE_PIP_C3) || (displayMode == DISPLAY_MODE_PIP_D3) ||
			(displayMode == DISPLAY_MODE_4SPLIT_QUAD))
		{
			aux = CHANNEL3;
		}
		else if((displayMode == DISPLAY_MODE_PIP_A4) || (displayMode == DISPLAY_MODE_PIP_B4) ||
			(displayMode == DISPLAY_MODE_PIP_C4) || (displayMode == DISPLAY_MODE_PIP_D4))
		{
			aux = CHANNEL4;
		}
		else
		{
			aux = CHANNEL2;
		}
	}
	
	return aux;
}

BOOL IsCroppingMode(eDisplayMode_t mode)
{
	BOOL cropping = FALSE;

	if((mode == DISPLAY_MODE_2SPLIT_HCROP_A) || (mode == DISPLAY_MODE_2SPLIT_VCROP_A) ||
		(mode == DISPLAY_MODE_2SPLIT_HCROP_B) || (mode == DISPLAY_MODE_2SPLIT_VCROP_B) ||
		(mode == DISPLAY_MODE_4SPLIT_R3CROP) || (mode == DISPLAY_MODE_4SPLIT_L3CROP) ||
		(mode == DISPLAY_MODE_4SPLIT_D3CROP) || (mode == DISPLAY_MODE_4SPLIT_U3CROP))
	{
		cropping = TRUE;
	}

	return cropping;
}

eCroppingDirection_t GetCroppingDirection(eDisplayMode_t mode)
{
	eCroppingDirection_t dir = CROPPING_NONE;

	if((mode == DISPLAY_MODE_2SPLIT_HCROP_A) || (mode == DISPLAY_MODE_2SPLIT_HCROP_B) ||
		(mode == DISPLAY_MODE_4SPLIT_R3CROP) || (mode == DISPLAY_MODE_4SPLIT_L3CROP))
	{
		dir = CROPPING_H;
	}
	else if((mode == DISPLAY_MODE_2SPLIT_VCROP_A) || (mode == DISPLAY_MODE_2SPLIT_VCROP_B) ||
			(mode == DISPLAY_MODE_4SPLIT_D3CROP) || (mode == DISPLAY_MODE_4SPLIT_U3CROP))
	{
		dir = CROPPING_V;
	}

	return dir;
}

#if 0
void GetCroppingOffsetValue(void)
{
//	eDisplayMode_t displayMode = GetCurrentDisplayMode();
//	eChannel_t channel;
	sCroppingOffset_t croppingOffset;

	Nv_

	switch(displayMode)
	{
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
			
			break;
	}

	if( TRUE == IsCroppingMode(displayMode))
	{
		channel = ConvertDisplayMode2Channel(displayMode);

		
	}
}
#endif
