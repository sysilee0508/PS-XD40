// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#include "NVP6158.h"        

#if defined(SYSTEM_USE_MDIN380)
// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------
#define GPIO_JUMP				GPIO_Pin_1	//PC1

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
static MDIN_OUTVIDEO_FORMAT_t GetOutVideoFrameRate(void)
{
	BYTE inputFormat = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	static MDIN_OUTVIDEO_FORMAT_t output = VIDOUT_1920x1080p60;

	if(IsVideoLossChannel(CHANNEL1) == FALSE)
	{
		inputFormat = GetInputVideoFormat(CHANNEL1);
	}
	else if(IsVideoLossChannel(CHANNEL2) == FALSE)
	{
		inputFormat = GetInputVideoFormat(CHANNEL2);
	}

	switch(inputFormat)
	{
		case AHD20_SD_SH720_NT:
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

		case AHD20_SD_SH720_PAL:
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
				output = VIDOUT_1920x1080p50;
			}
			else
			{
				output = VIDOUT_1920x1080p25;
			}
			break;
	}
	return output;
}

static MDIN_OUTVIDEO_FORMAT_t GetOutAuxFormat(MDIN_OUTVIDEO_FORMAT_t videoOut)
{
	MDIN_OUTVIDEO_FORMAT_t auxOut = videoOut;

	if(InputSelect != VIDEO_DIGITAL_NVP6158_2CH)	// CVBS out
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
	MDIN_SRCVIDEO_FORMAT_t format = VIDSRC_1920x1080p60;

	switch(GetInputVideoFormat(channel))
	{
		case AHD20_SD_SH720_NT:
		case AHD20_SD_SH720_PAL:
			break;

		case AHD20_1080P_60P:
		case AHD20_1080P_30P:
		case TVI_FHD_30P:
		case CVI_FHD_30P:
			format = VIDSRC_1920x1080p60;	//1080p60
			break;

		case AHD20_1080P_50P:
		case AHD20_1080P_25P:
		case TVI_FHD_25P:
		case CVI_FHD_25P:
			format = VIDSRC_1920x1080p50;	//1080p50
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
			format = VIDSRC_1280x720p60;	//720p60
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
			format = VIDSRC_1280x720p50;	//720p50
			break;
	}
	return format;
}

static void MDIN3xx_SetRegInitial(void)
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

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);

	// set aux display ON
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is main out
	stVideo.dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo.stSRC_a.frmt = VIDSRC_1920x1080p60;
	stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_a.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_a.offH = 0;
	stVideo.stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = VIDSRC_1920x1080p60;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_b.offH = 0;
	stVideo.stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo.stOUT_m.frmt = VIDOUT_1920x1080p60;
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
	// define video format of AUX-INPUT
	stVideo.stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24

	// define video format of AUX-OUTPUT (CVBS output)
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

	// audio block
	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	MDINHTX_SetHDMIBlock(&stVideo);		// initialize HDMI block

#if 0		// temporary blocked by kukuri
	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = MDIN_4CHVIEW_ALL;	 // set 4CH view mode
#endif
	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process

	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
	InputSelOld = 0xff;
	InputSelect = VIDEO_DIGITAL_NVP6158_A;

	PrevSrcMainFrmt = 0xff;
	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;
	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;
	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;
	OutMainMode = stVideo.stOUT_m.mode;
	TempOutMainMode = OutMainMode;

	PrevOutAuxFrmt = 0xff;
	OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;
	OutAuxMode = stVideo.stOUT_x.mode;
	PrevEncFrmt = 0xff;
	EncVideoFrmt = stVideo.encFRMT;
}

//--------------------------------------------------------------------------------------------------
// Set Input Video Path
//--------------------------------------------------------------------------------------------------
static void SetInVideoPath(BYTE src)
{
	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_2CH: //VIDEO_ADCNV_2HD_IN:
//		case VIDEO_DIGITAL_NVP6158_2CH_PIP: //VIDEO_SDI_2HD_POP:
			stVideo.srcPATH = PATH_MAIN_A_AUX_B;
			stVideo.dacPATH = DAC_PATH_MAIN_PIP;
			break;

		case VIDEO_DIGITAL_NVP6158_A:
			stVideo.srcPATH = PATH_MAIN_A_AUX_M;
			stVideo.dacPATH = DAC_PATH_MAIN_OUT;
			break;

		case VIDEO_DIGITAL_NVP6158_B:
			stVideo.srcPATH = PATH_MAIN_B_AUX_M;
			stVideo.dacPATH = DAC_PATH_MAIN_OUT;
			break;
	}
}

//--------------------------------------------------------------------------------------------------
//static BYTE GetSrcMainMode(BYTE src)
//{
//	BYTE mode;
//
//	switch (src)
//	{
//		case VIDEO_SDI_2HD_POP:
//			mode = MDIN_SRC_EMB422_8;
//			break;
//		case VIDEO_DIGITAL_SDI:
//		case VIDEO_DIGITAL_SDI2:
//			mode = MDIN_SRC_MUX656_8;
//			break;
//		default:
//			mode = MDIN_SRC_SEP422_8;
//			break;
//	}
//	return mode;
//}

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcMainFrmt(BYTE src)
{
	BYTE currentMainFrmt;

	switch(src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			currentMainFrmt = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
			break;

		case VIDEO_DIGITAL_NVP6158_B:
			currentMainFrmt = GetInSourceFormat(CHANNEL2);//stVideo.stSRC_b.frmt;
			break;

		case VIDEO_DIGITAL_NVP6158_2CH:
			currentMainFrmt = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
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
		case VIDEO_DIGITAL_NVP6158_B:
			mode = OutMainMode;
			break;
		case VIDEO_DIGITAL_NVP6158_2CH:
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
		case VIDEO_DIGITAL_NVP6158_B:
			if((OutMainFrmt == VIDOUT_1920x1080p60) || (OutMainFrmt == VIDOUT_1920x1080p30))
			{
				frmt = VIDSRC_1920x1080p60;
			}
			else
			{
				frmt = VIDSRC_1920x1080p50;
			}
			break;
		case VIDEO_DIGITAL_NVP6158_2CH:
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
		case VIDEO_DIGITAL_NVP6158_B:
			mode = MDIN_OUT_MUX656_8;
			break;
		case VIDEO_DIGITAL_NVP6158_2CH:
			mode = MDIN_OUT_RGB444_8;
			break;
	}
	return mode;
}
//--------------------------------------------------------------------------------------------------
static void InputSourceHandler(BYTE src)
{
	if (src==InputSelOld) return;

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

	memset(&stVideo.stVIEW_m, 0, 8);		// clear stVIEW_m
	memset(&stVideo.stVIEW_x, 0, 8);		// clear stVIEW_x

	if((SrcMainFrmt != 0xFF) && (SrcMainFrmt != 0xFE))
	{
		fSyncParsed = TRUE;
	}
	else
	{
		fSyncParsed = FALSE;
	}
	InputSelOld = src;
	PrevSrcMainFrmt = PrevSrcMainMode = PrevAdcFrmt = 0xff;
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
			MDIN3xx_SetDelayCLK_A(MDIN_CLK_DELAY5);
			MDIN3xx_SetDelayCLK_B(MDIN_CLK_DELAY5);
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
static void SetAUXVideoFilter(void)
{
	PMDIN_AUXFILT_COEF pCoef = GetAUXFilterCoef();

	MDINAUX_EnableFrontNRFilter(&stVideo, (pCoef==NULL)? OFF : ON);
	if (pCoef!=NULL) MDINAUX_SetFrontNRFilterCoef(pCoef);
}

//--------------------------------------------------------------------------------------------------
static void SetOSDMenuRefresh(void)
{
	OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], OFF, OFF);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo, ON);
}

//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess(BYTE src)
{
	if (fSyncParsed==FALSE) return;		// wait for sync detection

	if (EncVideoFrmt!=PrevEncFrmt)
	{
		PrevSrcMainFrmt = 0xff;
	}
	stVideo.encFRMT = EncVideoFrmt;
	PrevEncFrmt = EncVideoFrmt;

	if (SrcMainFrmt!=PrevSrcMainFrmt||SrcMainMode!=PrevSrcMainMode||
		OutMainFrmt!=PrevOutMainFrmt||OutMainMode!=PrevOutMainMode)
	{
		stVideo.exeFLAG |= MDIN_UPDATE_MAINFMT;
	}

	if (SrcAuxFrmt!=PrevSrcAuxFrmt||SrcAuxMode!=PrevSrcAuxMode||
		OutAuxFrmt!=PrevOutAuxFrmt||OutAuxMode!=PrevOutAuxMode)
	{
		stVideo.exeFLAG |= MDIN_UPDATE_AUXFMT;
	}

	if (stVideo.exeFLAG!=MDIN_UPDATE_CLEAR) // updated video formats
	{
		stVideo.stIPC_m.fine &= ~MDIN_DEINT_3DNR_ON;   //3DNR off

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

		SetOffChipFrmt(src);		// set InA offchip format
		SetSrcMainFine(src);		// set source video fine (fldid, offset)

		if (OutMainFrmt!=PrevOutMainFrmt) {
			stVideo.pHY_m		= 	NULL;		// restore MFCHY from API
			stVideo.pHC_m		= 	NULL;		// restore MFCHY from API
			stVideo.pVY_m		= 	NULL;		// restore MFCHY from API
			stVideo.pVC_m		= 	NULL;		// restore MFCHY from API
		}
		MDIN3xx_VideoProcess(&stVideo);		// mdin3xx main video process

	//	GetExtVideoAttb(src);	// update E-Video attribute (edge,swap,clk,offset)	//by hungry 2012.02.15
		SetIPCVideoFine(src);	// tune IPC-register (CVBS or HDMI)
		SetAUXVideoFilter();	// tune AUX-filter (DUAL or CVBS)
		//Set2HDVideoPathB();		// set 2HD pathB

		//SetMenuStatus(4,6,MBIT(stVideo.stOUT_m.stATTB.attb,MDIN_WIDE_RATIO));
		//DEMO_SetWindowPIPPOP(GetMenuStatus(4,3));	// update pip/pop window	//by kukuri
		//DEMO_SetImageMirrorV(GetMenuStatus(6,7));	// update v-mirror

		MDIN3xx_EnableAuxDisplay(&stVideo, ON);
		MDIN3xx_EnableMainDisplay(ON);

		// if src is 2HD input or dual display, trigger soft-reset.
//		if (src==VIDEO_ADCNV_2HD_IN||GetMenuStatus(4,4)) 	//kukuri.. check it
//			MDIN3xx_SoftReset();

		// Do we need below line? by kukuri
		SetOSDMenuRefresh();	// for framebuffer map bug
	//	SetOutHDMI_DVI();

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
	MDIN3xx_SetRegInitial();	// initialize MDIN-3xx
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
}

//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
	InputSourceHandler(InputSelect);
//	InputSyncHandler_A(InputSelect);
//	InputSyncHandler_B(InputSelect);		  //by hungry 2012.02.27
	VideoFrameProcess(InputSelect);
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	MDINHTX_CtrlHandler(&stVideo);
}

#endif	/* defined(SYSTEM_USE_MDIN380) */

/*  FILE_END_HERE */
