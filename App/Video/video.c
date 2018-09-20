// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"common.h"

#if defined(SYSTEM_USE_MDIN380)
// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
/*	 //by hungry 2012.03.07
// default value for in-offset using VDCNV
static ROMDATA WORD defVDCNVOffset[][2]		= {
//	 Hgap  Vgap
	{   0,    0},	// NTSC
	{   0,    0}	// PAL
};
*/
/*	   //by hungry 2012.03.07
// default value for in-offset using ADCNV
static ROMDATA WORD defADCNVOffset[][2]		= {
//	 Hgap  Vgap
	{  52,   28},	// 720x480i60
	{  66,   36},	// 720x576i50
	{  70,   29},	// 720x480p60
	{  66,   38},	// 720x576p50
	{ 122,   19},	// 1280x720p60
	{ 150,   19},	// 1280x720p50
	{  34,   28},	// 1920x1080i60
	{ 166,   28},	// 1920x1080i50
	{  34,   36},	// 1920x1080p30
	{ 166,   36},	// 1920x1080p25

	{  77,   33},	// 640x480p60
	{ 150,   23},	// 800x600p60
	{ 229,   29},	// 1024x768p60
	{ 293,   38},	// 1280x1024p60

	{ 261,   22},	// 1280x720pRGB
	{ 288,   36},	// 1280x960p60
	{ 301,   18},	// 1360x768p60
	{ 316,   28},	// 1440x900p60

#if VGA_SOURCE_EXTENSION == 1
	{ 101,   28},	// 640x480p72
	{ 117,   16},	// 640x480p75
	{ 133,   22},	// 800x600p56
	{ 117,   23},	// 800x600p72
	{ 174,   21},	// 800x600p75
	{ 213,   29},	// 1024x768p70
	{ 205,   28},	// 1024x768p75
	{ 317,   32},	// 1152x864p75
	{ 268,   24},	// 1280x800p60
#endif
};
*/
/*	//by hungry 2012.03.07
#if __BOARD_REV_ == 0
// default value for in-offset using HDMI
static ROMDATA WORD defDVIRXOffset[][2]		= {
//	 Hgap  Vgap
	{  59,   30},	// 720x480i60
	{  67,   38},	// 720x576i50
	{  60,   30},	// 720x480p60
	{  68,   39},	// 720x576p50
	{ 220,   20},	// 1280x720p60
	{ 220,   20},	// 1280x720p50
	{ 148,   30},	// 1920x1080i60
	{ 148,   30},	// 1920x1080i50
	{ 148,   36},	// 1920x1080p60
	{ 148,   15},	// 1920x1080p50

	{  48,   33},	// 640x480p60
	{  88,   23},	// 800x600p60
	{ 160,   29},	// 1024x768p60
	{ 248,   38},	// 1280x1024p60

	{ 258,   21},	// 1280x720pRGB
	{ 288,   36},	// 1280x960p60
	{ 298,   17},	// 1360x768p60
	{ 314,   30},	// 1440x900p60

#if VGA_SOURCE_EXTENSION == 1
	{  48,   33},	// 640x480p72
	{  48,   33},	// 640x480p75
	{ 133,   22},	// 800x600p56
	{  88,   23},	// 800x600p72
	{  88,   23},	// 800x600p75
	{ 160,   29},	// 1024x768p70
	{ 205,   28},	// 1024x768p75
	{ 317,   32},	// 1152x864p75
	{ 288,   36},	// 1280x800p60
#endif
};
#endif	*//*__BOARD_REV_ == 0	*/

// default value for 4CH-SBOX area
static ROMDATA WORD def4CHSBoxWND[][4]		= {
	{   0,    0,  719,  479},	// NTSC
	{   0,    0,  719,  575},	// PAL
};

// default value for AUX filter
static ROMDATA MDIN_AUXFILT_COEF defAUXFiltCoef[]	= {
  {	{0x00aa, 0x0047, 0x03e4, 0x0000, 0x0000},	// HY - 1920x1080 to 1280x1024
	{0x00aa, 0x0047, 0x03e4, 0x0000},			// HC
	{0x00f4, 0x000b, 0x03fb},					// VY
	{0x00f2, 0x0007}  },						// VC

  {	{0x008a, 0x0051, 0x03f8, 0x03f2, 0x0000},	// HY - 1920x1080 to 1024x768
	{0x0088, 0x003e, 0x03fe, 0x0000},			// HC
	{0x00b6, 0x0034, 0x03f1},					// VY
	{0x00b6, 0x0025}  },						// VC

  {	{0x006a, 0x0044, 0x000a, 0x03fd, 0x0000},	// HY - 1920x1080 to 800x600
	{0x006c, 0x0044, 0x0006, 0x0000},			// HC
	{0x008e, 0x003d, 0x03fc},					// VY
	{0x008e, 0x0039}  },						// VC

  {	{0x0060, 0x0043, 0x000f, 0x03fe, 0x0000},	// HY - 1920x1080 to 720x576
	{0x0060, 0x0046, 0x000a, 0x0000},			// HC
	{0x0088, 0x003f, 0x03fd},					// VY
	{0x008a, 0x003b}  },						// VC

  {	{0x0060, 0x0043, 0x000f, 0x03fe, 0x0000},	// HY - 1920x1080 to 720x480
	{0x0060, 0x0046, 0x000a, 0x0000},			// HC
	{0x0072, 0x0042, 0x0005},					// VY
	{0x0072, 0x0047}  },						// VC
};

MDIN_VIDEO_INFO		stVideo;
MDIN_INTER_WINDOW	stInterWND;
MDIN_VIDEO_WINDOW	stZOOM, stCROP;

BYTE AdjInterWND,  InputSelect, InputSelOld,  SrcSyncInfo;
BYTE SrcMainFrmt, PrevSrcMainFrmt, SrcMainMode, PrevSrcMainMode;
BYTE OutMainFrmt, PrevOutMainFrmt, OutMainMode, PrevOutMainMode;
BYTE SrcAuxFrmt, PrevSrcAuxFrmt, SrcAuxMode, PrevSrcAuxMode;
BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
BYTE AdcVideoFrmt, PrevAdcFrmt, EncVideoFrmt, PrevEncFrmt;
BYTE TempOutMainMode;		// 28Dec2011
BOOL fSyncParsed;

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
/*void test_stVideo_print(void)
{
	int i;
	
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	
	//------------------------------------------------------------------------------------------------
	printf("stVideo.exeFLAG = 0x%02x;\n",stVideo.exeFLAG);		// the flag of execution of video process
	printf("stVideo.dspFLAG = 0x%02x;\n",stVideo.dspFLAG);		// the flag of display of aux-video

	printf("stVideo.srcPATH = 0x%02x;\n",stVideo.srcPATH);		// path of input video (one of MDIN_SRCVIDEO_PATH_t)
	printf("stVideo.dacPATH = 0x%02x;\n",stVideo.dacPATH);		// path of DAC output (one of MDIN_DACVIDEO_PATH_t)

	printf("stVideo.encPATH = 0x%02x;\n",stVideo.encPATH);		// input-path of VENC (one of MDIN_VENC_SRCPATH_t)
	printf("stVideo.encFRMT = 0x%02x;\n",stVideo.encFRMT);		// video format of VENC (one of MDIN_VENC_FORMAT_t)

	printf("stVideo.ffcH_m = 0x%04x;\n",stVideo.ffcH_m);		// the size of front format conversion



	//------------------------------------------------------------------------------------------------
	// source video info of inport A
	printf("stVideo.stSRC_a.frmt = 0x%02x;\n",stVideo.stSRC_a.frmt);
	printf("stVideo.stSRC_a.mode = 0x%02x;\n",stVideo.stSRC_a.mode);
	printf("stVideo.stSRC_a.fine = 0x%04x;\n",stVideo.stSRC_a.fine);
	printf("stVideo.stSRC_a.offH = 0x%04x;\n",stVideo.stSRC_a.offH);
	printf("stVideo.stSRC_a.offV = 0x%04x;\n",stVideo.stSRC_a.offV);

	printf("stVideo.stSRC_a.stATTB.Htot = 0x%04x;\n",stVideo.stSRC_a.stATTB.Htot);
	printf("stVideo.stSRC_a.stATTB.attb = 0x%04x;\n",stVideo.stSRC_a.stATTB.attb);
	printf("stVideo.stSRC_a.stATTB.Hact = 0x%04x;\n",stVideo.stSRC_a.stATTB.Hact);
	printf("stVideo.stSRC_a.stATTB.Vact = 0x%04x;\n",stVideo.stSRC_a.stATTB.Vact);

	for(i=0;i<9;i++)
		printf("stVideo.stSRC_a.pCSC.coef[%d] = 0x%04x;\n",i,stVideo.stSRC_a.pCSC->coef[i]);
	
	for(i=0;i<3;i++)
		printf("stVideo.stSRC_a.pCSC.in[%d] = 0x%04x;\n",i,stVideo.stSRC_a.pCSC->in[i]);
	
	for(i=0;i<3;i++)
		printf("stVideo.stSRC_a.pCSC.out[%d] = 0x%04x;\n",i,stVideo.stSRC_a.pCSC->out[i]);

	printf("stVideo.stSRC_a.pCSC.ctrl = 0x%04x;\n",stVideo.stSRC_a.pCSC->ctrl);



	//------------------------------------------------------------------------------------------------
	// source video info of inport B
	printf("stVideo.stSRC_b.frmt = 0x%02x;\n",stVideo.stSRC_b.frmt);
	printf("stVideo.stSRC_b.mode = 0x%02x;\n",stVideo.stSRC_b.mode);
	printf("stVideo.stSRC_b.fine = 0x%04x;\n",stVideo.stSRC_b.fine);
	printf("stVideo.stSRC_b.offH = 0x%04x;\n",stVideo.stSRC_b.offH);
	printf("stVideo.stSRC_b.offV = 0x%04x;\n",stVideo.stSRC_b.offV);

	printf("stVideo.stSRC_b.stATTB.Htot = 0x%04x;\n",stVideo.stSRC_b.stATTB.Htot);
	printf("stVideo.stSRC_b.stATTB.attb = 0x%04x;\n",stVideo.stSRC_b.stATTB.attb);
	printf("stVideo.stSRC_b.stATTB.Hact = 0x%04x;\n",stVideo.stSRC_b.stATTB.Hact);
	printf("stVideo.stSRC_b.stATTB.Vact = 0x%04x;\n",stVideo.stSRC_b.stATTB.Vact);

	for(i=0;i<9;i++)
		printf("stVideo.stSRC_b.pCSC.coef[%d] = 0x%04x;\n",i,stVideo.stSRC_b.pCSC->coef[i]);

	for(i=0;i<3;i++)
		printf("stVideo.stSRC_b.pCSC.in[%d] = 0x%04x;\n",i,stVideo.stSRC_b.pCSC->in[i]);
	
	for(i=0;i<3;i++)
		printf("stVideo.stSRC_b.pCSC.out[%d] = 0x%04x;\n",i,stVideo.stSRC_b.pCSC->out[i]);

	printf("stVideo.stSRC_b.pCSC.ctrl = 0x%04x;\n",stVideo.stSRC_b.pCSC->ctrl);



	//------------------------------------------------------------------------------------------------
	// source video info of aux-path
	printf("stVideo.stSRC_x.frmt = 0x%02x;\n",stVideo.stSRC_x.frmt);
	printf("stVideo.stSRC_x.mode = 0x%02x;\n",stVideo.stSRC_x.mode);
	printf("stVideo.stSRC_x.fine = 0x%04x;\n",stVideo.stSRC_x.fine);
	printf("stVideo.stSRC_x.offH = 0x%04x;\n",stVideo.stSRC_x.offH);
	printf("stVideo.stSRC_x.offV = 0x%04x;\n",stVideo.stSRC_x.offV);

	printf("stVideo.stSRC_x.stATTB.Htot = 0x%04x;\n",stVideo.stSRC_x.stATTB.Htot);
	printf("stVideo.stSRC_x.stATTB.attb = 0x%04x;\n",stVideo.stSRC_x.stATTB.attb);
	printf("stVideo.stSRC_x.stATTB.Hact = 0x%04x;\n",stVideo.stSRC_x.stATTB.Hact);
	printf("stVideo.stSRC_x.stATTB.Vact = 0x%04x;\n",stVideo.stSRC_x.stATTB.Vact);

	for(i=0;i<9;i++)
		printf("stVideo.stSRC_x.pCSC.coef[%d] = 0x%04x;\n",i,stVideo.stSRC_x.pCSC->coef[i]);

	for(i=0;i<3;i++)
		printf("stVideo.stSRC_x.pCSC.in[%d] = 0x%04x;\n",i,stVideo.stSRC_x.pCSC->in[i]);
	
	for(i=0;i<3;i++)
		printf("stVideo.stSRC_x.pCSC.out[%d] = 0x%04x;\n",i,stVideo.stSRC_x.pCSC->out[i]);

	printf("stVideo.stSRC_x.pCSC.ctrl = 0x%04x;\n",stVideo.stSRC_x.pCSC->ctrl);



	//------------------------------------------------------------------------------------------------
	// output video info of main path
	printf("stVideo.stOUT_m.frmt = 0x%02x;\n",stVideo.stOUT_m.frmt);
	printf("stVideo.stOUT_m.mode = 0x%02x;\n",stVideo.stOUT_m.mode);
	printf("stVideo.stOUT_m.fine = 0x%04x;\n",stVideo.stOUT_m.fine);

	printf("stVideo.stOUT_m.brightness = 0x%02x;\n",stVideo.stOUT_m.brightness);
	printf("stVideo.stOUT_m.contrast = 0x%02x;\n",stVideo.stOUT_m.contrast);
	printf("stVideo.stOUT_m.saturation = 0x%02x;\n",stVideo.stOUT_m.saturation);
	printf("stVideo.stOUT_m.hue = 0x%02x;\n",stVideo.stOUT_m.hue);

	printf("stVideo.stOUT_m.stATTB.Htot = 0x%04x;\n",stVideo.stOUT_m.stATTB.Htot);
	printf("stVideo.stOUT_m.stATTB.attb = 0x%04x;\n",stVideo.stOUT_m.stATTB.attb);
	printf("stVideo.stOUT_m.stATTB.Hact = 0x%04x;\n",stVideo.stOUT_m.stATTB.Hact);
	printf("stVideo.stOUT_m.stATTB.Vact = 0x%04x;\n",stVideo.stOUT_m.stATTB.Vact);

	printf("stVideo.stOUT_m.pSYNC.posHD = 0x%04x;\n",stVideo.stOUT_m.pSYNC->posHD);
	printf("stVideo.stOUT_m.pSYNC.posVD = 0x%04x;\n",stVideo.stOUT_m.pSYNC->posVD);
	printf("stVideo.stOUT_m.pSYNC.posVB = 0x%04x;\n",stVideo.stOUT_m.pSYNC->posVB);
	
	printf("stVideo.stOUT_m.pSYNC.totHS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->totHS);
	printf("stVideo.stOUT_m.pSYNC.bgnHA = 0x%04x;\n",stVideo.stOUT_m.pSYNC->bgnHA);
	printf("stVideo.stOUT_m.pSYNC.endHA = 0x%04x;\n",stVideo.stOUT_m.pSYNC->endHA);
	printf("stVideo.stOUT_m.pSYNC.bgnHS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->bgnHS);
	printf("stVideo.stOUT_m.pSYNC.endHS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->endHS);
	
	printf("stVideo.stOUT_m.pSYNC.totVS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->totVS);
	printf("stVideo.stOUT_m.pSYNC.bgnVA = 0x%04x;\n",stVideo.stOUT_m.pSYNC->bgnVA);
	printf("stVideo.stOUT_m.pSYNC.endVA = 0x%04x;\n",stVideo.stOUT_m.pSYNC->endVA);
	printf("stVideo.stOUT_m.pSYNC.bgnVS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->bgnVS);
	printf("stVideo.stOUT_m.pSYNC.endVS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->endVS);

	printf("stVideo.stOUT_m.pSYNC.bgnVAB = 0x%04x;\n",stVideo.stOUT_m.pSYNC->bgnVAB);
	printf("stVideo.stOUT_m.pSYNC.endVAB = 0x%04x;\n",stVideo.stOUT_m.pSYNC->endVAB);
	printf("stVideo.stOUT_m.pSYNC.bgnVSB = 0x%04x;\n",stVideo.stOUT_m.pSYNC->bgnVSB);
	printf("stVideo.stOUT_m.pSYNC.endVSB = 0x%04x;\n",stVideo.stOUT_m.pSYNC->endVSB);
	printf("stVideo.stOUT_m.pSYNC.posFLD = 0x%04x;\n",stVideo.stOUT_m.pSYNC->posFLD);

	printf("stVideo.stOUT_m.pSYNC.vclkP = 0x%04x;\n",stVideo.stOUT_m.pSYNC->vclkP);
	printf("stVideo.stOUT_m.pSYNC.vclkM = 0x%04x;\n",stVideo.stOUT_m.pSYNC->vclkM);
	printf("stVideo.stOUT_m.pSYNC.vclkS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->vclkS);

	printf("stVideo.stOUT_m.pSYNC.xclkS = 0x%04x;\n",stVideo.stOUT_m.pSYNC->xclkS);
	printf("stVideo.stOUT_m.pSYNC.xclkF = 0x%04x;\n",stVideo.stOUT_m.pSYNC->xclkF);
	printf("stVideo.stOUT_m.pSYNC.xclkT = 0x%04x;\n",stVideo.stOUT_m.pSYNC->xclkT);

	for(i=0;i<9;i++)
		printf("stVideo.stOUT_m.pCSC.coef[%d] = 0x%04x;\n",i,stVideo.stOUT_m.pCSC->coef[i]);

	for(i=0;i<9;i++)
		printf("stVideo.stOUT_m.pCSC.in[%d] = 0x%04x;\n",i,stVideo.stOUT_m.pCSC->in[i]);

	for(i=0;i<9;i++)
		printf("stVideo.stOUT_m.pCSC.out[%d] = 0x%04x;\n",i,stVideo.stOUT_m.pCSC->out[i]);

	printf("stVideo.stOUT_m.pCSC.ctrl = 0x%04x;\n",stVideo.stOUT_m.pCSC->ctrl);


	for(i=0;i<37;i++)
		printf("stVideo.stOUT_m.pDAC.ctrl[%d] = 0x%04x;\n",i,stVideo.stOUT_m.pDAC->ctrl[i]);

	printf("stVideo.stOUT_m.pDAC.dummy = 0x%04x;\n",stVideo.stOUT_m.pDAC->dummy);

	
	//------------------------------------------------------------------------------------------------
	// crop window of main path for image cut
	printf("stVideo.stCROP_m.w = 0x%04x;\n",stVideo.stCROP_m.w);
	printf("stVideo.stCROP_m.h = 0x%04x;\n",stVideo.stCROP_m.h);
	printf("stVideo.stCROP_m.x = 0x%04x;\n",stVideo.stCROP_m.x);
	printf("stVideo.stCROP_m.y = 0x%04x;\n",stVideo.stCROP_m.y);

	//------------------------------------------------------------------------------------------------
	// zoom window of main path for overscan
	printf("stVideo.stZOOM_m.w = 0x%04x;\n",stVideo.stZOOM_m.w);
	printf("stVideo.stZOOM_m.h = 0x%04x;\n",stVideo.stZOOM_m.h);
	printf("stVideo.stZOOM_m.x = 0x%04x;\n",stVideo.stZOOM_m.x);
	printf("stVideo.stZOOM_m.y = 0x%04x;\n",stVideo.stZOOM_m.y);

	//------------------------------------------------------------------------------------------------
	// view window of main path for aspect ratio
	printf("stVideo.stVIEW_m.w = 0x%04x;\n",stVideo.stVIEW_m.w);
	printf("stVideo.stVIEW_m.h = 0x%04x;\n",stVideo.stVIEW_m.h);
	printf("stVideo.stVIEW_m.x = 0x%04x;\n",stVideo.stVIEW_m.x);
	printf("stVideo.stVIEW_m.y = 0x%04x;\n",stVideo.stVIEW_m.y);




	//------------------------------------------------------------------------------------------------
	// output video info of aux path
	printf("stVideo.stOUT_x.frmt = 0x%02x;\n",stVideo.stOUT_x.frmt);
	printf("stVideo.stOUT_x.mode = 0x%02x;\n",stVideo.stOUT_x.mode);
	printf("stVideo.stOUT_x.fine = 0x%04x;\n",stVideo.stOUT_x.fine);

	printf("stVideo.stOUT_x.brightness = 0x%02x;\n",stVideo.stOUT_x.brightness);
	printf("stVideo.stOUT_x.contrast = 0x%02x;\n",stVideo.stOUT_x.contrast);
	printf("stVideo.stOUT_x.saturation = 0x%02x;\n",stVideo.stOUT_x.saturation);
	printf("stVideo.stOUT_x.hue = 0x%02x;\n",stVideo.stOUT_x.hue);

	printf("stVideo.stOUT_x.stATTB.Htot = 0x%04x;\n",stVideo.stOUT_x.stATTB.Htot);
	printf("stVideo.stOUT_x.stATTB.attb = 0x%04x;\n",stVideo.stOUT_x.stATTB.attb);
	printf("stVideo.stOUT_x.stATTB.Hact = 0x%04x;\n",stVideo.stOUT_x.stATTB.Hact);
	printf("stVideo.stOUT_x.stATTB.Vact = 0x%04x;\n",stVideo.stOUT_x.stATTB.Vact);

	printf("stVideo.stOUT_x.pSYNC.posHD = 0x%04x;\n",stVideo.stOUT_x.pSYNC->posHD);
	printf("stVideo.stOUT_x.pSYNC.posVD = 0x%04x;\n",stVideo.stOUT_x.pSYNC->posVD);
	printf("stVideo.stOUT_x.pSYNC.posVB = 0x%04x;\n",stVideo.stOUT_x.pSYNC->posVB);
	
	printf("stVideo.stOUT_x.pSYNC.totHS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->totHS);
	printf("stVideo.stOUT_x.pSYNC.bgnHA = 0x%04x;\n",stVideo.stOUT_x.pSYNC->bgnHA);
	printf("stVideo.stOUT_x.pSYNC.endHA = 0x%04x;\n",stVideo.stOUT_x.pSYNC->endHA);
	printf("stVideo.stOUT_x.pSYNC.bgnHS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->bgnHS);
	printf("stVideo.stOUT_x.pSYNC.endHS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->endHS);
	
	printf("stVideo.stOUT_x.pSYNC.totVS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->totVS);
	printf("stVideo.stOUT_x.pSYNC.bgnVA = 0x%04x;\n",stVideo.stOUT_x.pSYNC->bgnVA);
	printf("stVideo.stOUT_x.pSYNC.endVA = 0x%04x;\n",stVideo.stOUT_x.pSYNC->endVA);
	printf("stVideo.stOUT_x.pSYNC.bgnVS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->bgnVS);
	printf("stVideo.stOUT_x.pSYNC.endVS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->endVS);

	printf("stVideo.stOUT_x.pSYNC.bgnVAB = 0x%04x;\n",stVideo.stOUT_x.pSYNC->bgnVAB);
	printf("stVideo.stOUT_x.pSYNC.endVAB = 0x%04x;\n",stVideo.stOUT_x.pSYNC->endVAB);
	printf("stVideo.stOUT_x.pSYNC.bgnVSB = 0x%04x;\n",stVideo.stOUT_x.pSYNC->bgnVSB);
	printf("stVideo.stOUT_x.pSYNC.endVSB = 0x%04x;\n",stVideo.stOUT_x.pSYNC->endVSB);
	printf("stVideo.stOUT_x.pSYNC.posFLD = 0x%04x;\n",stVideo.stOUT_x.pSYNC->posFLD);

	printf("stVideo.stOUT_x.pSYNC.vclkP = 0x%04x;\n",stVideo.stOUT_x.pSYNC->vclkP);
	printf("stVideo.stOUT_x.pSYNC.vclkM = 0x%04x;\n",stVideo.stOUT_x.pSYNC->vclkM);
	printf("stVideo.stOUT_x.pSYNC.vclkS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->vclkS);

	printf("stVideo.stOUT_x.pSYNC.xclkS = 0x%04x;\n",stVideo.stOUT_x.pSYNC->xclkS);
	printf("stVideo.stOUT_x.pSYNC.xclkF = 0x%04x;\n",stVideo.stOUT_x.pSYNC->xclkF);
	printf("stVideo.stOUT_x.pSYNC.xclkT = 0x%04x;\n",stVideo.stOUT_x.pSYNC->xclkT);

	for(i=0;i<9;i++)
		printf("stVideo.stOUT_x.pCSC.coef[%d] = 0x%04x;\n",i,stVideo.stOUT_x.pCSC->coef[i]);

	for(i=0;i<9;i++)
		printf("stVideo.stOUT_x.pCSC.in[%d] = 0x%04x;\n",i,stVideo.stOUT_x.pCSC->in[i]);

	for(i=0;i<9;i++)
		printf("stVideo.stOUT_x.pCSC.out[%d] = 0x%04x;\n",i,stVideo.stOUT_x.pCSC->out[i]);

	printf("stVideo.stOUT_x.pCSC.ctrl = 0x%04x;\n",stVideo.stOUT_x.pCSC->ctrl);


	for(i=0;i<37;i++)
		printf("stVideo.stOUT_x.pDAC.ctrl[%d] = 0x%04x;\n",i,stVideo.stOUT_x.pDAC->ctrl[i]);

	printf("stVideo.stOUT_x.pDAC.dummy = 0x%04x;\n",stVideo.stOUT_x.pDAC->dummy);

	
	//------------------------------------------------------------------------------------------------
	// crop window of aux path for image cut
	printf("stVideo.stCROP_x.w = 0x%04x;\n",stVideo.stCROP_x.w);
	printf("stVideo.stCROP_x.h = 0x%04x;\n",stVideo.stCROP_x.h);
	printf("stVideo.stCROP_x.x = 0x%04x;\n",stVideo.stCROP_x.x);
	printf("stVideo.stCROP_x.y = 0x%04x;\n",stVideo.stCROP_x.y);

	//------------------------------------------------------------------------------------------------
	// zoom window of aux path for overscan
	printf("stVideo.stZOOM_x.w = 0x%04x;\n",stVideo.stZOOM_x.w);
	printf("stVideo.stZOOM_x.h = 0x%04x;\n",stVideo.stZOOM_x.h);
	printf("stVideo.stZOOM_x.x = 0x%04x;\n",stVideo.stZOOM_x.x);
	printf("stVideo.stZOOM_x.y = 0x%04x;\n",stVideo.stZOOM_x.y);

	//------------------------------------------------------------------------------------------------
	// view window of aux path for aspect ratio
	printf("stVideo.stVIEW_x.w = 0x%04x;\n",stVideo.stVIEW_x.w);
	printf("stVideo.stVIEW_x.h = 0x%04x;\n",stVideo.stVIEW_x.h);
	printf("stVideo.stVIEW_x.x = 0x%04x;\n",stVideo.stVIEW_x.x);
	printf("stVideo.stVIEW_x.y = 0x%04x;\n",stVideo.stVIEW_x.y);



	//------------------------------------------------------------------------------------------------
	// frame buffer map of main&aux path
	printf("stVideo.stMAP_m.frmt = 0x%02x;\n",stVideo.stMAP_m.frmt);
	printf("stVideo.stMAP_m.dummy = 0x%02x;\n",stVideo.stMAP_m.dummy);

	printf("stVideo.stMAP_m.Y_m = 0x%02x;\n",stVideo.stMAP_m.Y_m);
	printf("stVideo.stMAP_m.Ynr = 0x%02x;\n",stVideo.stMAP_m.Ynr);
	printf("stVideo.stMAP_m.C_m = 0x%02x;\n",stVideo.stMAP_m.C_m);
	printf("stVideo.stMAP_m.Ymh = 0x%02x;\n",stVideo.stMAP_m.Ymh);
	printf("stVideo.stMAP_m.Y_x = 0x%02x;\n",stVideo.stMAP_m.Y_x);
	printf("stVideo.stMAP_m.C_x = 0x%02x;\n",stVideo.stMAP_m.C_x);



	//------------------------------------------------------------------------------------------------
	// deinterlacer of main path only
	printf("stVideo.stIPC_m.mode = 0x%02x;\n",stVideo.stIPC_m.mode);
	printf("stVideo.stIPC_m.film = 0x%02x;\n",stVideo.stIPC_m.film);
	printf("stVideo.stIPC_m.gain = 0x%02x;\n",stVideo.stIPC_m.gain);
	printf("stVideo.stIPC_m.dummy = 0x%02x;\n",stVideo.stIPC_m.dummy);

	printf("stVideo.stIPC_m.fine = 0x%04x;\n",stVideo.stIPC_m.fine);
	printf("stVideo.stIPC_m.attb = 0x%04x;\n",stVideo.stIPC_m.attb);


//	PMDIN_MFCFILT_COEF	pHY_m;		// MFCHY filter coefficient of main path only
//	PMDIN_MFCFILT_COEF	pHC_m;		// MFCHC filter coefficient of main path only
//	PMDIN_MFCFILT_COEF	pVY_m;		// MFCVY filter coefficient of main path only
//	PMDIN_MFCFILT_COEF	pVC_m;		// MFCVC filter coefficient of main path only

//	MDIN_SRCVIDEO_INFO	stSRC_m;	// source video info of main path, but only use in API function

	//------------------------------------------------------------------------------------------------
	// scaler info of main path, but only use in API function
	printf("stVideo.stMFC_m.stFFC.sw = %d;\n",stVideo.stMFC_m.stFFC.sw);
	printf("stVideo.stMFC_m.stFFC.sh = %d;\n",stVideo.stMFC_m.stFFC.sh);
	printf("stVideo.stMFC_m.stFFC.dw = %d;\n",stVideo.stMFC_m.stFFC.dw);
	printf("stVideo.stMFC_m.stFFC.dh = %d;\n",stVideo.stMFC_m.stFFC.dh);

	printf("stVideo.stMFC_m.stCRS.sw = %d;\n",stVideo.stMFC_m.stCRS.sw);
	printf("stVideo.stMFC_m.stCRS.sh = %d;\n",stVideo.stMFC_m.stCRS.sh);
	printf("stVideo.stMFC_m.stCRS.dw = %d;\n",stVideo.stMFC_m.stCRS.dw);
	printf("stVideo.stMFC_m.stCRS.dh = %d;\n",stVideo.stMFC_m.stCRS.dh);

	printf("stVideo.stMFC_m.stORS.sw = %d;\n",stVideo.stMFC_m.stORS.sw);
	printf("stVideo.stMFC_m.stORS.sh = %d;\n",stVideo.stMFC_m.stORS.sh);
	printf("stVideo.stMFC_m.stORS.dw = %d;\n",stVideo.stMFC_m.stORS.dw);
	printf("stVideo.stMFC_m.stORS.dh = %d;\n",stVideo.stMFC_m.stORS.dh);

	printf("stVideo.stMFC_m.stCUT.w = %d;\n",stVideo.stMFC_m.stCUT.w);
	printf("stVideo.stMFC_m.stCUT.h = %d;\n",stVideo.stMFC_m.stCUT.h);
	printf("stVideo.stMFC_m.stCUT.x = %d;\n",stVideo.stMFC_m.stCUT.x);
	printf("stVideo.stMFC_m.stCUT.y = %d;\n",stVideo.stMFC_m.stCUT.y);

	printf("stVideo.stMFC_m.stSRC.w = %d;\n",stVideo.stMFC_m.stSRC.w);
	printf("stVideo.stMFC_m.stSRC.h = %d;\n",stVideo.stMFC_m.stSRC.h);
	printf("stVideo.stMFC_m.stSRC.x = %d;\n",stVideo.stMFC_m.stSRC.x);
	printf("stVideo.stMFC_m.stSRC.y = %d;\n",stVideo.stMFC_m.stSRC.y);

	printf("stVideo.stMFC_m.stDST.w = %d;\n",stVideo.stMFC_m.stDST.w);
	printf("stVideo.stMFC_m.stDST.h = %d;\n",stVideo.stMFC_m.stDST.h);
	printf("stVideo.stMFC_m.stDST.x = %d;\n",stVideo.stMFC_m.stDST.x);
	printf("stVideo.stMFC_m.stDST.y = %d;\n",stVideo.stMFC_m.stDST.y);

	printf("stVideo.stMFC_m.stMEM.w = %d;\n",stVideo.stMFC_m.stMEM.w);
	printf("stVideo.stMFC_m.stMEM.h = %d;\n",stVideo.stMFC_m.stMEM.h);
	printf("stVideo.stMFC_m.stMEM.x = %d;\n",stVideo.stMFC_m.stMEM.x);
	printf("stVideo.stMFC_m.stMEM.y = %d;\n",stVideo.stMFC_m.stMEM.y);

	printf("stVideo.stMFC_m.st4CH.w = %d;\n",stVideo.stMFC_m.st4CH.w);
	printf("stVideo.stMFC_m.st4CH.h = %d;\n",stVideo.stMFC_m.st4CH.h);
	printf("stVideo.stMFC_m.st4CH.x = %d;\n",stVideo.stMFC_m.st4CH.x);
	printf("stVideo.stMFC_m.st4CH.y = %d;\n",stVideo.stMFC_m.st4CH.y);


	//------------------------------------------------------------------------------------------------
	// scaler info of aux path, but only use in API function
	printf("stVideo.stMFC_x.stFFC.sw = %d;\n",stVideo.stMFC_x.stFFC.sw);
	printf("stVideo.stMFC_x.stFFC.sh = %d;\n",stVideo.stMFC_x.stFFC.sh);
	printf("stVideo.stMFC_x.stFFC.dw = %d;\n",stVideo.stMFC_x.stFFC.dw);
	printf("stVideo.stMFC_x.stFFC.dh = %d;\n",stVideo.stMFC_x.stFFC.dh);

	printf("stVideo.stMFC_x.stCRS.sw = %d;\n",stVideo.stMFC_x.stCRS.sw);
	printf("stVideo.stMFC_x.stCRS.sh = %d;\n",stVideo.stMFC_x.stCRS.sh);
	printf("stVideo.stMFC_x.stCRS.dw = %d;\n",stVideo.stMFC_x.stCRS.dw);
	printf("stVideo.stMFC_x.stCRS.dh = %d;\n",stVideo.stMFC_x.stCRS.dh);

	printf("stVideo.stMFC_x.stORS.sw = %d;\n",stVideo.stMFC_x.stORS.sw);
	printf("stVideo.stMFC_x.stORS.sh = %d;\n",stVideo.stMFC_x.stORS.sh);
	printf("stVideo.stMFC_x.stORS.dw = %d;\n",stVideo.stMFC_x.stORS.dw);
	printf("stVideo.stMFC_x.stORS.dh = %d;\n",stVideo.stMFC_x.stORS.dh);

	printf("stVideo.stMFC_x.stCUT.w = %d;\n",stVideo.stMFC_x.stCUT.w);
	printf("stVideo.stMFC_x.stCUT.h = %d;\n",stVideo.stMFC_x.stCUT.h);
	printf("stVideo.stMFC_x.stCUT.x = %d;\n",stVideo.stMFC_x.stCUT.x);
	printf("stVideo.stMFC_x.stCUT.y = %d;\n",stVideo.stMFC_x.stCUT.y);

	printf("stVideo.stMFC_x.stSRC.w = %d;\n",stVideo.stMFC_x.stSRC.w);
	printf("stVideo.stMFC_x.stSRC.h = %d;\n",stVideo.stMFC_x.stSRC.h);
	printf("stVideo.stMFC_x.stSRC.x = %d;\n",stVideo.stMFC_x.stSRC.x);
	printf("stVideo.stMFC_x.stSRC.y = %d;\n",stVideo.stMFC_x.stSRC.y);

	printf("stVideo.stMFC_x.stDST.w = %d;\n",stVideo.stMFC_x.stDST.w);
	printf("stVideo.stMFC_x.stDST.h = %d;\n",stVideo.stMFC_x.stDST.h);
	printf("stVideo.stMFC_x.stDST.x = %d;\n",stVideo.stMFC_x.stDST.x);
	printf("stVideo.stMFC_x.stDST.y = %d;\n",stVideo.stMFC_x.stDST.y);

	printf("stVideo.stMFC_x.stMEM.w = %d;\n",stVideo.stMFC_x.stMEM.w);
	printf("stVideo.stMFC_x.stMEM.h = %d;\n",stVideo.stMFC_x.stMEM.h);
	printf("stVideo.stMFC_x.stMEM.x = %d;\n",stVideo.stMFC_x.stMEM.x);
	printf("stVideo.stMFC_x.stMEM.y = %d;\n",stVideo.stMFC_x.stMEM.y);

	printf("stVideo.stMFC_x.st4CH.w = %d;\n",stVideo.stMFC_x.st4CH.w);
	printf("stVideo.stMFC_x.st4CH.h = %d;\n",stVideo.stMFC_x.st4CH.h);
	printf("stVideo.stMFC_x.st4CH.x = %d;\n",stVideo.stMFC_x.st4CH.x);
	printf("stVideo.stMFC_x.st4CH.y = %d;\n",stVideo.stMFC_x.st4CH.y);


	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
}
*/

//--------------------------------------------------------------------------------------------------------------------------
static void MDIN3xx_SetRegInitial(void)
{
	WORD nID = 0;

	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id

	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP3);		// set in_data_map_mode = �11�
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP0);		// disable digital out
	MDINOSD_SetBGLayerColor(RGB(128,128,128));			// set BG-Layer color

	MDINOSD_SetBGBoxColor(RGB(255,255,255));			// set BG-BOX color

	// setup enhancement
	MDIN3xx_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDIN3xx_SetPeakingFilterCoef(NULL);		// set default peaking filter coef
	MDIN3xx_SetColorEnFilterCoef(NULL);		// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(NULL);		// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(NULL);		// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(NULL);	// set default colorton filter coef

	MDIN3xx_EnableLTI(OFF);					// set LTI off
	MDIN3xx_EnableCTI(OFF);					// set CTI off
	MDIN3xx_SetPeakingFilterLevel(0);		// set peaking gain
	MDIN3xx_EnablePeakingFilter(ON);		// set peaking on
	MDIN3xx_EnableColorEnFilter(ON);		// set color enhancement on, 29Mar2012

	MDIN3xx_EnableFrontNRFilter(OFF);		// set frontNR off
	MDIN3xx_EnableBWExtension(OFF);			// set B/W extension off

	MDIN3xx_SetIPCBlock();		// initialize IPC block (3DNR gain is 37)

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);

	// set aux display ON
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo.srcPATH = PATH_MAIN_A_AUX_A;	// set main is A, aux is A
	stVideo.dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo.stSRC_a.frmt =  VIDSRC_1920x1080p60;//VIDSRC_1920x1080p60 VIDSRC_1280x1024p60
	stVideo.stSRC_a.mode = MDIN_SRC_RGB444_8;
	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_a.offH = 0;	//API v0.31(2012.05.02)
	stVideo.stSRC_a.offV = 0;	//API v0.31(2012.05.02)

	// define video format of MAIN-OUTPUT

	stVideo.stOUT_m.frmt = VIDOUT_1920x1080p60;	   //by hungry 2012.03.07
	stVideo.stOUT_m.mode = MDIN_OUT_RGB444_8;	 //by hungry 2012.03.06		// test by chungsa
	stVideo.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo.stOUT_m.brightness = 128;			// set main picture factor
	stVideo.stOUT_m.contrast = 128;
	stVideo.stOUT_m.saturation = 128;
	stVideo.stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo.stOUT_m.g_gain = 128;
	stVideo.stOUT_m.b_gain = 128;
	stVideo.stOUT_m.r_offset = 128;
	stVideo.stOUT_m.g_offset = 128;
	stVideo.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo.stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo.stIPC_m.gain = 34;
	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	stVideo.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_ON

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = VIDSRC_720x480i60;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of AUX-INPUT
	stVideo.stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24

	// define video format of AUX-OUTPUT
	stVideo.stOUT_x.frmt = VIDOUT_720x480i60;
	stVideo.stOUT_x.mode = MDIN_OUT_MUX656_8;
	stVideo.stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run

	stVideo.stOUT_x.brightness = 128;			// set aux picture factor
	stVideo.stOUT_x.contrast = 128;
	stVideo.stOUT_x.saturation = 128;
	stVideo.stOUT_x.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo.stOUT_x.g_gain = 128;
	stVideo.stOUT_x.b_gain = 128;
	stVideo.stOUT_x.r_offset = 128;
	stVideo.stOUT_x.g_offset = 128;
	stVideo.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo.encFRMT = VID_VENC_NTSC_M;

	// define video format of HDMI-OUTPUT
	stVideo.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo.stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;
	MDINHTX_SetHDMIBlock(&stVideo);		// initialize HDMI block

	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = MDIN_4CHVIEW_ALL;	 // set 4CH view mode

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process

	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90;		stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
	InputSelOld = 0xff;	InputSelect = VIDEO_DIGITAL_SDI;		 //by hungry 2012.03.06

	PrevSrcMainFrmt = 0xff;	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;	OutMainMode = stVideo.stOUT_m.mode;
	TempOutMainMode = OutMainMode;	// 28Dec2011

	PrevSrcAuxFrmt = 0xff; SrcAuxFrmt = stVideo.stSRC_b.frmt;
	PrevSrcAuxMode = 0xff;	SrcAuxMode = stVideo.stSRC_b.mode;

	PrevOutAuxFrmt = 0xff; OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;	OutAuxMode = stVideo.stOUT_x.mode;

	PrevEncFrmt = 0xff;	EncVideoFrmt = stVideo.encFRMT;
}

/*void Change_Main_Aux_Input(BYTE sel)
{
	if(sel == 0)
		stVideo.srcPATH = PATH_MAIN_A_AUX_B;
	else 
		stVideo.srcPATH = PATH_MAIN_B_AUX_A;

	MDIN3xx_SetSrcVideoFrmt(&stVideo);
	//MDINAUX_SetSrcVideoFrmt(&stVideo);
}*/	

/*	//by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void DEMOKIT_InputSource(BYTE src)
{
	InputSelect = src;	SetOSDMenuID(0x6110);
	SetOSDItemID(src);	SetMenuStatus(6,1,src);
}
*/
//--------------------------------------------------------------------------------------------------
//static void DEMOKIT_Outputfrmt(BYTE frmt)		//by hungry 2012.03.06
void SetVideoOutputfrmt(BYTE frmt)		//by hungry 2012.03.06
{
//	SetOSDMenuID(0x6210);
//	SetOSDItemID(frmt);
//	SetMenuStatus(6,2,frmt);
	stVideo.stOUT_m.frmt = frmt;
	OutMainFrmt = frmt;
}
//--------------------------------------------------------------------------------------------------
void CreateVideoInstance(void)
{
	MDIN3xx_SetRegInitial();	// initialize MDIN-3xx
}
/*		 //by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void SetInputSource(BYTE src)
{
	BOOL vd5160 = OFF, vd2866 = OFF, adec = OFF, hdmi = OFF;
	VD2866_PORT_t vd2866_mode = VD2866_PORT_4CH;

	switch (src) {
		case VIDEO_VDCNV_TVP5160:			vd5160 = ON; 
//			vd2866_mode = VD2866_PORT_B;	vd2866 = ON; break;
			vd2866_mode = VD2866_PORT_B;	vd2866 = OFF; break;	  //API ver0.26 2012.02.13
		case VIDEO_VDCNV_TW2866:			
			vd2866_mode = VD2866_PORT_AB;	vd2866 = ON; break;
		case VIDEO_ADCNV_COMPONENT:		adec = ON;
//			vd2866_mode = VD2866_PORT_B;	vd2866 = ON; break;
			vd2866_mode = VD2866_PORT_B;	vd2866 = OFF; break;	  //API ver0.26 2012.02.13
		case VIDEO_ADCNV_PC_RGB:			adec = ON;
//			vd2866_mode = VD2866_PORT_B;	vd2866 = ON; break;
			vd2866_mode = VD2866_PORT_B;	vd2866 = OFF; break;		  //API ver0.26 2012.02.13
		case VIDEO_VDCNV_4CH_IN:		
			vd2866_mode = VD2866_PORT_4CH;	vd2866 = ON;	break;
		case VIDEO_DIGITAL_HDMI:			hdmi = ON;
#if __BOARD_REV_ == 11
			vd2866_mode = VD2866_PORT_A;	vd2866 = ON; break;	// modified on 29Mar2012
#else
			vd2866_mode = VD2866_PORT_B;	vd2866 = ON; break;
#endif
		case VIDEO_ADCNV_2HD_IN:			adec = ON; hdmi = ON;
			vd2866_mode = VD2866_PORT_AB;	vd2866 = OFF; break;
		case VIDEO_DIGITAL_SDI:										   //by hungry 2012.02.15
			vd2866_mode = VD2866_PORT_B;	vd2866 = OFF; break;		
			break;	

	}

//	VD5160_EnableOutVideo(vd5160);		  				//by hungry 2012.02.09
//	VD2866_EnableOutVideo(vd2866_mode, vd2866);		  	//by hungry 2012.02.09
//	ADCNV_EnableOutVideo(adec);						  	//by hungry 2012.02.09
//	HDRX_EnableOutVideo(hdmi);			  				//by hungry 2012.02.08
}
*/
//--------------------------------------------------------------------------------------------------
static void SetInVideoPath(BYTE src)
{
	switch (src) {
		case VIDEO_ADCNV_2HD_IN: 
			stVideo.srcPATH = PATH_MAIN_A_AUX_M;	stVideo.dacPATH = DAC_PATH_AUX_2HD;  break;

		case VIDEO_SDI_2HD_POP: 
			stVideo.srcPATH = PATH_MAIN_A_AUX_B;	stVideo.dacPATH = DAC_PATH_MAIN_PIP;  break;

		case VIDEO_DIGITAL_SDI: 
			stVideo.srcPATH = PATH_MAIN_A_AUX_A;	stVideo.dacPATH = DAC_PATH_MAIN_OUT; break;
		case VIDEO_DIGITAL_SDI2: 
			stVideo.srcPATH = PATH_MAIN_B_AUX_B;	stVideo.dacPATH = DAC_PATH_MAIN_OUT; break;
	}
}

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcMainMode(BYTE src)
{
	switch (src) 
	{
		case VIDEO_SDI_2HD_POP:		return MDIN_SRC_EMB422_8;	
		case VIDEO_DIGITAL_SDI:		return MDIN_SRC_RGB444_8;	//by hungry 2012.02.15
		case VIDEO_DIGITAL_SDI2:	return MDIN_SRC_RGB444_8;	//by flcl 2013.04.23

		default:					return MDIN_SRC_SEP422_8;
	}
}

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcMainFrmt(BYTE src)
{
/*		 //by hungry 2012.03.07
	switch (src) {
		case VIDEO_ADCNV_COMPONENT:	return VIDSRC_720x480i60;
		case VIDEO_ADCNV_PC_RGB:	return VIDSRC_640x480p60;
	}
*/
	return SrcMainFrmt;
}

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcAuxMode(BYTE src)
{
	switch (src) 
	{
		case VIDEO_SDI_2HD_POP:		return MDIN_SRC_EMB422_8;			 
		default:					return MDIN_SRC_MUX656_8;
	}
}

		//by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static BYTE GetSrcAuxFrmt(BYTE src)
{
	BYTE frmt = 0xff;

#if 0 //Louis
	switch (src) 
	{
		case VIDEO_SDI_2HD_POP:		frmt = SDIRX_GetVideoSystem(); break;
	}
#endif
	if (frmt == 0xff) frmt = 0x0;	// 02Jan2012
	
	return VIDSRC_1920x1080p60;//frmt; Louis temp
}

//--------------------------------------------------------------------------------------------------
/*	 //by hungry 2012.02.21
static void GetExtVideoFrmt(BYTE src)
{
	if (src!=VIDEO_EXTERNAL_IN) return;

	DEMO_SetInputFormat(GetMenuStatus(8,1));
	DEMO_SetInputMode(GetMenuStatus(8,2));
}
*/
//--------------------------------------------------------------------------------------------------
static void InputSourceHandler(BYTE src)
{
	if (src==InputSelOld) return;

//	MDIN3xx_EnableAuxDisplay(&stVideo, OFF);
//	MDIN3xx_EnableMainDisplay(OFF);

//	SetInputSource(src);		   //by hungry 2012.03.07

	SetInVideoPath(src);
	
	SrcMainMode = GetSrcMainMode(src);
	SrcMainFrmt = GetSrcMainFrmt(src);
	SrcAuxMode = GetSrcAuxMode(src);
	SrcAuxFrmt = GetSrcAuxFrmt(src);		 //by hungry 2012.03.07
	//GetExtVideoFrmt(src);	// get E-Video format,mode		//by hungry 2012.02.21

	memset(&stVideo.stVIEW_m, 0, 8);		// clear stVIEW_m
	memset(&stVideo.stVIEW_x, 0, 8);		// clear stVIEW_x

	//OutMainFrmt = GetMenuStatus(6,2);		// get out-format
	OutMainFrmt = Video_Out_Res_Val;		// get out-format
	OutAuxMode = MDIN_OUT_MUX656_8;		// set aux-mode
	OutAuxFrmt = VIDOUT_720x480i60;			// set aux-format

	fSyncParsed = FALSE;	InputSelOld = src;
	PrevSrcMainFrmt = PrevSrcMainMode = PrevAdcFrmt = 0xff;
}

//--------------------------------------------------------------------------------------------------
static void InputSyncHandler_A(BYTE src)
{
	BYTE frmt = 0xff;

	frmt = SrcMainFrmt;

/*	switch (src) 
	{
		case VIDEO_SDI_2HD_POP:		frmt = SDIRX_GetVideoSystem(); break;
		case VIDEO_DIGITAL_SDI:		frmt = SrcMainFrmt; break; //frmt = SDIRX_GetVideoSystem(); break;		   //by hungry 2012.02.15
		case VIDEO_DIGITAL_SDI2:	frmt = SDI2RX_GetVideoSystem(); break;	

		default:						frmt = SrcMainFrmt; break;
	}
*/
	if (frmt!=0xff&&frmt!=0xfe)
	{
		fSyncParsed = TRUE;
		SrcMainFrmt = frmt;
	}
	else							  //by hungry 2012.05.02 for dark screen on no video.
	{
		SrcMainFrmt = frmt;
	}

	SrcSyncInfo = (frmt==0xff)? VIDSRC_FORMAT_END : SrcMainFrmt;


	switch (stVideo.dacPATH) 
	{
		case DAC_PATH_MAIN_PIP:	
			SrcMainMode = MDIN_SRC_EMB422_8;	
			OutMainFrmt = Video_Out_Res_Val+(frmt%2);	
			OutMainMode = MDIN_OUT_RGB444_8;//MDIN_OUT_SEP422_8;
			break;
	}

}

	  //by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void InputSyncHandler_B(BYTE src)
{
	BYTE frmt = 0xff;

#if 0 //Louis
	switch (src) {
		case VIDEO_SDI_2HD_POP:		frmt = SDIRX_GetVideoSystem(); break;
	}
#endif
	
	if (frmt!=0xff&&frmt!=0xfe)
	{
		fSyncParsed = TRUE;
		SrcAuxFrmt = frmt;
	}
	/*else							  
	{
		SrcAuxFrmt = frmt;
	}*/


	switch (stVideo.dacPATH) 
	{
		case DAC_PATH_MAIN_PIP:	
			SrcAuxMode = MDIN_SRC_EMB422_8;	
			OutAuxFrmt = Video_Out_Res_Val+(frmt%2);	
			OutAuxMode = MDIN_OUT_RGB444_8;//MDIN_OUT_SEP422_8;
		break;
	}
}
	//by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void SetOffChipFrmtInA(BYTE src)
{
	switch (src) 
	{

		case VIDEO_SDI_2HD_POP : MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY5); break;
// 		case VIDEO_DIGITAL_HDMI: MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY5); break;	  //by hungry 2012.02.09
 		case VIDEO_DIGITAL_SDI: MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY5); break;	  //by hungry 2012.02.15
 		case VIDEO_DIGITAL_SDI2: MDIN3xx_SetDelayCLK_B(MDIN_CLK_DELAY5); break;	  //by flcl 2013.04.23
//		default:				 MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY0); break;
	}
}

	   //by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void SetOffChipFrmtInB(BYTE src)
{
	switch (src) 
	{
		case VIDEO_SDI_2HD_POP : MDIN3xx_SetDelayCLK_B(MDIN_CLK_DELAY5); break;
	}
}

/*	   //by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void SetVDCNVInVideoFine(void)
{
	PWORD pGAP = (PWORD)defVDCNVOffset[stVideo.stSRC_a.frmt];	// fix - demo is port A

	stVideo.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_a.offH = *pGAP++; stVideo.stSRC_a.offV = *pGAP++;
}

//--------------------------------------------------------------------------------------------------
static void SetADCNVInVideoFine(void)
{
	PWORD pGAP = (PWORD)defADCNVOffset[stVideo.stSRC_a.frmt];	// fix - demo is port A

	if (stVideo.stSRC_a.frmt==VIDSRC_1920x1080i60||stVideo.stSRC_a.frmt==VIDSRC_1920x1080i50)
		 stVideo.stSRC_a.fine = MDIN_FIELDID_BYPASS|MDIN_LOW_IS_TOPFLD;
	else stVideo.stSRC_a.fine = MDIN_FIELDID_BYPASS|MDIN_HIGH_IS_TOPFLD;

	stVideo.stSRC_a.offH = *pGAP++; stVideo.stSRC_a.offV = *pGAP++;
}

//--------------------------------------------------------------------------------------------------
static void SetHDMIRXInVideoFine(void)
{
//#if __BOARD_REV_ == 10				//by hungry 2012.02.15
#if __BOARD_REV_ >= 10					//by hungry 2012.02.15
	stVideo.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_a.offH = 0;		stVideo.stSRC_a.offV = 0;
#else
	PWORD pGAP = (PWORD)defDVIRXOffset[stVideo.stSRC_a.frmt];	// fix - demo is port A

	stVideo.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_BYPASS|MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_a.offH = *pGAP++; stVideo.stSRC_a.offV = *pGAP++;

	if (SrcMainFrmt==VIDSRC_1920x1080p60||SrcMainFrmt==VIDSRC_1920x1080p50)
		 MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY3);
	else MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY0);
#endif
}
*/	//by hungry 2012.03.07
//--------------------------------------------------------------------------------------------------
static void SetSDIRXInVideoFine(void)	   //by hungry 2012.02.15
{
 	stVideo.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
 	stVideo.stSRC_a.offH = 0;		stVideo.stSRC_a.offV = 0;
}
//--------------------------------------------------------------------------------------------------
static void SetSDI2RXInVideoFine(void)	   //by flcl 2013.04.23
{
 	stVideo.stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
 	stVideo.stSRC_b.offH = 0;		stVideo.stSRC_b.offV = 0;
}
//--------------------------------------------------------------------------------------------------
/*	   //by hungry 2012.03.07
static void SetEXTRXInVideoFine(void)
{
//	PWORD pGAP = (PWORD)defExt656RXOffset[stVideo.stSRC_a.frmt];

//	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT|MDIN_HIGH_IS_TOPFLD;
//	stVideo.stSRC_a.offH = *pGAP++; stVideo.stSRC_a.offV = *pGAP++;
}
*/
//--------------------------------------------------------------------------------------------------
static void SetSrcMainFine(BYTE src)
{
	switch (src) {
/*	//by hungry 2012.03.07
		case VIDEO_DIGITAL_HDMI:	SetHDMIRXInVideoFine(); break;
		case VIDEO_ADCNV_2HD_IN:	SetHDMIRXInVideoFine(); break;
*/		 
		case VIDEO_SDI_2HD_POP:		SetSDIRXInVideoFine(); break;
		case VIDEO_ADCNV_2HD_IN:	SetSDIRXInVideoFine(); break;
		case VIDEO_DIGITAL_SDI:		SetSDIRXInVideoFine(); break;		
		case VIDEO_DIGITAL_SDI2:	SetSDI2RXInVideoFine(); break;		
//		default:						SetEXTRXInVideoFine(); break;	//by hungry 2012.03.07
	}
}
/*
//--------------------------------------------------------------------------------------------------
static void SetOutVideoFine(void)
{
}
*/
//--------------------------------------------------------------------------------------------------
/*	//by hungry 2012.02.15
static void GetExtVideoAttb(BYTE src)
{
	if (src!=VIDEO_EXTERNAL_IN) return;

	DEMO_SetActiveRegion(GetMenuStatus(8,3));
	DEMO_SetCbCr422Swap(GetMenuStatus(8,4));
	DEMO_SetClockDelay(GetMenuStatus(8,5));
	DEMO_SetInFLDBypass(GetMenuStatus(8,6));
	DEMO_SetTopFLDLevel(GetMenuStatus(8,7));
	DEMO_SetHACTOffset(GetMenuStatus(8,8));
	DEMO_SetVACTOffset(GetMenuStatus(8,9));
}
*/
//--------------------------------------------------------------------------------------------------
static void SetIPCVideoFine(BYTE src)
{
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x256, 0, 8, 12);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x259, 0, 8,  4);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x25a, 8, 8,  1);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x25b, 0, 8,  8);
}

//--------------------------------------------------------------------------------------------------
static PMDIN_AUXFILT_COEF GetAUXFilterCoef(void)
{
	if (stVideo.stSRC_x.frmt!=VIDSRC_1920x1080i60&&stVideo.stSRC_x.frmt!=VIDSRC_1920x1080p60&&
		stVideo.stSRC_x.frmt!=VIDSRC_1920x1080i50&&stVideo.stSRC_x.frmt!=VIDSRC_1920x1080p50)
		return (PMDIN_AUXFILT_COEF)NULL;

	switch (stVideo.stOUT_x.frmt) {
		case VIDOUT_1280x1024p60:	return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[0];
		case VIDOUT_1024x768p60:	return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[1];
		//case VIDOUT_800x600p60:		return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[2];
		case VIDOUT_720x576i50:		return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[3];
		case VIDOUT_720x480i60:		return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[4];
		default:					return (PMDIN_AUXFILT_COEF)NULL;
	}
}

//--------------------------------------------------------------------------------------------------
//static void SetAUXVideoFilter(void)
//{
//	PMDIN_AUXFILT_COEF pCoef = GetAUXFilterCoef();

//	MDINAUX_EnableFrontNRFilter(&stVideo, (pCoef==NULL)? OFF : ON);
//	if (pCoef!=NULL) MDINAUX_SetFrontNRFilterCoef(pCoef);
//}

//--------------------------------------------------------------------------------------------------
/*static void Set2HDVideoPathB(void)
{
	if (stVideo.dacPATH!=DAC_PATH_AUX_2HD) return;
//	printf("Set2HDVideoPathB()!!!\n");
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 0, 2, 2);	// for 2HD mode ch1 display on 16Aug2012	// by chungsa
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x020, 12, 1, 1);
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x021, 12, 1, 1);
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x002, 15, 1, 1);
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x002, 7, 1, src);
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x020, 12, 1, src);
//	MDINDLY_mSec(10);	// delay 10ms
//	MDINHIF_RegField(MDIN_LOCAL_ID, 0x020, 12, 1, 0);

	
}*/
/*
//--------------------------------------------------------------------------------------------------
static void SetOutHDMI_DVI(void)
{
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x005, 0x03);	  //API ver0.26 2012.02.13
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x005, 0x00);	  //API ver0.26 2012.02.13
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x008, 0x37);	  //API ver0.26 2012.02.13
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x03e, 0x04);	  //API ver0.26 2012.02.13
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x049, 0x40);	  //API ver0.26 2012.02.13
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x12f, 0x20);	  //API ver0.26 2012.02.13
	MDINHIF_RegWrite(MDIN_HDMI_ID, 0x082, 0x20);	  //API ver0.26 2012.02.13
}
*/
//--------------------------------------------------------------------------------------------------
//static void SetSBoxAreaRefresh(void)
//{
//	PWORD pID = (PWORD)def4CHSBoxWND[SrcMainFrmt%2];
//	MDINOSD_SetSBoxArea(&stSBOX[0], pID[0], pID[1], pID[2], pID[3]);
//}

//--------------------------------------------------------------------------------------------------
/*static void SetOSDMenuDisable(void)
{
	// disable MAIN-OSD when OSD area is over video area
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], OFF);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX1], OFF);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX2], OFF);

	// disable AUX-OSD when OSD area is over video area
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX8], OFF);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX9], OFF);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEXA], OFF);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEXB], OFF);

	// disable SBOX-OSD when OSD area is over video area
	MDINOSD_EnableSBoxBorder(&stSBOX[0], OFF);

	// disable BGBOX-OSD when OSD area is over video area
	DEMO_EnableRectBGBOX(OFF);
}*/

//--------------------------------------------------------------------------------------------------
//static void SetOSDMenuRefresh(void)
void SetOSDMenuRefresh(void)
{
	BOOL h_rpt;//, OnOff = (stVideo.dacPATH==DAC_PATH_AUX_4CH)? ON : OFF;

	//OSD_SetFontMAP();	

	//SetOSDMenuID(GetOSDMenuID());	// refresh OSD-menu

	OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	//OSD_ModifyPalette_X((OutAuxMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);

	h_rpt = (OutAuxMode==MDIN_OUT_MUX656_8||OutAuxMode==MDIN_OUT_MUX656_10)? ON : OFF;
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], h_rpt, OFF);

	//if (OnOff==ON) SetSBoxAreaRefresh();				// refresh OSD-Sbox
	//MDINOSD_EnableSBoxBorder(&stSBOX[0], OnOff);
	//MDIN3xx_EnableAuxWithMainOSD(&stVideo, OnOff);

	//DEMO_EnableRectBGBOX((stVideo.dacPATH==DAC_PATH_MAIN_PIP)? ON : OFF);

//	MDINOSD_SetSpriteBMPInfo(&stOSD[SPRITE_INDEXB], st4CH.w_m, st4CH.h_m, st4CH.addr_m);
//	MDINOSD_SetSpritePosition(&stOSD[SPRITE_INDEXB], 876, 510);
	//MDINOSD_EnableSprite(&stOSD[SPRITE_INDEXB], OnOff);

//	OSD_SetDemo();
}

/*//--------------------------------------------------------------------------------------------------
void SetOSDMenuRefresh(void)
{
	BOOL h_rpt, OnOff = (stVideo.dacPATH==DAC_PATH_AUX_4CH)? ON : OFF;

	OSD_SetFontMAP();	SetOSDMenuID(GetOSDMenuID());	// refresh OSD-menu

	OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	OSD_ModifyPalette_X((OutAuxMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);

	h_rpt = (OutAuxMode==MDIN_OUT_MUX656_8||OutAuxMode==MDIN_OUT_MUX656_10)? ON : OFF;
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX2], h_rpt, OFF);

	if (OnOff==ON) SetSBoxAreaRefresh();				// refresh OSD-Sbox
	MDINOSD_EnableSBoxBorder(&stSBOX[0], OnOff);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo, OnOff);

	//DEMO_EnableRectBGBOX((stVideo.dacPATH==DAC_PATH_MAIN_PIP)? ON : OFF);

	MDINOSD_SetSpriteBMPInfo(&stOSD[SPRITE_INDEXB], st4CH.w_m, st4CH.h_m, st4CH.addr_m);
	MDINOSD_SetSpritePosition(&stOSD[SPRITE_INDEXB], 876, 510);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEXB], OnOff);

//	OSD_SetDemo();
}*/

//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess(BYTE src)
{
//	unsigned char current_split_mode_tmp = 0;

	if (fSyncParsed==FALSE) return;		// wait for sync detection

	if (EncVideoFrmt!=PrevEncFrmt)	PrevSrcMainFrmt = 0xff;
	stVideo.encFRMT = EncVideoFrmt;	PrevEncFrmt = EncVideoFrmt;

	if (SrcMainFrmt!=PrevSrcMainFrmt||SrcMainMode!=PrevSrcMainMode||
		OutMainFrmt!=PrevOutMainFrmt||OutMainMode!=PrevOutMainMode)
		stVideo.exeFLAG |= MDIN_UPDATE_MAINFMT;

	// Do not use aux display.. by kukuri
//	if (SrcAuxFrmt!=PrevSrcAuxFrmt||SrcAuxMode!=PrevSrcAuxMode||
//		OutAuxFrmt!=PrevOutAuxFrmt||OutAuxMode!=PrevOutAuxMode)
//		stVideo.exeFLAG |= MDIN_UPDATE_AUXFMT;

	if (stVideo.exeFLAG!=0)
	{//return;		// not change video formats

		stVideo.stIPC_m.fine &= ~MDIN_DEINT_3DNR_ON;   //3DNR off
//		SetMenuStatus(2,1,0);	//3DNR [off]

		if (stVideo.srcPATH == PATH_MAIN_B_AUX_B || stVideo.srcPATH == PATH_MAIN_B_AUX_A || stVideo.srcPATH == PATH_MAIN_B_AUX_M)
		{
			stVideo.stSRC_b.frmt = SrcMainFrmt; stVideo.stSRC_b.mode = SrcMainMode;
			stVideo.stSRC_a.frmt = SrcAuxFrmt; stVideo.stSRC_a.mode = SrcAuxMode;
		}
		else
		{
			stVideo.stSRC_a.frmt = SrcMainFrmt; stVideo.stSRC_a.mode = SrcMainMode;
			stVideo.stSRC_b.frmt = SrcAuxFrmt; stVideo.stSRC_b.mode = SrcAuxMode;
		}
		stVideo.stOUT_m.frmt = OutMainFrmt; stVideo.stOUT_m.mode = OutMainMode;
		stVideo.stOUT_x.frmt = OutAuxFrmt; stVideo.stOUT_x.mode = OutAuxMode;

		MDIN3xx_EnableAuxDisplay(&stVideo, OFF);
		MDIN3xx_EnableMainDisplay(OFF);

		//SetOSDMenuDisable();		// set OSD menu disable
		SetOffChipFrmtInA(src);		// set InA offchip format
		SetOffChipFrmtInB(src);		// set InB offchip format	//by hungry 2012.03.07
		SetSrcMainFine(src);		// set source video fine (fldid, offset)
	//	SetOutVideoFine();			// set output video fine (swap)

		//DEMO_SetOutputMode(GetMenuStatus(6,3));		// update out port mode, add on 16Aug2012

		if (OutMainFrmt!=PrevOutMainFrmt) {
	//		memset(&stVideo.stVIEW_m, 0, 8);	// restore aspect from API
			stVideo.pHY_m		= 	NULL;		// restore MFCHY from API
			stVideo.pHC_m		= 	NULL;		// restore MFCHY from API
			stVideo.pVY_m		= 	NULL;		// restore MFCHY from API
			stVideo.pVC_m		= 	NULL;		// restore MFCHY from API
		}
		MDIN3xx_VideoProcess(&stVideo);		// mdin3xx main video process

	//	GetExtVideoAttb(src);	// update E-Video attribute (edge,swap,clk,offset)	//by hungry 2012.02.15
		SetIPCVideoFine(src);	// tune IPC-register (CVBS or HDMI)
//		SetAUXVideoFilter();	// tune AUX-filter (DUAL or CVBS) // blocked by kukuri
		//Set2HDVideoPathB();		// set 2HD pathB

	//	SetMenuStatus(4,6,MBIT(stVideo.stOUT_m.stATTB.attb,MDIN_WIDE_RATIO)); //by kukuri
	//	DEMO_SetWindowPIPPOP(GetMenuStatus(4,3));	// update pip/pop window	//by kukuri
		//DEMO_SetAspectRatio(GetMenuStatus(4,6));	// update aspect ratio
		//DEMO_SetOverScanning(GetMenuStatus(4,5));	// update overscanning
		//DEMO_SetImageMirrorV(GetMenuStatus(6,7));	// update v-mirror

		MDIN3xx_EnableAuxDisplay(&stVideo, OFF);

	#if 0 //Louis
		if(SDIRX_change_flag)
		{
			SDIRX_change_flag = 0;
		}
	#endif

		MDIN3xx_EnableMainDisplay(ON);
		//if(sys_status.current_split_mode != FULL_9) MDIN3xx_EnableMainDisplay(ON);

		// if src is 2HD input or dual display, trigger soft-reset.
		if (src==VIDEO_ADCNV_2HD_IN)//||GetMenuStatus(4,4)) 
			MDIN3xx_SoftReset();

		// Do we need below line? by kukuri
//		SetOSDMenuRefresh();	// for framebuffer map bug
	//	SetOutHDMI_DVI();

		PrevSrcMainFrmt = SrcMainFrmt;	PrevSrcMainMode = SrcMainMode;
		PrevOutMainFrmt = OutMainFrmt;	PrevOutMainMode = OutMainMode;
		// Do not use aux display.. by kukuri
//		PrevSrcAuxFrmt = SrcAuxFrmt;	PrevSrcAuxMode = SrcAuxMode;
//		PrevOutAuxFrmt = OutAuxFrmt;	PrevOutAuxMode = OutAuxMode;

		//test_stVideo_print();
	#if 0 //Louis
		if(SDIRX_change_flag)
		{
			SDIRX_change_flag = 0;
		}
	#endif
	}
}

//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
	InputSourceHandler(InputSelect);
	InputSyncHandler_A(InputSelect);
	InputSyncHandler_B(InputSelect);		  //by hungry 2012.02.27
	VideoFrameProcess(InputSelect);
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	MDINHTX_CtrlHandler(&stVideo);
}
#endif	/* defined(SYSTEM_USE_MDIN380) */

/*  FILE_END_HERE */
