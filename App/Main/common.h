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

#include "mdintype.h"
#include "nv_storage.h"
#include "main.h"
#include "key.h"
#include "Rtc.h"
#include "osd_display.h"
#include "Sequence.h"

#include "MCU.h"
#include "i2c.h"

#include "delay.h"

#include "mdin3xx.h"

#include "video.h"
#include "mfccoef.h"

#include "osd.h"
#include "osdmenu.h"
#include "menu.h"
#include "item.h"

#include "alarm_remotekey.h"
#include "timer.h"


// msg.c
typedef enum {
	MSG_KEY = 0, MSG_IR, MSG_UART, MSG_USB
} 	MSG_CTRL_MODE;

typedef enum {
	MSG_LEVEL = 0, MSG_SHIFT, MSG_ZMOVE
} 	MSG_EXE_SPEED;


//#define TIME_AFTER(unknown,known)	(((long)(known)-(long)(unknown))<0)
//#define TIME_BEFORE(unknown,known)	(((long)(unknown)-(long)(known))<0)

#define NOP()	asm("NOP")

// -----------------------------------------------------------------------------

#define CHAR_WIDTH_E					12
#define CHAR_WIDTH_K					24
#define CHAR_HEIGHT						24


// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// External Variables declaration
// -----------------------------------------------------------------------------
extern volatile BOOL fZOOMMove, fCROPMove;

#endif	/* __COMMON_H__ */
