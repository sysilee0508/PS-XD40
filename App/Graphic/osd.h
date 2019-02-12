//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  OSD.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#ifndef		__OSD_H__
#define		__OSD_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
typedef	struct
{
	DWORD	addr_m;
	DWORD	addr_s;

	WORD	w_m;
	WORD	h_m;
	WORD	w_s;
	WORD	h_s;

}	stPACKED OSD_4CHVIEW_INFO, *POSD_4CHVIEW_INFO;

#define		OSD_RGB_PALETTE		0
#define		OSD_YUV_PALETTE		1

// ----------------------------------------------------------------------
// Exported Variables
// ----------------------------------------------------------------------
//extern GACMAP_FONT_INFO stFONT;
extern SPRITE_CTL_INFO stOSD[12];
extern LAYER_CTL_INFO stLayer[3];
//extern CURSOR_CTL_INFO stCurCTL;
//extern SBOX_CTL_INFO stSBOX[8];
//extern OSD_4CHVIEW_INFO st4CH;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
void CreateOSDInstance(void);
void OSD_ModifyPalette_M(BOOL rgb);
void OSD_ModifyPalette_X(BOOL rgb);
void OSD_SetFontGAC(SPRITE_INDEX_t index);
void OSD_EnableSprite(SPRITE_INDEX_t index, BOOL OnOff);
//void OSD_SetFontMAP(void);	// for framebuffer map bug
//void OSD_SetDemo(void);

//void OSD_SetSprite_layer0(void);
//void OSD_SetSprite_layer1(WORD x, WORD y);
//void OSD_SetSprite_layer2(WORD x, WORD y);


#endif	/* __OSD_H__ */
