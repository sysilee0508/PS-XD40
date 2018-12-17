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


#if 0 //kukuri
// vdcnv.c
#define		VDCNV_NTSC					0
#define		VDCNV_PAL					1
#define		VDCNV_AUTO				2
#define		VDCNV_NONE				3

typedef enum {
	VD2866_PORT_A = 0,
	VD2866_PORT_B,
	VD2866_PORT_AB,
	VD2866_PORT_4CH
}	VD2866_PORT_t;


typedef enum {
	DEMO_DEINT_FILM_DISABLE = 0,	// disables film mode
	DEMO_DEINT_FILM_32,				// enables 3:2 film mode only
	DEMO_DEINT_FILM_22,				// enables 2:2 film mode only
	DEMO_DEINT_FILM_ALL = 4			// enables both 3:2 and 2:2 film modes

}	DEMO_DEINT_FILM_t;

typedef enum {
	DEMO_DEINT_FAST_DISABLE = 0,	// disables fsat mode
	DEMO_DEINT_FAST_NORMAL,			// enables normal fast mode
	DEMO_DEINT_FAST_VERY			// enables very fast mode

}	DEMO_DEINT_FAST_t;

typedef enum {
	DEMO_DEINT_EDGE_DISABLE = 0,	// vertical (90-deg) interpolation only
	DEMO_DEINT_EDGE_45 = 2,			// 90-deg and 45-deg interpolation
	DEMO_DEINT_EDGE_26 = 3,			// up to 26-deg interpolation
	DEMO_DEINT_EDGE_ALL = 7			// all angle interpolation

}	DEMO_DEINT_EDGE_t;


typedef enum {
	DEMO_DISPLAY_PIP_OFF	= 0,	// disables pip/pop
	DEMO_DISPLAY_CROP_PIP,			// display pip crop
//	DEMO_DISPLAY_ONLY_PIP,			// display pip only
	DEMO_DISPLAY_ONLY_PIP1,			// display pip only
	DEMO_DISPLAY_ONLY_PIP2,			// display pip only
	DEMO_DISPLAY_ONLY_PIP3,			// display pip only
	DEMO_DISPLAY_ONLY_PIP4,			// display pip only
	DEMO_DISPLAY_HALF_POP,			// display pop half size
	DEMO_DISPLAY_FULL_POP,			// display pop full
	//DEMO_DISPLAY_FULL2_POP,			// display pop full
	DEMO_DISPLAY_SIZE_POP			// display pop size

}	DEMO_DISPLAY_PIP_t;

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
// video.c
//void test_stVideo_print(void);

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
//void Change_Main_Aux_Input(BYTE sel);

#if 0 //kukuri

// demo.c
void DEMO_SetInputSource(WORD nID);		// 1.SetUp
void DEMO_SetOutputFormat(WORD nID);
void DEMO_SetOutputMode(WORD nID);
//void DEMO_Set4ChMode(WORD nID);
//void DEMO_Set2HDInput(WORD nID);
void DEMO_SetPIPDisplay(WORD nID);
void DEMO_SetEncoderPath(WORD nID);
//void DEMO_SetDualDisplay(WORD nID);
void DEMO_SetOverScanning(WORD nID);
void DEMO_SetInputPattern(WORD nID);
void DEMO_SetOutputPattern(WORD nID);
void DEMO_SetImageMirrorV(WORD nID);

void DEMO_SetInputFormat(WORD nID);		// 2.E-Video
void DEMO_SetInputMode(WORD nID);
void DEMO_SetActiveRegion(WORD nID);
void DEMO_SetCbCr422Swap(WORD nID);
void DEMO_SetClockDelay(WORD nID);
void DEMO_SetInFLDBypass(WORD nID);
void DEMO_SetTopFLDLevel(WORD nID);
void DEMO_SetHACTOffset(WORD nID);
void DEMO_SetVACTOffset(WORD nID);

void DEMO_SetDeintMode(WORD nID);		// 3.Deinterlace
void DEMO_SetEdgeAngle(WORD nID);
void DEMO_SetMotionBorder(WORD nID);
void DEMO_SetSlowMotion(WORD nID);
void DEMO_SetFastMotion(WORD nID);
void DEMO_SetFilmMode(WORD nID);
void DEMO_SetFilmBadEdit(WORD nID);
void DEMO_SetNoiseRobust1(WORD nID);
void DEMO_SetNoiseRobust2(WORD nID);
void DEMO_SetZoomArtifact(WORD nID);
void DEMO_SetInterAreaMode(WORD nID);

void DEMO_Set3DNRMode(WORD nID);		// 4.Enhancer
void DEMO_SetCrossColor(WORD nID);
void DEMO_SetVD3DComb(WORD nID);
void DEMO_SetEdgeEnhance(WORD nID);
void DEMO_SetBlockNR(WORD nID);
void DEMO_SetMosquitoNR(WORD nID);
void DEMO_SetColorTone(WORD nID);
void DEMO_SetChromaError(WORD nID);

void DEMO_SetContrast(WORD nID);		// 5.Picture
void DEMO_SetBrightness(WORD nID);
void DEMO_SetSaturation(WORD nID);
void DEMO_SetHue(WORD nID);
void DEMO_SetSharpness(WORD nID);
void DEMO_Set3DNRLevel(WORD nID);
void DEMO_SetBWExtension(WORD nID);
void DEMO_SetLTIControl(WORD nID);
void DEMO_SetCTIControl(WORD nID);

void DEMO_SetMFCHYFilter(WORD nID);		// 6.Filters
void DEMO_SetMFCHCFilter(WORD nID);
void DEMO_SetMFCVYFilter(WORD nID);
void DEMO_SetMFCVCFilter(WORD nID);
void DEMO_SetFNRYCFilter(WORD nID);

BOOL DEMO_GetGraphicDisplay(void);
void DEMO_SetDemo1OSDDisplay(WORD nID);
void DEMO_SetDemo2OSDDisplay(WORD nID);
void DEMO_SetCURDisplay(WORD nID);

void DEMO_SetMDINSoftReset(void);		// SoftRST util

void DEMO_SetWindowPIPPOP(WORD nID);	// PIP/POP util
//void DEMO_SetPIPPosition(WORD nID);
//void DEMO_SetPIPSizeWIND(WORD nID);

//void DEMO_EnableRectBGBOX(BOOL OnOff);


void DEMO_SetPIPViewWIND(WORD nID);

void SetOSDMenuRefresh(void);


//void DEMO_SetAudInputSource(WORD nID);	// Audio. //by hungry 2012.03.14

void TEST_UpdateMFCCoef(void);
void TEST_SetUpdateMFC(BOOL OnOff);
void TEST_UpdateFRBSize(void);
void TEST_SetUpdateFRB(BOOL OnOff);
void TEST_UpdateRSTCtrl(void);
void TEST_SetUpdateRST(BOOL OnOff);
/*		//by hungry 2012.03.07
// adcnv.c
void ADCNV_EnableOutVideo(BOOL bOnOff);
void ADCNV_SetVideoSystem(BYTE mode);
BYTE ADCNV_GetVideoSystem(void);
BYTE ADCNV_GetPCVGASystem(void);

// vdcnv.c
//--------------------------------------------------------------------------------------------------------------------------
void VD5160_SetRegInitial(void);
void VD5160_EnableOutVideo(BOOL OnOff);
BYTE VD5160_GetVideoFormat(void);
BYTE VD5160_GetVideoSystem(void);
void VD5160_SetVideoSystem(BYTE mode);
void VD5160_Enable3DComb(BOOL OnOff);
void VD5160_SetContrast(BYTE value);
void VD5160_SetBrightness(BYTE value);
void VD5160_SetSaturation(BYTE value);
void VD5160_SetHue(BYTE value);


#if	defined(SYSTEM_USE_MDIN325) || defined(SYSTEM_USE_MDIN325A) || defined(SYSTEM_USE_MDIN380)
void VD2866_SetRegInitial(void);
void VD2866_EnableOutVideo(VD2866_PORT_t port, BOOL OnOff);
BYTE VD2866_GetVideoFormat(void);
void VD2866_SetVideoFormat(BYTE mode);
BYTE VD2866_GetVideoSystem(void);
void VD2866_SetVideoSystem(BYTE mode);
void VD2866_SetContrast(BYTE value);
void VD2866_SetBrightness(BYTE value);
void VD2866_SetSaturation(BYTE value);
void VD2866_SetHue(BYTE value);
#endif	*//*	defined(SYSTEM_USE_MDIN325) || defined(SYSTEM_USE_MDIN325A) || defined(SYSTEM_USE_MDIN380)	*/

/*	  //by hungry 2012.03.07
#if	defined(SYSTEM_USE_MDIN340)
void VD9910_SetRegInitial(void);
BYTE VD9910_GetVideoSystem(void);
void VD9910_SetVideoSystem(BYTE mode);
void VD9910_EnableOutVideo(BOOL OnOff);
void VD9910_SetContrast(BYTE value);
void VD9910_SetBrightness(BYTE value);
void VD9910_SetSaturation(BYTE value);
void VD9910_SetHue(BYTE value);
#endif	*//*	defined(SYSTEM_USE_MDIN340)	*/


//sdi_rx.c						//by hungry 2012.02.10
//--------------------------------------------------------------------------------------------------------------------------
BYTE SDIRX_GetVideoSystem(void);
void SDIRX_ShowStatus(void);
void MDIN_Status(void);		//by hungry 2012.03.16
void SDIRX_Input_Resolution(void);	  //by flcl 2012.12.06

extern u8 SDIRX_change_flag;


//sdi_tx.c						//by hungry 2012.02.16
//--------------------------------------------------------------------------------------------------------------------------
void SDITX_CtrlHandler(PMDIN_VIDEO_INFO pINFO);
void SDITX_ShowStatus(void);

#endif



#endif	/* __VIDEO_H__ */
