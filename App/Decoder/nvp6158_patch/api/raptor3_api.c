/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: Decoder api
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
#include <time.h>
#include <semaphore.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "video_decoder.h"
#include "clock.h"
#include "video_output.h"
#include "video_auto_detect.h"
#include "coax.h"
#include "coax_cmd_def.h"
#include "motion.h"
#include "audio.h"
#include "raptor3_api.h"
#include "raptor3_api_drv.h"

#include "hi_comm_vi.h"

int soc_vi_1mux[4] = {0, 4, 8, 12};
int soc_vi_2mux[8] = {0, 2, 4, 6, 8, 10, 12, 14};
int soc_vi_ch[16];

unsigned int gCoaxFirmUpdateFlag[16];

int RAPTOR3_LoadDriver(void)
{
	NC_VD_Driver_Init();
	return 0;
}

#define DEF_VI_CH_MAX (32)
typedef struct NC_HI_VI_STAT_S
{
	int status;

	int vd_dev;
	int vd_ch;

	NC_VI_CH_FMT_S *pChFmt;

} NC_HI_VI_STAT_S;

static NC_HI_VI_STAT_S  NCHI_VI_ChannelStatus[ DEF_VI_CH_MAX ];

static NC_HI_VI_STAT_S *NC_HI_VI_Channel_GetStatus( unsigned char ViChn )
{
	return &NCHI_VI_ChannelStatus[ ViChn ];
}

void RAPTOR3_VI_Channel_SetStatus( unsigned char ViChn, NC_VI_CH_FMT_S* pChFmt )
{
	NCHI_VI_ChannelStatus[ ViChn ].pChFmt = pChFmt;
	return;
}

char CoaxRead[1024] = "No Data";
char CoaxRxLBank[1024] = "Bank [0x00]";
char CoaxRxLAddr[1024] = "Addr [0x00]";
char CoaxRxLine1[1024] = "Rx Line1";
char CoaxRxLine2[1024] = "Rx Line2";
char CoaxRxLine3[1024] = "Rx Line3";
char CoaxRxLine4[1024] = "Rx Line4";
char CoaxRxLine5[1024] = "Rx Line5";
char CoaxRxLine6[1024] = "Rx Line6";
char CoaxRxLine7[1024] = "Rx Line7";
char CoaxRxLine8[1024] = "Rx Line8";

char FrimUP_Status[1024];

char CoaxRx[30][128] = {0, };


void RAPTOR3_KH_8M_X_30P_Rx_Test_Set( int ch )
{
	// Bank 0
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x08 + ch, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x58 + ch, 0x80);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x81 + ch, 0x0c);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x85 + ch, 0x04);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0xa0 + ch, 0x03);

	// Bank 1
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x84 + ch, 0x04);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x8C + ch, 0x44);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xCC + ch, 0x60);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xED, ch);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x7A, ch);

	// Bank 5
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x01, 0x62);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x69, 0x10);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xD1, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6E, 0x10);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6F, 0x50);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xB7, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xBB, 0x07);

	// Bank 9
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x50 + (ch * 4), 0x86);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x51 + (ch * 4), 0xb5);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x52 + (ch * 4), 0x6f);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x53 + (ch * 4), 0x48);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x44, ch);

	// Bank 11
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x00 + (ch * 0x20), 0x0F);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x02 + (ch * 0x20), 0x20);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x03 + (ch * 0x20), 0x07);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x04 + (ch * 0x20), 0x80);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x05 + (ch * 0x20), 0x08);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x06 + (ch * 0x20), 0x98);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x07 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x08 + (ch * 0x20), 0x90);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0a + (ch * 0x20), 0x08);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0b + (ch * 0x20), 0x70);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0c + (ch * 0x20), 0x08);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0d + (ch * 0x20), 0xca);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x10 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x11 + (ch * 0x20), 0x80);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x12 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x13 + (ch * 0x20), 0x70);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x14 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x15 + (ch * 0x20), 0x30);

	printf("RAPTOR3_SK_6158C_Port_change_Set::Error\n");

}

void RAPTOR3_KH_8M_X_25P_Rx_Test_Set( int ch )
{
	// Bank 0
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x08 + ch, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x58 + ch, 0x80);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x81 + ch, 0x0D);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x85 + ch, 0x04);
	NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0xa0 + ch, 0x03);

	// Bank 1
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x84 + ch, 0x04);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x8C + ch, 0x44);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xCC + ch, 0x60);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xED, ch);
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x7A, ch);

	// Bank 5
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x01, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x69, 0x10);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xD1, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6E, 0x10);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6F, 0x50);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xB7, 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xBB, 0x07);

	// Bank 9
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x50 + (ch * 4), 0x07);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x51 + (ch * 4), 0x1f);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x52 + (ch * 4), 0x7c);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x53 + (ch * 4), 0x48);
	NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x44, ch);

	// Bank 11
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x00 + (ch * 0x20), 0x0F);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x02 + (ch * 0x20), 0x8C);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x03 + (ch * 0x20), 0x07);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x04 + (ch * 0x20), 0x80);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x05 + (ch * 0x20), 0x0A);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x06 + (ch * 0x20), 0x50);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x07 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x08 + (ch * 0x20), 0x78);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0a + (ch * 0x20), 0x08);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0b + (ch * 0x20), 0x70);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0c + (ch * 0x20), 0x08);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x0d + (ch * 0x20), 0xca);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x10 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x11 + (ch * 0x20), 0x80);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x12 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x13 + (ch * 0x20), 0x70);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x14 + (ch * 0x20), 0x00);
	NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x15 + (ch * 0x20), 0x70);

	printf("RAPTOR3_SK_6158C_Port_change_Set::Error\n");

}

/*============================================================================================================================
 * Coaxial Protocol F/W Upgrade
 ============================================================================================================================*/


int RAPTOR3_Coax_FW_Upgrade_Init(VDEC_DEV_S *pVdevDev, int vi_ch)
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	int FileCnt = 0;

	FileCnt = NC_APP_VD_COAX_FirmUP_File_Count_Get("/mnt/usb");

	printf("[%s::%d]Coax_FirmUP update file count :: %d\n", __FILE__, __LINE__, FileCnt);

	return FileCnt;
}



/**************************************************************************************
 * @desc
 * 	RAPTOR3's The file name of the selected number in the firmware update file list
 * 	          Use to output the upgrade file name to the UI
 *
 * @param_in		(VDEC_DEV_S *)pVdevDev				            Chip information structure
 * @param_in		(int)vi_ch		                                Channel number to upgrade firmware
 * @param_in		(int)select		                                File number in the firmware upgrade list
 * @param_out	(char *)pFileName	                            The selected files in the firmware upgrade list
 *
 * @return   	void		       								None
 ***************************************************************************************/
void RAPTOR3_Coax_FW_File_Get(VDEC_DEV_S *pVdevDev, int vi_ch, char *pFileName, int select)
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S *pChFmt   = pChStat->pChFmt;

	//	int vd_dev = pChStat->vd_dev;
	//	int vd_ch  = pChStat->vd_ch;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_FirmUP_File_Select_Get( pFileName, select );
}

/**************************************************************************************
 * @desc
 * 	RAPTOR3's Start the firmware upgrade.
 *
 * @param_in		(VDEC_DEV_S *)pVdevDev				            Chip information structure
 * @param_in		(int)vi_ch		                                Channel number to upgrade firmware
 * @param_in		(int)FileSel		                            File number to firmware upgrade
 *
 * @return   	void		       								None
 ***************************************************************************************/
void RAPTOR3_Coax_FW_Upadate_Start(VDEC_DEV_S *pVdevDev, int vi_ch, int FileSel)
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	//	int vd_dev = pChStat->vd_dev;
	//	int vd_ch = pChStat->vd_ch;

	printf("[%s::%d]Coax_FirmUP Upgrade Start\n", __func__, __LINE__);

	NC_APP_VD_COAX_FirmUP_CoaxFWUpdate_Start(vi_ch, FileSel, format_standard );

}

/**************************************************************************************
 * @desc
 * 	RAPTOR3's Start the firmware upgrade.
 *
 * @param_in		void                                            None
 *
 * @return   	int  		       								0. Outputs the status of the upgrade as a character in the UI.
 *                                                               1. Starts the progress bar update of the UI.
 *                                                               2. Exit the firmware upgrade screen.
 ***************************************************************************************/
int RAPTOR3_Coax_FW_Update_Status_Get( void )
{
	int progress_flag = 0;
	progress_flag = NC_APP_VD_COAX_FirmUP_Status_Get(FrimUP_Status);

	return progress_flag;
}


int RAPTOR3_Coax_Test_Addr_Val_Get(VDEC_DEV_S *pVdevDev,int slave_addr, int bank, int addr )
{
	int read_val = 0;
	read_val = NC_APP_VD_COAX_Test_Val_Get( slave_addr, bank, addr);
	printf("RAPTOR3_Coax_Test_Addr_Val_Get::%d\n", read_val);
	return read_val;
}

/////////////////////////////////////// AUDIO ///////////////////////////////////

#include <raptor3_audio.h>

void RAPTOR3_Audio_Default_set( void )
{
	unsigned char devnum = 0;
	NC_VD_AUDIO_STR AudioSet;

	NC_VD_AUDIO_DEFAULT_SET(&AudioSet);
}

/********************************************************
 *														*
 *			 RAPTOR3 Motion Detection Function			*
 *			 											*
 ********************************************************/

int RAPTOR3_Motion_Detection_Get(VDEC_DEV_S *pVdevDev, unsigned char ch)
{
	NC_VD_MOTION_STR Motion_Set;

	Motion_Set.Ch  = ch%4;
	Motion_Set.devnum = ch/4;
	NC_VD_MOTION_Detection_Get(&Motion_Set);

	return Motion_Set.Set;
}

void RAPTOR3_Motion_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef)
{
	NC_VD_MOTION_STR Motion_Set;

	Motion_Set.Ch  = ch%4;
	Motion_Set.devnum = ch/4;
	Motion_Set.Set = sel;
	Motion_Set.FmtDef = fmtdef;
	NC_VD_MOTION_OnOff_Set(Motion_Set);
}

void RAPTOR3_Motion_Pixel_All_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef)
{
	NC_VD_MOTION_STR Motion_Pixel_Set;

	Motion_Pixel_Set.Ch  = ch%4;
	Motion_Pixel_Set.devnum = ch/4;
	Motion_Pixel_Set.Set = sel;

	Motion_Pixel_Set.FmtDef = fmtdef;
	NC_VD_MOTION_Pixel_All_OnOff_Set(Motion_Pixel_Set);
}

void RAPTOR3_Motion_Pixel_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef)
{
	NC_VD_MOTION_STR Motion_Pixel_Set;

	Motion_Pixel_Set.Ch  = ch%4;
	Motion_Pixel_Set.devnum = ch/4;
	Motion_Pixel_Set.Set = sel;

	Motion_Pixel_Set.FmtDef = fmtdef;
	NC_VD_MOTION_Pixel_OnOff_Set(Motion_Pixel_Set);
}

unsigned char RAPTOR3_Motion_Pixel_OnOff_get(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef)
{
	NC_VD_MOTION_STR Motion_Pixel_Get;

	Motion_Pixel_Get.Ch  = ch%4;
	Motion_Pixel_Get.devnum = ch/4;
	Motion_Pixel_Get.Set = sel;

	Motion_Pixel_Get.FmtDef = fmtdef;
	NC_VD_MOTION_Pixel_OnOff_Get(&Motion_Pixel_Get);

	return Motion_Pixel_Get.Set;
}

void RAPTOR3_Motion_TSEN_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef)
{
	NC_VD_MOTION_STR Motion_TSEN_Set;

	Motion_TSEN_Set.Ch  = ch%4;
	Motion_TSEN_Set.devnum = ch/4;
	Motion_TSEN_Set.Set = sel;

	Motion_TSEN_Set.FmtDef = fmtdef;
	NC_VD_MOTION_TSEN_Set(Motion_TSEN_Set);
}

void RAPTOR3_Motion_PSEN_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef)
{
	NC_VD_MOTION_STR Motion_PSEN_Set;

	Motion_PSEN_Set.Ch  = ch%4;
	Motion_PSEN_Set.devnum = ch/4;
	Motion_PSEN_Set.Set = sel;

	Motion_PSEN_Set.FmtDef = fmtdef;
	NC_VD_MOTION_PSEN_Set(Motion_PSEN_Set);
}

/////////////////////////////////////////////////////////////////////////////////////

/**************************************************************************************
 * @desc
 * 	A function for reading chip information of RAPTOR3.
 *
 * @param_out	(VDEC_DEV_INFORM_S *)ChipInform				The structure parameters to store the Chip ID, Chip Revision, and Chip Address of the Decoder.
 *
 * @return   	void		       							None
 ***************************************************************************************/
void RAPTOR3_Chip_Information_Get(VDEC_DEV_INFORM_S *ChipInform)
{
	NC_VD_CHIP_INFORMATION_GET(ChipInform);
}



int RAPTOR3_SAL_Init( RAPTOR3_INFORMATION_S *pRaptorinformation )
{
	int i;
	int oLogicalChannel;
	int oDevNum;
	VDEC_DEV_INFORM_S chipinform;

	/* Initialize VideoDecoder (RAPTOR3) */
	RAPTOR3_LoadDriver();

	/* Chip Information Read (Chip ID, Chip Rev) */
	RAPTOR3_Chip_Information_Get( &pRaptorinformation->chipinform );
	memcpy( &chipinform, &pRaptorinformation->chipinform, sizeof(VDEC_DEV_INFORM_S));

	for( i = 0; i < 4; i++ )
	{
		fprintf( stdout, "[%d] CHIPID:0x%02x, CHIP_REV:0x%02x, CHIP_ADDR:0x%02x\n", \
				i, chipinform.chip_id[i], chipinform.chip_rev[i], chipinform.chip_addr[i] );
	}
	fprintf( stdout, "Total_Chip_Cnt : %d\n", chipinform.Total_Chip_Cnt );

	/* set auto mode, but if you want to change mode from auto to manual mode, check this!(2017-07-30) */
	for( i = 0; i < ( chipinform.Total_Chip_Cnt*4); i++ )
	{
		oLogicalChannel = i;
		oDevNum			= i/4;

		NC_VD_AUTO_AutoMode_Set( oLogicalChannel, oDevNum );
	}

	/* set audio(default) */
	RAPTOR3_Audio_Default_set();

	/* set mux according to the CHIPID */
	for( i = 0; i < chipinform.Total_Chip_Cnt; i++ )
	{
		if( pRaptorinformation->chipinform.chip_id[i] == RAPTOR3_2PORT_R0_ID )
		{
			pRaptorinformation->oMux = VI_2MULTIPLEX_MODE;
			fprintf( stdout, ">>>>> RAPTOR3_2PORT_R0_ID\n");
		}
		else
		{
			pRaptorinformation->oMux = VI_1MULTIPLEX_MODE;
			fprintf( stdout, ">>>>> RAPTOR3_4PORT_R0_ID\n");
		}
	}

	return 0;
}

/*******************************************************************************
 *	Description		: debounce function
 *	Argurments		: ch(channel), value(current translated video format)
 *	Return value	: NC_VIVO_CH_FORMATDEF_UNKNOWN(NO video), retfmt(get video format)
 *	Modify			:
 *	warning			:
 *******************************************************************************/
unsigned char __video_fmt_debounce( unsigned char ch, unsigned char value )
{
	int i;
	unsigned char retfmt;
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VIVO_CH_FORMATDEF debouncebuf[8]={0,0,0,0,0,0,0,0};

	for(i=0; i<8; i++)
	{
		sVFC.Ch = ch%4;
		sVFC.Dev_Num = ch/4;
		NC_VD_AUTO_VFC_Get(&sVFC);
		debouncebuf[i] = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		usleep(30 * 1000);
	}

	/* if The value of three buffers are same, return retfmt */
	retfmt = value;

	if((retfmt == debouncebuf[0])&&(retfmt == debouncebuf[1])&&(retfmt == debouncebuf[2])&&(retfmt == debouncebuf[3])&&
	   (retfmt == debouncebuf[4])&&(retfmt == debouncebuf[5])&&(retfmt == debouncebuf[6])&&(retfmt == debouncebuf[7]))
    {
		return retfmt;
    }
	else
	{
		return NC_VIVO_CH_FORMATDEF_UNKNOWN;
	}
}

unsigned char __video_fmt_bank5_debounce( unsigned char ch, unsigned char value )
{
	int i;
	unsigned char retfmt;
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VIVO_CH_FORMATDEF debouncebuf[8]={0,0,0,0,0,0,0,0};

	for(i=0; i<8; i++)
	{
		sVFC.Ch = ch%4;
		sVFC.Dev_Num = ch/4;
		NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);
		debouncebuf[i] = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		usleep(30 * 1000);
	}

	/* if The value of three buffers are same, return retfmt */
	retfmt = value;

	if((retfmt == debouncebuf[0])&&(retfmt == debouncebuf[1])&&(retfmt == debouncebuf[2])&&(retfmt == debouncebuf[3])&&
	   (retfmt == debouncebuf[4])&&(retfmt == debouncebuf[5])&&(retfmt == debouncebuf[6])&&(retfmt == debouncebuf[7]))
    {
		return retfmt;
    }
	else if((retfmt == debouncebuf[0])&&(retfmt == debouncebuf[1])&&(retfmt == debouncebuf[2])&&(retfmt == debouncebuf[3])&&
			(retfmt == debouncebuf[4])&&(retfmt == debouncebuf[5])&&(retfmt == debouncebuf[6])&&(retfmt == debouncebuf[7]))
	{
		return NC_VIVO_CH_FORMATDEF_UNKNOWN;
	}
	else
	{
		return 0xFF /*N/A*/ ;
	}
}

/*******************************************************************************
 *	Description		: Get video format(each ch)
 *	Argurments		: pVFmtSet(raptor3 information)
 *	Return value	: N/A
 *	Modify			:
 *	warning			:
 *******************************************************************************/
int RAPTOR3_SAL_GetFormatEachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VD_AUTO_NOVIDEO_STR sNoVideo;
	NC_VIVO_CH_FORMATDEF oCurVidFmt;
	NC_VIDEO_ONOFF oCurVideoloss;

	/* initialize current video format - pInformation structure is for app */
	pInformation->curvideofmt[ch] = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	pInformation->curvideoloss[ch] = VIDEO_LOSS_OFF;
	pInformation->vfc[ch] = 0xff;

	/* initialize vfc(B5xF0) and videoloss information(B0xA8) */
	sVFC.Ch = ch%4;
	sVFC.Dev_Num = ch/4;
	sNoVideo.Ch = ch%4;
	sNoVideo.Dev_Num = ch/4;

	/* get vfc and videoloss */
	NC_VD_AUTO_VFC_Get(&sVFC);
	NC_VD_AUTO_NOVIDEO_Get(&sNoVideo);

	/* check vfc&videoloss and run debounce  */
	if(((((sVFC.VFC >> 4 ) & 0xF) != 0xF) && ((sVFC.VFC & 0x0F) != 0xF)) && !sNoVideo.NoVid) // OnVideo
	{
		/* convert vfc to formatDefine for APP and save videoloss information */
		oCurVidFmt = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		/* debouce */
		pInformation->curvideofmt[ch] = oCurVidFmt;
		pInformation->vfc[ch] = sVFC.VFC;
	}

	/* check novideo option */
	if( !sNoVideo.NoVid )
	{
		pInformation->curvideoloss[ch] = VIDEO_LOSS_ON;
	}

	return 0;
}

/*******************************************************************************
 *	Description		: Get video format bank13(each ch)
 *	Argurments		: pVFmtSet(raptor3 information)
 *	Return value	: N/A
 *	Modify			:
 *	warning			:
 *******************************************************************************/
int RAPTOR3_SAL_GetFormat_Bank5_EachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VD_AUTO_NOVIDEO_STR sNoVideo;
	NC_VIVO_CH_FORMATDEF oCurVidFmt;
	NC_VIDEO_ONOFF oCurVideoloss;

	/* initialize current video format - pInformation structure is for app */
	pInformation->curvideofmt[ch] = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	pInformation->curvideoloss[ch] = VIDEO_LOSS_OFF;
	pInformation->vfc[ch] = 0xff;

	/* initialize vfc(B5xF0) and videoloss information(B0xA8) */
	sVFC.Ch = ch%4;
	sVFC.Dev_Num = ch/4;
	sNoVideo.Ch = ch%4;
	sNoVideo.Dev_Num = ch/4;

	/* get vfc and videoloss */
	NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);
	NC_VD_AUTO_NOVIDEO_Get(&sNoVideo);

	/* check vfc&videoloss and run debounce  */
	if(((((sVFC.VFC >> 4 ) & 0xF) != 0xF) && ((sVFC.VFC & 0x0F) != 0xF)) && !sNoVideo.NoVid) // OnVideo
	{
		/* convert vfc to formatDefine for APP and save videoloss information */
		oCurVidFmt = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		/* debouce */
		pInformation->curvideofmt[ch] = __video_fmt_bank5_debounce( ch, oCurVidFmt );
		pInformation->vfc[ch] = sVFC.VFC;
	}

	/* check novideo option */
	if( !sNoVideo.NoVid )
	{
		pInformation->curvideoloss[ch] = VIDEO_LOSS_ON;
	}

	return 0;
}

/*******************************************************************************
 *	Description		: check Auto Debouce
 *	Argurments		: ch:channel, pInformation(raptor3 information), videomode(0:novideo, 1:Onvideo)
 *	Return value	: 0(not checked video format), -1(checked video format)
 *	Modify			: N/A
 *	warning			:
 *******************************************************************************/
int RAPTOR3_SAL_AutoDebouceCheck( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	int i;
	int ret = 0;
	unsigned char		oDevNum = 0;
	unsigned char		oDebncIdx = 0;
	unsigned char 		oVfc = 0;
	NC_VIVO_CH_FORMATDEF oFmtB5Def;
	NC_VD_AUTO_VFC_STR	sVFC;
	NC_VD_DEV_CH_INFO_STR sDevChInfo;


	sVFC.Ch = ch % 4;
	sVFC.Dev_Num = ch / 4;
	NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);

	oDebncIdx = pInformation->debounceidx[ch];
	pInformation->debounce[ch][oDebncIdx%MAX_DEBOUNCE_CNT] = sVFC.VFC;

	pInformation->debounceidx[ch]++;
	pInformation->debounceidx[ch] = ( (pInformation->debounceidx[ch] % MAX_DEBOUNCE_CNT) == 0 ) ? 0 : pInformation->debounceidx[ch];

	oVfc = pInformation->debounce[ch][pInformation->debounceidx[ch]];
	for( i = 0; i < MAX_DEBOUNCE_CNT; i++ )
	{
		if( oVfc != pInformation->debounce[ch][i])
		{
			break;
		}
	}
	if( i == MAX_DEBOUNCE_CNT )
	{
		oFmtB5Def = NC_VD_AUTO_VFCtoFMTDEF(oVfc);

			if( (
					( oFmtB5Def != AHD30_5M_20P )  &&
					( oFmtB5Def != CVI_8M_15P )    &&
					( oFmtB5Def != CVI_8M_12_5P )  &&
					( oFmtB5Def != CVI_HD_30P_EX ) &&
					( oFmtB5Def != CVI_4M_25P)  &&
					( oFmtB5Def != CVI_FHD_30P )
				)
					&&
					( oFmtB5Def != pInformation->prevideofmt[ch] )
			   )
		{
			ret = -1;
		}
	}

	return ret;
}

/*******************************************************************************
 *	Description		: set video format each channel
 *	Argurments		: ch:channel, pInformation(raptor3 information), videomode(0:novideo, 1:Onvideo)
 *	Return value	: NC_VIVO_CH_FORMATDEF_UNKNOWN(NO video), retfmt(get video format)
 *	Modify			: N/A
 *	warning			:
 *******************************************************************************/
int RAPTOR3_SAL_NoVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	NC_VO_PORT_FMT_S *pPortFmt;

	unsigned char oChannel;
	unsigned char devnum;
	unsigned char iPort;
	unsigned char oMux;
	unsigned char oInterface;
	unsigned char oClkEdge;
	NC_VD_DEV_CH_INFO_STR DEV_CH_INFO;

	NC_VIVO_CH_FORMATDEF oFmtDef = pInformation->curvideofmt[ch];

	if(pInformation->oMux == VI_WORK_MODE_2Multiplex)
	{
		oChannel = ch % 4;
		devnum = ch / 4;
		iPort = (( ch % 4 ) / 2) + 1;

		oMux = VI_WORK_MODE_2Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_DOUBLE;
	}
	else
	{
		oChannel = ch % 4;
		devnum = ch / 4;
		iPort = ch % 4;

		oMux = VI_WORK_MODE_1Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_SINGLE_UP;
	}

	DEV_CH_INFO.Ch = oChannel;
	DEV_CH_INFO.Dev_Num = devnum;
	DEV_CH_INFO.Fmt_Def = oFmtDef;

	/* This Part is enough EQ Setting */
	NC_VD_VI_H_Timing_Set(&DEV_CH_INFO);
	NC_VD_VI_COLOR_Set(&DEV_CH_INFO);
	NC_VD_VI_VAFE_Init(&DEV_CH_INFO);

	NC_VD_AUTO_NoVideo_Set(oChannel, devnum);

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( oFmtDef );
	NC_VD_VO_Mode_Set_New( oChannel, devnum, iPort, pPortFmt, oMux, oInterface, oClkEdge, ch%4, ch%4, ch%4, ch%4 );

	return 0;
}

/*******************************************************************************
 *	Description		: set video format each channel
 *	Argurments		: ch:channel, pInformation(raptor3 information), videomode(0:novideo, 1:Onvideo)
 *	Return value	: NC_VIVO_CH_FORMATDEF_UNKNOWN(NO video), retfmt(get video format)
 *	Modify			:
 *	warning			:
 *******************************************************************************/
int RAPTOR3_SAL_OnVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	int iPort = 0;
	unsigned char oChannel = 0x00;
	unsigned char oDevNum = 0x00;
	unsigned char oEdge = 0x00;
	unsigned char oMux;
	unsigned char oInterface;
	unsigned char oClkEdge;


	NC_VO_PORT_FMT_S *pPortFmt;		// output
	NC_VD_DEV_CH_INFO_STR DEV_CH_INFO;

	NC_VIVO_CH_FORMATDEF FmtDef = pInformation->curvideofmt[ch];

	if(pInformation->oMux == VI_WORK_MODE_2Multiplex)
	{
		iPort = (( ch % 4 ) / 2) + 1;
		oChannel = ch%4;
		oDevNum = ch/4;

		oMux = VI_WORK_MODE_2Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_DOUBLE;
	}
	else
	{
		iPort = ch%4;
		oChannel = ch%4;
		oDevNum = ch/4;

		oMux = VI_WORK_MODE_1Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_SINGLE_UP;
	}

	/* Actually, Now, this routine don't use, because the NC_VD_VO_Mode_Set calculate Edge */
	if(FmtDef == AHD30_4M_30P || \
			FmtDef == AHD30_4M_25P || \
			FmtDef == AHD30_3M_30P || \
			FmtDef == AHD30_3M_25P || \
			FmtDef == AHD30_5M_20P || \
			FmtDef == AHD30_5_3M_20P || \
			FmtDef == AHD30_6M_18P || \
			FmtDef == AHD30_6M_20P || \
			FmtDef == AHD30_8M_15P || \
			FmtDef == AHD30_8M_12_5P || \
			FmtDef == CVI_4M_25P || \
			FmtDef == CVI_4M_30P || \
			FmtDef == CVI_8M_12_5P || \
			FmtDef == CVI_8M_15P || \
			FmtDef == TVI_8M_12_5P || \
			FmtDef == TVI_8M_15P || \
			FmtDef == TVI_4M_25P || \
			FmtDef == TVI_4M_30P || \
			FmtDef == TVI_5M_20P
			)
	{
		if(oMux == VI_WORK_MODE_2Multiplex)
		{
			fprintf(stdout, " Not Support This Format .. High/RT Fmt:[%d]\n", FmtDef);
		}
		else
		{
			oEdge = VI_CLK_EDGE_DOUBLE;
		}
	}
	else
	{
		if(oMux == VI_WORK_MODE_2Multiplex)
		{
			oClkEdge = VI_CLK_EDGE_DOUBLE;
		}
		else
		{
			oClkEdge = VI_CLK_EDGE_SINGLE_UP;
		}
	}

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( FmtDef ); // output

	DEV_CH_INFO.Ch = oChannel;
	DEV_CH_INFO.Dev_Num = oDevNum;
	DEV_CH_INFO.Fmt_Def = FmtDef;
	NC_VD_AUTO_ONVIDEO_SET(&DEV_CH_INFO);

      /* This Part is enough EQ Setting */
	NC_VD_VI_H_Timing_Set(&DEV_CH_INFO);
	NC_VD_VI_COLOR_Set(&DEV_CH_INFO);
	NC_VD_VI_VAFE_Init(&DEV_CH_INFO);

	NC_VD_VO_Mode_Set_New( oChannel, oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );

	return 0;
}


