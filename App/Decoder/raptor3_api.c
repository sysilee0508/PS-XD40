/* Includes ------------------------------------------------------------------*/
#include "NVP6158.h"
#include "delay.h"

#if 1
/*******************************************************************************
 *	Description		: debounce function
 *	Argurments		: ch(channel), value(current translated video format)
 *	Return value	: NC_VIVO_CH_FORMATDEF_UNKNOWN(NO video), retfmt(get video format)
 *	Modify			:
 *	warning			:
 *******************************************************************************/
unsigned char __video_fmt_debounce( unsigned char ch, unsigned char value )
{
	int i;
	unsigned char retfmt;
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VIVO_CH_FORMATDEF debouncebuf[8]={0,0,0,0,0,0,0,0};

	for(i=0; i<8; i++)
	{
		sVFC.Ch = ch%4;
		sVFC.Dev_Num = ch/4;
		NC_VD_AUTO_VFC_Get(&sVFC);
		debouncebuf[i] = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		Delay_ms(30);
	}

	/* if The value of three buffers are same, return retfmt */
	retfmt = value;

	if((retfmt == debouncebuf[0])&&(retfmt == debouncebuf[1])&&(retfmt == debouncebuf[2])&&(retfmt == debouncebuf[3])&&
	   (retfmt == debouncebuf[4])&&(retfmt == debouncebuf[5])&&(retfmt == debouncebuf[6])&&(retfmt == debouncebuf[7]))
    {
		return retfmt;
    }
	else
	{
		return NC_VIVO_CH_FORMATDEF_UNKNOWN;
	}
}

unsigned char __video_fmt_bank5_debounce( unsigned char ch, unsigned char value )
{
	int i;
	unsigned char retfmt;
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VIVO_CH_FORMATDEF debouncebuf[8]={0,0,0,0,0,0,0,0};

	for(i=0; i<8; i++)
	{
		sVFC.Ch = ch%4;
		sVFC.Dev_Num = ch/4;
		NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);
		debouncebuf[i] = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		Delay_ms(30);
	}

	/* if The value of three buffers are same, return retfmt */
	retfmt = value;

	if((retfmt == debouncebuf[0])&&(retfmt == debouncebuf[1])&&(retfmt == debouncebuf[2])&&(retfmt == debouncebuf[3])&&
	   (retfmt == debouncebuf[4])&&(retfmt == debouncebuf[5])&&(retfmt == debouncebuf[6])&&(retfmt == debouncebuf[7]))
    {
		return retfmt;
    }
	else if((retfmt == debouncebuf[0])&&(retfmt == debouncebuf[1])&&(retfmt == debouncebuf[2])&&(retfmt == debouncebuf[3])&&
			(retfmt == debouncebuf[4])&&(retfmt == debouncebuf[5])&&(retfmt == debouncebuf[6])&&(retfmt == debouncebuf[7]))
	{
		return NC_VIVO_CH_FORMATDEF_UNKNOWN;
	}
	else
	{
		return 0xFF /*N/A*/ ;
	}
}
#endif

void NC_VD_VO_Auto_Data_Mode_Set(unsigned char ch, unsigned char devnum, unsigned char SetVal)
{
	NC_VD_AUTO_DATA_OUT_MODE_STR DataOutMode;

	DataOutMode.Ch = ch%4;
	DataOutMode.devnum = devnum;
	DataOutMode.SetVal = SetVal;

	video_output_data_out_mode_set((video_output_data_out_mode *)&DataOutMode);//NC_VD_AT_DAT_OUT_MODE_Set(&DataOutMode);
}

void NC_VD_AUTO_NoVideo_Set(unsigned char ch, unsigned char devnum)
{	
	NC_VD_AUTO_NOVIDEO_REG_STR AutoNoVidDet;

	AutoNoVidDet.Ch = ch%4;
	AutoNoVidDet.devnum = devnum;
	video_input_no_video_set((video_input_novid_set *)&AutoNoVidDet);
};


/*******************************************************************************
*	Description		: Get video format(each ch)
*	Argurments		: pVFmtSet(raptor3 information)
*	Return value	: N/A
*	Modify			:
*	warning			:
*******************************************************************************/
int RAPTOR3_SAL_GetFormatEachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VD_AUTO_NOVIDEO_STR sNoVideo;
//	motion_mode sMotion;
	NC_VIVO_CH_FORMATDEF oCurVidFmt;
	NC_VIDEO_ONOFF oCurVideoloss;

	/* initialize current video format - pInformation structure is for app */
	pInformation->curvideofmt[ch] = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	pInformation->curvideoloss[ch] = VIDEO_LOSS_OFF;
	pInformation->vfc[ch] = 0xff;

	/* initialize vfc(B5xF0) and videoloss information(B0xA8) */
	sVFC.Ch = ch%4;
	sVFC.Dev_Num = ch/4;
	sNoVideo.Ch = ch%4;
	sNoVideo.Dev_Num = ch/4;

	/* get vfc and videoloss */
	video_input_vfc_read((video_input_vfc *)&sVFC);
	video_input_novid_read((video_input_novid *)&sNoVideo);

	/* check vfc&videoloss and run debounce  */
	if(((((sVFC.VFC >> 4 ) & 0xF) != 0xF) && ((sVFC.VFC & 0x0F) != 0xF)) && !sNoVideo.NoVid) // OnVideo
	{
		/* convert vfc to formatDefine for APP and save videoloss information */
		oCurVidFmt = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		/* debouce */
		pInformation->curvideofmt[ch] = oCurVidFmt;
		pInformation->vfc[ch] = sVFC.VFC;
	}

//	sMotion.ch = ch%4;
//	sMotion.devnum = ch/4;
//	sMotion.fmtdef = pInformation->curvideofmt[ch];

	/* check novideo option */
	if( !sNoVideo.NoVid )
	{
		pInformation->curvideoloss[ch] = VIDEO_LOSS_ON;
		pInformation->videolosscheck[ch] = 0;
//		if(Get_MotionDetect_OnOff(sMotion.ch))
//		{
//			motion_detection_get(&sMotion);
//			pInformation->motiondetect |= sMotion.set_val;
//		}
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
	int iPort = 0;
	unsigned char oChannel = 0x00;
	unsigned char oDevNum = 0x00;
	unsigned char oEdge = 0x00;
	unsigned char oMux;
	unsigned char oInterface;
	unsigned char oClkEdge;
	NC_VD_EQ_STR sEQInfo;

	NC_VO_PORT_FMT_S *pPortFmt;		// output
	NC_VD_DEV_CH_INFO_STR DEV_CH_INFO;

	NC_VIVO_CH_FORMATDEF FmtDef = pInformation->curvideofmt[ch];

	if(pInformation->oMux == VI_WORK_MODE_2Multiplex)
	{

		iPort = (( ch % 4 ) / 2) + 1;
		oChannel = ch%4;
		oDevNum = ch/4;

		oMux = VI_WORK_MODE_2Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_DOUBLE;
	}
	else
	{
		iPort = ch%4;
		oChannel = ch%4;
		oDevNum = ch/4;

		oMux = VI_WORK_MODE_1Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_SINGLE_UP;
	}

	/* Actually, Now, this routine don't use, because the NC_VD_VO_Mode_Set calculate Edge */
	if(FmtDef == AHD30_4M_30P || \
			FmtDef == AHD30_4M_25P || \
			FmtDef == AHD30_3M_30P || \
			FmtDef == AHD30_3M_25P || \
			FmtDef == AHD30_5M_20P || \
			FmtDef == AHD30_5_3M_20P || \
			FmtDef == AHD30_6M_18P || \
			FmtDef == AHD30_6M_20P || \
			FmtDef == AHD30_8M_15P || \
			FmtDef == AHD30_8M_12_5P || \
			FmtDef == CVI_4M_25P || \
			FmtDef == CVI_4M_30P || \
			FmtDef == CVI_8M_12_5P || \
			FmtDef == CVI_8M_15P || \
			FmtDef == TVI_8M_12_5P || \
			FmtDef == TVI_8M_15P || \
			FmtDef == TVI_4M_25P || \
			FmtDef == TVI_4M_30P || \
			FmtDef == TVI_5M_20P
			)
	{
		if(oMux == VI_WORK_MODE_2Multiplex)
		{
			//fprintf(stdout, " Not Support This Format .. High/RT Fmt:[%d]\n", FmtDef);
		}
		else
		{
			oEdge = VI_CLK_EDGE_DOUBLE;
		}
	}
	else
	{
		if(oMux == VI_WORK_MODE_2Multiplex)
		{
			oClkEdge = VI_CLK_EDGE_DOUBLE;
		}
		else
		{
			oClkEdge = VI_CLK_EDGE_SINGLE_UP;
		}
	}

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( FmtDef ); // output

	DEV_CH_INFO.Ch = oChannel;
	DEV_CH_INFO.Dev_Num = oDevNum;
	DEV_CH_INFO.Fmt_Def = FmtDef;
	video_input_onvideo_set((decoder_dev_ch_info_s *)&DEV_CH_INFO);//NC_VD_AUTO_ONVIDEO_SET(&DEV_CH_INFO);

      /* This Part is enough EQ Setting */
	video_input_h_timing_set((decoder_dev_ch_info_s *)&DEV_CH_INFO);
	video_input_color_set((decoder_dev_ch_info_s *)&DEV_CH_INFO);
    	video_input_vafe_init((decoder_dev_ch_info_s *)&DEV_CH_INFO);
#ifndef USE_EXTENDED_RESOLUTION
    	sEQInfo.Ch = oChannel;
	sEQInfo.Dev_Num = oDevNum;
	sEQInfo.distance = SHORT_2M;
	sEQInfo.FmtDef = FmtDef;
    	video_input_eq_val_set((video_equalizer_info_s *)&sEQInfo);	//kukuri
#endif

	if(FmtDef == AHD20_SD_H960_2EX_Btype_NT || \
			FmtDef == AHD20_SD_H960_2EX_Btype_PAL || \
			FmtDef == AHD20_720P_30P_EX_Btype || \
			FmtDef == AHD20_720P_25P_EX_Btype || \
			FmtDef == TVI_HD_30P_EX || \
			FmtDef == TVI_HD_25P_EX || \
			FmtDef == TVI_HD_B_30P_EX || \
			FmtDef == TVI_HD_B_25P_EX || \
			FmtDef == CVI_HD_30P_EX || \
			FmtDef == CVI_HD_25P_EX
			)
	{
	    	NC_VD_VO_Mode_Set_New( oChannel, oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, (ch%4)+8, (ch%4)+8, (ch%4)+8, (ch%4)+8 );
	}
	else
	{
	    	NC_VD_VO_Mode_Set_New( oChannel, oDevNum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );
    	}

	return 0;
}

#if 0
/*******************************************************************************
 *	Description		: Get video format bank13(each ch)
 *	Argurments		: pVFmtSet(raptor3 information)
 *	Return value	: N/A
 *	Modify			:
 *	warning			:
 *******************************************************************************/
int RAPTOR3_SAL_GetFormat_Bank5_EachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation )
{
	NC_VD_AUTO_VFC_STR sVFC;
	NC_VD_AUTO_NOVIDEO_STR sNoVideo;
	NC_VIVO_CH_FORMATDEF oCurVidFmt;
	NC_VIDEO_ONOFF oCurVideoloss;

	/* initialize current video format - pInformation structure is for app */
	pInformation->curvideofmt[ch] = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	pInformation->curvideoloss[ch] = VIDEO_LOSS_OFF;
	pInformation->vfc[ch] = 0xff;

	/* initialize vfc(B5xF0) and videoloss information(B0xA8) */
	sVFC.Ch = ch%4;
	sVFC.Dev_Num = ch/4;
	sNoVideo.Ch = ch%4;
	sNoVideo.Dev_Num = ch/4;

	/* get vfc and videoloss */
	NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);
	NC_VD_AUTO_NOVIDEO_Get(&sNoVideo);

	/* check vfc&videoloss and run debounce  */
	if(((((sVFC.VFC >> 4 ) & 0xF) != 0xF) && ((sVFC.VFC & 0x0F) != 0xF)) && !sNoVideo.NoVid) // OnVideo
	{
		/* convert vfc to formatDefine for APP and save videoloss information */
		oCurVidFmt = NC_VD_AUTO_VFCtoFMTDEF(sVFC.VFC);

		/* debouce */
		pInformation->curvideofmt[ch] = __video_fmt_bank5_debounce( ch, oCurVidFmt );
		pInformation->vfc[ch] = sVFC.VFC;
	}

	/* check novideo option */
	if( !sNoVideo.NoVid )
	{
		pInformation->curvideoloss[ch] = VIDEO_LOSS_ON;
	}

	return 0;
}
#endif

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
	unsigned char		oDevNum = 0;
	unsigned char		oDebncIdx = 0;
	unsigned char 		oVfc = 0;
	NC_VIVO_CH_FORMATDEF oFmtB5Def;
	NC_VD_AUTO_VFC_STR	sVFC;
	NC_VD_DEV_CH_INFO_STR sDevChInfo;


	sVFC.Ch = ch % 4;
	sVFC.Dev_Num = ch / 4;
	video_input_onvideo_check_data((video_input_vfc *)&sVFC);//NC_VD_AUTO_ONVIDEO_CHECK(&sVFC);

	oDebncIdx = pInformation->debounceidx[ch];
	pInformation->debounce[ch][oDebncIdx%MAX_DEBOUNCE_CNT] = sVFC.VFC;

	pInformation->debounceidx[ch]++;
	pInformation->debounceidx[ch] = ( (pInformation->debounceidx[ch] % MAX_DEBOUNCE_CNT) == 0 ) ? 0 : pInformation->debounceidx[ch];

	oVfc = pInformation->debounce[ch][pInformation->debounceidx[ch]];
	for( i = 0; i < MAX_DEBOUNCE_CNT; i++ )
	{
		if( oVfc != pInformation->debounce[ch][i])
		{
			break;
		}
	}
	if( i == MAX_DEBOUNCE_CNT )
	{
		oFmtB5Def = NC_VD_AUTO_VFCtoFMTDEF(oVfc);

			if( (
					( oFmtB5Def != AHD30_5M_20P )  &&
					( oFmtB5Def != CVI_8M_15P )    &&
					( oFmtB5Def != CVI_8M_12_5P )  &&
					( oFmtB5Def != CVI_HD_30P_EX ) &&
					( oFmtB5Def != CVI_4M_25P)  &&
					( oFmtB5Def != CVI_FHD_30P )
				)
					&&
					( oFmtB5Def != pInformation->prevideofmt[ch] )
			   )
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
	NC_VD_DEV_CH_INFO_STR DEV_CH_INFO;
	NC_VD_EQ_STR sEQInfo;

	NC_VIVO_CH_FORMATDEF oFmtDef = pInformation->curvideofmt[ch];
	//video_input_novid_set AutoNoVidDet;

	if(pInformation->oMux == VI_WORK_MODE_2Multiplex)
	{
		oChannel = ch % 4;
		devnum = ch / 4;
		iPort = (( ch % 4 ) / 2) + 1;

		oMux = VI_WORK_MODE_2Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_DOUBLE;
	}
	else
	{
		oChannel = ch % 4;
		devnum = ch / 4;
		iPort = ch % 4;

		oMux = VI_WORK_MODE_1Multiplex;
		oInterface = VI_INPUT_MODE_BT656;
		oClkEdge = VI_CLK_EDGE_SINGLE_UP;
	}

	DEV_CH_INFO.Ch = oChannel;
	DEV_CH_INFO.Dev_Num = devnum;
	DEV_CH_INFO.Fmt_Def = oFmtDef;

	/* This Part is enough EQ Setting */
	video_input_h_timing_set((decoder_dev_ch_info_s *)&DEV_CH_INFO);
	video_input_color_set((decoder_dev_ch_info_s *)&DEV_CH_INFO);
    	video_input_vafe_init((decoder_dev_ch_info_s *)&DEV_CH_INFO);
#ifndef USE_EXTENDED_RESOLUTION
    	//video_input_vfmt_set((decoder_dev_ch_info_s *)&DEV_CH_INFO);	//kukuri

    	sEQInfo.Ch = oChannel;
	sEQInfo.Dev_Num = devnum;
	sEQInfo.distance = SHORT_2M;
	sEQInfo.FmtDef = oFmtDef;
    	video_input_eq_val_set((video_equalizer_info_s *)&sEQInfo);	//kukuri
#endif
	
	NC_VD_AUTO_NoVideo_Set(oChannel, devnum);

	pPortFmt = (NC_VO_PORT_FMT_S *)NC_HI_VO_Get_PortFormat( oFmtDef );
	
	if(oFmtDef == AHD20_SD_H960_2EX_Btype_NT || \
		oFmtDef == AHD20_SD_H960_2EX_Btype_PAL || \
		oFmtDef == AHD20_720P_30P_EX_Btype || \
		oFmtDef == AHD20_720P_25P_EX_Btype || \
		oFmtDef == TVI_HD_30P_EX || \
		oFmtDef == TVI_HD_25P_EX || \
		oFmtDef == TVI_HD_B_30P_EX || \
		oFmtDef == TVI_HD_B_25P_EX || \
		oFmtDef == CVI_HD_30P_EX || \
		oFmtDef == CVI_HD_25P_EX
		)
	{
	    	NC_VD_VO_Mode_Set_New( oChannel, devnum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, (ch%4)+8, (ch%4)+8, (ch%4)+8, (ch%4)+8 );
	}
	else
	{
	    	NC_VD_VO_Mode_Set_New( oChannel, devnum, iPort, pPortFmt, oMux, oInterface, oClkEdge/*N/A*/, ch%4, ch%4, ch%4, ch%4 );
    	}
	return 0;
}

