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
#include "motion.h"
#include "raptor3_api_drv.h"

void NC_VD_MOTION_Detection_Get(NC_VD_MOTION_STR *Motion_Set)
{
	NC_VD_MOTION_DETECTION_GET(Motion_Set);
}

void NC_VD_MOTION_OnOff_Set(NC_VD_MOTION_STR Motion_Set)
{
	NC_VD_MOTION_DATA_SET(&Motion_Set);
}

void NC_VD_MOTION_Pixel_All_OnOff_Set(NC_VD_MOTION_STR Motion_Set)
{
	NC_VD_MOTION_PIXEL_ALL_DATA_SET(&Motion_Set);
}

void NC_VD_MOTION_Pixel_OnOff_Set(NC_VD_MOTION_STR Motion_Set)
{
	NC_VD_MOTION_PIXEL_DATA_SET(&Motion_Set);
}

void NC_VD_MOTION_Pixel_OnOff_Get(NC_VD_MOTION_STR *Motion_Set)
{

	NC_VD_MOTION_PIXEL_DATA_GET(Motion_Set);
}

void NC_VD_MOTION_TSEN_Set(NC_VD_MOTION_STR Motion_Set)
{
	NC_VD_MOTION_TSEN_DATA_SET(&Motion_Set);
}

void NC_VD_MOTION_PSEN_Set(NC_VD_MOTION_STR Motion_Set)
{
	NC_VD_MOTION_PSEN_DATA_SET(&Motion_Set);
}
