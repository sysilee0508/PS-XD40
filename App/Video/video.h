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
#define MDIN_MULTI_DEVICE

#ifdef MDIN_MULTI_DEVICE		// added by kukuri

typedef enum {
	VIDEO_INPUT_NONE = 0,
	VIDEO_DIGITAL_NVP6158_A,
	VIDEO_DIGITAL_NVP6158_B,
	VIDEO_DIGITAL_NVP6158_AB
} MDIN_VIDEO_INPUT_t;


typedef enum
{
	COLOR_RGB,
	COLOR_YBR
} eColorFormat_t;

#define 		MDIN_I2C_L				0
#define 		MDIN_I2C_H				1
#define 		SELECT_MDIN(x)			selectedMDIN = x		

#else

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
#define		VIDEO_EXTERNAL_IN			6		   			//by hungry 2012.02.15
#define		VIDEO_VDCNV_4CH_IN			7					//by hungry 2012.02.15
#endif

#if defined(SYSTEM_USE_MDIN380)
#define		VIDEO_VDCNV_TVP5160			0
#define		VIDEO_VDCNV_TW2866			1
#define		VIDEO_ADCNV_COMPONENT		2
#define		VIDEO_ADCNV_PC_RGB			3
#define		VIDEO_DIGITAL_HDMI			4
#define		VIDEO_DIGITAL_SDI			5				   //by hungry 2012.02.15
#define		VIDEO_VDCNV_4CH_IN			6
#define		VIDEO_ADCNV_2HD_IN			7
#define		VIDEO_DIGITAL_SDI2			8				   //by flcl 2013.04.23
#define		VIDEO_SDI_2HD_POP			9
#endif

#endif	//MDIN_MULTI_DEVICE kukuri


#define RGB_WHITE					RGB(255, 255, 255)
#define RGB_BLACK					RGB(0,0,0)
#define RGB_GRAY					RGB(128, 128, 128)

#define YBR_WHITE					RGB(128, 235, 128)
#define YBR_BLACK					RGB(128, 16, 128)
#define YBR_GRAY					RGB(128, 125, 128)

#define WHITE(x)						(x == COLOR_RGB) ? RGB_WHITE : YBR_WHITE
#define BLACK(x)						(x == COLOR_RGB) ? RGB_BLACK : YBR_BLACK
#define GRAY(x)						(x == COLOR_RGB) ? RGB_BLACK : YBR_GRAY

#define ADJUST_WINDOW_STEP_MAX	20

// ----------------------------------------------------------------------
// Exported Variables
// ----------------------------------------------------------------------
extern MDIN_VIDEO_INFO		stVideo[MDIN_ID_MAX];

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
void ConfigI2C(MDIN_CHIP_ID_t mdin);
MDIN_OUTVIDEO_FORMAT_t GetOutVideoFormat(MDIN_CHIP_ID_t mdin);
void CreateVideoInstance(void);
void InitInputSource(void);
void SetInputSource(BYTE input);
void SetInputChanged(void);
void VideoProcessHandler(void);
void VideoHTXCtrlHandler(void);
void SetAuxOutMode_C(void);
eColorFormat_t GetCurrentColorFormat(void);
void CreateDisplayWindow_A(eDisplayMode_t displayMode);
void CreateDisplayWindow_B(eDisplayMode_t displayMode);
void CreateDisplayWindow_C(eDisplayMode_t displayMode);
void UpdateCroppingScreen(eChannel_t channel);

#endif	/* __VIDEO_H__ */
