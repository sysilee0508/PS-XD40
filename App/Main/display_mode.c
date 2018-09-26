//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

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

