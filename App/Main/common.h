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

#include "stm32f10x.h"

#include "mdintype.h"

#include "main.h"
#include "key.h"
#include "rtc.h"
#include "osd_display.h"
#include "Sequence.h"

#include "mcu.h"
#include "i2c.h"

#include "delay.h"

#include "mdin3xx.h"

#include "video.h"
#include "mfccoef.h"

#include "osd.h"
#include "osdmenu.h"
#include "menu.h"
#include "item.h"


// msg.c
typedef enum {
	MSG_KEY = 0, MSG_IR, MSG_UART, MSG_USB
} 	MSG_CTRL_MODE;

typedef enum {
	MSG_LEVEL = 0, MSG_SHIFT, MSG_ZMOVE
} 	MSG_EXE_SPEED;


#define TIME_AFTER(unknown,known)	(((long)(known)-(long)(unknown))<0)

#define NOP()	asm("NOP")

#define LOW		0
#define HIGH	1

#define FALSE	0
#define TRUE	1

#define CLEAR	0
#define SET		1

// -----------------------------------------------------------------------------
// External Variables declaration
// -----------------------------------------------------------------------------
extern volatile BOOL fZOOMMove, fCROPMove;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------


#endif	/* __COMMON_H__ */
