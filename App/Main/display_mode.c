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
static eDisplayMode_t systemDisplayMode = DISPLAY_MODE_SPLIT;
static eChannel_t systemDisplayChannel = CHANNEL_SPLIT;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================

static eVideoResolution_t Get_Current_Video_Resolution_Each_Channel(eChannel_t ch)
{
	BYTE oCurVideofmt =0x00;
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;
	
	oCurVideofmt = NVP6158_Current_Video_Format_Check(ch);
	switch(oCurVideofmt)
	{
		case AHD20_1080P_60P:
		case AHD20_1080P_50P:
		case AHD20_1080P_30P:
		case AHD20_1080P_25P:
		case TVI_FHD_30P:
		case TVI_FHD_25P:
			oCurVideoRes = VIDEO_RESOLUTION_1080P;
			break;

		case AHD20_720P_60P:
		case AHD20_720P_50P:
		case AHD20_720P_30P:
		case AHD20_720P_25P:
		case AHD20_720P_30P_EX:
		case AHD20_720P_25P_EX:
		case AHD20_720P_30P_EX_Btype:
		case AHD20_720P_25P_EX_Btype:
		case TVI_HD_60P:
		case TVI_HD_50P:
		case TVI_HD_30P:
		case TVI_HD_25P:
		case TVI_HD_30P_EX:
		case TVI_HD_25P_EX:	
			oCurVideoRes = VIDEO_RESOLUTION_720P;
			break;

		default:
			break;
	}

	return oCurVideoRes;	
}

static void Display_FullMode(eChannel_t ch)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	switch(ch)
	{
		case CHANNEL1:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					//VS4210_Input1080P_Output1080P_Mode0_w0();
					break;
				case VIDEO_RESOLUTION_720P:
					//VS4210_Input720P_Output1080P_Mode0_w0();
					break;	
				default : 
					//VS4210_Input1080P_Output1080P_Mode0_w0();
					break;	
			}
			break;
			
		case CHANNEL2:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					//VS4210_Input1080P_Output1080P_Mode0_w1();
					break;
				case VIDEO_RESOLUTION_720P:
					//VS4210_Input720P_Output1080P_Mode0_w1();
					break;	
				default : 
					//VS4210_Input1080P_Output1080P_Mode0_w1();
				break;	
			}
			break;
			
		case CHANNEL3:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					//VS4210_Input1080P_Output1080P_Mode0_w2();
					break;
				case VIDEO_RESOLUTION_720P:
					//VS4210_Input720P_Output1080P_Mode0_w2();
					break;	
				default : 
					//VS4210_Input1080P_Output1080P_Mode0_w2();
				break;	
			}
			break;			

		case CHANNEL4:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					//VS4210_Input1080P_Output1080P_Mode0_w3();
					break;
				case VIDEO_RESOLUTION_720P:
					//VS4210_Input720P_Output1080P_Mode0_w3();
					break;	
				default : 
					//VS4210_Input1080P_Output1080P_Mode0_w3();
				break;	
			}
			break;			
	}
}

static void Display_2SplitMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode12_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode12_w0();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode12_w0();
			break;				
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode12_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode12_w1();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode12_w1();
			break;	
	}
}

static void Display_3SplitAMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode8_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode8_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode8_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode8_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode8_w1();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode8_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode8_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode8_w2();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode8_w2();
			break;
	}
}

static void Display_3SplitBMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode9_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode9_w0();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode9_w0();
			break;				
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode9_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode9_w1();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode9_w1();
			break;	
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode9_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode9_w2();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode9_w2();
			break;		
	}		
}

static void Display_3SplitCMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode10_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode10_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode10_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode10_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode10_w1();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode10_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode10_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode10_w2();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode10_w2();
			break;
	}
}

static void Display_3SplitDMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode11_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode11_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode11_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode11_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode11_w1();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode11_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode11_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode11_w2();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode11_w2();
			break;
	}
}

static void Display_QuadAMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode1_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode1_w0();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode1_w0();
			break;				
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode1_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode1_w1();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode1_w1();
			break;	
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode1_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode1_w2();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode1_w2();
			break;		
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode1_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode1_w3();
			break;	
		default : 
			//VS4210_Input1080P_Output1080P_Mode1_w3();
			break;		
	}		
}

static void Display_QuadBMode(void) // mode-6
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode6_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode6_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode6_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode6_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode6_w1();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode6_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode6_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode6_w2();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode6_w2();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode6_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode6_w3();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode6_w3();
			break;
	}
}

static void Display_QuadCMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode7_w0();//VS4210_Input1080P_Output1080P_Mode6_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode7_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode7_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode7_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode7_w1();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode7_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode7_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode7_w2();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode7_w2();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode7_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode7_w3();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode7_w3();
			break;
	}
}

static void Display_QuadDMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode4_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode4_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode4_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode4_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode4_w1();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode4_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode4_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode4_w2();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode4_w2();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode4_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode4_w3();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode4_w3();
			break;
	}
}

static void Display_QuadEMode(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode5_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4210_Input720P_Output1080P_Mode5_w0();
			break;
		default :
			//VS4210_Input1080P_Output1080P_Mode5_w0();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4210_Input1080P_Output1080P_Mode5_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4120_Input720P_Output1080P_Mode5_w1();
			break;
		default :
			//VS4120_Input1080P_Output1080P_Mode5_w1();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4120_Input1080P_Output1080P_Mode5_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4120_Input720P_Output1080P_Mode5_w2();
			break;
		default :
			//VS4120_Input1080P_Output1080P_Mode5_w2();
			break;
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			//VS4120_Input1080P_Output1080P_Mode5_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			//VS4120_Input720P_Output1080P_Mode5_w3();
			break;
		default :
			//VS4120_Input1080P_Output1080P_Mode5_w3();
			break;
	}
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

static void Display_SplitMode(eSplitMode_t splitMode)
{
	switch(splitMode)
	{
		case DISPLAY_MODE_QUAD_A:
			Display_QuadAMode();
			break;

		case DISPLAY_MODE_QUAD_B:
			Display_QuadBMode();
			break;

		case DISPLAY_MODE_QUAD_C:
			Display_QuadCMode();
			break;

		case DISPLAY_MODE_QUAD_D:
			Display_QuadDMode();
			break;

		case DISPLAY_MODE_QUAD_E:
			Display_QuadEMode();
			break;

		case DISPLAY_MODE_3SPLIT_A:
			Display_3SplitAMode();
			break;

		case DISPLAY_MODE_3SPLIT_B:
			Display_3SplitBMode();
			break;

		case DISPLAY_MODE_3SPLIT_C:
			Display_3SplitCMode();
			break;

		case DISPLAY_MODE_3SPLIT_D:
			Display_3SplitDMode();
			break;

		case DISPLAY_MODE_2SPLIT:
			Display_2SplitMode();
			break;

		default:
			Display_QuadAMode();
			break;
	}
}

void Set_DisplayoutputMode_table(void)
{
	eDisplayMode_t displayMode = Get_SystemDisplayMode();

	if(Check_VideoFormat_Change() == TRUE)
	{
		if(displayMode == DISPLAY_MODE_FULL)
		{
			Display_FullMode(Get_SystemDisplayChannel());
		}
		else
		{
			Display_SplitMode(Get_SystemSplitMode());
		}
		Delay_ms(500);
	}
}

void Set_SystemSplitMode(eSplitMode_t mode)
{
	Write_NvItem_SplitMode(mode);
}

eSplitMode_t Get_SystemSplitMode(void)
{
	eSplitMode_t splitMode;
	Read_NvItem_SplitMode(&splitMode);

	return splitMode;
}

void Set_SystemDisplayMode(eDisplayMode_t mode)
{
	systemDisplayMode = mode;
}

eDisplayMode_t Get_SystemDisplayMode(void)
{
	return systemDisplayMode;
}

void Set_SystemDisplayChannel(eChannel_t channel)
{
	systemDisplayChannel = channel;
}

eChannel_t Get_SystemDisplayChannel(void)
{
	return systemDisplayChannel;
}

void DisplayMode_FullScreen(eChannel_t ch)
{
	Set_SystemDisplayChannel(ch);
	Set_SystemDisplayMode(DISPLAY_MODE_FULL);
	Display_FullMode(ch);
}

void DisplayMode_SplitScreen(eSplitMode_t splitMode)
{
	Set_SystemDisplayMode(DISPLAY_MODE_SPLIT);
	Set_SystemDisplayChannel(CHANNEL_SPLIT);
	Display_SplitMode(Get_SystemSplitMode());
}

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
