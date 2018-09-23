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
	return sys_status.current_split_mode;
}

void Set_DisplayoutputMode_FullScreen(BYTE ch)
{
	sys_status.current_split_mode = ch;
}

void Set_DisplayoutputMode_Splite4(void)
{
	sys_status.current_split_mode = SPLITMODE_SPLIT4_1;
}
