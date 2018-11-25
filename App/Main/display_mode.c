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
static eDisplayMode_t systemDisplayMode = DISPLAY_MODE_QUAD_A;
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

static void Display_FullScreen(eChannel_t ch)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	switch(ch)
	{
		case CHANNEL1:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					VS4210_Input1080P_Output1080P_Mode0_w0();
					break;
				case VIDEO_RESOLUTION_720P:
					VS4210_Input720P_Output1080P_Mode0_w0();
					break;	
				default : 
					VS4210_Input1080P_Output1080P_Mode0_w0();
					break;	
			}
			break;
			
		case CHANNEL2:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					VS4210_Input1080P_Output1080P_Mode0_w1();
					break;
				case VIDEO_RESOLUTION_720P:
					VS4210_Input720P_Output1080P_Mode0_w1();
					break;	
				default : 
					VS4210_Input1080P_Output1080P_Mode0_w1();
				break;	
			}
			break;
			
		case CHANNEL3:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					VS4210_Input1080P_Output1080P_Mode0_w2();
					break;
				case VIDEO_RESOLUTION_720P:
					VS4210_Input720P_Output1080P_Mode0_w2();
					break;	
				default : 
					VS4210_Input1080P_Output1080P_Mode0_w2();
				break;	
			}
			break;			

		case CHANNEL4:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					VS4210_Input1080P_Output1080P_Mode0_w3();
					break;
				case VIDEO_RESOLUTION_720P:
					VS4210_Input720P_Output1080P_Mode0_w3();
					break;	
				default : 
					VS4210_Input1080P_Output1080P_Mode0_w3();
				break;	
			}
			break;			
	}
}

static void Display_DualScreen(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode12_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode12_w0();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode12_w0();
			break;				
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode12_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode12_w1();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode12_w1();
			break;	
	}
}

static void Display_TripleScreen(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode9_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode9_w0();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode9_w0();
			break;				
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode9_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode9_w1();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode9_w1();
			break;	
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode9_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode9_w2();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode9_w2();
			break;		
	}		
}

static void Display_QuadScreen(void)
{
	eVideoResolution_t oCurVideoRes = VIDEO_RESOLUTION_MAX;

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w0();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode1_w0();
			break;				
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w1();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode1_w1();
			break;	
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL3);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w2();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode1_w2();
			break;		
	}

	oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL4);
	switch(oCurVideoRes)
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w3();
			break;	
		default : 
			VS4210_Input1080P_Output1080P_Mode1_w3();
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

void Set_DisplayoutputMode_table(void)
{
	eDisplayMode_t displayMode;
	eChannel_t displayChannel;

	if(Check_VideoFormat_Change() == TRUE)
	{
		displayMode = Get_SystemDisplayMode();
		if(displayMode == DISPLAY_MODE_FULL_SCREEN)
		{
			displayChannel = Get_SystemDisplayChannel();
			{
				Display_FullScreen(displayChannel);
			}
		}
		else if(displayMode == DISPLAY_MODE_QUAD)
		{
			Display_QuadScreen();
			//Display_DualScreen();
		}
		else
		{
			Display_QuadScreen();
			//Display_DualScreen();
		}
		Delay_ms(500);
	}
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
	Set_SystemDisplayMode(DISPLAY_MODE_FULL_SCREEN);
	Display_FullScreen(ch);
}

void DisplayMode_Split(eDisplayMode_t splitMode)
{
	Set_SystemDisplayMode(splitMode);
	Set_SystemDisplayChannel(CHANNEL_SPLIT);
	Display_QuadScreen();
	//Display_DualScreen();
}
