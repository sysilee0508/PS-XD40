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
#include <raptor3_fmt.h>

#define Thread_On  g_VFC_NOVIDEO[CH1].p_ThreadOn && g_AT_STATUS[CH1].p_ThreadOn
#define Thread_Off !g_VFC_NOVIDEO[CH1].p_ThreadOn && !g_AT_STATUS[CH1].p_ThreadOn

#define AUTO_TEST	1

#define VFC_TEST_SET_VAL 0x63
#define VFC_DEFAULT_SET_VAL 0xFF

#define CHIP_AUTO_SET_TEST
#define NO_POWER_OFF_CAMERA_TEST 0
//#define FORCE_RESET_TEST

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

typedef enum AUTO_VIDEO_STATUS
{
	FLAG_ON_VIDEO,
	FLAG_NO_VIDEO,

}AUTO_VIDEO_STATUS;

typedef enum DONE_CLEAR
{
	CLEAR,
	DONE,

}DONE_CLEAR;

typedef enum
{
	OUT_MODE_ON = 0x01,
	OUT_MODE_OFF = 0x0F
}DATA_OUT_MODE_NUM;

///////////////////////////////////// VERIFICATION ////////////////////////////////////
typedef struct _VERIFICATION_VAL
{
	unsigned char VFC_VAL;
	unsigned char NOVID_VAL;
	unsigned int SAM_VAL;
	unsigned int HSYNC_1;
	unsigned int HSYNC_2;
	unsigned int HSYNC_RESULT;
	unsigned int DIST_VAL;
	unsigned int DIST_COUNTER;
	unsigned int STABLE_COUNTER;
	unsigned char AGC_VAL;
	unsigned int FSC_1;
	unsigned int FSC_2;
	unsigned int FSC_RESULT;
	unsigned int ACC_GAIN_VAL;
	long PROCESS_TIME;
	long VFC_PROCESS_TIME;
	long SAM_PROCESS_TIME;


}VERIFICATION_VAL;

typedef struct _VERIFICATION_VAL_0330
{
	unsigned char B0_0xE0;  // 170330 VERIFICATION ONLY
	unsigned char B5_0xF0;	// 170330 VERIFICATION ONLY
	unsigned char B5_0xF1;	// 170330 VERIFICATION ONLY
	unsigned char B13_0xF0;	// 170330 VERIFICATION ONLY
}VERIFICATION_VAL_0330;

typedef struct _VERIFICATION_FLAG
{
	unsigned int DIST_FLAG;
	unsigned int VFC_FLAG;
	unsigned int SAM_FLAG;
	unsigned int HSYNC_FLAG;
	unsigned int AGC_FLAG;
	unsigned int FSC_FLAG;
	unsigned int ACC_GAIN_FLAG;
	unsigned int TIME_FLAG;
}VERIFICATION_FLAG;

//////////////////////////////////// COMMON ////////////////////////////////////
typedef struct _NC_VD_VI_AUTO_DETECT_REG_STR{
	unsigned char Ch;
	unsigned char DevNum;
	unsigned char DCmp;    		//B5/6/7/8 0x03
	unsigned char SliceLevel;  	//B5/6/7/8 0x08
	unsigned char ControlMode;  	//B5/6/7/8 0x47
	unsigned char GDFFixCoeff;  	//B5/6/7/8 0x50
	unsigned char DFERefSel;  	//B5/6/7/8 0x76
	unsigned char WPD_77;    	//B5/6/7/8 0x77
	unsigned char WPD_78;    	//B5/6/7/8 0x78
	unsigned char WPD_79;    	//B5/6/7/8 0x79
	unsigned char AutoGnosMode; 	//B5/6/7/8 0x82
	unsigned char AutoSyncMode; 	//B5/6/7/8 0x83
	unsigned char HafcBypass;  	//B5/6/7/8 0xB8

	unsigned char NoVidVFCInit; //B13 	   0x31 				// 170204 추가
	unsigned char StableMode1;	//B13	   0x0
	unsigned char StableMode2;	//B13	   0x1

	unsigned char NoVidDETECT;  //B0	 	   0x23/0x27/0x2B/0x2F  // 170204 추가

	NC_VD_VI_VAFE_STR VAfe;
}NC_VD_VI_AUTO_DETECT_REG_STR;

typedef struct _NC_VD_AUTO_DIST_STR{ // 170207 Cable Distance 항목 추가
 unsigned char Ch;
 unsigned char devnum;
 unsigned char Dist;					// B13 0xA0
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
 /*
 unsigned char SAMval_CD;			// B13 0xCD [7:0]
 unsigned char SAMval_CC;			// B13 0xCC [9:8]
 */
 unsigned int SAMval;
}NC_VD_AUTO_SAM_STR;

typedef struct _NC_VD_AUTO_HSYNC_STR{ // 170207 Hsync Accumulation
 unsigned char Ch;
 unsigned char dev_addr;
 unsigned char h_lock;				// Bank 0 0xE2 [3:0] [Ch3:Ch0]
 unsigned int Hsync_Accum_Val1;		// Value 1  			// 170210 Add
 unsigned int Hsync_Accum_Val2;		// Value 2				// 170210 Add
 unsigned int Hsync_Accum_Result;	// Value 1 - Value 2	// 170210 Fix
}NC_VD_AUTO_HSYNC_STR;

typedef struct _NC_VD_AUTO_AGC_STR{ // 170207 AGC Value 항목 추가
 unsigned char Ch;
 unsigned char devnum;
 unsigned char agc_lock;			// Bank 0 0xE0 [3:0] [Ch3:Ch0]
 unsigned char AGCval;				// B13 0xB8
}NC_VD_AUTO_AGC_STR;

typedef struct _NC_VD_AUTO_FSC_STR {	// 170214 FSC Value Read
	unsigned char Ch;
	unsigned char devnum;
	unsigned int FSC_Val1;				// B13 0xB4 [7:0] /0xB5 [15:8] /0xB6 [23:16] /0xB7 [31:24]
	unsigned int FSC_Val2;				// B13 0xB8 [7:0] /0xB9 [15:8] /0xBA [23:16] /0xBB [31:24]
	unsigned int FSC_Final;				// B13 0xBC [7:0] /0xBD [15:8] /0xBE [23:16] /0xBF [31:24]
}NC_VD_AUTO_FSC_STR;

typedef struct _NC_VD_AUTO_ACC_GAIN_STR{	// 170215 acc gain value read
	unsigned char Ch;
	unsigned char devnum;
	unsigned int AccGain;
        // DEBUG
	unsigned char FuncSel;
}NC_VD_AUTO_ACC_GAIN_STR;

typedef struct _NC_VD_AUTO_STATUS_INFO_STR {
	unsigned int 	p_ThreadOn;
	unsigned char	AGC_Stable_Check;
	NC_VD_AUTO_CABLE_DIST_STR Dist;
	NC_VD_AUTO_SAM_STR SAM;
	NC_VD_AUTO_HSYNC_STR HsyncAccum;
	NC_VD_AUTO_AGC_STR AGC;
	NC_VD_AUTO_FSC_STR FSC; // 170214 FSC Add
	NC_VD_AUTO_ACC_GAIN_STR ACC; // 170308 ACC GAIN Add
}NC_VD_AUTO_STATUS_INFO_STR;  // 170208 Add

typedef struct _NC_VD_VI_SLEEP_TIME_STR{	// 170215 acc gain value read
	unsigned char SleepTimeVal;
}NC_VD_VI_SLEEP_TIME_STR;

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

typedef struct _NC_VD_AUTO_NOVIDEO_FLAG_STR
{
	unsigned int NoVidCnt;
	AUTO_VIDEO_STATUS NoVidFlag;
}NC_VD_AUTO_NOVIDEO_FLAG_STR;

typedef struct _NC_VD_AUTO_ONVIDEO_STR{
	unsigned char Ch;
	unsigned char AutoGnosMode;
	unsigned char AutoSyncMode;
	unsigned char OpMd;			//B5/6/7/8 0xB8
}NC_VD_AUTO_ONVIDEO_STR;

typedef struct _NC_VD_VI_AT_ONVIDEO_2_STR{
	unsigned char Ch;
	unsigned char DFERefSel;  	//B5/6/7/8 0x76
	unsigned char WPD_77;    	//B5/6/7/8 0x77
	unsigned char WPD_78;    	//B5/6/7/8 0x78
	unsigned char WPD_79;    	//B5/6/7/8 0x79

	unsigned char Slice_Mode;	//B5/6/7/8 0x0E
}NC_VD_VI_AT_ONVIDEO_2_STR;

typedef struct _NC_VD_AUTO_NOVIDEO_REG_STR{
	unsigned char Ch;
	unsigned char devnum;	
	unsigned char ControlMode;
	unsigned char GDFFixCoeff;
	unsigned char AutoGnosMode;
	unsigned char AutoSyncMode;
	unsigned char HafcBypass;
	unsigned char DFERefSel;  	//B5/6/7/8 0x76
	unsigned char WPD_77;    	//B5/6/7/8 0x77
	unsigned char WPD_78;    	//B5/6/7/8 0x78
	unsigned char WPD_79;    	//B5/6/7/8 0x79

	unsigned char Slice_Mode;
}NC_VD_AUTO_NOVIDEO_REG_STR;

typedef struct _NC_VD_AUTO_VFC_NOVIDEO_STR {	// 170207 VFC, NOVID Struct
	unsigned int  	AutoDetectFlag;
	unsigned int 	p_ThreadOn;
	NC_VD_AUTO_VFC_STR VFC;
	NC_VD_AUTO_NOVIDEO_STR NOVID;
}NC_VD_AUTO_VFC_NOVIDEO_STR;

typedef struct _NC_VD_AUTO_THREAD_STR {	// 170207 VFC, NOVID Struct
	unsigned char  	AutoDetectFlag;
	unsigned char 	p_ThreadOn;
}NC_VD_AUTO_THREAD_STR;

typedef struct _NC_VD_VI_AT_VFC_SET_EN_STR{ // 170215	temp
	unsigned char Ch;
	unsigned char SetVal;			// B13 0x1F
}NC_VD_VI_AT_VFC_SET_EN_STR;

typedef struct _NC_VD_AUTO_DATA_OUT_MODE_STR{	// 170329
	unsigned char Ch;
	unsigned char devnum;
	unsigned char SetVal;
}NC_VD_AUTO_DATA_OUT_MODE_STR;

typedef struct _NC_VD_AUTO_MANUAL_MODE_STR{	// 170329
	unsigned char Ch;
	unsigned char dev_addr;
}NC_VD_AUTO_MANUAL_MODE_STR;

typedef struct _NC_VD_AUTO_HSCALER_TEST_STR{	// 170410 HScaler Setting Test
	unsigned char Ch;
	unsigned char SetEn;
}NC_VD_AUTO_HSCALER_TEST_STR;

typedef struct _NC_VD_AUTO_DETECT_FLAG_STR {
	unsigned char AutoDetect_Flag;
	unsigned char NoVideo_Flag;
}NC_VD_AUTO_DETECT_FLAG_STR;

typedef struct _NC_VD_AUTO_DETECT_INFO_STR {

	unsigned char autodetect_thread;

	 NC_VD_AUTO_DETECT_FLAG_STR	Flag;
	 NC_VD_AUTO_VFC_STR 	VFC;
	 NC_VD_AUTO_NOVIDEO_STR	NoVideo;
	 NC_VD_AUTO_THREAD_STR	AutoInfo;

}NC_VD_AUTO_DETECT_INFO_STR;

typedef struct _NC_VD_AUTO_ONVIDEO_CHECK_STR{
	unsigned char VFC;
	unsigned char SW_Rst;
	NC_VD_DEV_CH_INFO_STR Info;
}NC_VD_AUTO_ONVIDEO_CHECK_STR;


///////////////////////////////// CABLE_DISTANCE_DETECT /////////////////////////////////

typedef struct _NC_VD_VI_AT_FMT_SET_DONE_STR{ // 170209 Format Set Done
	unsigned char Ch;
 	 unsigned char Done;					// B13 0x70 [3:0] each channel
}NC_VD_VI_AT_FMT_SET_DONE_STR;

typedef struct _NC_VD_VI_AT_AGC_RESET_STR{	// 170221 AGC Reset
	unsigned char Ch;
	unsigned char ResetVal;
}NC_VD_VI_AT_AGC_RESET_STR;

///////////////////////////////// EQ /////////////////////////////////

typedef struct _NC_VD_VI_AT_AEQ_STR{ // 170214 AEQ Set
	unsigned char Ch;
	unsigned char AEQ_Val;				//B5/6/7/8 0x58       [7:4]
}NC_VD_VI_AT_AEQ_STR;

typedef struct _NC_VD_VI_AT_DEQ_STR{ // 170214 DEQ Set
	unsigned char Ch;
	unsigned char DEQ_Val;				// B9 0x80/0xA0/0xC0/0xE0 [3:0]
}NC_VD_VI_AT_DEQ_STR;

typedef struct _NC_VD_VI_AT_AEQ_DEQ_STR{ // 170214
	NC_VD_VI_AT_AEQ_STR AEQ;
	NC_VD_VI_AT_DEQ_STR DEQ;		// B13 0x70 [3:0] each channel
}NC_VD_VI_AT_EQ_STR;

typedef struct _NC_VD_EQ_STR{
	unsigned char Ch;
	unsigned char Dev_Num;		// Device Address
	unsigned char distance;
	unsigned char FmtDef;
} NC_VD_EQ_STR;

//////////////////////////////////////////////////////////////////////
void _PRINT_NOVIDEO_INFO_2_GUI(unsigned char Ch);
void _PRINT_MANUALMODE_INFO_2_GUI(unsigned char Ch);

/*
 * Auto Detection Flag Set Function
 */
void NC_VD_AUTO_FLAG_ON_SET(NC_VD_AUTO_DETECT_INFO_STR* pDetectData);
void NC_VD_AUTO_FLAG_OFF_SET(NC_VD_AUTO_DETECT_INFO_STR* pDetectData);

/*
 * Check Function*
 */
unsigned char NC_APP_VD_AUTO_ONVIDEO_Check(NC_VD_AUTO_VFC_STR *pVFC, NC_VD_AUTO_DETECT_FLAG_STR *pFlag);
unsigned char NC_APP_VD_AUTO_NOVIDEO_Check(NC_VD_AUTO_DETECT_FLAG_STR *pFlag);
unsigned char NC_APP_VD_AGC_STABLE_Check(NC_VD_AUTO_HSYNC_STR *pHsync, NC_VD_AUTO_AGC_STR* pAGC ,NC_VD_AUTO_SAM_STR *pSAM);

/*
 * Auto Detection Data Read Function
 */
void NC_APP_VD_AUTO_VFC_Get(NC_VD_AUTO_VFC_STR* pVFC, NC_VD_AUTO_DETECT_FLAG_STR* pFlag);
AUTO_VIDEO_STATUS NC_APP_VD_AUTO_NOVIDEO_Get(unsigned char Ch,NC_VD_AUTO_NOVIDEO_STR *pNoVideo);
CABLE_DISTANCE NC_APP_VD_AUTO_CABLE_DISTANCE_Get(unsigned char Ch);
CABLE_DISTANCE NC_VD_AUTO_CABLE_DISTANCE_Get (unsigned char Ch, NC_VD_AUTO_CABLE_DIST_STR *pDistance);
CABLE_DISTANCE NC_APP_VD_MANUAL_CABLE_DISTANCE_Get(unsigned char Ch, NC_VIVO_CH_FORMATDEF FmtDef );
void NC_VD_AUTO_SAM_Get (unsigned char Ch, NC_VD_AUTO_SAM_STR *pSAM);
void NC_VD_AUTO_HSYNC_Get (unsigned char Ch, NC_VD_AUTO_HSYNC_STR* pHsync);
void NC_VD_AUTO_AGC_Get (unsigned char Ch, NC_VD_AUTO_AGC_STR* pAGC);
void NC_VD_AUTO_ACC_GAIN_Get(unsigned char Ch, ACC_DEBUG FuncSel);
void NC_VD_AUTO_FSC_Get (unsigned char Ch, NC_VD_AUTO_FSC_STR* pFSC);

/*
 * Auto Detection Data Write Function
 */
void NC_APP_VD_AUTO_ONVIDEO_Set(unsigned char Ch, RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult , NC_VD_AUTO_VFC_STR *pVFC);
void NC_APP_VD_AUTO_NOVIDEO_Set(unsigned char Ch , RAPTOR3_FMT_DETECT_RESULT_S *pDetectResult, NC_VD_AUTO_DETECT_FLAG_STR *pFlag );
void NC_APP_VD_AUTO_VIDEO_EQ_Set(unsigned char Ch, unsigned int CableDistance,NC_VD_AUTO_DETECT_FLAG_STR *pFlag );
void NC_APP_VD_MANUAL_VIDEO_EQ_Set(unsigned char Ch, CABLE_DISTANCE Dist, NC_VIVO_CH_FORMATDEF FmtDef );
void NC_VD_AUTO_VIDEO_EQ_Set(unsigned char Ch, unsigned int CableDistance);
void NC_VD_AUTO_AutoMode_Set(unsigned char ch, unsigned char DevNum);
void NC_VD_VO_Auto_Data_Mode_Set(unsigned char ch, unsigned char devnum, unsigned char SetVal);

#endif /* _RAPTOR3_VIDEO_AUTO_DETECT_H_ */
