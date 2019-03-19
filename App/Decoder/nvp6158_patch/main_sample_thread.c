#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>

#include "api/raptor3_api.h"
#include "api/raptor3_api_drv.h"
#include "api/video_auto_detect.h"
#include "api/video_fmt_info.h"
#include "api/coax_cmd_def.h"
#include "video_decoder.h"

#ifdef USE_SOC_API
#include "nc_hi_vivo.h"
#include "soc_api.h"
#include "_debug_macro.h"

#define SOC_STATUS_CHANGE  1
#define SOC_DEFAULT  	   0
#endif

#define MAIN_SAMPLE_MAJOR_VER	   1
#define MAIN_SAMPLE_MINOR_VER	   1

#define MAIN_SAMPLE_DRVIER_MAJOR_VER	1
#define MAIN_SAMPLE_DRIVER_MINOR_VER    0

static pthread_t		s_tid_onedetection = 0;
RAPTOR3_INFORMATION_S	s_raptor3_vfmts;
pthread_mutex_t			s_hMutex;

static unsigned int		s_iMuxMode = 0;
static unsigned int 	s_MuxChannel = 0;
static unsigned int 	s_CurChannel = 0;
unsigned char			s_ManualTrigger[16] = {0, }; // 1:trigger on, 0:trigger off

static unsigned char	s_coax_bits[16] = {0, };


/*******************************************************************************
 *	Description		: clear screen and print the message
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
char *msg[] = {
	"=============================\n"
		"  Decoder test \n"
		"=============================\n"
		"1, LIVE   : set mux\n"
		"2, EQ     : set manual eq(0~5)\n"
		"3, COAX   : control osd\n"
		"4, VIDEO  : ch information\n"
		"=============================\n"
		"5, COAX  : 8bit<->16bit(AHD)\n"
		"=============================\n"
		"q, quit the program\n"
		"h, help\n"
		"=======================\n"};

void __clear_screen_print_message( void )
{
	fprintf(stdout, "\033[2J");
	fprintf(stdout, "\033[1;1H");
	fprintf(stdout, *msg);
}

/*******************************************************************************
 *	Description		: set soc mux
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
void __set_soc_mux( void )
{
#ifdef USE_SOC_API
	NC_HI_MUX_ATTR_S 	muxStr={0,};
	unsigned char 		Mux_w = 2, Mux_h = 2;

	if( s_iMuxMode <= 3 )
	{
		muxStr.single_en         = 1;
		muxStr.single_orgsize_en = 0;
		muxStr.single_ch = s_MuxChannel%4;

		/* save current channel */
		s_CurChannel = s_MuxChannel % 4;
		//printf("s_CurChannel : %d\n", s_MuxChannel);
		s_MuxChannel++;
		s_MuxChannel = s_MuxChannel % 4;

		/* mux mode */
		s_iMuxMode++;
	}
	else if( s_iMuxMode == 4 )
	{
		muxStr.w = 2; muxStr.h = 2;
		s_iMuxMode++;
	}
	else if( s_iMuxMode == 5 )
	{
		muxStr.w = 3; muxStr.h = 3;
		s_iMuxMode++;
	}
	else if( s_iMuxMode == 6 )
	{
		muxStr.w = 4; muxStr.h = 4;
		s_iMuxMode = 0;
	}

	NC_HI_VO_MuxAttrSet( &muxStr );
#endif
}

/*******************************************************************************
 *	Description		: set manual eq
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
void __set_manual_eq( void )
{
	unsigned int i;
	unsigned char input[128] = {0, };
	unsigned char oFmtDef = 0;
	unsigned char oLogicalChannel = s_CurChannel;


	/* exception processing */
	if( s_raptor3_vfmts.curvideoloss[oLogicalChannel] != VIDEO_LOSS_ON )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] CH:%d, VIDEO LOSS\n", __func__, __LINE__, oLogicalChannel );
		return;
	}

	while( 1 )
	{
		i = 0;
		printf("command[EQ stage 0 ~ 5, q:Exit] : ");
		while((input[i++] = getchar()) != '\n' )
			input[i] = '\0';

		switch( input[0] )
		{
			case 'q':
				fprintf( stdout, "Exit Manual EQ!\n");
				return;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
				oFmtDef = s_raptor3_vfmts.prevideofmt[oLogicalChannel];
				NC_APP_VD_MANUAL_VIDEO_EQ_Set( oLogicalChannel, atoi(input), oFmtDef);
				fprintf( stdout, ">>>>> APP[CH:%d] Set Manual EQ, stage[%d]\n", oLogicalChannel, atoi(input));
				break;
			default:
				break;
		}
	}
}

/*******************************************************************************
 *	Description		: control coaxial
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
void __control_coaxial( void )
{
	unsigned int i;
	unsigned char input[128] = {0, };
	NC_FORMAT_STANDARD   eFmtStd;
	NC_FORMAT_RESOLUTION eFmtRes;
	NC_FORMAT_FPS        eFmtFps;
	unsigned char		 oDevNum;
	NC_VI_CH_FMT_S 		*pChFmt;
	unsigned char oLogicalChannel = s_CurChannel;
	unsigned char oFmtDef = s_raptor3_vfmts.prevideofmt[s_CurChannel];


	/* exception processing */
	if( s_raptor3_vfmts.curvideoloss[oLogicalChannel] != VIDEO_LOSS_ON )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] CH:%d, VIDEO LOSS\n", __func__, __LINE__, oLogicalChannel );
		return;
	}

	pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( oFmtDef );
	eFmtStd = pChFmt->format_standard;
	eFmtRes = pChFmt->format_resolution;
	eFmtFps = pChFmt->format_fps;
	oDevNum = oLogicalChannel / 4;

	fprintf( stdout, "Control Coaxial, CH:%d\n", s_CurChannel );

	while( 1 )
	{
		i = 0;
		printf("command[enter:0, right:1, left:2, up:3, down:4, q:Exit] : ");
		while((input[i++] = getchar()) != '\n' )
			input[i] = '\0';

		switch( input[0] )
		{
			case 'q':
				fprintf( stdout, "Exit Coaxial!\n");
				return;
			case '0':
				if( s_coax_bits[oLogicalChannel] == 0 )
					NC_APP_VD_COAX_Tx_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_OSD_ENTER );
				else
					NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_OSD_ENTER );
				break;
			case '1':
				if( s_coax_bits[oLogicalChannel] == 0 )
					NC_APP_VD_COAX_Tx_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_RIGHT );
				else
					NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_RIGHT );
				break;
			case '2':
				if( s_coax_bits[oLogicalChannel] == 0 )
					NC_APP_VD_COAX_Tx_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_LEFT );
				else
					NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_LEFT );
				break;
			case '3':
				if( s_coax_bits[oLogicalChannel] == 0 )
					NC_APP_VD_COAX_Tx_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_UP );
				else
					NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_UP );
				break;
			case '4':
				if( s_coax_bits[oLogicalChannel] == 0 )
					NC_APP_VD_COAX_Tx_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_DOWN );
				else
					NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( oDevNum, oLogicalChannel, eFmtStd, eFmtRes, eFmtFps, COAX_CMD_PTZ_DOWN );
				break;
			default:
				break;
		}
	}
}

/*******************************************************************************
 *	Description		: display current video format
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
void __display_curvidfm( void )
{
	int i;

	for( i = 0; i < (s_raptor3_vfmts.chipinform.Total_Chip_Cnt*4); i++)
	{
		fprintf( stdout, "CH:%d[0x%02x] ", i, s_raptor3_vfmts.prevideofmt[i] );
	}
	fprintf( stdout, "\n");
}

/*******************************************************************************
 *	Description		: change 8bits<->16bits(AHD)
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
void __change_coax_bits_ahd( void )
{
	unsigned char		 oDevNum;
	NC_VI_CH_FMT_S 		*pChFmt;
	unsigned char oLogicalChannel = s_CurChannel;
	unsigned char oFmtDef = s_raptor3_vfmts.prevideofmt[s_CurChannel];


	/* exception processing */
	if( s_raptor3_vfmts.curvideoloss[oLogicalChannel] != VIDEO_LOSS_ON )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] CH:%d, VIDEO LOSS\n", __func__, __LINE__, oLogicalChannel );
		return;
	}

	s_coax_bits[oLogicalChannel] = (++s_coax_bits[oLogicalChannel]%2 == 0 ) ? 0 : s_coax_bits[oLogicalChannel];
	fprintf( stdout, " Coaxial[CH:%d] Set [Toggle : %s], Videoformat:0x%x\n", oLogicalChannel, (s_coax_bits[oLogicalChannel] == 0 ) ? "AHD 8bits" : "AHD 16bits", oFmtDef );

	pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( oFmtDef );
	oDevNum = oLogicalChannel / 4;

	/* set Coaxial */
	if( s_coax_bits[oLogicalChannel] == 0 )
	{
		NC_APP_VD_COAX_Tx_Init( oDevNum, oLogicalChannel, pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );
		NC_APP_VD_COAX_Rx_Init( oDevNum, oLogicalChannel, pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );
	}
	else
	{
		NC_APP_VD_COAX_Tx_16Bit_Init(oDevNum, oLogicalChannel, pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );
	}
}

/*******************************************************************************
 *	Description		: app start
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *	pthread_mutex_lock( &s_hMutex );
 *	pthread_mutex_unlock( &s_hMutex );
 *******************************************************************************/
int AppStart( void )
{
	unsigned int i;
	unsigned char input[128] = {0, };

	/* clear screen and print the message and wait for user command */
	__clear_screen_print_message();
	while( 1 )
	{
		i = 0;
		printf("command[main] : ");
		while((input[i++] = getchar()) != '\n' )
			input[i] = '\0';

		switch( input[0] )
		{
			case 'q':
				fprintf( stdout, "Exit program!\n");
				exit(0);

			case 'h':
				fprintf(stdout, *msg);
				break;

			case '1':
				__set_soc_mux();
				break;

			case '2':
				__set_manual_eq();
				break;

			case '3':
				__control_coaxial();
				break;

			case '4':
				__display_curvidfm();
				break;

			case '5':
				__change_coax_bits_ahd();
				break;

			default:
				break;
		}
	}

	return 0;
}

/*******************************************************************************
 *	Description		:
 *	Argurments		:
 *	Return value	:
 *	Modify			:
 *	warning			:
 *******************************************************************************/
#define DISTINGUISH_MAX_NUM	5
static unsigned int CVI_720P30[16]={0,};
static unsigned int DISTINGUISH_CNT[16] = { 0, };
static unsigned int CVI_1080P30[16] = {0, };
static unsigned int __First_Boot_DetectResult_CVI1080P[16] = {0, };

void *OneThreadProcDetection( void* arg )
{
	int i;
	int ret;
	unsigned char			oDevNum = 0;
	unsigned char 			Mux_w = 2, Mux_h = 2;
	unsigned char 			oCurVideofmt = 0x00;
	unsigned char 			oPreVideofmt = 0x00;
	unsigned char 			CableDistance = 0;
	unsigned char 			oLogicalChannel = 0;
	NC_VI_CH_FMT_S 			*pChFmt;
	NC_VIVO_CH_FORMATDEF	oFmtDef;
	NC_VIVO_CH_FORMATDEF 	oFmtB5Def;
	NC_VD_DEV_CH_INFO_STR 	sDevChInfo;

#ifdef USE_SOC_API
	VO_PUB_ATTR_S 			stPubAttr = {0,};
	NC_HI_MUX_ATTR_S 		muxStr={0,};
	int 					socChangeStatus[16] = {SOC_DEFAULT, };
#endif

	fprintf( stdout, ">>>>> Create OneThreadProcDetection\n" );

	pthread_mutex_init(&s_hMutex, NULL);

	/* loop */
	while(1)
	{
		for( oLogicalChannel = 0; oLogicalChannel < (s_raptor3_vfmts.chipinform.Total_Chip_Cnt*4); oLogicalChannel++ )
		{
			/*****************************************************************
			 *
			 * This is Auto mode
			 *
			 *****************************************************************/

			sDevChInfo.Ch = oLogicalChannel;
			sDevChInfo.Dev_Num = oDevNum;

			/* get video format */
			RAPTOR3_SAL_GetFormatEachCh( oLogicalChannel, &s_raptor3_vfmts );

			/* process video format on/off */
			oCurVideofmt  = s_raptor3_vfmts.curvideofmt[oLogicalChannel];
			oPreVideofmt  = s_raptor3_vfmts.prevideofmt[oLogicalChannel];

			if( s_raptor3_vfmts.curvideoloss[oLogicalChannel] == VIDEO_LOSS_ON)
			{
				/* on video */
				if( (oCurVideofmt != NC_VIVO_CH_FORMATDEF_UNKNOWN) && (oPreVideofmt == NC_VIVO_CH_FORMATDEF_UNKNOWN) )
				{
					oFmtDef = NC_VD_AUTO_VFCtoFMTDEF( s_raptor3_vfmts.vfc[oLogicalChannel] );
					sDevChInfo.Fmt_Def = oFmtDef;

					fprintf(stdout,"[CH:%d] >> Now Format Define Value : [%02x]\n", oLogicalChannel, oFmtDef);

#ifdef				SUPPORT_TVI_5M_20P
					if(oFmtDef == AHD30_5M_20P )
						{
							NC_VD_VI_AHD_TVI_Ditinguish(&sDevChInfo);
							oFmtDef = sDevChInfo.Fmt_Def;

							if( oFmtDef == TVI_5M_20P )
								s_raptor3_vfmts.curvideofmt[oLogicalChannel] = TVI_5M_20P;
						}
#endif
#ifdef				SUPPORT_TVI_8M
					if( oFmtDef == CVI_8M_15P || oFmtDef == CVI_8M_12_5P || oFmtDef == CVI_4M_25P )
					{
						NC_VD_VI_CVI_TVI_Ditinguish(&sDevChInfo);
						oFmtDef = sDevChInfo.Fmt_Def;
						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = sDevChInfo.Fmt_Def;
					}
#endif
#ifdef				SUPPORT_AHD_CVI_1M_ACCURACY_UP
					if( oFmtDef == AHD20_720P_30P_EX_Btype )
					{
						if(CVI_720P30[oLogicalChannel] == 0)
						{
							oFmtDef = CVI_HD_30P_EX;
							s_raptor3_vfmts.curvideofmt[oLogicalChannel] = CVI_HD_30P_EX;
							CVI_720P30[oLogicalChannel] = 1;
						}
					}
#endif
#ifdef				SUPPORT_TVI_1M_A_2_B
					if(oFmtDef == TVI_HD_30P_EX || oFmtDef == TVI_HD_25P_EX)
					{
						NC_FORMAT_STANDARD   format_standard=FMT_TVI;
						NC_FORMAT_RESOLUTION format_resolution=FMT_720P_EX;
						NC_FORMAT_FPS        format_fps=FMT_30P;

						switch( oFmtDef )
						{
							case TVI_HD_30P_EX : format_fps=FMT_30P; break;
							case TVI_HD_25P_EX : format_fps=FMT_25P; break;
							default : break;
						}

						NC_APP_VD_COAX_Tx_Init( oLogicalChannel/4, oLogicalChannel, format_standard, format_resolution, format_fps );
						NC_APP_VD_COAX_Tx_Command_Send_Set( oLogicalChannel/4, oLogicalChannel, format_standard, format_resolution, format_fps, COAX_CMD_TVI_1M_A_B_TYPE_CHANGE_A );
						fprintf(stdout, "TVI HD A Type to B type Send COAX CMD!!!!\n");
						continue;
					}
#endif
#ifdef 				SUPPORT_AHD_CVI_2M_ACCURACY_UP
					if( oFmtDef == CVI_FHD_30P )
					{
						DBG_INFO("[CH:%d] >>> First finding format CVI 2M 30P \n", oLogicalChannel );
						NC_VD_VI_AHD_CVI_Distinguish( &sDevChInfo );
						oFmtDef = sDevChInfo.Fmt_Def;
						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = oFmtDef;

						CVI_1080P30[oLogicalChannel] = 1;
					}
#endif
					/* set video format(DEC) */
					RAPTOR3_SAL_OnVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

					/* EQ - get stage and set eq value and show decoder */
					NC_APP_VD_MANUAL_VIDEO_EQ_Set(oLogicalChannel, 0, oFmtDef);

					NC_VD_AUTO_MANUAL_AGC_STABLE_DISABLE(&sDevChInfo);

					CableDistance = NC_APP_VD_MANUAL_CABLE_DISTANCE_Get( oLogicalChannel, oFmtDef );
					NC_APP_VD_MANUAL_VIDEO_EQ_Set(oLogicalChannel, CableDistance, oFmtDef);

#ifdef 				SUPPORT_AHD_CVI_1M_ACCURACY_UP
					if(oFmtDef == CVI_HD_30P_EX && CVI_720P30[oLogicalChannel] == 1)
					{
						/* hide decoder */
						NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,	OUT_MODE_OFF );
					//	usleep( 1500 * 1000 );
					}
					else
						NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_ON );
#else
					NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_ON );
#endif

					/* set Coaxial */
					pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( oFmtDef );
					NC_APP_VD_COAX_Tx_Init( oDevNum, oLogicalChannel, pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );
					NC_APP_VD_COAX_Rx_Init( oDevNum, oLogicalChannel, pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );

#ifdef USE_SOC_API
					socChangeStatus[oLogicalChannel] = SOC_STATUS_CHANGE;
#endif

					s_raptor3_vfmts.prevideofmt[oLogicalChannel] = s_raptor3_vfmts.curvideofmt[oLogicalChannel];

					fprintf( stdout, ">>>>> CH[%d], Set, On video set : 0x%02X\n", oLogicalChannel, s_raptor3_vfmts.curvideofmt[oLogicalChannel] );
				}
				else
				{
					/* check debouce video format(-1:changed, 0:not checked) when you changed video format */
					ret = RAPTOR3_SAL_AutoDebouceCheck( oLogicalChannel, &s_raptor3_vfmts );

#ifdef				SUPPORT_AHD_CVI_1M_ACCURACY_UP
					if(s_raptor3_vfmts.curvideofmt[oLogicalChannel]==CVI_HD_30P_EX && CVI_720P30[oLogicalChannel] == 1)
						{
						NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,	OUT_MODE_ON );
							//CVI_720P30[oLogicalChannel] = 0;
						}
#endif
#ifdef 				SUPPORT_AHD_CVI_2M_ACCURACY_UP
					if( CVI_1080P30[oLogicalChannel] &&  !__First_Boot_DetectResult_CVI1080P[oLogicalChannel] )
					{
						CVI_1080P30[oLogicalChannel] = 0;
						__First_Boot_DetectResult_CVI1080P[oLogicalChannel] = 1;

						ret = -1;
					}
#endif

					if( ret == -1 )
					{
						sDevChInfo.Ch = oLogicalChannel % 4;
						sDevChInfo.Dev_Num = oDevNum;
						sDevChInfo.Fmt_Def = CVI_HD_30P_EX;
						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = CVI_HD_30P_EX;
                                                CableDistance = 0;

						/* hide decoder */
						NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_OFF );

						/* decoder afe power down */
						NC_VD_AUTO_AFE_DOWN(&sDevChInfo);
						/* set no video- first(i:channel, raptor3_vfmts:information */
						RAPTOR3_SAL_NoVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

						/* decoder afe power up */
						NC_VD_AUTO_AFE_UP(&sDevChInfo);

						/* for forced agc stable */
						NC_VD_AUTO_MANUAL_AGC_STABLE_ENABLE(&sDevChInfo);
						usleep(500*1000);

						/* Auto Debounce Buffer Clear */
						s_raptor3_vfmts.debounceidx[oLogicalChannel] = 0;
						for(i  = 0; i < MAX_DEBOUNCE_CNT; i++)
							s_raptor3_vfmts.debounce[oLogicalChannel][i] = 0xff;

#ifdef USE_SOC_API
						socChangeStatus[oLogicalChannel] = SOC_STATUS_CHANGE;
#endif

					/* clear prevideofmt */
						s_raptor3_vfmts.prevideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

						fprintf( stdout, ">>>>> CH[%d], Reset, No video set: 0x%02X\n", oLogicalChannel, CVI_HD_30P_EX );
					}
				}
			}
			else
			{
				/* no video */
				if( oPreVideofmt != NC_VIVO_CH_FORMATDEF_UNKNOWN )
				{
					NC_VD_VO_Auto_Data_Mode_Set( oLogicalChannel, oDevNum,  OUT_MODE_OFF );

					s_raptor3_vfmts.curvideofmt[oLogicalChannel] = CVI_HD_30P_EX;
                    CableDistance = 0;

					/* set no video- first(i:channel, raptor3_vfmts:information */
					RAPTOR3_SAL_NoVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

#ifdef				SUPPORT_AHD_CVI_1M_ACCURACY_UP
					CVI_720P30[oLogicalChannel] = 0;
#endif
#ifdef				SUPPORT_AHD_CVI_2M_ACCURACY_UP
					CVI_1080P30[oLogicalChannel] = 0;
#endif

					/* Auto Debounce Buffer Clear */
					s_raptor3_vfmts.debounceidx[oLogicalChannel] = 0;
					for(i  = 0; i < MAX_DEBOUNCE_CNT; i++)
						s_raptor3_vfmts.debounce[oLogicalChannel][i] = 0xff;

#ifdef USE_SOC_API
					socChangeStatus[oLogicalChannel] = SOC_STATUS_CHANGE;
#endif
					/* clear prevideofmt */
					s_raptor3_vfmts.prevideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

					fprintf( stdout, ">>>>> CH[%d], Set, No video set : 0x%02X\n", oLogicalChannel, CVI_HD_30P_EX );
				}

			}

#ifdef USE_SOC_API
			if( socChangeStatus[oLogicalChannel] == SOC_STATUS_CHANGE )
			{
				if( s_raptor3_vfmts.oMux == VI_2MULTIPLEX_MODE)
					SOC_SAL_Video_2MuxSetFormat(oLogicalChannel, &s_raptor3_vfmts.curvideofmt);
				else
					SOC_SAL_Video_SetFormat( oLogicalChannel, &s_raptor3_vfmts.curvideofmt, CableDistance );
                    /* clear curvideofmt */
                    s_raptor3_vfmts.curvideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

				socChangeStatus[oLogicalChannel] = SOC_DEFAULT;
			}
#endif
		}

		/* sleep */
		usleep(200 * 1000);
	}
}

/*******************************************************************************
 *	Description		: start main
 *	Argurments		:
 *	Return value	:
 *	Modify			:
 *	warning			:
 *******************************************************************************/
int main(int argc, char *argv[])
{
	int i;
	int 				iTotalChipCnt = 0;
#ifdef USE_SOC_API
	VO_PUB_ATTR_S 		stPubAttr = {0,};
	NC_HI_MUX_ATTR_S 	muxStr={0,};
#endif
	unsigned char 		Mux_w = 2, Mux_h = 2;	// Soc Mux
	VDEC_DEV_INFORM_S*	pDevInfo = &s_raptor3_vfmts.chipinform;


	/* Initialize SoC(VO, FB, MouseInput, VI, MUX-2x2, 3x3, 4x4) */
#ifdef USE_SOC_API
	stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	stPubAttr.enIntfType = VO_INTF_HDMI;
	stPubAttr.u32BgColor = 0x0000FF;
	muxStr.w = Mux_w;
	muxStr.h = Mux_h;
	NC_HI_VO_Init( &stPubAttr );
	NC_HI_FB_Init();
	NC_HI_VI_Init();
	NC_HI_VO_MuxAttrSet( &muxStr );
#endif

	/* Initialize video decoder and get chip information */
	RAPTOR3_SAL_Init( &s_raptor3_vfmts );

	/* Initialize SoC used chip(Video Decoder)'s information */
#ifdef USE_SOC_API
	SOC_SAL_Init( pDevInfo );
#endif

	/* create thread */
	pthread_create( &s_tid_onedetection, NULL, OneThreadProcDetection, NULL);

	/* loop */
	return AppStart();
}

/*******************************************************************************
 *	End of contents
 *******************************************************************************/
