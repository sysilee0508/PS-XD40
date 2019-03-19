/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Decoder api header
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _RAPTOR3_RAPTOR3_API_H_
#define _RAPTOR3_RAPTOR3_API_H_

#include <video_decoder.h>

#define RAPTOR3_MAX_CH_PER_CHIP 4
#define RAPTOR3_MAX_PORT_PER_CHIP 4

#define RAPTOR3_4PORT_R0_ID 0xA1
#define RAPTOR3_2PORT_R0_ID 0xA0
#define RAPTOR3_1PORT_R0_ID 0xA2
#define CHIP_ID_UNKNOWN 	0xFF

#define MAX_DEBOUNCE_CNT	  5

#define DEV0 0
#define DEV1 1
#define DEV2 2
#define DEV3 3

#define RAPTOR3_MAX_CH ( RAPTOR3_MAX_CH_PER_CHIP * RAPTOR3_MAX_PORT_PER_CHIP )

#define VI_1MULTIPLEX_MODE  0
#define VI_2MULTIPLEX_MODE	1

typedef enum _NC_VIDEO_ONOFF
{
	VIDEO_LOSS_ON = 0,
	VIDEO_LOSS_OFF = 1,

} NC_VIDEO_ONOFF;

typedef struct _RAPTOR3_INFORMATION_S
{
	unsigned char			oMux;	/* 0:1mux,  */
	NC_VIVO_CH_FORMATDEF 	curvideofmt[ RAPTOR3_MAX_CH ];
	NC_VIVO_CH_FORMATDEF 	prevideofmt[ RAPTOR3_MAX_CH ];
	unsigned char 		 	curvideoloss[ RAPTOR3_MAX_CH ];
	unsigned char			vfc[RAPTOR3_MAX_CH];
	unsigned char			debounce[RAPTOR3_MAX_CH][MAX_DEBOUNCE_CNT];
	unsigned char			debounceidx[RAPTOR3_MAX_CH];
	VDEC_DEV_INFORM_S 		chipinform;

} RAPTOR3_INFORMATION_S;

typedef struct RAPTOR3_FMT_DETECT_RESULT_S
{
	int FormatChanged;
	int NoVideo_F;
	int OnVideo_F;
	NC_VIVO_CH_FORMATDEF FmtDef;

	VDEC_CH_CFG_S ch_cfg;
	VDEC_PORT_CFG_S port_cfg;

	unsigned int CableDistance;

} RAPTOR3_FMT_DETECT_RESULT_S;

typedef struct _RAPTOR3_AUTO_DETECT_FLAG_STR {
	unsigned char AutoDetect_Flag;
	unsigned char NoVideo_Flag;
}RAPTOR3_AUTO_DETECT_FLAG_STR;

typedef struct _RAPTOR3_AUTO_DETECT_VFC_STR{
 unsigned char Ch;
 unsigned char VFC;				//B5/6/7/8 0xf0
 unsigned char Dev_Num;		// Device Address
}RAPTOR3_AUTO_DETECT_VFC_STR;

typedef struct _RAPTOR3_AUTO_DETECT_NOVIDEO_STR{
 unsigned char Ch;
 unsigned char NoVid;				// B0 0xA8
 unsigned char Dev_Num;
}RAPTOR3_AUTO_DETECT_NOVIDEO_STR;

typedef struct _RAPTOR3_AUTO_DETECT_INFO_STR {	// 170207 VFC, NOVID Struct
	unsigned char  	AutoDetectFlag;
	unsigned char 	p_ThreadOn;
}RAPTOR3_AUTO_DETECT_INFO_STR;

typedef struct _RAPTOR3_AUTO_DETECT_DATA_STR {

	unsigned char autodetect_thread;

	RAPTOR3_AUTO_DETECT_FLAG_STR		Flag;
	RAPTOR3_AUTO_DETECT_VFC_STR 		VFC;
	RAPTOR3_AUTO_DETECT_NOVIDEO_STR		NoVideo;
	RAPTOR3_AUTO_DETECT_INFO_STR		AutoInfo;

}RAPTOR3_AUTO_DETECT_DATA_STR;

int RAPTOR3_SAL_GetFormat( RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_GetFormatEachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_GetFormat_Bank5_EachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_OnVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_NoVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_AutoDebouceCheck( unsigned char, RAPTOR3_INFORMATION_S *pInformation );

void RAPTOR3_Chip_Information_Init(unsigned char* chip_id, unsigned char* chip_rev, unsigned char* chip_addr, VDEC_DEV_INFORM_S* DevInfo);
void RAPTOR3_PortCfg_Init(VDEC_CH_CFG_S** ch_cfg, VDEC_PORT_CFG_S** port_cfg);
/////////////////////////////////////// Video Channel, Port Setting //////////////////////////////////////

void RAPTOR3_Video_SetConfig( VDEC_DEV_S *pVdevDev, VDEC_CH_CFG_S *ch_cfg, VDEC_PORT_CFG_S *port_cfg, RAPTOR3_AUTO_DETECT_DATA_STR* pDetectData );
int RAPTOR3_Video_GetConfig( VDEC_DEV_S *pVdevDev, VDEC_CH_CFG_S *ch_cfg, VDEC_PORT_CFG_S *port_cfg );
/////////////////////////////////////// Format Detection /////////////////////////////////////////////////


void RAPTOR3_AutoDetection_Init( FMT_DETECT_CONFIG_S *pDetectCfg, RAPTOR3_AUTO_DETECT_DATA_STR *pDetectData );
void RAPTOR3_AutoDetection_DoDetection( FMT_DETECT_CONFIG_S *pDetectCfg, RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult, RAPTOR3_AUTO_DETECT_DATA_STR *pDetectData);
void RAPTOR3_AutoDetection_Information_Read_Thread( FMT_DETECT_CONFIG_S *pDetectCfg );


/////////////////////////////////////// Motion ///////////////////////////////////////////////////////////
void RAPTOR3_Motion_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_Pixel_All_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch,  unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_Pixel_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch,  unsigned char sel, unsigned char fmtdef);
unsigned char RAPTOR3_Motion_Pixel_OnOff_get(VDEC_DEV_S *pVdevDev, unsigned char ch,  unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_TSEN_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_PSEN_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef);


/////////////////////////////////////// Coaxial ///////////////////////////////////////////////////////////

#include "coax_cmd_def.h"

int RAPTOR3_Coax_FW_Upgrade_Init(VDEC_DEV_S *pVdevDev, int vi_ch);
void RAPTOR3_Coax_FW_File_Get(VDEC_DEV_S *pVdevDev, int vi_ch, char *pFileName, int select);
int RAPTOR3_Coax_FW_Progress_Status_Get( void );
void RAPTOR3_Coax_FW_Upadate_Start(VDEC_DEV_S *pVdevDev, int vi_ch, int select);

// Coaxial all channel Tx, Rx initialize
void RAPTOR3_Coax_Tx_Rx_Init_Channel( int Ch );

// Coaxial Protoco Tx
void RAPTOR3_Coax_Tx_Init_Channel(VDEC_DEV_S *pVdevDev, int vi_ch );
void RAPTOR3_Coax_Tx_Send_Command(VDEC_DEV_S *pVdevDev, int vi_ch, NC_COAX_CMD_DEF cmd );

// Coaxial Protoco Test
void RAPTOR3_Coax_Tx_Test_Shot_Set(VDEC_DEV_S *pVdevDev, int vi_ch, int Val );
void RAPTOR3_Coax_Tx_Test_Init_Val_Set(VDEC_DEV_S *pVdevDev, int menu, int Val );
void RAPTOR3_Coax_Test_Tx_Init_Val_Get(VDEC_DEV_S *pVdevDev, int menu );
void RAPTOR3_Coax_Rx_Test_EQ_Filter_Set(VDEC_DEV_S *pVdevDev, int vi_ch, int sel );

void RAPTOR3_Coax_Test_Addr_Val_Set(VDEC_DEV_S *pVdevDev, int slave_addr, int bank, int addr, int val );
int RAPTOR3_Coax_Test_Addr_Val_Get(VDEC_DEV_S *pVdevDev,int slave_addr, int bank, int addr );

// Coaxial Protoco Rx
NC_FORMAT_STANDARD RAPTOR3_Coax_Rx_Init(VDEC_DEV_S *pVdevDev, int vi_ch );
void RAPTOR3_Coax_Rx_DeInit(VDEC_DEV_S *pVdevDev, int vi_ch );
void RAPTOR3_Coax_Rx_Buffer_Clear(VDEC_DEV_S *pVdevDev, int vi_ch );
void RAPTOR3_Coax_Rx_Val_Read(VDEC_DEV_S *pVdevDev, int vi_ch, NC_FORMAT_STANDARD format_standard );

NC_FORMAT_STANDARD RAPTOR3_Coax_NC_FORMAT_STANDARD_Read(VDEC_DEV_S *pVdevDev, int vi_ch);

void RAPTOR3_SK_2M_60P_Rx_Test_Set( int ch );
void RAPTOR3_SK_2M_50P_Rx_Test_Set( int ch );

void Raptor3_ThreadforDEMO_171024( void );

#endif
