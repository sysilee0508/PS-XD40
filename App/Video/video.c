// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
#include "raptor4_fmt.h"
#endif

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

#define COMPENSATION_MARGIN					10

#define DUMP_REG								0

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

MDIN_VIDEO_INFO		stVideo[MDIN_ID_MAX];

MDIN_INTER_WINDOW	stInterWND;

MDIN_VIDEO_INPUT_t InputSelect, InputSelOld;
MDIN_SRCVIDEO_PATH_t SrcPath[MDIN_ID_MAX];
static MDIN_SRCVIDEO_FORMAT_t SrcMainFrmt[MDIN_ID_MAX], PrevSrcMainFrmt[MDIN_ID_MAX];
static MDIN_OUTVIDEO_FORMAT_t OutMainFrmt[MDIN_ID_MAX], PrevOutMainFrmt[MDIN_ID_MAX];
static MDIN_SRCVIDEO_FORMAT_t SrcAuxFrmt[MDIN_ID_MAX], PrevSrcAuxFrmt[MDIN_ID_MAX];
BOOL fInputChanged;

#if DUMP_REG
BOOL fRegDump = FALSE;
#endif

MDIN_VIDEO_WINDOW stMainVIEW[MDIN_ID_MAX], stAuxVIEW[MDIN_ID_MAX];
MDIN_VIDEO_WINDOW stMainCROP[MDIN_ID_MAX], stAuxCROP[MDIN_ID_MAX];

static MDIN_OUTVIDEO_FORMAT_t outVideoResolution = VIDOUT_1920x1080p60;
static BOOL fUpdatedOutResolution;
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
void ConfigI2C(MDIN_CHIP_ID_t mdin)
{
	switch(mdin)
	{
		case MDIN_ID_A:
			I2C_SET_CHANNEL(I2C_MAIN);
			SELECT_MDIN(MDIN_I2C_L);
			break;
			
		case MDIN_ID_B:
			I2C_SET_CHANNEL(I2C_MAIN);
			SELECT_MDIN(MDIN_I2C_H);
			break;
			
		case MDIN_ID_C:
			I2C_SET_CHANNEL(I2C_SUB);
			SELECT_MDIN(MDIN_I2C_L);
			break;
			
		case MDIN_ID_D:
			I2C_SET_CHANNEL(I2C_SUB);
			SELECT_MDIN(MDIN_I2C_H);
			break;

		default:
			break;
	}
}

MDIN_OUTVIDEO_FORMAT_t GetOutVideoFormat(MDIN_CHIP_ID_t mdin)
{
	return OutMainFrmt[mdin];
}

static MDIN_SRCVIDEO_FORMAT_t GetInSourceFormat(eChannel_t channel)
{
	static MDIN_SRCVIDEO_FORMAT_t format[NUM_OF_CHANNEL] = {VIDSRC_1280x720p60, VIDSRC_1280x720p60};

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
			break;
	}

	return format[channel];
}


//--------------------------------------------------------------------------------------------------
static void SetIPCVideoFine(void)
{
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x256, 0, 8, 12);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x259, 0, 8,  4);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x25a, 8, 8,  1);
	MDINHIF_RegField(MDIN_LOCAL_ID, 0x25b, 0, 8,  8);
}

static void MDIN3xx_SetRegInitial_AB(MDIN_CHIP_ID_t mdin)
{
	WORD nID = 0;

	if((mdin != MDIN_ID_A) && (mdin != MDIN_ID_B))
	{
		while(1);
	}
	else
	{
		M380_ID = mdin;
	}


	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id

	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig(mdin);			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(mdin, MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP0);		// set in_data_map_mode
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP5);		// disable digital out

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

	memset((PBYTE)&stVideo[M380_ID], 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo[M380_ID], NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo[M380_ID], NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo[M380_ID], NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo[M380_ID], NULL);

	stVideo[M380_ID].chipID = mdin;

	// set aux display ON
	stVideo[M380_ID].dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo[M380_ID].srcPATH = PATH_MAIN_A_AUX_B;	// set main is A, aux is main out
	stVideo[M380_ID].dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo[M380_ID].encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo[M380_ID].stSRC_a.frmt = VIDSRC_1280x720p60;
	stVideo[M380_ID].stSRC_a.mode = MDIN_SRC_MUX656_8;
	//stVideo[M380_ID].stSRC_a.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo[M380_ID].stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	stVideo[M380_ID].stSRC_a.offH = 0;
	stVideo[M380_ID].stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo[M380_ID].stSRC_b.frmt = VIDSRC_1280x720p60;
	stVideo[M380_ID].stSRC_b.mode = MDIN_SRC_MUX656_8;
	//stVideo_A.stSRC_b.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo[M380_ID].stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
	stVideo[M380_ID].stSRC_b.offH = 0;
	stVideo[M380_ID].stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo[M380_ID].stOUT_m.frmt = VIDOUT_1920x1080pRB; // kukuri  //VIDOUT_1280x720p60;
	stVideo[M380_ID].stOUT_m.mode = MDIN_OUT_EMB422_8 ; // MDIN_OUT_MUX656_8;	 //by hungry 2012.03.06		// test by chungsa
	stVideo[M380_ID].stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo[M380_ID].stOUT_m.brightness = 128;			// set main picture factor
	stVideo[M380_ID].stOUT_m.contrast = 128;
	stVideo[M380_ID].stOUT_m.saturation = 128;
	stVideo[M380_ID].stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo[M380_ID].stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo[M380_ID].stOUT_m.g_gain = 128;
	stVideo[M380_ID].stOUT_m.b_gain = 128;
	stVideo[M380_ID].stOUT_m.r_offset = 128;
	stVideo[M380_ID].stOUT_m.g_offset = 128;
	stVideo[M380_ID].stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo[M380_ID].stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo[M380_ID].stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo[M380_ID].stIPC_m.gain = 34;
	stVideo[M380_ID].stIPC_m.fine = MDIN_DEINT_3DNR_OFF | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	stVideo[M380_ID].stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_OFF;	// when MDIN_DEINT_3DNR_OFF
	
	// define video format of AUX-INPUT
	stVideo[M380_ID].stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24
	// define video format of AUX-OUTPUT (CVBS output)
	stVideo[M380_ID].stOUT_x.frmt = VIDOUT_1920x1080pRB;
	stVideo[M380_ID].stOUT_x.mode = MDIN_OUT_MUX656_8;
	stVideo[M380_ID].stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run

	stVideo[M380_ID].stOUT_x.brightness = 128;			// set aux picture factor
	stVideo[M380_ID].stOUT_x.contrast = 128;
	stVideo[M380_ID].stOUT_x.saturation = 128;
	stVideo[M380_ID].stOUT_x.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo[M380_ID].stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo[M380_ID].stOUT_x.g_gain = 128;
	stVideo[M380_ID].stOUT_x.b_gain = 128;
	stVideo[M380_ID].stOUT_x.r_offset = 128;
	stVideo[M380_ID].stOUT_x.g_offset = 128;
	stVideo[M380_ID].stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo[M380_ID].encFRMT = VID_VENC_AUTO;

	stVideo[M380_ID].exeFLAG = MDIN_UPDATE_ALL;	// execution of video process
	MDIN3xx_VideoInProcess(&stVideo[M380_ID]);
	MDIN3xx_VideoProcess(&stVideo[M380_ID]);                            // mdin3xx main video process
	MDINAUX_VideoProcess(&stVideo[M380_ID]);             // mdin3xx aux video process

	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
	PrevSrcMainFrmt[mdin] = 0xff;
	SrcMainFrmt[mdin] = stVideo[M380_ID].stSRC_a.frmt;
	PrevOutMainFrmt[mdin] = 0xff;
	OutMainFrmt[mdin] = stVideo[M380_ID].stOUT_m.frmt;
}

static void MDIN3xx_SetRegInitial_C(void)
{
	WORD nID = 0;
	eOutVideoFormat_t video;

	Read_NvItem_AuxVideoFormat(&video);

	M380_ID = MDIN_ID_C;
	
	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id

	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig(MDIN_ID_C);			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_ID_C, MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA36_MAP1); //MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP0);		// set in_data_map_mode
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP7);	//MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP2);		// disable digital out
	MDINOSD_SetBGLayerColor(GRAY(GetCurrentColorFormat()));			// set BG-Layer color

	MDINOSD_SetBGBoxColor(WHITE(GetCurrentColorFormat()));			// set BG-BOX color

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

	memset((PBYTE)&stVideo[M380_ID], 0, sizeof(MDIN_VIDEO_INFO));
	
	MDIN3xx_SetMFCHYFilterCoef(&stVideo[M380_ID], NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo[M380_ID], NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo[M380_ID], NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo[M380_ID], NULL);

	stVideo[M380_ID].chipID = MDIN_ID_C;
	
	// set aux display ON
	stVideo[M380_ID].dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo[M380_ID].srcPATH = PATH_MAIN_A_AUX_B;	// set main is A, aux is main out
	stVideo[M380_ID].dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo[M380_ID].encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo[M380_ID].stSRC_a.frmt = VIDSRC_1920x1080p60;
	stVideo[M380_ID].stSRC_a.mode = MDIN_SRC_EMB422_8; //MDIN_SRC_MUX656_8;
	stVideo[M380_ID].stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	stVideo[M380_ID].stSRC_a.offH = 0;
	stVideo[M380_ID].stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo[M380_ID].stSRC_b.frmt = VIDSRC_1280x720p60;
	stVideo[M380_ID].stSRC_b.mode = MDIN_SRC_EMB422_8; //MDIN_SRC_MUX656_8;
	stVideo[M380_ID].stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
	stVideo[M380_ID].stSRC_b.offH = 0;
	stVideo[M380_ID].stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo[M380_ID].stOUT_m.frmt = VIDOUT_1920x1080p60;
	stVideo[M380_ID].stOUT_m.mode = (video == VIDEO_VGA)? MDIN_OUT_RGB444_8 : MDIN_OUT_EMB422_8;	 //by hungry 2012.03.06		// test by chungsa
	stVideo[M380_ID].stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo[M380_ID].stOUT_m.brightness = 128;			// set main picture factor
	stVideo[M380_ID].stOUT_m.contrast = 128;
	stVideo[M380_ID].stOUT_m.saturation = 128;
	stVideo[M380_ID].stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo[M380_ID].stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo[M380_ID].stOUT_m.g_gain = 128;
	stVideo[M380_ID].stOUT_m.b_gain = 128;
	stVideo[M380_ID].stOUT_m.r_offset = 128;
	stVideo[M380_ID].stOUT_m.g_offset = 128;
	stVideo[M380_ID].stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo[M380_ID].stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo[M380_ID].stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo[M380_ID].stIPC_m.gain = 34;
	stVideo[M380_ID].stIPC_m.fine = MDIN_DEINT_3DNR_OFF | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	stVideo[M380_ID].stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_OFF;	// when MDIN_DEINT_3DNR_ON
	
	// define video format of AUX-INPUT
	stVideo[M380_ID].stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24
	#if 1
	// define video format of AUX-OUTPUT (CVBS output)
	stVideo[M380_ID].stOUT_x.frmt = VIDOUT_1920x1080p60;
	stVideo[M380_ID].stOUT_x.mode = (video == VIDEO_VGA)? MDIN_OUT_RGB444_8 : MDIN_OUT_EMB422_8;;
	stVideo[M380_ID].stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run
	stVideo[M380_ID].stOUT_x.brightness = 128;			// set aux picture factor
	stVideo[M380_ID].stOUT_x.contrast = 128;
	stVideo[M380_ID].stOUT_x.saturation = 128;
	stVideo[M380_ID].stOUT_x.hue = 128;
	#endif
#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo[M380_ID].stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo[M380_ID].stOUT_x.g_gain = 128;
	stVideo[M380_ID].stOUT_x.b_gain = 128;
	stVideo[M380_ID].stOUT_x.r_offset = 128;
	stVideo[M380_ID].stOUT_x.g_offset = 128;
	stVideo[M380_ID].stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo[M380_ID].encFRMT = VID_VENC_AUTO;
	// define video format of HDMI-OUTPUT
	stVideo[M380_ID].stVID_h.mode  =HDMI_OUT_RGB444_8;
	stVideo[M380_ID].stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	MDINHTX_SetHDMIBlock(&stVideo[M380_ID]);		// initialize HDMI block

	stVideo[M380_ID].exeFLAG = MDIN_UPDATE_ALL;	// execution of video process
	MDIN3xx_VideoInProcess(&stVideo[M380_ID]);
	MDIN3xx_VideoProcess(&stVideo[M380_ID]);                            // mdin3xx main video process
	MDINAUX_VideoProcess(&stVideo[M380_ID]);             // mdin3xx aux video process

	// define window for inter-area (PIP window? kukuri)
	stInterWND.lx = 315;
	stInterWND.rx = 405;
	stInterWND.ly = 90;
	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	// define variable for EDK application
	PrevSrcMainFrmt[MDIN_ID_C]= 0xff;
	SrcMainFrmt[MDIN_ID_C] = stVideo[M380_ID].stSRC_a.frmt;
	PrevOutMainFrmt[MDIN_ID_C] = 0xff;
	OutMainFrmt[MDIN_ID_C] = stVideo[M380_ID].stOUT_m.frmt;
}

static void MDIN3xx_SetRegInitial_D(void)
{
	BYTE retry = 0;
	WORD nID = 0;

	M380_ID = MDIN_ID_D;

	do
	{
		MDIN3xx_GetChipID(&nID);	// get chip-id
		if(nID != 0x85)
		{
			retry++;
		}
	} while((retry <= 3) && (nID != 0x85));

	if(retry > 3)	return;

//	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig(MDIN_ID_D);			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_ID_D, MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP0);		// set in_data_map_mode
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP5);		// disable digital out

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

	memset((PBYTE)&stVideo[M380_ID], 0, sizeof(MDIN_VIDEO_INFO));
	
	MDIN3xx_SetMFCHYFilterCoef(&stVideo[M380_ID], NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo[M380_ID], NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo[M380_ID], NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo[M380_ID], NULL);

	stVideo[M380_ID].chipID = MDIN_ID_D;
	
	// set aux display ON
	stVideo[M380_ID].dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path
	stVideo[M380_ID].srcPATH = PATH_MAIN_A_AUX_A;	// set main is A, aux is main out
	stVideo[M380_ID].dacPATH = DAC_PATH_MAIN_OUT;	// set main only
	stVideo[M380_ID].encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo[M380_ID].stSRC_a.frmt = VIDSRC_1920x1080p60;
	stVideo[M380_ID].stSRC_a.mode = MDIN_SRC_EMB422_8;
	//stVideo[M380_ID].stSRC_a.fine = MDIN_FIELDID_BYPASS | MDIN_LOW_IS_TOPFLD;
	stVideo[M380_ID].stSRC_a.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2012.02.27
	stVideo[M380_ID].stSRC_a.offH = 0;
	stVideo[M380_ID].stSRC_a.offV = 0;

	// define video format of PORTB-INPUT
	stVideo[M380_ID].stSRC_b.frmt = VIDSRC_1920x1080p60;
	stVideo[M380_ID].stSRC_b.mode = MDIN_SRC_EMB422_8;
	stVideo[M380_ID].stSRC_b.fine = MDIN_CbCrSWAP_OFF|MDIN_FIELDID_INPUT|MDIN_LOW_IS_TOPFLD; //by hungry 2013.04.23
	stVideo[M380_ID].stSRC_b.offH = 0;
	stVideo[M380_ID].stSRC_b.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo[M380_ID].stOUT_m.frmt = VIDOUT_1920x1080p60;
	stVideo[M380_ID].stOUT_m.mode = MDIN_OUT_RGB444_8;	 //by hungry 2012.03.06		// test by chungsa
	stVideo[M380_ID].stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo[M380_ID].stOUT_m.brightness = 128;			// set main picture factor
	stVideo[M380_ID].stOUT_m.contrast = 128;
	stVideo[M380_ID].stOUT_m.saturation = 128;
	stVideo[M380_ID].stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo[M380_ID].stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo[M380_ID].stOUT_m.g_gain = 128;
	stVideo[M380_ID].stOUT_m.b_gain = 128;
	stVideo[M380_ID].stOUT_m.r_offset = 128;
	stVideo[M380_ID].stOUT_m.g_offset = 128;
	stVideo[M380_ID].stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo[M380_ID].stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo[M380_ID].stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo[M380_ID].stIPC_m.gain = 34;
	stVideo[M380_ID].stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;

	// define map of frame buffer
	stVideo[M380_ID].stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_ON

	// define video format of AUX-INPUT
	stVideo[M380_ID].stSRC_x.fine = MDIN_CbCrSWAP_OFF;		//by hungry 2012.02.24
	// define video format of AUX-OUTPUT (CVBS output)
	stVideo[M380_ID].stOUT_x.frmt = VIDOUT_720x480i60;
	stVideo[M380_ID].stOUT_x.mode = MDIN_OUT_MUX656_8;
	stVideo[M380_ID].stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run
	stVideo[M380_ID].stOUT_x.brightness = 128;			// set aux picture factor
	stVideo[M380_ID].stOUT_x.contrast = 128;
	stVideo[M380_ID].stOUT_x.saturation = 128;
	stVideo[M380_ID].stOUT_x.hue = 128;
#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo[M380_ID].stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo[M380_ID].stOUT_x.g_gain = 128;
	stVideo[M380_ID].stOUT_x.b_gain = 128;
	stVideo[M380_ID].stOUT_x.r_offset = 128;
	stVideo[M380_ID].stOUT_x.g_offset = 128;
	stVideo[M380_ID].stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo[M380_ID].encFRMT = VID_VENC_NTSC_M;	

	stVideo[M380_ID].exeFLAG = MDIN_UPDATE_ALL;	// execution of video process

	MDIN3xx_VideoInProcess(&stVideo[M380_ID]);
	MDIN3xx_VideoProcess(&stVideo[M380_ID]);                            // mdin3xx main video process
	MDINAUX_VideoProcess(&stVideo[M380_ID]);             // mdin3xx aux video process

	//MDIN3xx_EnableMainDisplay(ON);
	MDIN3xx_EnableAuxFreeze(&stVideo[M380_ID], OFF);
	MDIN3xx_EnableAuxDisplay(&stVideo[M380_ID], ON);

}

//--------------------------------------------------------------------------------------------------
// Set Input Video Path
//--------------------------------------------------------------------------------------------------
static void SetInputVideoPath(MDIN_VIDEO_INPUT_t src, eDisplayMode_t mode)
{	
	if(fInputChanged==TRUE)
	{
		switch (src)
		{
			case VIDEO_DIGITAL_NVP6158_A:	// use only mdin_a
				if(mode == DISPLAY_MODE_FULL_CH1)
				{
					stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_A_AUX_M;
					stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_OUT;
				}
				else if(mode == DISPLAY_MODE_FULL_CH2)
				{
					stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_B_AUX_M;
					stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_OUT;
				}
				else
				{
					stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
					stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_PIP;
				}
				stVideo[MDIN_ID_C].srcPATH = PATH_MAIN_A_AUX_M;
				stVideo[MDIN_ID_C].dacPATH = DAC_PATH_MAIN_OUT;
				break;

			case VIDEO_DIGITAL_NVP6158_B:	// use only mdin_b
				if(mode == DISPLAY_MODE_FULL_CH3)
				{
					stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_A_AUX_M;
					stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
				}
				else if(mode == DISPLAY_MODE_FULL_CH4)
				{
					stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_B_AUX_M;
					stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
				}
				else
				{
					stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
					stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_PIP;
				}
				stVideo[MDIN_ID_C].srcPATH = PATH_MAIN_B_AUX_M;
				stVideo[MDIN_ID_C].dacPATH = DAC_PATH_MAIN_OUT;
				break;

			case VIDEO_DIGITAL_NVP6158_AB:	// use mdin_a & mdin_b
				switch(mode)
				{
					case DISPLAY_MODE_2SPLIT_HSCALE_A :
					case DISPLAY_MODE_2SPLIT_HCROP_A :
					case DISPLAY_MODE_2SPLIT_VSCALE_A :
					case DISPLAY_MODE_2SPLIT_VCROP_A:
						stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
						stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_OUT;
						
						stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
						stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
						break;
						
					case DISPLAY_MODE_2SPLIT_HSCALE_B:	
					case DISPLAY_MODE_2SPLIT_HCROP_B:
					case DISPLAY_MODE_2SPLIT_VSCALE_B:
					case DISPLAY_MODE_2SPLIT_VCROP_B:
						stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_B_AUX_A;
						stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_OUT;
						
						stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_B_AUX_A;
						stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
						break;

					case DISPLAY_MODE_3SPLIT_R2SCALE:
					case DISPLAY_MODE_3SPLIT_R2CROP:
					case DISPLAY_MODE_3SPLIT_L2SCALE:
					case DISPLAY_MODE_3SPLIT_L2CROP:
					case DISPLAY_MODE_3SPLIT_D2SCALE:
					case DISPLAY_MODE_3SPLIT_D2CROP:
					case DISPLAY_MODE_3SPLIT_U2SCALE:
					case DISPLAY_MODE_3SPLIT_U2CROP:
						stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
						stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_PIP;
						
						stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_A_AUX_M;
						stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_OUT;
						break;

					default:	// 4 splits
						stVideo[MDIN_ID_A].srcPATH = PATH_MAIN_A_AUX_B;
						stVideo[MDIN_ID_A].dacPATH = DAC_PATH_MAIN_PIP;
						
						stVideo[MDIN_ID_B].srcPATH = PATH_MAIN_A_AUX_B;
						stVideo[MDIN_ID_B].dacPATH = DAC_PATH_MAIN_PIP;
						break;

				}

				if((mode == DISPLAY_MODE_4SPLIT_H) || (mode == DISPLAY_MODE_4SPLIT_X))
				{
					stVideo[MDIN_ID_C].srcPATH = PATH_MAIN_B_AUX_A;
				}
				else
				{
					stVideo[MDIN_ID_C].srcPATH = PATH_MAIN_A_AUX_B;
				}
				stVideo[MDIN_ID_C].dacPATH = DAC_PATH_MAIN_PIP;
				break;
		}
	}
}

//--------------------------------------------------------------------------------------------------
// Config main&aux source, main out video format for all MDINs
static void ConfigVideoFrmt(MDIN_VIDEO_INPUT_t src)
{
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	MDIN_SRCVIDEO_FORMAT_t srcFrmt[NUM_OF_CHANNEL];
	eChannel_t channel;
	MDIN_CHIP_ID_t mdin;
	eResolution_t outRes;
	
	static MDIN_OUTVIDEO_FORMAT_t preOutAuxFrmt = VIDOUT_720x480i60;
	static MDIN_VENC_FORMAT_t preEncFrmt = VID_VENC_NTSC_M;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		srcFrmt[channel] = GetInSourceFormat(channel);
	}

	Read_NvItem_Resolution(&outRes);

	switch(src)
	{
		case VIDEO_DIGITAL_NVP6158_A:
			SrcMainFrmt[MDIN_ID_A] = srcFrmt[FindMainChannel(displayMode, MDIN_ID_A)];
			SrcAuxFrmt[MDIN_ID_A] = srcFrmt[FindAuxChannel(displayMode, MDIN_ID_A)];
			
			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				if(IS_PIP_MODE(displayMode) == TRUE) 
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				}
				else
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1920x1080pRB;// VIDOUT_1280x720p60;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1920x1080p60; //VIDSRC_1280x720p60;
				}
				
				if(outRes == RESOLUTION_1920_1080_60P)
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
				}
				else
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p30;
				}
			}
			else
			{
				if(IS_PIP_MODE(displayMode) == TRUE)
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				}
				else
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1920x1080pRB2; // VIDOUT_1280x720p50;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1920x1080p50; // VIDSRC_1280x720p50;
				}
				if(outRes == RESOLUTION_1920_1080_60P)
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
				}
				else
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p25;
				}
			}
			break;

		case VIDEO_DIGITAL_NVP6158_B:
			SrcMainFrmt[MDIN_ID_B] = srcFrmt[FindMainChannel(displayMode, MDIN_ID_B)];
			SrcAuxFrmt[MDIN_ID_B] = srcFrmt[FindAuxChannel(displayMode, MDIN_ID_B)];
			
			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1920x1080pRB;// VIDOUT_1280x720p60;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1920x1080p60; //VIDSRC_1280x720p60;
				//OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
				if(outRes == RESOLUTION_1920_1080_60P)
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
				}
				else
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p30;
				}

			}
			else
			{
				OutMainFrmt[MDIN_ID_B] = VIDOUT_1920x1080pRB2; // VIDOUT_1280x720p50;
				SrcMainFrmt[MDIN_ID_C] = VIDSRC_1920x1080p50; // VIDSRC_1280x720p50;
				if(outRes == RESOLUTION_1920_1080_60P)
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
				}
				else
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p25;
				}
			}
			break;

		case VIDEO_DIGITAL_NVP6158_AB:
			SrcMainFrmt[MDIN_ID_A] = srcFrmt[FindMainChannel(displayMode, MDIN_ID_A)];
			SrcAuxFrmt[MDIN_ID_A] = srcFrmt[FindAuxChannel(displayMode, MDIN_ID_A)];
			SrcMainFrmt[MDIN_ID_B] = srcFrmt[FindMainChannel(displayMode, MDIN_ID_B)];
			SrcAuxFrmt[MDIN_ID_B] = srcFrmt[FindAuxChannel(displayMode, MDIN_ID_B)];

			if((SrcMainFrmt[MDIN_ID_A] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_A] == VIDSRC_1280x720p60))
			{
				if((displayMode == DISPLAY_MODE_4SPLIT_H) ||(displayMode == DISPLAY_MODE_4SPLIT_X) )
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
					SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				}
				else if((displayMode == DISPLAY_MODE_4SPLIT_R3SCALE)  || (displayMode == DISPLAY_MODE_4SPLIT_R3CROP))
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p60;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				}
				else
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1920x1080pRB;//VIDOUT_1280x720p60;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1920x1080p60; //VIDSRC_1280x720p60;
				}
				//OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
				if(outRes == RESOLUTION_1920_1080_60P)
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
				}
				else
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p30;
				}
			}
			else
			{
				
				if((displayMode == DISPLAY_MODE_4SPLIT_H) ||(displayMode == DISPLAY_MODE_4SPLIT_X) )
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				}
				else if((displayMode == DISPLAY_MODE_4SPLIT_R3SCALE)  || (displayMode == DISPLAY_MODE_4SPLIT_R3CROP))
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1280x720p50;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				}
				else
				{
					OutMainFrmt[MDIN_ID_A] = VIDOUT_1920x1080pRB2;// VIDOUT_1280x720p50;
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1920x1080p50; //VIDSRC_1280x720p50;
				}
				//OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p50;
				if(outRes == RESOLUTION_1920_1080_60P)
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p60;
				}
				else
				{
					OutMainFrmt[MDIN_ID_C] = VIDOUT_1920x1080p30;
				}
			}

			if((SrcMainFrmt[MDIN_ID_B] == VIDSRC_960x480i60) || 
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1920x1080p60) ||
				(SrcMainFrmt[MDIN_ID_B] == VIDSRC_1280x720p60))
			{
				if(IS_2SPLIT_MODE(displayMode) == TRUE)
				{
					OutMainFrmt[MDIN_ID_B] = VIDOUT_1920x1080pRB;
				}
				else
				{
					OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p60;
				}
				
				if((displayMode == DISPLAY_MODE_4SPLIT_H) ||(displayMode == DISPLAY_MODE_4SPLIT_X) )
				{
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				}
				else if(IS_2SPLIT_MODE(displayMode) == TRUE)
				{
					SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1920x1080p60;
				}
				else
				{
					SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p60;
				}
			}
			else
			{
				if(IS_2SPLIT_MODE(displayMode) == TRUE)
				{
					OutMainFrmt[MDIN_ID_B] = VIDOUT_1920x1080pRB2;
				}
				else
				{
					OutMainFrmt[MDIN_ID_B] = VIDOUT_1280x720p50;
				}
				
				if((displayMode == DISPLAY_MODE_4SPLIT_H) ||(displayMode == DISPLAY_MODE_4SPLIT_X) )
				{
					SrcMainFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				}
				else if(IS_2SPLIT_MODE(displayMode) == TRUE)
				{
					SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1920x1080p50;
				}
				else
				{
					SrcAuxFrmt[MDIN_ID_C] = VIDSRC_1280x720p50;
				}
			}
			break;

		default:
			//currentMainFrmt = VIDSRC_1280x720p60;
			break;
	}

	// MDIN_D
	switch(OutMainFrmt[MDIN_ID_C])
	{
		case VIDOUT_1920x1080p60:
		case VIDOUT_1920x1080p30:
			stVideo[MDIN_ID_D].stSRC_a.frmt = VIDSRC_1920x1080p60;
			stVideo[MDIN_ID_D].stSRC_b.frmt = VIDSRC_1920x1080p60;
			stVideo[MDIN_ID_D].stOUT_x.frmt = VIDOUT_720x480i60;
			stVideo[MDIN_ID_D].encFRMT = VID_VENC_NTSC_M;
			break;

		case VIDOUT_1920x1080p50:
		case VIDOUT_1920x1080p25:
			stVideo[MDIN_ID_D].stSRC_a.frmt = VIDSRC_1920x1080p50;
			stVideo[MDIN_ID_D].stSRC_b.frmt = VIDSRC_1920x1080p50;
			stVideo[MDIN_ID_D].stOUT_x.frmt = VIDOUT_720x576i50;
			stVideo[MDIN_ID_D].encFRMT = VID_VENC_PAL_B;
			break;
	}

	for(mdin = MDIN_ID_A; mdin <= MDIN_ID_MAX; mdin++)
	{
		if(OutMainFrmt[mdin] != PrevOutMainFrmt[mdin])
		{
			stVideo[mdin].exeFLAG |= MDIN_UPDATE_MAIN_OUT;
		}

		if(SrcMainFrmt[mdin] != PrevSrcMainFrmt[mdin])
		{
			stVideo[mdin].exeFLAG |= MDIN_UPDATE_MAIN_IN;
		}

		if(SrcAuxFrmt[mdin] != PrevSrcAuxFrmt[mdin])
		{
			stVideo[mdin].exeFLAG |= MDIN_UPDATE_AUX_IN;
		}

		if(stVideo[MDIN_ID_D].stOUT_x.frmt != preOutAuxFrmt)
		{
			stVideo[MDIN_ID_D].exeFLAG |= MDIN_UPDATE_AUX_OUT | MDIN_UPDATE_ENC;
			preOutAuxFrmt = stVideo[MDIN_ID_D].stOUT_x.frmt;
			preEncFrmt = stVideo[MDIN_ID_D].encFRMT;
		}
	}

}


//--------------------------------------------------------------------------------------------------
void CreateDisplayWindow_A(eDisplayMode_t displayMode)
{
	WORD mainTotalWidth, mainTotalHeight;
	WORD auxTotalWidth, auxTotalHeight;
	WORD winWidth, winHeight;
	sCroppingOffset_t croppingOffset;

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
			mainTotalWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			mainTotalHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			mainTotalWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			mainTotalHeight = DISPLAY_HEIGHT_1280x720;
			break;

		case VIDSRC_960x480i60:
			mainTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			mainTotalHeight = DISPLAY_HEIGHT_480;
			break;

		case VIDSRC_960x576i50:
			mainTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			mainTotalHeight = DISPLAY_HEIGHT_576;
			break;
	}

	switch(SrcAuxFrmt[MDIN_ID_A])
	{
		case VIDSRC_1920x1080p60:
		case VIDSRC_1920x1080p50:
			auxTotalWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			auxTotalHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			auxTotalWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			auxTotalHeight = DISPLAY_HEIGHT_1280x720;
			break;
			
		case VIDSRC_960x480i60:
			auxTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			auxTotalHeight = DISPLAY_HEIGHT_480;
			break;

		case VIDSRC_960x576i50:
			auxTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			auxTotalHeight = DISPLAY_HEIGHT_576;
			break;

	}

	pMainCrop->w = mainTotalWidth;
	pMainCrop->h = mainTotalHeight;

	pAuxCrop->w = auxTotalWidth;
	pAuxCrop->h = auxTotalHeight;

	if(OutMainFrmt[MDIN_ID_A] == VIDOUT_1280x720p60)
	{
		winWidth = DISPLAY_WIDTH_1280x720;
		winHeight = DISPLAY_HEIGHT_1280x720;
	}
	else
	{
		winWidth = DISPLAY_WIDTH_1920X1080;
		winHeight = DISPLAY_HEIGHT_1920x1080;
	}
	
	switch(displayMode)
	{
		case DISPLAY_MODE_2SPLIT_HCROP_A:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth/2;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth /(2*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;
			pMainCrop->y = 0;

			pMainView->w = winWidth/2;
			pMainView->h = winHeight;
			pMainView->x = 0;
			pMainView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_HCROP_B:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL3);
			pMainCrop->w = mainTotalWidth/2;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth /(2*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;
			pMainCrop->y = 0;

			pMainView->w = winWidth/2;
			pMainView->h = winHeight;
			pMainView->x = 0;
			pMainView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_A:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(2*ADJUST_WINDOW_STEP_MAX))*croppingOffset.v_offset;

			pMainView->w = winWidth;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_B:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL3);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(2*ADJUST_WINDOW_STEP_MAX))*croppingOffset.v_offset;

			pMainView->w = winWidth;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = 0;
			break;
			
		case DISPLAY_MODE_3SPLIT_R2CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth/2;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth /(2*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;
			pMainCrop->y = 0;
		case DISPLAY_MODE_3SPLIT_R2SCALE:
			pMainView->w = winWidth/2;
			pMainView->h = winHeight;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = winWidth/2;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_3SPLIT_L2CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth/2;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth /(2*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;
			pMainCrop->y = 0;
		case DISPLAY_MODE_3SPLIT_L2SCALE:
			pMainView->w = winWidth/2;
			pMainView->h = winHeight;
			pMainView->x = winWidth;
			pMainView->y = 0;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_3SPLIT_D2CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(2*ADJUST_WINDOW_STEP_MAX))*croppingOffset.v_offset;
		case DISPLAY_MODE_3SPLIT_D2SCALE:
			pMainView->w = winWidth;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = 0;
			pAuxView->y = winHeight/2;
			break;
			
		case DISPLAY_MODE_3SPLIT_U2CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(2*ADJUST_WINDOW_STEP_MAX))*croppingOffset.v_offset;
		case DISPLAY_MODE_3SPLIT_U2SCALE:
			pMainView->w = winWidth;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = winHeight/2;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_R3CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth*2/3;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth/(3*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;//0;
			pMainCrop->y = 0;
		case DISPLAY_MODE_4SPLIT_R3SCALE:
			pMainView->w = winWidth*2/3 + 1;
			pMainView->h = winHeight;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/3;
			pAuxView->h = winHeight/3;
			pAuxView->x = (winWidth/3)*2;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_L3CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth*2/3;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth/(3*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;//0;
			pMainCrop->y = 0;
		case DISPLAY_MODE_4SPLIT_L3SCALE:
			pMainView->w = winWidth*2/3;
			pMainView->h = winHeight;
			pMainView->x = winWidth/3;
			pMainView->y = 0;

			pAuxView->w = winWidth/3;
			pAuxView->h = winHeight/3;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_D3CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight*2/3;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(3*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.v_offset;//0;
		case DISPLAY_MODE_4SPLIT_D3SCALE:
			pMainView->w = winWidth;
			pMainView->h = winHeight*2/3;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/3;
			pAuxView->h = winHeight/3;
			pAuxView->x = 0;
			pAuxView->y = (winHeight/3)*2;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3CROP:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL1);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight*2/3;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(3*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.v_offset;//0;
		case DISPLAY_MODE_4SPLIT_U3SCALE:
			pMainView->w = winWidth;
			pMainView->h = winHeight*2/3;
			pMainView->x = 0;
			pMainView->y = winHeight/3;
			
			pAuxView->w = winWidth/3;
			pAuxView->h = winHeight/3;
			pAuxView->x = 0;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_QUAD:
			pMainView->w = winWidth/2;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = 0;
			pAuxView->y = winHeight/2;
			break;
			
		case DISPLAY_MODE_4SPLIT_H:		
			pMainView->w = winWidth/2;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = 0;
			pAuxView->y = winHeight/2;
			break;
			
		case DISPLAY_MODE_4SPLIT_X:
			pMainView->w = winWidth/2;
			pMainView->h = winHeight/2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = winWidth/2;
			pAuxView->h = winHeight/2;
			pAuxView->x = winWidth/2;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
			pAuxView->w = PIP_WINDOW_WIDTH;
			pAuxView->h = PIP_WINDOW_HEIGHT;
			pAuxView->x = (DISPLAY_WIDTH_1280x720 - PIP_WINDOW_WIDTH -PIP_POSITION_MARGIN);
			pAuxView->y = PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
			pAuxView->w = PIP_WINDOW_WIDTH;
			pAuxView->h = PIP_WINDOW_HEIGHT;
			pAuxView->x = PIP_POSITION_MARGIN;
			pAuxView->y = DISPLAY_HEIGHT_1280x720 - PIP_WINDOW_HEIGHT-PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
			pAuxView->w = PIP_WINDOW_WIDTH;
			pAuxView->h = PIP_WINDOW_HEIGHT;
			pAuxView->x = (DISPLAY_WIDTH_1280x720-PIP_WINDOW_WIDTH-PIP_POSITION_MARGIN);
			pAuxView->y = DISPLAY_HEIGHT_1280x720-PIP_WINDOW_HEIGHT-PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_D2:
		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			pAuxView->w = PIP_WINDOW_WIDTH;
			pAuxView->h = PIP_WINDOW_HEIGHT;
			pAuxView->x = PIP_POSITION_MARGIN;
			pAuxView->y = PIP_POSITION_MARGIN;
			break;
	}

	memcpy(&stVideo[MDIN_ID_A].stCROP_m, pMainCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_A].stVIEW_m, pMainView, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_A].stCROP_x, pAuxCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_A].stVIEW_x, pAuxView, sizeof(MDIN_VIDEO_WINDOW));
}


//--------------------------------------------------------------------------------------------------
void CreateDisplayWindow_B(eDisplayMode_t displayMode)
{
	//eChannel_t mainChannel, auxChannel;
	WORD mainTotalWidth, mainTotalHeight;
	WORD auxTotalWidth, auxTotalHeight;
//	WORD winWidth, winHeight;
	sCroppingOffset_t croppingOffset;

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
			mainTotalWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			mainTotalHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			mainTotalWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			mainTotalHeight = DISPLAY_HEIGHT_1280x720;
			break;

		case VIDSRC_960x480i60:
			mainTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			mainTotalHeight = DISPLAY_HEIGHT_480;
			break;

		case VIDSRC_960x576i50:
			mainTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			mainTotalHeight = DISPLAY_HEIGHT_576;
			break;
	}

	switch(SrcAuxFrmt[MDIN_ID_B])
	{
		case VIDSRC_1920x1080p60:
		case VIDSRC_1920x1080p50:
			auxTotalWidth = DISPLAY_WIDTH_1920X1080 - COMPENSATION_MARGIN;
			auxTotalHeight = DISPLAY_HEIGHT_1920x1080;
			break;

		case VIDSRC_1280x720p60:
		case VIDSRC_1280x720p50:
			auxTotalWidth = DISPLAY_WIDTH_1280x720 - COMPENSATION_MARGIN;
			auxTotalHeight = DISPLAY_HEIGHT_1280x720;
			break;
			
		case VIDSRC_960x480i60:
			auxTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			auxTotalHeight = DISPLAY_HEIGHT_480;
			break;

		case VIDSRC_960x576i50:
			auxTotalWidth = DISPLAY_WIDTH_960*2 - (COMPENSATION_MARGIN+5);
			auxTotalHeight = DISPLAY_HEIGHT_576;
			break;

	}

	pMainCrop->w = mainTotalWidth;
	pMainCrop->h = mainTotalHeight;

	pAuxCrop->w = auxTotalWidth;
	pAuxCrop->h = auxTotalHeight;

	switch(displayMode)
	{
		case DISPLAY_MODE_2SPLIT_HCROP_A:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL2);
			pMainCrop->w = mainTotalWidth/2;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth /(2*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;
			pMainCrop->y = 0;

			pMainView->w = DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_HCROP_B:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL4);
			pMainCrop->w = mainTotalWidth/2;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = (mainTotalWidth /(2*ADJUST_WINDOW_STEP_MAX)) * croppingOffset.h_offset;
			pMainCrop->y = 0;

			pMainView->w = DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_A:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL2);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(2*ADJUST_WINDOW_STEP_MAX))*croppingOffset.v_offset;

			pMainView->w = DISPLAY_WIDTH;
			pMainView->h = DISPLAY_HALF_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_B:
			Read_NvItem_CroppingOffset(&croppingOffset, CHANNEL4);
			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/2;
			pMainCrop->x = 0;
			pMainCrop->y = (mainTotalHeight/(2*ADJUST_WINDOW_STEP_MAX))*croppingOffset.v_offset;

			pMainView->w = DISPLAY_WIDTH;
			pMainView->h = DISPLAY_HALF_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
			pMainView->w = DISPLAY_WIDTH_1280x720 / 2;
			pMainView->h = DISPLAY_HEIGHT_1280x720 / 2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720 / 2;
			pAuxView->h =  DISPLAY_HEIGHT_1280x720 / 2;
			pAuxView->x = DISPLAY_WIDTH_1280x720 / 2;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
		case DISPLAY_MODE_4SPLIT_QUAD:
			pMainView->w = DISPLAY_WIDTH_1280x720 / 2;
			pMainView->h =  DISPLAY_HEIGHT_1280x720 / 2;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720 / 2;
			pAuxView->h =  DISPLAY_HEIGHT_1280x720 / 2;
			pAuxView->x = 0;
			pAuxView->y =  DISPLAY_HEIGHT_1280x720 / 2;
			break;
			
		case DISPLAY_MODE_4SPLIT_H:	
			pMainView->w = DISPLAY_WIDTH_1280x720 / 4;
			pMainView->h = DISPLAY_HEIGHT_1280x720;
			pMainView->x = 0;
			pMainView->y = 0;

			pMainCrop->w = mainTotalWidth/4;
			pMainCrop->h = mainTotalHeight;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720 / 4;
			pAuxView->h = DISPLAY_HEIGHT_1280x720;
			pAuxView->x = DISPLAY_WIDTH_1280x720*3/4;
			pAuxView->y = 0;

			pAuxCrop->w = auxTotalWidth/4;
			pAuxCrop->h = auxTotalHeight;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_X:
			pMainView->w = DISPLAY_WIDTH_1280x720/2;
			pMainView->h = DISPLAY_HEIGHT_1280x720/8;
			pMainView->x = 0;
			pMainView->y = 0;

			pMainCrop->w = mainTotalWidth;
			pMainCrop->h = mainTotalHeight/4;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxView->w = DISPLAY_WIDTH_1280x720/2;
			pAuxView->h = DISPLAY_HEIGHT_1280x720/8;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT_1280x720*3/8;

			pAuxCrop->w = auxTotalWidth;
			pAuxCrop->h = auxTotalHeight/4;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;
	}

	memcpy(&stVideo[MDIN_ID_B].stCROP_m, pMainCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_B].stVIEW_m, pMainView, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_B].stCROP_x, pAuxCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_B].stVIEW_x, pAuxView, sizeof(MDIN_VIDEO_WINDOW));
}


//--------------------------------------------------------------------------------------------------
void CreateDisplayWindow_C(eDisplayMode_t displayMode)
{
	PMDIN_VIDEO_WINDOW pMainView = &stMainVIEW[MDIN_ID_C];
	PMDIN_VIDEO_WINDOW pAuxView = &stAuxVIEW[MDIN_ID_C];
	PMDIN_VIDEO_WINDOW pMainCrop = &stMainCROP[MDIN_ID_C];
	PMDIN_VIDEO_WINDOW pAuxCrop = &stAuxCROP[MDIN_ID_C];
	sCroppingOffset_t croppingOffset;
	
	// initialize each object
	memset(pMainView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxView, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pMainCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));
	memset(pAuxCrop, 0x00, sizeof(MDIN_VIDEO_WINDOW));

	switch(displayMode)
	{
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
			pMainCrop->w = DISPLAY_HALF_WIDTH;
			pMainCrop->h = DISPLAY_HEIGHT;
			pMainCrop->x = 0;
			pMainCrop->y = 0;

			pAuxCrop->w = DISPLAY_HALF_WIDTH;
			pAuxCrop->h = DISPLAY_HEIGHT;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;	
		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
			pMainView->w = DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT;
			pAuxView->x = DISPLAY_HALF_WIDTH;
			pAuxView->y = 0;
			break;

		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			pMainCrop->w = DISPLAY_WIDTH;
			pMainCrop->h = DISPLAY_HALF_HEIGHT;
			pMainCrop->x = 0;
			pMainCrop->y = 0;
			
			pAuxCrop->w = DISPLAY_WIDTH;
			pAuxCrop->h = DISPLAY_HALF_HEIGHT;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
		
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
			pMainView->w = DISPLAY_WIDTH;
			pMainView->h = DISPLAY_HALF_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			pAuxView->w = DISPLAY_WIDTH;
			pAuxView->h = DISPLAY_HALF_HEIGHT;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HALF_HEIGHT;
			break;
			

		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HALF_HEIGHT;
			pAuxView->x = DISPLAY_HALF_WIDTH;
			pAuxView->y = DISPLAY_HALF_HEIGHT;
			break;
			
		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HALF_HEIGHT;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HALF_HEIGHT;
			break;

		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HALF_HEIGHT;
			pAuxView->x = DISPLAY_HALF_WIDTH;
			pAuxView->y = 0;
			break;
			
		case DISPLAY_MODE_4SPLIT_QUAD:
			pMainView->w = DISPLAY_HALF_WIDTH;
			pMainView->h = DISPLAY_HEIGHT;
			pMainView->x = 0;
			pMainView->y = 0;

			pMainCrop->w = DISPLAY_HALF_WIDTH;
			pMainCrop->h = DISPLAY_HEIGHT;
			pMainCrop->x = 0;
			pMainCrop->y = 0;
			
			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT;
			pAuxView->x = DISPLAY_HALF_WIDTH;
			pAuxView->y = 0;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720/2;
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

			pAuxCrop->w = DISPLAY_WIDTH_1280x720/2;
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

			pAuxCrop->w = DISPLAY_WIDTH_1280x720/2;
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
			pAuxView->w = DISPLAY_HALF_WIDTH;
			pAuxView->h = DISPLAY_HEIGHT;
			pAuxView->x = DISPLAY_WIDTH/4;
			pAuxView->y = 0;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720/2;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;

		case DISPLAY_MODE_4SPLIT_X:
			pMainCrop->w = DISPLAY_WIDTH_1280x720/2;
			pMainCrop->h = DISPLAY_HEIGHT_1280x720/2;
			pMainCrop->x = 0;
			pMainCrop->y = 0;
			
			pAuxView->w = DISPLAY_WIDTH;
			pAuxView->h = DISPLAY_HALF_HEIGHT;
			pAuxView->x = 0;
			pAuxView->y = DISPLAY_HEIGHT/4;

			pAuxCrop->w = DISPLAY_WIDTH_1280x720;
			pAuxCrop->h = DISPLAY_HEIGHT_1280x720/2;
			pAuxCrop->x = 0;
			pAuxCrop->y = 0;
			break;
	}

	memcpy(&stVideo[MDIN_ID_C].stCROP_m, pMainCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_C].stVIEW_m, pMainView, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_C].stCROP_x, pAuxCrop, sizeof(MDIN_VIDEO_WINDOW));
	memcpy(&stVideo[MDIN_ID_C].stVIEW_x, pAuxView, sizeof(MDIN_VIDEO_WINDOW));
}



//--------------------------------------------------------------------------------------------------
static void InputSourceHandler(MDIN_VIDEO_INPUT_t src)
{
	eDisplayMode_t dispalyMode = GetCurrentDisplayMode();
	
	SetInputVideoPath(src, dispalyMode);
	ConfigVideoFrmt(src);

	if(src != VIDEO_DIGITAL_NVP6158_B)
	{
		CreateDisplayWindow_A(dispalyMode);
	}
	if(src != VIDEO_DIGITAL_NVP6158_A)
	{
		CreateDisplayWindow_B(dispalyMode);
	}
	CreateDisplayWindow_C(dispalyMode);

	InputSelOld = src;
}

//--------------------------------------------------------------------------------------------------
static void SetOSDMenuRefresh(void)
{
//	ConfigI2C(MDIN_ID_C);
	M380_ID = MDIN_ID_C;
	OSD_ModifyPalette_M((stVideo[MDIN_ID_C].stOUT_m.mode == MDIN_OUT_RGB444_8)? OSD_RGB_PALETTE : OSD_YUV_PALETTE);
	MDINOSD_EnableLayerRepeat(&stLayer[LAYER_INDEX0], OFF, OFF);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo[MDIN_ID_C], ON);
}

//--------------------------------------------------------------------------------------------------
static void VideoFrameProcess(void)
{
	BYTE ID;

	if (stVideo[MDIN_ID_A].exeFLAG == 0 && stVideo[MDIN_ID_B].exeFLAG == 0 && stVideo[MDIN_ID_C].exeFLAG == 0)// && stVideo[MDIN_ID_D].exeFLAG == 0) 
		return;

	for(ID = MDIN_ID_A; ID < MDIN_ID_MAX-1; ID++)
	{
		if (stVideo[ID].srcPATH == PATH_MAIN_B_AUX_B || stVideo[ID].srcPATH == PATH_MAIN_B_AUX_A || stVideo[ID].srcPATH == PATH_MAIN_B_AUX_M)
		{
			stVideo[ID].stSRC_b.frmt = SrcMainFrmt[ID];
			stVideo[ID].stSRC_a.frmt = SrcAuxFrmt[ID];
		}
		else
		{
			stVideo[ID].stSRC_a.frmt = SrcMainFrmt[ID];
			stVideo[ID].stSRC_b.frmt = SrcAuxFrmt[ID];
		}
		stVideo[ID].stOUT_m.frmt = OutMainFrmt[ID];

		PrevOutMainFrmt[ID] = OutMainFrmt[ID];
		PrevSrcMainFrmt[ID] = SrcMainFrmt[ID];
		PrevSrcAuxFrmt[ID] = SrcAuxFrmt[ID];
	}

	for (ID = MDIN_ID_A ; ID < MDIN_ID_MAX-1; ID++)
	{
		M380_ID = ID;

		if (stVideo[ID].exeFLAG&MDIN_UPDATE_MAIN_IN)
		{
			MDIN3xx_OutDarkScreen(ON);
			MDIN3xx_EnableMainDisplay(OFF);
			MDIN3xx_EnableMainFreeze(ID, ON);
		}

		if (stVideo[ID].exeFLAG&MDIN_UPDATE_AUX_IN)
		{
			MDIN3xx_AuxDarkScreen(ON);
			MDIN3xx_EnableAuxDisplay(&stVideo[ID], OFF);
			MDIN3xx_EnableAuxFreeze(&stVideo[ID], ON);
		}

		if (stVideo[ID].exeFLAG&MDIN_UPDATE_IN)	MDIN3xx_VideoInProcess(&stVideo[ID]);
		if (stVideo[ID].exeFLAG&MDIN_UPDATE_MAIN)	MDIN3xx_VideoProcess(&stVideo[ID]);
		if (stVideo[ID].exeFLAG&MDIN_UPDATE_AUX)	MDINAUX_VideoProcess(&stVideo[ID]);

	}

	M380_ID = MDIN_ID_C;
	SetIPCVideoFine();	// tune IPC-register (CVBS or HDMI)

	for (ID = MDIN_ID_A ; ID < MDIN_ID_MAX-1; ID++)
	{
		M380_ID = ID;
		MDIN3xx_EnableMainFreeze(ID, OFF);
		MDIN3xx_EnableMainDisplay(ON);
		MDIN3xx_EnableAuxFreeze(&stVideo[ID], OFF);
		MDIN3xx_EnableAuxDisplay(&stVideo[ID], ON);
	}
	MDINDLY_mSec(100);	// delay 100ms
	
	for (ID = MDIN_ID_A ; ID < MDIN_ID_MAX-1; ID++)
	{
		M380_ID = ID;	
		MDIN3xx_OutDarkScreen(OFF);
		MDIN3xx_AuxDarkScreen(OFF);
	}

	// MDIN_ID_D
	if(stVideo[MDIN_ID_D].exeFLAG != MDIN_UPDATE_CLEAR)
	{
		MDIN3xx_EnableAuxDisplay(&stVideo[ID], OFF);
		
		MDIN3xx_VideoInProcess(&stVideo[MDIN_ID_D]);
		MDIN3xx_VideoProcess(&stVideo[MDIN_ID_D]);
		MDINAUX_VideoProcess(&stVideo[MDIN_ID_D]);

		MDIN3xx_EnableAuxDisplay(&stVideo[ID], ON);
	}
	
	M380_ID = MDIN_ID_C;
}

static void TurnOff_VideoLossChannels(MDIN_VIDEO_INPUT_t src)
{
	eChannel_t channel;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		if(GetInputVideoFormat(channel)  == NC_VIVO_CH_FORMATDEF_UNKNOWN)
		{
			if(channel == FindMainChannel(displayMode, MDIN_ID_A))
			{
				M380_ID =  MDIN_ID_A;
				//MDIN3xx_OutDarkScreen(ON);
				MDIN3xx_EnableMainDisplay(OFF);
			}
			else if(channel == FindAuxChannel(displayMode, MDIN_ID_A))
			{
				M380_ID =  MDIN_ID_A;
#if 0
				if(IS_PIP_MODE(displayMode) == TRUE)
				{
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x145, 4, 2, 3);
				}
				else
				{
					MDIN3xx_AuxDarkScreen(ON);
					MDIN3xx_EnableAuxDisplay(&stVideo[MDIN_ID_A], OFF);
				}
#else
				MDIN3xx_AuxDarkScreen(ON);
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x145, 4, 2, 3);
#endif
			}
			else if(channel == FindMainChannel(displayMode, MDIN_ID_B))
			{
				M380_ID =  MDIN_ID_B;
				MDIN3xx_EnableMainDisplay(OFF);
			}
			else if(channel == FindAuxChannel(displayMode, MDIN_ID_B))
			{
				M380_ID =  MDIN_ID_B;
				MDIN3xx_AuxDarkScreen(ON);
				MDIN3xx_EnableAuxDisplay(&stVideo[MDIN_ID_B], OFF);
			}
		}
	}

	M380_ID =  MDIN_ID_C;
}
// ----------------------------------------------------------------------
// Exported functions
// ----------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void CreateVideoInstance(void)
{
	memset((PBYTE)&stVideo[MDIN_ID_D], 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_D();
	
	memset((PBYTE)&stVideo[MDIN_ID_A], 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_AB(MDIN_ID_A);

	memset((PBYTE)&stVideo[MDIN_ID_B], 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_AB(MDIN_ID_B);

	memset((PBYTE)&stVideo[MDIN_ID_C], 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetRegInitial_C();
	
	InitInputSource();
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
	stVideo[MDIN_ID_A].exeFLAG = stVideo[MDIN_ID_B].exeFLAG = MDIN_UPDATE_ALL;
	stVideo[MDIN_ID_C].exeFLAG |= MDIN_UPDATE_IN;
	//stVideo[MDIN_ID_D].exeFLAG |= MDIN_UPDATE_IN;
}
//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
	//eDisplayMode_t dispalyMode = GetCurrentDisplayMode();
	
	if(fInputChanged == TRUE)
	{
		InputSourceHandler(InputSelect);
		VideoFrameProcess();
		TurnOff_VideoLossChannels(InputSelect);
		SetOSDMenuRefresh();
#if DUMP_REG
		fRegDump = TRUE;
#endif
	}
	fInputChanged = FALSE;

#if DUMP_REG
	if(fRegDump == TRUE)
	{
		M380_ID = MDIN_ID_C;
		MDIN_DumpRegister(MDIN_HOST_ID, 0x0000, 0x100); // Host Register 0x000~0x0FF
		MDINDLY_mSec(1);
		MDIN_DumpRegister(MDIN_LOCAL_ID, 0x0000, 0x400); // Local Register 0x000~0x3FF

		M380_ID = MDIN_ID_D;
		MDIN_DumpRegister(MDIN_HOST_ID, 0x0000, 0x100); // Host Register 0x000~0x0FF
		MDINDLY_mSec(1);
		MDIN_DumpRegister(MDIN_LOCAL_ID, 0x0000, 0x400); // Local Register 0x000~0x3FF
		
		fRegDump = FALSE;
	}
#endif
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	M380_ID = MDIN_ID_C;
	MDINHTX_CtrlHandler(&stVideo[MDIN_ID_C]);
}


//--------------------------------------------------------------------------------------------------
void SetAuxOutMode_C(void)
{
	eOutVideoFormat_t format;

	Read_NvItem_AuxVideoFormat(&format);

	M380_ID = MDIN_ID_C;
	if(format == VIDEO_VGA)
	{
		stVideo[MDIN_ID_C].stOUT_m.mode = MDIN_OUT_RGB444_8;
		OSD_ModifyPalette_M(OSD_RGB_PALETTE);
	}
	else
	{
		stVideo[MDIN_ID_C].stOUT_m.mode = MDIN_OUT_EMB422_8;
		OSD_ModifyPalette_M(OSD_YUV_PALETTE);
	}
	stVideo[MDIN_ID_C].exeFLAG |= MDIN_UPDATE_ALL;//MDIN_UPDATE_AUX;
}


//--------------------------------------------------------------------------------------------------
eColorFormat_t GetCurrentColorFormat(void)
{
	eOutVideoFormat_t out;

	Read_NvItem_AuxVideoFormat(&out);
	return (out == VIDEO_VGA) ? COLOR_RGB : COLOR_YBR;;
}

void UpdateCroppingScreen(eChannel_t channel)
{
	if((channel == CHANNEL2) || (channel == CHANNEL4))
	{
		MDIN3xx_SetScaleProcess(&stVideo[MDIN_ID_B]);
	}
	else
	{
		MDIN3xx_SetScaleProcess(&stVideo[MDIN_ID_A]);
	}
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
	
	memset(pBuf, 0x0000, sizeof(pBuf));
	
	for(index = 0; index < len; index++)
	{
		MDINHIF_RegRead(id, addr, pBuf+index);
		addr++;
	}
}
#endif
/*  FILE_END_HERE */

