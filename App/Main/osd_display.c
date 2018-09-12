#include "common.h"

#define DISPLAY_WIDTH_1920X1080			1920
#define DISPLAY_HEIGHT_1920x1080		1080
#define NUM_OF_POSITION					6
#define MARGIN_X						2
#define MARGIN_Y						4

#define DATE_TIME_LENGTH				20 // "yyyy-mm-dd hh:mm:ss "

typedef struct
{
	u16 pos_x;
	u16 pos_y;
} sPosition_t;

struct osd_location 
{
	u8 state;
	u8 length;
	sPosition_t location;
};

struct osd_location osd_ch_name_location_buf[NUM_OF_CHANNEL];
struct osd_location osd_video_lose_location_buf[NUM_OF_CHANNEL];
struct osd_location osd_freeze_autoseq_location_buf;

static const sPosition_t tbl_OSD_SPLIT4_POSITION_1920x1080[NUM_OF_CHANNEL][NUM_OF_POSITION] =
{
//  TopLeft		TopCenter		TopRight    	BottomLeft		BottomCenter	BottomRight
	{{0, 0},  	{480, 0},		{960, 0},		{0, 540},		{480, 540},		{960, 540}},	//CH01
	{{960, 0}, 	{1440, 0},		{1920, 0},		{960, 540},		{1440, 540},	{1920, 540}},	//CH02
	{{0, 540},  {480, 540},		{960, 540},		{0,1080},		{480,1080},		{960,1080}},	//CH03
	{{960, 540}, {1440, 540},  	{1920, 540},	{960,1080},		{1440,1080},	{1920,1080}}	//CH04
};

static const u16 tblTimeDisplayLoc_X[TIME_POSITION_MAX] =
{
	6,
	(DISPLAY_WIDTH_1920X1080 - (DATE_TIME_LENGTH*CHAR_WIDTH_E))/2 -12,
	DISPLAY_WIDTH_1920X1080 - (DATE_TIME_LENGTH*CHAR_WIDTH_E) - 6,
};

BYTE bMode_change_flag = CLEAR;
BYTE str_Blank[] = "            ";
//-----------------------------------------------------------------------------
const u8 str_Freeze[] 		= "FREEZE  ";
const u8 str_Freeze2[] 		= "FREEZE";
const u8 str_Freeze3[] 		= "  FREEZE";
const u8 str_Freeze_BLK[] 	= "        ";
const u8 str_Freeze_BLK2[] 	= "      ";
//-----------------------------------------------------------------------------
const u8 str_AUTO[] = "AUTO  ";
const u8 str_AUTO2[] = "AUTO";
const u8 str_AUTO3[] = "  AUTO";
const u8 str_AUTO_BLK[] = "      ";
const u8 str_AUTO_BLK2[] = "    ";
//-----------------------------------------------------------------------------
const u8 str_NO_VIDEO[]= "VIDEO LOSS";
const u8 str_NO_VIDEO_Blk[]= "          ";


//-----------------------------------------------------------------------------
// static Functions
//-----------------------------------------------------------------------------
static BOOL IsTitlePositionTop(void)
{
	eTitlePosition_t titlePosition;
	BOOL result = FALSE;

	Read_NvItem_TitlePosition(&titlePosition);
	if((titlePosition == TITLE_POSITION_TOP_LEFT) ||
		(titlePosition == TITLE_POSITION_TOP_RIGHT) ||
		(titlePosition == TITLE_POSITION_TOP_CENTER) ||
		(titlePosition == TITLE_POSITION_4SPILIT_CENTER))
	{
		result = TRUE;
	}
	return result;
}

static void OSD_Print_String(sPosition_t position, u8 *pData, u16 size)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

static void OSD_Display_Time_Erase(void)
{
	sPosition_t position;
	u8 str_buf[DATE_TIME_LENGTH];
	eTimePosition_t timePosition;

	if(IsTitlePositionTop() == TRUE) // if title position is top
	{
		//if title position is top then time should be located at bottom
		position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT - MARGIN_Y;
	}
	else
	{
		// If not, time position goes top
		position.pos_y = MARGIN_Y;
	}

	Read_NvItem_TimePosition(&timePosition);
	position.pos_x = tblTimeDisplayLoc_X[timePosition];

	memset(str_buf, ' ', sizeof(str_buf));
	OSD_Print_String(position, str_buf, sizeof(str_buf));
}

static void OSD_Str_Loss_Erase(void)
{
	eChannel_t channel;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		if(osd_video_lose_location_buf[channel].state == ON)
		{
			MDINGAC_SetDrawXYMode(osd_video_lose_location_buf[channel].location.pos_y,
					osd_video_lose_location_buf[channel].location.pos_x,
					str_Blank,
					osd_video_lose_location_buf[channel].length,
					0);
		}
	}
}

static void OSD_Str_Freeze_autoseq_Erase(void)
{
	if(osd_freeze_autoseq_location_buf.state == ON)
	{
		MDINGAC_SetDrawXYMode(osd_freeze_autoseq_location_buf.location.pos_y,
				osd_freeze_autoseq_location_buf.location.pos_x,
				str_Blank,
				osd_freeze_autoseq_location_buf.length,
				0);
	}
}

static sPosition_t OSD_TitleStringPosition(eChannel_t channel, eTitlePosition_t titlePosition, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	ReadNvItem_ChannelName(channel_name, channel);
	switch(titlePosition)
	{
		case TITLE_POSITION_TOP_LEFT://0:
		{
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = MARGIN_X;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x + MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
				break;
			}
		}
		break;

		case TITLE_POSITION_TOP_CENTER://1:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = (DISPLAY_WIDTH_1920X1080 - (strlen(channel_name) * CHAR_WIDTH_K)) / 2;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E)/2;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
				break;

			}
		break;

		case TITLE_POSITION_TOP_RIGHT://2:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = DISPLAY_WIDTH_1920X1080 - MARGIN_X - (length * CHAR_WIDTH_K);
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) - MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
				break;

			}
		break;

		case TITLE_POSITION_BOTTOM_LEFT://3:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = MARGIN_X;
					position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x + MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
		break;

		case TITLE_POSITION_BOTTOM_CENTER: //4
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = (DISPLAY_WIDTH_1920X1080 - (length * CHAR_WIDTH_K)) / 2;
					position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) / 2;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
		break;

		case TITLE_POSITION_BOTTOM_RIGHT://5:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = DISPLAY_WIDTH_1920X1080 - MARGIN_X - (length * CHAR_WIDTH_K);
					position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - MARGIN_X - (length * CHAR_WIDTH_E);
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
		break;

		case TITLE_POSITION_4SPILIT_CENTER://6:

			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN: //center-top
					position.pos_x = (DISPLAY_WIDTH_1920X1080 - (length * CHAR_WIDTH_K)) / 2;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					if(channel == CHANNEL1 || channel == CHANNEL2) //center-bottom
					{
						position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) / 2;
						position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
					}
					else if(channel == CHANNEL3 || channel == CHANNEL4) //center-top
					{
						position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E)/2;
						position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
					}
				break;
			}
		break;
	}
	return position;
}

static void DateCon(u8 *buf)
{
	eDateFormat_t dataFormat;
	u8 year[4] = {'2','0',};
	u8 month[2];
	u8 day[2];

	year[2] = (rtc_year >> 4) + '0';
	year[3] = (rtc_year & 0x0f) + '0';
	month[0] = (rtc_month >> 4) + '0';
	month[1] = (rtc_month & 0x0f) + '0';
	day[0] = (rtc_day >> 4) + '0';
	day[1] = (rtc_day & 0x0f) + '0';

	Read_NvItem_DateFormat(&dataFormat);
	switch(dataFormat)
	{
		case DATE_FORMAT_YMD: //ASIA
			strncpy(buf, year, sizeof(year));
			buf += sizeof(year);
			strncpy(buf, '-', 1);
			buf++;
			strncpy(buf, month, sizeof(month));
			buf += sizeof(month);
			strncpy(buf, '-', 1);
			buf++;
			strncpy(buf, day, sizeof(day));
			break;

		case DATE_FORMAT_MDY: //US
			strncpy(buf, month, sizeof(month));
			buf += sizeof(month);
			strncpy(buf, '-', 1);
			buf++;
			strncpy(buf, day, sizeof(day));
			buf += sizeof(day);
			strncpy(buf, '-', 1);
			buf++;
			strncpy(buf, year, sizeof(year));
			break;

		case DATE_FORMAT_DMY: //EURO
			strncpy(buf, day, sizeof(day));
			buf += sizeof(day);
			strncpy(buf, '-', 1);
			buf++;
			strncpy(buf, month, sizeof(month));
			buf += sizeof(month);
			strncpy(buf, '-', 1);
			buf++;
			strncpy(buf, year, sizeof(year));
			break;
	}
}

static void TimeCon(u8 *buf)
{
    buf[0] = ((rtc_hour>>4)+ '0');
    buf[1] = ((rtc_hour&0x0f)+ '0');
    buf[2] = ':';
    buf[3] = ((rtc_min>>4)+ '0');
    buf[4] = ((rtc_min&0x0f)+ '0');
    buf[5] = ':';
    buf[6] = ((rtc_sec>>4)+ '0');
	buf[7] = ((rtc_sec&0x0f)+ '0');
}


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void Osd_Init_Erase(void)
{
	u16 i;
	BYTE pSTR[DISPLAY_WIDTH_1920X1080/CHAR_WIDTH_E];
	
	memset(pSTR, ' ', sizeof(pSTR));

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < DISPLAY_HEIGHT_1920x1080/CHAR_HEIGHT; i++)
	{
		MDINGAC_SetDrawXYMode(i*CHAR_HEIGHT, 0, pSTR, sizeof(pSTR), 0);
	}

	// Initialize channel name location buffer
	memset(osd_ch_name_location_buf, 0, sizeof(osd_ch_name_location_buf));
}

void Erase_OSD(void)
{
	u8 i = 0;

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < NUM_OF_CHANNEL; i++)
	{
		if(osd_ch_name_location_buf[i].state == ON)
		{
			MDINGAC_SetDrawXYMode(osd_ch_name_location_buf[i].location.pos_y,
								osd_ch_name_location_buf[i].location.pos_x,
								str_Blank,
								osd_ch_name_location_buf[i].length,
								0);
		}
	}
	OSD_Str_Loss_Erase();
	if(!bAuto_Seq_Flag)
	{
		OSD_Str_Freeze_autoseq_Erase();
	}
	if(bSETUP)
	{
		OSD_Display_Time_Erase();
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSD_Display(void)
{
	BOOL osdDisplayOn;
	BOOL titleDisplayOn;
	BOOL timeDisplayOn;

	Read_NvItem_OsdOn(&osdDisplayOn);
	Read_NvItem_TitleDispalyOn(&titleDisplayOn);
	Read_NvItem_TimeDisplayOn(&timeDisplayOn);

	if(osdDisplayOn == ON)
	{
		if(bMode_change_flag && (!bSETUP))
		{
			if(titleDisplayOn == ON)
				OSD_Display_CH_name();
			if(timeDisplayOn == ON)
				OSD_Display_Time_NOW();
		}
		if(!bSETUP)
			OSD_Display_State();

		if((timeDisplayOn == ON) && (!bSETUP))
			OSD_Display_Time();
	}
	bMode_change_flag = CLEAR;
}

void OSD_Display_CH_name(void)
{
	eChannel_t channel;
	sPosition_t positionValue;
	eTitlePosition_t titlePosition;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	Read_NvItem_TitlePosition(&titlePosition);
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
	{
		channel = (eChannel_t)sys_status.current_split_mode;
		Read_NvItem_ChannelName(channel_name, channel);
		positionValue =  OSD_TitleStringPosition(channel, titlePosition, DISPLAY_MODE_FULL_SCREEN, strlen(channel_name));
		OSD_Print_String(positionValue.pos_x, positionValue.pos_y, channel_name, strlen(channel_name));
		osd_ch_name_location_buf[channel].state = ON;
		osd_ch_name_location_buf[channel].length = strlen(channel_name);
		osd_ch_name_location_buf[channel].location = positionValue;
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			Read_NvItem_ChannelName(channel_name, channel);
			positionValue =  OSD_TitleStringPosition(channel, titlePosition, DISPLAY_MODE_4SPLIT, strlen(channel_name));
			OSD_Print_String(positionValue.pos_x, positionValue.pos_y, channel_name, strlen(channel_name));
			osd_ch_name_location_buf[channel].state = ON;
			osd_ch_name_location_buf[channel].length = strlen(channel_name);
			osd_ch_name_location_buf[channel].location = positionValue;
		}
	}
}

//-----------------------------------------------------------------------------
void OSD_Display_Freeze(void)
{
	u16 tblFreezeDisplayLoc_X[TIME_POSITION_MAX] = {0,};
	BOOL current_freezeMode = IsScreenFreeze();
	sPosition_t position;
	BOOL timeOn;
	eTimePosition_t timePosition;
	static BOOL previous_freezeMode = CLEAR;

	Read_NvItem_TimeDisplayOn(&timeOn);
	if(timeOn == ON)
	{
		Read_NvItem_TimePosition(&timePosition);
	}

	if(previous_freezeMode != current_freezeMode)
	{
		previous_freezeMode = current_freezeMode;
		if(timeOn == ON)
		{
			// "FREEZE" should be located after date&time
			tblFreezeDisplayLoc_X[TIME_POSITION_LEFT] =
					(DATE_TIME_LENGTH * CHAR_WIDTH_E) + 6;
			tblFreezeDisplayLoc_X[TIME_POSITION_CENTER] =
					(DISPLAY_WIDTH_1920X1080 + (DATE_TIME_LENGTH * CHAR_WIDTH_E)) / 2 ;
			// "FREEZE" should be located befor date&time
			tblFreezeDisplayLoc_X[TIME_POSITION_RIGHT] =
					DISPLAY_WIDTH_1920X1080 - ((DATE_TIME_LENGTH * CHAR_WIDTH_E) + 6) - (strlen(str_Freeze) * CHAR_WIDTH_E);
		}
		else
		{
			tblFreezeDisplayLoc_X[TIME_POSITION_LEFT] = 6;
			tblFreezeDisplayLoc_X[TIME_POSITION_CENTER] =
					(DISPLAY_WIDTH_1920X1080 - (sizeof(str_Freeze2) * CHAR_WIDTH_E)) / 2;
			tblFreezeDisplayLoc_X[TIME_POSITION_RIGHT] =
					DISPLAY_WIDTH_1920X1080 - ((sizeof(str_Freeze2) * CHAR_WIDTH_E) + 6);
		}

		position.pos_x = tblFreezeDisplayLoc_X[timePosition];
		if(IsTitlePositionTop() == TRUE)
		{
			position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT - MARGIN_Y;
		}
		else
		{
			position.pos_y = MARGIN_Y;
		}

		if(current_freezeMode)
		{
			if(timeOn == ON)
			{
				if(timePosition == TIME_POSITION_RIGHT)
				{
					OSD_Print_String(position, str_Freeze, sizeof(str_Freeze));
					osd_freeze_autoseq_location_buf.length = sizeof(str_Freeze);
				}
				else
				{
					OSD_Print_String(position, str_Freeze, sizeof(str_Freeze3));
					osd_freeze_autoseq_location_buf.length = sizeof(str_Freeze3);
				}
			}
			else
			{
					OSD_Print_String(position, str_Freeze, sizeof(str_Freeze2));
					osd_freeze_autoseq_location_buf.length = sizeof(str_Freeze2);
			}
		}
		else
		{
			if(timeOn == ON)
			{
				// erase
				OSD_Print_String(position, str_Freeze, sizeof(str_Freeze_BLK));
				osd_freeze_autoseq_location_buf.length = sizeof(str_Freeze_BLK);
			}
			else
			{
				OSD_Print_String(position, str_Freeze, sizeof(str_Freeze_BLK2));
				osd_freeze_autoseq_location_buf.length = sizeof(str_Freeze_BLK2);
			}
		}
		osd_freeze_autoseq_location_buf.state = ON;//1;
		osd_freeze_autoseq_location_buf.location = position;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSD_Display_AUTO(void)
{
	u16 tblAUTODisplayLoc_X[TIME_POSITION_MAX] = {0,};
	static u8 Pre_bAuto_Seq_Flag = CLEAR;
	BOOL timeOn;
	eTimePosition_t timePosition;
	sPosition_t position;

	Read_NvItem_TimeDisplayOn(&timeOn);
	if(timeOn == ON)
	{
		Read_NvItem_TimePosition(&timePosition);
	}

	if(Pre_bAuto_Seq_Flag != bAuto_Seq_Flag)
	{
		Pre_bAuto_Seq_Flag = bAuto_Seq_Flag;
		
		if(timeOn == ON)
		{
			// "FREEZE" should be located after date&time
			tblAUTODisplayLoc_X[TIME_POSITION_LEFT] =
					(DATE_TIME_LENGTH * CHAR_WIDTH_E) + 6;
			tblAUTODisplayLoc_X[TIME_POSITION_CENTER] =
					(DISPLAY_WIDTH_1920X1080 + (DATE_TIME_LENGTH * CHAR_WIDTH_E)) / 2 ;
			// "FREEZE" should be located befor date&time
			tblAUTODisplayLoc_X[TIME_POSITION_RIGHT] =
					DISPLAY_WIDTH_1920X1080 - ((DATE_TIME_LENGTH * CHAR_WIDTH_E) + 6) - (strlen(str_Freeze) * CHAR_WIDTH_E);
		}
		else
		{
			tblAUTODisplayLoc_X[TIME_POSITION_LEFT] = 6;
			tblAUTODisplayLoc_X[TIME_POSITION_CENTER] =
					(DISPLAY_WIDTH_1920X1080 - (sizeof(str_AUTO2) * CHAR_WIDTH_E)) / 2;
			tblAUTODisplayLoc_X[TIME_POSITION_RIGHT] =
					DISPLAY_WIDTH_1920X1080 - ((sizeof(str_AUTO2) * CHAR_WIDTH_E) + 6);
		}

		position.pos_x = tblAUTODisplayLoc_X[timePosition];
		if(IsTitlePositionTop() == TRUE)
		{
			position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT - MARGIN_Y;
		}
		else
		{
			position.pos_y = MARGIN_Y;
		}

		if(bAuto_Seq_Flag)
		{
			if(timeOn == ON)
			{
				if(timePosition == TIME_POSITION_RIGHT)
				{
					OSD_Print_String(position, str_Freeze, sizeof(str_AUTO));
					osd_freeze_autoseq_location_buf.length = sizeof(str_AUTO);
				}
				else
				{
					OSD_Print_String(position, str_Freeze, sizeof(str_AUTO3));
					osd_freeze_autoseq_location_buf.length = sizeof(str_AUTO3);
				}
			}
			else
			{
				OSD_Print_String(position, str_Freeze, sizeof(str_AUTO2));
				osd_freeze_autoseq_location_buf.length = sizeof(str_AUTO2);
			}
		}
		else
		{
			if(timeOn == ON)
			{
				OSD_Print_String(position, str_Freeze, sizeof(str_AUTO_BLK));
				osd_freeze_autoseq_location_buf.length = sizeof(str_AUTO_BLK);
			}
			else
			{
				OSD_Print_String(position, str_Freeze, sizeof(str_AUTO_BLK2));
				osd_freeze_autoseq_location_buf.length = sizeof(str_AUTO_BLK2);
			}
		}
		osd_freeze_autoseq_location_buf.state = ON;//1;
		osd_freeze_autoseq_location_buf.location = position;
	}
}

//-----------------------------------------------------------------------------
// Video Loss ǥ��
//-----------------------------------------------------------------------------
void OSD_Display_Video_Loss(void)
{
	sPosition_t position[20];
	eChannel_t channel;
	u8 channelNum;
	eChannel_t startChannel;
	eDisplayMode_t displayMode;
	BOOL videoLossDiplayOn;

	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
	
	if((videoLossDiplayOn == ON) & (Loss_Event_Flag || bMode_change_flag))
	{
		if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
		{
			displayMode = DISPLAY_MODE_FULL_SCREEN;
			channelNum = 1;
			channel = (eChannel_t)sys_status.current_split_mode;
		}
		else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
		{
			displayMode = DISPLAY_MODE_4SPLIT;
			channelNum = NUM_OF_CHANNEL;
			channel = CHANNEL1;
		}

		Loss_Event_Flag = CLEAR;
	
		if(displayMode == DISPLAY_MODE_FULL_SCREEN)
		{
			position[channel].pos_x = (DISPLAY_WIDTH_1920X1080 - (strlen(str_NO_VIDEO)*CHAR_WIDTH_E))/2;
			position[channel].pos_y = (DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT)/2;
		}
		else if(displayMode == DISPLAY_MODE_4SPLIT)
		{
			for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
			{
				position[channel].pos_x =
						tbl_OSD_SPLIT4_POSITION_1920x1080[channel][TITLE_POSITION_TOP_CENTER] - (strlen(str_NO_VIDEO)*CHAR_WIDTH_E)/2;
				position[channel].pos_y =
						tbl_OSD_SPLIT4_POSITION_1920x1080[channel][TITLE_POSITION_TOP_CENTER] + (DISPLAY_HEIGHT_1920x1080/2 -CHAR_HEIGHT)/2;
			}
		}

		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			if(vVideo_Loss & (0x01<<channel))
			{
				OSD_Print_String(position, str_NO_VIDEO, strlen(str_NO_VIDEO));
				osd_video_lose_location_buf[channel].length = strlen(str_NO_VIDEO);
			}
			else
			{
				OSD_Print_String(position, str_NO_VIDEO, strlen(str_NO_VIDEO_Blk));
				osd_video_lose_location_buf[channel].length = strlen(str_NO_VIDEO_Blk);
			}
			osd_video_lose_location_buf[channel].state = ON;//1	//state? 1??
			osd_video_lose_location_buf[channel].location = position;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSD_Display_State(void)
{
	OSD_Display_Freeze();
	OSD_Display_AUTO();
	OSD_Display_Video_Loss();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSD_Display_Time(void)
{
	sPosition_t position;
	eTimePosition_t timePosition;
	u8 str_buf[DATE_TIME_LENGTH];

	if(sec_flag)
	{
		sec_flag = CLEAR;
		
		if(IsTitlePositionTop() == TRUE)
		{
			position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT - MARGIN_Y;
		}
		else
		{
			position.pos_y = MARGIN_Y;
		}
		Read_NvItem_TimePosition(&timePosition);
		position.pos_x = tblTimeDisplayLoc_X[timePosition];

		DateCon(&str_buf[0]);
		TimeCon(&str_buf[12]);
		str_buf[10] = ' ';
		str_buf[11] = ' ';
		OSD_Print_String(position, str_buf, DATE_TIME_LENGTH);
	}
}

void OSD_Display_Time_NOW(void)
{
	sPosition_t position;
	eTimePosition_t timePosition;
	u8 str_buf[DATE_TIME_LENGTH];

	if(IsTitlePositionTop() == TRUE)
	{
		position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT - MARGIN_Y;
	}
	else
	{
		position.pos_y = MARGIN_Y;
	}
	Read_NvItem_TimePosition(&timePosition);
	position.pos_x = tblTimeDisplayLoc_X[timePosition];

	DateCon(&str_buf[0]);
	TimeCon(&str_buf[12]);
	str_buf[10] = ' ';
	str_buf[11] = ' ';
	Print_OSD_Char_Time(position.pos_x, position.pos_y, str_buf, sizeof(str_buf));
}
