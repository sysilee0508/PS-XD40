/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Clock information
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

#include "clock.h"
#include <raptor3_ioctl.h>
#include "raptor3_api_drv.h"

extern int g_fd_raptor3_drv;

void NC_VD_CLK_ADC_Set(NC_VD_CLK_STR *ClkADC)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_ADC_CLOCK_SET, ClkADC );
}
void NC_VD_CLK_PRE_Set(NC_VD_CLK_STR *ClkPRE)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_PRE_CLOCK_SET, ClkPRE );
}

void NC_VD_CLK_VO_AUTO_Set(NC_VD_CLK_VO_STR *VoutClk)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_AUTO_VCLK_SET, VoutClk);
}

void NC_VD_CLK_SET(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_CLOCK_SET, Dev_Ch_Info);

}
