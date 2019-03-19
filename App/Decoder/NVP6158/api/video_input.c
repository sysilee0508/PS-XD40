/********************************************************************************
*
*  Copyright (C) 2016 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Video Input
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "_debug_macro.h"
#include <raptor3_ioctl.h>
#include "raptor3_api_drv.h"
#include "video_auto_detect.h"
#include "video_fmt_info.h"
#include "clock.h"

void NC_VD_VI_Mode_Set(unsigned char ch, NC_VI_CH_FMT_S *pChFmt)
{
	NC_VD_DEV_CH_INFO_STR DEV_CH_INFO;

	unsigned char oChannel = ch % 4;
	unsigned char oDevNum = ch / 4;

	DEV_CH_INFO.Ch = oChannel;
	DEV_CH_INFO.Dev_Num = oDevNum;
	DEV_CH_INFO.Fmt_Def = NC_HI_VI_FindFormatDef(pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );

	NC_VD_VI_VAFE_Init(&DEV_CH_INFO);
	NC_VD_VI_Format_Set(&DEV_CH_INFO);
	NC_VD_VI_Chroma_Set(&DEV_CH_INFO);
	NC_VD_VI_H_Timing_Set(&DEV_CH_INFO);
	NC_VD_VI_H_Scaler_Mode_Set(&DEV_CH_INFO);
	NC_VD_VI_HPLL_Set(&DEV_CH_INFO);

	NC_VD_VI_COLOR_Set(&DEV_CH_INFO);
	NC_VD_VI_NEW_FORMAT_Set(&DEV_CH_INFO);
	NC_VD_CLK_SET(&DEV_CH_INFO);
}




