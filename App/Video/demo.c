// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"

#if 1
// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

#if	defined(SYSTEM_USE_MDIN380)
// default value for PIP-CROP window
/*static ROMDATA MDIN_VIDEO_WINDOW defPIPCropWND[][1]		= {
	{ // 720x480
		{ 360,  240,    0,    0},
	},
	{ // 540x360
		{ 270,  180,    0,    0},
	},
	{ // 360x240
		{ 180,  120,    0,    0},
	}
};*/

// default value for PIP-VIEW window
static ROMDATA MDIN_VIDEO_WINDOW defPIPViewWND_1680X1050[][4] = //1680X1050
{
	{ // 630x354
		{ 630,  354,   42,   38}, { 630,  354, 1008,   38},	// left/up, right/up
		{ 630,  354,   42,  658}, { 630,  354, 1008,  658},	// left/down, right/down
	}
};
static ROMDATA MDIN_VIDEO_WINDOW defPIPViewWND_1440X900[][4] = 	//1440X900
{
	{ // 538x302
		{ 538,  302,   38,   32}, { 538,  302, 864,   32},	// left/up, right/up
		{ 538,  302,   38,  566}, { 538,  302, 864,  566},	// left/down, right/down
	}
};
static ROMDATA MDIN_VIDEO_WINDOW defPIPViewWND_1360X768[][4] =  //1360X768
{
	{ // 510x286
		{ 510,  286,   36,   28}, { 510,  286, 814,   28},	// left/up, right/up
		{ 510,  286,   36,  454}, { 510,  286, 814,  454},	// left/down, right/down
	}
};
static ROMDATA MDIN_VIDEO_WINDOW defPIPViewWND_1280X1024[][4] = //1280X1024
{
	{ // 480x360
		{ 480,  360,   34,   38}, { 480,  360, 766,   38},	// left/up, right/up
		{ 480,  360,   34,  626}, { 480,  360, 766,  626},	// left/down, right/down
	}
};
static ROMDATA MDIN_VIDEO_WINDOW defPIPViewWND_1024X768[][4] = 	//1024X768
{
	{ // 384x288
		{ 384,  288,   26,   28}, { 384,  288, 614,   28},	// left/up, right/up
		{ 384,  288,   26,  452}, { 384,  288, 614,  452},	// left/down, right/down
	}
};

#endif	/*	defined(SYSTEM_USE_MDIN380)	*/

// default value for B/W extension
static ROMDATA MDIN_BWPOINT_COEF defMDINBWPoint[]	= {
	{	{  16,  28,  48, 70}, {  16,  50,  70, 76}	},	// type1 [in|out]
	{	{ 32,  64, 192, 224}, { 15,  64, 192, 238}	},	// type2 [in|out]
	{	{ 25,  64, 192, 228}, { 43,  64, 192, 208}	},	// type3 [in|out]
};

static BOOL fDemo1OSD, fDemo2OSD, fCUR32;
#if	defined(SYSTEM_USE_MDIN380)
static BYTE	pipID, popID;
#endif	/*	defined(SYSTEM_USE_MDIN380)	*/

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Exported functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
void DEMO_SetInputSource(WORD nID)
{
	InputSelect = nID;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetOutputFormat(WORD nID)
{
	OutMainFrmt = nID;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetOutputMode(WORD nID)
{
	OutMainMode = nID;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetPIPDisplay(WORD nID)
{
#if	defined(SYSTEM_USE_MDIN380)
	WORD sID = (nID==DEMO_DISPLAY_CROP_PIP)? 0 : nID;

	stVideo.exeFLAG |= MDIN_UPDATE_MAINFMT;
	stVideo.dspFLAG  = MDIN_AUX_DISPLAY_ON;
	stVideo.srcPATH  = (sID==0)? PATH_MAIN_A_AUX_A : PATH_MAIN_A_AUX_B;
	stVideo.dacPATH  = (nID==0)? DAC_PATH_MAIN_OUT : DAC_PATH_MAIN_PIP;
	stVideo.encPATH  = (nID==0)? VENC_PATH_PORT_X : VENC_PATH_PORT_B;

	memset(&stVideo.stVIEW_m, 0, 8);		// clear stVIEW_m
	memset(&stVideo.stVIEW_x, 0, 8);		// clear stVIEW_x
	memset(&stVideo.stCROP_x, 0, 8);		// clear stCROP_x
	OutMainFrmt = GetMenuStatus(6,2);		// get out-format
	OutAuxMode = (nID==0)? MDIN_OUT_MUX656_8 : MDIN_OUT_RGB444_8;
	OutAuxFrmt = VIDOUT_720x480i60;		// set video.c or set aux-format

	if (nID==DEMO_DISPLAY_CROP_PIP) fCROPMove = TRUE;
	popID = nID;	pipID = 0;
#endif	//	defined(SYSTEM_USE_MDIN380)	
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetEncoderPath(WORD nID)
{
#if	defined(SYSTEM_USE_MDIN325) || defined(SYSTEM_USE_MDIN325A) || defined(SYSTEM_USE_MDIN380)
	stVideo.exeFLAG |= MDIN_UPDATE_MAINFMT;
	stVideo.dspFLAG  = MDIN_AUX_DISPLAY_ON;
	stVideo.srcPATH  = (nID==2)? PATH_MAIN_A_AUX_M : PATH_MAIN_A_AUX_A;
	stVideo.dacPATH  = DAC_PATH_MAIN_OUT;
	stVideo.encPATH  = (nID==0)? VENC_PATH_PORT_A : VENC_PATH_PORT_X;

	memset(&stVideo.stVIEW_m, 0, 8);		// clear stVIEW_m
	memset(&stVideo.stVIEW_x, 0, 8);		// clear stVIEW_x
	memset(&stVideo.stCROP_x, 0, 8);		// clear stCROP_x
	OutMainFrmt = GetMenuStatus(6,2);		// get out-format
	OutAuxMode = MDIN_OUT_MUX656_8;
	OutAuxFrmt = VIDOUT_720x480i60;		// set video.c or set aux-format
#endif	/*	defined(SYSTEM_USE_MDIN325) || defined(SYSTEM_USE_MDIN325A)	*/
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetInputPattern(WORD nID)
{
	MDIN3xx_SetSrcTestPattern(&stVideo, (MDIN_IN_TEST_t)nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetOutputPattern(WORD nID)
{
	MDIN3xx_SetOutTestPattern((MDIN_OUT_TEST_t)nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetImageMirrorV(WORD nID)
{
	MDIN3xx_EnableMirrorV(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetInputFormat(WORD nID)
{
	SrcMainFrmt = nID;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetInputMode(WORD nID)
{
	SrcMainMode = nID;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetActiveRegion(WORD nID)
{
	MDIN3xx_SetSrcHACTPolarity(&stVideo, (nID&2)? 1 : 0);
	MDIN3xx_SetSrcVACTPolarity(&stVideo, (nID&1)? 1 : 0);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetCbCr422Swap(WORD nID)
{
	MDIN3xx_SetSrcCbCrSwap(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetClockDelay(WORD nID)
{
	MDIN3xx_SetDelayCLK_A((MDIN_CLK_DELAY_t)nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetInFLDBypass(WORD nID)
{
	MDIN3xx_SetSrcInputFieldID(&stVideo, (nID)? 0 : 1);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetTopFLDLevel(WORD nID)
{
	MDIN3xx_SetSrcHighTopFieldID(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetHACTOffset(WORD nID)
{
	MDIN3xx_SetSrcOffset(&stVideo, 2*nID, stVideo.stSRC_a.offV);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetVACTOffset(WORD nID)
{
	MDIN3xx_SetSrcOffset(&stVideo, stVideo.stSRC_a.offH, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetDeintMode(WORD nID)
{
	nID = nID + 1;		// not use non-still mode
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x210, 12, 2, nID);		// deint Y-mode
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetEdgeAngle(WORD nID)
{
	if		(nID==1) nID = DEMO_DEINT_EDGE_45;
	else if (nID==2) nID = DEMO_DEINT_EDGE_26;
	else if (nID==3) nID = DEMO_DEINT_EDGE_ALL;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x220, 0, 3, nID);		// edge angle
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMotionBorder(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x241, 15, 1, nID);		// border
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x208, 10, 1, nID);		// block match adp en

	MDINHIF_RegField(MDIN_LOCAL_ID, 0x241, 0, 7, (nID)? 16 : 32);	// except_hborder_th	 // only on demo
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetSlowMotion(WORD nID)
{
	nID = (nID)? 20 : 0;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x213, 0, 8, nID);		// slow motion thres
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetFastMotion(WORD nID)
{
	nID = (nID)? ON : OFF;
	if (nID)
	{
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x210, 12, 2, 1);	// adaptive
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x211, 0, 8, 18);	// next motion thres
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x242, 15, 1, 1);	// fast-motion on
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x250, 12, 3, 4);	// film all
	}
	else
	{
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x210, 12, 2, 0);	// non-still
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x211, 0, 8, 249);	// next motion thres
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x242, 15, 1, 0);	// fast-motion off
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x250, 12, 3, 0);	// film disable
	}
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetFilmMode(WORD nID)
{
	switch (nID) {
		case  0: nID = MDIN_DEINT_FILM_OFF; break;
		case  1: nID = MDIN_DEINT_FILM_32;  break;
		case  2: nID = MDIN_DEINT_FILM_22;  break;
		default: nID = MDIN_DEINT_FILM_ALL; break;
	}
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x250, 12, 3, nID);		// film mode
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetFilmBadEdit(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x250, 15, 1, nID);		// bad-edit
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetNoiseRobust1(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x218, 15, 1, nID);		// 
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x218, 7, 1, nID);		// 
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x266, 7, 1, nID);		// 

	MDINHIF_RegField(MDIN_LOCAL_ID, 0x210, 12, 2, nID);		// adaptive/non-still
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x262, 14, 1, nID);		// next motion NR use on/off

	nID = (nID)?20 : 15;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x211, 0, 8, nID);		// next motion thres
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetNoiseRobust2(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x218, 15, 1, nID);		// 
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x218, 7, 1, nID);		// 
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x266, 7, 1, nID);		// 

	MDINHIF_RegField(MDIN_LOCAL_ID, 0x210, 12, 2, nID);		// adaptive/non-still
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x262, 14, 1, nID);		// next motion NR use on/off
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x241, 15, 1, nID);		// border on/off

	nID = (nID)? 20: 15;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x211, 0, 8, nID);		// next motion thres
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetZoomArtifact(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableZoomArtifact(nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetInterAreaMode(WORD nID)
{
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, (nID)? ON : OFF);
	MDIN3xx_DisplayDeintInterWND((nID==2)? ON : OFF);
	AdjInterWND = (nID==2)? ON : OFF;
}

//--------------------------------------------------------------------------------------------------
void DEMO_Set3DNRMode(WORD nID)
{
	MDIN3xx_EnableDeint3DNR(&stVideo, (nID==0)? OFF : ON);
	MDIN3xx_SetDeint3DNRGain(&stVideo, (nID==1)? 34 : 44);	// NR factor level

//	nID = (nID)? ON : OFF;
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x260, 9, 1, nID);		// NR adaptor enable
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetCrossColor(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableDeintCCS(&stVideo, nID);					// cross color
}
//--------------------------------------------------------------------------------------------------
void DEMO_SetVD3DComb(WORD nID)
{
	nID = (nID)? ON : OFF;
//	VD5160_Enable3DComb(nID);			 //by hungry 2012.03.07
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetEdgeEnhance(WORD nID)
{
	MDIN3xx_EnablePeakingFilter((nID==3)? OFF : ON);

	if (nID==0) MDIN3xx_SetPeakingFilterLevel(16);	// Typ
	if (nID==1) MDIN3xx_SetPeakingFilterLevel(28);	// Mid
	if (nID==2) MDIN3xx_SetPeakingFilterLevel(60);	// Max
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetBlockNR(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableBlockNRFilter(nID);						// block NR
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMosquitoNR(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableMosquitFilter(nID);						// mosquito NR
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetColorTone(WORD nID)
{
	if (nID == 1) MDIN3xx_SetColorTonFilterCoef((PMDIN_COLORTON_COEF)&MDIN_ColorTonFilter_Default[0]);	// skin color
	if (nID == 2) MDIN3xx_SetColorTonFilterCoef((PMDIN_COLORTON_COEF)&MDIN_ColorTonFilter_Default[2]);	// green color
	MDIN3xx_EnableColorTonFilter((nID)? ON : OFF);						// color tone
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetChromaError(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableDeintCUE(&stVideo, nID);					// chroma upsampling error
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetContrast(WORD nID)
{
	MDIN3xx_SetPictureContrast(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetBrightness(WORD nID)
{
	MDIN3xx_SetPictureBrightness(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetSaturation(WORD nID)
{
	MDIN3xx_SetPictureSaturation(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetHue(WORD nID)
{
	MDIN3xx_SetPictureHue(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetSharpness(WORD nID)
{
	MDIN3xx_SetPeakingFilterLevel(nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_Set3DNRLevel(WORD nID)
{
	MDIN3xx_SetDeint3DNRGain(&stVideo, nID);	// NR factor level

	nID = (nID)? ON : OFF;
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x260, 9, 1, nID);		// NR adaptor enable
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetBWExtension(WORD nID)
{
	if (nID) MDIN3xx_SetBWExtensionPoint((PMDIN_BWPOINT_COEF)&defMDINBWPoint[nID-1]);
	MDIN3xx_EnableBWExtension((nID==0)? OFF : ON);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetLTIControl(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableLTI(nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetCTIControl(WORD nID)
{
	nID = (nID)? ON : OFF;
	MDIN3xx_EnableCTI(nID);
}

//--------------------------------------------------------------------------------------------------------------------------
static PMDIN_MFCFILT_COEF GetMFCFilterCoef(BYTE mode)
{
	switch (mode) {
		case  0: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_Lanczos_4;
		case  1: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_Spline_1_00;
		case  2: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_Spline_0_75;
		case  3: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_Spline_0_50;
		case  4: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_BSpline_Mitchell;
		case  5: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_BSpline_More_Blur;
		case  6: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_BSpline_Most_Blur;
		case  7: return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_Bilinear;
	}
	return (PMDIN_MFCFILT_COEF)&EDK_MFCFilter_Spline_1_00;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMFCHYFilter(WORD nID)
{
	PMDIN_MFCFILT_COEF pCoef = GetMFCFilterCoef(nID);
	MDIN3xx_SetMFCHYFilterCoef(&stVideo, pCoef);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMFCHCFilter(WORD nID)
{
	PMDIN_MFCFILT_COEF pCoef = GetMFCFilterCoef(nID);
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, pCoef);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMFCVYFilter(WORD nID)
{
	PMDIN_MFCFILT_COEF pCoef = GetMFCFilterCoef(nID);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, pCoef);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMFCVCFilter(WORD nID)
{
	PMDIN_MFCFILT_COEF pCoef = GetMFCFilterCoef(nID);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, pCoef);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetFNRYCFilter(WORD nID)
{
	MDIN3xx_SetFrontNRFilterCoef((PMDIN_FNRFILT_COEF)&EDK_FNRY1HDFilter[nID]);
	MDIN3xx_EnableFrontNRFilter((nID)? ON : OFF);
}

//--------------------------------------------------------------------------------------------------
BOOL DEMO_GetGraphicDisplay(void)
{
	return (fDemo1OSD||fDemo2OSD)? ON : OFF;
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetDemo1OSDDisplay(WORD nID)
{
	fDemo1OSD = ~fDemo1OSD&1;	SetOSDMenuID(0x0000);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX3], fDemo1OSD);

	nID = (fDemo1OSD||fDemo2OSD)? OFF : ON;
	MDIN3xx_EnableDeint3DNR(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetDemo2OSDDisplay(WORD nID)
{
	fDemo2OSD = ~fDemo2OSD&1;	SetOSDMenuID(0x0000);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX4], fDemo2OSD);

	nID = (fDemo1OSD||fDemo2OSD)? OFF : ON;
	MDIN3xx_EnableDeint3DNR(&stVideo, nID);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetCURDisplay(WORD nID)
{
	fCUR32 = ~fCUR32&1;
	MDINCUR_EnableCursor(&stCurCTL, fCUR32);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetMDINSoftReset(void)
{
	MDIN3xx_SoftReset();
}

#if	defined(SYSTEM_USE_MDIN380)
//--------------------------------------------------------------------------------------------------
//static void DEMO_SetPIPViewWIND(WORD nID)
void DEMO_SetPIPViewWIND(WORD nID)
{
	MDIN_VIDEO_WINDOW stVIEW;

	if (pipID||popID>DEMO_DISPLAY_ONLY_PIP4) return;

	if(Video_Out_Res_Val >= VIDOUT_1920x1080i60 && Video_Out_Res_Val <= VIDOUT_1920x1080p50)
	{
		{
			stVIEW.w = 1920;
			stVIEW.h = 1080;
			stVIEW.x = 0;
			stVIEW.y = 0;
		}
	}
	else if(Video_Out_Res_Val >= VIDOUT_1680x1050p60)
	{
		if(popID == DEMO_DISPLAY_ONLY_PIP1)
			memcpy(&stVIEW, &defPIPViewWND_1680X1050[0][0], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP2)
			memcpy(&stVIEW, &defPIPViewWND_1680X1050[0][1], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP3)
			memcpy(&stVIEW, &defPIPViewWND_1680X1050[0][2], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP4)
			memcpy(&stVIEW, &defPIPViewWND_1680X1050[0][3], 8);
	}
	else if(Video_Out_Res_Val >= VIDOUT_1440x900p60)
	{
		if(popID == DEMO_DISPLAY_ONLY_PIP1)
			memcpy(&stVIEW, &defPIPViewWND_1440X900[0][0], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP2)
			memcpy(&stVIEW, &defPIPViewWND_1440X900[0][1], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP3)
			memcpy(&stVIEW, &defPIPViewWND_1440X900[0][2], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP4)
			memcpy(&stVIEW, &defPIPViewWND_1440X900[0][3], 8);
	}
	else if(Video_Out_Res_Val >= VIDOUT_1360x768p60)
	{
		if(popID == DEMO_DISPLAY_ONLY_PIP1)
			memcpy(&stVIEW, &defPIPViewWND_1360X768[0][0], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP2)
			memcpy(&stVIEW, &defPIPViewWND_1360X768[0][1], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP3)
			memcpy(&stVIEW, &defPIPViewWND_1360X768[0][2], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP4)
			memcpy(&stVIEW, &defPIPViewWND_1360X768[0][3], 8);
	}
	else if(Video_Out_Res_Val >= VIDOUT_1280x1024p60)
	{
		if(popID == DEMO_DISPLAY_ONLY_PIP1)
			memcpy(&stVIEW, &defPIPViewWND_1280X1024[0][0], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP2)
			memcpy(&stVIEW, &defPIPViewWND_1280X1024[0][1], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP3)
			memcpy(&stVIEW, &defPIPViewWND_1280X1024[0][2], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP4)
			memcpy(&stVIEW, &defPIPViewWND_1280X1024[0][3], 8);
	}
	else if(Video_Out_Res_Val >= VIDOUT_1024x768p60)
	{
		if(popID == DEMO_DISPLAY_ONLY_PIP1)
			memcpy(&stVIEW, &defPIPViewWND_1024X768[0][0], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP2)
			memcpy(&stVIEW, &defPIPViewWND_1024X768[0][1], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP3)
			memcpy(&stVIEW, &defPIPViewWND_1024X768[0][2], 8);
		else if(popID == DEMO_DISPLAY_ONLY_PIP4)
			memcpy(&stVIEW, &defPIPViewWND_1024X768[0][3], 8);
	}
	
	//MDINAUX_SetVideoWindowVIEW(&stVideo, stVIEW);
	memcpy(&stVideo.stVIEW_x, &stVIEW, sizeof(MDIN_VIDEO_WINDOW));
	MDIN3xx_EnableWriteFRMB(&stVideo, 0);
	MDINAUX_SetScaleProcess(&stVideo);
	MDIN3xx_EnableWriteFRMB(&stVideo, 1);
	MDIN3xx_EnableMainDisplay(ON);
}

//--------------------------------------------------------------------------------------------------
void DEMO_SetWindowPIPPOP(WORD nID)
{
	if (stVideo.dacPATH!=DAC_PATH_MAIN_PIP) return;

	DEMO_SetPIPViewWIND(nID);
}

#endif	// defined(SYSTEM_USE_MDIN380)	

#endif
/*  FILE_END_HERE */
