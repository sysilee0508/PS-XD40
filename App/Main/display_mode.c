//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"
#include "display_mode.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
#include "NVP6168.h"
#endif

//=============================================================================
//  Define & MACRO
//=============================================================================
#define IS_CROPPING(mode)		\
	((mode == DISPLAY_MODE_2SPLIT_CROP) || (mode == DISPLAY_MODE_3SPLIT_A_CROP) || \
	(mode == DISPLAY_MODE_3SPLIT_B_CROP) || (mode == DISPLAY_MODE_3SPLIT_C_CROP) || \
	(mode == DISPLAY_MODE_3SPLIT_D_CROP) || (mode == DISPLAY_MODE_QUAD_B_CROP) || \
	(mode == DISPLAY_MODE_QUAD_C_CROP) || (mode == DISPLAY_MODE_QUAD_D_CROP) || \
	 (mode == DISPLAY_MODE_QUAD_E_CROP))?TRUE:FALSE


#if BD_NVP == NVP6158
#define Get_Current_Video_Format(ch)				 NVP6158_Current_Video_Format_Check(ch)
#elif BD_NVP == NVP6168
#define Get_Current_Video_Format(ch)				 NVP6168_Current_VideoFormat_Get(ch)
#endif
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
	static eVideoResolution_t oCurVideoRes[NUM_OF_CHANNEL] = {0,};
				//{VIDEO_RESOLUTION_720P, VIDEO_RESOLUTION_720P, VIDEO_RESOLUTION_720P, VIDEO_RESOLUTION_720P};
	
	switch(Get_Current_Video_Format(ch))
	{
		//case AHD20_1080P_60P:
		//case AHD20_1080P_50P:
		case AHD_1080P_30P:
		case AHD_1080P_25P:
		case TVI_FHD_30P:
		case TVI_FHD_25P:
		case CVI_FHD_30P:
		case CVI_FHD_25P:
			oCurVideoRes[ch] = VIDEO_RESOLUTION_1080P;
			break;

		case AHD_720P_60P:
		case AHD_720P_50P:
		case AHD_720P_30P:
		case AHD_720P_25P:
		case AHD_720P_30P_EX:
		case AHD_720P_25P_EX:
		case AHD_720P_30P_EX_Btype:
		case AHD_720P_25P_EX_Btype:
		case TVI_HD_60P:
		case TVI_HD_50P:
		case TVI_HD_30P:
		case TVI_HD_25P:
		case TVI_HD_30P_EX:
		case TVI_HD_25P_EX:
		case TVI_HD_B_30P:
		case TVI_HD_B_25P:
		case TVI_HD_B_30P_EX:
		case TVI_HD_B_25P_EX:
		case CVI_HD_60P:
		case CVI_HD_50P:
		case CVI_HD_30P:
		case CVI_HD_25P:
		case CVI_HD_30P_EX:
		case CVI_HD_25P_EX:
			oCurVideoRes[ch] = VIDEO_RESOLUTION_720P;
			break;
	}

	return oCurVideoRes[ch];	
}

static void Display_FullMode(eChannel_t ch)
{
	eVideoResolution_t oCurVideoRes = Get_Current_Video_Resolution_Each_Channel(ch);

	switch(ch)
	{
		case CHANNEL1:
			if(Get_Current_Video_Resolution_Each_Channel(ch) == VIDEO_RESOLUTION_720P)
			{
				VS4210_Input720P_Output1080P_Mode0_w0();
			}
			else
			{
				VS4210_Input1080P_Output1080P_Mode0_w0();
			}
			break;
			
		case CHANNEL2:
			if(Get_Current_Video_Resolution_Each_Channel(ch) == VIDEO_RESOLUTION_720P)
			{
				VS4210_Input720P_Output1080P_Mode0_w1();
			}
			else
			{
				VS4210_Input1080P_Output1080P_Mode0_w1();
			}
			break;
			
		case CHANNEL3:
			if(Get_Current_Video_Resolution_Each_Channel(ch) == VIDEO_RESOLUTION_720P)
			{
				VS4210_Input720P_Output1080P_Mode0_w2();
			}
			else
			{
				VS4210_Input1080P_Output1080P_Mode0_w2();
			}
			break;			

		case CHANNEL4:
			if(Get_Current_Video_Resolution_Each_Channel(ch) == VIDEO_RESOLUTION_720P)
			{
				VS4210_Input720P_Output1080P_Mode0_w3();
			}
			else
			{
				VS4210_Input1080P_Output1080P_Mode0_w3();
			}
			break;			
	}
}

static void Display_2SplitMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode12_w0();
			else
				VS4210_Input1080P_Output1080P_Mode12_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode12_w0();
			else
				VS4210_Input720P_Output720P_Mode12_w0_cropping();
			break;
	}
	
	//ch2
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P) 
			{
				if(fCropping==FALSE)
					VS4210_Input1080P_Output1080P_Mode12_w1();
				else
					VS4210_Input1080P_Output1080P_Mode12_w1_cropping();
			}
			else		
			{
				if(fCropping==FALSE)
					VS4210_Input1080P_Output720P_Mode12_w1();
				else
					VS4210_Input1080P_Output720P_Mode12_w1_cropping();
			}
			break;
			
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
			{
				if(fCropping==FALSE)
					VS4210_Input720P_Output1080P_Mode12_w1();
				else
					VS4210_Input720P_Output1080P_Mode12_w1_cropping();
			}
			else
			{
				if(fCropping==FALSE)
					VS4210_Input720P_Output720P_Mode12_w1();
				else
					VS4210_Input720P_Output720P_Mode12_w1_cropping();
			}
			break;	
	}
}

static void Display_3SplitAMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode8_w0();
			else
				VS4210_Input1080P_Output1080P_Mode8_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode8_w0();
			else
				VS4210_Input720P_Output720P_Mode8_w0_cropping();
			break;
	}

	//ch 2
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode8_w1();
			else
				VS4210_Input1080P_Output720P_Mode8_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode8_w1();
			else
				VS4210_Input720P_Output720P_Mode8_w1();
			break;
	}
	//ch 3
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode8_w2();
			else
				VS4210_Input1080P_Output720P_Mode8_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode8_w2();
			else
				VS4210_Input720P_Output720P_Mode8_w2();			
			break;
	}
}

static void Display_3SplitBMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode9_w0();
			else
				VS4210_Input1080P_Output1080P_Mode9_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode9_w0();
			else
				VS4210_Input720P_Output720P_Mode9_w0_cropping();
			break;	
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode9_w1();
			else
				VS4210_Input1080P_Output720P_Mode9_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode9_w1();
			else
				VS4210_Input720P_Output720P_Mode9_w1();
			break;	
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode9_w2();
			else
				VS4210_Input1080P_Output720P_Mode9_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode9_w2();
			else
				VS4210_Input720P_Output720P_Mode9_w2();
			break;	
	}		
}

static void Display_3SplitCMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode10_w0();
			else
				VS4210_Input1080P_Output1080P_Mode10_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode10_w0();
			else
				VS4210_Input720P_Output720P_Mode10_w0_cropping();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode10_w1();
			else
				VS4210_Input1080P_Output720P_Mode10_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode10_w1();
			else
				VS4210_Input720P_Output720P_Mode10_w1();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode10_w2();
			else
				VS4210_Input1080P_Output720P_Mode10_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode10_w2();
			else
				VS4210_Input720P_Output720P_Mode10_w2();
			break;
	}
}

static void Display_3SplitDMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode11_w0();
			else
				VS4210_Input1080P_Output1080P_Mode11_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode11_w0();
			else
				VS4210_Input720P_Output720P_Mode11_w0_cropping();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode11_w1();
			else
				VS4210_Input1080P_Output720P_Mode11_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode11_w1();
			else
				VS4210_Input720P_Output720P_Mode11_w1();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode11_w2();
			else
				VS4210_Input1080P_Output720P_Mode11_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode11_w2();
			else
				VS4210_Input720P_Output720P_Mode11_w2();
			break;
	}
}

static void Display_QuadAMode(void)
{
	SetSrcMainFrmt(VIDSRC_1920x1080p60);
	
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL1))
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w0();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w0();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w1();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w2();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL4))
	{
		case VIDEO_RESOLUTION_1080P:
			VS4210_Input1080P_Output1080P_Mode1_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			VS4210_Input720P_Output1080P_Mode1_w3();
			break;
	}
}

static void Display_QuadBMode(BOOL fCropping) // mode-6
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode6_w0();
			else
				VS4210_Input1080P_Output1080P_Mode6_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode6_w0();
			else
				VS4210_Input720P_Output720P_Mode6_w0_cropping();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode6_w1();
			else
				VS4210_Input1080P_Output720P_Mode6_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode6_w1();
			else
				VS4210_Input720P_Output720P_Mode6_w1();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode6_w2();
			else
				VS4210_Input1080P_Output720P_Mode6_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode6_w2();
			else
				VS4210_Input720P_Output720P_Mode6_w2();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL4))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode6_w3();
			else
				VS4210_Input1080P_Output720P_Mode6_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode6_w3();
			else
				VS4210_Input720P_Output720P_Mode6_w3();
			break;
	}
}

static void Display_QuadCMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode7_w0();
			else
				VS4210_Input1080P_Output1080P_Mode7_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode7_w0_cropping();
			else
				VS4210_Input720P_Output720P_Mode7_w0();
			break;
	}
	//Ch2
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode7_w1();
			else
				VS4210_Input1080P_Output720P_Mode7_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode7_w1();
			else
				VS4210_Input720P_Output720P_Mode7_w1();
			break;
	}
	//Ch3
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode7_w2();
			else
				VS4210_Input1080P_Output720P_Mode7_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode7_w2();
			else
				VS4210_Input720P_Output720P_Mode7_w2();			
			break;
	}
	//Ch4
	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL4))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode7_w3();
			else
				VS4210_Input1080P_Output720P_Mode7_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode7_w3();
			else
				VS4210_Input720P_Output720P_Mode7_w3();
			break;
	}

}

static void Display_QuadDMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode4_w0();
			else
				VS4210_Input1080P_Output1080P_Mode4_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode4_w0();
			else
				VS4210_Input720P_Output720P_Mode4_w0_cropping();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode4_w1();
			else
				VS4210_Input1080P_Output720P_Mode4_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode4_w1();
			else
				VS4210_Input720P_Output720P_Mode4_w1();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode4_w2();
			else
				VS4210_Input1080P_Output720P_Mode4_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode4_w2();
			else
				VS4210_Input720P_Output720P_Mode4_w2();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL4))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode4_w3();
			else
				VS4210_Input1080P_Output720P_Mode4_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode4_w3();
			else
				VS4210_Input720P_Output720P_Mode4_w3();
			break;
	}
}

static void Display_QuadEMode(BOOL fCropping)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	//ch 1
	switch(outRes)
	{
		case VIDEO_RESOLUTION_1080P:
			if(fCropping==FALSE)
				VS4210_Input1080P_Output1080P_Mode5_w0();
			else
				VS4210_Input1080P_Output1080P_Mode5_w0_cropping();
			break;
		case VIDEO_RESOLUTION_720P:
			if(fCropping==FALSE)
				VS4210_Input720P_Output720P_Mode5_w0();
			else
				VS4210_Input720P_Output720P_Mode5_w0_cropping();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL2))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode5_w1();
			else
				VS4210_Input1080P_Output720P_Mode5_w1();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode5_w1();
			else
				VS4210_Input1080P_Output720P_Mode5_w1();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL3))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode5_w2();
			else
				VS4210_Input1080P_Output720P_Mode5_w2();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode5_w2();
			else
				VS4210_Input720P_Output720P_Mode5_w2();
			break;
	}

	switch(Get_Current_Video_Resolution_Each_Channel(CHANNEL4))
	{
		case VIDEO_RESOLUTION_1080P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input1080P_Output1080P_Mode5_w3();
			else
				VS4210_Input1080P_Output720P_Mode5_w3();
			break;
		case VIDEO_RESOLUTION_720P:
			if(outRes == VIDEO_RESOLUTION_1080P)
				VS4210_Input720P_Output1080P_Mode5_w3();
			else
				VS4210_Input720P_Output720P_Mode5_w3();
			break;
	}
}

static BYTE Check_VideoFormat_Change(void)
{
	eChannel_t channel;
	BYTE oCurVideofmt;
	static BYTE oPreVideofmt[NUM_OF_CHANNEL] = {0,};
	BYTE videoFormatChanged = FALSE;

	for(channel = CHANNEL1; channel <NUM_OF_CHANNEL; channel++)
	{
		oCurVideofmt = Get_Current_Video_Format(channel);
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
	BOOL fCropping = IS_CROPPING(splitMode);

	vs4210_system_init();
	Delay_ms(10);
	switch(splitMode)
	{
		case DISPLAY_MODE_QUAD_A:
			Display_QuadAMode();
			break;

		case DISPLAY_MODE_QUAD_B:
		case DISPLAY_MODE_QUAD_B_CROP:
			Display_QuadBMode(fCropping);
			break;

		case DISPLAY_MODE_QUAD_C:
		case DISPLAY_MODE_QUAD_C_CROP:
			Display_QuadCMode(fCropping);
			break;

		case DISPLAY_MODE_QUAD_D:
		case DISPLAY_MODE_QUAD_D_CROP:
			Display_QuadDMode(fCropping);
			break;

		case DISPLAY_MODE_QUAD_E:
		case DISPLAY_MODE_QUAD_E_CROP:
			Display_QuadEMode(fCropping);
			break;

		case DISPLAY_MODE_3SPLIT_A:
		case DISPLAY_MODE_3SPLIT_A_CROP:
			Display_3SplitAMode(fCropping);
			break;

		case DISPLAY_MODE_3SPLIT_B:
		case DISPLAY_MODE_3SPLIT_B_CROP:
			Display_3SplitBMode(fCropping);
			break;

		case DISPLAY_MODE_3SPLIT_C:
		case DISPLAY_MODE_3SPLIT_C_CROP:
			Display_3SplitCMode(fCropping);
			break;

		case DISPLAY_MODE_3SPLIT_D:
		case DISPLAY_MODE_3SPLIT_D_CROP:
			Display_3SplitDMode(fCropping);
			break;

		case DISPLAY_MODE_2SPLIT:
		case DISPLAY_MODE_2SPLIT_CROP:
			Display_2SplitMode(fCropping);
			break;

		default:
			Display_QuadAMode();
			break;
	}
}

void Set_DisplayoutputMode_table(void)
{
	eDisplayMode_t displayMode;
	
	if(Check_VideoFormat_Change() == TRUE)
	{
		displayMode = Get_SystemDisplayMode();
		if(displayMode == DISPLAY_MODE_FULL)
		{
			Display_FullMode(Get_SystemDisplayChannel());
			//DisplayMode_FullScreen(Get_SystemDisplayChannel());
		}
		else
		{
			//Display_SplitMode(Get_SystemSplitMode());
			DisplayMode_SplitScreen(Get_SystemSplitMode());
		}
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
	SetSrcMainFrmt(VIDSRC_1920x1080p60);
	Display_FullMode(ch);
}

void DisplayMode_SplitScreen(eSplitMode_t splitMode)
{
	eVideoResolution_t outRes = Get_Current_Video_Resolution_Each_Channel(CHANNEL1);
	
	Set_SystemDisplayMode(DISPLAY_MODE_SPLIT);
	Set_SystemDisplayChannel(CHANNEL_SPLIT);

	if((splitMode == DISPLAY_MODE_QUAD_A) || (outRes == VIDEO_RESOLUTION_1080P))
	{
		SetSrcMainFrmt(VIDSRC_1920x1080p60);
	}
	else
	{
		SetSrcMainFrmt(VIDSRC_1280x720p60);
	}

	Display_SplitMode(splitMode);
}

eInputVideoMode_t Get_InputVideoMode(eChannel_t channel)
{
	eInputVideoMode_t videoMode;

	switch(Get_Current_Video_Format(channel))
	{
		//case AHD20_1080P_60P:
		case AHD_1080P_30P:
		case TVI_FHD_30P:
		case CVI_FHD_30P:
			videoMode = INPUT_VIDEO_1080P30;
			break;

		//case AHD20_1080P_50P:
		case AHD_1080P_25P:
		case TVI_FHD_25P:
		case CVI_FHD_25P:
			videoMode = INPUT_VIDEO_1080P25;
			break;

		case AHD_720P_60P:
		case AHD_720P_30P:
		case AHD_720P_30P_EX:
		case AHD_720P_30P_EX_Btype:
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
		case TVI_HD_B_30P:
		case TVI_HD_B_30P_EX:
		case CVI_HD_60P:
		case CVI_HD_30P:
		case CVI_HD_30P_EX:
			videoMode = INPUT_VIDEO_720P30;
			break;

		case AHD_720P_50P:
		case AHD_720P_25P:
		case AHD_720P_25P_EX:
		case AHD_720P_25P_EX_Btype:
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
		case TVI_HD_B_25P:
		case TVI_HD_B_25P_EX:
		case CVI_HD_50P:
		case CVI_HD_25P:
		case CVI_HD_25P_EX:
			videoMode = INPUT_VIDEO_720P25;
			break;

		case SD_H960_NT:
		case SD_SH720_NT:
		case SD_H1280_NT:
		case SD_H1440_NT:
		case SD_H960_EX_NT:
		case SD_H960_2EX_NT:
		case SD_H960_2EX_Btype_NT:
			videoMode = INPUT_VIDEO_CVBS_NTSC;
			break;
			
		case SD_H960_PAL:
		case SD_SH720_PAL:
		case SD_H1280_PAL:
		case SD_H1440_PAL:
		case SD_H960_EX_PAL:
		case SD_H960_2EX_PAL:
		case SD_H960_2EX_Btype_PAL:
			videoMode = INPUT_VIDEO_CVBS_PAL;
			break;

		default:
			videoMode = INPUT_VIDEO_MAX;
			break;
	}
	return videoMode;
}
