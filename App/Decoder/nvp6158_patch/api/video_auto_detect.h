/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Auto detection
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _RAPTOR3_VIDEO_AUTO_DETECT_H_
#define _RAPTOR3_VIDEO_AUTO_DETECT_H_

#include <video_decoder.h>
#include "raptor3_api.h"
#include "raptor3_api_drv.h"

unsigned char *arrVfcName[0x100];
NC_VIVO_CH_FORMATDEF arrVfcType[0x100];

typedef enum ACC_DEBUG
{
	ACC_GAIN_NORMAL,
	ACC_GAIN_DEBUG,

}ACC_DEBUG;

typedef enum DISTANCE
{
	SHORT_2M,
	LONG_100M,
	LONG_200M,
	LONG_300M,
	LONG_400M,
	LONG_500M,
}CABLE_DISTANCE;

typedef enum ON_OFF
{
	OFF,
	ON,

}ON_OFF;

typedef enum
{
	OUT_MODE_ON = 0x01,
	OUT_MODE_OFF = 0x0F
}DATA_OUT_MODE_NUM;

//////////////////////////////////// COMMON ////////////////////////////////////
typedef struct _NC_VD_VI_AUTO_DETECT_REG_STR{
	unsigned char Ch;
	unsigned char DevNum;
}NC_VD_VI_AUTO_DETECT_REG_STR;

typedef struct _NC_VD_AUTO_DIST_STR{ 
 unsigned char Ch;
 unsigned char devnum;
 unsigned char Dist;				
}NC_VD_AUTO_CABLE_DIST_STR;

typedef struct _NC_VD_MANUAL_DIST_STR{
 unsigned char Ch;
 unsigned char dev_addr;
 unsigned char Dist;
 unsigned char FmtDef;
 unsigned char cabletype;				// 0:coax, 1:utp, 2:reserved1, 3:reserved2
}NC_VD_MANUAL_CABLE_DIST_STR;

typedef struct _NC_VD_AUTO_SAM_STR{ // 170207 SAM Value 항목 추가
 unsigned char Ch;
 unsigned char dev_addr;
 unsigned int SAMval;
}NC_VD_AUTO_SAM_STR;

typedef struct _NC_VD_AUTO_HSYNC_STR{ 
 unsigned char Ch;
 unsigned char dev_addr;
 unsigned char h_lock;				// Bank 0 0xE2 [3:0] [Ch3:Ch0]
 unsigned int Hsync_Accum_Val1;		// Value 1  			
 unsigned int Hsync_Accum_Val2;		// Value 2				
 unsigned int Hsync_Accum_Result;	// Value 1 - Value 2	
}NC_VD_AUTO_HSYNC_STR;

typedef struct _NC_VD_AUTO_AGC_STR{
 unsigned char Ch;
 unsigned char devnum;
 unsigned char agc_lock;			// Bank 0 0xE0 [3:0] [Ch3:Ch0]
 unsigned char AGCval;				// B13 0xB8
}NC_VD_AUTO_AGC_STR;

typedef struct _NC_VD_AUTO_ACC_GAIN_STR{	
	unsigned char Ch;
	unsigned char devnum;
	unsigned int AccGain;
        // DEBUG
	unsigned char FuncSel;
}NC_VD_AUTO_ACC_GAIN_STR;

///////////////////////////////// VIDEO_FORMAT_DETECT /////////////////////////////////
typedef struct _NC_VD_AUTO_VFC_STR{
 unsigned char Ch;
 unsigned char VFC;				//B5/6/7/8 0xf0
 unsigned char Dev_Num;		// Device Address
}NC_VD_AUTO_VFC_STR;

// NO Video Read 					// 170204 No Video Read 항목 추가
typedef struct _NC_VD_AUTO_NOVIDEO_STR{
 unsigned char Ch;
 unsigned char NoVid;				// B0 0xA8
 unsigned char Dev_Num;
}NC_VD_AUTO_NOVIDEO_STR;

typedef struct _NC_VD_AUTO_NOVIDEO_REG_STR{
	unsigned char Ch;
	unsigned char devnum;	
}NC_VD_AUTO_NOVIDEO_REG_STR;

typedef struct _NC_VD_AUTO_DATA_OUT_MODE_STR{	// 170329
	unsigned char Ch;
	unsigned char devnum;
	unsigned char SetVal;
}NC_VD_AUTO_DATA_OUT_MODE_STR;

///////////////////////////////// EQ /////////////////////////////////

typedef struct _NC_VD_EQ_STR{
	unsigned char Ch;
	unsigned char Dev_Num;		// Device Address
	unsigned char distance;
	unsigned char FmtDef;
} NC_VD_EQ_STR;

/*
 * Check Function*
 */
unsigned char NC_APP_VD_AGC_STABLE_Check(NC_VD_AUTO_HSYNC_STR *pHsync, NC_VD_AUTO_AGC_STR* pAGC ,NC_VD_AUTO_SAM_STR *pSAM);

/*
 * Auto Detection Data Read Function
 */
CABLE_DISTANCE NC_APP_VD_AUTO_CABLE_DISTANCE_Get(unsigned char Ch);
CABLE_DISTANCE NC_VD_AUTO_CABLE_DISTANCE_Get (unsigned char Ch, NC_VD_AUTO_CABLE_DIST_STR *pDistance);
CABLE_DISTANCE NC_APP_VD_MANUAL_CABLE_DISTANCE_Get(unsigned char Ch, NC_VIVO_CH_FORMATDEF FmtDef );
void NC_VD_AUTO_SAM_Get (unsigned char Ch, NC_VD_AUTO_SAM_STR *pSAM);
void NC_VD_AUTO_HSYNC_Get (unsigned char Ch, NC_VD_AUTO_HSYNC_STR* pHsync);
void NC_VD_AUTO_AGC_Get (unsigned char Ch, NC_VD_AUTO_AGC_STR* pAGC);
void NC_VD_AUTO_ACC_GAIN_Get(unsigned char Ch, ACC_DEBUG FuncSel);

/*
 * Auto Detection Data Write Function
 */
void NC_APP_VD_MANUAL_VIDEO_EQ_Set(unsigned char Ch, CABLE_DISTANCE Dist, NC_VIVO_CH_FORMATDEF FmtDef );
void NC_VD_AUTO_VIDEO_EQ_Set(unsigned char Ch, unsigned int CableDistance);
void NC_VD_AUTO_AutoMode_Set(unsigned char ch, unsigned char DevNum);
void NC_VD_VO_Auto_Data_Mode_Set(unsigned char ch, unsigned char devnum, unsigned char SetVal);

#endif /* _RAPTOR3_VIDEO_AUTO_DETECT_H_ */
