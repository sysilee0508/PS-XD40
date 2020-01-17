// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
#include "NVP6168.h"
#include "raptor4_fmt.h"
#endif

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------
#define DUMP_REG								0

#define GPIO_JUMP				GPIO_Pin_1	//PC1
#define COMPENSATION_MARGIN	16//40

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
MDIN_VIDEO_INFO		stVideo;
MDIN_INTER_WINDOW	stInterWND;

BYTE AdjInterWND,  InputSelect, InputSelOld,  SrcSyncInfo;
BYTE SrcMainFrmt, PrevSrcMainFrmt, SrcMainMode, PrevSrcMainMode;
BYTE OutMainFrmt, PrevOutMainFrmt, OutMainMode, PrevOutMainMode;
BYTE SrcAuxFrmt, PrevSrcAuxFrmt, SrcAuxMode, PrevSrcAuxMode;
BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
BYTE AdcVideoFrmt, PrevAdcFrmt, EncVideoFrmt, PrevEncFrmt;
BYTE TempOutMainMode;		// 28Dec2011
BOOL fInputChanged;

MDIN_VIDEO_WINDOW stMainVIEW, stAuxVIEW;
MDIN_VIDEO_WINDOW stMainCROP, stAuxCROP;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------
#if DUMP_REG
void MDIN_DumpRegister(BYTE id, WORD start, WORD len);
#endif

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
	else if(GetInputVideoFormat(CHANNEL2) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInputVideoFormat(CHANNEL2);
	}

	switch(inputFormat)
	{
		case SD_H960_NT:
		case SD_H960_EX_NT:
		case SD_H960_2EX_NT:
		case SD_H960_2EX_Btype_NT:
		case AHD_1080P_30P:
		case AHD_720P_60P:
		case AHD_720P_30P:
		case AHD_720P_30P_EX:
		case AHD_720P_30P_EX_Btype:
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

		case SD_H960_PAL:
		case SD_H960_EX_PAL:
		case SD_H960_2EX_PAL:
		case SD_H960_2EX_Btype_PAL:
		case AHD_1080P_25P:
		case AHD_720P_50P:
		case AHD_720P_25P:
		case AHD_720P_25P_EX:
		case AHD_720P_25P_EX_Btype:
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

	if(InputSelect != VIDEO_DIGITAL_NVP6158_2CH)	// CVBS out
	{
		switch(videoOut)
		{
			case VIDOUT_1920x1080p60:
			case VIDOUT_1920x1080p30:
				auxOut = VIDOUT_720x480i60;
				stVideo.encFRMT = VID_VENC_NTSC_M;
				break;
			case VIDOUT_1920x1080p50:
			case VIDOUT_1920x1080p25:
				auxOut = VIDOUT_720x576i50;
				stVideo.encFRMT = VID_VENC_PAL_B;
				break;
		}
	}
	return auxOut;
}

static MDIN_SRCVIDEO_FORMAT_t GetInSourceFormat(eChannel_t channel)
{
	static MDIN_SRCVIDEO_FORMAT_t format[NUM_OF_CHANNEL] = {VIDSRC_1920x1080p60, VIDSRC_1920x1080p60};

	switch(GetInputVideoFormat(channel))
	{
		case SD_H960_NT:
		case SD_H960_EX_NT:
		case SD_H960_2EX_NT:
		case SD_H960_2EX_Btype_NT:
			format[channel] = VIDSRC_960x480i60;	//720x480p 60hz
			break;

		case SD_H960_PAL:
		case SD_H960_EX_PAL:
		case SD_H960_2EX_PAL:
		case SD_H960_2EX_Btype_PAL:
			format[channel] = VIDSRC_960x576i50;	//720x576p 50hz
			break;
		case AHD_1080P_30P:
		case TVI_FHD_30P:
		case TVI_FHD_60P:
		case CVI_FHD_30P:
			format[channel] = VIDSRC_1920x1080p60;	//1080p60
			break;

		case AHD_1080P_25P:
		case TVI_FHD_25P:
		case CVI_FHD_25P:
			format[channel] = VIDSRC_1920x1080p50;	//1080p50
			break;

		case AHD_720P_60P:
		case AHD_720P_30P:
		case AHD_720P_30P_EX:
		case AHD_720P_30P_EX_Btype:
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

		case AHD_720P_50P:
		case AHD_720P_25P:
		case AHD_720P_25P_EX:
		case AHD_720P_25P_EX_Btype:
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
	stVideo.stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	stVideo.stSRC_a.offH = 0;
	stVideo.stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = VIDSRC_1920x1080p60;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23

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
	stVideo.stIPC_m.mode = MDIN_DEINT_NONSTILL;//MDIN_DEINT_ADAPTIVE;
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
//	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
//	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
//	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
//							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	MDINHTX_SetHDMIBlock(&stVideo);		// initialize HDMI block

#if 0		// temporary blocked by kukuri
	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = MDIN_4CHVIEW_ALL;	 // set 4CH view mode
#endif
	stVideo.exeFLAG = MDIN_UPDATE_ALL;	// execution of video process
	MDIN3xx_VideoInProcess(&stVideo);
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process
	MDINAUX_VideoProcess(&stVideo);
	
	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	MDIN3xx_EnableAuxDisplay(&stVideo, ON);
	MDIN3xx_EnableMainDisplay(ON);

	// define variable for EDK application
//	InputSelOld = 0xff;
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
		case VIDEO_DIGITAL_NVP6158_A:
			stVideo.srcPATH = PATH_MAIN_A_AUX_M;
			stVideo.dacPATH = DAC_PATH_MAIN_OUT;
			stVideo.encPATH = VENC_PATH_PORT_X;
			break;
/*
		case VIDEO_DIGITAL_NVP6158_B:
			stVideo.srcPATH = PATH_MAIN_B_AUX_M;
			stVideo.dacPATH = DAC_PATH_MAIN_OUT;
			stVideo.encPATH = VENC_PATH_PORT_X;
			break;
*/			
		case VIDEO_DIGITAL_NVP6158_2CH:
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
#if BD_NVP == NVP6158
			if(NVP6158_Get_VportMap() == VPORT_MAP0)
#elif BD_NVP == NVP6168
			if(NVP6168_Get_VportMap() == VPORT_MAP0)
#endif
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
			break;

		case VIDEO_DIGITAL_NVP6158_2CH:
			currentMainFrmt = GetInSourceFormat(CHANNEL1);//stVideo.stSRC_a.frmt;
			break;

		default:
//			currentMainFrmt = VIDSRC_1280x720p60;
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
//		case VIDEO_DIGITAL_NVP6158_B:
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
//		case VIDEO_DIGITAL_NVP6158_B:
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

	if((SrcMainFrmt != PrevSrcMainFrmt) || (SrcMainMode != PrevSrcMainMode))
	{
		stVideo.exeFLAG |= MDIN_UPDATE_MAIN_IN;
	}

	if((SrcAuxFrmt != PrevSrcAuxFrmt) || (SrcAuxMode != PrevSrcAuxMode))
	{
		stVideo.exeFLAG |= MDIN_UPDATE_AUX_IN;
	}
		
	if((OutMainFrmt != PrevOutMainFrmt) || (OutMainMode != PrevOutMainMode))
	{
		stVideo.exeFLAG |= MDIN_UPDATE_MAIN_OUT;
	}

	if((OutAuxFrmt != PrevOutAuxFrmt) || (OutAuxMode != PrevOutAuxMode))
	{
		stVideo.exeFLAG |= MDIN_UPDATE_AUX_OUT | MDIN_UPDATE_ENC;
	}

	Set_DisplayWindow(GetCurrentDisplayMode());

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
	OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], OFF, OFF);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo, ON);
}

//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess(void)
{
	if(stVideo.exeFLAG == MDIN_UPDATE_CLEAR)	return;

	stVideo.stSRC_a.frmt = SrcMainFrmt;
	stVideo.stSRC_b.frmt = SrcAuxFrmt;
	//stVideo.stSCR_x.frmt = SrcAuxFrmt;

	stVideo.stOUT_m.frmt = OutMainFrmt;
	stVideo.stOUT_x.frmt = OutAuxFrmt;
	stVideo.stOUT_x.mode = OutAuxMode;
	
	PrevOutMainFrmt = OutMainFrmt;
	PrevSrcMainFrmt = SrcMainFrmt;
	PrevSrcAuxFrmt = SrcAuxFrmt;
	PrevOutAuxFrmt = OutAuxFrmt;
	PrevOutAuxMode = OutAuxMode;
	
	if (stVideo.exeFLAG&MDIN_UPDATE_MAIN_IN)
	{
		//MDIN3xx_OutDarkScreen(ON);
		MDIN3xx_EnableMainDisplay(OFF);
		//MDIN3xx_EnableMainFreeze(ON);
	}
	if (stVideo.exeFLAG&MDIN_UPDATE_AUX_IN)
	{
		//MDIN3xx_AuxDarkScreen(ON);
		MDIN3xx_EnableAuxDisplay(&stVideo, OFF);
		//MDIN3xx_EnableAuxFreeze(&stVideo, ON);
	}

	if (stVideo.exeFLAG&MDIN_UPDATE_IN)		
		MDIN3xx_VideoInProcess(&stVideo);
	if (stVideo.exeFLAG&MDIN_UPDATE_MAIN)	
		MDIN3xx_VideoProcess(&stVideo);
	if (stVideo.exeFLAG&MDIN_UPDATE_AUX)		
		MDINAUX_VideoProcess(&stVideo);

	SetIPCVideoFine();	// tune IPC-register (CVBS or HDMI)

	if(InputSelect== VIDEO_DIGITAL_NVP6158_A)
	{ 
		MDIN3xx_EnableOutputPAD(MDIN_PAD_ENC_OUT, ON);
	}
	else
	{
		MDIN3xx_EnableOutputPAD(MDIN_PAD_ENC_OUT, OFF);
	}
	
	MDIN3xx_EnableAuxDisplay(&stVideo, ON);
	MDIN3xx_EnableMainDisplay(ON);

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

void SetInputChanged(void)
{
	fInputChanged = TRUE;
	stVideo.exeFLAG = MDIN_UPDATE_ALL;
}
//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
	if(fInputChanged == TRUE)
	{
		InputSourceHandler(InputSelect);
		VideoFrameProcess();
		SetOSDMenuRefresh();

#if DUMP_REG
		MDIN_DumpRegister(MDIN_HOST_ID, 0x0000, 0x100); // Host Register 0x000~0x0FF
		MDINDLY_mSec(1);
		MDIN_DumpRegister(MDIN_LOCAL_ID, 0x0000, 0x400); // Local Register 0x000~0x3FF
#endif
	}
	fInputChanged = FALSE;
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	MDINHTX_CtrlHandler(&stVideo);
}

void Request2VideoProcess(void)
{
	stVideo.exeFLAG = MDIN_UPDATE_MAIN | MDIN_UPDATE_AUX;
}

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
			mainWidth = DISPLAY_WIDTH_960*2- COMPENSATION_MARGIN;
			mainHeight = DISPLAY_HEIGHT_480;
			break;

		case VIDSRC_960x576i50:
			mainWidth = DISPLAY_WIDTH_960*2- COMPENSATION_MARGIN;
			mainHeight = DISPLAY_HEIGHT_576;
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
			auxWidth = DISPLAY_WIDTH_960*2- COMPENSATION_MARGIN;
			auxHeight = DISPLAY_HEIGHT_480;
			break;

		case VIDSRC_960x576i50:
			auxWidth = DISPLAY_WIDTH_960*2- COMPENSATION_MARGIN;
			auxHeight = DISPLAY_HEIGHT_576;
			break;

	}

	stMainCROP.w = mainWidth;
	stMainCROP.h = mainHeight;
	stMainCROP.x = 0;
	stMainCROP.y = 0;

	stAuxCROP.w = auxWidth;
	stAuxCROP.h = auxHeight;
	stAuxCROP.x = 0;
	stAuxCROP.y = 0;

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
	MDIN3xx_SetVideoWindowCROP(&stVideo, stMainCROP);
	MDIN3xx_SetVideoWindowVIEW(&stVideo, stMainVIEW);
	MDINAUX_SetVideoWindowCROP(&stVideo, stAuxCROP);
	MDINAUX_SetVideoWindowVIEW(&stVideo, stAuxVIEW);
}


#if DUMP_REG
// register dump
WORD regHost[256];
WORD regLocal[1024];	// 2k

void MDIN_DumpRegister(BYTE id, WORD start, WORD len)
{
	WORD addr, index;
	PWORD pBuf;
	
	// init
	addr = start;
	index = 0;

	if(id == MDIN_HOST_ID)	pBuf = regHost;
	else if(id == MDIN_LOCAL_ID)	pBuf = regLocal;
	
	memset((void *)pBuf, 0x00, sizeof(WORD)*len);
	
	for(index = 0; index < len; index++)
	{
		MDINHIF_RegRead(id, addr, pBuf+index);
		addr++;
	}
}
#endif

/*  FILE_END_HERE */
