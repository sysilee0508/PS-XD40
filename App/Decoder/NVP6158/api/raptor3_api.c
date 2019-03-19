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

#include "clock.h"
#include "video_output.h"
#include "video_input.h"
#include "video_auto_detect.h"
#include "raptor3_ioctl.h"
#include "coax.h"
#include "coax_cmd_def.h"
#include "motion.h"
#include "audio.h"
#include <raptor3.h>
#include "raptor3_api.h"
#include "_debug_macro.h"
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

int RAPTOR3_InitDevice( VDEC_DEV_S *pVdevDev )
{
	pVdevDev->ch_per_chip = RAPTOR3_MAX_CH_PER_CHIP;
	pVdevDev->port_per_chip = RAPTOR3_MAX_PORT_PER_CHIP;

	// cfgs for detection threads
	int ii;
	for(ii=0;ii<pVdevDev->ch_per_chip;ii++)
	{
		pVdevDev->detect_cfg[ii].ch_in_dev = ii;
		pVdevDev->detect_cfg[ii].pVdevDev = pVdevDev;
	}

	return 0;
}

static void _Video_SetConfig_Lock(VDEC_DEV_S *pVdevDev)
{
	pVdevDev->detection_process_lock = 1;

	int timeout = 0;
	int i = pVdevDev->ch_per_chip;
	 while(i--)
	 {
	  if(pVdevDev->detection_process_inuse[i])
	  {
		  usleep(1000);
		  i++;
		  timeout++;

		  if(timeout >= 1000)
			  DBG_INFO("TIMEOUT detection_process_inuse[%d]\n", i);
	  }
	 }
}

static void _Video_SetConfig_UnLock(VDEC_DEV_S *pVdevDev)
{
	pVdevDev->detection_process_lock = 0;
}

int RAPTOR3_Video_GetConfig( VDEC_DEV_S *pVdevDev, VDEC_CH_CFG_S *ch_cfg, VDEC_PORT_CFG_S *port_cfg )
{
	if(ch_cfg)
	memcpy( ch_cfg, pVdevDev->vdec_saved_ch_cfg, sizeof(VDEC_CH_CFG_S)*RAPTOR3_MAX_CH_PER_CHIP);
	if(port_cfg)
	memcpy( port_cfg, pVdevDev->vdec_saved_port_cfg, sizeof(VDEC_PORT_CFG_S)*RAPTOR3_MAX_PORT_PER_CHIP);


//	int ii = 0;
//	for(ii = 0; ii < RAPTOR3_MAX_PORT_PER_CHIP; ii++)
//		DBG_INFO("Setconfig : pVdevDev->vdec_saved_port_cfg [2].SEQ[%d] :[%d]\n",ii,pVdevDev->vdec_saved_port_cfg[2].SEQ[ii]);

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

/**************************************************************************************
* @desc
* 	RAPTOR3's Video Format Config Setting Function
*
* @param_in		(VDEC_DEV_S *)pVdevDev							A structure parameter that contains information for a single or multiple decoders.
* @param_in		(VDEC_CH_CFG_S *)ch_cfg							A structure parameter that has a setting value per channel.
* @param_in		(VDEC_PORT_CFG_S *)port_cfg						A structure parameter that has a setting value per port.
* @param_in		(RAPTOR3_AUTO_DETECT_DATA_STR *)pDetectData		If there is a value other than '0' in the parameter, that is, the structure parameter exists, Auto Detection related setting is proceeded. If it has value '0', it performs Manual Format Setting process.
*
* @return   	void		       								None
***************************************************************************************/
void RAPTOR3_Video_SetConfig( VDEC_DEV_S *pVdevDev, VDEC_CH_CFG_S *ch_cfg, VDEC_PORT_CFG_S *port_cfg, RAPTOR3_AUTO_DETECT_DATA_STR* pDetectData )
{
	_Video_SetConfig_Lock(pVdevDev);

	/// Setup VD Ports ( NC_VD_VO_Mode_Set )

	int port=0;
	int ch = 0;
	int oLogicalChannel = 0;

	if(pDetectData != 0)
	{
		oLogicalChannel = (pVdevDev->i2c_dev_num * 4) + pDetectData->autodetect_thread ;

		unsigned char oChannel = oLogicalChannel % 4;
		unsigned char oDevNum = oLogicalChannel / 4;


			NC_VD_AUTO_DETECT_INFO_STR		*pDetect;
			pDetect = (NC_VD_AUTO_DETECT_INFO_STR*) pDetectData;

			NC_VD_AUTO_FLAG_ON_SET(pDetect);

			NC_VD_AUTO_AutoMode_Set(oChannel, oDevNum);
		}
	else
	{
		NC_VD_AUTO_MANUAL_MODE_STR VinManualModeSet;

		if(ch_cfg)
			memcpy( pVdevDev->vdec_saved_ch_cfg, ch_cfg, sizeof(VDEC_CH_CFG_S)*RAPTOR3_MAX_CH_PER_CHIP );
		if(port_cfg)
			memcpy( pVdevDev->vdec_saved_port_cfg, port_cfg, sizeof(VDEC_PORT_CFG_S)*RAPTOR3_MAX_PORT_PER_CHIP );

		for( port=pVdevDev->vid_port_start ; port < pVdevDev->vid_port_total ; port++)  // TODO : multiple port
		//for( port=0 ; port <  4; port++)  // TODO : multiple port
		{

			VDEC_PORT_CFG_S *pPortCfg = &port_cfg[port - pVdevDev->vid_port_start];
			VDEC_CH_CFG_S *pChCfg = &ch_cfg[ port - pVdevDev->vid_port_start];

			NC_VO_PORT_FMT_S *pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( pChCfg->FmtDef ); // TODO : unnecessary arguments
			NC_VD_VO_Mode_Set( pVdevDev->i2c_dev_num, port, pPortFmt, pPortCfg->mux_mode, pPortCfg->output_interface,
					pPortCfg->edge, pPortCfg->SEQ[0], pPortCfg->SEQ[1], pPortCfg->SEQ[2], pPortCfg->SEQ[3]);

			if(pPortCfg->output_interface == VI_MODE_BT1120_STANDARD)
				port+=1;
		}

			for(ch=0;ch< pVdevDev->ch_per_chip ;ch++)
			{
				int vd_ch = ch;

				VinManualModeSet.Ch = ch % 4;
				VinManualModeSet.dev_addr = ch / 4;
				NC_VD_VI_MANUAL_MODE_Set(&VinManualModeSet);

				// Get VD Channel Format Info
				VDEC_CH_CFG_S *pChCfg = &ch_cfg[ ch ];
				//NC_VI_CH_FMT_S *pChFmt = NC_HI_VI_Get_ChannelFormat( pChCfg->FmtDef );
				NC_VI_CH_FMT_S *pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( pChCfg->FmtDef );

				// TODO :
				if(port==pVdevDev->vid_port_start)
					NCHI_VI_ChannelStatus[ ch ].pChFmt = pChFmt;

				int vi_ch;
				if(port_cfg->output_interface == VI_INPUT_MODE_BT656)
					vi_ch = ch;
				else
					vi_ch = ch*4;

				NC_VD_VI_Mode_Set((unsigned char)ch, pChFmt);

			NC_APP_VD_MANUAL_VIDEO_EQ_Set(ch, 0, pChCfg->FmtDef);

				/* Display ON */
				NC_VD_VO_Auto_Data_Mode_Set(ch % 4, ch / 4, OUT_MODE_ON);


			if( pChCfg->FmtDef == AHD20_1080P_60P || pChCfg->FmtDef == AHD20_1080P_50P )
			{
				if( pChCfg->FmtDef == AHD20_1080P_60P )
				{
					printf("RAPTOR3_SK_2M_60P_Rx_Test_Set:ch%d, fmt:%d\n", ch, pChCfg->FmtDef);
				RAPTOR3_SK_2M_60P_Rx_Test_Set(ch);
				}
				else if( pChCfg->FmtDef == AHD20_1080P_50P )
				{
					printf("RAPTOR3_SK_2M_50P_Rx_Test_Set:ch%d, fmt:%d\n", ch, pChCfg->FmtDef);
					RAPTOR3_SK_2M_50P_Rx_Test_Set(ch);
			}
			}

			}




	}
	_Video_SetConfig_UnLock(pVdevDev);
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

/*============================================================================================================================
 * Mark Add : Bank Dump to GUI - Test API
 ============================================================================================================================*/
void RAPTOR3_Coax_Bank_Dump(VDEC_DEV_S *pVdevDev, int slave_addr, unsigned char bank, unsigned char *pData)
{
	unsigned char bank_data[256] = {0, 0};

	NC_APP_VD_COAX_Bank_Dump(slave_addr, bank, 0, bank_data);

	memcpy(pData, bank_data, sizeof(bank_data) );
}

void RAPTOR3_SK_Decoder_2Mux_Set( void )
{
	int chip_num = 0;
	// VI Channel Name
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x00, 0x55, 0x10);
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x00, 0x56, 0x10);

	// VPORT SEQ
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc0, 0x00);      // Port A
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc1, 0x00);      // Port A
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc2, 0x10);  // Port B
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc3, 0x10);  // Port B
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc4, 0x32);  // Port C
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc5, 0x32);  // Port C
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc6, 0x00);      // Port D
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc7, 0x00);      // Port D

	// Output mux setting
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc8, 0x22); // 0, 2, 8
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xc9, 0x22);

	// Output port Enable
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xca, 0x66);

	// Vout Clock
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xcc, 0x46);
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xcd, 0x58); // port1
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xce, 0x58); // port2
	NC_APP_VD_SK_Test_Val_Set(chip_num, 0x01, 0xcf, 0x46);

}

void RAPTOR3_SK_6158C_Port_change_Set( int ch )
{

	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc0, 0x00);           // Port A
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc1, 0x00);           // Port A
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc6, 0x00);           // Port D
	NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc7, 0x00);           // Port D


	if(ch == 0)
	{
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc2, 0x00);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc3, 0x00);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc4, 0x00);  // Port C
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc5, 0x00);  // Port C
	}
	else if(ch == 1)
	{
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc2, 0x11);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc3, 0x11);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc4, 0x11);  // Port C
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc5, 0x11);  // Port C
	}
	else if(ch == 2)
	{
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc2, 0x22);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc3, 0x22);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc4, 0x22);  // Port C
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc5, 0x22);  // Port C
	}
	else if(ch == 3)
	{
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc2, 0x33);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc3, 0x33);  // Port B
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc4, 0x33);  // Port C
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xc5, 0x33);  // Port C
	}
	else
		printf("RAPTOR3_SK_6158C_Port_change_Set::Error\n");


}

void RAPTOR3_SK_2M_60P_Rx_Test_Set( int ch )
{
		// Bank 0
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x08 + ch, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x58 + ch, 0x80);
		//NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x58, 0xC1);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x64 + ch, 0x20);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x81 + ch, 0x02);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x85 + ch, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x89 + ch, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x8E + ch, 0x02);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x18 + ch, 0x00);

		// Bank 1
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x84 + ch, 0x04);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x8C + ch, 0x44);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xCC + ch, 0x60);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xED, ch);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x7A, ch);

		// Bank 5
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x01, 0x62);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x57, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x69, 0x10);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x90, 0x01);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xD1, 0x30);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x24, 0x1A);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x5F, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6E, 0x10);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6F, 0x2B);

		// Bank 9
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x50 + (ch * 4), 0x86);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x51 + (ch * 4), 0xb5);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x52 + (ch * 4), 0x6f);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x53 + (ch * 4), 0x48);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x44, ch);

		// Bank A
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x60 + ((ch % 2) * 0x80), 0xA8);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x61 + ((ch % 2) * 0x80), 0x05);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x62 + ((ch % 2) * 0x80), 0x1D);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x63 + ((ch % 2) * 0x80), 0x29);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x64 + ((ch % 2) * 0x80), 0x0A);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x65 + ((ch % 2) * 0x80), 0x80);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x66 + ((ch % 2) * 0x80), 0x25);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x67 + ((ch % 2) * 0x80), 0x36);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x68 + ((ch % 2) * 0x80), 0x13);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x69 + ((ch % 2) * 0x80), 0x29);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6A + ((ch % 2) * 0x80), 0x29);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6B + ((ch % 2) * 0x80), 0x41);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6C + ((ch % 2) * 0x80), 0x1A);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6D + ((ch % 2) * 0x80), 0x2B);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6E + ((ch % 2) * 0x80), 0x49);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6F + ((ch % 2) * 0x80), 0x38);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x70 + ((ch % 2) * 0x80), 0x22);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x71 + ((ch % 2) * 0x80), 0x27);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x72 + ((ch % 2) * 0x80), 0x48);

		// Bank 11
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x00 + (ch * 0x20), 0x05);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x02 + (ch * 0x20), 0x32);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x03 + (ch * 0x20), 0x07);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x04 + (ch * 0x20), 0x80);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x05 + (ch * 0x20), 0x08);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x06 + (ch * 0x20), 0x98);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x08 + (ch * 0x20), 0x78);
		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x11 + (ch * 0x20), 0xAA);


		printf("RAPTOR3_SK_6158C_Port_change_Set::Error\n");

}

void RAPTOR3_SK_2M_50P_Rx_Test_Set( int ch )
{
		// Bank 0
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x08 + ch, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x58 + ch, 0x84);
		//NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x58, 0xC1);
		//NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x64, 0x03);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x81 + ch, 0x03);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x85 + ch, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x89 + ch, 0x10);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x8E + ch, 0x02);
		NC_APP_VD_SK_Test_Val_Set(0, 0x00, 0x18 + ch, 0x00);

		// Bank 1
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x84 + ch, 0x04);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x8C + ch, 0x44);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xCC + ch, 0x60);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0xED, ch);
		NC_APP_VD_SK_Test_Val_Set(0, 0x01, 0x7A, ch);

		// Bank 5
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x01, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x57, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x69, 0x10);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x90, 0x01);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0xD1, 0x80);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x24, 0x24);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x5F, 0x00);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6E, 0x10);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x6F, 0x2C);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x28, 0x90);
		NC_APP_VD_SK_Test_Val_Set(0, 0x05 + ch, 0x44, 0x0E);

		// Bank 9
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x50 + (ch * 4), 0x07);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x51 + (ch * 4), 0x1f);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x52 + (ch * 4), 0x7c);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x53 + (ch * 4), 0x48);
		NC_APP_VD_SK_Test_Val_Set(0, 0x09, 0x44, ch);

		// Bank A
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x60 + ((ch % 2) * 0x80), 0xA8);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x61 + ((ch % 2) * 0x80), 0x05);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x62 + ((ch % 2) * 0x80), 0x1D);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x63 + ((ch % 2) * 0x80), 0x29);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x64 + ((ch % 2) * 0x80), 0x0A);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x65 + ((ch % 2) * 0x80), 0x80);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x66 + ((ch % 2) * 0x80), 0x25);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x67 + ((ch % 2) * 0x80), 0x36);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x68 + ((ch % 2) * 0x80), 0x13);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x69 + ((ch % 2) * 0x80), 0x29);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6A + ((ch % 2) * 0x80), 0x29);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6B + ((ch % 2) * 0x80), 0x41);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6C + ((ch % 2) * 0x80), 0x1A);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6D + ((ch % 2) * 0x80), 0x2B);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6E + ((ch % 2) * 0x80), 0x49);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x6F + ((ch % 2) * 0x80), 0x38);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x70 + ((ch % 2) * 0x80), 0x22);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x71 + ((ch % 2) * 0x80), 0x27);
		NC_APP_VD_SK_Test_Val_Set(0, ch > 2 ? 0x0B : 0x0A, 0x72 + ((ch % 2) * 0x80), 0x48);

		// Bank 11
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x00 + (ch * 0x20), 0x05);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x02 + (ch * 0x20), 0x32);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x03 + (ch * 0x20), 0x07);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x04 + (ch * 0x20), 0x80);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x05 + (ch * 0x20), 0x08);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x06 + (ch * 0x20), 0x98);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x08 + (ch * 0x20), 0x78);
//		NC_APP_VD_SK_Test_Val_Set(0, 0x11, 0x11 + (ch * 0x20), 0xAA);

		printf("RAPTOR3_SK_6158C_Port_change_Set::Error\n");

}

/*============================================================================================================================
 * Coaxial Protocol F/W Upgrade
 ============================================================================================================================*/
/**************************************************************************************
* @desc
* 	RAPTOR3's Memory allocation by the number of firmware upgrade files
* 	          Check and return the number of firmware upgrade files.
*
* @param_in		(VDEC_DEV_S *)pVdevDev				            Chip information structure
* @param_in		(int)vi_ch		                                Channel number to upgrade firmware
* @param_out	(RAPTOR3_FMT_DETECT_RESULT_S *)pDetectResult	Number of firmware update files
*
* @return   	int		       								    Number of firmware upgrade files.
***************************************************************************************/
int RAPTOR3_Video_Init_Info_Get( VDEC_DEV_S *pVdevDev, int ch, int select )
{
	int ret = -1;

	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[ch] );
	NC_VI_CH_FMT_S *pChFmt   = pChStat->pChFmt;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;
	NC_VIVO_CH_FORMATDEF vivo_fmt          = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );

	if( select == 0 )
	{
		ret = format_standard;
	}
	else if( select == 1 )
	{
		ret = format_resolution;
	}
	else if( select == 2 )
	{
		ret = format_fps;
	}
	else if( select == 3 )
	{
		ret = vivo_fmt;
	}

	return ret;
}

int RAPTOR3_Video_Init_Info_New_Get( VDEC_DEV_S *pVdevDev, int ch, int select )
{
	int ret = -1;

	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[ch] );
	NC_VI_CH_FMT_S *pChFmt   = pChStat->pChFmt;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;
	NC_VIVO_CH_FORMATDEF vivo_fmt          = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );

	if( select == 0 )
	{
		ret = format_standard;
	}
	else if( select == 1 )
	{
		ret = format_resolution;
	}
	else if( select == 2 )
	{
		ret = format_fps;
	}
	else if( select == 3 )
	{
		ret = vivo_fmt;
	}

	return ret;
}



void RAPTOR3_Coax_Rx_Value_Read( VDEC_DEV_S *pVdevDev, int ch, NC_FORMAT_STANDARD fmt, unsigned char *pData )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[ch] );
	NC_VI_CH_FMT_S *pChFmt   = pChStat->pChFmt;

	int vd_dev = ch / 4;
	int vd_ch =  ch % 4;
	int ii = 0;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_VD_COAX_STR RxData;

	memset(CoaxRx, 0, sizeof(CoaxRx));

	if( (format_standard == FMT_TVI) || (format_standard == FMT_CVI) )
	{
		NC_APP_VD_COAX_Rx_Data_Get( vd_dev, vd_ch, format_standard, format_resolution, format_fps, &RxData );
		for(ii=0; ii<5; ii++)
		{
			pData[ii] = RxData.rx_data1[ii];
			pData[ii+5] = RxData.rx_data2[ii];
			pData[ii+10] = RxData.rx_data3[ii];
			pData[ii+15] = RxData.rx_data4[ii];
			pData[ii+20] = RxData.rx_data5[ii];
			pData[ii+25] = RxData.rx_data6[ii];

			sprintf(CoaxRx[ii], "%X", RxData.rx_data1[ii]);
			sprintf(CoaxRx[ii+5], "%X", RxData.rx_data2[ii]);
			sprintf(CoaxRx[ii+10], "%X", RxData.rx_data3[ii]);
			sprintf(CoaxRx[ii+15], "%X", RxData.rx_data4[ii]);
			sprintf(CoaxRx[ii+20], "%X", RxData.rx_data5[ii]);
			sprintf(CoaxRx[ii+25], "%X", RxData.rx_data6[ii]);
		}
	}
	else
	{
		NC_APP_VD_COAX_Rx_Data_Get( 0, ch, format_standard, format_resolution, format_fps, &RxData );
		for(ii=0; ii<8; ii++)
		{
			pData[ii] = RxData.rx_pelco_data[ii];
			sprintf(CoaxRx[ii], "%X", RxData.rx_pelco_data[ii]);
		}
	}
}


int RAPTOR3_YUV_Capture_USB_Mount( void )
{
	int ret = -1;
    unsigned char path[128] = {0, };
	unsigned char cmd[64] = {0, };

	ret = NC_APP_VD_USB_Mount();

	return ret;
}

int RAPTOR3_YUV_Capture_USB_Umount( void )
{
	NC_APP_VD_USB_Umount();
	return 0;
}

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

/**************************************************************************************
* @desc
* 	RAPTOR3's Firmware upgrade data transfer rate.
*
* @param_in		void                                            None
*
* @return   	int  		       								0~100. Data transfer rate
*
***************************************************************************************/
int RAPTOR3_Coax_FW_Progress_Status_Get( void )
{
	return NC_APP_VD_COAX_FirmUP_ProgressBar_Status_Get();
}

extern VDEC_DEV_S raptor3_dev0;
extern VDEC_DEV_S raptor3_dev1;
extern VDEC_DEV_S raptor3_dev2;
extern VDEC_DEV_S raptor3_dev3;

int RAPTOR3_Current_Get_Mux_Mode(VDEC_DEV_S *pVdevDev)
{
	return pVdevDev->Mux_Mode;
}

/*============================================================================================================================
 * Coaxial Protocol UP Stream Function
 ============================================================================================================================*/
void RAPTOR3_Coax_Tx_Rx_Init_Channel( int Ch )
{
	int vd_dev = Ch / 4;
	int vd_ch =  Ch % 4;
	int vd_mux_mode = 0;

	if(vd_dev == 0)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev0);
	else if(vd_dev == 1)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev1);
	else if(vd_dev == 2)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev2);
	else if(vd_dev == 3)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev3);


	if(vd_mux_mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(vd_mux_mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[Ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;



	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Tx_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
	NC_APP_VD_COAX_Rx_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
}

void RAPTOR3_Coax_Tx_8_16Bit_Mode_Init_Channel( int Ch, int mode )
{
	int vd_dev = Ch / 4;
	int vd_ch =  Ch % 4;
	int vd_mux_mode = 0;

	if(vd_dev == 0)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev0);
	else if(vd_dev == 1)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev1);
	else if(vd_dev == 2)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev2);
	else if(vd_dev == 3)
		vd_mux_mode = RAPTOR3_Current_Get_Mux_Mode(&raptor3_dev3);


	if(vd_mux_mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(vd_mux_mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[Ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	if(mode)
	{
		NC_APP_VD_COAX_Tx_16Bit_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
	}
	else
	{
		NC_APP_VD_COAX_Tx_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
	}
}

void RAPTOR3_Coax_Tx_Bit_Mode_Send_Command( VDEC_DEV_S *pVdevDev, int Ch, int mode, NC_COAX_CMD_DEF cmd )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[Ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

#if 1 // Andy
	int vd_dev = Ch / 4;
	int vd_ch =  Ch % 4;
#else
	int vd_dev = pChStat->vd_dev;
	int vd_ch = pChStat->vd_ch;
#endif

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	if(mode)
	{
		NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( vd_dev, vd_ch, format_standard, format_resolution, format_fps, cmd );
	}
	else
	{
		NC_APP_VD_COAX_Tx_Command_Send_Set( vd_dev, vd_ch, format_standard, format_resolution, format_fps, cmd );
	}

}


/**************************************************************************************
* @desc
* 	RAPTOR3's Firmware upgrade data transfer rate.
*
* @param_in		void                                            None
*
* @return   	int  		       								0~100. Data transfer rate
*
***************************************************************************************/
void RAPTOR3_Coax_Tx_Init_Channel( VDEC_DEV_S *pVdevDev, int vi_ch )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Tx_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
}

void RAPTOR3_Coax_Tx_Send_Command( VDEC_DEV_S *pVdevDev, int vi_ch, NC_COAX_CMD_DEF cmd )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch]  );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;


	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Tx_Command_Send_Set( vd_dev, vd_ch, format_standard, format_resolution, format_fps, cmd );
}

void RAPTOR3_Coax_Tx_16Bit_Init_Channel( VDEC_DEV_S *pVdevDev, int vi_ch )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Tx_16Bit_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
}

void RAPTOR3_Coax_Tx_16Bit_Send_Command( VDEC_DEV_S *pVdevDev, int vi_ch, NC_COAX_CMD_DEF cmd )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch]  );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( vd_dev, vd_ch, format_standard, format_resolution, format_fps, cmd );
}

/*============================================================================================================================
 * Coaxial Protocol DOWN Stream Function
 ============================================================================================================================*/
NC_FORMAT_STANDARD RAPTOR3_Coax_Rx_Init(VDEC_DEV_S *pVdevDev, int vi_ch )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Rx_Init( vd_dev, vd_ch, format_standard, format_resolution, format_fps );

	return format_standard;
}

void RAPTOR3_Coax_Rx_DeInit(VDEC_DEV_S *pVdevDev, int vi_ch )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;


	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Rx_DeInit( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
}

void RAPTOR3_Coax_Rx_Buffer_Clear(VDEC_DEV_S *pVdevDev, int vi_ch )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S *pChFmt   = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_APP_VD_COAX_Rx_Buffer_Clear( vd_dev, vd_ch, format_standard, format_resolution, format_fps );
}

void RAPTOR3_Coax_Rx_Val_Read(VDEC_DEV_S *pVdevDev, int vi_ch, NC_FORMAT_STANDARD fmt )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;


	NC_VD_COAX_STR RxData;

	if( (format_standard == FMT_TVI) || (format_standard == FMT_CVI) )
	{
		NC_APP_VD_COAX_Rx_Data_Get( vd_dev, vd_ch, format_standard, format_resolution, format_fps, &RxData );
		sprintf(CoaxRxLine1, "L1: %x / %x / %x / %x / %x", RxData.rx_data1[0], RxData.rx_data1[1], RxData.rx_data1[2], RxData.rx_data1[3], RxData.rx_data1[4] );
		sprintf(CoaxRxLine2, "L2: %x / %x / %x / %x / %x", RxData.rx_data2[0], RxData.rx_data2[1], RxData.rx_data2[2], RxData.rx_data2[3], RxData.rx_data2[4] );
		sprintf(CoaxRxLine3, "L3: %x / %x / %x / %x / %x", RxData.rx_data3[0], RxData.rx_data3[1], RxData.rx_data3[2], RxData.rx_data3[3], RxData.rx_data3[4] );
		sprintf(CoaxRxLine4, "L4: %x / %x / %x / %x / %x", RxData.rx_data4[0], RxData.rx_data4[1], RxData.rx_data4[2], RxData.rx_data4[3], RxData.rx_data4[4] );
		sprintf(CoaxRxLine5, "L5: %x / %x / %x / %x / %x", RxData.rx_data5[0], RxData.rx_data5[1], RxData.rx_data5[2], RxData.rx_data5[3], RxData.rx_data5[4] );
		sprintf(CoaxRxLine6, "L6: %x / %x / %x / %x / %x", RxData.rx_data6[0], RxData.rx_data6[1], RxData.rx_data6[2], RxData.rx_data6[3], RxData.rx_data6[4] );
	}
	else
	{
		NC_APP_VD_COAX_Rx_Data_Get( 0, vi_ch, format_standard, format_resolution, format_fps, &RxData );
		sprintf(CoaxRxLine1, "L1: %x", RxData.rx_pelco_data[0] );
		sprintf(CoaxRxLine2, "L2: %x", RxData.rx_pelco_data[1] );
		sprintf(CoaxRxLine3, "L3: %x", RxData.rx_pelco_data[2] );
		sprintf(CoaxRxLine4, "L4: %x", RxData.rx_pelco_data[3] );
		sprintf(CoaxRxLine5, "L5: %x", RxData.rx_pelco_data[4] );
		sprintf(CoaxRxLine6, "L6: %x", RxData.rx_pelco_data[5] );
		sprintf(CoaxRxLine7, "L7: %x", RxData.rx_pelco_data[6] );
		sprintf(CoaxRxLine8, "L8: %x", RxData.rx_pelco_data[7] );
	}
}

void JAGUAR1_Coax_Rx_Value_Read( int vi_ch, NC_FORMAT_STANDARD fmt, unsigned char *pData )
{
	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( vi_ch*4 );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;
	int ii = 0;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_VD_COAX_STR RxData;

	memset(CoaxRx, 0, sizeof(CoaxRx));

	if( (format_standard == FMT_TVI) || (format_standard == FMT_CVI) )
	{
		NC_APP_VD_COAX_Rx_Data_Get( vd_dev, vd_ch, format_standard, format_resolution, format_fps, &RxData );
		for(ii=0; ii<5; ii++)
		{
			pData[ii] = RxData.rx_data1[ii];
			pData[ii+5] = RxData.rx_data2[ii];
			pData[ii+10] = RxData.rx_data3[ii];
			pData[ii+15] = RxData.rx_data4[ii];
			pData[ii+20] = RxData.rx_data5[ii];
			pData[ii+25] = RxData.rx_data6[ii];

			sprintf(CoaxRx[ii], "%X", RxData.rx_data1[ii]);
			sprintf(CoaxRx[ii+5], "%X", RxData.rx_data2[ii]);
			sprintf(CoaxRx[ii+10], "%X", RxData.rx_data3[ii]);
			sprintf(CoaxRx[ii+15], "%X", RxData.rx_data4[ii]);
			sprintf(CoaxRx[ii+20], "%X", RxData.rx_data5[ii]);
			sprintf(CoaxRx[ii+25], "%X", RxData.rx_data6[ii]);
		}
	}
	else
	{
		NC_APP_VD_COAX_Rx_Data_Get( 0, vi_ch, format_standard, format_resolution, format_fps, &RxData );
		for(ii=0; ii<8; ii++)
		{
			pData[ii] = RxData.rx_pelco_data[ii];
			sprintf(CoaxRx[ii], "%X", RxData.rx_pelco_data[ii]);
		}
	}
}

int JAGUAR1_Coax_Rx_Detection_Get( int vi_ch )
{
#if 0
	NC_VD_COAX_STR RxData;

	NC_APP_VD_COAX_Rx_Detection_Get( 0, vi_ch, &RxData );

	return RxData.param;
#endif

	return 0;
}

void RAPTOR3_Coax_Option_RT_NRT_Mode_Change_Set(VDEC_DEV_S *pVdevDev, int vi_ch, int param )
{
	int vd_dev = vi_ch / 4;
	int vd_ch =  vi_ch % 4;

	NC_APP_VD_COAX_Option_RT_NRT_Mode_Change_Set( vd_dev, vd_ch, param );
}

/*============================================================================================================================
 * Coaxial Protocol Test
 ============================================================================================================================*/
void RAPTOR3_Coax_Tx_Test_Shot_Set( VDEC_DEV_S *pVdevDev, int vi_ch, int Val )
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

//	int vd_dev = pChStat->vd_dev;
//	int vd_ch  = pChStat->vd_ch;

	NC_FORMAT_STANDARD format_standard = pChFmt->format_standard;

	NC_APP_VD_COAX_Tx_Test_Shot_Set( vi_ch, format_standard, Val );
}

void RAPTOR3_Coax_Tx_Test_Init_Val_Set(VDEC_DEV_S *pVdevDev, int menu, int Val )
{
	NC_APP_VD_COAX_Tx_Test_Init_Val_Set( menu, Val);
}

void RAPTOR3_Coax_Test_Tx_Init_Val_Get( VDEC_DEV_S *pVdevDev, int menu )
{
	int ReadAddr = 0;
	int ReadData = 0;

	NC_APP_VD_COAX_Tx_Test_Init_Val_Get(menu, &ReadData, &ReadAddr);

	sprintf(CoaxRead, "0x%02X > 0x%02X", ReadAddr, ReadData);
}

void RAPTOR3_Coax_Rx_Test_EQ_Filter_Set( VDEC_DEV_S *pVdevDev, int vi_ch, int sel )
{
	NC_APP_VD_COAX_Rx_Test_EQ_Filter_Set(vi_ch, sel);
}

NC_FORMAT_STANDARD RAPTOR3_Coax_NC_FORMAT_STANDARD_Read(VDEC_DEV_S *pVdevDev, int vi_ch)
{
	if(pVdevDev->Mux_Mode == 0)
		memcpy(soc_vi_ch, soc_vi_1mux, sizeof(soc_vi_1mux));
	else if(pVdevDev->Mux_Mode == 1)
		memcpy(soc_vi_ch, soc_vi_2mux, sizeof(soc_vi_2mux));

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	NC_FORMAT_STANDARD format_standard = pChFmt->format_standard;

	return format_standard;
}

void RAPTOR3_Coax_Test_Addr_Val_Set(VDEC_DEV_S *pVdevDev, int slave_addr, int bank, int addr, int val )
{
	NC_APP_VD_COAX_Test_Val_Set( slave_addr, bank, addr, val);
}

int RAPTOR3_Coax_Test_Addr_Val_Get(VDEC_DEV_S *pVdevDev,int slave_addr, int bank, int addr )
{
	int read_val = 0;
	read_val = NC_APP_VD_COAX_Test_Val_Get( slave_addr, bank, addr);
	printf("RAPTOR3_Coax_Test_Addr_Val_Get::%d\n", read_val);
	return read_val;
}

int RAPTOR3_Coax_Channel_FMT_Str_Get( VDEC_DEV_S *pVdevDev, int vi_ch, char *pFormatStr  )
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

	NC_VIVO_CH_FORMATDEF fmt = NC_HI_VI_FindFormatDef(format_standard, format_resolution, format_fps);
	NC_VI_CH_FMT_S *pFmtStr  = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( fmt );

	sprintf(pFormatStr, "ch%d: %s", vi_ch+1, pFmtStr->name);

	return 0;
}

NC_VIVO_CH_FORMATDEF RAPTOR3_Coax_Channel_FMT_Get( VDEC_DEV_S *pVdevDev, int vi_ch )
{
	int soc_vi_ch[4] = {0, 4, 8, 12};

	NC_HI_VI_STAT_S *pChStat = NC_HI_VI_Channel_GetStatus( soc_vi_ch[vi_ch] );
	NC_VI_CH_FMT_S  *pChFmt  = pChStat->pChFmt;

	NC_FORMAT_STANDARD   format_standard   = pChFmt->format_standard;
	NC_FORMAT_RESOLUTION format_resolution = pChFmt->format_resolution;
	NC_FORMAT_FPS        format_fps        = pChFmt->format_fps;

	NC_VIVO_CH_FORMATDEF fmt = NC_HI_VI_FindFormatDef(format_standard, format_resolution, format_fps);
//	NC_VI_CH_FMT_S *pFmtStr  = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( fmt );

	return fmt;
}

int RAPTOR3_Coax_System_FirmUP_Check( void )
{
	NC_APP_VD_COAX_System_FirmUP_Check();
	return 0;
}


/*============================================================================================================================
 * Coaxial Protocol Function End
 ============================================================================================================================*/

NC_VIVO_CH_FORMATDEF RAPTOR3_Video_FindFormatDef(NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps)
{
	NC_VIVO_CH_FORMATDEF ret_FmtDef = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );

	return ret_FmtDef;
}

void RAPTOR3_Video_SetColorBar( VDEC_DEV_S *pVdevDev, int enable )
{
	NC_VD_VO_COLORBAR_Set();
}

/////////////////////////////////////// AUDIO ///////////////////////////////////

#include <raptor3_audio.h>

void RAPTOR3_Audio_Recode_Set(VDEC_DEV_S *pVdevDev, NC_AUDIO_MASTER_SLAVE master_slave,
									   NC_AUDIO_INTERFACE audio_interface,
									   NC_AUDIO_SAMPLING_RATE audio_sampling_rate,
									   NC_AUDIO_VOL audio_in_volume,
									   NC_AUDIO_VOL audio_out_volume,
									   NC_AUDIO_CH audio_ch
										)
{
	DBG_INFO("SysSendCmd_Setup_Audio_Recode_Set !!\n");
}

void RAPTOR3_Audio_PlayBack_Set(VDEC_DEV_S *pVdevDev, NC_AUDIO_MASTER_SLAVE master_slave,
									   NC_AUDIO_INTERFACE audio_interface,
									   NC_AUDIO_SAMPLING_RATE audio_sampling_rate,
									   NC_AUDIO_VOL audio_in_volume,
									   NC_AUDIO_VOL audio_out_volume,
									   NC_AUDIO_CH audio_ch
										)
{
	DBG_INFO("SysSendCmd_Setup_Audio_PlayBack_Set !!\n");
}

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

				/********************************************************
				 *														*
				 *			 RAPTOR3 Auto Detection Function			*
				 *			 											*
				 ********************************************************/

/**************************************************************************************
* @desc
* 	RAPTOR3's Auto Detection Thread Initialize Function
*
* @param_in		(FMT_DETECT_CONFIG_S *)pDetectCfg				A structure parameter that stores the basic information of the decoder for which Auto Format Detection will operate.
* @param_out	(RAPTOR3_AUTO_DETECT_DATA_STR *)pDetectData		A structure parameter that stores the necessary data for auto format detection and the flags needed during the auto detection process.
*
* @return   	void		       								None
***************************************************************************************/
void RAPTOR3_AutoDetection_Init( FMT_DETECT_CONFIG_S *pDetectCfg, RAPTOR3_AUTO_DETECT_DATA_STR *pDetectData )
{
	int Ch = pDetectCfg->ch_in_dev;
	VDEC_DEV_S *pVdevDev = pDetectCfg->pVdevDev;

	pDetectData->autodetect_thread 		= Ch;
	pDetectData->AutoInfo.p_ThreadOn	= OFF;
}

RAPTOR3_INFORMATION_S	s_raptor3_vfmts;
/**************************************************************************************
* @desc
* 	RAPTOR3's Auto Detection Thread main function
*
* @param_in		(FMT_DETECT_CONFIG_S *)pDetectCfg				Parameter to operate auto format detection Thread per channel.
* @param_in		(RAPTOR3_AUTO_DETECT_DATA_STR *)pDetectData		A structure parameter that stores the necessary data for auto format detection and the flags needed during the auto detection process.
* @param_out	(RAPTOR3_FMT_DETECT_RESULT_S *)pDetectResult	Parameter to store Detect result when auto format detection is finished by channel.
*
* @return   	void		       								None
***************************************************************************************/
#if 1
void RAPTOR3_AutoDetection_DoDetection( FMT_DETECT_CONFIG_S *pDetectCfg, RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult, RAPTOR3_AUTO_DETECT_DATA_STR *pDetectData)
{
		int i;
		int ret;
		int ch = pDetectCfg->ch_in_dev;
		unsigned char oChannel = ch%4;

		VDEC_DEV_S *pVdevDev = pDetectCfg->pVdevDev;
		int oDevNum = pVdevDev->i2c_dev_num;

		CABLE_DISTANCE				Distance;
		NC_VD_AUTO_DETECT_INFO_STR		*pDetect = (NC_VD_AUTO_DETECT_INFO_STR*) pDetectData;

		unsigned char 		oCurVideofmt = 0x00;
		unsigned char 		oPreVideofmt = 0x00;
		unsigned char 		CableDistance = 0;
		NC_VIVO_CH_FORMATDEF oFmtDef;
		NC_VIVO_CH_FORMATDEF oFmtB5Def;
		NC_VD_DEV_CH_INFO_STR sDevChInfo;

	int oLogicalChannel = pVdevDev->i2c_dev_num*4 + pDetectCfg->ch_in_dev;
	s_raptor3_vfmts.oMux = pVdevDev->Mux_Mode;

	//DBG_INFO(">>>> CH:[%d] MuxMode:[%d]\n", oLogicalChannel, s_raptor3_vfmts.oMux);

			if(pDetect->AutoInfo.p_ThreadOn)
			{
				sDevChInfo.Ch = oChannel;
				sDevChInfo.Dev_Num = oDevNum;

				/* get video format */
		RAPTOR3_SAL_GetFormatEachCh( oLogicalChannel, &s_raptor3_vfmts );

				/* process video format on/off */
		oCurVideofmt  = s_raptor3_vfmts.curvideofmt[oLogicalChannel];
		oPreVideofmt  = s_raptor3_vfmts.prevideofmt[oLogicalChannel];

		if( s_raptor3_vfmts.curvideoloss[oLogicalChannel] == VIDEO_LOSS_ON)
				{
					/* on video */
					if( (oCurVideofmt != NC_VIVO_CH_FORMATDEF_UNKNOWN) && (oPreVideofmt == NC_VIVO_CH_FORMATDEF_UNKNOWN) )
					{
				oFmtDef = NC_VD_AUTO_VFCtoFMTDEF( s_raptor3_vfmts.vfc[oLogicalChannel] );
				sDevChInfo.Fmt_Def = oFmtDef;
				/* Not Yet Support TVI 5M 20P ... 170818. by edward */
				/* This Part is TVI 5M 20P and TVI 4M 15P for Supporting */

				DBG_INFO("[CH:%d] >> Now Format Define Value : [%02x]\n", oLogicalChannel, oFmtDef);
				DBG_INFO("[CH:%d] >> Now Format Define Value : [%02x]\n", oLogicalChannel, oFmtDef);
				DBG_INFO("[CH:%d] >> Now Format Define Value : [%02x]\n", oLogicalChannel, oFmtDef);

#if 1

				if(oFmtDef == AHD30_5M_20P )
				{
					/* Read Bank13 0xA4, 0xA5 ---> integral_status_ahd_vbi[7:0], integral_status_ahd_vbi[15:8]  */
					/* integral_vbi level is Bank13 0x14 														*/
					/* If integral_status_ahd_vbi < threshold ----> TVI_5M_20P
					 * else AHD30_5M_20P Setting
					 */
					if( oFmtDef == AHD30_5M_20P )
					{
					DBG_INFO("[CH:%d] >> finding format:AHD 5M 20P....\n", oLogicalChannel);
					DBG_INFO("[CH:%d] >> finding format:AHD 5M 20P....\n", oLogicalChannel);
					DBG_INFO("[CH:%d] >> finding format:AHD 5M 20P....\n", oLogicalChannel);
					}

					NC_VD_VI_AHD_TVI_Ditinguish(&sDevChInfo);
					oFmtDef = sDevChInfo.Fmt_Def;

					if( oFmtDef == TVI_5M_20P )
					{
						DBG_INFO("[CH:%d] >> changing format:TVI 5M 20P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> changing format:TVI 5M 20P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> changing format:TVI 5M 20P....\n", oLogicalChannel);

						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = TVI_5M_20P;
					}
				}
#else
				if(oFmtDef == AHD30_5M_20P || oFmtDef == 0x48 )
				{
					/* Read Bank13 0xA4, 0xA5 ---> integral_status_ahd_vbi[7:0], integral_status_ahd_vbi[15:8]  */
					/* integral_vbi level is Bank13 0x14 														*/
					/* If integral_status_ahd_vbi < threshold ----> TVI_5M_20P
					 * else AHD30_5M_20P Setting
					 */
					if( oFmtDef == AHD30_5M_20P )
					{
						DBG_INFO("[CH:%d] >> finding format:AHD 5M 20P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> finding format:AHD 5M 20P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> finding format:AHD 5M 20P....\n", oLogicalChannel);
					}
					else if( oFmtDef == 0x48 )
					{
						DBG_INFO("[CH:%d] >> finding format:Unknown.. 0x4F....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> finding format:Unknown.. 0x4F....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> finding format:Unknown.. 0x4F....\n", oLogicalChannel);
					}

					NC_VD_VI_AHD_TVI_Ditinguish(&sDevChInfo);
					oFmtDef = sDevChInfo.Fmt_Def;

					if( oFmtDef == TVI_5M_20P )
					{
						DBG_INFO("[CH:%d] >> changing format:TVI 5M 20P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> changing format:TVI 5M 20P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> changing format:TVI 5M 20P....\n", oLogicalChannel);

						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = TVI_5M_20P;
					}
					else if( oFmtDef == TVI_4M_15P )
					{
						DBG_INFO("[CH:%d] >> changing format:TVI 4M 15P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> changing format:TVI 4M 15P....\n", oLogicalChannel);
						DBG_INFO("[CH:%d] >> changing format:TVI 4M 15P....\n", oLogicalChannel);

						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = TVI_4M_15P;
					}
				}

#endif
				
				{
						/* Change Status SoC Setting [Resolution & Frame rate] */
						pDetectResult->FormatChanged = 1;
						pDetectResult->FmtDef = oFmtDef;
						pDetectResult->ch_cfg.FmtDef = oFmtDef;
						pDetectResult->port_cfg.output_interface = VI_INPUT_MODE_BT656;

					if(s_raptor3_vfmts.oMux == VI_WORK_MODE_2Multiplex)
						pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_2Multiplex;
					else
						pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_1Multiplex;

					if(s_raptor3_vfmts.oMux == VI_WORK_MODE_2Multiplex)
					{
						pDetectResult->port_cfg.edge = VI_CLK_EDGE_DOUBLE;
					}
					else
					{
						if( oFmtDef == AHD30_4M_30P  || oFmtDef == AHD30_4M_25P ||
							oFmtDef == AHD30_3M_30P  || oFmtDef == AHD30_3M_25P ||
							oFmtDef == AHD30_5M_20P  ||

							oFmtDef == AHD30_5_3M_20P||
							oFmtDef == AHD30_6M_18P  || oFmtDef == AHD30_6M_20P  ||
							oFmtDef == AHD30_8M_15P  || oFmtDef == AHD30_8M_12_5P||

							oFmtDef == CVI_4M_25P 	 || oFmtDef == CVI_4M_30P 	||
							oFmtDef == CVI_8M_12_5P  || oFmtDef == CVI_8M_15P 	||
							oFmtDef == TVI_4M_25P 	 || oFmtDef == TVI_4M_30P 	||
							oFmtDef == TVI_5M_20P
							)
						{
								pDetectResult->port_cfg.edge = VI_CLK_EDGE_DOUBLE;
							fprintf(stdout, "SoC Double Edge.. FormatDefine [%d]\n", oFmtDef);
						}
						else
						{
							pDetectResult->port_cfg.edge = VI_CLK_EDGE_SINGLE_UP;
							fprintf(stdout, "SoC Single Edge.. FormatDefine [%d]\n", oFmtDef);
						}
					}

					/* set video format(DEC) */
					RAPTOR3_SAL_OnVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

					/* EQ - get stage and set eq value and show decoder */
					NC_APP_VD_MANUAL_VIDEO_EQ_Set(oLogicalChannel, 0, oFmtDef);

					NC_VD_AUTO_MANUAL_AGC_STABLE_DISABLE(&sDevChInfo);

					CableDistance = NC_APP_VD_MANUAL_CABLE_DISTANCE_Get( oLogicalChannel, oFmtDef );
					NC_APP_VD_MANUAL_VIDEO_EQ_Set(oLogicalChannel, CableDistance, oFmtDef);

					NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_ON );

					pDetectResult->CableDistance = CableDistance;
						}

				s_raptor3_vfmts.prevideofmt[oLogicalChannel] = s_raptor3_vfmts.curvideofmt[oLogicalChannel];

				fprintf( stdout, ">>>>> CH[%d], Set video format : 0x%02X\n", oLogicalChannel, oCurVideofmt );
					}
					else
					{
				/* check debouce video format(-1:changed, 0:not checked) when you changed video format(Dahua-NVP2450). */
				ret = RAPTOR3_SAL_AutoDebouceCheck( oLogicalChannel, &s_raptor3_vfmts );
				if( ( ret == -1 ) && ( gCoaxFirmUpdateFlag[oLogicalChannel] == 0 ) )
						{
					sDevChInfo.Ch = oLogicalChannel % 4;
							sDevChInfo.Dev_Num = oDevNum;
							sDevChInfo.Fmt_Def = oFmtB5Def;

					NC_VD_AUTO_CONTRAST_OFF(&sDevChInfo);

					/* hide decoder */
					NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_OFF );

					/* decoder afe power down */
					NC_VD_AUTO_AFE_DOWN(&sDevChInfo);
							/* set no video- first(i:channel, raptor3_vfmts:information */
					RAPTOR3_SAL_NoVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );
					/* decoder afe power up */
					NC_VD_AUTO_AFE_UP(&sDevChInfo);

					/* for forced agc stable */
					NC_VD_AUTO_MANUAL_AGC_STABLE_ENABLE(&sDevChInfo);
					usleep(500*1000);

							/* save onvideo to prevideofmt */
					s_raptor3_vfmts.prevideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

					fprintf( stdout, ">>>>> CH[%d], Reset, Set No video : 0x%02X\n", oLogicalChannel, oCurVideofmt );
						}
					}
				}
				else
				{
					/* no video */
					if( oPreVideofmt != NC_VIVO_CH_FORMATDEF_UNKNOWN )
					{
				NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_OFF );

						/* Change Status SoC Setting [Resolution & Frame rate] */
						pDetectResult->FormatChanged = 1;
				pDetectResult->FmtDef = CVI_HD_30P_EX;
				pDetectResult->ch_cfg.FmtDef = CVI_HD_30P_EX;
						pDetectResult->port_cfg.output_interface = VI_INPUT_MODE_BT656;
				pDetectResult->CableDistance = 0;

				if(s_raptor3_vfmts.oMux == VI_WORK_MODE_2Multiplex)
				{
					pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_2Multiplex;
					pDetectResult->port_cfg.edge = VI_CLK_EDGE_DOUBLE;
				}
				else
				{
						pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_1Multiplex;
						pDetectResult->port_cfg.edge = VI_CLK_EDGE_SINGLE_UP;
				}

				/* set no video- first(i:channel, raptor3_vfmts:information */
				RAPTOR3_SAL_NoVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

				s_raptor3_vfmts.prevideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

				fprintf( stdout, ">>>>> CH[%d], Set No video : 0x%02X\n", oLogicalChannel, oCurVideofmt );
					}

				}
				/* save video format to prevideofmt */

				/* display debug message */
				#if 0
				//for( i = 0; i < (s_raptor3_vfmts.chipinform.Total_Chip_Cnt*4); i++)
				{
					fprintf( stdout, "[Ch:%d] 0x%02x \n", ch, s_raptor3_vfmts.curvideofmt[ch] );
				}
					#endif

			}
			else if(!pDetect->AutoInfo.p_ThreadOn)
			{


			}
	pVdevDev->detection_process_inuse[oLogicalChannel]  = 0;
}

#elif 0
void RAPTOR3_AutoDetection_DoDetection( FMT_DETECT_CONFIG_S *pDetectCfg, RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult, RAPTOR3_AUTO_DETECT_DATA_STR *pDetectData)
{
	/*  Auto Format Setting Sequence  */
	int i;
	int ch = pDetectCfg->ch_in_dev;
	unsigned char oChannel = ch%4;

	VDEC_DEV_S *pVdevDev = pDetectCfg->pVdevDev;
	int oDevNum = pVdevDev->i2c_dev_num;

	CABLE_DISTANCE				Distance;
	NC_VD_AUTO_DETECT_INFO_STR		*pDetect = (NC_VD_AUTO_DETECT_INFO_STR*) pDetectData;

	unsigned char 		oCurVideofmt = 0x00;
	unsigned char 		oPreVideofmt = 0x00;
	unsigned char 		CableDistance = 0;
	NC_VIVO_CH_FORMATDEF oFmtDef;

	NC_VD_DEV_CH_INFO_STR Dev_Ch_Info;

	if(pDetect->AutoInfo.p_ThreadOn)
	{
		Dev_Ch_Info.Ch = oChannel;
		Dev_Ch_Info.Dev_Num = oDevNum;

		/* get video format */
		RAPTOR3_SAL_GetFormatEachCh( ch, &s_raptor3_vfmts );

		/* process video format on/off */
		oCurVideofmt  = s_raptor3_vfmts.curvideofmt[ch];
		oPreVideofmt  = s_raptor3_vfmts.prevideofmt[ch];

		if(s_raptor3_vfmts.curvideoloss[ch] != VIDEO_LOSS_ON)
		{
			/* no video */
			if( oCurVideofmt != oPreVideofmt )
			{
				/* set no video- first(i:channel, raptor3_vfmts:information */
				RAPTOR3_SAL_NoVIdeoSetFormat( ch, &s_raptor3_vfmts );

				/* Change Status SoC Setting [Resolution & Frame rate] */
				pDetectResult->FormatChanged = 1;
				pDetectResult->FmtDef = CVI_HD_30P;
				pDetectResult->ch_cfg.FmtDef = CVI_HD_30P;
				pDetectResult->port_cfg.output_interface = VI_INPUT_MODE_BT656;
				pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_1Multiplex;
				pDetectResult->port_cfg.edge = VI_CLK_EDGE_SINGLE_UP;

				NC_VD_VO_Auto_Data_Mode_Set( ch, oDevNum,  OUT_MODE_OFF );
				fprintf( stdout, ">>>>> CH[%d], Set No video : 0x%02X\n", ch, oCurVideofmt );
			}
		}
		else
		{
			if( oCurVideofmt != NC_VIVO_CH_FORMATDEF_UNKNOWN )
			{
				/* on video */
				if( ( oCurVideofmt != oPreVideofmt ) /* && ( oPreVideofmt == NC_VIVO_CH_FORMATDEF_UNKNOWN ) */)
				{

					/* set video format(DEC) */
					RAPTOR3_SAL_OnVIdeoSetFormat( ch, &s_raptor3_vfmts );

					/* EQ - get stage and set eq value and show decoder */
					oFmtDef = NC_VD_AUTO_VFCtoFMTDEF( s_raptor3_vfmts.vfc[ch] );

					CableDistance = NC_APP_VD_MANUAL_CABLE_DISTANCE_Get( ch, oFmtDef );
					NC_APP_VD_MANUAL_VIDEO_EQ_Set(ch, CableDistance, oFmtDef);
					NC_VD_VO_Auto_Data_Mode_Set( ch, oDevNum, 0x01/*1 is OUT_MODE_ON*/ );

					/* Change Status SoC Setting [Resolution & Frame rate] */
					pDetectResult->FormatChanged = 1;
					pDetectResult->FmtDef = oFmtDef;
					pDetectResult->ch_cfg.FmtDef = oFmtDef;
					pDetectResult->port_cfg.output_interface = VI_INPUT_MODE_BT656;
					pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_1Multiplex;

					if( oFmtDef == AHD30_4M_30P  || oFmtDef == AHD30_4M_25P ||
						oFmtDef == AHD30_3M_30P  || oFmtDef == AHD30_3M_25P ||
						oFmtDef == AHD30_5M_20P  ||
#ifndef Customer_Support
						oFmtDef == AHD30_5_3M_20P||
						oFmtDef == AHD30_6M_18P  || oFmtDef == AHD30_6M_20P  ||
						oFmtDef == AHD30_8M_15P  || oFmtDef == AHD30_8M_12_5P||
#endif
						oFmtDef == CVI_4M_25P 	 || oFmtDef == CVI_4M_30P 	||
						oFmtDef == CVI_8M_12_5P  || oFmtDef == CVI_8M_15P 	||
						oFmtDef == TVI_4M_25P 	 || oFmtDef == TVI_4M_30P )
					{
							pDetectResult->port_cfg.edge = VI_CLK_EDGE_DOUBLE;
					}
					else
					{
						pDetectResult->port_cfg.edge = VI_CLK_EDGE_SINGLE_UP;
					}
					fprintf( stdout, ">>>>> CH[%d], Set video format : 0x%02X\n", ch, oCurVideofmt );
				}

			}
			else if( oCurVideofmt == NC_VIVO_CH_FORMATDEF_UNKNOWN )
			{
				/* Get VFC of B5/6/7/8 0xF0 */
				NC_VD_AUTO_VFC_STR sVFC;
				sVFC.Ch = oChannel;
				sVFC.Dev_Num = oDevNum;
				NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);
				oFmtDef = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);
				if(oFmtDef == NC_VIVO_CH_FORMATDEF_UNKNOWN)
				{
					NC_VD_VO_Auto_Data_Mode_Set( ch, oDevNum, OUT_MODE_OFF );
					/* set no video- first(i:channel, raptor3_vfmts:information */
					RAPTOR3_SAL_NoVIdeoSetFormat( ch, &s_raptor3_vfmts );

					/* save video format to prevideofmt */
					//NC_VD_AUTO_AFE_RST(&Dev_Ch_Info);
					NC_VD_AUTO_ONVIDEO_RST(&Dev_Ch_Info);
					usleep(500*1000);

					/* Change Status SoC Setting [Resolution & Frame rate] */
					pDetectResult->FormatChanged = 1;
					pDetectResult->FmtDef = CVI_HD_30P;
					pDetectResult->ch_cfg.FmtDef = CVI_HD_30P;
					pDetectResult->port_cfg.output_interface = VI_INPUT_MODE_BT656;
					pDetectResult->port_cfg.mux_mode = VI_WORK_MODE_1Multiplex;
					pDetectResult->port_cfg.edge = VI_CLK_EDGE_SINGLE_UP;

					fprintf( stdout, ">>>>> CH[%d], Set No video : 0x%02X\n", ch, oCurVideofmt );
				}
			}
		}

		/* save video format to prevideofmt */
		s_raptor3_vfmts.prevideofmt[ch] = s_raptor3_vfmts.curvideofmt[ch];
		/* display debug message */
		#if 0
		//for( i = 0; i < (s_raptor3_vfmts.chipinform.Total_Chip_Cnt*4); i++)
		{
			fprintf( stdout, "[Ch:%d] 0x%02x \n", ch, s_raptor3_vfmts.curvideofmt[ch] );
		}
			#endif

	}
	else if(!pDetect->AutoInfo.p_ThreadOn)
	{


	}
	pVdevDev->detection_process_inuse[ch]  = 0;
}
#endif

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


VDEC_DEV_S raptor3_dev0;
VDEC_DEV_S raptor3_dev1;
VDEC_DEV_S raptor3_dev2;
VDEC_DEV_S raptor3_dev3;

VDEC_DEV_S* decoder_dev0 = NULL;
VDEC_DEV_S* decoder_dev1 = NULL;
VDEC_DEV_S* decoder_dev2 = NULL;
VDEC_DEV_S* decoder_dev3 = NULL;

VDEC_DEV_INFORM_S chipinform;

unsigned char raptor3_chip_id[4];
unsigned char raptor3_chip_rev[4];
unsigned char raptor3_chip_addr[4];

unsigned char Chip_Cnt = 0;
unsigned char Total_Port_Num = 0;

void RAPTOR3_Chip_Information_Init(unsigned char* chip_id, unsigned char* chip_rev, unsigned char* chip_addr, VDEC_DEV_INFORM_S* DevInfo)
{
	int i;

	for( i = 0; i < 4; i++ )
	{
		chip_id[i] =  DevInfo->chip_id[i];
		chip_rev[i] = DevInfo->chip_rev[i];
		chip_addr[i] = DevInfo->chip_addr[i];
	}

	int chip;

	for(chip = 0; chip < 4; chip++)
	{
		switch(chip_id[chip])
		{
			case RAPTOR3_4PORT_R0_ID :
					if(chip == 0)
					{
						raptor3_dev0.i2c_dev_addr = chip_addr[chip];
						raptor3_dev0.i2c_dev_num = 0;
						raptor3_dev0.vid_port_total = Total_Port_Num+4;
						raptor3_dev0.vid_port_start = Total_Port_Num;

						decoder_dev0 = &raptor3_dev0;

					}
					else if(chip == 1)
					{
						raptor3_dev1.i2c_dev_addr = chip_addr[chip];
						raptor3_dev1.i2c_dev_num = 1;
						raptor3_dev1.vid_port_total = Total_Port_Num+4;
						raptor3_dev1.vid_port_start = Total_Port_Num;

						decoder_dev1 = &raptor3_dev1;

					}
					else if(chip == 2)
					{
						raptor3_dev2.i2c_dev_addr = chip_addr[chip];
						raptor3_dev2.i2c_dev_num = 2;
						raptor3_dev2.vid_port_total = Total_Port_Num+4;
						raptor3_dev2.vid_port_start = Total_Port_Num;

						decoder_dev2 = &raptor3_dev2;

					}
					else
					{
						raptor3_dev3.i2c_dev_addr = chip_addr[chip];
						raptor3_dev3.i2c_dev_num = 3;
						raptor3_dev3.vid_port_total = Total_Port_Num+4;
						raptor3_dev3.vid_port_start = Total_Port_Num;

						decoder_dev3 = &raptor3_dev3;

					}
					Chip_Cnt += 1;
					Total_Port_Num += 4;
					DBG_INFO("CHIP_ID[0x%x] CHIP_NUM[%d], CHIP_ADDR[0x%x] \n", chip_id[chip], chip, chip_addr[chip]);
			 break;
			case RAPTOR3_2PORT_R0_ID :
				if(chip == 0)
				{
					raptor3_dev0.i2c_dev_addr = chip_addr[chip];
					raptor3_dev0.i2c_dev_num = 0;
					raptor3_dev0.vid_port_total = Total_Port_Num+2;  // FIXME : KWANHO
					raptor3_dev0.vid_port_start = Total_Port_Num;

					decoder_dev0 = &raptor3_dev0;
				}
				else if(chip == 1)
				{
					raptor3_dev1.i2c_dev_addr = chip_addr[chip];
					raptor3_dev1.i2c_dev_num = 1;
					raptor3_dev1.vid_port_total = Total_Port_Num+2;
					raptor3_dev1.vid_port_start = Total_Port_Num;

					decoder_dev1 = &raptor3_dev1;
				}
				else if(chip == 2)
				{
					raptor3_dev2.i2c_dev_addr = chip_addr[chip];
					raptor3_dev2.i2c_dev_num = 2;
					raptor3_dev2.vid_port_total = Total_Port_Num+2;
					raptor3_dev2.vid_port_start = Total_Port_Num;

					decoder_dev2 = &raptor3_dev2;
				}
				else
				{
					raptor3_dev3.i2c_dev_addr = chip_addr[chip];
					raptor3_dev3.i2c_dev_num = 3;
					raptor3_dev3.vid_port_total = Total_Port_Num+2;
					raptor3_dev3.vid_port_start = Total_Port_Num;

					decoder_dev3 = &raptor3_dev3;
				}
				Chip_Cnt += 1;
				Total_Port_Num += 2;
				DBG_INFO("CHIP_ID[0x%x] CHIP_NUM[%d], CHIP_ADDR[0x%x] \n", chip_id[chip], chip, chip_addr[chip]);
			break;

			case RAPTOR3_1PORT_R0_ID :
				if(chip == 0)
				{
					raptor3_dev0.i2c_dev_addr = chip_addr[chip];
					raptor3_dev0.i2c_dev_num = 0;
					raptor3_dev0.vid_port_total = Total_Port_Num+1;
					raptor3_dev0.vid_port_start = Total_Port_Num;

					decoder_dev0 = &raptor3_dev0;

				}
				else if(chip == 1)
				{
					raptor3_dev1.i2c_dev_addr = chip_addr[chip];
					raptor3_dev1.i2c_dev_num = 1;
					raptor3_dev1.vid_port_total = Total_Port_Num+1;
					raptor3_dev1.vid_port_start = Total_Port_Num;

					decoder_dev1 = &raptor3_dev1;
				}
				else if(chip == 2)
				{
					raptor3_dev2.i2c_dev_addr = chip_addr[chip];
					raptor3_dev2.i2c_dev_num = 2;
					raptor3_dev2.vid_port_total = Total_Port_Num+1;
					raptor3_dev2.vid_port_start = Total_Port_Num;

					decoder_dev2 = &raptor3_dev2;
				}
				else
				{
					raptor3_dev3.i2c_dev_addr = chip_addr[chip];
					raptor3_dev3.i2c_dev_num = 3;
					raptor3_dev3.vid_port_total = Total_Port_Num+1;
					raptor3_dev3.vid_port_start = Total_Port_Num;

					decoder_dev3 = &raptor3_dev3;
				}
				Chip_Cnt += 1;
				Total_Port_Num += 1;
				DBG_INFO("CHIP_ID[0x%x] CHIP_NUM[%d], CHIP_ADDR[0x%x] \n", chip_id[chip], chip, chip_addr[chip]);
			break;
			case CHIP_ID_UNKNOWN :
				DBG_ERR("CHIP_ID_UNKNOWN  chip num[%d], chip address[%08x] \n",chip, chip_id[chip]);
			break;

			default :
				DBG_ERR("NOT_RAPTOR3_CHIP_ID  chip num[%d], chip id[0x%x], chip addr[0x%x]\n",chip, chip_id[chip], chip_addr[chip]);

		}
	}

	//COLOR_DBG_INFO(INFO_TEAL, "TOTAL CHIP CNT : [%d] TOTAL PORT CNT : [%d]", Chip_Cnt, Total_Port_Num);

	for(chip = 3; chip >= 0; chip--)
	{
		if(chip_addr[chip] != 0xff)
		{
			switch(chip)
			{
				case 3 : RAPTOR3_InitDevice( &raptor3_dev3 );
				case 2 : RAPTOR3_InitDevice( &raptor3_dev2 );
				case 1 : RAPTOR3_InitDevice( &raptor3_dev1 );
				case 0 : RAPTOR3_InitDevice( &raptor3_dev0 );
						 break;
				default : DBG_ERR(" Decoder No Detect \n");
						  break;
			}
			break;
		}

	}
}

static int __DVR_System_Set_Config(VDEC_DEV_S* device, VDEC_CH_CFG_S** ch_cfg, VDEC_PORT_CFG_S** port_cfg, unsigned char dev_num)
{
	if(device != NULL)
	{
		RAPTOR3_Video_SetConfig( device, ch_cfg[dev_num], port_cfg[dev_num],  0);
		RAPTOR3_Audio_Default_set();

		return 0;
	}
	else
	{
		DBG_ERR("de-activation device [%d]", dev_num);
		return -1;
	}
}

int RAPTOR3_SAL_Init( RAPTOR3_INFORMATION_S *pRaptorinformation )
{
	int i;

	/* Initialize VideoDecoder (RAPTOR3) */
	RAPTOR3_LoadDriver();

	/* Chip Information Read (Chip ID, Chip Rev) */
	RAPTOR3_Chip_Information_Get( &chipinform );

	memcpy( &pRaptorinformation->chipinform, &chipinform, sizeof(VDEC_DEV_INFORM_S));
	#if 1
	for( i = 0; i < 4; i++ )
	{
		fprintf( stdout, "[%d] CHIPID:0x%02x, CHIP_REV:0x%02x, CHIP_ADDR:0x%02x\n", \
				i, chipinform.chip_id[i], chipinform.chip_rev[i], chipinform.chip_addr[i] );
	}
	fprintf( stdout, "Total_Chip_Cnt : %d\n", chipinform.Total_Chip_Cnt );
	#endif

	/* set auto mode, but if you want to change mode from auto to manual mode, check this!(2017-07-30) */
	for( i = 0; i < ( chipinform.Total_Chip_Cnt*4); i++ )
	{
		NC_VD_AUTO_AutoMode_Set(i, i/4);
	}

	/* set audio(default) */
	RAPTOR3_Audio_Default_set();

	/* set mux according to the CHIPID */
	for( i = 0; i < 4; i++ )
	{
		if( pRaptorinformation->chipinform.chip_id[i] == RAPTOR3_2PORT_R0_ID )
		{
			pRaptorinformation->oMux = VI_2MULTIPLEX_MODE;
			fprintf( stdout, ">>>>> RAPTOR3_2PORT_R0_ID\n");
			break;
		}
		else
		{
			pRaptorinformation->oMux = VI_1MULTIPLEX_MODE;
			fprintf( stdout, ">>>>> RAPTOR3_4PORT_R0_ID\n");
			break;
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

	//DBG_INFO("video_fmt_debounce ch[%d] buf[0][0x%02x],buf[1][0x%02x],buf[2][0x%02x],buf[3][0x%02x]\n", \
	//		ch, debouncebuf[0], debouncebuf[1], debouncebuf[2], debouncebuf[3]);

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

	//DBG_INFO("video_fmt_debounce ch[%d] buf[0][0x%02x],buf[1][0x%02x],buf[2][0x%02x],buf[3][0x%02x]\n", \
	//		ch, debouncebuf[0], debouncebuf[1], debouncebuf[2], debouncebuf[3]);

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

	/* For Debug Ch1 Only */
	/*
	if( ch == 0)
		printf("debunce:0x%02X, debncIdx:%d\n", pInformation->debounce[ch][pInformation->debounceidx[ch]], pInformation->debounceidx[ch]);
	*/
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
		if( ( oFmtB5Def != AHD30_5M_20P ) && ( oFmtB5Def != pInformation->prevideofmt[ch] ) )
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

	NC_VD_AUTO_NoVideo_Set(oChannel, devnum);

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( CVI_HD_30P_EX );
	//NC_VD_VO_Mode_Set( devnum, iPort, pPortFmt, oMux, oInterface, oClkEdge, ch%4, ch%4, ch%4, ch%4 );
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

    NC_VD_VI_NEW_FORMAT_Set(&DEV_CH_INFO);
	
	//NC_VD_VO_Mode_Set( oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );
    NC_VD_VO_Mode_Set_New( oChannel, oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );

	return 0;
}

unsigned int gChannelChangedForDemo[4] = { 0, };

void Raptor3_ThreadforDEMO_171024( void )
{
	int ii;
	int oNovideo;
	NC_VD_AUTO_NOVIDEO_STR sNoVideo;
	NC_VD_VO_SEQ_STR VOutSeq;

	while(1)
	{
		for(ii = 0; ii < 4; ii++)
		{
			sNoVideo.Ch = ii % 4;
			sNoVideo.Dev_Num = ii / 4;
			VOutSeq.devnum	= ii / 4;

			NC_VD_AUTO_NOVIDEO_Get(&sNoVideo);
			oNovideo = sNoVideo.NoVid;

			if(!oNovideo && !gChannelChangedForDemo[ii])
			{
				VOutSeq.Port = 0;

				switch(ii)
				{
					case 0 : VOutSeq.ChNum01 = 0x10;
							 VOutSeq.ChNum23 = 0x32;
							break;
					case 1 : VOutSeq.ChNum01 = 0x01;
							 VOutSeq.ChNum23 = 0x32;
							break;
					case 2 : VOutSeq.ChNum01 = 0x32;
							 VOutSeq.ChNum23 = 0x10;
							break;
					case 3 : VOutSeq.ChNum01 = 0x23;
							 VOutSeq.ChNum23 = 0x10;
							break;
					default :
						break;
				}

				NC_VD_VO_CHN_Sequence_Set(&VOutSeq);

				/* Display ON */
				NC_VD_VO_Auto_Data_Mode_Set(ii % 4, ii / 4, OUT_MODE_ON);
				DBG_INFO(" >>>> Change Show Channel : [%d]\n", ii);

				gChannelChangedForDemo[ii] = 1;
			}
			else if( oNovideo )
			{
				NC_VD_VO_Auto_Data_Mode_Set(ii % 4, ii / 4, OUT_MODE_OFF);
				gChannelChangedForDemo[ii] = 0;
			}
			else
			{
				if(oNovideo &&
				   !gChannelChangedForDemo[0] &&
				   !gChannelChangedForDemo[1] &&
				   !gChannelChangedForDemo[2] &&
				   !gChannelChangedForDemo[3]
				   )
				{

				}
			}
		}
		usleep(300*1000);
	}

}
