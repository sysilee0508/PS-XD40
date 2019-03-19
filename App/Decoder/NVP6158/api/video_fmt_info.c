/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Video Format Information
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

#include "video_fmt_info.h"
#include "video_fmt_input.h"
#include "video_fmt_output.h"
#include "clock.h"
#include "_debug_macro.h"


NC_VI_CH_FMT_S *NC_HI_VI_Get_ChannelFormat( NC_VIVO_CH_FORMATDEF def )
{

	NC_VI_CH_FMT_S *pRet = &VD_VI_ChannelFormatDefs[ def ];
	if(pRet->width==0 || pRet->height==0)
	{
		DBG_ERR("Not Supported format Yet!!!(%d)\n",def);
	}
	return  pRet;
}

NC_VO_PORT_FMT_S *NC_HI_VO_Get_PortFormat( NC_VIVO_CH_FORMATDEF def )
{
	NC_VO_PORT_FMT_S *pRet = &VD_VO_PortFormatDefs[ def ];
	if(pRet->width==0 || pRet->height==0)
	{
		DBG_ERR("Not Supported format Yet!!!(%d)\n",def);
	}
	return  pRet;
}

NC_VIVO_CH_FORMATDEF NC_HI_VI_FindFormatDef( NC_FORMAT_STANDARD format_standard,
		NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps  )
{
	int ii;

	if(format_standard == FMT_AUTO)
		return NC_VIVO_CH_FORMATDEF_AUTO;

	for(ii=0;ii<NC_VIVO_CH_FORMATDEF_MAX;ii++)
	{
		NC_VI_CH_FMT_S *pFmt = &VD_VI_ChannelFormatDefs[ ii ];

		if( pFmt->format_standard == format_standard )
		if( pFmt->format_resolution == format_resolution )
		if( pFmt->format_fps == format_fps )
			return ii;
	}

	DBG_ERR("UNKNOWN format!!!\n");

	return NC_VIVO_CH_FORMATDEF_UNKNOWN;
}







