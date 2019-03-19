/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Motion information
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _RAPTOR3_MOTION_H_
#define _RAPTOR3_MOTION_H_

typedef struct _NC_VD_MOTION_STR{
	unsigned char Ch;
	unsigned char devnum;
	unsigned char Set;

	unsigned char FmtDef;
}NC_VD_MOTION_STR;

void NC_VD_MOTION_OnOff_Set(NC_VD_MOTION_STR Motion_Set);
void NC_VD_MOTION_Display_OnOff_Set(NC_VD_MOTION_STR Motion_Set);
void NC_VD_MOTION_Pixel_All_OnOff_Set(NC_VD_MOTION_STR Motion_Set);
void NC_VD_MOTION_Pixel_OnOff_Set(NC_VD_MOTION_STR Motion_Set);
void NC_VD_MOTION_Pixel_OnOff_Get(NC_VD_MOTION_STR *Motion_Set);
void NC_VD_MOTION_TSEN_Set(NC_VD_MOTION_STR Motion_Set);
void NC_VD_MOTION_PSEN_Set(NC_VD_MOTION_STR Motion_Set);

#endif /* _RAPTOR3_MOTION_H_ */
