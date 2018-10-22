//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"
#include "display_mode.h"

extern NC_VIVO_CH_FORMATDEF NVP6158_Current_Video_Format_Check(unsigned char oLogicalChannel);

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
BYTE Get_DisplayoutputMode(void)
{
	eDisplayMode_t displayMode;

	Read_NvItem_DisplayMode(&displayMode);
	return displayMode;//sys_status.current_split_mode;
}

BYTE Get_DisplayoutputChannel(void)
{
	eChannel_t channel;

	Read_NvItem_DisplayChannel(&channel);
	return (BYTE)channel;
}

void Set_DisplayoutputMode_FullScreen(BYTE ch)
{
	Write_NvItem_DisplayChannel((eChannel_t)ch);
	Write_NvItem_DisplayMode(DISPLAY_MODE_FULL_SCREEN);
//	sys_status.current_split_mode = ch;
}

void Set_DisplayoutputMode_Splite4(void)
{
	Write_NvItem_DisplayMode(DISPLAY_MODE_4SPLIT);
	Write_NvItem_DisplayChannel(CHANNEL_QUAD);
//	sys_status.current_split_mode = SPLITMODE_SPLIT4;
}

eVideoResolution_t Get_Current_Video_Resolution_Each_Channel(eChannel_t ch)
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

void Display_FullScreen(eChannel_t ch)
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

void Display_QuadScreen(void)
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

void Set_DisplayoutputMode_table(void)
{
	BYTE displayMode;
	BYTE displayChannel;

	displayMode = Get_DisplayoutputMode();
	if(displayMode == DISPLAY_MODE_FULL_SCREEN)
	{
		displayChannel = Get_DisplayoutputChannel();
		{
			Display_FullScreen((eChannel_t)displayChannel);
		}
	}
	else if(displayMode == DISPLAY_MODE_4SPLIT)
	{
		Display_QuadScreen();
	}
	else
	{
		Display_QuadScreen();
	}		
}

