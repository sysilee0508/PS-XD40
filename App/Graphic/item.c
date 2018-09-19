//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	ITEM.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"common.h"

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
static WORD OSDItemID;
static BOOL fItemExecute;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
void SetOSDItemID(WORD nID)
{
	OSDItemID = nID;
	fItemExecute = ON;
	SetOSDCombID(nID);	// for VOUT key
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeIPCMode(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_SetDeintMode(OSDItemID); break;		// IPC Mode
		case  2: DEMO_SetEdgeAngle(OSDItemID); break;		// Edge Angle
		case  3: DEMO_SetMotionBorder(OSDItemID); break;	// Motion Border
		case  4: DEMO_SetFastMotion(OSDItemID); break;		// Fast Motion
		case  5: DEMO_SetSlowMotion(OSDItemID); break;		// Slow Motion
		case  6: DEMO_SetZoomArtifact(OSDItemID); break;		// Block Match
		case  7: DEMO_SetNoiseRobust1(OSDItemID); break;	// Noise Robust1
		case  8: DEMO_SetFilmMode(OSDItemID); break;		// Film Mode
		case  9: DEMO_SetFilmBadEdit(OSDItemID); break;		// Film BadEdit
		case 10: DEMO_SetInterAreaMode(OSDItemID); break;	// Inter Area
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeNRMode(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_Set3DNRMode(OSDItemID); break;		// 3DNR Mode
		case  3: DEMO_SetMosquitoNR(OSDItemID); break;		// Mosquito NR
		case  4: DEMO_SetVD3DComb(OSDItemID); break;		// TVP 3D Comb
		case  5: DEMO_SetCrossColor(OSDItemID); break;		// Cross Color
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeEnhance(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_SetBWExtension(OSDItemID); break;		// B/W Extension
		case  2: DEMO_SetColorTone(OSDItemID); break;		// Color Tone
		case  3: DEMO_SetEdgeEnhance(OSDItemID); break;		// Edge Enhance
//		case  7: DEMO_SetChromaError(OSDItemID); break;		// Chroma Error
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeInOutMode(BYTE nID)
{
	switch (nID) {
//		case  1: DEMO_Set4ChMode(OSDItemID); break;		// [4xD1 Mux]		   //by hungry 2012.03.15
//		case  2: DEMO_Set2HDInput(OSDItemID); break;		// [2HD Input]	   //by hungry 2012.03.15
//		case  3: DEMO_SetPIPDisplay(OSDItemID); break;		// PIP display	   //by hungry 2012.03.15
//		case  4: DEMO_SetDualDisplay(OSDItemID); break;		// Dual display	   //by hungry 2012.03.15
		case  5: DEMO_SetOverScanning(OSDItemID); break;	// Over Scanning
		case  6: DEMO_SetAspectRatio(OSDItemID); break;		// Aspect Ratio
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExePicture(BYTE nID)
{
	switch (nID) {
		case  5: DEMO_SetLTIControl(OSDItemID); break;		// LTI
		case  6: DEMO_SetCTIControl(OSDItemID); break;		// CTI
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeSetUp(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_SetInputSource(OSDItemID); break;		// input source
		case  2: DEMO_SetOutputFormat(OSDItemID); break;	// out resolution
		case  3: DEMO_SetOutputMode(OSDItemID); break;		// out port mode
		case  4: DEMO_SetEncoderPath(OSDItemID); break;		// Encoder path
		case  5: DEMO_SetInputPattern(OSDItemID); break;	// in pattern
		case  6: DEMO_SetOutputPattern(OSDItemID); break;	// out pattern
		case  7: DEMO_SetImageMirrorV(OSDItemID); break;	// V-mirror
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeFilter(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_SetMFCHYFilter(OSDItemID); break;		// MFC HY filter
		case  2: DEMO_SetMFCHCFilter(OSDItemID); break;		// MFC HC filter
		case  3: DEMO_SetMFCVYFilter(OSDItemID); break;		// MFC VY filter
		case  4: DEMO_SetMFCVCFilter(OSDItemID); break;		// MFC VC filter
		case  5: DEMO_SetFNRYCFilter(OSDItemID); break;		// FNR YC filter
	}
}
/*		  //by hungry 2012.03.20
//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExeEVideo(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_SetInputFormat(OSDItemID); break;		// input format
		case  2: DEMO_SetInputMode(OSDItemID); break;		// in port mode
		case  3: DEMO_SetActiveRegion(OSDItemID); break;	// Sync polarity
		case  4: DEMO_SetCbCr422Swap(OSDItemID); break;	// CbCr422 swap
		case  5: DEMO_SetClockDelay(OSDItemID); break;		// clock delay
		case  6: DEMO_SetInFLDBypass(OSDItemID); break;	// InFLD bypass
		case  7: DEMO_SetTopFLDLevel(OSDItemID); break;	// TopFLD level
	}
}
*/
//--------------------------------------------------------------------------------------------------------------------------
/*static void ItemCombExeAudio(BYTE nID)			  //by hungry 2012.03.14
{
	switch (nID) {
		case  1: DEMO_SetAudInputSource(OSDItemID); break;		// input source(ADC/Playback)
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------
static void ItemProgExeNR(BYTE nID)
{
	switch (nID) {
		case  2: DEMO_Set3DNRLevel(OSDItemID); break;		// 3d NR levels
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemProgExeEnhance(BYTE nID)
{
	switch (nID) {
		case  4: DEMO_SetSharpness(OSDItemID); break;		// sharpness
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemProgExePicture(BYTE nID)
{
	switch (nID) {
		case  1: DEMO_SetContrast(OSDItemID); break;		// contrast
		case  2: DEMO_SetBrightness(OSDItemID); break;		// brightness
		case  3: DEMO_SetSaturation(OSDItemID); break;		// saturation
		case  4: DEMO_SetHue(OSDItemID); break;				// hue
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemProgExeEVideo(BYTE nID)
{
	switch (nID) {
		case  8: DEMO_SetHACTOffset(OSDItemID); break;		// H-Offset
		case  9: DEMO_SetVACTOffset(OSDItemID); break;		// V-Offset
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemCombExecute(BYTE nID)
{
	switch (HI4BIT(nID)) {
		case  1: ItemCombExeIPCMode(LO4BIT(nID)); break;
		case  2: ItemCombExeNRMode(LO4BIT(nID)); break;
		case  3: ItemCombExeEnhance(LO4BIT(nID)); break;
		case  4: ItemCombExeInOutMode(LO4BIT(nID)); break;
		case  5: ItemCombExePicture(LO4BIT(nID)); break;
		case  6: ItemCombExeSetUp(LO4BIT(nID)); break;
		case  7: ItemCombExeFilter(LO4BIT(nID)); break;
//		case  8: ItemCombExeEVideo(LO4BIT(nID)); break;			   //by hungry 2012.03.14
//		case  8: ItemCombExeAudio(LO4BIT(nID)); break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
static void ItemProgExecute(BYTE nID)
{
	switch (HI4BIT(nID)) {
		case  2: ItemProgExeNR(LO4BIT(nID)); break;
		case  3: ItemProgExeEnhance(LO4BIT(nID)); break;
		case  5: ItemProgExePicture(LO4BIT(nID)); break;
		case  8: ItemProgExeEVideo(LO4BIT(nID)); break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void MenuItemExeHandler(void)
{
	WORD nID = GetOSDMenuID();

	if (fItemExecute==OFF) return;
	else	   fItemExecute = OFF;

//	UARTprintf("nID = %04X\n", nID);
	if (HI4BIT(LOBYTE(nID))) ItemCombExecute(HIBYTE(nID));
	else					 ItemProgExecute(HIBYTE(nID));

}

/*  FILE_END_HERE */
