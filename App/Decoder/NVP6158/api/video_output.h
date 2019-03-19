/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Video Output
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _RAPTOR3_VIDEO_OUTPUT_
#define _RAPTOR3_VIDEO_OUTPUT_

#define MAX_CH_PER_CHIP 4

#include <raptor3_fmt.h>
#include "video_fmt_info.h"

void NC_VD_VO_Mode_ChSeq_Get( NC_CH *SEQ0, NC_CH *SEQ1, NC_CH *SEQ2, NC_CH *SEQ3 );
void NC_VD_VO_Mode_Set( unsigned char devnum, NC_PORT port, NC_VO_PORT_FMT_S *pPortFmt,
						  NC_OUTPUT_MUX_MODE mux_mode,
						  NC_OUTPUT_INTERFACE output_interface,
						  NC_OUTPUT_EDGE edge,
						  NC_CH SEQ0, NC_CH SEQ1, NC_CH SEQ2, NC_CH SEQ3);

void NC_VD_VO_Mode_Set_New( unsigned char ch, unsigned char devnum, NC_PORT port, NC_VO_PORT_FMT_S *pPortFmt,
						  NC_OUTPUT_MUX_MODE mux_mode,
						  NC_OUTPUT_INTERFACE output_interface,
						  NC_OUTPUT_EDGE edge,
						  NC_CH SEQ0, NC_CH SEQ1, NC_CH SEQ2, NC_CH SEQ3);

#endif

