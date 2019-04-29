// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#include "NVP6158.h"        

#if defined(SYSTEM_USE_MDIN380)
// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------
//#define GPIO_JUMP				GPIO_Pin_1	//PC1
#define COMPENSATION_MARGIN		0

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
#if 0
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
#endif

static MDIN_VIDEO_INFO		stVideo[MDIN_CHIP_ID_MAX];
//MDIN_INTER_WINDOW	stInterWND;

static MDIN_VIDEO_INPUT_t videoInput, prevVideoInput;//,  SrcSyncInfo;
//Source Video Format
//static MDIN_SRCVIDEO_FORMAT_t SrcMainFrmt[MDIN_CHIP_ID_MAX];
static MDIN_SRCVIDEO_FORMAT_t PrevSrcMainFrmt[MDIN_CHIP_ID_MAX];
//static MDIN_SRCVIDEO_FORMAT_t SrcAuxFrmt[MDIN_CHIP_ID_MAX];
static MDIN_SRCVIDEO_FORMAT_t PrevSrcAuxFrmt[MDIN_CHIP_ID_MAX];
// Source Video Mode
//static MDIN_SRCPORT_MODE_t SrcMainMode[MDIN_CHIP_ID_MAX];
//static MDIN_SRCPORT_MODE_t PrevSrcMainMode[MDIN_CHIP_ID_MAX];
//static MDIN_SRCPORT_MODE_t SrcAuxMode[MDIN_CHIP_ID_MAX];
//static MDIN_SRCPORT_MODE_t PrevSrcAuxMode[MDIN_CHIP_ID_MAX];
// Out Format
//static MDIN_OUTVIDEO_FORMAT_t OutMainFrmt[MDIN_CHIP_ID_MAX];
static MDIN_OUTVIDEO_FORMAT_t PrevOutMainFrmt[MDIN_CHIP_ID_MAX];
//static MDIN_OUTVIDEO_FORMAT_t OutAuxFrmt[MDIN_CHIP_ID_MAX];
static MDIN_OUTVIDEO_FORMAT_t PrevOutAuxFrmt[MDIN_CHIP_ID_MAX];
// Out Mode
//static MDIN_OUTPORT_MODE_t OutMainMode[MDIN_CHIP_ID_MAX];
//static MDIN_OUTPORT_MODE_t PrevOutMainMode[MDIN_CHIP_ID_MAX];
//static MDIN_OUTPORT_MODE_t OutAuxMode[MDIN_CHIP_ID_MAX];
//static MDIN_OUTPORT_MODE_t PrevOutAuxMode[MDIN_CHIP_ID_MAX];
// Encoder
//static MDIN_VENC_FORMAT_t EncVideoFrmt[MDIN_CHIP_ID_MAX];
static MDIN_VENC_FORMAT_t PrevEncFrmt[MDIN_CHIP_ID_MAX];

static MDIN_VIDEO_WINDOW stMainVIEW[MDIN_CHIP_ID_MAX];
static MDIN_VIDEO_WINDOW stAuxVIEW[MDIN_CHIP_ID_MAX];
static MDIN_VIDEO_WINDOW stMainCROP[MDIN_CHIP_ID_MAX];
static MDIN_VIDEO_WINDOW stAuxCROP[MDIN_CHIP_ID_MAX];

//BYTE SrcMainFrmt, PrevSrcMainFrmt, SrcMainMode, PrevSrcMainMode;
//BYTE OutMainFrmt, PrevOutMainFrmt, OutMainMode, PrevOutMainMode;
//BYTE SrcAuxFrmt, PrevSrcAuxFrmt, SrcAuxMode, PrevSrcAuxMode;
//BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
//static BYTE EncVideoFrmt, PrevEncFrmt; //AdcVideoFrmt, PrevAdcFrmt,

//BYTE TempOutMainMode;		// 28Dec2011
//BOOL fSyncParsed;
static BOOL fInputChanged;


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
// Set Input Video Path
//--------------------------------------------------------------------------------------------------
static void SetInVideoPath(MDIN_VIDEO_INPUT_t src)
{
	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			stVideo[MDIN_CHIP_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_A].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_A].encPATH = VENC_PATH_PORT_X;

			stVideo[MDIN_CHIP_ID_C].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_C].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_C].encPATH = VENC_PATH_PORT_X;
			break;

		case VIDEO_DIGITAL_NVP6158_AB:
			stVideo[MDIN_CHIP_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_A].dacPATH = DAC_PATH_MAIN_PIP;
			stVideo[MDIN_CHIP_ID_A].encPATH = VENC_PATH_PORT_B;

			stVideo[MDIN_CHIP_ID_C].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_C].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_C].encPATH = VENC_PATH_PORT_X;
			break;

		case VIDEO_DIGITAL_NVP6158_C:
			stVideo[MDIN_CHIP_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_B].encPATH = VENC_PATH_PORT_X;

			stVideo[MDIN_CHIP_ID_C].srcPATH = PATH_MAIN_B_AUX_A;
			stVideo[MDIN_CHIP_ID_C].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_C].encPATH = VENC_PATH_PORT_X;
			break;

		case VIDEO_DIGITAL_NVP6158_CD:
			stVideo[MDIN_CHIP_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_B].dacPATH = DAC_PATH_MAIN_PIP;
			stVideo[MDIN_CHIP_ID_B].encPATH = VENC_PATH_PORT_B;

			stVideo[MDIN_CHIP_ID_C].srcPATH = PATH_MAIN_B_AUX_A;
			stVideo[MDIN_CHIP_ID_C].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_C].encPATH = VENC_PATH_PORT_X;
			break;

		case VIDEO_DIGITAL_NVP6158_ABCD:
			stVideo[MDIN_CHIP_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_A].dacPATH = DAC_PATH_MAIN_PIP;
			stVideo[MDIN_CHIP_ID_A].encPATH = VENC_PATH_PORT_B;

			stVideo[MDIN_CHIP_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_B].dacPATH = DAC_PATH_MAIN_PIP;
			stVideo[MDIN_CHIP_ID_B].encPATH = VENC_PATH_PORT_B;

			stVideo[MDIN_CHIP_ID_C].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_C].dacPATH = DAC_PATH_MAIN_PIP;
			stVideo[MDIN_CHIP_ID_C].encPATH = VENC_PATH_PORT_B;
			break;

		case VIDEO_DIGITAL_NVP6158_AC:
		case VIDEO_DIGITAL_NVP6158_AD:
			stVideo[MDIN_CHIP_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_A].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_A].encPATH = VENC_PATH_PORT_X;

			stVideo[MDIN_CHIP_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
			stVideo[MDIN_CHIP_ID_B].encPATH = VENC_PATH_PORT_X;

			stVideo[MDIN_CHIP_ID_C].srcPATH = PATH_MAIN_A_AUX_B;
			stVideo[MDIN_CHIP_ID_C].dacPATH = DAC_PATH_MAIN_PIP;
			stVideo[MDIN_CHIP_ID_C].encPATH = VENC_PATH_PORT_B;
			break;
	}
}

// this function will return input video source format of MDIN A and B
static MDIN_SRCVIDEO_FORMAT_t GetInSourceFormat(eChannel_t channel)
{
	static MDIN_SRCVIDEO_FORMAT_t format[NUM_OF_CHANNEL] = {VIDSRC_1280x720p60, VIDSRC_1280x720p60, VIDSRC_1280x720p60, VIDSRC_1280x720p60};

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

//--------------------------------------------------------------------------------------------------
static BYTE GetSrcMainFrmt(MDIN_VIDEO_INPUT_t src, MDIN_CHIP_ID_t chipId)
{
//	MDIN_SRCVIDEO_FORMAT_t currentMainFrmt;

	if((chipId == MDIN_CHIP_ID_A) || (chipId == MDIN_CHIP_ID_B))
	{
		switch(src)
		{
			case VIDEO_DIGITAL_NVP6158_A:
				if(NVP6158_Get_VportMap() == VPORT_MAP0)
				{
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL1);
				}
				else
				{
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL2);
				}
				break;

			case VIDEO_DIGITAL_NVP6158_AB:
				stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL1);
				break;

			case VIDEO_DIGITAL_NVP6158_C:
				if(NVP6158_Get_VportMap() == VPORT_MAP0)
				{
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL3);
				}
				else
				{
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL4);
				}
				break;

			case VIDEO_DIGITAL_NVP6158_CD:
				stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL3);
				break;

			case VIDEO_DIGITAL_NVP6158_ABCD:
			case VIDEO_DIGITAL_NVP6158_AC:
				if(chipId == MDIN_CHIP_ID_A)
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL1);
				else
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL3);
				break;

			case VIDEO_DIGITAL_NVP6158_AD:
				if(chipId == MDIN_CHIP_ID_A)
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL1);
				else
					stVideo[chipId].stSRC_a.frmt = GetInSourceFormat(CHANNEL4);
				break;

			default:
				stVideo[chipId].stSRC_a.frmt = VIDSRC_1280x720p60;
				break;
		}

	}
	else if(chipId == MDIN_CHIP_ID_C)
	{
		switch(src)
		{
			case VIDEO_DIGITAL_NVP6158_A:
			case VIDEO_DIGITAL_NVP6158_AB:
			case VIDEO_DIGITAL_NVP6158_AC:
			case VIDEO_DIGITAL_NVP6158_AD:
			case VIDEO_DIGITAL_NVP6158_ABCD:
				if(GetOutVideoFrameRate_A() == VIDOUT_1920x1080p30)
				{
					stVideo[chipId].stSRC_a.frmt = VIDSRC_1920x1080p60;
				}
				else
				{
					stVideo[chipId].stSRC_a.frmt = VIDSRC_1920x1080p50;
				}
				break;

			case VIDEO_DIGITAL_NVP6158_C:
			case VIDEO_DIGITAL_NVP6158_CD:
				if(GetOutVideoFrameRate_B() == VIDOUT_1920x1080p30)
				{
					stVideo[chipId].stSRC_a.frmt = VIDSRC_1920x1080p60;
				}
				else
				{
					stVideo[chipId].stSRC_a.frmt = VIDSRC_1920x1080p50;
				}
				break;
		}
	}
	else	// mdin D
	{

	}

	return stVideo[chipId].stSRC_a.frmt;
}

//--------------------------------------------------------------------------------------------------
#if 0
static BYTE GetSrcAuxMode(MDIN_VIDEO_INPUT_t src)
{
	BYTE mode;

	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
//		case VIDEO_DIGITAL_NVP6158_B:
//			mode = ;
			break;
		case VIDEO_DIGITAL_NVP6158_AB:
		default:
			mode = MDIN_SRC_MUX656_8;
			break;
	}

	return mode;
}
#endif
//--------------------------------------------------------------------------------------------------
static BYTE GetSrcAuxFrmt(MDIN_VIDEO_INPUT_t src, MDIN_CHIP_ID_t chipId)
{
	BYTE frmt = 0xFF;

	if((chipId == MDIN_CHIP_ID_A) || (chipId == MDIN_CHIP_ID_B))
	{
		switch (src)
		{
			case VIDEO_DIGITAL_NVP6158_AB:
				frmt = GetInSourceFormat(CHANNEL2);
				break;

			case VIDEO_DIGITAL_NVP6158_CD:
				frmt = GetInSourceFormat(CHANNEL4);
				break;

			case VIDEO_DIGITAL_NVP6158_ABCD:
				if(chipId == MDIN_CHIP_ID_A)
					frmt = GetInSourceFormat(CHANNEL2);
				else
					frmt = GetInSourceFormat(CHANNEL4);
				break;
		}
	}
	stVideo[chipId].stSRC_b.frmt = frmt;

	return frmt;
}

#if 0
static BYTE GetOutAuxMode(MDIN_VIDEO_INPUT_t src)
{
	BYTE mode;

	switch (src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
//		case VIDEO_DIGITAL_NVP6158_B:
			mode = MDIN_OUT_MUX656_8;
			break;
		case VIDEO_DIGITAL_NVP6158_AB:
			mode = MDIN_OUT_RGB444_8;
			break;
	}
	return mode;
}
#endif

static MDIN_OUTVIDEO_FORMAT_t GetOutVideoFrameRate_A(void)
{
	BYTE inputFormat = VIDSRC_1280x720p60;
	MDIN_OUTVIDEO_FORMAT_t output = VIDOUT_1920x1080p30;

	if(GetInputVideoFormat(CHANNEL1) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInSourceFormat(CHANNEL1);
	}
	else if(GetInputVideoFormat(CHANNEL1) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInSourceFormat(CHANNEL2);
	}

	switch(inputFormat)
	{
		case VIDSRC_960x480i60:
		case VIDSRC_1280x720p60:
		case VIDSRC_1920x1080p60:
			output = VIDOUT_1920x1080p30;
			break;

		case VIDSRC_960x576i50:
		case VIDSRC_1280x720p50:
		case VIDSRC_1920x1080p50:
			output = VIDOUT_1920x1080p25;
			break;
	}
	stVideo[MDIN_CHIP_ID_A].stOUT_m.frmt = output;

	return output;
}

static MDIN_OUTVIDEO_FORMAT_t GetOutVideoFrameRate_B(void)
{
	BYTE inputFormat = VIDSRC_1280x720p60;
	MDIN_OUTVIDEO_FORMAT_t output = VIDOUT_1920x1080p30;

	if(GetInputVideoFormat(CHANNEL3) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInSourceFormat(CHANNEL3);
	}
	else if(GetInputVideoFormat(CHANNEL4) != NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		inputFormat = GetInSourceFormat(CHANNEL4);
	}

	switch(inputFormat)
	{
		case VIDSRC_960x480i60:
		case VIDSRC_1280x720p60:
		case VIDSRC_1920x1080p60:
			output = VIDOUT_1920x1080p30;
			break;

		case VIDSRC_960x576i50:
		case VIDSRC_1280x720p50:
		case VIDSRC_1920x1080p50:
			output = VIDOUT_1920x1080p25;
			break;
	}
	stVideo[MDIN_CHIP_ID_B].stOUT_m.frmt = output;

	return output;
}

static MDIN_OUTVIDEO_FORMAT_t GetOutVideoFrameRate_C(void)
{
	MDIN_OUTVIDEO_FORMAT_t output = VIDOUT_1920x1080p30;

	if(stVideo[MDIN_CHIP_ID_C].stSRC_m.frmt == VIDSRC_1920x1080p50)
	{
		output = VIDOUT_1920x1080p25;
	}
	stVideo[MDIN_CHIP_ID_C].stOUT_m.frmt = output;

	return output;
}

static MDIN_OUTVIDEO_FORMAT_t GetOutVideoFrameRate_D(void)
{
	MDIN_OUTVIDEO_FORMAT_t output = VIDOUT_1920x1080p30;

//	if(stVideo[MDIN_CHIP_ID_D].stSRC_m.frmt == VIDSRC_1920x1080p50)
//	{
//		output = VIDOUT_1920x1080p25;
//	}
	stVideo[MDIN_CHIP_ID_D].stOUT_m.frmt = output;

	return output;
}


static MDIN_OUTVIDEO_FORMAT_t GetOutAuxFormat(MDIN_VIDEO_INPUT_t src, MDIN_CHIP_ID_t chipId)
{
//	MDIN_OUTVIDEO_FORMAT_t auxOut = videoOut;
//
//	switch(src)
//	{
//		case VIDEO_DIGITAL_NVP6158_AB:
//			if(chipId == MDIN_CHIP_ID_A)
//			{
//				stVideo[chipId].stOUT_x.frmt =
//			}
//			break;
//	}
//
//	if(videoInput != VIDEO_DIGITAL_NVP6158_AB)	// CVBS out
//	{
//		switch(videoOut)
//		{
//			case VIDOUT_1920x1080p60:
//			case VIDOUT_1920x1080p30:
//				auxOut = VIDOUT_720x480i60;
//				EncVideoFrmt = VID_VENC_NTSC_M;
//				break;
//			case VIDOUT_1920x1080p50:
//			case VIDOUT_1920x1080p25:
//				auxOut = VIDOUT_720x576i50;
//				EncVideoFrmt = VID_VENC_PAL_B;
//				break;
//		}
//	}
//	return auxOut;
}


static void Video_DefaultSetup(void)
{
	MDIN_CHIP_ID_t id;

	//common
	for(id = MDIN_CHIP_ID_A; id < MDIN_CHIP_ID_MAX; id++)
	{
		// initialize
		memset((PBYTE)&stVideo[id], 0, sizeof(MDIN_VIDEO_INFO));

		//chip id
		stVideo[id].chipId = id;
		// set aux display ON
		stVideo[id].dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

		// set video path
		stVideo[id].srcPATH = PATH_MAIN_A_AUX_B;	// set main is A, aux is B
		stVideo[id].dacPATH = DAC_PATH_MAIN_OUT;	// set main only
		stVideo[id].encPATH = VENC_PATH_PORT_X;		// set venc is aux

		// define video format of PORTA-INPUT
		stVideo[id].stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
		stVideo[id].stSRC_a.offH = 0;
		stVideo[id].stSRC_a.offV = 0;

		// define video format of PORTB-INPUT
		stVideo[id].stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
		stVideo[id].stSRC_b.offH = 0;
		stVideo[id].stSRC_b.offV = 0;

		stVideo[id].stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run
		stVideo[id].stOUT_m.brightness = 128;			// set main picture factor
		stVideo[id].stOUT_m.contrast = 128;
		stVideo[id].stOUT_m.saturation = 128;
		stVideo[id].stOUT_m.hue = 128;

	#if RGB_GAIN_OFFSET_TUNE == 1
		stVideo[id].stOUT_m.r_gain = 128;				// set main gain/offset
		stVideo[id].stOUT_m.g_gain = 128;
		stVideo[id].stOUT_m.b_gain = 128;
		stVideo[id].stOUT_m.r_offset = 128;
		stVideo[id].stOUT_m.g_offset = 128;
		stVideo[id].stOUT_m.b_offset = 128;
	#endif

		// define video format of IPC-block
		stVideo[id].stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
		stVideo[id].stIPC_m.film = MDIN_DEINT_FILM_OFF;
		stVideo[id].stIPC_m.gain = 34;
		stVideo[id].stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;

		// define map of frame buffer
		stVideo[id].stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_ON
		// define video format of AUX-INPUT
		stVideo[id].stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24

		stVideo[id].stOUT_x.fine = MDIN_SYNC_FREERUN;
		stVideo[id].stOUT_x.brightness = 128;			// set aux picture factor
		stVideo[id].stOUT_x.contrast = 128;
		stVideo[id].stOUT_x.saturation = 128;
		stVideo[id].stOUT_x.hue = 128;

	#if RGB_GAIN_OFFSET_TUNE == 1
		stVideo[id].stOUT_x.r_gain = 128;				// set aux gain/offset
		stVideo[id].stOUT_x.g_gain = 128;
		stVideo[id].stOUT_x.b_gain = 128;
		stVideo[id].stOUT_x.r_offset = 128;
		stVideo[id].stOUT_x.g_offset = 128;
		stVideo[id].stOUT_x.b_offset = 128;
	#endif

	// audio block
	//	stVideo[id].stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	//	stVideo[id].stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	//	stVideo[id].stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
	//							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	}

	// define video format of PORTA-INPUT
	stVideo[MDIN_CHIP_ID_A].stSRC_a.frmt = VIDSRC_1280x720p60;
	stVideo[MDIN_CHIP_ID_A].stSRC_a.mode = MDIN_SRC_MUX656_8;

	stVideo[MDIN_CHIP_ID_B].stSRC_a.frmt = VIDSRC_1280x720p60;
	stVideo[MDIN_CHIP_ID_B].stSRC_a.mode = MDIN_SRC_MUX656_8;

	stVideo[MDIN_CHIP_ID_C].stSRC_a.frmt = VIDSRC_1920x1080p60;
	stVideo[MDIN_CHIP_ID_C].stSRC_a.mode = MDIN_SRC_MUX656_10;

//	stVideo[MDIN_CHIP_ID_D].stSRC_a.frmt = VIDSRC_1920x1080p60;
//	stVideo[MDIN_CHIP_ID_D].stSRC_a.mode = MDIN_SRC_MUX656_10;

	// define video format of PORTB-INPUT
	stVideo[MDIN_CHIP_ID_A].stSRC_b.frmt = VIDSRC_1280x720p60;
	stVideo[MDIN_CHIP_ID_A].stSRC_b.mode = MDIN_SRC_MUX656_8;

	stVideo[MDIN_CHIP_ID_B].stSRC_b.frmt = VIDSRC_1280x720p60;
	stVideo[MDIN_CHIP_ID_B].stSRC_b.mode = MDIN_SRC_MUX656_8;

	stVideo[MDIN_CHIP_ID_C].stSRC_b.frmt = VIDSRC_1920x1080p60;
	stVideo[MDIN_CHIP_ID_C].stSRC_b.mode = MDIN_SRC_MUX656_10;

//	stVideo[MDIN_CHIP_ID_D].stSRC_b.frmt = VIDSRC_1280x720p60;
//	stVideo[MDIN_CHIP_ID_D].stSRC_b.mode = MDIN_SRC_MUX656_8;

	// define video format of MAIN-OUTPUT
	stVideo[MDIN_CHIP_ID_A].stOUT_m.frmt = VIDOUT_1920x1080p30;
	stVideo[MDIN_CHIP_ID_A].stOUT_m.mode = MDIN_OUT_MUX656_10;	//Chip A,B out : BT.656 10-bit

	stVideo[MDIN_CHIP_ID_B].stOUT_m.frmt = VIDOUT_1920x1080p30;
	stVideo[MDIN_CHIP_ID_B].stOUT_m.mode = MDIN_OUT_MUX656_10;	//Chip A,B out : BT.656 10-bit

	// MDIN C : out BT.1120 16-bit (or 20-bit) :
	// Check configuration!!!!! 	kukuri
	stVideo[MDIN_CHIP_ID_C].stOUT_m.frmt = VIDOUT_1920x1080p60;
	stVideo[MDIN_CHIP_ID_C].stOUT_m.mode = MDIN_OUT_RGB444_8;

	// define video format of AUX-OUTPUT
	// A,B : do NOT use AUX output
	// C,D ??
//	stVideo[MDIN_CHIP_ID_D].stOUT_x.frmt = VIDOUT_720x480i60;
//	stVideo[MDIN_CHIP_ID_D].stOUT_x.mode = MDIN_OUT_MUX656_8;

	// define video format of video encoder
//	stVideo[MDIN_CHIP_ID_D].encFRMT = VID_VENC_NTSC_M;

	// define video format of HDMI-OUTPUT
	// kukuri : only MDIN C uses HDMI output
	stVideo[MDIN_CHIP_ID_C].stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo[MDIN_CHIP_ID_C].stVID_h.fine  = HDMI_CLK_EDGE_RISE;
}

static void MDIN3xx_SetRegInitial(MDIN_VIDEO_INFO* pInfo)
{
	WORD nID = 0;
	MDIN_CHIP_ID_t chipId = pInfo->chipId;
	
	while (nID!=0x85) MDIN3xx_GetChipID(chipId, &nID);	// get chip-id

	MDIN3xx_EnableMainDisplay(chipId, OFF);		// set main display off
	MDIN3xx_SetMemoryConfig(chipId);			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(chipId, MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(chipId, MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(chipId, MDIN_IN_DATA24_MAP0);		// set in_data_map_mode
	if(chipId == MDIN_CHIP_ID_C)
	{
		MDIN3xx_SetDIGOutMapMode(chipId, MDIN_DIG_OUT_M_MAP2);		// disable digital out

		MDINOSD_SetBGLayerColor(RGB(128,128,128));			// set BG-Layer color
		MDINOSD_SetBGBoxColor(RGB(255,255,255));			// set BG-BOX color
	}
	else
	{
		MDIN3xx_SetDIGOutMapMode(chipId, MDIN_DIG_OUT_M_MAP0);		// disable digital out
	}

	// setup enhancement
	MDIN3xx_SetFrontNRFilterCoef(chipId, NULL);		// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(chipId, NULL);		// set default frontNR filter coef
	MDIN3xx_SetPeakingFilterCoef(chipId, NULL);		// set default peaking filter coef
	MDIN3xx_SetColorEnFilterCoef(chipId, NULL);		// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(chipId, NULL);		// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(chipId, NULL);		// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(chipId, NULL);	// set default colorton filter coef

	MDIN3xx_EnableLTI(chipId, OFF);					// set LTI off
	MDIN3xx_EnableCTI(chipId, OFF);					// set CTI off
	MDIN3xx_SetPeakingFilterLevel(chipId, 0);		// set peaking gain
	MDIN3xx_EnablePeakingFilter(chipId, ON);		// set peaking on
	MDIN3xx_EnableColorEnFilter(chipId, ON);		// set color enhancement on, 29Mar2012

	MDIN3xx_EnableFrontNRFilter(chipId, OFF);		// set frontNR off
	MDIN3xx_EnableBWExtension(chipId, OFF);			// set B/W extension off

	MDIN3xx_SetIPCBlock(chipId);		// initialize IPC block (3DNR gain is 37)

	//memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(pInfo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(pInfo, NULL);
	MDIN3xx_SetMFCVYFilterCoef(pInfo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(pInfo, NULL);

	// move to default setup
	MDINHTX_SetHDMIBlock(pInfo);		// initialize HDMI block

#if 0		// temporary blocked by kukuri
	// define video format of 4CH-display
	stVideo[chipId].st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo[chipId].st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo[chipId].st4CH_x.view  = MDIN_4CHVIEW_ALL;	 // set 4CH view mode
#endif

	stVideo[chipId].exeFLAG = (MDIN_UPDATE_MAINFMT | MDIN_UPDATE_AUXFMT);	// execution of video process

	MDIN3xx_VideoProcess(pInfo);			// mdin3xx main video process

	// define window for inter-area
//	stInterWND.lx = 315;
//	stInterWND.rx = 405;
//	stInterWND.ly = 90;
//	stInterWND.ry = 150;
//	MDIN3xx_SetDeintInterWND(chipId, &stInterWND, MDIN_INTER_BLOCK0);
//	MDIN3xx_EnableDeintInterWND(chipId, MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
//	InputSelOld = 0xff;

//	PrevSrcMainFrmt[chipId] = 0xff;
//	SrcMainFrmt[chipId] = stVideo[chipId].stSRC_a.frmt;
//	PrevSrcMainMode[chipId] = 0xff;
//	SrcMainMode[chipId] = stVideo[chipId].stSRC_a.mode;
//
//	PrevOutMainFrmt[chipId] = 0xff;
//	OutMainFrmt[chipId] = stVideo[chipId].stOUT_m.frmt;
//	PrevOutMainMode[chipId] = 0xff;
//	OutMainMode[chipId] = stVideo[chipId].stOUT_m.mode;
////	TempOutMainMode = OutMainMode;
//
//	PrevOutAuxFrmt[chipId] = 0xff;
//	OutAuxFrmt[chipId] = stVideo[chipId].stOUT_x.frmt;
//	PrevOutAuxMode[chipId] = 0xff;
//	OutAuxMode[chipId] = stVideo[chipId].stOUT_x.mode;
//	PrevEncFrmt[chipId] = 0xff;
//	EncVideoFrmt[chipId] = stVideo[chipId].encFRMT;

	videoInput = VIDEO_DIGITAL_NVP6158_A;
}

//--------------------------------------------------------------------------------------------------
static void InputSourceHandler(MDIN_VIDEO_INPUT_t src)
{
	if(fInputChanged == FALSE)  return;

	SetInVideoPath(src);

	// update source & out format of each MDIN
	stVideo[MDIN_CHIP_ID_A].stSRC_m.frmt = GetSrcMainFrmt(src, MDIN_CHIP_ID_A);
	stVideo[MDIN_CHIP_ID_A].stSRC_x.frmt = GetSrcAuxFrmt(src, MDIN_CHIP_ID_A);
	GetOutVideoFrameRate_A();		// get out-main format

	stVideo[MDIN_CHIP_ID_B].stSRC_m.frmt = GetSrcMainFrmt(src, MDIN_CHIP_ID_B);
	stVideo[MDIN_CHIP_ID_B].stSRC_x.frmt = GetSrcAuxFrmt(src, MDIN_CHIP_ID_B);
	GetOutVideoFrameRate_B();		// get out-main format

	stVideo[MDIN_CHIP_ID_C].stSRC_m.frmt = GetSrcMainFrmt(src, MDIN_CHIP_ID_C);
	stVideo[MDIN_CHIP_ID_C].stSRC_x.frmt = GetSrcAuxFrmt(src, MDIN_CHIP_ID_C);
	GetOutVideoFrameRate_C();		// get out-main format

	// source : main
	//SrcMainMode = MDIN_SRC_MUX656_8; // this is fixed (never changed)
//	SrcMainFrmt = GetSrcMainFrmt(src);
//	// source : aux (full screen : main out / Split(PIP) : ch2)
//	//SrcAuxMode = GetSrcAuxMode(src);
//	SrcAuxFrmt = GetSrcAuxFrmt(src);
//	// out format
//	stVideo[MDIN_CHIP_ID_A].stOUT_m.frmt = GetOutVideoFrameRate_A();		// get out-main format
//	stVideo[MDIN_CHIP_ID_B].stOUT_m.frmt = GetOutVideoFrameRate_B();
//	stVideo[MDIN_CHIP_ID_C].stOUT_m.frmt = GetOutVideoFrameRate_C();
////	stVideo[MDIN_CHIP_ID_D].stOUT_m.frmt = GetOutVideoFrameRate_D();
//	//OutMainMode = MDIN_OUT_RGB444_8;
//	OutAuxFrmt = GetOutAuxFormat(OutMainFrmt);	// get out-aux format
//	//OutAuxMode = GetOutAuxMode(src);

//	Set_DisplayWindow(GetCurrentDisplayMode());
	prevVideoInput = src;

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
		case VIDEO_DIGITAL_NVP6158_AB :
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
		case VIDEO_DIGITAL_NVP6158_AB:
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
static void SetIPCVideoFine(MDIN_CHIP_ID_t chipId)//, MDIN_VIDEO_INPUT_t src)
{
	MDINHIF_RegField(chipId, MDIN_LOCAL_ID, 0x256, 0, 8, 12);
	MDINHIF_RegField(chipId, MDIN_LOCAL_ID, 0x259, 0, 8,  4);
	MDINHIF_RegField(chipId, MDIN_LOCAL_ID, 0x25a, 8, 8,  1);
	MDINHIF_RegField(chipId, MDIN_LOCAL_ID, 0x25b, 0, 8,  8);
}

#if 0
//--------------------------------------------------------------------------------------------------
static PMDIN_AUXFILT_COEF GetAUXFilterCoef(void)
{
/*
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
*/
}

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
	//OSD_ModifyPalette_M((OutMainMode==MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	OSD_ModifyPalette_M(OSD_RGB_PALETTE);
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], OFF, OFF);
	//MDIN3xx_EnableAuxWithMainOSD(&stVideo, ON);
}

//--------------------------------------------------------------------------------------------------
//static void VideoFrameProcess(MDIN_VIDEO_INPUT_t src, MDIN_VIDEO_INFO* pInfo)
static void VideoFrameProcess(void)//(MDIN_VIDEO_INFO* pInfo)
{
	MDIN_CHIP_ID_t id;

	if(fInputChanged == FALSE) return;

	for(id = MDIN_CHIP_ID_A; id < MDIN_CHIP_ID_MAX; id++)
	{
		if(stVideo[id].encFRMT != PrevEncFrmt[id])
		{
	//		PrevSrcMainFrmt = 0xff;	//??
			PrevEncFrmt[id] = stVideo[id].encFRMT;
			stVideo[id].exeFLAG |= MDIN_UPDATE_ENCFMT;
		}

		if((stVideo[id].stSRC_a.frmt != PrevSrcMainFrmt[id]) ||
				(stVideo[id].stOUT_m.frmt != PrevOutMainFrmt[id]))
		{
			PrevSrcMainFrmt[id] = stVideo[id].stSRC_a.frmt;
			PrevOutMainFrmt[id] = stVideo[id].stOUT_m.frmt;
			stVideo[id].exeFLAG |= MDIN_UPDATE_MAINFMT;
		}

		if((stVideo[id].stSRC_x.frmt != PrevSrcAuxFrmt[id]) ||
				(stVideo[id].stOUT_x.frmt != PrevOutAuxFrmt[id]))
		{
			PrevSrcAuxFrmt[id] = stVideo[id].stSRC_x.frmt;
			PrevOutAuxFrmt[id] = stVideo[id].stOUT_x.frmt;
			stVideo[id].exeFLAG |= MDIN_UPDATE_AUXFMT;
		}

		if (stVideo[id].exeFLAG!=MDIN_UPDATE_CLEAR) // updated video formats
		{
			stVideo[id].stIPC_m.fine &= ~MDIN_DEINT_3DNR_ON;   //3DNR off

	//		if (stVideo[id].srcPATH == PATH_MAIN_B_AUX_B || stVideo[id].srcPATH == PATH_MAIN_B_AUX_A || stVideo[id].srcPATH == PATH_MAIN_B_AUX_M)
	//		{
	//			stVideo[id].stSRC_b.frmt = SrcMainFrmt; stVideo[id].stSRC_b.mode = SrcMainMode;
	//			stVideo[id].stSRC_a.frmt = SrcAuxFrmt; stVideo[id].stSRC_a.mode = SrcAuxMode;
	//		}
	//		else
	//		{
	//			stVideo[id].stSRC_a.frmt = SrcMainFrmt; stVideo[id].stSRC_a.mode = SrcMainMode;
	//			stVideo[id].stSRC_b.frmt = SrcAuxFrmt; stVideo[id].stSRC_b.mode = SrcAuxMode;
	//		}
	//
	//		stVideo[id].stOUT_m.frmt = OutMainFrmt; stVideo[id].stOUT_m.mode = OutMainMode;
	//		stVideo[id].stOUT_x.frmt = OutAuxFrmt; stVideo[id].stOUT_x.mode = OutAuxMode;

			//Set main & aux window scale, crop, zoom
			//memcpy(&stVideo[id].stCROP_m, &stMainCROP, sizeof(MDIN_VIDEO_WINDOW));
			//memcpy(&stVideo[id].stCROP_x, &stAuxCROP, sizeof(MDIN_VIDEO_WINDOW));
			//memcpy(&stVideo[id].stVIEW_m, &stMainVIEW, sizeof(MDIN_VIDEO_WINDOW));
			//memcpy(&stVideo[id].stVIEW_x, &stAuxVIEW, sizeof(MDIN_VIDEO_WINDOW));

			//MDIN3xx_SetScaleProcess(&stVideo);

			MDIN3xx_EnableAuxDisplay(&stVideo[id], OFF);
			MDIN3xx_EnableMainDisplay(id, OFF);

			//SetOffChipFrmt(src);		// set InA offchip format
			//SetSrcMainFine(src);		// set source video fine (fldid, offset)

	//		if (OutMainFrmt!=PrevOutMainFrmt) {
	//			stVideo[id].pHY_m		= 	NULL;		// restore MFCHY from API
	//			stVideo[id].pHC_m		= 	NULL;		// restore MFCHY from API
	//			stVideo[id].pVY_m		= 	NULL;		// restore MFCHY from API
	//			stVideo[id].pVC_m		= 	NULL;		// restore MFCHY from API
	//		}
			MDIN3xx_VideoProcess(&stVideo[id]);		// mdin3xx main video process
			//MDINAUX_VideoProcess(&stVideo[id]);

			SetIPCVideoFine(id);//, src);	// tune IPC-register (CVBS or HDMI)
			//SetAUXVideoFilter();	// tune AUX-filter (DUAL or CVBS)

			MDIN3xx_EnableAuxDisplay(&stVideo[id], ON);
			MDIN3xx_EnableMainDisplay(id, ON);

			//SetOSDMenuRefresh();

	//		PrevSrcMainFrmt = SrcMainFrmt;	PrevSrcMainMode = SrcMainMode;
	//		PrevOutMainFrmt = OutMainFrmt;	PrevOutMainMode = OutMainMode;
	//
	//		PrevSrcAuxFrmt = SrcAuxFrmt;	PrevSrcAuxMode = SrcAuxMode;
	//		PrevOutAuxFrmt = OutAuxFrmt;	PrevOutAuxMode = OutAuxMode;
		}
	}
}


// ----------------------------------------------------------------------
// Exported functions
// ----------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void CreateVideoInstance(void)
{
	MDIN_CHIP_ID_t id;

	memset(PrevSrcMainFrmt, 0xFF, sizeof(PrevSrcMainFrmt));
//	memset(PrevSrcMainMode, 0xFF, sizeof(PrevSrcMainFrmt));
	memset(PrevSrcAuxFrmt, 0xFF, sizeof(PrevSrcMainFrmt));
//	memset(PrevSrcAuxMode, 0xFF, sizeof(PrevSrcMainFrmt));
	memset(PrevOutMainFrmt, 0xFF, sizeof(PrevOutMainFrmt));
//	memset(PrevOutMainMode, 0xFF, sizeof(PrevSrcMainFrmt));
	memset(PrevOutAuxFrmt, 0xFF, sizeof(PrevSrcMainFrmt));
//	memset(PrevOutAuxMode, 0xFF, sizeof(PrevSrcMainFrmt));
	memset(PrevEncFrmt, 0xFF, sizeof(PrevSrcMainFrmt));

	for(id = MDIN_CHIP_ID_A; id < MDIN_CHIP_ID_MAX; id++)
	{
		Video_DefaultSetup();
		MDIN3xx_SetRegInitial(&stVideo[id]);
	}
}
//
void InitInputSource(void)
{
	prevVideoInput = 0xFF;
	videoInput = 0xFF;
}

//--------------------------------------------------------------------------------------------------
void SetInputSource(BYTE input)
{
	videoInput = input;
//	fInputChanged = TRUE;
}

void SetInputChanged(void)
{
	fInputChanged = TRUE;
}
//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
	InputSourceHandler(videoInput);
//	InputSyncHandler_A(InputSelect);
//	InputSyncHandler_B(InputSelect);		  //by hungry 2012.02.27

	VideoFrameProcess();//InputSelect);
	SetOSDMenuRefresh();
	fInputChanged = FALSE;
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	MDINHTX_CtrlHandler(&stVideo[MDIN_CHIP_ID_C]);
}

//void Request2VideoProcess(void)
//{
////	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT | MDIN_UPDATE_AUXFMT;
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

//	switch(SrcMainFrmt)
//	{
//		case VIDSRC_1920x1080p60:
//		case VIDSRC_1920x1080p50:
//			mainWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
//			mainHeight = DISPLAY_HEIGHT_1920x1080;
//			break;
//
//		case VIDSRC_1280x720p60:
//		case VIDSRC_1280x720p50:
//			mainWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
//			mainHeight = DISPLAY_HEIGHT_1280x720;
//			break;
//
//		case VIDSRC_960x480i60:
//			mainWidth = DISPLAY_WIDTH_960*2;
//			mainHeight = DISPLAY_HEIGHT_480/2;
//			break;
//
//		case VIDSRC_960x576i50:
//			mainWidth = DISPLAY_WIDTH_960*2;
//			mainHeight = DISPLAY_HEIGHT_576/2;
//			break;
//	}
//
//	switch(SrcAuxFrmt)
//	{
//		case VIDSRC_1920x1080p60:
//		case VIDSRC_1920x1080p50:
//			auxWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
//			auxHeight = DISPLAY_HEIGHT_1920x1080;
//			break;
//
//		case VIDSRC_1280x720p60:
//		case VIDSRC_1280x720p50:
//			auxWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
//			auxHeight = DISPLAY_HEIGHT_1280x720;
//			break;
//
//		case VIDSRC_960x480i60:
//			auxWidth = DISPLAY_WIDTH_960 * 2;
//			auxHeight = DISPLAY_HEIGHT_480/2;
//			break;
//
//		case VIDSRC_960x576i50:
//			auxWidth = DISPLAY_WIDTH_960 * 2;
//			auxHeight = DISPLAY_HEIGHT_576/2;
//			break;
//	}
/*
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
			auxHeight = DISPLAY_HEIGHT_1280x720;
		}
	}
*/
	stMainCROP.w = mainWidth;
	stMainCROP.h = mainHeight;
	stMainCROP.x = 0;
	stMainCROP.y = 0;

	stAuxCROP.w = auxWidth;
	stAuxCROP.h = auxHeight;
	stAuxCROP.x = 0;
	stAuxCROP.y = 0;
/*
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
	}
	
	MDIN3xx_SetVideoWindowCROP(&stVideo, stMainCROP);
	MDIN3xx_SetVideoWindowVIEW(&stVideo, stMainVIEW);
	MDINAUX_SetVideoWindowCROP(&stVideo, stAuxCROP);
	MDINAUX_SetVideoWindowVIEW(&stVideo, stAuxVIEW);
*/
}

#endif	/* defined(SYSTEM_USE_MDIN380) */

/*  FILE_END_HERE */
