//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  OSDMENU.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#if 0


#ifndef		__OSDMENU_H__
#define		__OSDMENU_H__

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
static ROMDATA char defMainLeftChar[][16]	= {
#if defined(SYSTEM_USE_MDIN380)
#if CPU_ACCESS_BUS_NBYTE == 1
	" -------------- ",	"  MDIN380-DK16  ",	" -------------- ",
#else
//	" -------------- ",	"  MDIN380-DK08  ",	" -------------- ",
	" -------------- ",	" SDI to MDIN380 ",	" -------------- ",		//by hungry 2012.03.15

#endif
#endif
#if defined(SYSTEM_USE_MDIN340)
	" -------------- ",	"  MDIN340-DK16  ",	" -------------- ",
#endif
#if defined(SYSTEM_USE_MDIN325)
	" -------------- ",	"  MDIN325-DK16  ",	" -------------- ",
#endif
#if defined(SYSTEM_USE_MDIN325A)
	" -------------- ",	" MDIN325A-DK16  ",	" -------------- ",
#endif
	" 1.Deinterlace  ",	" 2.Noise Reduce ",	" 3.Enhancement  ",
	" 4.In/Out mode  ", " 5.Picture      ", 	" 6.Setup        ",
//	" 7.Filters      ", " 8.EXT-Video    ",		 
	" 7.Filters      ",	" 8.Audio        ",	 //by hungry 2012.03.14
//	" -------------- ",	"   MacroImage   ", "   Technology   ", 	"  v2.50/120330  ",	" -------------- "
	" -------------- ",	"   MacroImage   ", "   Technology   ", 	"  v0.14/120514  ",	" -------------- "
};

static ROMDATA BYTE defMainLeftAttb[]		= {
	0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71
};

#define	LeftMenuSize	8

static ROMDATA char defMainRightChar[][13][16]	= {
	{	// 1.Deinterlace
		" Deint Mode     ",	" Edge Angle     ",	" Motion Border  ",
		" Fast Motion    ",	" Slow Motion    ",	" Zoom Artifact  ", 	
		" Noise Robust   ", 	" Film Mode      ",	" Film Bad Edit  ",
		" InterArea Set  ",	"                ",	"                ",	"                "
	},
	{	// 2.Noise Reduction
		" 3DNR Set       ",	" 3DNR Level     ",	" Mosquito NR    ",
		" V/D 3D Comb    ", " Cross Color    ",	"                ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",	"                "
	},
	{	// 3.Enhancement/Scaling
		" B/W Extension  ",	" Color Tone     ", " Edge Enhance   ",
		" Sharpness      ", "                ", "                ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",	"                "
	},
	{	// 4.In/Out Mode
#if defined(SYSTEM_USE_MDIN340) | defined(SYSTEM_USE_MDIN325)
		"                ",	"                ",	"                ", "                ", 
#endif
#if defined(SYSTEM_USE_MDIN325A)
		" 4xD1 Mux       ", "                ", "                ", "                ", 
#endif
#if defined(SYSTEM_USE_MDIN380)
		"                ", "                ", "                ", "                ",	  //by hungry 2012.03.06
#endif
		" Over Scanning  ", " Aspect Ratio   ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",	"                "
	},
	{	// 5.Picture
		" Contrast       ", " Brightness     ", " Saturation     ",
		" Hue            ", " LTI            ", " CTI            ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",	"                "
	},
	{	// 6. Setup
		" Input Source   ",	 " Out Resolut.   ", " Out Port Mode  ",
#if defined(SYSTEM_USE_MDIN340)
		"                ",	
#endif
#if defined(SYSTEM_USE_MDIN325)||defined(SYSTEM_USE_MDIN325A)||defined(SYSTEM_USE_MDIN380)
		" Encoder Path   ",	
#endif
		" In Pattern     ", " Out Pattern    ",
		" V-Mirror       ",	" In-Sync Info   ",	"                ",
		"                ",	"                ",	"                ",	"                "
	},
	{	// 7. Filters
		" MFC HY Filter  ", " MFC HC Filter  ", " MFC VY Filter  ", 
		" MFC VC Filter  ",  " FNR YC Filter  ",	"                ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",	"                "
	},
	{	// 8. Audio					  //by hungry 2012.03.14
		" Input Source   ",	"                ",	"                ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",
		"                ",	"                ",	"                ",	"                "
 	}
//	{	// 8. Ext Input					  //by hungry 2012.03.06
//		" Input Format   ",	" In Port Mode   ", " Active Region  ",
//		" CbCr422 Swap   ", " Clock Delay    ", " In-FLD Bypass  ",
//		" Top-FLD Level  ", " HACT-Offset    ",	" VACT-Offset    ",
//		"                ",	"                ",	"                ",	"                ",
//	},
};

static ROMDATA BYTE defMainRightAttb[][13]		= {
//    1        2       3        4       5       6       7       8       9       10      11      12      13
	{0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00},	// 1.Deinterlace
	{0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// 2.Noise Reduction
	{0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// 3.Enhancement/Scaling
	{0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// 4.In/Out Mode
	{0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// 5.Picture
	{0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00},	// 6.Setup
	{0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// 7.Filters
//	{0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x00, 0x00, 0x00},	// 8.Ext Input			//by hungry 2012.03.06
	{0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// 8.Audio		//by hungry 2012.03.14
 };

//--------------------------------------------------------------------------------------------------------------------------
static ROMDATA char OnOffModeComb[][5]		= {" Off ", "  On "};

//--------------------------------------------------------------------------------------------------------------------------
// 1.Deinterlace comb
//--------------------------------------------------------------------------------------------------------------------------
static ROMDATA char DeintModeComb[][7]		= {
	" Adapt ", " Intra ", " Inter "
};

static ROMDATA char EdgeAngleComb[][6]		= {
	"  Off ", "  45  ", "  26  ", "  All "
};

//static ROMDATA char EdgeMotionBComb[][9]		= {
//	"   Off   ", "  Mode1  ", "  Mode2  "
//};

//static ROMDATA char DeintFastComb[][11]		= {
//	"    Off    ", " Norm Fast ", " Very Fast "
//};

static ROMDATA char DeintFilmComb[][5]		= {
	" Off ", " 3:2 ", " 2:2 ", " All "
};

static ROMDATA char InterAreaComb[][5]		= {
	" Off ", "  On ", " Adj "
};

// [DeintMode] [EdgeAngle] [MotionBorder]
// [FastMotion] [SlowMotion] [ZoomArtifact]
// [NoiseRobust] [FilmMode] [FilmBadEdit] [InterArea]

static ROMDATA BYTE defIPCMode[]		= {0, 3, 1, 1, 1, 1, 1, 3, 1, 0};	// initial value

static ROMDATA BYTE defIPCModeMax[]		= {
	sizeof(DeintModeComb)/sizeof(DeintModeComb[0]),	// [DeintMode]
	sizeof(EdgeAngleComb)/sizeof(EdgeAngleComb[0]),	// [EdgeAngle]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [MotionBorder]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [FastMotion]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [SlowMotion]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [ZoomArtifact]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [NoiseRobust]
	sizeof(DeintFilmComb)/sizeof(DeintFilmComb[0]),		// [FilmMode]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [FilmBadEdit]
	sizeof(InterAreaComb)/sizeof(InterAreaComb[0])	// [InterArea]
};

static ROMDATA char *pIPCModeItem[] = {
	DeintModeComb[0],	// [DeintMode]
	EdgeAngleComb[0],	// [EdgeAngle]
	OnOffModeComb[0],	// [MotionBorder]
	OnOffModeComb[0],	// [FastMotion]
	OnOffModeComb[0],	// [SlowMotion]
	OnOffModeComb[0],	// [ZoomArtifact]
	OnOffModeComb[0],	// [NoiseRobust]
	DeintFilmComb[0],	// [FilmMode]
	OnOffModeComb[0],	// [FilmBadEdit]
	InterAreaComb[0]	// [InterArea]
};

static ROMDATA BYTE defIPCModeSize[] = {
	sizeof(DeintModeComb[0]),		// [DeintMode]
	sizeof(EdgeAngleComb[0]),		// [EdgeAngle]
	sizeof(OnOffModeComb[0]),	// [MotionBorder]
	sizeof(OnOffModeComb[0]),	// [FastMotion]
	sizeof(OnOffModeComb[0]),	// [SlowMotion]
	sizeof(OnOffModeComb[0]),	// [ZoomArtifact]
	sizeof(OnOffModeComb[0]),	// [NoiseRobust]
	sizeof(DeintFilmComb[0]),		// [FilmMode]
	sizeof(OnOffModeComb[0]),	// [FilmBadEdit]
	sizeof(InterAreaComb[0])		// [InterArea]
};

//--------------------------------------------------------------------------------------------------------------------------
// 2.Noise Reduction comb
//--------------------------------------------------------------------------------------------------------------------------
static ROMDATA char Level3DNRComb[][10]	= {
	"   Off    ", "   Typ    ", "  Strong  "
};

// [3DNR Set] [3DNR Level] [Mosquito NR]
// [V/D 3D Comb] [Cross Color]

static ROMDATA BYTE defNRMode[] = {1, 34, 1, 1, 1};	// initial value

static ROMDATA BYTE defNRModeMax[] = {
	sizeof(Level3DNRComb)/sizeof(Level3DNRComb[0]),	// [3DNR Set]
	60,		// [3DNR Level]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [Mosquito NR]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [V/D 3D Comb]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [Cross Color]
};

static ROMDATA char *pNRModeItem[]		= {
	Level3DNRComb[0],	// [3DNR Set]
	NULL,				// [3DNR Level]
	OnOffModeComb[0],	// [Mosquito NR]
	OnOffModeComb[0],	// [V/D 3D Comb]
	OnOffModeComb[0]	// [Cross Color]
};

static ROMDATA BYTE defNRModeSize[]	= {
	sizeof(Level3DNRComb[0]),		// [3DNR Set]
	0xFF,						// [3DNR Level]
	sizeof(OnOffModeComb[0]),	// [Mosquito NR]
	sizeof(OnOffModeComb[0]),	// [V/D 3D Comb]
	sizeof(OnOffModeComb[0]),	// [Cross Color]
};

//--------------------------------------------------------------------------------------------------------------------------
// 3.Enhancement/Scaling
//--------------------------------------------------------------------------------------------------------------------------

static ROMDATA char ColorToneComb[][9]	= {
	"   Off   ", "  Skin   ", "  Green  "
};

static ROMDATA char EdgeEnhanComb[][7]	= {
	"  Typ  ", "  Mid  ", "  Max  ", "  Off  "
};

//	[B/W Extension] [Color Tone] [Edge Enhance]
//	[Sharpness]

static ROMDATA BYTE defEnhance[]		= {0, 0, 0, 0};	// initial value

static ROMDATA BYTE defEnhanceMax[]		= {
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [B/W Extension]
	sizeof(ColorToneComb)/sizeof(ColorToneComb[0]),	// [Color Tone]
	sizeof(EdgeEnhanComb)/sizeof(EdgeEnhanComb[0]),	// [Edge Enhance]
	127												// [Sharpness]
};

static ROMDATA char *pEnhanceItem[]		= {
	OnOffModeComb[0],	// [B/W Extension]
	ColorToneComb[0],	// [Color Tone]
	EdgeEnhanComb[0],	// [Edge Enhance]
	NULL				// [Sharpness]
};

static ROMDATA BYTE defEnhanceSize[]	= {
	sizeof(OnOffModeComb[0]),	// [B/W Extension]
	sizeof(ColorToneComb[0]),		// [Color Tone]
	sizeof(EdgeEnhanComb[0]),	// [Edge Enhance]
	0xFF						// [Sharpness]
};

//--------------------------------------------------------------------------------------------------------------------------
// 4.In/Out Mode
//--------------------------------------------------------------------------------------------------------------------------

static ROMDATA char Set4ChComb[][6]		= {
	" Off  ", " ALL  ", " CH01 ", " CH02 ", " CH03 ",
	" CH04 ",	" CH12 ",	" CH34 ",	" CH13 ",	" CH24 ",	
	" CH14 ",	" CH23 "
};

static ROMDATA char SetPIPPOPComb[][7]		= {
	"  Off  ", " PIP-A ", " PIP-B ", " POP-1 ", " POP-2 "
};

static ROMDATA char OuputDualComb[][13]		= {
	"     Off     ", " SVGA60(InA) ", "  XGA60(InA) ", " SXGA60(InA) ",
	" SVGA60(InB) ", "  XGA60(InB) ", " SXGA60(InB) "
};

static ROMDATA char ZoomRatioComb[][5]		= {
	" Off ", "  5% ", " 10% ", " 15% ", " 20% ", " 50% "
};

static ROMDATA char ViewRatioComb[][6]		= {
	"  4:3 ", " 16:9 "
};

//	[4xD1 Mux] [2HD Input] [PIP/POP Mode]
//	[Dual Display] [Over Scanning] [Aspect Ratio]

//static ROMDATA BYTE defInOutMode[]		= {0, 0, 0, 1};	// initial value

static ROMDATA BYTE defInOutMode[]		= {0, 0, 0, 0, 0, 0};	// initial value	 //by hungry 2012.03.15

static ROMDATA BYTE defInOutModeMax[]		= {
	sizeof(Set4ChComb)/sizeof(Set4ChComb[0]),	// [4xD1 Mux]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [2HD Input]
	sizeof(SetPIPPOPComb)/sizeof(SetPIPPOPComb[0]),	// [PIP/POP Mode]
	sizeof(OuputDualComb)/sizeof(OuputDualComb[0]),	// [Dual Display]
	sizeof(ZoomRatioComb)/sizeof(ZoomRatioComb[0]),	// [Over Scanning]
	sizeof(ViewRatioComb)/sizeof(ViewRatioComb[0]), 	// [Aspect Ratio]
};

static ROMDATA char *pInOutModeItem[]		= {
	Set4ChComb[0],	// [4xD1 Mux]
	OnOffModeComb[0],	// [2HD Input]
	SetPIPPOPComb[0],	// [PIP/POP Mode]
	OuputDualComb[0],	// [Dual Display]
	ZoomRatioComb[0],	// [Over Scanning]
	ViewRatioComb[0], 	// [Aspect Ratio]
};

static ROMDATA BYTE defInOutModeSize[]	= {
	sizeof(Set4ChComb[0]),	// [4xD1 Mux]
	sizeof(OnOffModeComb[0]),	// [2HD Input]
	sizeof(SetPIPPOPComb[0]),		// [PIP/POP Mode]
	sizeof(OuputDualComb[0]),		// [Dual Display]
	sizeof(ZoomRatioComb[0]),	// [Over Scanning]
	sizeof(ViewRatioComb[0]),	 	// [Aspect Ratio]
};

//--------------------------------------------------------------------------------------------------------------------------
// 5.Picture
//--------------------------------------------------------------------------------------------------------------------------

//	[Contrast] [Brightness] [Saturation]
//	[Hue] [LTI] [CTI]

static ROMDATA BYTE defPicture[] = {128, 128, 128, 128, 0, 0};	// initial value

static ROMDATA BYTE defPictureMax[] = {
	255, 	//	[Brightness] 
	255, 	//	[Contrast] 
	255, 	//	[Saturation]
	255,		//	[Hue] 
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	//	[LTI] 
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0])	//	[CTI]
};

static ROMDATA char *pPictureItem[]		= {
	NULL, 	//	[Brightness] 
	NULL, 	//	[Contrast] 
	NULL, 	//	[Saturation]
	NULL,  	//	[Hue] 
	OnOffModeComb[0], 	//	[LTI] 
	OnOffModeComb[0]	//	[CTI]
};

static ROMDATA BYTE defPictureSize[]	= {
	0xff, 	//	[Brightness] 
	0xff, 	//	[Contrast] 
	0xff, 	//	[Saturation] 
	0xff,  	//	[Hue]  
	sizeof(OnOffModeComb[0]), 	//	[LTI] 
	sizeof(OnOffModeComb[0])		//	[CTI]
};

//--------------------------------------------------------------------------------------------------------------------------
// 6.Setup
//--------------------------------------------------------------------------------------------------------------------------

static ROMDATA char InputPortComb[][11]		= {
#if defined(SYSTEM_USE_MDIN340)
//	" CVBS-5160 ", " CVBS-9910 ", "    HDMI   ", " EXT-Video "		 
	"  SDI-IN   "		 											//by hungry 2012.03.20
#endif
#if defined(SYSTEM_USE_MDIN325)||defined(SYSTEM_USE_MDIN325A)
//	" CVBS-5160 ", " CVBS-2866 ", " Component ", "   PC-VGA  ",
//	"    HDMI   ", " EXT-Video "									  
	"  SDI-IN   "													  
#endif																  //by hungry 2012.03.20
#if defined(SYSTEM_USE_MDIN380)
//	" CVBS-5160 ", " CVBS-2866 ", " Component ", "   PC-VGA  ",		  //by hungry 2012.03.06
//	"    HDMI   ", " EXT-Video "									  //by hungry 2012.02.15
	"  SDI-IN   "													  //by hungry 2012.03.06
#endif
};

/*static ROMDATA char OuputFrmtComb[][14]		= {
	"  SD(480i)60  ",	"  SD(576i)50  ",	"  SD(480p)60  ",	"  SD(576p)50  ",
//	"  HD(720p)60  ",	"  HD(720p)59  ",	"  HD(720p)50  ",	
//	"  HD(720p)30  ",	"  HD(720p)25  ",	"  HD(720p)24  ",
//	"  HD(720p)30  ",	"  HD(720p)25  ",						 //by hungry 2012.03.19
	"  HD(1080i)60 ",	"  HD(1080i)59 ",	"  HD(1080i)50 ",
	"  HD(1080p)60 ",	"  HD(1080p)59 ",	"  HD(1080p)50 ",
//	"  HD(1080p)30 ",	"  HD(1080p)25 ",	"  HD(1080p)24 ",
//	"   MON(VGA)60 ",	"   MON(VGA)75 ",	"  MON(SVGA)60 ",	"  MON(SVGA)75 ",
	"   MON(XGA)60 ",	//"   MON(XGA)75 ",
	"  MON(SXGA)60 ",	//"  MON(SXGA)75 ",
	"  1360x768p60 ",	//" 1600x1200p60 ",	
	"  1440x900p60 ",	//"  1440x900p75 ",
	" 1680x1050p60 ",	//" 1680x1050pRB ",	" 1920x1080pRB ",	" 1920x1200pRB "
};
*/
static ROMDATA char OuputFrmtComb[][14]		= {
	"  SD(480i)60  ",	"  SD(576i)50  ",	"  SD(480p)60  ",	"  SD(576p)50  ",
	"  HD(720p)60  ",	"  HD(720p)59  ",	"  HD(720p)50  ",	
//	"  HD(720p)30  ",	"  HD(720p)25  ",	"  HD(720p)24  ",
	"  HD(720p)30  ",	"  HD(720p)25  ",						 //by hungry 2012.03.19
	"  HD(1080i)60 ",	"  HD(1080i)59 ",	"  HD(1080i)50 ",
	"  HD(1080p)60 ",	"  HD(1080p)59 ",	"  HD(1080p)50 ",
	"  HD(1080p)30 ",	"  HD(1080p)25 ",	"  HD(1080p)24 ",
	"   MON(VGA)60 ",	"   MON(VGA)75 ",	"  MON(SVGA)60 ",	"  MON(SVGA)75 ",
	"   MON(XGA)60 ",	"   MON(XGA)75 ",	"  MON(SXGA)60 ",	"  MON(SXGA)75 ",
	"  1360x768p60 ",	" 1600x1200p60 ",	"  1440x900p60 ",	"  1440x900p75 ",
	" 1680x1050p60 ",	" 1680x1050pRB ",	" 1920x1080pRB ",	" 1920x1200pRB "
};


static ROMDATA char OuputModeComb[][14]		= {
	"  RGB(24Bit)  ", " YC444(24Bit) ", " YC422(16EMB) ", " YC422(16SEP) ",
	" YC422(20EMB) ", " YC422(20SEP) ", " BT656( 8Bit) ", " BT656(10Bit) "
};

static ROMDATA char VENCInputComb[][6]		= {
	" IN-A ", "  AUX ", " MAIN "
};

static ROMDATA char InPatternComb[][9]		= {
	" Disable ", "  Window ", "  H-Ramp ", "  V-Ramp ", "  H-Gray ", "  V-Gray ",
	" H-Color ", " V-Color "
};

static ROMDATA char OutPattenComb[][10]		= {
	"  Disable ", "   White  ", "   Cross  ", "   Hatch  ", "   Color  ", "   Gray   ",
	"  Window  ", "  H-Ramp  ", " W-HRamp  ", " W-VRamp  ", " Diagonal "
};

static ROMDATA char InSynInfoComb[][14]		= {
	"  SD(480i)60  ", "  SD(576i)50  ",	"  SD(480p)60  ", "  SD(576p)50  ",
	"  HD(720p)60  ", "  HD(720p)50  ", "  HD(1080i)60 ", "  HD(1080i)50 ",
	"  HD(1080p)60 ", "  HD(1080p)50 ",

	"   PC(VGA)60  ", "  PC(SVGA)60  ", "   PC(XGA)60  ", "  PC(SXGA)60  ",
	"  1280x720p60 ", "  1280x960p60 ", "  1360x768p60 ", "  1440x900p60 ",

#if VGA_SOURCE_EXTENSION == 1
	"   PC(VGA)72  ", "   PC(VGA)75  ", "  PC(SVGA)56  ", "  PC(SVGA)72  ",
	"  PC(SVGA)75  ", "   PC(XGA)70  ", "   PC(XGA)75  ",
	"  1152x864p75 ", "  1280x800p60 ",
#endif
	"    NO-SYNC   "
};

//	[Input Source] [Out Resolut] [Out Port Mode]
//	[Encoder Path] [In Pattern] [Out Pattern]
//	[V-Mirror] [In-Sync Info]

//static ROMDATA BYTE defSetUp[]		= {0, 14, 0, 0, 0, 0, 0, 0};	// initial value
//static ROMDATA BYTE defSetUp[]		= {0, 14, 0, 1, 0, 0, 0, 0};	// initial value	//API ver0.26 2012.02.13
static ROMDATA BYTE defSetUp[]		= {0, 12, 4, 1, 0, 0, 0, 0};	// initial value	//by hungry 2012.03.20

static ROMDATA BYTE defSetUpMax[]	= {
	sizeof(InputPortComb)/sizeof(InputPortComb[0]),	// [Input Source]
	sizeof(OuputFrmtComb)/sizeof(OuputFrmtComb[0]),	// [Out Resolut]
	sizeof(OuputModeComb)/sizeof(OuputModeComb[0]),	// [Out Port Mode]
	sizeof(VENCInputComb)/sizeof(VENCInputComb[0]), 	// [Encoder Path]
	sizeof(InPatternComb)/sizeof(InPatternComb[0]),	// [In Pattern]
	sizeof(OutPattenComb)/sizeof(OutPattenComb[0]),	// [Out Pattern]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [V-Mirror]
	sizeof(InSynInfoComb)/sizeof(InSynInfoComb[0])	// [In-Sync Info]
};

static ROMDATA char *pSetUpItem[]		= {
	InputPortComb[0],	// [Input Source]
	OuputFrmtComb[0],	// [Out Resolut]
	OuputModeComb[0],	// [Out Port Mode]
	VENCInputComb[0], 	// [Encoder Path]
	InPatternComb[0],	// [In Pattern]
	OutPattenComb[0],	// [Out Pattern]
	OnOffModeComb[0],	// [V-Mirror]
	InSynInfoComb[0]	// [In-Sync Info]
};

static ROMDATA BYTE defSetUpSize[]		= {
	sizeof(InputPortComb[0]),		// [Input Source]
	sizeof(OuputFrmtComb[0]),		// [Out Resolut]
	sizeof(OuputModeComb[0]),	// [Out Port Mode]
	sizeof(VENCInputComb[0]), 	// [Encoder Path]
	sizeof(InPatternComb[0]),		// [In Pattern]
	sizeof(OutPattenComb[0]),		// [Out Pattern]
	sizeof(OnOffModeComb[0]),	// [V-Mirror]
	sizeof(InSynInfoComb[0])		// [In-Sync Info]
};

//--------------------------------------------------------------------------------------------------------------------------
// 7.Filters
//--------------------------------------------------------------------------------------------------------------------------

static ROMDATA char MFCFilterComb[][13]		= {
	"  Lanczos_4  ", " Spline_1_00 ", " Spline_0_75 ", " Spline_0_50 ",
	"  Mitchell   ", "  More_Blur  ", "  Most_Blur  ", "  Bi-Linear  "
};

static ROMDATA char FNRFiltYCComb[][10]		= {
	"  Bypass  ", " Cut0.563 ", " Cut0.375 ", " Cut0.333 ", " Cut0.250 "
};

//	[MFC HY Filter] [MFC HC Filter] [MFC VY Filter]
//	[MFC VC Filter] [FNR YC Filter]

static ROMDATA BYTE defFilter[]			= {3, 3, 3, 3, 0};		// initial value

static ROMDATA BYTE defFilterMax[]		= {
	sizeof(MFCFilterComb)/sizeof(MFCFilterComb[0]),		// [MFC HY Filter]
	sizeof(MFCFilterComb)/sizeof(MFCFilterComb[0]),		// [MFC HC Filter]
	sizeof(MFCFilterComb)/sizeof(MFCFilterComb[0]),		// [MFC VY Filter]
	sizeof(MFCFilterComb)/sizeof(MFCFilterComb[0]),		// [MFC VC Filter]
	sizeof(FNRFiltYCComb)/sizeof(FNRFiltYCComb[0])		// [FNR YC Filter]
};

static ROMDATA char *pFilterItem[]		= {
	MFCFilterComb[0],		// [MFC HY Filter]
	MFCFilterComb[0],		// [MFC HC Filter]
	MFCFilterComb[0],		// [MFC VY Filter]
	MFCFilterComb[0],		// [MFC VC Filter]
	FNRFiltYCComb[0]		// [FNR YC Filter]
};

static ROMDATA BYTE defFilterSize[]		= {
	sizeof(MFCFilterComb[0]),		// [MFC HY Filter]
	sizeof(MFCFilterComb[0]),		// [MFC HC Filter]
	sizeof(MFCFilterComb[0]),		// [MFC VY Filter]
	sizeof(MFCFilterComb[0]),		// [MFC VC Filter]
	sizeof(FNRFiltYCComb[0])		// [FNR YC Filter]
};
/*	//by hungry 2012.03.14
//--------------------------------------------------------------------------------------------------------------------------
// 8.Ext Input
//--------------------------------------------------------------------------------------------------------------------------

static ROMDATA char InputFrmtComb[][14]		= {
	"  SD(480i)60  ", "  SD(576i)50  ",	"  SD(480p)60  ", "  SD(576p)50  ",
	"  HD(720p)60  ", "  HD(720p)50  ",	"  HD(1080i)60 ", "  HD(1080i)50 ",
	"  HD(1080p)60 ", "  HD(1080p)50 ",

	"   PC(VGA)60  ", "  PC(SVGA)60  ", "   PC(XGA)60  ", "  PC(SXGA)60  ",
	"  1280x720p60 ", "  1280x960p60 ", "  1360x768p60 ", "  1440x900p60 ",

#if VGA_SOURCE_EXTENSION == 1
	"   PC(VGA)72  ", "   PC(VGA)75  ", "  PC(SVGA)56  ", "  PC(SVGA)72  ",
	"  PC(SVGA)75  ", "   PC(XGA)70  ", "   PC(XGA)75  ",
	"  1152x864p75 ", "  1280x800p60 "
#endif
};

static ROMDATA char InputModeComb[][12]		= {
	" RGB(24BIT) ", " YUV(24Bit) ", " 422(16EMB) ", " 422(16SEP) ",
	" 422(20EMB) ", " 422(20SEP) ",	" 656(08EMB) ", " 656(08SEP) ",
	" 656(10EMB) ", " 656(10SEP) ",
};

static ROMDATA char ActRegionComb[][13]		= {
	" HA-P / VA-P ", " HA-P / VA-N ", " HA-N / VA-P ", " HA-N / VA-N "
};

static ROMDATA char SrcClkDlyComb[][10]		= {
	" Off(0ns) ", " DLY(1ns) ", " DLY(2ns) ", " DLY(3ns) ",
	" INV(0ns) ", " INV(1ns) ", " INV(2ns) ", " INV(3ns) "
};

static ROMDATA char TFldLevelComb[][6]		= {
	"  Low ", " High "
};

//	[Input Format] [In Port Mode] [Active Region]
//	[CbCr422 Swap] [Clock Delay] [In-FLD Bypass]
//	[Top-FLD Level] [HACT-Offset] [VACT-Offset]

static ROMDATA BYTE defEVideo[]			= {0, 6, 0, 0, 0, 0, 0, 0, 0};	// initial value

static ROMDATA BYTE defEVideoMax[]		= {
	sizeof(InputFrmtComb)/sizeof(InputFrmtComb[0]),	// [Input Format]
	sizeof(InputModeComb)/sizeof(InputModeComb[0]),	// [In Port Mode]
	sizeof(ActRegionComb)/sizeof(ActRegionComb[0]),	// [Active Region]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [CbCr422 Swap]
	sizeof(SrcClkDlyComb)/sizeof(SrcClkDlyComb[0]),		// [Clock Delay]
	sizeof(OnOffModeComb)/sizeof(OnOffModeComb[0]),	// [In-FLD Bypass]
	sizeof(TFldLevelComb)/sizeof(TFldLevelComb[0]),		// [Top-FLD Level]
	255,		// [HACT-Offset]
	255		// [VACT-Offset]
};

static ROMDATA char *pEVideoItem[]		= {
	InputFrmtComb[0],	// [Input Format]
	InputModeComb[0],	// [In Port Mode]
	ActRegionComb[0],	// [Active Region]
	OnOffModeComb[0],	// [CbCr422 Swap]
	SrcClkDlyComb[0],	// [Clock Delay]
	OnOffModeComb[0],	// [In-FLD Bypass]
	TFldLevelComb[0],	// [Top-FLD Level]
	NULL,		// [HACT-Offset]
	NULL		// [VACT-Offset]
};

static ROMDATA BYTE defEVideoSize[]		= {
	sizeof(InputFrmtComb[0]),		// [Input Format]
	sizeof(InputModeComb[0]),	// [In Port Mode]
	sizeof(ActRegionComb[0]),		// [Active Region]
	sizeof(OnOffModeComb[0]),	// [CbCr422 Swap]
	sizeof(SrcClkDlyComb[0]),		// [Clock Delay]
	sizeof(OnOffModeComb[0]),	// [In-FLD Bypass]
	sizeof(TFldLevelComb[0]),		// [Top-FLD Level]
	0xFF,		// [HACT-Offset]
	0xFF		// [VACT-Offset]
};
*/	//by hungry 2012.03.14

//--------------------------------------------------------------------------------------------------------------------------
// 8-1.Audio				 //by hungry 2012.03.14
//--------------------------------------------------------------------------------------------------------------------------

static ROMDATA char InputSourceComb[][11]		= {
	" SDI Audio ", " Stereo LR "
};

// [ Input Source ]
static ROMDATA BYTE defAudio[]			= {0,0};	// initial value

static ROMDATA BYTE defAudioMax[]		= {
	sizeof(InputSourceComb)/sizeof(InputSourceComb[0])	// [Audio Input Source]
};

static ROMDATA char *pAudioItem[]		= {
	InputSourceComb[0],			// [Audio Input Source]
	InputSourceComb[1]			// [Audio Input Source]
};

static ROMDATA BYTE defAudioSize[]		= {
	sizeof(InputSourceComb[0])		// [Audio Input Source]
};


#endif	/* __OSDMENU_H__ */


#endif
