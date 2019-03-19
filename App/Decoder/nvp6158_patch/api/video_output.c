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

#include "video_fmt_info.h"
#include "video_output.h"
#include "raptor3_api_drv.h"
#include "clock.h"

#include "hi_comm_vi.h"

extern  int g_fd_raptor3_drv;

static NC_CH _CHSEQ[ RAPTOR3_MAX_CH_PER_CHIP ];

static char g_PortEn;
static char g_MergeEn;

unsigned char __IsOver3MRTVideoFormat( const char *poFmtName )
{
	unsigned char ret = 1; //

	if((strcmp(poFmtName, "AHD30_3M_30P") == 0) ||
	   (strcmp(poFmtName, "AHD30_3M_25P") == 0) ||
	   (strcmp(poFmtName, "AHD30_4M_30P") == 0) ||
	   (strcmp(poFmtName, "AHD30_4M_25P") == 0) ||
	   (strcmp(poFmtName, "AHD30_5M_20P") == 0) ||
	   (strcmp(poFmtName, "AHD30_5_3M_20P") == 0) ||
	   (strcmp(poFmtName, "AHD30_6M_18P") == 0) ||
	   (strcmp(poFmtName, "AHD30_6M_20P") == 0) ||
	   (strcmp(poFmtName, "AHD30_8M_12_5P") == 0) ||
	   (strcmp(poFmtName, "AHD30_8M_15P") == 0) ||
	   (strcmp(poFmtName, "TVI_4M_30P") == 0) ||
	   (strcmp(poFmtName, "TVI_4M_25P") == 0) ||
	   (strcmp(poFmtName, "TVI_5M_20P") == 0) ||
	   (strcmp(poFmtName, "CVI_4M_25P") == 0) ||
	   (strcmp(poFmtName, "CVI_4M_30P") == 0) ||
	   (strcmp(poFmtName, "CVI_8M_15P") == 0) ||
	   (strcmp(poFmtName, "CVI_8M_12_5P") == 0) ||
	   (strcmp(poFmtName, "TVI_8M_15P") == 0) 

	   )
	   

	{
		ret = 0;
	}
	return ret; // 0:Over 3M RT, 1:other formation
}


void NC_VD_VO_Mode_ChSeq_Get( NC_CH *SEQ0, NC_CH *SEQ1, NC_CH *SEQ2, NC_CH *SEQ3 )
{
	if(SEQ0) *SEQ0=_CHSEQ[0];
	if(SEQ1) *SEQ1=_CHSEQ[1];
	if(SEQ2) *SEQ2=_CHSEQ[2];
	if(SEQ3) *SEQ3=_CHSEQ[3];
}

void NC_VD_VO_Mode_Set( unsigned char devnum, NC_PORT port, NC_VO_PORT_FMT_S *pPortFmt,
		NC_OUTPUT_MUX_MODE mux_mode,
		NC_OUTPUT_INTERFACE output_interface,
		NC_OUTPUT_EDGE edge,
		NC_CH SEQ0, NC_CH SEQ1, NC_CH SEQ2, NC_CH SEQ3)
{
	unsigned char ClockDelay;
	NC_VD_CLK_VO_STR VOutClk;
	NC_VD_VO_SEQ_STR VOutSeq;
	NC_VD_VO_PORT_MUX_CHID_STR VoutChID;
	NC_VD_VO_PORT_MUX_MODE_STR VOutPortMuxMode;
	NC_VD_DEV_PORT_ENABLE_STR sPortEnable;

	VOutSeq.devnum = devnum;
	VoutChID.devnum = devnum;


	_CHSEQ[0] = SEQ0;
	_CHSEQ[1] = SEQ1;
	_CHSEQ[2] = SEQ2;
	_CHSEQ[3] = SEQ3;

#ifdef __DEBUG_MSG
	printf("Name        = %s\n", pPortFmt->name);
	printf("width       = %d\n", pPortFmt->width);
	printf("height      = %d\n", pPortFmt->height);
	printf("vo_clk;     = 0x%x\n", pPortFmt->vo_clk);
	printf("merge;      = 0x%x\n", pPortFmt->merge);
	printf("mux_mode;   = 0x%x\n", pPortFmt->mux_mode);
	printf("seq_ch01;   = 0x%x\n", pPortFmt->seq_ch01);
	printf("seq_ch23;   = 0x%x\n", pPortFmt->seq_ch23);
	printf("chid_vin01; = 0x%x\n", pPortFmt->chid_vin01);
	printf("chid_vin23; = 0x%x\n", pPortFmt->chid_vin23);
#endif

	// Output Interface Setting

	if(output_interface == NC_OI_BT656) // BT656 Port Setting
	{
			VOutSeq.Port = port;
			VOutSeq.ChNum01 = ((SEQ0 & 0x0f) | ((SEQ1 & 0xf)<<4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = ((SEQ2 & 0x0f) | ((SEQ3 & 0xf)<<4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

		if( __IsOver3MRTVideoFormat ( pPortFmt->name ) == 0 )
				g_MergeEn |= (0x1 << port);
			else
				g_MergeEn &= (~(0x1 << port));

	                g_PortEn |= (0x11 << port);

			fprintf(stdout, "bt656 g_MergeEn ; [%x]\n\n", g_MergeEn);
	}
	else // BT1120 Port Setting
	{
		/*
		   0~3  :  Normal Display of Channel 1~4
		   4~7  :  Only Y Display of Channel 1~4
		   8~B  :  H_CIF Display of Channel 1~4
		   C~F  :  Only C Display of Channel 1~4
		   */
		if(port == PORTA)
		{
			//Y Only Set for BT1120
			VOutSeq.Port = PORTB;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.Port = PORTA;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);
		}
		else if(port == PORTC)
		{
			VOutSeq.Port = PORTD;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.Port = PORTC;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);
		}
		else
		{

		}

		g_MergeEn |= (0x1 << port);
	        g_PortEn |= (0x11 << port);
		fprintf(stdout, "bt1120 g_MergeEn ; [%x]\n\n", g_MergeEn);
	}

	pPortFmt->merge = g_MergeEn;
	pPortFmt->devnum = devnum;
	NC_VD_VO_Port_Format_Set(pPortFmt);

	VOutPortMuxMode.devnum = devnum;
	VOutPortMuxMode.Port = port;

	switch(mux_mode)
	{
		case VI_WORK_MODE_1Multiplex :  VOutPortMuxMode.Mode = 0; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_WORK_MODE_2Multiplex :	VOutPortMuxMode.Mode = 0x02; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_WORK_MODE_4Multiplex :  VOutPortMuxMode.Mode = 0x08; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
	}

	NC_VD_VO_Port_Mux_Mode_Set(&VOutPortMuxMode);
	NC_VD_VO_Port_Mux_ChId_Set(&VoutChID);

	if(output_interface == NC_OI_BT656)
	{
		VOutClk.Port = port;	// PORT_A, PORTB, PORTC, PORTD
		VOutClk.devnum = devnum;
		VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
		NC_VD_CLK_VO_AUTO_Set(&VOutClk);
	}
	else	// BT1120
	{
		switch(port)
		{
			case PORTA :
				VOutClk.Port = PORTA;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				VOutClk.Port = PORTB;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				break;
			case PORTC :
				VOutClk.Port = PORTC;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				VOutClk.Port = PORTD;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				break;
		}
	}

	sPortEnable.Dev_Num = devnum;
	sPortEnable.enable = g_PortEn;

	NC_VD_VO_Port_Enable(&sPortEnable);
}


void NC_VD_VO_Mode_Set_New( unsigned char ch, unsigned char devnum, NC_PORT port, NC_VO_PORT_FMT_S *pPortFmt,
		NC_OUTPUT_MUX_MODE mux_mode,
		NC_OUTPUT_INTERFACE output_interface,
		NC_OUTPUT_EDGE edge,
		NC_CH SEQ0, NC_CH SEQ1, NC_CH SEQ2, NC_CH SEQ3)
{
	unsigned char ClockDelay;
	NC_VD_CLK_VO_STR VOutClk;
	NC_VD_VO_SEQ_STR VOutSeq;
	NC_VD_VO_PORT_MUX_CHID_STR VoutChID;
	NC_VD_VO_PORT_MUX_MODE_STR VOutPortMuxMode;
	NC_VD_DEV_PORT_ENABLE_STR sPortEnable;
	NC_VD_VO_SEQ_REG_STR 	sVOutSeqreg;

	VOutSeq.devnum = devnum;
	VoutChID.devnum = devnum;
	sVOutSeqreg.devnum = devnum;

	_CHSEQ[0] = SEQ0;
	_CHSEQ[1] = SEQ1;
	_CHSEQ[2] = SEQ2;
	_CHSEQ[3] = SEQ3;

	/********************************
	 * Set sequence
	 ********************************/
	if(output_interface == NC_OI_BT656) // BT656 Port Setting
	{
		if(mux_mode == VI_WORK_MODE_2Multiplex)
		{
			NC_VD_VO_CHN_Sequence_Get(&sVOutSeqreg);
#ifdef SUPPORT_3520D_6158C
			if(port == PORTB)
			{
				if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				{
					VOutSeq.Port = port+1;
					VOutSeq.ChNum01 = (ch==0)?(0x08|(sVOutSeqreg.b1c4&0xf0)):(0x90|(sVOutSeqreg.b1c4&0x0f));
					VOutSeq.ChNum23 = (ch==0)?(0x08|(sVOutSeqreg.b1c4&0xf0)):(0x90|(sVOutSeqreg.b1c4&0x0f));
				}
				else
				{
					VOutSeq.Port = port+1;
					VOutSeq.ChNum01 = (ch==0)?(0x00|(sVOutSeqreg.b1c4&0xf0)):(0x10|(sVOutSeqreg.b1c4&0x0f));
					VOutSeq.ChNum23 = (ch==0)?(0x00|(sVOutSeqreg.b1c4&0xf0)):(0x10|(sVOutSeqreg.b1c4&0x0f));
				}
			}
			else if(port == PORTC)
			{
				if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				{
					VOutSeq.Port = port-1;
					VOutSeq.ChNum01 = (ch==2)?(0x0a|(sVOutSeqreg.b1c2&0xf0)):(0xb0|(sVOutSeqreg.b1c2&0x0f));
					VOutSeq.ChNum23 = (ch==2)?(0x0a|(sVOutSeqreg.b1c2&0xf0)):(0xb0|(sVOutSeqreg.b1c2&0x0f));
				}
				else
				{
					VOutSeq.Port = port-1;
					VOutSeq.ChNum01 = (ch==2)?(0x02|(sVOutSeqreg.b1c2&0xf0)):(0x30|(sVOutSeqreg.b1c2&0x0f));
					VOutSeq.ChNum23 = (ch==2)?(0x02|(sVOutSeqreg.b1c2&0xf0)):(0x30|(sVOutSeqreg.b1c2&0x0f));
				}
			}
#else
			if(port == PORTB)
			{
				if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				{
					VOutSeq.Port = port;
					VOutSeq.ChNum01 = (ch==0)?(0x08|(sVOutSeqreg.b1c2&0xf0)):(0x90|(sVOutSeqreg.b1c2&0x0f));
					VOutSeq.ChNum23 = (ch==0)?(0x08|(sVOutSeqreg.b1c2&0xf0)):(0x90|(sVOutSeqreg.b1c2&0x0f));
				}
				else
				{
					VOutSeq.Port = port;
					VOutSeq.ChNum01 = (ch==0)?(0x00|(sVOutSeqreg.b1c2&0xf0)):(0x10|(sVOutSeqreg.b1c2&0x0f));
					VOutSeq.ChNum23 = (ch==0)?(0x00|(sVOutSeqreg.b1c2&0xf0)):(0x10|(sVOutSeqreg.b1c2&0x0f));
				}
			}
			else if(port == PORTC)
			{
				if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				{
					VOutSeq.Port = port;
					VOutSeq.ChNum01 = (ch==2)?(0x0a|(sVOutSeqreg.b1c4&0xf0)):(0xb0|(sVOutSeqreg.b1c4&0x0f));
					VOutSeq.ChNum23 = (ch==2)?(0x0a|(sVOutSeqreg.b1c4&0xf0)):(0xb0|(sVOutSeqreg.b1c4&0x0f));
				}
				else
				{
					VOutSeq.Port = port;
					VOutSeq.ChNum01 = (ch==2)?(0x02|(sVOutSeqreg.b1c4&0xf0)):(0x30|(sVOutSeqreg.b1c4&0x0f));
					VOutSeq.ChNum23 = (ch==2)?(0x02|(sVOutSeqreg.b1c4&0xf0)):(0x30|(sVOutSeqreg.b1c4&0x0f));
				}
			}
#endif
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				g_MergeEn |= (0x1 << ch);
			else
				g_MergeEn &= (~(0x1 << ch));
		}
		else
		{
			VOutSeq.Port = port;
			VOutSeq.ChNum01 = ((SEQ0 & 0x0f) | ((SEQ1 & 0xf)<<4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = ((SEQ2 & 0x0f) | ((SEQ3 & 0xf)<<4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				g_MergeEn |= (0x1 << port);
			else
				g_MergeEn &= (~(0x1 << port));

			fprintf(stdout, "bt656 g_MergeEn ; [%x]\n\n", g_MergeEn);
		}
	}
	else // BT1120 Port Setting
	{
		/*
		   0~3  :  Normal Display of Channel 1~4
		   4~7  :  Only Y Display of Channel 1~4
		   8~B  :  H_CIF Display of Channel 1~4
		   C~F  :  Only C Display of Channel 1~4
		   */
		if(port == PORTA)
		{
			//Y Only Set for BT1120
			VOutSeq.Port = PORTB;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.Port = PORTA;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);
		}
		else if(port == PORTC)
		{
			VOutSeq.Port = PORTD;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.Port = PORTC;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);
		}
		else
		{

		}

		g_MergeEn |= (0x1 << port);
		fprintf(stdout, "bt1120 g_MergeEn ; [%x]\n\n", g_MergeEn);
	}

	/********************************
	 * Mux YC merge
	 ********************************/
	pPortFmt->merge = g_MergeEn;
	pPortFmt->devnum = devnum;
	NC_VD_VO_Port_Format_Set(pPortFmt);

	/* Output Port Data Ch Number Selection */
	VOutPortMuxMode.devnum = devnum;
	VOutPortMuxMode.Port = port;
	switch(mux_mode)
	{
		case VI_WORK_MODE_1Multiplex :  VOutPortMuxMode.Mode = 0; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_WORK_MODE_2Multiplex :	VOutPortMuxMode.Mode = 0x02; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_WORK_MODE_4Multiplex :  VOutPortMuxMode.Mode = 0x08; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
	}
	NC_VD_VO_Port_Mux_Mode_Set(&VOutPortMuxMode);
	NC_VD_VO_Port_Mux_ChId_Set(&VoutChID);

	/********************************
	 * Video Output Clock and delay
	 ********************************/
	if(output_interface == NC_OI_BT656)
	{
		VOutClk.Port = port;	// PORT_A, PORTB, PORTC, PORTD
		VOutClk.devnum = devnum;
		VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
		NC_VD_CLK_VO_AUTO_Set(&VOutClk);
	}
	else// BT1120
	{
		switch(port)
		{
			case PORTA :
				VOutClk.Port = PORTA;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				VOutClk.Port = PORTB;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				break;
			case PORTC :
				VOutClk.Port = PORTC;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				VOutClk.Port = PORTD;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				break;
		}
	}

	/********************************
	 * Output Port CLK Enable
	 ********************************/
	sPortEnable.Dev_Num = devnum;
	if(mux_mode == VI_WORK_MODE_2Multiplex)
	{
		sPortEnable.enable = 0x66;
	}
	else
	{
		sPortEnable.enable = 0xff;
	}
	NC_VD_VO_Port_Enable(&sPortEnable);
}


void NC_VD_VO_Mode_2Mux_Set( unsigned char devnum, NC_PORT port, NC_VO_PORT_FMT_S *pPortFmt,
		NC_OUTPUT_MUX_MODE mux_mode,
		NC_OUTPUT_INTERFACE output_interface,
		NC_OUTPUT_EDGE edge,
		NC_CH SEQ0, NC_CH SEQ1, NC_CH SEQ2, NC_CH SEQ3)
{
	unsigned char ClockDelay;
	NC_VD_CLK_VO_STR VOutClk;
	NC_VD_VO_SEQ_STR VOutSeq;
	NC_VD_VO_PORT_MUX_CHID_STR VoutChID;
	NC_VD_VO_PORT_MUX_MODE_STR VOutPortMuxMode;
	NC_VD_DEV_PORT_ENABLE_STR sPortEnable;

	VOutSeq.devnum = devnum;
	VoutChID.devnum = devnum;


	_CHSEQ[0] = SEQ0;
	_CHSEQ[1] = SEQ1;
	_CHSEQ[2] = SEQ2;
	_CHSEQ[3] = SEQ3;

#ifdef __DEBUG_MSG
	printf("Name        = %s\n", pPortFmt->name);
	printf("width       = %d\n", pPortFmt->width);
	printf("height      = %d\n", pPortFmt->height);
	printf("vo_clk;     = 0x%x\n", pPortFmt->vo_clk);
	printf("merge;      = 0x%x\n", pPortFmt->merge);
	printf("mux_mode;   = 0x%x\n", pPortFmt->mux_mode);
	printf("seq_ch01;   = 0x%x\n", pPortFmt->seq_ch01);
	printf("seq_ch23;   = 0x%x\n", pPortFmt->seq_ch23);
	printf("chid_vin01; = 0x%x\n", pPortFmt->chid_vin01);
	printf("chid_vin23; = 0x%x\n", pPortFmt->chid_vin23);
#endif

	// Output Interface Setting

	if(output_interface == NC_OI_BT656) // BT656 Port Setting
	{
		if(mux_mode == VI_WORK_MODE_2Multiplex)
		{
			if(port == PORTB)
			{
				VOutSeq.Port = port;
				VOutSeq.ChNum01 = ((0x00 & 0x0f) | ((0x01 & 0xf)<<4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
				VOutSeq.ChNum23 = ((0x00 & 0x0f) | ((0x01 & 0xf)<<4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			}
			else if(port == PORTC)
			{
				VOutSeq.Port = port;
				VOutSeq.ChNum01 = ((0x02 & 0x0f) | ((0x03 & 0xf)<<4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
				VOutSeq.ChNum23 = ((0x02 & 0x0f) | ((0x03 & 0xf)<<4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			}

			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

		}
		else
		{
			VOutSeq.Port = port;
			VOutSeq.ChNum01 = ((SEQ0 & 0x0f) | ((SEQ1 & 0xf)<<4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = ((SEQ2 & 0x0f) | ((SEQ3 & 0xf)<<4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			if((strcmp(pPortFmt->name, "AHD30_3M_30P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_3M_25P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_4M_30P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_4M_25P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_5M_20P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_5_3M_20P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_6M_18P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_6M_20P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_8M_12_5P") == 0) ||
					(strcmp(pPortFmt->name, "AHD30_8M_15P") == 0) ||
					(strcmp(pPortFmt->name, "TVI_4M_30P") == 0) ||
					(strcmp(pPortFmt->name, "TVI_4M_25P") == 0) ||
					(strcmp(pPortFmt->name, "TVI_5M_20P") == 0) ||
					(strcmp(pPortFmt->name, "CVI_4M_25P") == 0) ||
					(strcmp(pPortFmt->name, "CVI_4M_30P") == 0) ||
					(strcmp(pPortFmt->name, "CVI_8M_15P") == 0) ||
					(strcmp(pPortFmt->name, "CVI_8M_12_5P") == 0) ||
					(strcmp(pPortFmt->name, "TVI_8M_15P") == 0) ||
					(strcmp(pPortFmt->name, "TVI_8M_12_5P") == 0)
			  )
				g_MergeEn |= (0x1 << port);
			else
				g_MergeEn &= (~(0x1 << port));

			fprintf(stdout, "bt656 g_MergeEn ; [%x]\n\n", g_MergeEn);
		}
	}
	else // BT1120 Port Setting
	{
		/*
		   0~3  :  Normal Display of Channel 1~4
		   4~7  :  Only Y Display of Channel 1~4
		   8~B  :  H_CIF Display of Channel 1~4
		   C~F  :  Only C Display of Channel 1~4
		   */
		if(port == PORTA)
		{
			//Y Only Set for BT1120
			VOutSeq.Port = PORTB;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.Port = PORTA;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);
		}
		else if(port == PORTC)
		{
			VOutSeq.Port = PORTD;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.Port = PORTC;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ChNum01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ChNum23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			NC_VD_VO_CHN_Sequence_Set(&VOutSeq);
		}
		else
		{

		}

		g_MergeEn |= (0x1 << port);
		fprintf(stdout,"bt1120 g_MergeEn ; [%x]\n\n", g_MergeEn);
	}

	pPortFmt->merge = g_MergeEn;
	pPortFmt->devnum = devnum;
	NC_VD_VO_Port_Format_Set(pPortFmt);

	VOutPortMuxMode.devnum = devnum;
	VOutPortMuxMode.Port = port;

	switch(mux_mode)
	{
		case VI_WORK_MODE_1Multiplex :  VOutPortMuxMode.Mode = 0; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_WORK_MODE_2Multiplex :	VOutPortMuxMode.Mode = 0x02; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_WORK_MODE_4Multiplex :  VOutPortMuxMode.Mode = 0x08; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
										//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
	}

	NC_VD_VO_Port_Mux_Mode_Set(&VOutPortMuxMode);
	NC_VD_VO_Port_Mux_ChId_Set(&VoutChID);

	if(output_interface == NC_OI_BT656)
	{
		VOutClk.Port = port;	// PORT_A, PORTB, PORTC, PORTD
		VOutClk.devnum = devnum;
		VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
		NC_VD_CLK_VO_AUTO_Set(&VOutClk);
	}
	else	// BT1120
	{
		switch(port)
		{
			case PORTA :
				VOutClk.Port = PORTA;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				VOutClk.Port = PORTB;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				break;
			case PORTC :
				VOutClk.Port = PORTC;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				VOutClk.Port = PORTD;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				NC_VD_CLK_VO_AUTO_Set(&VOutClk);
				break;
		}
	}

	sPortEnable.Dev_Num = devnum;

	if(mux_mode == VI_WORK_MODE_2Multiplex)
		sPortEnable.enable = 0x66;
	else if(mux_mode == VI_WORK_MODE_1Multiplex)
		sPortEnable.enable = 0xff;

	NC_VD_VO_Port_Enable(&sPortEnable);
}


