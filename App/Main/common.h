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
#include "nv_storage.h"
#include "video.h"
#include "osd_display.h"

#define NOP()	asm("NOP")

#define CLEAR							0

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// External Variables declaration
// -----------------------------------------------------------------------------

#endif	/* __COMMON_H__ */
