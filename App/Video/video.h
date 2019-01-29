//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  VIDEO.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#ifndef		__VIDEO_H__
#define		__VIDEO_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
// video.c

#if defined(SYSTEM_USE_MDIN340)
#define		VIDEO_VDCNV_TVP5160			0
#define		VIDEO_VDCNV_TW9910			1
#define		VIDEO_DIGITAL_HDMI			2
//#define		VIDEO_EXTERNAL_IN				3		   //by hungry 2012.02.15
#define		VIDEO_DIGITAL_SDI			3				   //by hungry 2012.02.15
#define		VIDEO_EXTERNAL_IN			4				   //by hungry 2012.02.15
#endif

#if defined(SYSTEM_USE_MDIN325)||defined(SYSTEM_USE_MDIN325A)
#define		VIDEO_VDCNV_TVP5160			0
#define		VIDEO_VDCNV_TW2866			1
#define		VIDEO_ADCNV_COMPONENT		2
#define		VIDEO_ADCNV_PC_RGB			3
#define		VIDEO_DIGITAL_HDMI			4
#define		VIDEO_DIGITAL_SDI			5				   	//by hungry 2012.02.15
//#define		VIDEO_EXTERNAL_IN			5		   		//by hungry 2012.02.15
#define		VIDEO_EXTERNAL_IN			6		   			//by hungry 2012.02.15
//#define		VIDEO_VDCNV_4CH_IN			6				//by hungry 2012.02.15
#define		VIDEO_VDCNV_4CH_IN			7					//by hungry 2012.02.15
#endif

#if defined(SYSTEM_USE_MDIN380)
#define		VIDEO_VDCNV_TVP5160			0
#define		VIDEO_VDCNV_TW2866			1
#define		VIDEO_ADCNV_COMPONENT		2
#define		VIDEO_ADCNV_PC_RGB			3
#define		VIDEO_DIGITAL_HDMI			4
//#define		VIDEO_EXTERNAL_IN				5		   //by hungry 2012.02.15
#define		VIDEO_DIGITAL_SDI			5				   //by hungry 2012.02.15
#define		VIDEO_VDCNV_4CH_IN			6
#define		VIDEO_ADCNV_2HD_IN			7
#define		VIDEO_DIGITAL_SDI2			8				   //by flcl 2013.04.23
#define		VIDEO_SDI_2HD_POP			9
#endif

typedef enum
{
	RESOLUTION_1920_1080_60P,
	RESOLUTION_1920_1080_50P,
	RESOLUTION_MAX
} eResolution_t;

// ----------------------------------------------------------------------
// Exported Variables
// ----------------------------------------------------------------------
extern MDIN_VIDEO_INFO		stVideo;
extern MDIN_INTER_WINDOW	stInterWND;
extern MDIN_VIDEO_WINDOW	stZOOM, stCROP;

extern BYTE AdjInterWND,  InputSelect, InputSelOld,  SrcSyncInfo;
extern BYTE SrcMainFrmt, PrevSrcMainFrmt, SrcMainMode, PrevSrcMainMode;
extern BYTE OutMainFrmt, PrevOutMainFrmt, OutMainMode, PrevOutMainMode;
extern BYTE SrcAuxFrmt, PrevSrcAuxFrmt, SrcAuxMode, PrevSrcAuxMode;
extern BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
extern BYTE AdcVideoFrmt, PrevAdcFrmt, EncVideoFrmt, PrevEncFrmt;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
void CreateVideoInstance(void);
void VideoProcessHandler(void);
void VideoHTXCtrlHandler(void);
void VideoSetEdgeEnhance(BYTE mode);
void VideoSetAspectRatio(BYTE mode);
void VideoSetOverScanning(BYTE mode);
void VideoSetMFCHYFilter(BYTE mode);
void VideoSetMFCHCFilter(BYTE mode);
void VideoSetMFCVYFilter(BYTE mode);
void VideoSetMFCVCFilter(BYTE mode);
void VideoSetOutCSCCoef(BYTE mode);
void VideoSetInCSCCoef(BYTE mode);
void VideoSetIPCNoiseRobust1(BOOL OnOff);
void VideoSetIPCNoiseRobust2(BOOL OnOff);
void VideoSetIPCSlowMotion(BOOL OnOff);
void VideoSDITXCtrlHandler(void);	   	//by hungry 2012.02.23
void SetVideoOutputfrmt(BYTE frmt);

void UpdateVideoResolution(eResolution_t resolution);

#endif	/* __VIDEO_H__ */
