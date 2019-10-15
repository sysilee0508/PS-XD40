#include "common.h"
#include "osd_string.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
//#include "NVP6168.h"
#include "raptor4_fmt.h"
#endif

#define MARGIN_X						5
#define MARGIN_Y						10

#define TITLE_LENGTH					5
#define VIDEO_FORMAT_LENGTH_MAX			20

#define VIDEO_LOSS_LENGTH				8

#define REQUEST_OSD_REFRESH_TITLE			0x01
#define REQUEST_OSD_REFRESH_NOVIDEO		0x02
#define REQUEST_OSD_REFRESH_FORMAT		0x04

static BOOL requestRefreshScreen = CLEAR;

static const sPosition_t titlePosition =
{
		(DISPLAY_WIDTH - (TITLE_LENGTH * CHAR_WIDTH)) / 2, 	//x
		MARGIN_Y	//y
};
static const sPosition_t videoInFormatPosition_Full =
{
		(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, //x
		DISPLAY_HALF_HEIGHT - CHAR_HEIGHT - MARGIN_Y	//y
};
static const sPosition_t videoOutFormatPosition_Full =
{
		(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, //x
		DISPLAY_HALF_HEIGHT + MARGIN_Y	//y
};
static const sPosition_t videoLossPosition_Full =
{
		(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2,
		(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2
};

static const sPosition_t videoInFormatPosition_Split[NUM_OF_SPLIT][NUM_OF_CHANNEL] =
{
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}},
};

static const sPosition_t videoOutFormatPosition_Split[NUM_OF_SPLIT][NUM_OF_CHANNEL] =
{
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT+MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT+MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT+MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT+MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2,DISPLAY_HALF_HEIGHT+MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT+MARGIN_Y}},
};

static const sPosition_t videoLossPosition_Split[NUM_OF_SPLIT+NUM_OF_PIP][NUM_OF_CHANNEL] =
{
	// SPLIT -------------------------------------------------------------------------------------------------------------//
	{{(DISPLAY_HALF_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HALF_HEIGHT - CHAR_HEIGHT) / 2},
			{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HALF_HEIGHT - CHAR_HEIGHT) / 2},
			{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT/2}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2}},
	// PIP --------------------------------------------------------------------------------------------------------------//
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-PIP_POSITION_MARGIN-PIP_WINDOW_WIDTH/2-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2, PIP_POSITION_MARGIN+(PIP_WINDOW_HEIGHT-CHAR_HEIGHT)/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{PIP_POSITION_MARGIN+(PIP_WINDOW_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT/2-CHAR_HEIGHT/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-PIP_POSITION_MARGIN-PIP_WINDOW_WIDTH/2-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2, DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT/2-CHAR_HEIGHT/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{PIP_POSITION_MARGIN+(PIP_WINDOW_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, PIP_POSITION_MARGIN+(PIP_WINDOW_HEIGHT-CHAR_HEIGHT)/2}},
};

//-----------------------------------------------------------------------------
static void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	OSD_EnableSprite(SPRITE_INDEX0, ON);
}

//-----------------------------------------------------------------------------
// Erase
//-----------------------------------------------------------------------------
static void OSD_EraseTitle(void)
{
	OSD_PrintString(titlePosition, osdStr_Space5, TITLE_LENGTH);
}

static void OSD_EraseNoVideo(void)
{
	OSD_PrintString(videoLossPosition_Full, osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[SPLIT_A][CHANNEL1], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[SPLIT_A][CHANNEL2], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[SPLIT_C][CHANNEL1], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[SPLIT_C][CHANNEL2], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[PIP_A+NUM_OF_SPLIT][CHANNEL2], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[PIP_B+NUM_OF_SPLIT][CHANNEL2], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[PIP_C+NUM_OF_SPLIT][CHANNEL2], osdStr_Space8, VIDEO_LOSS_LENGTH);
	OSD_PrintString(videoLossPosition_Split[PIP_D+NUM_OF_SPLIT][CHANNEL2], osdStr_Space8, VIDEO_LOSS_LENGTH);
}

void OSD_EraseVideoFormat(void)
{
	OSD_PrintString(videoInFormatPosition_Full, osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Full, osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);

	OSD_PrintString(videoInFormatPosition_Split[SPLIT_A][CHANNEL1], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_A][CHANNEL1], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoInFormatPosition_Split[SPLIT_A][CHANNEL2], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_A][CHANNEL2], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);

	OSD_PrintString(videoInFormatPosition_Split[SPLIT_C][CHANNEL1], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_C][CHANNEL1], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoInFormatPosition_Split[SPLIT_C][CHANNEL2], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_C][CHANNEL2], osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);

}

//-----------------------------------------------------------------------------
// No Video
//-----------------------------------------------------------------------------
static void OSD_DisplayNoVideo(void)
{
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	eChannel_t iChannel;

	if((GetVideoLossEvent() == SET) || (requestRefreshScreen & REQUEST_OSD_REFRESH_NOVIDEO))
	{
		if(displayMode < DISPLAY_MODE_SPLIT_A)
		{
			iChannel = (eChannel_t)displayMode;
			if(GetInputVideoFormat(iChannel) == NC_VIVO_CH_FORMATDEF_UNKNOWN)//(IsVideoLossChannel(iChannel) == TRUE)
			{
				OSD_PrintString(videoLossPosition_Full, osdStr_NoVideo, VIDEO_LOSS_LENGTH);
			}
			else
			{
				OSD_PrintString(videoLossPosition_Full, osdStr_Space8, VIDEO_LOSS_LENGTH);
			}
		}
		else if(displayMode < DISPLAY_MODE_MAX)
		{
			for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
			{
				if(GetInputVideoFormat(iChannel) == NC_VIVO_CH_FORMATDEF_UNKNOWN)//(IsVideoLossChannel(iChannel) == TRUE)
				{
					OSD_PrintString(
							videoLossPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
							osdStr_NoVideo, VIDEO_LOSS_LENGTH);
				}
				else
				{
					OSD_PrintString(
							videoLossPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
							osdStr_Space8, VIDEO_LOSS_LENGTH);
				}
			}
		}
		requestRefreshScreen &= ~REQUEST_OSD_REFRESH_NOVIDEO;
		SetVideoLossEvent(CLEAR);
	}
}

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
static u8* GetOutVideoFormatString(void)
{
	u8* outStr;

	switch(OutMainFrmt)
	{
		case VIDOUT_1920x1080p60:
			outStr = (u8 *)osdStr_Format_Out_1080p60;
			break;

		case VIDOUT_1920x1080p30:
			outStr = (u8 *)osdStr_Format_Out_1080p30;
			break;

		case VIDOUT_1920x1080p50:
			outStr = (u8 *)osdStr_Format_Out_1080p50;
			break;

		case VIDOUT_1920x1080p25:
			outStr = (u8 *)osdStr_Format_Out_1080p25;
			break;
		default:
			outStr = (u8 *)osdStr_Space20;
			break;

	}
	
	return outStr;
}

static u8* GetInVideoFormatString(eChannel_t channel)
{
	u8* inStr;

	switch(GetInputVideoFormat(channel))
	{
		// 1080p
		case AHD_1080P_30P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p30;
			break;
		case AHD_1080P_25P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p25;
			break;
		case TVI_FHD_30P:
		case TVI_FHD_60P:
			inStr = (u8 *)osdStr_Format_In_TVI_1080p30;
			break;
		case TVI_FHD_25P:
			inStr = (u8 *)osdStr_Format_In_TVI_1080p25;
			break;
		case CVI_FHD_30P:
			inStr = (u8 *)osdStr_Format_In_CVI_1080p30;
			break;
		case CVI_FHD_25P:
			inStr = (u8 *)osdStr_Format_In_CVI_1080p25;
			break;
		// 720p
		case AHD_720P_60P:
		case AHD_720P_30P:
		case AHD_720P_30P_EX:
		case AHD_720P_30P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p30;
			break;
		case AHD_720P_50P:
		case AHD_720P_25P:
		case AHD_720P_25P_EX:
		case AHD_720P_25P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p25;
			break;
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
		case TVI_HD_B_30P:
		case TVI_HD_B_30P_EX:
			inStr = (u8 *)osdStr_Format_In_TVI_720p30;
			break;
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
		case TVI_HD_B_25P:
		case TVI_HD_B_25P_EX:
			inStr = (u8 *)osdStr_Format_In_TVI_720p25;
			break;
		case CVI_HD_60P:
		case CVI_HD_30P:
		case CVI_HD_30P_EX:
			inStr = (u8 *)osdStr_Format_In_CVI_720p30;
			break;
		case CVI_HD_50P:
		case CVI_HD_25P:
		case CVI_HD_25P_EX:
			inStr = (u8 *)osdStr_Format_In_CVI_720p25;
			break;

		case SD_H960_NT:
		case SD_H960_EX_NT:
		case SD_H960_2EX_NT:
		case SD_H960_2EX_Btype_NT:
			inStr = (u8 *)osdStr_Format_In_CVBS_NTSC;
			break;

		case SD_H960_PAL:
		case SD_H960_EX_PAL:
		case SD_H960_2EX_PAL:
		case SD_H960_2EX_Btype_PAL:
			inStr = (u8 *)osdStr_Format_In_CVBS_PAL;
			break;

		default:
			inStr = (u8 *)osdStr_Space20;
			break;
	}
	return inStr;
}

void OSD_RefreshScreen(void)
{
	requestRefreshScreen = REQUEST_OSD_REFRESH_FORMAT | REQUEST_OSD_REFRESH_NOVIDEO | REQUEST_OSD_REFRESH_TITLE;
}

void OSD_DisplayTitle(void)
{
	static eDisplayMode_t preDisplayMode = DISPLAY_MODE_MAX;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();

	if(preDisplayMode != displayMode)
	{
		OSD_PrintString(titlePosition, osdStr_Title[displayMode], strlen(osdStr_Title[displayMode]));
	}
	preDisplayMode = displayMode;
	requestRefreshScreen &= ~ REQUEST_OSD_REFRESH_TITLE;
}

#define DEFAULT_DISPLAY_TIME		5
void OSD_DisplayVideoFormat(void)
{
	static u8 formatDisplayTime[NUM_OF_CHANNEL] = {DEFAULT_DISPLAY_TIME*2, DEFAULT_DISPLAY_TIME*2};
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	eChannel_t iChannel;
	u8* inVideo[NUM_OF_CHANNEL];
	u8* outVideo;
	static u8* preInVideo[NUM_OF_CHANNEL] = {NULL, NULL};
	sSystemTick_t* pSystemTime = GetSystemTime();
	static u32 preTimeInSec;
	static eDisplayMode_t preDisplayMode = DISPLAY_MODE_MAX; 

	//currentTimeInSec = pSystemTime->tickCount_1s;
	//if(requestRefreshScreen & REQUEST_OSD_REFRESH_FORMAT)
	{
		if((preDisplayMode != displayMode) && (preDisplayMode != DISPLAY_MODE_MAX))
		{
			for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
			{
				formatDisplayTime[iChannel] = DEFAULT_DISPLAY_TIME;
				preInVideo[iChannel] = NULL;
			}
		}
		preDisplayMode = displayMode;
		
		if(displayMode < DISPLAY_MODE_SPLIT_A)	// full mode 
		{
			iChannel = (eChannel_t)displayMode;
			if(GetInputVideoFormat(iChannel) != NC_VIVO_CH_FORMATDEF_UNKNOWN)//(IsVideoLossChannel(iChannel) == FALSE)
			{
				if(formatDisplayTime[iChannel] > 0)
				{
					inVideo[iChannel] = GetInVideoFormatString(iChannel);
					outVideo = outVideo = GetOutVideoFormatString();
					if(pSystemTime->tickCount_1s > preTimeInSec)
					{
						formatDisplayTime[iChannel] -= (pSystemTime->tickCount_1s - preTimeInSec);
					}
				}
				else
				{
					inVideo[iChannel] = (u8 *)osdStr_Space20;
					outVideo = (u8 *)osdStr_Space20;
				}

				if(inVideo[iChannel] != preInVideo[iChannel])
				{
					OSD_PrintString(videoInFormatPosition_Full, inVideo[iChannel], strlen((const u8*)inVideo[iChannel]));
					OSD_PrintString(videoOutFormatPosition_Full, outVideo, strlen((const u8*)outVideo));
					if(( formatDisplayTime[iChannel] < DEFAULT_DISPLAY_TIME) && (inVideo[iChannel] != osdStr_Space20))
					{
						formatDisplayTime[iChannel] = DEFAULT_DISPLAY_TIME;
					}
					preInVideo[iChannel] = inVideo[iChannel];
				}
			}
			else// if(preInVideo[iChannel] != osdStr_Space20)
			{
				OSD_PrintString(videoInFormatPosition_Full, osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
				OSD_PrintString(videoOutFormatPosition_Full, osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
				formatDisplayTime[iChannel] = DEFAULT_DISPLAY_TIME;
				preInVideo[iChannel] = (u8 *)osdStr_Space20;
			}
		}
		else if(displayMode < DISPLAY_MODE_PIP_A)	// split mode
		{
			for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
			{
				if(GetInputVideoFormat(iChannel) != NC_VIVO_CH_FORMATDEF_UNKNOWN)//(IsVideoLossChannel(iChannel) == FALSE)
				{
					if(formatDisplayTime[iChannel] > 0)
					{
						inVideo[iChannel] = GetInVideoFormatString(iChannel);
						outVideo = GetOutVideoFormatString();
						if(pSystemTime->tickCount_1s > preTimeInSec)
						{
							formatDisplayTime[iChannel] -= (pSystemTime->tickCount_1s - preTimeInSec);
						}
					}
					else
					{
						inVideo[iChannel] = (u8 *)osdStr_Space20;
						outVideo = (u8 *)osdStr_Space20;
					}

					if(inVideo[iChannel] != preInVideo[iChannel])
					{
						OSD_PrintString(
								videoInFormatPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
								inVideo[iChannel],
								strlen((const u8*)inVideo[iChannel]));
						OSD_PrintString(
								videoOutFormatPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
								outVideo,
								strlen((const u8*)outVideo));
						if(( formatDisplayTime[iChannel] < DEFAULT_DISPLAY_TIME) && (inVideo[iChannel] != osdStr_Space20))
						{
							formatDisplayTime[iChannel] = DEFAULT_DISPLAY_TIME;
						}
						preInVideo[iChannel] = inVideo[iChannel];
					}
				}
				else// if(preInVideo[iChannel] != osdStr_Space20)
				{
					OSD_PrintString(
							videoInFormatPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
							osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
					OSD_PrintString(
							videoOutFormatPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
							osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
					preInVideo[iChannel] = (u8 *)osdStr_Space20;
					formatDisplayTime[iChannel] = DEFAULT_DISPLAY_TIME;
				}
			}
		}
		else		//pip mode
		{
			if(GetInputVideoFormat(CHANNEL1) != NC_VIVO_CH_FORMATDEF_UNKNOWN)//(IsVideoLossChannel(CHANNEL1) == FALSE)
			{
				if(formatDisplayTime[CHANNEL1] > 0)
				{
					inVideo[CHANNEL1] = GetInVideoFormatString(CHANNEL1);
					outVideo = GetOutVideoFormatString();
					if(pSystemTime->tickCount_1s > preTimeInSec)
					{
						formatDisplayTime[CHANNEL1] -= (pSystemTime->tickCount_1s - preTimeInSec);
					}
				}
				else
				{
					inVideo[CHANNEL1] = (u8 *)osdStr_Space20;
					outVideo = (u8 *)osdStr_Space20;
				}

				if(inVideo[CHANNEL1] != preInVideo[CHANNEL1])
				{
					OSD_PrintString(videoInFormatPosition_Full, inVideo[CHANNEL1], strlen((const u8*)inVideo[CHANNEL1]));
					OSD_PrintString(videoOutFormatPosition_Full, outVideo, strlen((const u8*)outVideo));
					if(( formatDisplayTime[CHANNEL1] < DEFAULT_DISPLAY_TIME) && (inVideo[CHANNEL1] != osdStr_Space20))
					{
						formatDisplayTime[CHANNEL1] = DEFAULT_DISPLAY_TIME;
					}
					preInVideo[CHANNEL1] = inVideo[CHANNEL1];
				}
			}
			else// if(preInVideo[CHANNEL1] != osdStr_Space20)
			{
				OSD_PrintString(videoInFormatPosition_Full, osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
				OSD_PrintString(videoOutFormatPosition_Full, osdStr_Space20, VIDEO_FORMAT_LENGTH_MAX);
				preInVideo[CHANNEL1] = (u8 *)osdStr_Space20;
				formatDisplayTime[CHANNEL1] = DEFAULT_DISPLAY_TIME;
			}
		}
		requestRefreshScreen &= ~REQUEST_OSD_REFRESH_FORMAT;
	}
	preTimeInSec = pSystemTime->tickCount_1s;
}

//-----------------------------------------------------------------------------
void Osd_ClearScreen(void)
{
	u16 i;
	BYTE pSTR[DISPLAY_WIDTH/CHAR_WIDTH];

	memset(pSTR, ASCII_SPACE, sizeof(pSTR));

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < DISPLAY_HEIGHT/CHAR_HEIGHT; i++)
	{
		MDINGAC_SetDrawXYMode(i*CHAR_HEIGHT, 0, pSTR, sizeof(pSTR), 0);
	}
}
//-----------------------------------------------------------------------------
void OSD_EraseAllText(void)
{
	OSD_EraseTitle();
	OSD_EraseVideoFormat();
	OSD_EraseNoVideo();
}

//-----------------------------------------------------------------------------
void OSD_Display(void)
{
	OSD_DisplayTitle();
	OSD_DisplayVideoFormat();
	OSD_DisplayNoVideo();
	//requestRefreshScreen = CLEAR;
}
//-----------------------------------------------------------------------------
void OSD_DrawBorderLine(void)
{
	MDINOSD_SetBGBoxColor(RGB(255,255,255));
	MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);

	switch(GetCurrentDisplayMode())
	{
		// Split Vertical
		case DISPLAY_MODE_SPLIT_A:
		case DISPLAY_MODE_SPLIT_B:
		case DISPLAY_MODE_SPLIT_E:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_HALF_WIDTH-2, 0, 2, DISPLAY_HEIGHT);
			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;

		// Split Horizontal
		case DISPLAY_MODE_SPLIT_C:
		case DISPLAY_MODE_SPLIT_D:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;

		case DISPLAY_MODE_PIP_A:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN + PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		case DISPLAY_MODE_PIP_B:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				PIP_POSITION_MARGIN+PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		case DISPLAY_MODE_PIP_C:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT - PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		case DISPLAY_MODE_PIP_D:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN + PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				PIP_POSITION_MARGIN + PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		default:
			MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
			break;
	}
}
