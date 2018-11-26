#include "common.h"
#include "osd_string.h"

#define NUM_OF_POSITION					6
#define MARGIN_X						2
#define MARGIN_Y						4

#define DATE_TIME_LENGTH				21 // "yyyy-mmm-dd hh:mm:ss "
#define DATE_LENGTH_4DIGIT				11 // "yyyy-mmm-dd"
#define DATE_LENGTH_2DIGIT				9 // "yy-mmm-dd"
#define TIME_LENGTH						8 // "hh:mm:ss"

static u8 displayingDateTimeLength = 0;

static const sPosition_t tbl_OSD_SPLIT4_POSITION[NUM_OF_CHANNEL] = //[DISPLAY_MODE_MAX-1] =
{
	{DISPLAY_QUAD_WIDTH, 0},
	{DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0},
	{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},
	{DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}
};

static const sPosition_t titlePositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
{
	//QUAD_A
	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}},
	//Quad_B
	{{DISPLAY_WIDTH/3, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), (DISPLAY_HEIGHT/3)*2}},
	//quad_c
	{{(DISPLAY_WIDTH/3)*2, 0}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, (DISPLAY_HEIGHT/3)*2}},
	//quad_d
	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3*2}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3*2}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3*2}},
	//quad_e
	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}},
	//3split_a
	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
	//3split_b
	{{DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
	//3split_c
	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
	//3split_d
	{{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH,0}, {0, 0}},
	//split
	{{DISPLAY_QUAD_WIDTH,0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {0, 0}, {0, 0}}
};

static const sPosition_t indicatorPositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
{
	//QUAD_A
	{{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}},
	//Quad_B
	{{(DISPLAY_WIDTH/3)*2, DISPLAY_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH, (DISPLAY_HEIGHT/3)*2}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}},
	//quad_c
	{{DISPLAY_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_WIDTH/3, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/3, (DISPLAY_HEIGHT/3)*2}, {DISPLAY_WIDTH/3, DISPLAY_HEIGHT}},
	//quad_d
	{{DISPLAY_WIDTH, (DISPLAY_HEIGHT/3)*2}, {DISPLAY_WIDTH/3, DISPLAY_HEIGHT}, {DISPLAY_WIDTH/3*2, DISPLAY_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}},
	//quad_e
	{{DISPLAY_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_WIDTH/3, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/3*2, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH, DISPLAY_HEIGHT/3}},
	//3split_a
	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}, {0, 0}},
	//3split_b
	{{DISPLAY_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT}, {0, 0}},
	//3split_c
	{{DISPLAY_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}, {0, 0}},
	//3split_d
	{{DISPLAY_WIDTH, DISPLAY_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH,DISPLAY_HALF_HEIGHT}, {0, 0}},
	//split
	{{DISPLAY_HALF_WIDTH,DISPLAY_HEIGHT}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}, {0, 0}, {0, 0}}
};
static BOOL requestRefreshScreen = CLEAR;

static sPosition_t OSD_TitleStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;
	eSplitMode_t splitMode = Get_SystemSplitMode();

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL:
			position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH)) / 2;
			position.pos_y = 0;
			break;

		case DISPLAY_MODE_SPLIT:
			position.pos_x = titlePositionTable_Split[splitMode][channel].pos_x - ((length * CHAR_WIDTH)/2);
			position.pos_y = titlePositionTable_Split[splitMode][channel].pos_y;
			break;
	}

	return position;
}

static sPosition_t OSD_IndicatorStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;
	eSplitMode_t splitMode = Get_SystemSplitMode();

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL:
			position.pos_x = DISPLAY_WIDTH - (length * CHAR_WIDTH) - MARGIN_X;
			position.pos_y = DISPLAY_HEIGHT - MARGIN_Y;
			break;

		case DISPLAY_MODE_SPLIT:
			position.pos_x = indicatorPositionTable_Split[splitMode][channel].pos_x - (length * CHAR_WIDTH) - MARGIN_X;
			position.pos_y = indicatorPositionTable_Split[splitMode][channel].pos_y - MARGIN_Y;
			break;
	}
	return position;
}

static u8 Get_NumOfDisplayChannels(void)
{
	u8 channels = NUM_OF_CHANNEL;

	switch(Get_SystemSplitMode())
	{
		case DISPLAY_MODE_QUAD_A:
		case DISPLAY_MODE_QUAD_B:
		case DISPLAY_MODE_QUAD_C:
		case DISPLAY_MODE_QUAD_D:
		case DISPLAY_MODE_QUAD_E:
			channels = 4;
			break;
		case DISPLAY_MODE_3SPLIT_A:
		case DISPLAY_MODE_3SPLIT_B:
		case DISPLAY_MODE_3SPLIT_C:
		case DISPLAY_MODE_3SPLIT_D:
			channels = 3;
			break;
		case DISPLAY_MODE_2SPLIT:
			channels = 2;
	}

	return channels;
}

static u8 CreateDateString(u8 *pDateStr)
{
  static sTimeDate_t rtcDate;
	eDateFormat_t dateFormat;
	BOOL year4digit;
	u8 year[5];
	u8 month[3];
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
	strncpy(month, pStrMonthName[rtcDate.month - 1], sizeof(month));
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

static sPosition_t OSD_GetAutoPosition(u8 strLength)
{
	sPosition_t position;
	eDisplayMode_t displayMode = Get_SystemDisplayMode();
	BOOL timeOn;
	BOOL dateOn;
	BOOL year4digit;
	u8 dateTimeLength = 0;

	Read_NvItem_YearFormat(&year4digit);
	Read_NvItem_DateDisplayOn(&dateOn);
	Read_NvItem_TimeDisplayOn(&timeOn);

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
		position.pos_x = (DISPLAY_WIDTH + (MIN(DATE_TIME_LENGTH, dateTimeLength) * CHAR_WIDTH)) / 2 ;
	}
	else
	{
		position.pos_x = (DISPLAY_WIDTH - (strLength * CHAR_WIDTH)) / 2;
	}
	// y
	position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;

	return position;
}

static u8* OSD_GetInfoStr(eChannel_t channel) //freeze, alarm, motion, loss
{
	BOOL videoLossDiplayOn;
	u8* pInfoStr = (u8*)osdStr_Space10;

	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);

	// check video loss only if video loss display is on
	if((videoLossDiplayOn == ON) & ((GetVideoLossEvent() == SET) || (requestRefreshScreen == SET)))
	{
		SetVideoLossEvent(CLEAR);
		if(IsVideoLossChannel(channel) == TRUE)
		{
			pInfoStr = (u8*)osdStr_NoVideo;
		}
	}

	//if()

	return pInfoStr;
}
//-----------------------------------------------------------------------------
// Erase
//-----------------------------------------------------------------------------
static void OSD_EraseChannelName(void)
{
	eChannel_t channel;
	sPosition_t position;
	eDisplayMode_t displayMode;
	eChannel_t max_channel = NUM_OF_CHANNEL;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	displayMode = Get_SystemDisplayMode();

	if(displayMode == DISPLAY_MODE_FULL)
	{
		channel = Get_SystemDisplayChannel();
		Read_NvItem_ChannelName(channel_name, channel);
		position =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
		OSD_PrintString(position, osdStr_Space12, strlen(channel_name));
	}
	else
	{
		max_channel = Get_NumOfDisplayChannels();
		for(channel = CHANNEL1; channel < max_channel; channel++)
		{
			Read_NvItem_ChannelName(channel_name, channel);
			position =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
			OSD_PrintString(position, osdStr_Space12, strlen(channel_name));

			position = OSD_IndicatorStringPosition(channel, displayMode, strlen(osdStr_Alarm));
		}
	}
}

static void OSD_EraseTimeDate(void)
{
	sPosition_t position;
	eDisplayMode_t displayMode = Get_SystemDisplayMode();
	u8 strSpaces[DATE_TIME_LENGTH];

	position.pos_x = (DISPLAY_WIDTH - (DATE_TIME_LENGTH*CHAR_WIDTH))/2 - 12;
	position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;

	memset(strSpaces, ASCII_SPACE, sizeof(strSpaces));
	OSD_PrintString(position, strSpaces, displayingDateTimeLength);
	displayingDateTimeLength = 0;
}

static void OSD_EraseNoVideo(void)
{
	eChannel_t channel;
	sPosition_t position;
	eDisplayMode_t displayMode = Get_SystemDisplayMode();

	if(displayMode == DISPLAY_MODE_FULL)
	{
		position.pos_x = (DISPLAY_WIDTH - (strlen(osdStr_Space10)*CHAR_WIDTH))/2;
		position.pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;
		OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
	}
	else //if(displayMode == DISPLAY_MODE_QUAD)
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			position.pos_x =
					tbl_OSD_SPLIT4_POSITION[channel].pos_x - (strlen(osdStr_Space10)*CHAR_WIDTH)/2;
			position.pos_y =
					tbl_OSD_SPLIT4_POSITION[channel].pos_y + ((DISPLAY_HEIGHT/2) - CHAR_HEIGHT)/2;
			OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
		}
	}
}

static void OSD_EraseAuto(void)
{
	sPosition_t position;
	BOOL timeOn;

	Read_NvItem_TimeDisplayOn(&timeOn);
	position = OSD_GetAutoPosition(strlen(osdStr_Space4));

	if(timeOn == ON)
	{
		position.pos_x += (2 * CHAR_WIDTH);
	}
	OSD_PrintString(position, osdStr_Space6, strlen(osdStr_Space4));
}

//-----------------------------------------------------------------------------
// auto
//-----------------------------------------------------------------------------
static void OSD_DisplayAuto(void)
{
	BOOL autoOn;
	sPosition_t position;
	BOOL timeOn;
	static BOOL previousAutoSeqOn = CLEAR;
	u8* pStr;

	autoOn = (GetCurrentAutoSeq() == AUTO_SEQ_NORMAL)?SET:CLEAR;

	if((previousAutoSeqOn !=  autoOn) || (requestRefreshScreen == SET))
	{
		previousAutoSeqOn = autoOn;

		position = OSD_GetAutoPosition(sizeof(osdStr_AUTO));
		Read_NvItem_TimeDisplayOn(&timeOn);
		if(timeOn == ON)
		{
			position.pos_x += (2 * CHAR_WIDTH);
		}

		if(autoOn == SET)
		{
			pStr = (u8 *)osdStr_AUTO;
		}
		else
		{
			pStr = (u8 *)osdStr_Space4;
		}
		OSD_PrintString(position, pStr, strlen(pStr));
	}
}

//-----------------------------------------------------------------------------
// Video Loss
//-----------------------------------------------------------------------------
static void OSD_DisplayNoVideo(void)
{
	sPosition_t position[NUM_OF_CHANNEL];
	eChannel_t channel;
	eChannel_t startChannel;
	eDisplayMode_t displayMode = Get_SystemDisplayMode();
	BOOL videoLossDiplayOn;
	u8* pInfoStr[NUM_OF_CHANNEL];

	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
//	Read_NvItem_DisplayMode(&displayMode);
	
	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		pInfoStr[channel] = OSD_GetInfoStr(channel);
	}

	if((videoLossDiplayOn == ON) & ((GetVideoLossEvent() == SET) || (requestRefreshScreen == SET)))
	{
		if(displayMode == DISPLAY_MODE_FULL)
		{
			channel = Get_SystemDisplayChannel();
		}

		SetVideoLossEvent(CLEAR);
	
		if(displayMode == DISPLAY_MODE_FULL)
		{
			position[channel].pos_x = (DISPLAY_WIDTH - (strlen(osdStr_NoVideo)*CHAR_WIDTH))/2;
			position[channel].pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;

			if(IsVideoLossChannel(channel) == TRUE)
			{
				OSD_PrintString(position[channel], osdStr_NoVideo, strlen((const u8*)pInfoStr));
			}
			else
			{
				OSD_PrintString(position[channel], osdStr_Space10, strlen(osdStr_Space10));
			}
		}
		else if(displayMode == DISPLAY_MODE_QUAD_A)
		{
			for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
			{
				position[channel].pos_x =
						tbl_OSD_SPLIT4_POSITION[channel].pos_x - (strlen((const u8*)pInfoStr)*CHAR_WIDTH)/2;
				position[channel].pos_y =
						tbl_OSD_SPLIT4_POSITION[channel].pos_y + ((DISPLAY_HEIGHT/2) - CHAR_HEIGHT)/2;
				if(IsVideoLossChannel(channel) == TRUE)
				{
					OSD_PrintString(position[channel], osdStr_NoVideo, strlen((const u8*)pInfoStr));
				}
				else
				{
					OSD_PrintString(position[channel], osdStr_Space10, strlen(osdStr_Space10));
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Date & Time
//-----------------------------------------------------------------------------
static void OSD_DisplayDateTime(void)
{
	BOOL timeDisplayOn;
	BOOL dateDisplayOn;
	sPosition_t position;
	eDisplayMode_t displayMode = Get_SystemDisplayMode();
	u8 dateTimeString[DATE_TIME_LENGTH+1];
	u8* pStr;

	Read_NvItem_DateDisplayOn(&dateDisplayOn);
	Read_NvItem_TimeDisplayOn(&timeDisplayOn);

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
		displayingDateTimeLength = strlen(dateTimeString);

		if(displayingDateTimeLength > 0)
		{
			position.pos_x = (DISPLAY_WIDTH - (strlen(dateTimeString)*CHAR_WIDTH))/2 - 12;
			position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
			OSD_PrintString(position, dateTimeString, displayingDateTimeLength);
		}
	}
}


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void OSD_RefreshScreen(void)
{
	requestRefreshScreen = SET;
}

void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

void OSD_DisplayChannelName(void)
{
	eChannel_t channel, max_channel = NUM_OF_CHANNEL;
	sPosition_t positionValue;
	BOOL titleDisplayOn;
	eDisplayMode_t displayMode = Get_SystemDisplayMode();
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	Read_NvItem_TitleDispalyOn(&titleDisplayOn);

	if(titleDisplayOn == ON)
	{
		if(displayMode == DISPLAY_MODE_FULL)
		{
			channel = Get_SystemDisplayChannel();
			Read_NvItem_ChannelName(channel_name, channel);
			positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
			OSD_PrintString(positionValue, channel_name, strlen(channel_name));
		}
		else
		{
			max_channel = Get_NumOfDisplayChannels();
			for(channel = CHANNEL1; channel < max_channel; channel++)
			{
				Read_NvItem_ChannelName(channel_name, channel);
				positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
			}
		}
	}
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
	BOOL titleOn;
	BOOL timeOn;
	BOOL dateOn;
	BOOL osdOn;

	Read_NvItem_OsdOn(&osdOn);
	Read_NvItem_TitleDispalyOn(&titleOn);
	Read_NvItem_TimeDisplayOn(&timeOn);
	Read_NvItem_DateDisplayOn(&dateOn);

	if(osdOn == TRUE)
	{
		// erase freeze or auto
		if(titleOn == TRUE)
		{
			//erase title
			OSD_EraseChannelName();
		}
		if((dateOn == TRUE) || (timeOn == TRUE))
		{
			// erase time and/or date
			OSD_EraseTimeDate();
		}
		OSD_EraseNoVideo();
		OSD_EraseAuto();
	}
}

//-----------------------------------------------------------------------------
void OSD_Display(void)
{
	BOOL osdDisplayOn;

	Read_NvItem_OsdOn(&osdDisplayOn);

	if((osdDisplayOn == ON) && (GetSystemMode() == SYSTEM_NORMAL_MODE))
	{
		OSD_DisplayDateTime();
		OSD_DisplayChannelName();
		OSD_DisplayNoVideo();
		OSD_DisplayAuto();
		//OSD_DisplayAlarm();
		//OSD_DisplayMotion();
	}
	else if(GetSystemMode() == SYSTEM_SETUP_MODE)
	{
		DisplayTimeInMenu();
	}

	requestRefreshScreen = CLEAR;
}
//-----------------------------------------------------------------------------
void OSD_DrawBorderLine(void)
{
	BOOL border_line;
	eSplitMode_t splitMode = Get_SystemSplitMode();

	Read_NvItem_BorderLineDisplay(&border_line);
	if((border_line == ON) && (Get_SystemDisplayMode() == DISPLAY_MODE_SPLIT))
	{
		MDINOSD_SetBGBoxColor(RGB(255,255,255));
		switch(splitMode)
		{
			case DISPLAY_MODE_QUAD_A:
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
				break;

			case DISPLAY_MODE_QUAD_B:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_WIDTH - (DISPLAY_WIDTH/3), 0, 2, DISPLAY_HEIGHT);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_WIDTH - (DISPLAY_WIDTH/3), DISPLAY_HEIGHT / 3, DISPLAY_WIDTH/3, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, DISPLAY_WIDTH - (DISPLAY_WIDTH/3), DISPLAY_HEIGHT / 3 * 2, DISPLAY_WIDTH/3, 2);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;

			case DISPLAY_MODE_QUAD_C:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, (DISPLAY_WIDTH/3), 0, 2, DISPLAY_HEIGHT);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 0, DISPLAY_HEIGHT/3, DISPLAY_WIDTH/3, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 0, (DISPLAY_HEIGHT/3) * 2, DISPLAY_WIDTH/3, 2);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;

			case DISPLAY_MODE_QUAD_D:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, (DISPLAY_HEIGHT/3)*2, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_WIDTH/3, (DISPLAY_HEIGHT/3)*2, 2, DISPLAY_HEIGHT/3);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, (DISPLAY_WIDTH/3) * 2, (DISPLAY_HEIGHT/3)*2, 2, DISPLAY_HEIGHT/3);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;

			case DISPLAY_MODE_QUAD_E:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, (DISPLAY_HEIGHT/3), DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_WIDTH/3, 0, 2, DISPLAY_HEIGHT/3);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, (DISPLAY_WIDTH/3) * 2, 0, 2, DISPLAY_HEIGHT/3);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;

			case DISPLAY_MODE_3SPLIT_A:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_HALF_WIDTH-1, DISPLAY_HALF_HEIGHT-1, DISPLAY_HALF_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;

			case DISPLAY_MODE_3SPLIT_B:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_HALF_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;
			case DISPLAY_MODE_3SPLIT_C:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, DISPLAY_HALF_HEIGHT-1, 2, DISPLAY_HALF_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;
			case DISPLAY_MODE_3SPLIT_D:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HALF_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;
			case DISPLAY_MODE_2SPLIT:
				MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
				break;
                                
			default :
				MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
                break;
		}
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
