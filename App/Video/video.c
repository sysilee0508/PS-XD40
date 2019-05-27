// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#include "NVP6158.h"        

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

#define PIP_WINDOW_WIDTH_1280				DISPLAY_WIDTH_1280x720/3
#define PIP_WINDOW_WIDTH_1920				DISPLAY_WIDTH_1920X1080/3
#define PIP_WINDOW_HEIGHT_720				DISPLAY_HEIGHT_1280x720/3
#define PIP_WINDOW_HEIGHT_1080				DISPLAY_HEIGHT_1920x1080/3
#define PIP_POSITION_MARGIN_720				70
#define PIP_POSITION_MARGIN_1080				100

#define COMPENSATION_MARGIN					0

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
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

MDIN_VIDEO_INFO	 stVideo_A, stVideo_B, stVideo_C;
MDIN_INTER_WINDOW	stInterWND;

MDIN_VIDEO_INPUT_t InputSelect, InputSelOld;
MDIN_SRCVIDEO_PATH_t SrcPath[MDIN_ID_MAX];
MDIN_SRCVIDEO_FORMAT_t SrcMainFrmt[MDIN_ID_MAX], PrevSrcMainFrmt[MDIN_ID_MAX];//, SrcMainMode, PrevSrcMainMode;
MDIN_OUTVIDEO_FORMAT_t OutMainFrmt[MDIN_ID_MAX], PrevOutMainFrmt[MDIN_ID_MAX];// OutMainMode, PrevOutMainMode;
MDIN_SRCVIDEO_FORMAT_t SrcAuxFrmt[MDIN_ID_MAX], PrevSrcAuxFrmt[MDIN_ID_MAX];//, SrcAuxMode, PrevSrcAuxMode;
//BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
MDIN_VENC_FORMAT_t EncVideoFrmt, PrevEncFrmt;//, AdcVideoFrmt, PrevAdcFrmt, ;
BOOL fInputChanged;

MDIN_VIDEO_WINDOW stMainVIEW[MDIN_ID_MAX], stAuxVIEW[MDIN_ID_MAX];
MDIN_VIDEO_WINDOW stMainCROP[MDIN_ID_MAX], stAuxCROP[MDIN_ID_MAX];

const static PMDIN_VIDEO_INFO pVideoInfo[MDIN_ID_MAX] = {&stVideo_A, &stVideo_B, &stVideo_C};

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------
static void ConfigI2C(MDIN_CHIP_ID_t mdin)
{
	switch(mdin)
	{
		case MDIN_ID_A:
			I2C_SET_CHANNEL(I2C_MAIN);
			SELECT_MDIN(MDIN_A);
			break;
			
		case MDIN_ID_B:
			I2C_SET_CHANNEL(I2C_MAIN);
			SELECT_MDIN(MDIN_B);
			break;
			
		case MDIN_ID_C:
			I2C_SET_CHANNEL(I2C_SUB);
			SELECT_MDIN(MDIN_A);
			break;
			
		default:
			break;
	}
}

static MDIN_OUTVIDEO_FORMAT_t GetOutVideoFrameRate(void)
{
	BYTE inputFormat = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	static MDIN_OUTVIDEO_FORMAT_t output = VIDOUT_1920x1080p60;

	if(GetInputVideoFormat(CHANNEL1) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInputVideoFormat(CHANNEL1);
	}
	else if(GetInputVideoFormat(CHANNEL1) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInputVideoFormat(CHANNEL2);
	}
	else
	{
		inputFormat = CVI_HD_30P;
	}

	switch(inputFormat)
	{
		case AHD20_SD_H960_NT:
		case AHD20_SD_H960_EX_NT:
		case AHD20_SD_H960_2EX_NT:
		case AHD20_SD_H960_2EX_Btype_NT:
		case AHD20_1080P_60P:
		case AHD20_1080P_30P:
		case AHD20_720P_60P:
		case AHD20_720P_30P:
		case AHD20_720P_30P_EX:
		case AHD20_720P_30P_EX_Btype:
		case TVI_FHD_30P:
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
		case TVI_HD_B_30P:
		case TVI_HD_B_30P_EX:
		case CVI_FHD_30P:
		case CVI_HD_60P:
		case CVI_HD_30P:
		case CVI_HD_30P_EX:
			output = VIDOUT_1920x1080p60;
			break;

		case AHD20_SD_H960_PAL:
		case AHD20_SD_H960_EX_PAL:
		case AHD20_SD_H960_2EX_PAL:
		case AHD20_SD_H960_2EX_Btype_PAL:
		case AHD20_1080P_50P:
		case AHD20_1080P_25P:
		case AHD20_720P_50P:
		case AHD20_720P_25P:
		case AHD20_720P_25P_EX:
		case AHD20_720P_25P_EX_Btype:
		case TVI_FHD_25P:
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
		case TVI_HD_B_25P:
		case TVI_HD_B_25P_EX:
		case CVI_FHD_25P:
		case CVI_HD_50P:
		case CVI_HD_25P:
		case CVI_HD_25P_EX:
			output = VIDOUT_1920x1080p50;;
			break;
	}
	return output;
}

static MDIN_SRCVIDEO_FORMAT_t GetInSourceFormat(eChannel_t channel)
{
	static MDIN_SRCVIDEO_FORMAT_t format[NUM_OF_CHANNEL] = {VIDSRC_1280x720p60, VIDSRC_1280x720p60};

	switch(GetInputVideoFormat(channel))
	{
		case AHD20_SD_H960_NT:
		case AHD20_SD_H960_EX_NT:
		case AHD20_SD_H960_2EX_NT:
		case AHD20_SD_H960_2EX_Btype_NT:
			format[channel] = VIDSRC_960x480i60;	//720x480p 60hz
			break;

		case AHD20_SD_H960_PAL:
		case AHD20_SD_H960_EX_PAL:
		case AHD20_SD_H960_2EX_PAL:
		case AHD20_SD_H960_2EX_Btype_PAL:
			format[channel] = VIDSRC_960x576i50;	//720x576p 50hz
			break;
		case AHD20_1080P_60P:
		case AHD20_1080P_30P:
		case TVI_FHD_30P:
		case CVI_FHD_30P:
			format[channel] = VIDSRC_1920x1080p60;	//1080p60
			break;

		case AHD20_1080P_50P:
		case AHD20_1080P_25P:
		case TVI_FHD_25P:
		case CVI_FHD_25P:
			format[channel] = VIDSRC_1920x1080p50;	//1080p50
			break;

		case AHD20_720P_60P:
		case AHD20_720P_30P:
		case AHD20_720P_30P_EX:
		case AHD20_720P_30P_EX_Btype:
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
		case TVI_HD_B_30P:
		case TVI_HD_B_30P_EX:
		case CVI_HD_60P:
		case CVI_HD_30P:
		case CVI_HD_30P_EX:
			format[channel] = VIDSRC_1280x720p60;	//720p60
			break;

		case AHD20_720P_50P:
		case AHD20_720P_25P:
		case AHD20_720P_25P_EX:
		case AHD20_720P_25P_EX_Btype:
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
		case TVI_HD_B_25P:
		case TVI_HD_B_25P_EX:
		case CVI_HD_50P:
		case CVI_HD_25P:
		case CVI_HD_25P_EX:
			format[channel] = VIDSRC_1280x720p50;	//720p50
			break;

		default:
			//format[channel] = VIDSRC_1280x720p60;
			break;
	}

	return format[channel];
}

static void MDIN3xx_SetRegInitial_AB(MDIN_CHIP_ID_t mdin)
{
	WORD nID = 0;
	PMDIN_VIDEO_INFO pVideo;

	if((mdin != MDIN_ID_A) && (mdin != MDIN_ID_B))
	{
		while(1);
	}
	else
	{
		pVideo = pVideoInfo[mdin];
	}

	ConfigI2C(mdin);
	
	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id

	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP0);		// set in_data_map_mode
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP5);		// disable digital out
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

	MDIN3xx_SetMFCHYFilterCoef(pVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(pVideo, NULL);
	MDIN3xx_SetMFCVYFilterCoef(pVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(pVideo, NULL);

	// set aux display ON
	pVideo->dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	pVideo->srcPATH = PATH_MAIN_A_AUX_B;	// set main is A, aux is main out
	pVideo->dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	pVideo->encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	pVideo->stSRC_a.frmt = VIDSRC_1280x720p60;
	pVideo->stSRC_a.mode = MDIN_SRC_MUX656_8;
	//stVideo_A.stSRC_a.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	pVideo->stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	pVideo->stSRC_a.offH = 0;
	pVideo->stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	pVideo->stSRC_b.frmt = VIDSRC_1280x720p60;
	pVideo->stSRC_b.mode = MDIN_SRC_MUX656_8;
	//stVideo_A.stSRC_b.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	pVideo->stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
	pVideo->stSRC_b.offH = 0;
	pVideo->stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	pVideo->stOUT_m.frmt = VIDOUT_1280x720p60;
	pVideo->stOUT_m.mode = MDIN_OUT_MUX656_8;	 //by hungry 2012.03.06		// test by chungsa
	pVideo->stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	pVideo->stOUT_m.brightness = 128;			// set main picture factor
	pVideo->stOUT_m.contrast = 128;
	pVideo->stOUT_m.saturation = 128;
	pVideo->stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	pVideo->stOUT_m.r_gain = 128;				// set main gain/offset
	pVideo->stOUT_m.g_gain = 128;
	pVideo->stOUT_m.b_gain = 128;
	pVideo->stOUT_m.r_offset = 128;
	pVideo->stOUT_m.g_offset = 128;
	pVideo->stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	pVideo->stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	pVideo->stIPC_m.film = MDIN_DEINT_FILM_OFF;
	pVideo->stIPC_m.gain = 34;
	pVideo->stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	pVideo->stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_ON
	
	// define video format of AUX-INPUT
	pVideo->stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24
	// define video format of AUX-OUTPUT (CVBS output)
	pVideo->stOUT_x.frmt = VIDOUT_720x480i60;
	pVideo->stOUT_x.mode = MDIN_OUT_MUX656_8;
	pVideo->stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run
	pVideo->stOUT_x.brightness = 128;			// set aux picture factor
	pVideo->stOUT_x.contrast = 128;
	pVideo->stOUT_x.saturation = 128;
	pVideo->stOUT_x.hue = 128;
#if RGB_GAIN_OFFSET_TUNE == 1
	pVideo->stOUT_x.r_gain = 128;				// set aux gain/offset
	pVideo->stOUT_x.g_gain = 128;
	pVideo->stOUT_x.b_gain = 128;
	pVideo->stOUT_x.r_offset = 128;
	pVideo->stOUT_x.g_offset = 128;
	pVideo->stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	pVideo->encFRMT = VID_VENC_NTSC_M;	//VID_VENC_AUTO

#if 0	// temporary blocked by kukuri
	// define video format of HDMI-OUTPUT
	pVideo->stVID_h.mode  = HDMI_OUT_RGB444_8;
	pVideo->stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	// audio block
	pVideo->stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	pVideo->stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	pVideo->stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	MDINHTX_SetHDMIBlock(pVideo);		// initialize HDMI block
#endif
	pVideo->exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(pVideo);			// mdin3xx main video process

	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
	PrevSrcMainFrmt[mdin] = 0xff;
	SrcMainFrmt[mdin] = pVideo->stSRC_a.frmt;
	PrevOutMainFrmt[mdin] = 0xff;
	OutMainFrmt[mdin] = pVideo->stOUT_m.frmt;
	PrevEncFrmt = 0xff;
	EncVideoFrmt = pVideo->encFRMT;
}

static void MDIN3xx_SetRegInitial_C(void)
{
	WORD nID = 0;

	ConfigI2C(MDIN_ID_C);
	
	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id

	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP0);		// set in_data_map_mode
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

//	memset((PBYTE)&stVideo_C, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo_C, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo_C, NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo_C, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo_C, NULL);

	// set aux display ON
	stVideo_C.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo_C.srcPATH = PATH_MAIN_A_AUX_B;	// set main is A, aux is main out
	stVideo_C.dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo_C.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo_C.stSRC_a.frmt = VIDSRC_1280x720p60;
	stVideo_C.stSRC_a.mode = MDIN_SRC_MUX656_8;
	//stVideo_C.stSRC_a.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo_C.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	stVideo_C.stSRC_a.offH = 0;
	stVideo_C.stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo_C.stSRC_b.frmt = VIDSRC_1280x720p60;
	stVideo_C.stSRC_b.mode = MDIN_SRC_MUX656_8;
	//stVideo_C.stSRC_b.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo_C.stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
	stVideo_C.stSRC_b.offH = 0;
	stVideo_C.stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo_C.stOUT_m.frmt = VIDOUT_1920x1080p60;
	stVideo_C.stOUT_m.mode = MDIN_OUT_RGB444_8;	 //by hungry 2012.03.06		// test by chungsa
	stVideo_C.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo_C.stOUT_m.brightness = 128;			// set main picture factor
	stVideo_C.stOUT_m.contrast = 128;
	stVideo_C.stOUT_m.saturation = 128;
	stVideo_C.stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo_C.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo_C.stOUT_m.g_gain = 128;
	stVideo_C.stOUT_m.b_gain = 128;
	stVideo_C.stOUT_m.r_offset = 128;
	stVideo_C.stOUT_m.g_offset = 128;
	stVideo_C.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo_C.stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo_C.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo_C.stIPC_m.gain = 34;
	stVideo_C.stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	stVideo_C.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_ON
	
	// define video format of AUX-INPUT
	stVideo_C.stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24
	// define video format of AUX-OUTPUT (CVBS output)
	stVideo_C.stOUT_x.frmt = VIDOUT_720x480i60;
	stVideo_C.stOUT_x.mode = MDIN_OUT_MUX656_8;
	stVideo_C.stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run
	stVideo_C.stOUT_x.brightness = 128;			// set aux picture factor
	stVideo_C.stOUT_x.contrast = 128;
	stVideo_C.stOUT_x.saturation = 128;
	stVideo_C.stOUT_x.hue = 128;
#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo_C.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo_C.stOUT_x.g_gain = 128;
	stVideo_C.stOUT_x.b_gain = 128;
	stVideo_C.stOUT_x.r_offset = 128;
	stVideo_C.stOUT_x.g_offset = 128;
	stVideo_C.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo_C.encFRMT = VID_VENC_NTSC_M;	//VID_VENC_AUTO
	// define video format of HDMI-OUTPUT
	stVideo_C.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo_C.stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	// audio block
	stVideo_C.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo_C.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo_C.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	MDINHTX_SetHDMIBlock(&stVideo_C);		// initialize HDMI block

	stVideo_C.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo_C);			// mdin3xx main video process

	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application

	PrevSrcMainFrmt[MDIN_ID_C]= 0xff;
	SrcMainFrmt[MDIN_ID_C] = stVideo_C.stSRC_a.frmt;
	PrevOutMainFrmt[MDIN_ID_C] = 0xff;
	OutMainFrmt[MDIN_ID_C] = stVideo_C.stOUT_m.frmt;
	PrevEncFrmt = 0xff;
	EncVideoFrmt = stVideo_C.encFRMT;
}

//--------------------------------------------------------------------------------------------------
// Set Input Video Path
//--------------------------------------------------------------------------------------------------
static void SetInVideoPath(MDIN_VIDEO_INPUT_t src)
{
	if(src != InputSelOld)
	{
		switch (src)
		{
			case VIDEO_DIGITAL_NVP6158_A:
				stVideo_A.srcPATH = PATH_MAIN_A_AUX_M;
				stVideo_A.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_A.encPATH = VENC_PATH_PORT_X;

				stVideo_C.srcPATH = PATH_MAIN_A_AUX_M;
				stVideo_C.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_C.encPATH = VENC_PATH_PORT_X;
				break;
				
			case VIDEO_DIGITAL_NVP6158_AB:
				stVideo_A.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_A.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_A.encPATH = VENC_PATH_PORT_B;
				
				stVideo_C.srcPATH = PATH_MAIN_A_AUX_M;
				stVideo_C.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_C.encPATH = VENC_PATH_PORT_X;
				break;

			case VIDEO_DIGITAL_NVP6158_C:
				stVideo_B.srcPATH = PATH_MAIN_A_AUX_M;
				stVideo_B.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_B.encPATH = VENC_PATH_PORT_X;

				stVideo_C.srcPATH = PATH_MAIN_B_AUX_M;
				stVideo_C.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_C.encPATH = VENC_PATH_PORT_X;
				break;
				
			case VIDEO_DIGITAL_NVP6158_CD:
				stVideo_B.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_B.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_B.encPATH = VENC_PATH_PORT_B;
				
				stVideo_C.srcPATH = PATH_MAIN_B_AUX_M;
				stVideo_C.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_C.encPATH = VENC_PATH_PORT_X;
				break;

			case VIDEO_DIGITAL_NVP6158_AC:
				stVideo_A.srcPATH = PATH_MAIN_A_AUX_M;
				stVideo_A.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_A.encPATH = VENC_PATH_PORT_X;
				
				stVideo_B.srcPATH = PATH_MAIN_A_AUX_M;
				stVideo_B.dacPATH = DAC_PATH_MAIN_OUT;
				stVideo_B.encPATH = VENC_PATH_PORT_X;

				stVideo_C.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_C.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_C.encPATH = VENC_PATH_PORT_B;
				break;
				
			case VIDEO_DIGITAL_NVP6158_ABCD:
				stVideo_A.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_A.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_A.encPATH = VENC_PATH_PORT_B;
				
				stVideo_B.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_B.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_B.encPATH = VENC_PATH_PORT_B;

				stVideo_C.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_C.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_C.encPATH = VENC_PATH_PORT_B;
				break;

			case VIDEO_DIGITAL_NVP6158_ABCD2:
				stVideo_A.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_A.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_A.encPATH = VENC_PATH_PORT_B;
				
				stVideo_B.srcPATH = PATH_MAIN_A_AUX_B;
				stVideo_B.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_B.encPATH = VENC_PATH_PORT_B;

				stVideo_C.srcPATH = PATH_MAIN_B_AUX_A;
				stVideo_C.dacPATH = DAC_PATH_MAIN_PIP;
				stVideo_C.encPATH = VENC_PATH_PORT_B;
				break;
				
		}
	}
}

//--------------------------------------------------------------------------------------------------
// Config main&aux source, main out video format for all MDINs
static void ConfigVideoFrmt(BYTE src)
{
	eVPORT_MAP_t vport_map = NVP6158_Get_VportMap();
	
	switch(src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			if(vport_map == VPORT_MAP0)
			{
				SrcMainFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
			}
			else if(vport_map == VPORT_MAP1)
			{
				SrcMainFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL2);//stVideo.stSRC_a.frmt;
			}

			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}
			break;

		case VIDEO_DIGITAL_NVP6158_AB:
			SrcMainFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
			SrcAuxFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL2);

			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}
			break;

		case VIDEO_DIGITAL_NVP6158_C:
			// main source & out
			if(vport_map == VPORT_MAP0)
			{
				SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL3);//stVideo.stSRC_a.frmt;
			}
			else// if(vport_map == VPORT_MAP1)
			{
				SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL4);//stVideo.stSRC_a.frmt;
			}

			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}
			break;

		case VIDEO_DIGITAL_NVP6158_CD:
			SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL3);//stVideo.stSRC_a.frmt;
			SrcAuxFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL4);

			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}
			break;

		case VIDEO_DIGITAL_NVP6158_AC:
			SrcMainFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL1);
			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}

			if(vport_map == VPORT_MAP0)
			{
				SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL3);//stVideo.stSRC_a.frmt;
			}
			else// if(vport_map == VPORT_MAP1)
			{
				SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL4);//stVideo.stSRC_a.frmt;
			}
			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p60;
				SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p50;
				SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
			}
			break;

		case VIDEO_DIGITAL_NVP6158_ABCD:
			SrcMainFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL1);
			SrcAuxFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL2);
			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}

			SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL3);//stVideo.stSRC_a.frmt;
			SrcAuxFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL4);
			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p60;
				SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p50;
				SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
			}
			break;
			
		case VIDEO_DIGITAL_NVP6158_ABCD2:
			SrcMainFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL1);
			SrcAuxFrmt[MDIN_ID_A] = GetInSourceFormat(CHANNEL2);
			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
				SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
				SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
			}

			SrcMainFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL3);//stVideo.stSRC_a.frmt;
			SrcAuxFrmt[MDIN_ID_B] = GetInSourceFormat(CHANNEL4);
			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
			}
			else
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
			}
			break;
		default:
			//currentMainFrmt = VIDSRC_1280x720p60;
			break;
	}

}

#if 0
//--------------------------------------------------------------------------------------------------
static BYTE GetSrcAuxMode(BYTE src)
{
	BYTE mode;

	switch (src) 
	{
		case VIDEO_DIGITAL_NVP6158_A:
			mode = OutMainMode;
			break;
		case VIDEO_DIGITAL_NVP6158_AB:
		default:
			mode = MDIN_SRC_MUX656_8;
			break;
	}

	return mode;
}

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcAuxFrmt(BYTE src)
{
	BYTE frmt;

	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			if((OutMainFrmt == VIDOUT_1920x1080p60) || (OutMainFrmt == VIDOUT_1920x1080p30))
			{
				frmt = VIDSRC_1920x1080p60;
			}
			else
			{
				frmt = VIDSRC_1920x1080p50;
			}
			break;
		case VIDEO_DIGITAL_NVP6158_AB:
			frmt = GetInSourceFormat(CHANNEL2);
			break;
	}

	return frmt;
}
#endif

#if 0
static BYTE GetOutAuxMode(BYTE src)
{
	BYTE mode;

	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			mode = MDIN_OUT_MUX656_8;
			break;
		case VIDEO_DIGITAL_NVP6158_AB:
			mode = MDIN_OUT_RGB444_8;
			break;
	}
	return mode;
}
#endif
//--------------------------------------------------------------------------------------------------
static void InputSourceHandler(MDIN_VIDEO_INPUT_t src)
{
	SetInVideoPath(src);
	ConfigVideoFrmt(src);

	if((src == VIDEO_DIGITAL_NVP6158_A) || (src == VIDEO_DIGITAL_NVP6158_AB) || 
		(src == VIDEO_DIGITAL_NVP6158_AC) || (src == VIDEO_DIGITAL_NVP6158_ABCD) || 
		(src == VIDEO_DIGITAL_NVP6158_ABCD2))
	{
		CreateDisplayWindow_A(GetCurrentDisplayMode());
	}
	if((src == VIDEO_DIGITAL_NVP6158_C) || (src == VIDEO_DIGITAL_NVP6158_CD) ||
		(src == VIDEO_DIGITAL_NVP6158_AC) || (src == VIDEO_DIGITAL_NVP6158_ABCD) || 
		(src == VIDEO_DIGITAL_NVP6158_ABCD2))
	{
		CreateDisplayWindow_B(GetCurrentDisplayMode());
	}
	CreateDisplayWindow_C(GetCurrentDisplayMode());
	
	InputSelOld = src;
}

//--------------------------------------------------------------------------------------------------
static void SetIPCVideoFine(void)
{
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x256, 0, 8, 12);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x259, 0, 8,  4);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x25a, 8, 8,  1);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x25b, 0, 8,  8);
}


//--------------------------------------------------------------------------------------------------
static void SetOSDMenuRefresh(void)
{
	ConfigI2C(MDIN_ID_C);
	//OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	OSD_ModifyPalette_M(OSD_RGB_PALETTE);
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], OFF, OFF);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo_C, ON);
}

//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess(MDIN_CHIP_ID_t mdin_id)
{
	PMDIN_VIDEO_INFO pVideo = pVideoInfo[mdin_id];

	if (EncVideoFrmt!=PrevEncFrmt)
	{
		PrevSrcMainFrmt[mdin_id] = 0xff;
	}
	pVideo->encFRMT = EncVideoFrmt;
	PrevEncFrmt = EncVideoFrmt;

	if((SrcMainFrmt[mdin_id] != PrevSrcMainFrmt[mdin_id])|| (OutMainFrmt[mdin_id] !=PrevOutMainFrmt[mdin_id]) ||
		(SrcPath[mdin_id] != pVideo->srcPATH))
	{
		pVideo->exeFLAG |= MDIN_UPDATE_MAINFMT;
	}

	if (SrcAuxFrmt[mdin_id] !=PrevSrcAuxFrmt[mdin_id])
	{
		pVideo->exeFLAG |= MDIN_UPDATE_AUXFMT;
	}

	if (pVideo->exeFLAG!=MDIN_UPDATE_CLEAR) // updated video formats
	{
		ConfigI2C(mdin_id);
	
		pVideo->stIPC_m.fine &= ~MDIN_DEINT_3DNR_ON;   //3DNR off

		if (pVideo->srcPATH == PATH_MAIN_B_AUX_B || pVideo->srcPATH == PATH_MAIN_B_AUX_A || pVideo->srcPATH == PATH_MAIN_B_AUX_M)
		{
			pVideo->stSRC_b.frmt = SrcMainFrmt[mdin_id];
			pVideo->stSRC_a.frmt = SrcAuxFrmt[mdin_id];
		}
		else
		{
			pVideo->stSRC_a.frmt = SrcMainFrmt[mdin_id];
			pVideo->stSRC_b.frmt = SrcAuxFrmt[mdin_id];
		}
		pVideo->stOUT_m.frmt = OutMainFrmt[mdin_id];

		MDIN3xx_EnableAuxDisplay(pVideo, OFF);
		MDIN3xx_EnableMainDisplay(OFF);

		if (OutMainFrmt[mdin_id] !=PrevOutMainFrmt[mdin_id]) 
		{
			pVideo->pHY_m		= 	NULL;		// restore MFCHY from API
			pVideo->pHC_m		= 	NULL;		// restore MFCHY from API
			pVideo->pVY_m		= 	NULL;		// restore MFCHY from API
			pVideo->pVC_m		= 	NULL;		// restore MFCHY from API
		}
		MDIN3xx_VideoProcess(pVideo);		// mdin3xx main video process
		
		SetIPCVideoFine();	// tune IPC-register (CVBS or HDMI)

		MDIN3xx_EnableAuxDisplay(pVideo, ON);
		MDIN3xx_EnableMainDisplay(ON);

		PrevSrcMainFrmt[mdin_id] = SrcMainFrmt[mdin_id];
		PrevOutMainFrmt[mdin_id] = OutMainFrmt[mdin_id];

		PrevSrcAuxFrmt[mdin_id] = SrcAuxFrmt[mdin_id];
		SrcPath[mdin_id] = pVideo->srcPATH;
	}
}


// ----------------------------------------------------------------------
// Exported functions
// ----------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void CreateVideoInstance(void)
{
	memset((PBYTE)&stVideo_A, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_AB(MDIN_ID_A);	// initialize MDIN-3xx
	memset((PBYTE)&stVideo_B, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_AB(MDIN_ID_B);	// initialize MDIN-3xx
	memset((PBYTE)&stVideo_C, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_C();
	
	InputSelect = VIDEO_DIGITAL_NVP6158_A;
}

void InitInputSource(void)
{
	InputSelOld = 0xFF;
	InputSelect = 0xFF;
}

//--------------------------------------------------------------------------------------------------
void SetInputSource(BYTE input)
{
	InputSelect = input;
	if(InputSelect != InputSelOld)
	{
		fInputChanged = TRUE;
	}
}

void SetInputChanged(void)
{
	fInputChanged = TRUE;
}
//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
	//eDisplayMode_t displayMode = GetCurrentDisplayMode();

	if(fInputChanged == TRUE)
	{
		InputSourceHandler(InputSelect);

		if((InputSelect == VIDEO_DIGITAL_NVP6158_A) || (InputSelect == VIDEO_DIGITAL_NVP6158_AB) || 
			(InputSelect == VIDEO_DIGITAL_NVP6158_AC) || (InputSelect == VIDEO_DIGITAL_NVP6158_ABCD) ||
			(InputSelect == VIDEO_DIGITAL_NVP6158_ABCD2))
		{
			VideoFrameProcess(MDIN_ID_A);
		}
		if((InputSelect == VIDEO_DIGITAL_NVP6158_C) || (InputSelect == VIDEO_DIGITAL_NVP6158_CD) ||
			(InputSelect == VIDEO_DIGITAL_NVP6158_AC) || (InputSelect == VIDEO_DIGITAL_NVP6158_ABCD) ||
			(InputSelect == VIDEO_DIGITAL_NVP6158_ABCD2))
		{
			VideoFrameProcess(MDIN_ID_B);
		}

		VideoFrameProcess(MDIN_ID_C);
	}
	SetOSDMenuRefresh();
	fInputChanged = FALSE;
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	ConfigI2C(MDIN_ID_C);
	MDINHTX_CtrlHandler(&stVideo_C);
}


//static void ConfigWindowSize(eDisplayMode_t displayMode, MDIN_CHIP_ID_t mdin)

void CreateDisplayWindow_A(eDisplayMode_t displayMode)
{
	WORD mainWidth, mainHeight;
	WORD auxWidth, auxHeight;

	PMDIN_VIDEO_WINDOW pMainView = &stMainVIEW[MDIN_ID_A];
	PMDIN_VIDEO_WINDOW pAuxView = &stAuxVIEW[MDIN_ID_A];
	PMDIN_VIDEO_WINDOW pMainCrop = &stMainCROP[MDIN_ID_A];
	PMDIN_VIDEO_WINDOW pAuxCrop = &stAuxCROP[MDIN_ID_A];
	
	// initialize each object
	memset(pMainView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pMainCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));

	switch(SrcMainFrmt[MDIN_ID_A])
	{
		case VIDSRC_1920x1080p60:
		case VIDSRC_1920x1080p50:
			mainWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			mainHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			mainWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			mainHeight = DISPLAY_HEIGHT_1280x720;
			break;

		case VIDSRC_960x480i60:
			mainWidth = DISPLAY_WIDTH_960*2;
			mainHeight = DISPLAY_HEIGHT_480/2;
			break;

		case VIDSRC_960x576i50:
			mainWidth = DISPLAY_WIDTH_960*2;
			mainHeight = DISPLAY_HEIGHT_576/2;
			break;
	}

	switch(SrcAuxFrmt[MDIN_ID_A])
	{
		case VIDSRC_1920x1080p60:
		case VIDSRC_1920x1080p50:
			auxWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			auxHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			auxWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			auxHeight = DISPLAY_HEIGHT_1280x720;
			break;
			
		case VIDSRC_960x480i60:
			auxWidth = DISPLAY_WIDTH_960*2;
			auxHeight = DISPLAY_HEIGHT_480/2;
			break;

		case VIDSRC_960x576i50:
			auxWidth = DISPLAY_WIDTH_960*2;
			auxHeight = DISPLAY_HEIGHT_576/2;
			break;

	}

	//Check video loss channel
	if((GetInputVideoFormat(CHANNEL1) == NC_VIVO_CH_FORMATDEF_UNKNOWN) && (displayMode != DISPLAY_MODE_FULL_CH2))
	{
		// channel 1 is always main
		mainWidth = DISPLAY_WIDTH_1280x720/2;
		mainHeight = DISPLAY_HEIGHT_1280x720;
	}

	if((GetInputVideoFormat(CHANNEL2) == NC_VIVO_CH_FORMATDEF_UNKNOWN) && (displayMode != DISPLAY_MODE_FULL_CH1))
	{
		// channel2 is main or aux?
		if(displayMode == DISPLAY_MODE_FULL_CH2)
		{
			mainWidth = DISPLAY_WIDTH_1280x720/2;
			mainHeight = DISPLAY_HEIGHT_1280x720;
		}
		else
		{
			auxWidth = DISPLAY_WIDTH_1280x720/2;
			auxHeight = DISPLAY_HEIGHT_480/2;
		}
	}

	pMainCrop->w = mainWidth;
	pMainCrop->h = mainHeight;
	//pMainCrop->x = 0;
	//pMainCrop->y = 0;

	pAuxCrop->w = auxWidth;
	pAuxCrop->h = auxHeight;
	//pAuxCrop->x = 0;
	//pAuxCrop->y = 0;

	switch(displayMode)
	{	
		case DISPLAY_MODE_2SPLIT_HCROP_A:
			pMainCrop->w = mainWidth/2;
			pMainCrop->h = mainHeight;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxCrop->w = auxWidth/2;
			pAuxCrop->h = auxHeight;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
		case DISPLAY_MODE_2SPLIT_HSCALE_A:
			pMainView->w = DISPLAY_WIDTH_1280x720/2; //DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT_1280x720;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2; //DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT_1280x720;
			pAuxView->x = DISPLAY_WIDTH_1280x720; //DISPLAY_HALF_WIDTH;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_A:
			pMainCrop->w = mainWidth;
			pMainCrop->h = mainHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxCrop->w = auxWidth;
			pAuxCrop->h = auxHeight/2;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/2; 
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720/2;
			break;

		case DISPLAY_MODE_4SPLIT_R3SCALE:
			pMainView->w = DISPLAY_WIDTH_1280x720*2/3;
			pMainView->h = DISPLAY_HEIGHT_1280x720;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = DISPLAY_WIDTH_1280x720*4/3;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_R3CROP:
			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = DISPLAY_WIDTH_1280x720*4/3;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_L3SCALE:
			pMainView->w = DISPLAY_WIDTH_1280x720*2/3;
			pMainView->h = DISPLAY_HEIGHT_1280x720;
			pMainView->x = DISPLAY_WIDTH_1280x720*2/3 +1;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_L3CROP:
			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_D3SCALE:
			pMainView->w = DISPLAY_WIDTH_1280x720;
			pMainView->h = DISPLAY_HEIGHT_1280x720*2/3;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720*2/3;
			break;
			
		case DISPLAY_MODE_4SPLIT_D3CROP:
			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720*2/3;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3SCALE:
			pMainView->w = DISPLAY_WIDTH_1280x720;
			pMainView->h = DISPLAY_HEIGHT_1280x720*2/3;
			pMainView->x = 0;
			pMainView->y = DISPLAY_HEIGHT_1280x720/3;

			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3CROP:
			pAuxView->w = DISPLAY_WIDTH_1280x720/3;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/3;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_QUAD:
		case DISPLAY_MODE_4SPLIT_H:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720/2;
			break;

		case DISPLAY_MODE_4SPLIT_X:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxView->x = DISPLAY_WIDTH_1280x720;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_PIP_A2:
			pAuxView->w = PIP_WINDOW_WIDTH_1280;
			pAuxView->h = PIP_WINDOW_HEIGHT_720;
			pAuxView->x = (DISPLAY_WIDTH_1280x720-PIP_WINDOW_WIDTH_1280-PIP_POSITION_MARGIN_720)*2;
			pAuxView->y = PIP_POSITION_MARGIN_720;
			break;

		case DISPLAY_MODE_PIP_B2:
			pAuxView->w = PIP_WINDOW_WIDTH_1280;
			pAuxView->h = PIP_WINDOW_HEIGHT_720;
			pAuxView->x = PIP_POSITION_MARGIN_720*2;
			pAuxView->y = DISPLAY_HEIGHT_1280x720-PIP_WINDOW_HEIGHT_720-PIP_POSITION_MARGIN_720;
			break;

		case DISPLAY_MODE_PIP_C2:
			pAuxView->w = PIP_WINDOW_WIDTH_1280;
			pAuxView->h = PIP_WINDOW_HEIGHT_720;
			pAuxView->x = (DISPLAY_WIDTH_1280x720-PIP_WINDOW_WIDTH_1280-PIP_POSITION_MARGIN_720)*2;
			pAuxView->y = DISPLAY_HEIGHT_1280x720-PIP_WINDOW_HEIGHT_720-PIP_POSITION_MARGIN_720;
			break;

		case DISPLAY_MODE_PIP_D2:
			pAuxView->w = PIP_WINDOW_WIDTH_1280;
			pAuxView->h = PIP_WINDOW_HEIGHT_720;
			pAuxView->x = PIP_POSITION_MARGIN_720*2;
			pAuxView->y = PIP_POSITION_MARGIN_720;
			break;
/*
		default:
			pMainView->w = DISPLAY_WIDTH;
			pMainView->h = DISPLAY_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			//pAuxView->w = DISPLAY_WIDTH;
			//pAuxView->h = DISPLAY_HEIGHT;
			//pAuxView->x = 0;
			//pAuxView->y = 0;
			break;
*/
	}

	memcpy(&stVideo_A.stCROP_m, pMainCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_A.stVIEW_m, pMainView, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_A.stCROP_x, pAuxCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_A.stVIEW_x, pAuxView, sizeof(MDIN_VIDEO_WINDOW));
	ConfigI2C(MDIN_ID_A);
	MDIN3xx_SetScaleProcess(&stVideo_A);

}


void CreateDisplayWindow_B(eDisplayMode_t displayMode)
{
	WORD mainWidth, mainHeight;
	WORD auxWidth, auxHeight;

	PMDIN_VIDEO_WINDOW pMainView = &stMainVIEW[MDIN_ID_B];
	PMDIN_VIDEO_WINDOW pAuxView = &stAuxVIEW[MDIN_ID_B];
	PMDIN_VIDEO_WINDOW pMainCrop = &stMainCROP[MDIN_ID_B];
	PMDIN_VIDEO_WINDOW pAuxCrop = &stAuxCROP[MDIN_ID_B];
	
	// initialize each object
	memset(pMainView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pMainCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));

	switch(SrcMainFrmt[MDIN_ID_B])
	{
		case VIDSRC_1920x1080p60:
		case VIDSRC_1920x1080p50:
			mainWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			mainHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			mainWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			mainHeight = DISPLAY_HEIGHT_1280x720;
			break;

		case VIDSRC_960x480i60:
			mainWidth = DISPLAY_WIDTH_960*2;
			mainHeight = DISPLAY_HEIGHT_480/2;
			break;

		case VIDSRC_960x576i50:
			mainWidth = DISPLAY_WIDTH_960*2;
			mainHeight = DISPLAY_HEIGHT_576/2;
			break;
	}

	switch(SrcAuxFrmt[MDIN_ID_B])
	{
		case VIDSRC_1920x1080p60:
		case VIDSRC_1920x1080p50:
			auxWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			auxHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			auxWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			auxHeight = DISPLAY_HEIGHT_1280x720;
			break;
			
		case VIDSRC_960x480i60:
			auxWidth = DISPLAY_WIDTH_960*2;
			auxHeight = DISPLAY_HEIGHT_480/2;
			break;

		case VIDSRC_960x576i50:
			auxWidth = DISPLAY_WIDTH_960*2;
			auxHeight = DISPLAY_HEIGHT_576/2;
			break;

	}

	//Check video loss channel
	if((GetInputVideoFormat(CHANNEL3) == NC_VIVO_CH_FORMATDEF_UNKNOWN) && (displayMode != DISPLAY_MODE_FULL_CH4))
	{
		// channel 3 is always main
		mainWidth = DISPLAY_WIDTH_1280x720/2;
		mainHeight = DISPLAY_HEIGHT_1280x720;
	}

	if((GetInputVideoFormat(CHANNEL4) == NC_VIVO_CH_FORMATDEF_UNKNOWN) && (displayMode != DISPLAY_MODE_FULL_CH3))
	{
		// channel 4 is main or aux?
		if(displayMode == DISPLAY_MODE_FULL_CH4)
		{
			mainWidth = DISPLAY_WIDTH_1280x720/2;
			mainHeight = DISPLAY_HEIGHT_1280x720;
		}
		else
		{
			auxWidth = DISPLAY_WIDTH_1280x720/2;
			auxHeight = DISPLAY_HEIGHT_480/2;
		}
	}

	pMainCrop->w = mainWidth;
	pMainCrop->h = mainHeight;
	//pMainCrop->x = 0;
	//pMainCrop->y = 0;

	pAuxCrop->w = auxWidth;
	pAuxCrop->h = auxHeight;
	//pAuxCrop->x = 0;
	//pAuxCrop->y = 0;

	switch(displayMode)
	{	
		case DISPLAY_MODE_2SPLIT_HCROP_B:
			pMainCrop->w = mainWidth/2;
			pMainCrop->h = mainHeight;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxCrop->w = auxWidth/2;
			pAuxCrop->h = auxHeight;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
			pMainView->w = DISPLAY_WIDTH_1280x720/2; //DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT_1280x720;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2; //DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT_1280x720;
			pAuxView->x = DISPLAY_WIDTH_1280x720; //DISPLAY_HALF_WIDTH;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_B:
			pMainCrop->w = mainWidth;
			pMainCrop->h = mainHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxCrop->w = auxWidth;
			pAuxCrop->h = auxHeight/2;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_4SPLIT_QUAD:
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/2; //DISPLAY_HALF_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/2; //DISPLAY_HALF_HEIGHT;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720/2; //DISPLAY_HALF_HEIGHT;
			break;

		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/2; //DISPLAY_HALF_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/2; //DISPLAY_HALF_HEIGHT;
			pAuxView->x = DISPLAY_WIDTH_1280x720;
			pAuxView->y = 0; //DISPLAY_HALF_HEIGHT;
			break;

		case DISPLAY_MODE_4SPLIT_H:
			pMainView->w = DISPLAY_WIDTH_1280x720/4;
			pMainView->h = DISPLAY_HEIGHT_1280x720;
			pMainView->x = 0;
			pMainView->y = 0;

			pMainCrop->w = mainWidth/4;
			pMainCrop->h = mainHeight;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/4;
			pAuxView->h = DISPLAY_HEIGHT_1280x720;
			pAuxView->x = DISPLAY_WIDTH_1280x720*3/2;
			pAuxView->y = 0;

			pAuxCrop->w = auxWidth/4;
			pAuxCrop->h = auxHeight;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_X:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/8;
			pMainView->x = 0;
			pMainView->y = 0;

			pMainCrop->w = mainWidth;
			pMainCrop->h = mainHeight/4;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/8;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720*3/8;

			pAuxCrop->w = auxWidth;
			pAuxCrop->h = auxHeight/4;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;
	}

	memcpy(&stVideo_B.stCROP_m, pMainCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_B.stVIEW_m, pMainView, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_B.stCROP_x, pAuxCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_B.stVIEW_x, pAuxView, sizeof(MDIN_VIDEO_WINDOW));
	ConfigI2C(MDIN_ID_B);
	MDIN3xx_SetScaleProcess(&stVideo_B);

}


void CreateDisplayWindow_C(eDisplayMode_t displayMode)
{
	WORD mainWidth, mainHeight;
	WORD auxWidth, auxHeight;

	PMDIN_VIDEO_WINDOW pMainView = &stMainVIEW[MDIN_ID_C];
	PMDIN_VIDEO_WINDOW pAuxView = &stAuxVIEW[MDIN_ID_C];
	PMDIN_VIDEO_WINDOW pMainCrop = &stMainCROP[MDIN_ID_C];
	PMDIN_VIDEO_WINDOW pAuxCrop = &stAuxCROP[MDIN_ID_C];
	
	// initialize each object
	memset(pMainView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pMainCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));

	switch(displayMode)
	{
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			pMainCrop->w = DISPLAY_WIDTH_1280x720 / 2;
			pMainCrop->h = DISPLAY_HEIGHT_1280x720;
			pMainCrop->x = 0;
			pMainCrop->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_QUAD:
			pMainView->w = DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			pMainCrop->w = DISPLAY_WIDTH_1280x720 / 2;
			pMainCrop->h = DISPLAY_HEIGHT_1280x720;
			pMainCrop->x = 0;
			pMainCrop->y = 0;
			
			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT;
			pAuxView->x = DISPLAY_HALF_WIDTH;
			pAuxView->y = 0;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720 / 2;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
			pAuxView->w = DISPLAY_WIDTH/3;
			pAuxView->h = DISPLAY_HEIGHT*2/3;
			pAuxView->x = DISPLAY_WIDTH*2/3;
			pAuxView->y = DISPLAY_HEIGHT/3;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720 / 2;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
			pAuxView->w = DISPLAY_WIDTH/3;
			pAuxView->h = DISPLAY_HEIGHT*2/3;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT/3;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720 / 2;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
			pAuxView->w = DISPLAY_WIDTH*2/3;
			pAuxView->h = DISPLAY_HEIGHT/3;
			pAuxView->x = DISPLAY_WIDTH/3;
			pAuxView->y = DISPLAY_HEIGHT*2/3;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
			pAuxView->w = DISPLAY_WIDTH*2/3;
			pAuxView->h = DISPLAY_HEIGHT/3;
			pAuxView->x = DISPLAY_WIDTH/3;
			pAuxView->y = 0;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_H:
			pAuxView->w = DISPLAY_WIDTH/2;
			pAuxView->h = DISPLAY_HEIGHT;
			pAuxView->x = DISPLAY_WIDTH/4;
			pAuxView->y = 0;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720/2;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_X:
			pMainCrop->w = DISPLAY_WIDTH_1280x720 / 2;
			pMainCrop->h = DISPLAY_HEIGHT_1280x720 /2;
			pMainCrop->x = 0;
			pMainCrop->y = 0;
			
			pAuxView->w = DISPLAY_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT/2;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT/4;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
			pAuxView->w = PIP_WINDOW_WIDTH_1920;
			pAuxView->h = PIP_WINDOW_HEIGHT_1080;
			pAuxView->x = DISPLAY_WIDTH -PIP_WINDOW_WIDTH_1920-PIP_POSITION_MARGIN_1080;
			pAuxView->y = PIP_POSITION_MARGIN_1080;
			break;

		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
			pAuxView->w = PIP_WINDOW_WIDTH_1920;
			pAuxView->h = PIP_WINDOW_HEIGHT_1080;
			pAuxView->x = PIP_POSITION_MARGIN_1080;
			pAuxView->y = DISPLAY_HEIGHT -PIP_WINDOW_HEIGHT_1080-PIP_POSITION_MARGIN_1080;
			break;

		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
			pAuxView->w = PIP_WINDOW_WIDTH_1920;
			pAuxView->h = PIP_WINDOW_HEIGHT_1080;
			pAuxView->x = DISPLAY_WIDTH -PIP_WINDOW_WIDTH_1920-PIP_POSITION_MARGIN_1080;
			pAuxView->y = DISPLAY_HEIGHT -PIP_WINDOW_HEIGHT_1080-PIP_POSITION_MARGIN_1080;
			break;

		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			pAuxView->w = PIP_WINDOW_WIDTH_1920;
			pAuxView->h = PIP_WINDOW_HEIGHT_1080;
			pAuxView->x = PIP_POSITION_MARGIN_1080;
			pAuxView->y = PIP_POSITION_MARGIN_1080;
			break;

	}

	memcpy(&stVideo_C.stCROP_m, pMainCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_C.stVIEW_m, pMainView, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_C.stCROP_x, pAuxCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo_C.stVIEW_x, pAuxView, sizeof(MDIN_VIDEO_WINDOW));

	//ConfigI2C(MDIN_ID_C);
	//MDIN3xx_SetScaleProcess(&stVideo_C);
	//Delay_ms(100);
	stVideo_C.exeFLAG = MDIN_UPDATE_MAINFMT;
}

/*  FILE_END_HERE */
