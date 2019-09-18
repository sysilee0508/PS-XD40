//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  COMMON.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#ifndef		__COMMON_H__
#define		__COMMON_H__


// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "mdintype.h"
#include "mdin3xx.h"

#include "main.h"
#include "key.h"
#include "Rtc.h"
#include "Sequence.h"
#include "i2c.h"
#include "MCU.h"
#include "delay.h"
#include "mfccoef.h"
#include "timer.h"

#include "osd.h"
//#include "osdmenu.h"
#include "menu.h"
//#include "item.h"
#include "video_loss.h"

#include "MotionDetect.h"
#include "alarm_remotekey.h"
#include "display_mode.h"
#include "video.h"
#include "osd_display.h"
#include "NVP6168.h"
#include "nv_storage.h"

#define NOP()	asm("NOP")

#define CLEAR							0

#define NVP6158			0
#define NVP6168			1

#define BD_NVP			NVP6168

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

extern BYTE M380_ID;

typedef enum
{
	I2C_OK = 0,
	I2C_NOT_FREE,
	I2C_NOT_START,
	I2C_HOST_NACK,
	I2C_NO_DATA,
	I2C_TIME_OUT
}I2C_ErrorCode;

typedef enum {
	FADE_IN = 0,
	FADE_OUT,
	DARK,
	BRIGHT,
	FADE_IN_NO
}FADEIN_NO_t;

// -----------------------------------------------------------------------------
// External Variables declaration
// -----------------------------------------------------------------------------

#endif	/* __COMMON_H__ */
