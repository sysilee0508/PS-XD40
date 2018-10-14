/* Includes ------------------------------------------------------------------*/
#include "NVP6158.h"


/*******************************************************************************
*	Description		: Get video format(each ch)
*	Argurments		: pVFmtSet(raptor3 information)
*	Return value	: N/A
*	Modify			:
*	warning			:
*******************************************************************************/
int RAPTOR3_SAL_GetFormatEachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	video_input_vfc sVFC;
	video_input_novid sNoVideo;
	motion_mode sMotion;
	NC_VIVO_CH_FORMATDEF oCurVidFmt;

	/* initialize current video format - pInformation structure is for app */
	pInformation->curvideofmt[ch] = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	pInformation->curvideoloss[ch] = VIDEO_LOSS_OFF;
	pInformation->vfc[ch] = 0xff;

	/* initialize vfc(B5xF0) and videoloss information(B0xA8) */
	sVFC.ch = ch%4;
	sVFC.devnum = ch/4;
	sNoVideo.ch = ch%4;
	sNoVideo.devnum = ch/4;

	/* get vfc and videoloss */
	video_input_vfc_read(&sVFC);
	video_input_novid_read(&sNoVideo);

	/* check vfc&videoloss and run debounce  */
	if(((((sVFC.vfc >> 4 ) & 0xF) != 0xF) && ((sVFC.vfc & 0x0F) != 0xF)) && !sNoVideo.novid) // OnVideo
	{
		/* convert vfc to formatDefine for APP and save videoloss information */
		oCurVidFmt = NC_VD_AUTO_VFCtoFMTDEF(sVFC.vfc);

		/* debouce */
		pInformation->curvideofmt[ch] = oCurVidFmt;
		pInformation->vfc[ch] = sVFC.vfc;
	}

	sMotion.ch = ch%4;
	sMotion.devnum = ch/4;
	sMotion.fmtdef = pInformation->curvideofmt[ch];
	sMotion.set_val = Get_MotionDetect_OnOff(sMotion.ch);
	
	motion_onoff_set(&sMotion);

	motion_pixel_all_onoff_set(&sMotion);
	sMotion.set_val = Get_MotionDetect_Sensitivity(sMotion.ch);
	motion_tsen_set(&sMotion);

	/* check novideo option */
	if( !sNoVideo.novid )
	{
		pInformation->curvideoloss[ch] = VIDEO_LOSS_ON;
		pInformation->videolosscheck[ch] = 0;
		if(Get_MotionDetect_OnOff(sMotion.ch))
		{
			motion_detection_get(&sMotion);
			pInformation->motiondetect |= sMotion.set_val;
		}
	}
	else
	{
		pInformation->videolosscheck[ch] = 0x1 << ch;
	}

	return 0;
}

/*******************************************************************************
*	Description		: set video format each channel
*	Argurments		: ch:channel, pInformation(raptor3 information), videomode(0:novideo, 1:Onvideo)
*	Return value	: NC_VIVO_CH_FORMATDEF_UNKNOWN(NO video), retfmt(get video format)
*	Modify			:
*	warning			:
*******************************************************************************/
int RAPTOR3_SAL_OnVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	unsigned char iPort = 0;
	unsigned char oChannel = 0x00;
	unsigned char oDevNum = 0x00;
	unsigned char oMux;
	unsigned char oInterface;
	unsigned char oClkEdge;

	NC_VO_PORT_FMT_S *pPortFmt;		// output
	decoder_dev_ch_info_s DEV_CH_INFO;

	NC_VIVO_CH_FORMATDEF FmtDef = pInformation->curvideofmt[ch];

	if(pInformation->oMux == VI_2MULTIPLEX_MODE)
	{

		iPort = (( ch % 4 ) / 2) + 1;
		oChannel = ch%4;
		oDevNum = ch/4;

		oMux = VI_2MULTIPLEX_MODE;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_DOUBLE;
	}
	else
	{
		iPort = ch%4;
		oChannel = ch%4;
		oDevNum = ch/4;

		oMux = VI_1MULTIPLEX_MODE;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_SINGLE_UP;
	}

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( FmtDef ); // output

	DEV_CH_INFO.ch = oChannel;
	DEV_CH_INFO.devnum = oDevNum;
	DEV_CH_INFO.fmt_def = FmtDef;
	video_input_onvideo_set(&DEV_CH_INFO);

      /* This Part is enough EQ Setting */
	video_input_h_timing_set(&DEV_CH_INFO);
	video_input_color_set(&DEV_CH_INFO);
    video_input_vafe_init(&DEV_CH_INFO);

    video_input_new_format_set(&DEV_CH_INFO);
	
	//NC_VD_VO_Mode_Set( oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );
    NC_VD_VO_Mode_Set_New( oChannel, oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );

	return 0;
}

/*******************************************************************************
*	Description		: check Auto Debouce
*	Argurments		: ch:channel, pInformation(raptor3 information), videomode(0:novideo, 1:Onvideo)
*	Return value	: 0(not checked video format), -1(checked video format)
*	Modify			: N/A
*	warning			:
*******************************************************************************/
int RAPTOR3_SAL_AutoDebouceCheck( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	int i;
	int ret = 0;
	unsigned char		oDebncIdx = 0;
	unsigned char 		oVfc = 0;
	NC_VIVO_CH_FORMATDEF oFmtB5Def;
	video_input_vfc	sVFC;

	sVFC.ch = ch % 4;
	sVFC.devnum = ch / 4;
	video_input_onvideo_check_data(&sVFC);

	oDebncIdx = pInformation->debounceidx[ch];
	pInformation->debounce[ch][oDebncIdx%5] = sVFC.vfc;

	/* For Debug Ch1 Only */
	/*
	if( ch == 0)
		printf("debunce:0x%02X, debncIdx:%d\n", pInformation->debounce[ch][pInformation->debounceidx[ch]], pInformation->debounceidx[ch]);
	*/
	pInformation->debounceidx[ch]++;
	pInformation->debounceidx[ch] = ( (pInformation->debounceidx[ch] % 5) == 0 ) ? 0 : pInformation->debounceidx[ch];

	oVfc = pInformation->debounce[ch][pInformation->debounceidx[ch]];
	for( i = 0; i < 5; i++ )
	{
		if( oVfc != pInformation->debounce[ch][i])
		{
			break;
		}
	}
	if( i == 5 )
	{
		oFmtB5Def = NC_VD_AUTO_VFCtoFMTDEF(oVfc);
		if( ( oFmtB5Def != AHD30_5M_20P ) && ( oFmtB5Def != pInformation->prevideofmt[ch] ) )
		{
			ret = -1;
		}
	}


	return ret;
}

/*******************************************************************************
*	Description		: set video format each channel
*	Argurments		: ch:channel, pInformation(raptor3 information), videomode(0:novideo, 1:Onvideo)
*	Return value	: NC_VIVO_CH_FORMATDEF_UNKNOWN(NO video), retfmt(get video format)
*	Modify			: N/A
*	warning			:
*******************************************************************************/
int RAPTOR3_SAL_NoVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	NC_VO_PORT_FMT_S *pPortFmt;

	unsigned char oChannel;
	unsigned char devnum;
	unsigned char iPort;
	unsigned char oMux;
	unsigned char oInterface;
	unsigned char oClkEdge;
	video_input_novid_set AutoNoVidDet;

	if(pInformation->oMux == VI_2MULTIPLEX_MODE)
	{
		oChannel = ch % 4;
		devnum = ch / 4;
		iPort = (( ch % 4 ) / 2) + 1;

		oMux = VI_2MULTIPLEX_MODE;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_DOUBLE;
	}
	else
	{
		oChannel = ch % 4;
		devnum = ch / 4;
		iPort = ch % 4;

		oMux = VI_1MULTIPLEX_MODE;
		oInterface = VI_INPUT_MODE_DIGITAL_CAMERA;//VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_SINGLE_UP;
	}

	AutoNoVidDet.ch = oChannel;
	AutoNoVidDet.devnum = devnum;
	video_input_no_video_set(&AutoNoVidDet);

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( CVI_HD_30P_EX );
	//NC_VD_VO_Mode_Set( devnum, iPort, pPortFmt, oMux, oInterface, oClkEdge, ch%4, ch%4, ch%4, ch%4 );
	NC_VD_VO_Mode_Set_New( oChannel, devnum, iPort, pPortFmt, oMux, oInterface, oClkEdge, ch%4, ch%4, ch%4, ch%4 );

	return 0;
}

