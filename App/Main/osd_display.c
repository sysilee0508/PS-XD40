#include "common.h"
#include "osd_string.h"

#define NUM_OF_POSITION					6
#define MARGIN_X						2
#define MARGIN_Y						4

#define DATE_TIME_LENGTH				20 // "yyyy-mm-dd hh:mm:ss "
#define DATE_LENGTH_4DIGIT				10 // "yyyy-mm-dd"
#define DATE_LENGTH_2DIGIT				8 // "yy-mm-dd"
#define TIME_LENGTH						8 // "hh:mm:ss"

struct osd_location 
{
	u8 state;
	u8 length;
	sPosition_t location;
};

struct osd_location osd_ch_name_location_buf[NUM_OF_CHANNEL];
struct osd_location osd_video_lose_location_buf[NUM_OF_CHANNEL];
struct osd_location osd_freeze_autoseq_location_buf;

static const sPosition_t tbl_OSD_SPLIT4_POSITION[NUM_OF_CHANNEL][NUM_OF_POSITION] =
{
//  TopLeft										TopCenter													TopRight
//			BottomLeft									BottomCenter												BottomRight
	{{0, 0},									{DISPLAY_QUAD_WIDTH, 0},									{DISPLAY_HALF_WIDTH, 0},
			{0, DISPLAY_HALF_HEIGHT},					{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},					{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}},	//CH01
	{{DISPLAY_HALF_WIDTH, 0},					{DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0},					{DISPLAY_WIDTH, 0},
			{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT},	{DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},	{DISPLAY_WIDTH, DISPLAY_HALF_HEIGHT}},	//CH02
	{{0, DISPLAY_HALF_HEIGHT},					{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},					{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT},
			{0,DISPLAY_HEIGHT},							{DISPLAY_QUAD_WIDTH,DISPLAY_HEIGHT},						{DISPLAY_HALF_WIDTH,DISPLAY_HEIGHT}},	//CH03
	{{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT},	{DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},	{DISPLAY_WIDTH, DISPLAY_HALF_HEIGHT},
			{DISPLAY_HALF_WIDTH,DISPLAY_HEIGHT},		{DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH,DISPLAY_HEIGHT},		{DISPLAY_WIDTH,DISPLAY_HEIGHT}}	//CH04
};

BYTE changedDisplayMode = CLEAR;

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

//static void OSD_Display_Time_Erase(void)
//{
//	sPosition_t position;
//	u8 str_buf[DATE_TIME_LENGTH];
////	eTimePosition_t timePosition;
//
//	if(IsTitlePositionTop() == TRUE) // if title position is top
//	{
//		//if title position is top then time should be located at bottom
//		position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
//	}
//	else
//	{
//		// If not, time position goes top
//		position.pos_y = MARGIN_Y;
//	}
////
////	Read_NvItem_TimePosition(&timePosition);
////	position.pos_x = tblTimeDisplayLoc_X[timePosition];
//	position.pos_x = (DISPLAY_WIDTH - (DATE_TIME_LENGTH*CHAR_WIDTH_E))/2 - 12;
//
//	memset(str_buf, ASCII_SPACE, sizeof(str_buf));
//	OSD_PrintString(position,  str_buf, sizeof(str_buf));
//}
//
//static void OSD_Str_Loss_Erase(void)
//{
//	eChannel_t channel;
//
//	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
//	{
//		if(osd_video_lose_location_buf[channel].state == ON)
//		{
//			MDINGAC_SetDrawXYMode(osd_video_lose_location_buf[channel].location.pos_y,
//					osd_video_lose_location_buf[channel].location.pos_x,
//					(u8 *)str_Blank,
//					osd_video_lose_location_buf[channel].length,
//					0);
//		}
//	}
//}
//
//static void OSD_Str_Freeze_autoseq_Erase(void)
//{
//	if(osd_freeze_autoseq_location_buf.state == ON)
//	{
//		MDINGAC_SetDrawXYMode(osd_freeze_autoseq_location_buf.location.pos_y,
//				osd_freeze_autoseq_location_buf.location.pos_x,
//				(u8 *)str_Blank,
//				osd_freeze_autoseq_location_buf.length,
//				0);
//	}
//}

static sPosition_t OSD_TitleStringPosition(eChannel_t channel, eTitlePosition_t titlePosition, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	Read_NvItem_ChannelName(channel_name, channel);

	switch(titlePosition)
	{
		case TITLE_POSITION_TOP_LEFT://0:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = MARGIN_X;
					position.pos_y = 0;
					break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_x + MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_y;
					break;
			}
			break;

		case TITLE_POSITION_TOP_CENTER://1
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = (DISPLAY_WIDTH - (strlen(channel_name) * CHAR_WIDTH_K)) / 2;
					position.pos_y = 0;
					break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E)/2;
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_y;
					break;
			}
			break;

		case TITLE_POSITION_TOP_RIGHT://2
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = DISPLAY_WIDTH - MARGIN_X - (length * CHAR_WIDTH_K);
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) - MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_y;
				break;

			}
			break;

		case TITLE_POSITION_BOTTOM_LEFT://3:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = MARGIN_X;
					position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_x + MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
			break;

		case TITLE_POSITION_BOTTOM_CENTER: //4
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH_K)) / 2;
					position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) / 2;
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
			break;

		case TITLE_POSITION_BOTTOM_RIGHT://5:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = DISPLAY_WIDTH - MARGIN_X - (length * CHAR_WIDTH_K);
					position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT;
					break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_x - MARGIN_X - (length * CHAR_WIDTH_E);
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
			break;

		case TITLE_POSITION_4SPILIT_CENTER://6:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN: //center-top
					position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH_K)) / 2;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					if(channel == CHANNEL1 || channel == CHANNEL2) //center-bottom
					{
						position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][TITLE_POSITION_BOTTOM_CENTER].pos_x - (length * CHAR_WIDTH_E) / 2;
						position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][TITLE_POSITION_BOTTOM_CENTER].pos_y - CHAR_HEIGHT;
					}
					else if(channel == CHANNEL3 || channel == CHANNEL4) //center-top
					{
						position.pos_x = tbl_OSD_SPLIT4_POSITION[channel][TITLE_POSITION_TOP_CENTER].pos_x - (length * CHAR_WIDTH_E)/2;
					position.pos_y = tbl_OSD_SPLIT4_POSITION[channel][TITLE_POSITION_TOP_CENTER].pos_y;
					}
				break;
			}
			break;
	}
	return position;
}

static u8 CreateDateString(u8 *pDateStr)
{
  static sTimeDate_t rtcDate;
	eDateFormat_t dateFormat;
	BOOL year4digit;
	u8 year[5];
	u8 month[2];
	u8 day[2];
	u8 dateLength;

	RTC_GetTime(&rtcDate);
	Read_NvItem_DateFormat(&dateFormat);
	Read_NvItem_YearFormat(&year4digit);

	memset(year, NULL, sizeof(year));
	if(year4digit == FALSE) //2digit
	{
		year[0] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
		year[1] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
		dateLength = DATE_LENGTH_2DIGIT;
	}
	else //4 digit
	{
		year[0] = 2 + ASCII_ZERO;
		year[1] = ASCII_ZERO;
		year[2] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
		year[3] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
		dateLength = DATE_LENGTH_4DIGIT;
	}
	month[0] = (rtcDate.month / 10) + ASCII_ZERO;
	month[1] = (rtcDate.month % 10) + ASCII_ZERO;
	day[0] = (rtcDate.day / 10) + ASCII_ZERO;
	day[1] = (rtcDate.day % 10) + ASCII_ZERO;


	switch(dateFormat)
	{
		case DATE_FORMAT_YMD: //ASIA
			strncpy(pDateStr, year, strlen(year));
			pDateStr += strlen(year);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, month, sizeof(month));
			pDateStr += sizeof(month);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, day, sizeof(day));
			break;

		case DATE_FORMAT_MDY: //US
			strncpy(pDateStr, month, sizeof(month));
			pDateStr += sizeof(month);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, day, sizeof(day));
			pDateStr += sizeof(day);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, year, strlen(year));
			break;

		case DATE_FORMAT_DMY: //EURO
			strncpy(pDateStr, day, sizeof(day));
			pDateStr += sizeof(day);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, month, sizeof(month));
			pDateStr += sizeof(month);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, year, strlen(year));
			break;
	}
	return dateLength;
}

static u8 CreateTimeString(u8 *pTimeStr)
{
	sTimeDate_t rtcTime;

	RTC_GetTime(&rtcTime);
    *pTimeStr = ((rtcTime.hour / 10)+ ASCII_ZERO);
    *(++pTimeStr) = ((rtcTime.hour % 10)+ ASCII_ZERO);
    *(++pTimeStr) = ':';
    *(++pTimeStr) = ((rtcTime.min / 10)+ ASCII_ZERO);
    *(++pTimeStr) = ((rtcTime.min % 10)+ ASCII_ZERO);
    *(++pTimeStr) = ':';
    *(++pTimeStr) = ((rtcTime.sec / 10)+ ASCII_ZERO);
    *(++pTimeStr) = ((rtcTime.sec % 10)+ ASCII_ZERO);

	return TIME_LENGTH;
}

static sPosition_t OSD_GetAutoFreezePosition(u8 strLength)
{
	sPosition_t position;
	eDisplayMode_t displayMode;
	BOOL timeOn;
	BOOL dateOn;
	BOOL year4digit;
	u8 dateTimeLength = 0;

	Read_NvItem_YearFormat(&year4digit);
	Read_NvItem_DateDisplayOn(&dateOn);
	Read_NvItem_TimeDisplayOn(&timeOn);
	Read_NvItem_DisplayMode(&displayMode);

	if((dateOn == ON) && (year4digit == TRUE))
	{
		dateTimeLength = DATE_LENGTH_4DIGIT + 2;
	}
	else if((dateOn == ON) && (year4digit == FALSE))
	{
		dateTimeLength = DATE_LENGTH_2DIGIT + 2;
	}

	if(timeOn == ON)
	{
		dateTimeLength += TIME_LENGTH;
	}

	// x
	if(dateTimeLength > 0)
	{
		position.pos_x = (DISPLAY_WIDTH + (MIN(DATE_TIME_LENGTH, dateTimeLength) * CHAR_WIDTH_E)) / 2 ;
	}
	else
	{
		position.pos_x = (DISPLAY_WIDTH - (strLength * CHAR_WIDTH_E)) / 2;
	}
	// y
	if((IsTitlePositionTop() == FALSE) && (displayMode == DISPLAY_MODE_FULL_SCREEN))
	{
		position.pos_y = MARGIN_Y;
	}
	else
	{
		position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
	}

	return position;
}
//-----------------------------------------------------------------------------
static void OSD_Display_Freeze(void)
{
	BOOL current_freezeMode = IsScreenFreeze();
	sPosition_t position;
	BOOL timeOn;
	static BOOL previous_freezeMode = CLEAR;

	Read_NvItem_TimeDisplayOn(&timeOn);

	if(previous_freezeMode != current_freezeMode)
	{
		previous_freezeMode = current_freezeMode;
		position = OSD_GetAutoFreezePosition(sizeof(osdStr_Freeze));

		if(timeOn == ON)
		{
			position.pos_x += (2 * CHAR_WIDTH_E);
		}

		if(current_freezeMode)
		{
			OSD_PrintString(position,osdStr_Freeze, sizeof(osdStr_Freeze));
			osd_freeze_autoseq_location_buf.length = sizeof(osdStr_Freeze);
		}
		else
		{
			OSD_PrintString(position, osdStr_Space6, sizeof(osdStr_Space6));
			osd_freeze_autoseq_location_buf.length = sizeof(osdStr_Space6);
		}
		osd_freeze_autoseq_location_buf.state = ON;//1
		osd_freeze_autoseq_location_buf.location = position;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void OSD_Display_AUTO(void)
{
	static u8 Pre_bAuto_Seq_Flag = CLEAR;
	BOOL timeOn;
	eDisplayMode_t displayMode;
	sPosition_t position;

	Read_NvItem_TimeDisplayOn(&timeOn);
	Read_NvItem_DisplayMode(&displayMode);

	if(Pre_bAuto_Seq_Flag != bAuto_Seq_Flag)
	{
		Pre_bAuto_Seq_Flag = bAuto_Seq_Flag;
		position = OSD_GetAutoFreezePosition(sizeof(osdStr_AUTO));

		if(timeOn == ON)
		{
			position.pos_x += (2 * CHAR_WIDTH_E);
		}

		if(bAuto_Seq_Flag)
		{
			OSD_PrintString(position, osdStr_AUTO, sizeof(osdStr_AUTO));
			osd_freeze_autoseq_location_buf.length = sizeof(osdStr_AUTO);
		}
		else
		{
			OSD_PrintString(position, osdStr_Space6, sizeof(osdStr_Space6));
			osd_freeze_autoseq_location_buf.length = sizeof(osdStr_Space6);
		}
		osd_freeze_autoseq_location_buf.state = ON;//1
		osd_freeze_autoseq_location_buf.location = position;
	}
}

//-----------------------------------------------------------------------------
// Video Loss
//-----------------------------------------------------------------------------
static void OSD_Display_Video_Loss(void)
{
	sPosition_t position[NUM_OF_CHANNEL];
	eChannel_t channel;
	eChannel_t startChannel;
	eDisplayMode_t displayMode;
	BOOL videoLossDiplayOn;

	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
	Read_NvItem_DisplayMode(&displayMode);
	
	if((videoLossDiplayOn == ON) & ((Loss_Event_Flag == SET) || (changedDisplayMode == SET)))
	{
		if(displayMode == DISPLAY_MODE_FULL_SCREEN)
		{
			channel = (eChannel_t)sys_status.current_split_mode;
		}

		Loss_Event_Flag = CLEAR;
	
		if(displayMode == DISPLAY_MODE_FULL_SCREEN)
		{
			position[channel].pos_x = (DISPLAY_WIDTH - (strlen(osdStr_NoVideo)*CHAR_WIDTH_E))/2;
			position[channel].pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;

			if(vVideo_Loss & (0x01<<channel))
			{
				OSD_PrintString(position[channel], osdStr_NoVideo, strlen(osdStr_NoVideo));
				osd_video_lose_location_buf[channel].length = strlen(osdStr_NoVideo);
			}
			else
			{
				OSD_PrintString(position[channel], osdStr_Space10, strlen(osdStr_Space10));
				osd_video_lose_location_buf[channel].length = strlen(osdStr_Space10);
			}
			osd_video_lose_location_buf[channel].state = ON;
			osd_video_lose_location_buf[channel].location = position[channel];
		}
		else if(displayMode == DISPLAY_MODE_4SPLIT)
		{
			for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
			{
				position[channel].pos_x =
						tbl_OSD_SPLIT4_POSITION[channel][TITLE_POSITION_TOP_CENTER].pos_x - (strlen(osdStr_NoVideo)*CHAR_WIDTH_E)/2;
				position[channel].pos_y =
						tbl_OSD_SPLIT4_POSITION[channel][TITLE_POSITION_TOP_CENTER].pos_y + ((DISPLAY_HEIGHT/2) - CHAR_HEIGHT)/2;
				if(vVideo_Loss & (0x01<<channel))
				{
					OSD_PrintString(position[channel], osdStr_NoVideo, strlen(osdStr_NoVideo));
					osd_video_lose_location_buf[channel].length = strlen(osdStr_NoVideo);
				}
				else
				{
					OSD_PrintString(position[channel], osdStr_Space10, strlen(osdStr_Space10));
					osd_video_lose_location_buf[channel].length = strlen(osdStr_Space10);
				}
				osd_video_lose_location_buf[channel].state = ON;
				osd_video_lose_location_buf[channel].location = position[channel];
			}
		}
	}
}

//-----------------------------------------------------------------------------
static void OSD_DisplayDateTime(void)
{
	BOOL timeDisplayOn;
	BOOL dateDisplayOn;
	sPosition_t position;
	eDisplayMode_t displayMode;
	u8 dateTimeString[DATE_TIME_LENGTH+1];
	u8* pStr;

	Read_NvItem_DateDisplayOn(&dateDisplayOn);
	Read_NvItem_TimeDisplayOn(&timeDisplayOn);
	Read_NvItem_DisplayMode(&displayMode);

	pStr = &dateTimeString[0];
	memset(pStr, NULL, sizeof(dateTimeString));

	if(RTC_GetDisplayTimeStatus() == SET)
	{
		RTC_ChangeDisplayTimeStatus(CLEAR);

		if((dateDisplayOn == ON) && (timeDisplayOn == ON))
		{
			pStr += CreateDateString(pStr);
			strcpy(pStr, osdStr_Space2);
			pStr += strlen(osdStr_Space2);
			CreateTimeString(pStr);
		}
		else if((dateDisplayOn == ON) && (timeDisplayOn == OFF))
		{
			CreateDateString(pStr);
		}
		else if((dateDisplayOn == OFF) && (timeDisplayOn == ON))
		{
			CreateTimeString(pStr);
		}

		if(strlen(dateTimeString) > 0)
		{
			position.pos_x = (DISPLAY_WIDTH - (strlen(dateTimeString)*CHAR_WIDTH_E))/2 - 12;
			if((IsTitlePositionTop() == FALSE) && (displayMode == DISPLAY_MODE_FULL_SCREEN))
			{
				position.pos_y = MARGIN_Y;
			}
			else
			{
				position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
			}
			OSD_PrintString(position, dateTimeString, DATE_TIME_LENGTH);
		}
	}
}

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

void OSD_Display_ChannelName(void)
{
	eChannel_t channel;
	sPosition_t positionValue;
	eTitlePosition_t titlePosition;
	BOOL titleDisplayOn;
	eDisplayMode_t displayMode;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	Read_NvItem_TitleDispalyOn(&titleDisplayOn);
	Read_NvItem_TitlePosition(&titlePosition);
	Read_NvItem_DisplayMode(&displayMode);

	if(titleDisplayOn == ON)
	{
		if(displayMode == DISPLAY_MODE_FULL_SCREEN)
		{
			channel = (eChannel_t)sys_status.current_split_mode;
			Read_NvItem_ChannelName(channel_name, channel);
			positionValue =  OSD_TitleStringPosition(channel, titlePosition, displayMode, strlen(channel_name));
			Read_NvItem_ChannelName(channel_name, channel);
			OSD_PrintString(positionValue, channel_name, strlen(channel_name));
			osd_ch_name_location_buf[channel].state = ON;
			osd_ch_name_location_buf[channel].length = strlen(channel_name);
			osd_ch_name_location_buf[channel].location = positionValue;
		}
		else if(displayMode == DISPLAY_MODE_4SPLIT)
		{
			for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
			{
				Read_NvItem_ChannelName(channel_name, channel);
				positionValue =  OSD_TitleStringPosition(channel, titlePosition, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				osd_ch_name_location_buf[channel].state = ON;
				osd_ch_name_location_buf[channel].length = strlen(channel_name);
				osd_ch_name_location_buf[channel].location = positionValue;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void Osd_ClearScreen(void)
{
	u16 i;
	BYTE pSTR[DISPLAY_WIDTH/CHAR_WIDTH_E];

	memset(pSTR, ASCII_SPACE, sizeof(pSTR));

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < DISPLAY_HEIGHT/CHAR_HEIGHT; i++)
	{
		MDINGAC_SetDrawXYMode(i*CHAR_HEIGHT, 0, pSTR, sizeof(pSTR), 0);
	}

	// Initialize channel name location buffer
	memset(osd_ch_name_location_buf, 0, sizeof(osd_ch_name_location_buf));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void OSD_Display(void)
{
	BOOL osdDisplayOn;

	Read_NvItem_OsdOn(&osdDisplayOn);

	if((osdDisplayOn == ON) && (GetSystemMode() == SYSTEM_NORMAL_MODE))
	{
		OSD_DisplayDateTime();
		OSD_Display_ChannelName();
		OSD_Display_Freeze();
		OSD_Display_AUTO();
		OSD_Display_Video_Loss();
	}
	else if(GetSystemMode() == SYSTEM_SETUP_MODE)
	{
		DisplayTimeInMenu();
	}

	changedDisplayMode = CLEAR;
}
//-----------------------------------------------------------------------------
void OSD_DrawBorderLine(void)
{
	BOOL border_line;
	eDisplayMode_t displayMode;

	Read_NvItem_BorderLineDisplay(&border_line);
	Read_NvItem_DisplayMode(&displayMode);

	if((border_line == ON) && (displayMode == DISPLAY_MODE_4SPLIT))
	{
		MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
		MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
		MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
		MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
		MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
	}
	else
	{
		MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
	}
}
