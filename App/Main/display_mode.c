//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
typedef enum
{
	VIDEO_RESOLUTION_1080P,	// 1080P
	VIDEO_RESOLUTION_720P,		// 720P
	VIDEO_RESOLUTION_MAX
} eVideoResolution_t;

typedef enum NC_VIVO_CH_FORMATDEF
{
	NC_VIVO_CH_FORMATDEF_UNKNOWN = 0,
	NC_VIVO_CH_FORMATDEF_AUTO,

	AHD20_SD_H960_NT,
	AHD20_SD_H960_PAL,
	AHD20_SD_SH720_NT,
	AHD20_SD_SH720_PAL,
	AHD20_SD_H1280_NT,
	AHD20_SD_H1280_PAL,
	AHD20_SD_H1440_NT,
	AHD20_SD_H1440_PAL,
	AHD20_SD_H960_EX_NT,
	AHD20_SD_H960_EX_PAL,
	AHD20_SD_H960_2EX_NT,
	AHD20_SD_H960_2EX_PAL,
	AHD20_SD_H960_2EX_Btype_NT,
	AHD20_SD_H960_2EX_Btype_PAL,

	AHD20_1080P_60P, // For Test
	AHD20_1080P_50P, // For Test

	AHD20_1080P_30P,
	AHD20_1080P_25P,

	AHD20_720P_60P,
	AHD20_720P_50P,
	AHD20_720P_30P,
	AHD20_720P_25P,
	AHD20_720P_30P_EX,
	AHD20_720P_25P_EX,
	AHD20_720P_30P_EX_Btype,
	AHD20_720P_25P_EX_Btype,

	AHD30_4M_30P,
	AHD30_4M_25P,
	AHD30_4M_15P,
	AHD30_3M_30P,
	AHD30_3M_25P,
	AHD30_3M_18P,
	AHD30_5M_12_5P,
	AHD30_5M_20P,


	AHD30_5_3M_20P,
	AHD30_6M_18P,
	AHD30_6M_20P,
	AHD30_8M_X_30P,
	AHD30_8M_X_25P,
	AHD30_8M_7_5P,
	AHD30_8M_12_5P,
	AHD30_8M_15P,

	TVI_FHD_30P,
	TVI_FHD_25P,
	TVI_HD_60P,
	TVI_HD_50P,
	TVI_HD_30P,
	TVI_HD_25P,
	TVI_HD_30P_EX,
	TVI_HD_25P_EX,
	TVI_HD_B_30P,
	TVI_HD_B_25P,
	TVI_HD_B_30P_EX,
	TVI_HD_B_25P_EX,
	TVI_3M_18P,
	TVI_5M_12_5P,

	TVI_5M_20P, /*By Edward for testing when 170912 */

	TVI_4M_30P,
	TVI_4M_25P,
	TVI_4M_15P,  /*By Edward for testing when 170912 */

	CVI_FHD_30P,
	CVI_FHD_25P,
	CVI_HD_60P,
	CVI_HD_50P,
	CVI_HD_30P,
	CVI_HD_25P,
	CVI_HD_30P_EX,
	CVI_HD_25P_EX,
	CVI_4M_30P,
	CVI_4M_25P,
	CVI_8M_15P,
	CVI_8M_12_5P,

	NC_VIVO_CH_FORMATDEF_MAX,

} NC_VIVO_CH_FORMATDEF;

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
					VS4210_Input1080P_Output1080P_Mode0_w0();
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
					VS4210_Input1080P_Output1080P_Mode0_w1();
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
					VS4210_Input1080P_Output1080P_Mode0_w1();
					break;
				case VIDEO_RESOLUTION_720P:
					VS4210_Input1080P_Output1080P_Mode0_w1();
					break;	
				default : 
					VS4210_Input1080P_Output1080P_Mode0_w1();
				break;	
			}
			break;			

		case CHANNEL4:
			oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL2);
			switch(oCurVideoRes)
			{
				case VIDEO_RESOLUTION_1080P:
					VS4210_Input1080P_Output1080P_Mode0_w0();
					break;
				case VIDEO_RESOLUTION_720P:
					VS4210_Input1080P_Output1080P_Mode0_w0();
					break;	
				default : 
					VS4210_Input1080P_Output1080P_Mode0_w0();
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

