#include "common.h"
#include "osd_string.h"
#include "NVP6158.h"

#define MARGIN_X						5
#define MARGIN_Y						5

#define TITLE_LENGTH					5
#define VIDEO_FORMAT_LENGTH_MAX			20

//#define DATE_TIME_LENGTH				21 // "yyyy-mmm-dd hh:mm:ss "
//#define DATE_LENGTH_4DIGIT				11 // "yyyy-mmm-dd"
//#define DATE_LENGTH_2DIGIT				9 // "yy-mmm-dd"
//#define TIME_LENGTH						8 // "hh:mm:ss"

//static u8 displayingDateTimeLength = 0;
//static const sPosition_t titlePositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
//{
//	//QUAD_A
//	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}},
//	//Quad_B
//	{{DISPLAY_WIDTH/3, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), (DISPLAY_HEIGHT/3)*2}},
//	//quad_c
//	{{(DISPLAY_WIDTH/3)*2, 0}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, (DISPLAY_HEIGHT/3)*2}},
//	//quad_d
//	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3*2}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3*2}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3*2}},
//	//quad_e
//	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}},
//	//3split_a
//	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
//	//3split_b
//	{{DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
//	//3split_c
//	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
//	//3split_d
//	{{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH,0}, {0, 0}},
//	//split
//	{{DISPLAY_QUAD_WIDTH,0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {0, 0}, {0, 0}}
//};

//static const sPosition_t indicatorPositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
//{
//	//QUAD_A
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X , DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}},
//	//Quad_B
//	{{(DISPLAY_WIDTH/3)*2-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
//			{(DISPLAY_WIDTH/3)*2+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
//	//quad_c
//	{{(DISPLAY_WIDTH/3)+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
//	//quad_d
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)*2-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y},
//			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)*2+MARGIN_Y+CHAR_HEIGHT}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
//	//quad_e
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HEIGHT/3+MARGIN_Y+CHAR_HEIGHT}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y}},
//	//3split_a
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {0, 0}},
//	//3split_b
//	{{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {0, 0}},
//	//3split_c
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {0, 0}},
//	//3split_d
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT+MARGIN_Y+CHAR_HEIGHT}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
//			{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {0, 0}},
//	//split
//	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {0, 0}, {0, 0}}
//};

static BOOL requestRefreshScreen = CLEAR;

//static sPosition_t OSD_TitleStringPosition(u8 length)
//{
//	sPosition_t position;
//
//	position.pos_x = (DISPLAY_WIDTH - (TITLE_LENGTH * CHAR_WIDTH)) / 2;
//	position.pos_y = MARGIN_Y;
//
//	return position;
//}
//
//static sPosition_t OSD_IndicatorStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 length)
//{
//	sPosition_t position;
//	eSplitMode_t splitMode = Get_SystemSplitMode();
//
//	switch(displayMode)
//	{
//		case DISPLAY_MODE_FULL:
//			position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
//			position.pos_y = DISPLAY_HEIGHT - 2*CHAR_HEIGHT - MARGIN_Y;
//			break;
//
//		case DISPLAY_MODE_SPLIT:
//			position.pos_x = indicatorPositionTable_Split[splitMode][channel].pos_x;
//			position.pos_y = indicatorPositionTable_Split[splitMode][channel].pos_y;
//			break;
//	}
//	return position;
//}

//static u8 Get_NumOfDisplayChannels(void)
//{
//	u8 channels = NUM_OF_CHANNEL;
//
//	switch(Get_SystemSplitMode())
//	{
//		case DISPLAY_MODE_QUAD_A:
//		case DISPLAY_MODE_QUAD_B:
//		case DISPLAY_MODE_QUAD_C:
//		case DISPLAY_MODE_QUAD_D:
//		case DISPLAY_MODE_QUAD_E:
//			channels = 4;
//			break;
//		case DISPLAY_MODE_3SPLIT_A:
//		case DISPLAY_MODE_3SPLIT_B:
//		case DISPLAY_MODE_3SPLIT_C:
//		case DISPLAY_MODE_3SPLIT_D:
//			channels = 3;
//			break;
//		case DISPLAY_MODE_2SPLIT:
//			channels = 2;
//	}
//
//	return channels;
//}

//static u8 CreateDateString(u8 *pDateStr)
//{
//  static sTimeDate_t rtcDate;
//	eDateFormat_t dateFormat;
//	BOOL year4digit;
//	u8 year[5];
//	u8 month[3];
//	u8 day[2];
//	u8 dateLength;
//
//	RTC_GetTime(&rtcDate);
//	Read_NvItem_DateFormat(&dateFormat);
//	Read_NvItem_YearFormat(&year4digit);
//
//	memset(year, NULL, sizeof(year));
//	if(year4digit == FALSE) //2digit
//	{
//		year[0] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
//		year[1] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
//		dateLength = DATE_LENGTH_2DIGIT;
//	}
//	else //4 digit
//	{
//		year[0] = 2 + ASCII_ZERO;
//		year[1] = ASCII_ZERO;
//		year[2] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
//		year[3] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
//		dateLength = DATE_LENGTH_4DIGIT;
//	}
//	strncpy(month, pStrMonthName[rtcDate.month - 1], sizeof(month));
//	day[0] = (rtcDate.day / 10) + ASCII_ZERO;
//	day[1] = (rtcDate.day % 10) + ASCII_ZERO;
//
//
//	switch(dateFormat)
//	{
//		case DATE_FORMAT_YMD: //ASIA
//			strncpy(pDateStr, year, strlen(year));
//			pDateStr += strlen(year);
//			strncpy(pDateStr, "-", 1);
//			pDateStr++;
//			strncpy(pDateStr, month, sizeof(month));
//			pDateStr += sizeof(month);
//			strncpy(pDateStr, "-", 1);
//			pDateStr++;
//			strncpy(pDateStr, day, sizeof(day));
//			break;
//
//		case DATE_FORMAT_MDY: //US
//			strncpy(pDateStr, month, sizeof(month));
//			pDateStr += sizeof(month);
//			strncpy(pDateStr, "-", 1);
//			pDateStr++;
//			strncpy(pDateStr, day, sizeof(day));
//			pDateStr += sizeof(day);
//			strncpy(pDateStr, "-", 1);
//			pDateStr++;
//			strncpy(pDateStr, year, strlen(year));
//			break;
//
//		case DATE_FORMAT_DMY: //EURO
//			strncpy(pDateStr, day, sizeof(day));
//			pDateStr += sizeof(day);
//			strncpy(pDateStr, "-", 1);
//			pDateStr++;
//			strncpy(pDateStr, month, sizeof(month));
//			pDateStr += sizeof(month);
//			strncpy(pDateStr, "-", 1);
//			pDateStr++;
//			strncpy(pDateStr, year, strlen(year));
//			break;
//	}
//	return dateLength;
//}

//static u8 CreateTimeString(u8 *pTimeStr)
//{
//	sTimeDate_t rtcTime;
//
//	RTC_GetTime(&rtcTime);
//    *pTimeStr = ((rtcTime.hour / 10)+ ASCII_ZERO);
//    *(++pTimeStr) = ((rtcTime.hour % 10)+ ASCII_ZERO);
//    *(++pTimeStr) = ':';
//    *(++pTimeStr) = ((rtcTime.min / 10)+ ASCII_ZERO);
//    *(++pTimeStr) = ((rtcTime.min % 10)+ ASCII_ZERO);
//    *(++pTimeStr) = ':';
//    *(++pTimeStr) = ((rtcTime.sec / 10)+ ASCII_ZERO);
//    *(++pTimeStr) = ((rtcTime.sec % 10)+ ASCII_ZERO);
//
//	return TIME_LENGTH;
//}

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





//-----------------------------------------------------------------------------
static void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	OSD_EnableSprite(SPRITE_INDEX0, ON);//MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

//-----------------------------------------------------------------------------
// Erase
//-----------------------------------------------------------------------------
static void OSD_EraseTitle(void)
{
	OSD_PrintString(titlePosition, osdStr_Space5, TITLE_LENGTH);
}

//static void OSD_EraseTimeDate(void)
//{
//	sPosition_t position;
//	eDisplayMode_t displayMode = Get_SystemDisplayMode();
//	u8 strSpaces[DATE_TIME_LENGTH];
//
//	position.pos_x = (DISPLAY_WIDTH - (DATE_TIME_LENGTH*CHAR_WIDTH))/2 - 12;
//	position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
//
//	memset(strSpaces, ASCII_SPACE, sizeof(strSpaces));
//	OSD_PrintString(position, strSpaces, displayingDateTimeLength);
//	displayingDateTimeLength = 0;
//}
//
//static void OSD_EraseIndicator(void)
//{
//	eChannel_t channel;
//	sPosition_t position;
//	eDisplayMode_t displayMode = Get_SystemDisplayMode();
//
//	if(displayMode == DISPLAY_MODE_FULL)
//	{
//		position = OSD_IndicatorStringPosition(Get_SystemDisplayChannel(), displayMode, strlen(osdStr_Space10));
//		OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
//	}
//	else
//	{
//		for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(); channel++)
//		{
//			position = OSD_IndicatorStringPosition(channel, displayMode, strlen(osdStr_Space1));
//			OSD_PrintString(position, osdStr_Space1, strlen(osdStr_Space1));
//		}
//	}
//}
//
//static void OSD_EraseAuto(void)
//{
//	sPosition_t position;
//	BOOL timeOn;
//
//	Read_NvItem_TimeDisplayOn(&timeOn);
//	position = OSD_GetAutoPosition(strlen(osdStr_Space4));
//
//	if(timeOn == ON)
//	{
//		position.pos_x += (2 * CHAR_WIDTH);
//	}
//	OSD_PrintString(position, osdStr_Space6, strlen(osdStr_Space4));
//}

static void OSD_EraseNoVideo(void)
{
//	sPosition_t position;
//
//	if(DISPLAY_MODE_FULL == Get_SystemDisplayMode())
//	{
//		position.pos_x = (DISPLAY_WIDTH - (strlen(osdStr_Space10)*CHAR_WIDTH))/2;
//		position.pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;
//		OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
//	}
}

void OSD_EraseVideoFormat(void)
{
	OSD_PrintString(videoInFormatPosition_Full, osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Full, osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);

	OSD_PrintString(videoInFormatPosition_Split[SPLIT_A][CHANNEL1], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_A][CHANNEL1], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoInFormatPosition_Split[SPLIT_A][CHANNEL2], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_A][CHANNEL2], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);

	OSD_PrintString(videoInFormatPosition_Split[SPLIT_C][CHANNEL1], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_C][CHANNEL1], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoInFormatPosition_Split[SPLIT_C][CHANNEL2], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);
	OSD_PrintString(videoOutFormatPosition_Split[SPLIT_C][CHANNEL2], osdSTr_Space20, VIDEO_FORMAT_LENGTH_MAX);

}

//-----------------------------------------------------------------------------
// auto
//-----------------------------------------------------------------------------
//static void OSD_DisplayAuto(void)
//{
//	BOOL autoOn;
//	sPosition_t position;
//	BOOL timeOn;
//	static BOOL previousAutoSeqOn = CLEAR;
//	u8* pStr;
//
//	autoOn = (GetCurrentAutoSeq() == AUTO_SEQ_NORMAL)?SET:CLEAR;
//
//	if((previousAutoSeqOn !=  autoOn) || (requestRefreshScreen == SET))
//	{
//		previousAutoSeqOn = autoOn;
//
//		position = OSD_GetAutoPosition(sizeof(osdStr_AUTO));
//		Read_NvItem_TimeDisplayOn(&timeOn);
//		if(timeOn == ON)
//		{
//			position.pos_x += (2 * CHAR_WIDTH);
//		}
//
//		if(autoOn == SET)
//		{
//			pStr = (u8 *)osdStr_AUTO;
//		}
//		else
//		{
//			pStr = (u8 *)osdStr_Space4;
//		}
//		OSD_PrintString(position, pStr, strlen(pStr));
//	}
//}

//-----------------------------------------------------------------------------
// No Video
//-----------------------------------------------------------------------------
static void OSD_DisplayNoVideo(void)
{
	sPosition_t position;
//	sPosition_t position;
//	eDisplayMode_t displayMode = Get_SystemDisplayMode();
//	eSplitMode_t splitMode;
//
//	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
//
//	if((videoLossDiplayOn == ON) & ((GetVideoLossEvent() == SET) || (requestRefreshScreen == SET)))
//	{
//		SetVideoLossEvent(CLEAR);
//
//		if(displayMode == DISPLAY_MODE_FULL)
//		{
//			position.pos_x = (DISPLAY_WIDTH - (strlen(osdStr_NoVideoFull)*CHAR_WIDTH))/2;
//			position.pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;
//
//			if(IsVideoLossChannel(Get_SystemDisplayChannel()) == TRUE)
//			{
//				OSD_PrintString(position, osdStr_NoVideoFull, strlen(osdStr_NoVideoFull));
//			}
//			else
//			{
//				OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
//			}
//		}
//	}
}

//-----------------------------------------------------------------------------
// Indicator - Freeze, Alarm, Motion
//-----------------------------------------------------------------------------
//static void OSD_DisplayIndicator(void)
//{
//	sPosition_t position;
//	eChannel_t channel;
//	eDisplayMode_t displayMode = Get_SystemDisplayMode();
//	eSplitMode_t splitMode = Get_SystemSplitMode();
//	BOOL videoLossDiplayOn;
//	u8* pIndicator;
//
//	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
//
//	if(displayMode == DISPLAY_MODE_FULL)//Full
//	{
//		channel = Get_SystemDisplayChannel();
////		if((IsVideoLossChannel(channel) == TRUE) && (videoLossDiplayOn == ON))
////		{
////			pIndicator = (u8*)osdStr_NoVideoFull;
////		}
////		else if(GetAlarmStatus(channel) == ALARM_SET)
//		if(GetAlarmStatus(channel) == ALARM_SET)
//		{
//			pIndicator = (u8*)osdStr_AlarmFull;
//		}
//		else if(Get_MotionDetectedStatus(channel))//motion
//		{
//			pIndicator = (u8*)osdStr_MotionFull;
//		}
//		else if(IsScreenFreeze())//freeze
//		{
//			pIndicator = (u8*)osdStr_FreezeFull;
//		}
//		else
//		{
//			pIndicator = (u8*)osdStr_Space6;
//		}
//		position = OSD_IndicatorStringPosition(channel, DISPLAY_MODE_FULL, strlen((const u8*)pIndicator));
//		OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
//	}
//	else//Split
//	{
//		for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(); channel++)
//		{
//			if((IsVideoLossChannel(channel) == TRUE) && (videoLossDiplayOn == ON))
//			{
//				pIndicator = (u8*)osdStr_NoVideo;
//			}
//			else if(GetAlarmStatus(channel) == ALARM_SET)
//			{
//				pIndicator = (u8*)osdStr_Alarm;
//			}
//			else if(Get_MotionDetectedStatus(channel))
//			{
//				pIndicator = (u8*)osdStr_Motion;
//			}
//			else if(IsScreenFreeze())
//			{
//				pIndicator = (u8*)osdStr_Freeze;
//			}
//			else
//			{
//				pIndicator = (u8*)osdStr_Space1;
//			}
//			position = OSD_IndicatorStringPosition(channel, DISPLAY_MODE_SPLIT, strlen((const u8*)pIndicator));
//			OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
//		}
//	}
//}

//-----------------------------------------------------------------------------
// Date & Time
//-----------------------------------------------------------------------------
//static void OSD_DisplayDateTime(void)
//{
//	BOOL timeDisplayOn;
//	BOOL dateDisplayOn;
//	sPosition_t position;
//	eDisplayMode_t displayMode = Get_SystemDisplayMode();
//	u8 dateTimeString[DATE_TIME_LENGTH+1];
//	u8* pStr;
//
//	Read_NvItem_DateDisplayOn(&dateDisplayOn);
//	Read_NvItem_TimeDisplayOn(&timeDisplayOn);
//
//	pStr = &dateTimeString[0];
//	memset(pStr, NULL, sizeof(dateTimeString));
//
//	if(RTC_GetDisplayTimeStatus() == SET)
//	{
//		RTC_ChangeDisplayTimeStatus(CLEAR);
//
//		if((dateDisplayOn == ON) && (timeDisplayOn == ON))
//		{
//			pStr += CreateDateString(pStr);
//			strcpy(pStr, osdStr_Space2);
//			pStr += strlen(osdStr_Space2);
//			CreateTimeString(pStr);
//		}
//		else if((dateDisplayOn == ON) && (timeDisplayOn == OFF))
//		{
//			CreateDateString(pStr);
//		}
//		else if((dateDisplayOn == OFF) && (timeDisplayOn == ON))
//		{
//			CreateTimeString(pStr);
//		}
//		displayingDateTimeLength = strlen(dateTimeString);
//
//		if(displayingDateTimeLength > 0)
//		{
//			position.pos_x = (DISPLAY_WIDTH - (strlen(dateTimeString)*CHAR_WIDTH))/2 - 12;
//			position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
//			OSD_PrintString(position, dateTimeString, displayingDateTimeLength);
//		}
//	}
//}

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
static u8* GetOutVideoFormatString(void)
{
	return (u8 *)osdStr_Format_Out_1080p60;
}

static u8* GetInVideoFormatString(eChannel_t channel)
{
	u8* inStr;

	switch(GetInputVideoFormat(channel))
	{
		// 1080p
		case AHD20_1080P_60P:
		case AHD20_1080P_30P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p30;
			break;
		case AHD20_1080P_50P:
		case AHD20_1080P_25P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p25;
			break;
		case TVI_FHD_30P:
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
		case AHD20_720P_60P:
		case AHD20_720P_30P:
		case AHD20_720P_30P_EX:
		case AHD20_720P_30P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p30;
			break;
		case AHD20_720P_50P:
		case AHD20_720P_25P:
		case AHD20_720P_25P_EX:
		case AHD20_720P_25P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p25;
			break;
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
			inStr = (u8 *)osdStr_Format_In_TVI_720p30;
			break;
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
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

		default:
			inStr = (u8 *)osdSTr_Space20;
			break;
	}
	return inStr;
}

void OSD_RefreshScreen(void)
{
	requestRefreshScreen = SET;
}

void OSD_DisplayTitle(void)
{
	eDisplayMode_t displayMode;
	u8* titleStr;

	Read_NvItem_DisplayMode(&displayMode);
	titleStr = (u8 *)osdStr_Title[displayMode];
	//OSD_EraseTitle();
	OSD_PrintString(titlePosition, titleStr, TITLE_LENGTH);
}

void OSD_DisplayVideoFormat(void)
{
	eDisplayMode_t displayMode;
	eChannel_t iChannel;
	u8* inVideo[NUM_OF_CHANNEL];
	u8* outVideo = GetOutVideoFormatString();
        
	Read_NvItem_DisplayMode(&displayMode);
	if(displayMode < DISPLAY_MODE_SPLIT_A)
	{
		inVideo[displayMode] = GetInVideoFormatString((eChannel_t)displayMode);
		OSD_PrintString(videoInFormatPosition_Full, inVideo[displayMode], strlen((const u8*)inVideo[displayMode]));
		OSD_PrintString(videoOutFormatPosition_Full, outVideo, strlen((const u8*)outVideo));
	}
	else
	{
		for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
		{
			inVideo[iChannel] = GetInVideoFormatString(iChannel);
			OSD_PrintString(
					videoInFormatPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
					inVideo[iChannel],
					strlen((const u8*)inVideo[iChannel]));
			OSD_PrintString(
					videoOutFormatPosition_Split[displayMode-DISPLAY_MODE_SPLIT_A][iChannel],
					outVideo,
					strlen((const u8*)outVideo));
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
	requestRefreshScreen = CLEAR;
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
		// FULL
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
			MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
			break;

		// Split Vertical
		case DISPLAY_MODE_SPLIT_A:
		case DISPLAY_MODE_SPLIT_B:
		case DISPLAY_MODE_SPLIT_E:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;

		// Split Horizontal
		case DISPLAY_MODE_SPLIT_C:
		case DISPLAY_MODE_SPLIT_D:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;

		// PIP
		case DISPLAY_MODE_PIP_A:
//			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
//			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;
		case DISPLAY_MODE_PIP_B:
//			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
//			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;
		case DISPLAY_MODE_PIP_C:
//			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
//			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;
		case DISPLAY_MODE_PIP_D:
//			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
//			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			break;
	}
}
