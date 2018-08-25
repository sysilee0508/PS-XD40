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

#include "..\drivers\mdintype.h"

#include "..\main\main.h"
#include "..\main\key.h"
#include "..\main\rtc.h"
#include "..\main\osd_display.h"
#include "..\main\Sequence.h"

#include "..\mcu\mcu.h"
#include "..\mcu\i2c.h"
#include "..\mcu\spi.h"

#include "..\etc\delay.h"

#include "..\drivers\mdin3xx.h"

#include "..\video\video.h"
#include "..\video\mfccoef.h"

#include "..\graphic\osd.h"
#include "..\graphic\osdmenu.h"
#include "..\graphic\menu.h"
#include "..\graphic\item.h"


// msg.c
typedef enum {
	MSG_KEY = 0, MSG_IR, MSG_UART, MSG_USB
} 	MSG_CTRL_MODE;

typedef enum {
	MSG_LEVEL = 0, MSG_SHIFT, MSG_ZMOVE
} 	MSG_EXE_SPEED;


#define TIME_AFTER(unknown,known)	(((long)(known)-(long)(unknown))<0)
#define TIME_BEFORE(unknown,known)	(((long)(unknown)-(long)(known))<0)

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
extern volatile BOOL fUSBXferMode, fZOOMMove, fCROPMove;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------


#endif	/* __COMMON_H__ */
