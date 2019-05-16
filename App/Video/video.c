// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#include "NVP6158.h"        

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------
#define GPIO_JUMP				GPIO_Pin_1	//PC1
#define COMPENSATION_MARGIN	40

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

MDIN_VIDEO_INFO	 stVideo_A;
MDIN_VIDEO_INFO stVideo_C;
MDIN_VIDEO_INFO stVideo;
MDIN_INTER_WINDOW	stInterWND;

BYTE AdjInterWND,  InputSelect, InputSelOld,  SrcSyncInfo;
BYTE SrcMainFrmt, PrevSrcMainFrmt, SrcMainMode, PrevSrcMainMode;
BYTE OutMainFrmt, PrevOutMainFrmt, OutMainMode, PrevOutMainMode;
BYTE SrcAuxFrmt, PrevSrcAuxFrmt, SrcAuxMode, PrevSrcAuxMode;
BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
BYTE AdcVideoFrmt, PrevAdcFrmt, EncVideoFrmt, PrevEncFrmt;
BYTE TempOutMainMode;		// 28Dec2011
//BOOL fSyncParsed;
BOOL fInputChanged;

MDIN_VIDEO_WINDOW stMainVIEW, stAuxVIEW;
MDIN_VIDEO_WINDOW stMainCROP, stAuxCROP;


// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------
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
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_JUMP) == LOW)	//short?
			{
				output = VIDOUT_1920x1080p30;
			}
			else
			{
				output = VIDOUT_1920x1080p60;
			}
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
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_JUMP) == LOW)	//short?
			{
				output = VIDOUT_1920x1080p25;
			}
			else
			{
				output = VIDOUT_1920x1080p50;;
			}
			break;
	}
	return output;
}

static MDIN_OUTVIDEO_FORMAT_t GetOutAuxFormat(MDIN_OUTVIDEO_FORMAT_t videoOut)
{
	MDIN_OUTVIDEO_FORMAT_t auxOut = videoOut;

	if(InputSelect != VIDEO_DIGITAL_NVP6158_AB)	// CVBS out
	{
		switch(videoOut)
		{
			case VIDOUT_1920x1080p60:
			case VIDOUT_1920x1080p30:
				auxOut = VIDOUT_720x480i60;
				EncVideoFrmt = VID_VENC_NTSC_M;
				break;
			case VIDOUT_1920x1080p50:
			case VIDOUT_1920x1080p25:
				auxOut = VIDOUT_720x576i50;
				EncVideoFrmt = VID_VENC_PAL_B;
				break;
		}
	}
	return auxOut;
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

#if 1
static void MDIN3xx_SetRegInitial_A(void)
{
	WORD nID = 0;
	
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

	memset((PBYTE)&stVideo_A, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo_A, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo_A, NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo_A, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo_A, NULL);

	// set aux display ON
	stVideo_A.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo_A.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is main out
	stVideo_A.dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo_A.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo_A.stSRC_a.frmt = VIDSRC_1280x720p60;
	stVideo_A.stSRC_a.mode = MDIN_SRC_MUX656_8;
	//stVideo_A.stSRC_a.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo_A.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	stVideo_A.stSRC_a.offH = 0;
	stVideo_A.stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo_A.stSRC_b.frmt = VIDSRC_1280x720p60;
	stVideo_A.stSRC_b.mode = MDIN_SRC_MUX656_8;
	//stVideo_A.stSRC_b.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo_A.stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
	stVideo_A.stSRC_b.offH = 0;
	stVideo_A.stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo_A.stOUT_m.frmt = VIDOUT_1280x720p60;
	stVideo_A.stOUT_m.mode = MDIN_OUT_MUX656_8;	 //by hungry 2012.03.06		// test by chungsa
	stVideo_A.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo_A.stOUT_m.brightness = 128;			// set main picture factor
	stVideo_A.stOUT_m.contrast = 128;
	stVideo_A.stOUT_m.saturation = 128;
	stVideo_A.stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo_A.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo_A.stOUT_m.g_gain = 128;
	stVideo_A.stOUT_m.b_gain = 128;
	stVideo_A.stOUT_m.r_offset = 128;
	stVideo_A.stOUT_m.g_offset = 128;
	stVideo_A.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo_A.stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo_A.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo_A.stIPC_m.gain = 34;
	stVideo_A.stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	stVideo_A.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_ON
	// define video format of AUX-INPUT
	stVideo_A.stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24

	// define video format of AUX-OUTPUT (CVBS output)
	stVideo_A.stOUT_x.frmt = VIDOUT_720x480i60;
	stVideo_A.stOUT_x.mode = MDIN_OUT_MUX656_8;
	stVideo_A.stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run

	stVideo_A.stOUT_x.brightness = 128;			// set aux picture factor
	stVideo_A.stOUT_x.contrast = 128;
	stVideo_A.stOUT_x.saturation = 128;
	stVideo_A.stOUT_x.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo_A.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo_A.stOUT_x.g_gain = 128;
	stVideo_A.stOUT_x.b_gain = 128;
	stVideo_A.stOUT_x.r_offset = 128;
	stVideo_A.stOUT_x.g_offset = 128;
	stVideo_A.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo_A.encFRMT = VID_VENC_NTSC_M;
	// define video format of HDMI-OUTPUT
	stVideo_A.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo_A.stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	// audio block
	stVideo_A.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo_A.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo_A.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	MDINHTX_SetHDMIBlock(&stVideo_A);		// initialize HDMI block

#if 0		// temporary blocked by kukuri
	// define video format of 4CH-display
	stVideo_A.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo_A.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo_A.st4CH_x.view  = MDIN_4CHVIEW_ALL;	 // set 4CH view mode
#endif
	stVideo_A.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo_A);			// mdin3xx main video process

	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
//	InputSelOld = 0xff;
	InputSelect = VIDEO_DIGITAL_NVP6158_A;

	PrevSrcMainFrmt = 0xff;
	SrcMainFrmt = stVideo_A.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;
	SrcMainMode = stVideo_A.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;
	OutMainFrmt = stVideo_A.stOUT_m.frmt;
	PrevOutMainMode = 0xff;
	OutMainMode = stVideo_A.stOUT_m.mode;
	TempOutMainMode = OutMainMode;

	PrevOutAuxFrmt = 0xff;
	OutAuxFrmt = stVideo_A.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;
	OutAuxMode = stVideo_A.stOUT_x.mode;
	PrevEncFrmt = 0xff;
	EncVideoFrmt = stVideo_A.encFRMT;
}
#endif
static void MDIN3xx_SetRegInitial_C(void)
{
	WORD nID = 0;
	
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

	memset((PBYTE)&stVideo_C, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo_C, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo_C, NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo_C, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo_C, NULL);

	// set aux display ON
	stVideo_C.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo_C.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is main out
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
	stVideo_C.encFRMT = VID_VENC_NTSC_M;
	// define video format of HDMI-OUTPUT
	stVideo_C.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo_C.stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	// audio block
	stVideo_C.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo_C.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo_C.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	MDINHTX_SetHDMIBlock(&stVideo_C);		// initialize HDMI block

#if 0		// temporary blocked by kukuri
	// define video format of 4CH-display
	stVideo_C.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo_C.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo_C.st4CH_x.view  = MDIN_4CHVIEW_ALL;	 // set 4CH view mode
#endif
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
//	InputSelOld = 0xff;
	InputSelect = VIDEO_DIGITAL_NVP6158_A;

	PrevSrcMainFrmt = 0xff;
	SrcMainFrmt = stVideo_C.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;
	SrcMainMode = stVideo_C.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;
	OutMainFrmt = stVideo_C.stOUT_m.frmt;
	PrevOutMainMode = 0xff;
	OutMainMode = stVideo_C.stOUT_m.mode;
	TempOutMainMode = OutMainMode;

	PrevOutAuxFrmt = 0xff;
	OutAuxFrmt = stVideo_C.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;
	OutAuxMode = stVideo_C.stOUT_x.mode;
	PrevEncFrmt = 0xff;
	EncVideoFrmt = stVideo_C.encFRMT;
}

//--------------------------------------------------------------------------------------------------
// Set Input Video Path
//--------------------------------------------------------------------------------------------------
static void SetInVideoPath(BYTE src)
{
	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			stVideo.srcPATH = PATH_MAIN_A_AUX_M;
			stVideo.dacPATH = DAC_PATH_MAIN_OUT;
			stVideo.encPATH = VENC_PATH_PORT_X;
			break;

//		case VIDEO_DIGITAL_NVP6158_B:
//			stVideo.srcPATH = PATH_MAIN_B_AUX_M;
//			stVideo.dacPATH = DAC_PATH_MAIN_OUT;
//			stVideo.encPATH = VENC_PATH_PORT_X;
//			break;
			
		case VIDEO_DIGITAL_NVP6158_AB:
			stVideo.srcPATH = PATH_MAIN_A_AUX_B;
			stVideo.dacPATH = DAC_PATH_MAIN_PIP;
			stVideo.encPATH = VENC_PATH_PORT_B;
			break;
	}
}

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcMainFrmt(BYTE src)
{
	BYTE currentMainFrmt;

	switch(src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			if(NVP6158_Get_VportMap() == VPORT_MAP0)
			{
				currentMainFrmt = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
			}
			else
			{
				currentMainFrmt = GetInSourceFormat(CHANNEL2);//stVideo.stSRC_a.frmt;
			}
			break;

//		case VIDEO_DIGITAL_NVP6158_B:
//			currentMainFrmt = GetInSourceFormat(CHANNEL2);//stVideo.stSRC_b.frmt;
//			break;

		case VIDEO_DIGITAL_NVP6158_AB:
			currentMainFrmt = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
			break;

		default:
			currentMainFrmt = VIDSRC_1280x720p60;
			break;
	}

	return currentMainFrmt;
}

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
//--------------------------------------------------------------------------------------------------
static void InputSourceHandler(BYTE src)
{
	if(fInputChanged != TRUE)  return;

	SetInVideoPath(src);

	// source : main
	SrcMainMode = MDIN_SRC_MUX656_8; // this is fixed (never changed)
	SrcMainFrmt = GetSrcMainFrmt(src);
	// source : aux (full screen : main out / Split(PIP) : ch2)
	SrcAuxMode = GetSrcAuxMode(src);
	SrcAuxFrmt = GetSrcAuxFrmt(src);
	// out format
	OutMainFrmt = GetOutVideoFrameRate();		// get out-main format
	OutMainMode = MDIN_OUT_RGB444_8;
	OutAuxFrmt = GetOutAuxFormat(OutMainFrmt);	// get out-aux format
	OutAuxMode = GetOutAuxMode(src);

	Set_DisplayWindow(GetCurrentDisplayMode());
	InputSelOld = src;

	//PrevSrcMainFrmt = PrevSrcMainMode = PrevAdcFrmt = 0xff;
}

#if 0
//--------------------------------------------------------------------------------------------------
static void InputSyncHandler_A(BYTE src)
{
	BYTE frmt = 0xff;

	frmt = SrcMainFrmt;
	if ((frmt!=0xff) && (frmt!=0xfe))
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
			//OutMainFrmt = videoOutResolution+(frmt%2);
			OutMainFrmt = OutMainFrmt+(frmt%2);
			OutMainMode = MDIN_OUT_RGB444_8;//MDIN_OUT_SEP422_8;
			break;
	}

}

//--------------------------------------------------------------------------------------------------
static void InputSyncHandler_B(BYTE src)
{
	BYTE frmt = 0xff;

	if (frmt!=0xff&&frmt!=0xfe)
	{
		fSyncParsed = TRUE;
		SrcAuxFrmt = frmt;
	}
	switch (stVideo.dacPATH) 
	{
		case DAC_PATH_MAIN_PIP:	
			SrcAuxMode = MDIN_SRC_EMB422_8;	
			//OutAuxFrmt = videoOutResolution+(frmt%2);
			OutAuxFrmt = OutMainFrmt+(frmt%2);
			OutAuxMode = MDIN_OUT_RGB444_8;//MDIN_OUT_SEP422_8;
		break;
	}
}
#endif

#if 0
//--------------------------------------------------------------------------------------------------
static void SetOffChipFrmt(BYTE src)
{
	switch (src) 
	{
 		case VIDEO_DIGITAL_NVP6158_A:
 			MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY0);
 			break;
 		case VIDEO_DIGITAL_NVP6158_B:
 			MDIN3xx_SetDelayCLK_B(MDIN_CLK_DELAY0);
 			break;
		case VIDEO_DIGITAL_NVP6158_2CH :
			MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY0);
			MDIN3xx_SetDelayCLK_B(MDIN_CLK_DELAY0);
			break;
		default:
 			MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY0);
 			MDIN3xx_SetDelayCLK_B(MDIN_CLK_DELAY0);
 			break;
	}
}

//--------------------------------------------------------------------------------------------------
static void SetSrcMainFine(BYTE src)
{
	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
		case VIDEO_DIGITAL_NVP6158_2CH:
		 	stVideo.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
		 	stVideo.stSRC_a.offH = 0;
		 	stVideo.stSRC_a.offV = 0;
			break;
		case VIDEO_DIGITAL_NVP6158_B:
		 	stVideo.stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
		 	stVideo.stSRC_b.offH = 0;
		 	stVideo.stSRC_b.offV = 0;
			break;
	}
}
#endif
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
		case VIDOUT_720x576i50:		return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[3];
		case VIDOUT_720x480i60:		return (PMDIN_AUXFILT_COEF)&defAUXFiltCoef[4];
		default:					return (PMDIN_AUXFILT_COEF)NULL;
	}
}

#if 0
//--------------------------------------------------------------------------------------------------
static void SetAUXVideoFilter(void)
{
	PMDIN_AUXFILT_COEF pCoef = GetAUXFilterCoef();

	MDINAUX_EnableFrontNRFilter(&stVideo, (pCoef==NULL)? OFF : ON);
	if (pCoef!=NULL) MDINAUX_SetFrontNRFilterCoef(pCoef);
}
#endif 

//--------------------------------------------------------------------------------------------------
static void SetOSDMenuRefresh(void)
{
	OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], OFF, OFF);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo_C, ON);
}

#if 1
//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess_A(BYTE src)
{
	//if (fSyncParsed==FALSE) return;		// wait for sync detection
	if(fInputChanged == FALSE) return;

	if (EncVideoFrmt!=PrevEncFrmt)
	{
		PrevSrcMainFrmt = 0xff;
	}
	stVideo_A.encFRMT = EncVideoFrmt;
	PrevEncFrmt = EncVideoFrmt;

	if (SrcMainFrmt!=PrevSrcMainFrmt||SrcMainMode!=PrevSrcMainMode||
		OutMainFrmt!=PrevOutMainFrmt||OutMainMode!=PrevOutMainMode)
	{
		stVideo_A.exeFLAG |= MDIN_UPDATE_MAINFMT;
	}

	if (SrcAuxFrmt!=PrevSrcAuxFrmt||SrcAuxMode!=PrevSrcAuxMode||
		OutAuxFrmt!=PrevOutAuxFrmt||OutAuxMode!=PrevOutAuxMode)
	{
		stVideo_A.exeFLAG |= MDIN_UPDATE_AUXFMT;
	}

	//if (stVideo_A.exeFLAG!=MDIN_UPDATE_CLEAR) // updated video formats
	{
		stVideo_A.stIPC_m.fine &= ~MDIN_DEINT_3DNR_ON;   //3DNR off
#if 0
		if (stVideo_A.srcPATH == PATH_MAIN_B_AUX_B || stVideo_A.srcPATH == PATH_MAIN_B_AUX_A || stVideo_A.srcPATH == PATH_MAIN_B_AUX_M)
		{
			stVideo_A.stSRC_b.frmt = SrcMainFrmt; stVideo_A.stSRC_b.mode = SrcMainMode;
			stVideo_A.stSRC_a.frmt = SrcAuxFrmt; stVideo_A.stSRC_a.mode = SrcAuxMode;
		}
		else
		{
			stVideo_A.stSRC_a.frmt = SrcMainFrmt; stVideo_A.stSRC_a.mode = SrcMainMode;
			stVideo_A.stSRC_b.frmt = SrcAuxFrmt; stVideo_A.stSRC_b.mode = SrcAuxMode;
		}

		stVideo_A.stOUT_m.frmt = OutMainFrmt; stVideo_A.stOUT_m.mode = OutMainMode;
		stVideo_A.stOUT_x.frmt = OutAuxFrmt; stVideo_A.stOUT_x.mode = OutAuxMode;
#endif
		//Set main & aux window scale, crop, zoom
		//memcpy(&stVideo_A.stCROP_m, &stMainCROP, sizeof(MDIN_VIDEO_WINDOW));
		//memcpy(&stVideo_A.stCROP_x, &stAuxCROP, sizeof(MDIN_VIDEO_WINDOW));
		//memcpy(&stVideo_A.stVIEW_m, &stMainVIEW, sizeof(MDIN_VIDEO_WINDOW));
		//memcpy(&stVideo_A.stVIEW_x, &stAuxVIEW, sizeof(MDIN_VIDEO_WINDOW));

		//MDIN3xx_SetScaleProcess(&stVideo_A);

		MDIN3xx_EnableAuxDisplay(&stVideo_A, OFF);
		MDIN3xx_EnableMainDisplay(OFF);

		//SetOffChipFrmt(src);		// set InA offchip format
		//SetSrcMainFine(src);		// set source video fine (fldid, offset)

		//if (OutMainFrmt!=PrevOutMainFrmt) 
		{
			stVideo_A.pHY_m		= 	NULL;		// restore MFCHY from API
			stVideo_A.pHC_m		= 	NULL;		// restore MFCHY from API
			stVideo_A.pVY_m		= 	NULL;		// restore MFCHY from API
			stVideo_A.pVC_m		= 	NULL;		// restore MFCHY from API
		}
		MDIN3xx_VideoProcess(&stVideo_A);		// mdin3xx main video process
		//MDINAUX_VideoProcess(&stVideo_A);

		SetIPCVideoFine(src);	// tune IPC-register (CVBS or HDMI)
		//SetAUXVideoFilter();	// tune AUX-filter (DUAL or CVBS)

		MDIN3xx_EnableAuxDisplay(&stVideo_A, ON);
		MDIN3xx_EnableMainDisplay(ON);

		//SetOSDMenuRefresh();
		
		PrevSrcMainFrmt = SrcMainFrmt;	PrevSrcMainMode = SrcMainMode;
		PrevOutMainFrmt = OutMainFrmt;	PrevOutMainMode = OutMainMode;

		PrevSrcAuxFrmt = SrcAuxFrmt;	PrevSrcAuxMode = SrcAuxMode;
		PrevOutAuxFrmt = OutAuxFrmt;	PrevOutAuxMode = OutAuxMode;
	}
}
#endif
//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess_C(BYTE src)
{
	//if (fSyncParsed==FALSE) return;		// wait for sync detection
	if(fInputChanged == FALSE) return;

	if (EncVideoFrmt!=PrevEncFrmt)
	{
		PrevSrcMainFrmt = 0xff;
	}
	stVideo_C.encFRMT = EncVideoFrmt;
	PrevEncFrmt = EncVideoFrmt;

	if (SrcMainFrmt!=PrevSrcMainFrmt||SrcMainMode!=PrevSrcMainMode||
		OutMainFrmt!=PrevOutMainFrmt||OutMainMode!=PrevOutMainMode)
	{
		stVideo_C.exeFLAG |= MDIN_UPDATE_MAINFMT;
	}

	if (SrcAuxFrmt!=PrevSrcAuxFrmt||SrcAuxMode!=PrevSrcAuxMode||
		OutAuxFrmt!=PrevOutAuxFrmt||OutAuxMode!=PrevOutAuxMode)
	{
		stVideo_C.exeFLAG |= MDIN_UPDATE_AUXFMT;
	}

	//if (stVideo_C.exeFLAG!=MDIN_UPDATE_CLEAR) // updated video formats
	{
		stVideo_C.stIPC_m.fine &= ~MDIN_DEINT_3DNR_ON;   //3DNR off
#if 0
		if (stVideo_C.srcPATH == PATH_MAIN_B_AUX_B || stVideo_C.srcPATH == PATH_MAIN_B_AUX_A || stVideo_C.srcPATH == PATH_MAIN_B_AUX_M)
		{
			stVideo_C.stSRC_b.frmt = SrcMainFrmt; stVideo_C.stSRC_b.mode = SrcMainMode;
			stVideo_C.stSRC_a.frmt = SrcAuxFrmt; stVideo_C.stSRC_a.mode = SrcAuxMode;
		}
		else
		{
			stVideo_C.stSRC_a.frmt = SrcMainFrmt; stVideo_C.stSRC_a.mode = SrcMainMode;
			stVideo_C.stSRC_b.frmt = SrcAuxFrmt; stVideo_C.stSRC_b.mode = SrcAuxMode;
		}

		stVideo_C.stOUT_m.frmt = OutMainFrmt; stVideo_C.stOUT_m.mode = OutMainMode;
		stVideo_C.stOUT_x.frmt = OutAuxFrmt; stVideo_C.stOUT_x.mode = OutAuxMode;
#endif
		//Set main & aux window scale, crop, zoom
		//memcpy(&stVideo_C.stCROP_m, &stMainCROP, sizeof(MDIN_VIDEO_WINDOW));
		//memcpy(&stVideo_C.stCROP_x, &stAuxCROP, sizeof(MDIN_VIDEO_WINDOW));
		//memcpy(&stVideo_C.stVIEW_m, &stMainVIEW, sizeof(MDIN_VIDEO_WINDOW));
		//memcpy(&stVideo_C.stVIEW_x, &stAuxVIEW, sizeof(MDIN_VIDEO_WINDOW));

		//MDIN3xx_SetScaleProcess(&stVideo_C);

		MDIN3xx_EnableAuxDisplay(&stVideo_C, OFF);
		MDIN3xx_EnableMainDisplay(OFF);

		//SetOffChipFrmt(src);		// set InA offchip format
		//SetSrcMainFine(src);		// set source video fine (fldid, offset)

		if (OutMainFrmt!=PrevOutMainFrmt) 
		{
			stVideo_C.pHY_m		= 	NULL;		// restore MFCHY from API
			stVideo_C.pHC_m		= 	NULL;		// restore MFCHY from API
			stVideo_C.pVY_m		= 	NULL;		// restore MFCHY from API
			stVideo_C.pVC_m		= 	NULL;		// restore MFCHY from API
		}
		MDIN3xx_VideoProcess(&stVideo_C);		// mdin3xx main video process
		//MDINAUX_VideoProcess(&stVideo_C);

		SetIPCVideoFine(src);	// tune IPC-register (CVBS or HDMI)
		//SetAUXVideoFilter();	// tune AUX-filter (DUAL or CVBS)

		MDIN3xx_EnableAuxDisplay(&stVideo_C, ON);
		MDIN3xx_EnableMainDisplay(ON);

		//SetOSDMenuRefresh();
		
		PrevSrcMainFrmt = SrcMainFrmt;	PrevSrcMainMode = SrcMainMode;
		PrevOutMainFrmt = OutMainFrmt;	PrevOutMainMode = OutMainMode;

		PrevSrcAuxFrmt = SrcAuxFrmt;	PrevSrcAuxMode = SrcAuxMode;
		PrevOutAuxFrmt = OutAuxFrmt;	PrevOutAuxMode = OutAuxMode;
	}
}

// ----------------------------------------------------------------------
// Exported functions
// ----------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void CreateVideoInstance(void)
{
#if 0 //C-A
	I2C_SET_CHANNEL(I2C_SUB);
	MDIN3xx_SetRegInitial_C();
	Delay_ms(1);
	I2C_SET_CHANNEL(I2C_MAIN);
	MDIN3xx_SetRegInitial_A();	// initialize MDIN-3xx

#else // A-C
	I2C_SET_CHANNEL(I2C_MAIN);
	MDIN3xx_SetRegInitial_A();	// initialize MDIN-3xx
	Delay_ms(1);
	I2C_SET_CHANNEL(I2C_SUB);
	MDIN3xx_SetRegInitial_C();
#endif
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
//	fInputChanged = TRUE;
}

void SetInputChanged(void)
{
	fInputChanged = TRUE;
}
//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
//	InputSourceHandler(InputSelect);
//	InputSyncHandler_A(InputSelect);
//	InputSyncHandler_B(InputSelect);		  //by hungry 2012.02.27
	I2C_SET_CHANNEL(I2C_MAIN);
	VideoFrameProcess_A(InputSelect);
//	Delay_ms(1);
	I2C_SET_CHANNEL(I2C_SUB);
	VideoFrameProcess_C(InputSelect);
//	SetOSDMenuRefresh();
//	fInputChanged = FALSE;
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	I2C_SET_CHANNEL(I2C_SUB);
	MDINHTX_CtrlHandler(&stVideo_C);
}

//void Request2VideoProcess(void)
//{
//	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT | MDIN_UPDATE_AUXFMT;
//}

void Set_DisplayWindow(eDisplayMode_t displayMode)
{
	WORD mainWidth, mainHeight;
	WORD auxWidth, auxHeight;
	
	// initialize each object
	memset(&stMainVIEW, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(&stAuxVIEW, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(&stMainCROP, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(&stAuxCROP, 0x00, sizeof(MDIN_VIDEO_WINDOW));

	switch(SrcMainFrmt)
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

	switch(SrcAuxFrmt)
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

	stMainCROP.w = mainWidth;
	stMainCROP.h = mainHeight;
	stMainCROP.x = 0;
	stMainCROP.y = 0;

	stAuxCROP.w = auxWidth;
	stAuxCROP.h = auxHeight;
	stAuxCROP.x = 0;
	stAuxCROP.y = 0;
#if 0
	switch(displayMode)
	{	
		case DISPLAY_MODE_SPLIT_A:
			stMainVIEW.w = DISPLAY_HALF_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = DISPLAY_HALF_WIDTH;
			stAuxVIEW.h = DISPLAY_HEIGHT;
			stAuxVIEW.x = DISPLAY_HALF_WIDTH;
			stAuxVIEW.y = 0;
			break;

		case DISPLAY_MODE_SPLIT_B:
			stMainCROP.w = mainWidth/2;
			stMainCROP.h = mainHeight;
			stMainCROP.x = 0;
			stMainCROP.y = 0;

			stAuxCROP.w = auxWidth/2;
			stAuxCROP.h = auxHeight;
			stAuxCROP.x = 0;
			stAuxCROP.y = 0;

			stMainVIEW.w = DISPLAY_HALF_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = DISPLAY_HALF_WIDTH;
			stAuxVIEW.h = DISPLAY_HEIGHT;
			stAuxVIEW.x = DISPLAY_HALF_WIDTH;
			stAuxVIEW.y = 0;
			break;

		case DISPLAY_MODE_SPLIT_C:
			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HALF_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = DISPLAY_WIDTH;
			stAuxVIEW.h = DISPLAY_HALF_HEIGHT;
			stAuxVIEW.x = 0;
			stAuxVIEW.y = DISPLAY_HALF_HEIGHT;
			break;

		case DISPLAY_MODE_SPLIT_D:
			stMainCROP.w = mainWidth;
			stMainCROP.h = mainHeight/2;
			stMainCROP.x = 0;
			stMainCROP.y = 0;

			stAuxCROP.w = auxWidth;
			stAuxCROP.h = auxHeight/2;
			stAuxCROP.x = 0;
			stAuxCROP.y = 0;

			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HALF_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = DISPLAY_WIDTH;
			stAuxVIEW.h = DISPLAY_HALF_HEIGHT;
			stAuxVIEW.x = 0;
			stAuxVIEW.y = DISPLAY_HALF_HEIGHT;
			break;

		case DISPLAY_MODE_SPLIT_E:
			stMainVIEW.w = DISPLAY_HALF_WIDTH;
			stMainVIEW.h = DISPLAY_HALF_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = DISPLAY_QUAD_HEIGHT;

			stAuxVIEW.w = DISPLAY_HALF_WIDTH;
			stAuxVIEW.h = DISPLAY_HALF_HEIGHT;
			stAuxVIEW.x = DISPLAY_HALF_WIDTH;
			stAuxVIEW.y = DISPLAY_QUAD_HEIGHT;
			break;

		case DISPLAY_MODE_PIP_A:
			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = PIP_WINDOW_WIDTH;
			stAuxVIEW.h = PIP_WINDOW_HEIGHT;
			stAuxVIEW.x = DISPLAY_WIDTH -PIP_WINDOW_WIDTH -PIP_POSITION_MARGIN;
			stAuxVIEW.y = PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_B:
			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = PIP_WINDOW_WIDTH;
			stAuxVIEW.h = PIP_WINDOW_HEIGHT;
			stAuxVIEW.x = PIP_POSITION_MARGIN;
			stAuxVIEW.y = DISPLAY_HEIGHT -PIP_WINDOW_HEIGHT -PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_C:
			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = PIP_WINDOW_WIDTH;
			stAuxVIEW.h = PIP_WINDOW_HEIGHT;
			stAuxVIEW.x = DISPLAY_WIDTH -PIP_WINDOW_WIDTH -PIP_POSITION_MARGIN;
			stAuxVIEW.y = DISPLAY_HEIGHT -PIP_WINDOW_HEIGHT -PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_D:
			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			stAuxVIEW.w = PIP_WINDOW_WIDTH;
			stAuxVIEW.h = PIP_WINDOW_HEIGHT;
			stAuxVIEW.x = PIP_POSITION_MARGIN;
			stAuxVIEW.y = PIP_POSITION_MARGIN;
			break;
/*
		default:
			stMainVIEW.w = DISPLAY_WIDTH;
			stMainVIEW.h = DISPLAY_HEIGHT;
			stMainVIEW.x = 0;
			stMainVIEW.y = 0;

			//stAuxVIEW.w = DISPLAY_WIDTH;
			//stAuxVIEW.h = DISPLAY_HEIGHT;
			//stAuxVIEW.x = 0;
			//stAuxVIEW.y = 0;
			break;
*/
	}
#endif
	MDIN3xx_SetVideoWindowCROP(&stVideo, stMainCROP);
	MDIN3xx_SetVideoWindowVIEW(&stVideo, stMainVIEW);
	MDINAUX_SetVideoWindowCROP(&stVideo, stAuxCROP);
	MDINAUX_SetVideoWindowVIEW(&stVideo, stAuxVIEW);
}


/*  FILE_END_HERE */
