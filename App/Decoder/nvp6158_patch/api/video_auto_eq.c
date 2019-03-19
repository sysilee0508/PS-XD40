/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: equalizer
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>

#include "video_fmt_info.h"
#include "video_output.h"
#include "clock.h"
#include "video_auto_detect.h"


/**************************************************************************************
* @desc
* 	Function to proceed with EQ (Analog, Digital EQ) setting according to cable distance.
*
* @param_in		(unsigned char)Ch								Video Channel
* @param_out	(CABLE_DISTANCE)Dist					Distance Value
* @param_in		(NC_VD_AUTO_DETECT_FLAG_STR *)pFlag		Structure parameter for Flag update during auto format detection process.
*
* @return   	void		       						None
***************************************************************************************/
void NC_APP_VD_MANUAL_VIDEO_EQ_Set(unsigned char Ch, CABLE_DISTANCE Dist, NC_VIVO_CH_FORMATDEF FmtDef )
{
	unsigned char oChannel;
	unsigned char oDevNum;
	NC_VD_EQ_STR sEQInfo;

	oChannel = Ch % 4;
	oDevNum = Ch / 4;

	sEQInfo.Ch = oChannel;
	sEQInfo.Dev_Num = oDevNum;
	sEQInfo.distance = Dist;
	sEQInfo.FmtDef = FmtDef;

	NC_VD_MANUAL_VIDEO_EQ_Set(&sEQInfo);
}

