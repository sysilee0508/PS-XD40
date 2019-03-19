/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Driver Interface
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

#include <video_decoder.h>
#include "_debug_macro.h"
#include <raptor3_ioctl.h>
#include "coax_firmup.h"
#include "coax.h"
#include "motion.h"
#include "video_input.h"
#include "video_auto_detect.h"
#include "audio.h"

#define DEV_PATH_VDEC			"/dev/raptor3"

int g_fd_raptor3_drv=-1;

static int OpenDeviceVdec( void )
{
	int fd = -1;

	fd = open(DEV_PATH_VDEC, O_RDONLY);

	if( fd < 0 )
	{
		DBG_ERR("VDEC OPEN ERROR!! : %s\n", DEV_PATH_VDEC );
		return fd;
	}

	DBG_INFO("%s\n",DEV_PATH_VDEC);

	return fd;
}

static int CloseDeviceVdec( int fd )
{
	DBG_INFO("%s\n",DEV_PATH_VDEC);

	if(fd>=0)
	{
		close( fd );
		fd = -1;
	}

	return 0;
}

int NC_VD_Driver_Init(void)
{
	g_fd_raptor3_drv = OpenDeviceVdec();

	return 0;
}


void NC_VD_AUTO_Auto_Detection_Set(NC_VD_VI_AUTO_DETECT_REG_STR *VinAutoDet)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_DETECT_MODE_SET, VinAutoDet);
}

void NC_VD_VI_Format_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_FORMAT_SET, Dev_Ch_Info);
}


void NC_VD_VI_Chroma_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_CHROMA_SET, Dev_Ch_Info);
}

void NC_VD_VI_H_Timing_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_H_TIMING_SET, Dev_Ch_Info);
}

void NC_VD_VI_H_Scaler_Mode_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_H_SCALER_MODE_SET, Dev_Ch_Info);
}

void NC_VD_VI_HPLL_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_HPLL_SET, Dev_Ch_Info);

}

void NC_VD_VI_CHID_Set(NC_VD_VI_CHID_STR *VinChid)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_CHID_SET, VinChid);
}

void NC_VD_AUTO_VFC_Get(NC_VD_AUTO_VFC_STR *VinVFC)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_VFC_GET, VinVFC);
}

// 170223 HZOOM SET
void NC_VD_VI_HZOOM_Set(NC_VD_VI_HZOOM_STR *VinHZOOM) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_HZOOM_SET, VinHZOOM);

}

// 170223 AGC SET
void NC_VD_VI_AGC_Set(NC_VD_VI_AGC_STR *VinAGC) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_AGC_SET, VinAGC);

}

// 170223 COMB_TH SET
void NC_VD_VI_COMB_TH_Set(NC_VD_VI_COMB_TH_STR *VinCOMBTH) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_COMB_TH_SET, VinCOMBTH);

}

// 170223 Y_LPF SET
void NC_VD_VI_Y_LPF_Set(NC_VD_VI_YLPF_STR *VinYLPF) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_YLPF_SET, VinYLPF);

}

// 170314
void NC_VD_VI_SYNC_EN_Set(NC_VD_VI_SYNC_EN_STR *VinSYNCEn) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_SYNC_EN_SET, VinSYNCEn);

}

// 170314
void NC_VD_VI_VBLK_END_EXT_Set(NC_VD_VI_VBLK_END_EXT_STR *VinVBLKEnd) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_VBLK_END_EXT_SET, VinVBLKEnd);

}

void NC_VD_VI_YDELAY_Set(NC_VD_VI_FB_YDELAY_STR *VinYDelay) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_YDELAY_SET, VinYDelay);

}

void NC_VD_VI_CDELAY_Set(NC_VD_VI_FB_CDELAY_STR *VinCDelay) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_CDELAY_SET, VinCDelay);
}

void NC_VD_VI_COLOR_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_COLOR_SET, Dev_Ch_Info);

}

void NC_VD_VI_VAFE_Init(NC_VD_DEV_CH_INFO_STR *VinVAFE) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_INPUT_AFE_INIT, VinVAFE);

}


// No Video Read 		
void NC_VD_AUTO_NOVIDEO_Get(NC_VD_AUTO_NOVIDEO_STR *VinNOVID)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_NOVID_GET, VinNOVID);
}


// Auto NO Video Set	
void NC_VD_AUTO_NOVIDEO_Set(NC_VD_AUTO_NOVIDEO_REG_STR *AutoNOVID)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_NOVID_SET, AutoNOVID);
}

// Cable Distance(auto)			
void NC_VD_AUTO_CABLE_DISTANCE_Read(NC_VD_AUTO_CABLE_DIST_STR *VinCableDist )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_CABLE_DIST_GET, VinCableDist);
}

// Cable Distance(manual)
void NC_VD_MANUAL_CABLE_DISTANCE_Read(NC_VD_MANUAL_CABLE_DIST_STR *VinCableDist)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MANUAL_CABLE_DIST_GET, VinCableDist);
}

// SAM Value			
void NC_VD_VI_SAM_VAL_Read( NC_VD_AUTO_SAM_STR *VinSAMVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_SAM_VAL_GET, VinSAMVal);
}

// Hsync Accumulation	
void NC_VD_VI_HSYNC_ACCUM_Read(NC_VD_AUTO_HSYNC_STR *VinHsyncAccum )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_HSYNC_ACCUM_GET, VinHsyncAccum);
}

// AGC Value			
void NC_VD_AUTO_AGC_Read(NC_VD_AUTO_AGC_STR *VinAGCVal )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_AGC_VAL_GET, VinAGCVal);
}


// FSC Value Read	- 170214 Add
void NC_VD_VI_FSC_VAL_Read(NC_VD_AUTO_FSC_STR *VinFSCVal )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_FSC_GET, VinFSCVal);
}

// AEQ Set - 170214 Add
void NC_VD_AUTO_AEQ_Set(NC_VD_VI_AT_AEQ_STR *AEQ)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_AEQ_SET, AEQ);
}

// DEQ Set - 170214 Add
void NC_VD_AUTO_DEQ_Set(NC_VD_VI_AT_DEQ_STR *DEQ)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_DEQ_SET, DEQ);
}

void NC_VD_MANUAL_VIDEO_EQ_Set(NC_VD_EQ_STR *psEQInfo)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MANUAL_EQ_DIST_SET, psEQInfo);
}

// ACC Gain Read - 170215 Add
void NC_VD_AUTO_ACC_GAIN_Read(NC_VD_AUTO_ACC_GAIN_STR *VinACCGain)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_ACC_GAIN_GET, VinACCGain);
};

void NC_VD_AT_DAT_OUT_MODE_Set(NC_VD_AUTO_DATA_OUT_MODE_STR *VoDataOutMode)	// VIDEO OUTPUT ON/OFF , same RAPTOR2 hide/show function
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_DATA_OUT_MODE_SET, VoDataOutMode);
}

void NC_VD_VI_MANUAL_MODE_Set(NC_VD_AUTO_MANUAL_MODE_STR *VinManualMode)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MANUAL_MODE_SET, VinManualMode);
}

void NC_VD_VO_CHN_Sequence_Set(NC_VD_VO_SEQ_STR *VOutSeq)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_SET, VOutSeq);
}

void NC_VD_VO_CHN_Sequence_Get(NC_VD_VO_SEQ_REG_STR *VOutSeqreg)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_GET, VOutSeqreg);
}

void NC_VD_VO_Port_Mux_Mode_Set(NC_VD_VO_PORT_MUX_MODE_STR *VOutPortMuxMode)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_PORT_MUX_MODE_SET, VOutPortMuxMode);
}

void NC_VD_VO_Port_Enable( NC_VD_DEV_PORT_ENABLE_STR *pPortEnable )
//
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_PORT_ENABLE_SET, pPortEnable);
}

void NC_VD_VO_Port_Format_Set(NC_VO_PORT_FMT_S *fmt )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_PORT_FORMAT_SET, fmt);
}

void NC_VD_VO_Port_Mux_ChId_Set(NC_VD_VO_PORT_MUX_CHID_STR *VoutChID)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_PORT_MUX_CHID_SET, VoutChID);
}

void NC_VD_VO_COLORBAR_Set(void)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}

	ioctl( g_fd_raptor3_drv, IOC_VDEC_OUTPUT_COLOR_BAR_SET, NULL);
}

/*================================================================================================================================
 * Coaxial Protocol
 =================================================================================================================================*/
/*================================================================
 * Coaxial UP Stream
 =================================================================*/
void NC_VD_COAX_Tx_INIT(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TX_INIT, CoaxVal );
}

void NC_VD_COAX_Tx_CMD_SEND(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TX_CMD_SEND, CoaxVal );
}

void NC_VD_COAX_Tx_16Bit_INIT(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TX_16BIT_INIT, CoaxVal );
}

void NC_VD_COAX_Tx_16Bit_CMD_SEND(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TX_16BIT_CMD_SEND, CoaxVal );
}

void NC_VD_COAX_Tx_CVI_NEW_CMD_SEND(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TX_CVI_NEW_CMD_SEND, CoaxVal );
}


/*================================================================
 * Coaxial Down Stream
 =================================================================*/
void NC_VD_COAX_Rx_INIT(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_RX_INIT, CoaxVal );
}

void NC_VD_COAX_Rx_DATA_READ(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_RX_DATA_READ, CoaxVal );
}

void NC_VD_COAX_Rx_BUF_CLEAR(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_RX_BUF_CLEAR, CoaxVal );
}

void NC_VD_COAX_Rx_DEINIT(NC_VD_COAX_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_RX_DEINIT, CoaxVal );
}


/*================================================================
 * Coaxial Firmware Update
 =================================================================*/
void NC_VD_COAX_FW_ACP_HEADER_GET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_ACP_HEADER_GET, pstFileInfo );
}

void NC_VD_COAX_FW_READY_CMD_SET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_READY_CMD_SET, pstFileInfo );
}

void NC_VD_COAX_FW_READY_ACK_GET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_READY_ACK_GET, pstFileInfo );
}

void NC_VD_COAX_FW_START_CMD_SET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_START_CMD_SET, pstFileInfo );
}

void NC_VD_COAX_FW_START_ACK_GET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_START_ACK_GET, pstFileInfo );
}

void NC_VD_COAX_FW_ONE_PACKET_SEND_DATA_SET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_SEND_DATA_SET, pstFileInfo );
}

void NC_VD_COAX_FW_ONE_PACKET_SEND_ACK_GET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_SEND_ACK_GET, pstFileInfo );
}


void NC_VD_COAX_FW_END_CMD_SET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_END_CMD_SET, pstFileInfo );
}

void NC_VD_COAX_FW_END_ACK_GET( FIRMWARE_UP_FILE_INFO *pstFileInfo )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_FW_END_ACK_GET, pstFileInfo );
}
/*================================================================
 * Coaxial Option
 =================================================================*/
void NC_VD_OPTION_RT_NRT_MODE_CHANGE_SET( NC_VD_COAX_STR *pParam )
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_RT_NRT_MODE_CHANGE_SET, pParam );
}


/*================================================================
 * Coaxial Test
 =================================================================*/
void NC_VD_COAX_TEST_Tx_INIT_DATA_GET(NC_VD_COAX_TEST_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TEST_TX_INIT_DATA_READ, CoaxVal );
}

/*================================================================
 * Coaxial Bank Dump - Test API
 =================================================================*/
void NC_VD_COAX_BANK_DATA_GET(unsigned char *BankReadval)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_BANK_DUMP_GET, BankReadval );
}
/*================================================================
 * Coaxial Bank Dump - Test API END
 =================================================================*/

void NC_VD_COAX_TEST_DATA_SET(NC_VD_COAX_TEST_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TEST_DATA_SET, CoaxVal );
}

void NC_VD_COAX_TEST_DATA_READ(NC_VD_COAX_TEST_STR *CoaxVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_COAX_TEST_DATA_READ, CoaxVal );
}
/*================================================================
 * Coaxial End
 =================================================================*/

////////////////////// MOTION ///////////////////////////

void NC_VD_MOTION_DETECTION_GET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_DETECTION_GET, MotionVal );

}

void NC_VD_MOTION_DATA_SET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_SET, MotionVal );

}

void NC_VD_MOTION_PIXEL_ALL_DATA_SET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_ALL_PIXEL_SET, MotionVal );
}

void NC_VD_MOTION_PIXEL_DATA_SET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_PIXEL_SET, MotionVal );
}

void NC_VD_MOTION_PIXEL_DATA_GET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_PIXEL_GET, MotionVal );
}
void NC_VD_MOTION_TSEN_DATA_SET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_TSEN_SET, MotionVal );
}

void NC_VD_MOTION_PSEN_DATA_SET(NC_VD_MOTION_STR *MotionVal)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MOTION_PSEN_SET, MotionVal );
}

/////////////////////// AUDIO /////////////////////////

void NC_VD_AUDIO_DEFAULT_SET(NC_VD_AUDIO_STR *AudioSet)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUDIO_DEFAULT_SET, AudioSet );
}

////////////////////// CHIP INFORMATION //////////////////////

void NC_VD_CHIP_INFORMATION_GET(VDEC_DEV_INFORM_S *ChipInform)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_GET_CHIP_ID, ChipInform );
}

void NC_VD_AUTO_ONVIDEO_SET(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_ONVIDEO_SET, Dev_Ch_Info );
}

#if 0
unsigned char NC_VD_AUTO_ONVIDEO_SW_RST_CHECK(NC_VD_AUTO_ONVIDEO_CHECK_STR *SW_Rst)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_ONVIDEO_CHECK, SW_Rst );

	return SW_Rst->SW_Rst;
}
#else
void NC_VD_AUTO_ONVIDEO_CHECK(NC_VD_AUTO_VFC_STR *VinVFC)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AUTO_ONVIDEO_CHECK, VinVFC );
}

#endif



void NC_VD_AUTO_ONVIDEO_RST(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_CH_SW_RESET, Dev_Ch_Info );
}

void NC_VD_AUTO_AFE_RST(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AFE_RESET, Dev_Ch_Info );
}

void NC_VD_AUTO_AFE_UP(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AFE_CONTROL_PU, Dev_Ch_Info );
}

void NC_VD_AUTO_AFE_DOWN(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AFE_CONTROL_PD, Dev_Ch_Info );
}

void NC_VD_MANUAL_ON_OFF_HAFC_CTRL(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_HAFC_GAIN12_CTRL, Dev_Ch_Info );
}

void NC_VD_AUTO_MANUAL_AGC_STABLE_ENABLE(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MANUAL_AGC_STABLE_ENABLE, Dev_Ch_Info );
}

void NC_VD_AUTO_MANUAL_AGC_STABLE_DISABLE(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_MANUAL_AGC_STABLE_DISABLE, Dev_Ch_Info );
}

void NC_VD_AUTO_CONTRAST_OFF(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info)
{
	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}

	ioctl( g_fd_raptor3_drv, IOC_VDEC_CONTRAST_OFF, Dev_Ch_Info );
}

void NC_VD_VI_NEW_FORMAT_Set(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_NEW_FORMAT_SET, Dev_Ch_Info);
}

void NC_VD_VI_AHD_TVI_Ditinguish(NC_VD_DEV_CH_INFO_STR *Dev_Ch_Info) {

	if(g_fd_raptor3_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor3_drv, IOC_VDEC_AHD_TVI_DISTINGUISH, Dev_Ch_Info);
}


