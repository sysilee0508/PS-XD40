//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	MENU.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"
#include "menu.h"
#include "menu_string.h"

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
#define	INCREASE					1
#define	DECREASE					0

#define CHARACTERS_IN_MENU_LINE		MENU_WIDTH / CHAR_WIDTH_S
#define LINES_IN_MENU				MENU_HEIGHT / CHAR_HEIGHT

#define ASCII_SPACE					0x20
#define ASCII_ZERO					0x30
#define ASCII_TILDE					0x7E

#define SELECTED_MARK				0x7F

#define UNDER_BAR					0x01

#define TIME_STRING_LENGTH			8
#define DATE_STRING_LENGTH			10

#define LINE0_OFFSET_Y				2
#define LINE1_OFFSET_Y				5// + 3
#define LINE2_OFFSET_Y				7// + 2
#define LINE3_OFFSET_Y				9
#define LINE4_OFFSET_Y				11
#define LINE5_OFFSET_Y				13
#define LINE6_OFFSET_Y				15
#define LINE7_OFFSET_Y				17
#define LINE8_OFFSET_Y				19
#define LINE9_OFFSET_Y				21
#define LINE10_OFFSET_Y				23

#define ITEM_X(x)					0x0001 << x

// Vertical items of each pages
enum
{
	MENU_PAGE_MAIN = 0,
	MENU_PAGE_TIME_DATE,
	MENU_PAGE_CAMERA_TITLE,
	MENU_PAGE_AUTO_SEQ,
	MENU_PAGE_DISPLAY,
	MENU_PAGE_ALARM_REMOCON,
	MENU_PAGE_MOTION,
	MENU_PAGE_DEVICE_INFO,
	MENU_PAGE_MAX
};

enum
{
	MAINMENU_ITEM_Y_TITLE = 0,
	MAINMENU_ITEM_Y_TIME_DATE,
	MAINMENU_ITEM_Y_CAMERA_TITLE,
	MAINMENU_ITEM_Y_AUTOSEQ,
	MAINMENU_ITEM_Y_DISPLAY,
	MAINMENU_ITEM_Y_ALARM_REMOCON,
	MAINMENU_ITEM_Y_MOTION,
	MAINMENU_ITEM_Y_DEVICE_INFO,
	MAINMENU_ITEM_Y_MAX
};
enum
{
	TIMEDATE_ITEM_Y_TITLE = 0,
	TIMEDATE_ITEM_Y_TIME,
	TIMEDATE_ITEM_Y_DATE,
	TIMEDATE_ITEM_Y_TIME_DISPLAY,
	TIMEDATE_ITEM_Y_DATE_DISPALY,
	TIMEDATE_ITEM_Y_DATE_FORMAT,
	TIMEDATE_ITEM_Y_YEAR_FORMAT,
	TIMEDATE_ITEM_Y_TIME_CORRECTION,
	TIMEDATE_ITEM_Y_MAX
};

enum
{
	CAMERATITLE_ITEM_Y_TITLE = 0,
	CAMERATITLE_ITEM_Y_CH1,
	CAMERATITLE_ITEM_Y_CH2,
	CAMERATITLE_ITEM_Y_CH3,
	CAMERATITLE_ITEM_Y_CH4,
	CAMERATITLE_ITEM_Y_DISPLAY_ON,
//	CAMERATITLE_ITEM_Y_POSITION,
	CAMERATITLE_ITEM_Y_MAX
};

enum
{
	AUTOSEQ_ITEM_Y_TITLE = 0,
	AUTOSEQ_ITEM_Y_CH1_DISPLAY_TIME,
	AUTOSEQ_ITEM_Y_CH2_DISPLAY_TIME,
	AUTOSEQ_ITEM_Y_CH3_DISPLAY_TIME,
	AUTOSEQ_ITEM_Y_CH4_DISPLAY_TIME,
	AUTOSEQ_ITEM_Y_LOSS_SKIP,
	AUTOSEQ_ITEM_Y_MAX
};

enum
{
	DISPLAY_ITEM_Y_TITLE = 0,
	DISPLAY_ITEM_Y_RESOLUTION,
	DISPLAY_ITEM_Y_OSD_DISPLAY,
	DISPLAY_ITEM_Y_BORDER_LINE_DISPLAY,
	DISPLAY_ITEM_Y_SPLIT_MODE,
	DISPLAY_ITEM_Y_MAX
};

enum
{
	ALARM_ITEM_Y_TITLE = 0,
	ALARM_ITEM_Y_ALARM_REMOCON,
	ALARM_ITEM_Y_CH1,
	ALARM_ITEM_Y_CH2,
	ALARM_ITEM_Y_CH3,
	ALARM_ITEM_Y_CH4,
	ALARM_ITEM_Y_ALARMOUT_TIME,
	ALARM_ITEM_Y_ALARM_BUZZER_TIME,
	ALARM_ITEM_Y_LOSS_BUZZER_TIME,
	ALARM_ITEM_Y_REMOCONID,
	ALARM_ITEM_Y_BAUDRATE,
	ALARM_ITEM_Y_MAX
};

enum
{
	MOTION_ITEM_Y_TITLE = 0,
	MOTION_ITEM_Y_CH1,
	MOTION_ITEM_Y_CH2,
	MOTION_ITEM_Y_CH3,
	MOTION_ITEM_Y_CH4,
	MOTION_ITEM_Y_SENSITIVITY,
	MOTION_ITEM_Y_MOTION_MODE,
	MOTION_ITEM_Y_MAX
};

enum
{
	DEVICEINFO_ITEM_Y_TITLE = 0,
	DEVICEINFO_ITEM_Y_FW_VERSION,
	DEVICEINFO_ITEM_Y_MAX
};

typedef struct
{
	u16 offset_x;
	u16 offset_y;
	const BYTE* str;
} sLocationNString_t;

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
static WORD OSDMenuID, OSDCombID;
static BYTE fMenuUpdate;//, fMenuGraphic;

//-----------------------------------------------------------------
// declare static variables
//-----------------------------------------------------------------
static u8 currentPage = MENU_PAGE_MAIN;
static u8 lineBuffer[CHARACTERS_IN_MENU_LINE];
static BOOL requestEnterKeyProc = CLEAR;
static u8 systemMode = SYSTEM_NORMAL_MODE;

//-----------------------------------------------------------------
// declare global variables
//---------------------------------------------------------------

#if 1
//-----------------------------------------------------------------
// constant variables
//-----------------------------------------------------------------
static BYTE demoIPCMode[sizeof(defIPCModeSize)];
static BYTE demoNRMode[sizeof(defNRModeSize)];
static BYTE demoEnhance[sizeof(defEnhanceSize)];
static BYTE demoInOutMode[sizeof(defInOutModeSize)];
static BYTE demoPicture[sizeof(defPictureSize)];
static BYTE demoSetUp[sizeof(defSetUpSize)];
static BYTE demoFilter[sizeof(defFilterSize)];
static BYTE demoAudio[sizeof(defAudioSize)];	   //by hungry 2012.03.14

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
void SetOSDMenuID(WORD nID)
{
	OSDMenuID = nID;
	fMenuUpdate = ON;
}

//--------------------------------------------------------------------------------------------------
WORD GetOSDMenuID(void)
{
	return OSDMenuID;
}

//--------------------------------------------------------------------------------------------------
void SetOSDCombID(WORD nID)
{
	OSDCombID = nID;
}

//--------------------------------------------------------------------------------------------------
WORD GetMenuStatus(BYTE nID, BYTE sID)
{
	if (nID==1) return demoIPCMode[sID-1];
	if (nID==2) return demoNRMode[sID-1];
	if (nID==3) return demoEnhance[sID-1];
	if (nID==4) return demoInOutMode[sID-1];
	if (nID==5) return demoPicture[sID-1];
	if (nID==6) return demoSetUp[sID-1];
	if (nID==7) return demoFilter[sID-1];
//	if (nID==8) return demoEVideo[sID-1];		 //by hungry 2012.03.14
	if (nID==8) return demoAudio[sID-1];		 //by hungry 2012.03.14
	return 0;
}

//--------------------------------------------------------------------------------------------------
void SetMenuStatus(BYTE nID, BYTE sID, BYTE val)
{
	if (nID==1) demoIPCMode[sID-1] = val;
	if (nID==2) demoNRMode[sID-1] = val;
	if (nID==3) demoEnhance[sID-1] = val;
	if (nID==4) demoInOutMode[sID-1] = val;
	if (nID==5) demoPicture[sID-1] = val;
	if (nID==6) demoSetUp[sID-1] = val;
	if (nID==7) demoFilter[sID-1] = val;
//	if (nID==8) demoEVideo[sID-1] = val;		   //by hungry 2012.03.14
	if (nID==8) demoAudio[sID-1] = val;		   //by hungry 2012.03.14
}

//--------------------------------------------------------------------------------------------------
void SetMenuDefaultStatus(void)
{
	memcpy(demoIPCMode, defIPCMode, sizeof(defIPCMode));
	memcpy(demoNRMode, defNRMode, sizeof(defNRMode));
	memcpy(demoEnhance, defEnhance, sizeof(defEnhance));
	memcpy(demoInOutMode, defInOutMode, sizeof(defInOutMode));
	memcpy(demoPicture, defPicture, sizeof(defPicture));
	memcpy(demoSetUp, defSetUp, sizeof(defSetUp));
	memcpy(demoFilter, defFilter, sizeof(defFilter));
//	memcpy(demoEVideo, defEVideo, sizeof(defEVideo));		//by hungry 2012.03.14
	memcpy(demoAudio, defAudio, sizeof(defAudio));		//by hungry 2012.03.14
//	OSDMenuID = OSDCombID = 0;
}

#endif
//--------------------------------------------------------------------------------------------------
static void Toggle(BOOL* pObject)
{
	*pObject = (*pObject == FALSE)?TRUE:FALSE;
}

//static void Int2Bcd(u8 in_data, u8* out_data)
//{
//	*out_data = ((in_data / 10) << 4) | ((in_data % 10) & 0x0F);
//}

static void Int2Str(u8 in_data, u8* out_data)
{
	*out_data = ((in_data / 10) + ASCII_ZERO);
  	*(out_data+1) = ((in_data % 10) + ASCII_ZERO);
}

static void IncreaseDecreaseCount(u8 max,u8 min,BOOL inc_dec,u8 *pCount)
{
	switch(inc_dec)
	{
		case INCREASE:
			if(*pCount < max)
			{
				(*pCount)++;
			}
			else
			{
				*pCount = min;
			}
			break;

		case DECREASE:
			if(*pCount > min)
			{
				(*pCount)--;
			}
			else
			{
				*pCount = max;
			}
			break;

		default:
			break;
	}
}

//--------------------------------------------------------------------------------------
static void Print_StringWithSelectedMarkSize(u16 offset_x, u16 offset_y, const u8 *data, u8 attribute, u16 sizeOfMark)
{
	u16 i;
	sPosition_t position;

	position.pos_x = MENU_START_POSITION_X + (offset_x * CHAR_WIDTH_S);
	position.pos_y = MENU_START_POSITION_Y + (offset_y * CHAR_HEIGHT);
	
	OSD_PrintString(position, data, strlen(data));

	memset(lineBuffer, ASCII_SPACE, CHARACTERS_IN_MENU_LINE);
	for(i = 0; i < sizeOfMark; i++)
	{
		lineBuffer[i] = (attribute == NULL)? ASCII_SPACE : SELECTED_MARK;
	}
    position.pos_y += CHAR_HEIGHT;
	OSD_PrintString(position, lineBuffer, sizeOfMark);
}
//--------------------------------------------------------------------------------------
static void Print_StringWithSelectedMark(u16 offset_x, u16 offset_y, const u8 *data, u8 attribute, u16 size)
{
	u16 i;
	sPosition_t position;

	position.pos_x = MENU_START_POSITION_X + (offset_x * CHAR_WIDTH_S);
	position.pos_y = MENU_START_POSITION_Y + (offset_y * CHAR_HEIGHT);

	OSD_PrintString(position, data, size);

	memset(lineBuffer, ASCII_SPACE, CHARACTERS_IN_MENU_LINE);
	for(i = 0; i < size; i++)
	{
		lineBuffer[i] = (attribute == NULL)? ASCII_SPACE : SELECTED_MARK;
	}

	position.pos_y += CHAR_HEIGHT;
	OSD_PrintString(position, lineBuffer, size);
}
//--------------------------------------------------------------------------------------
static void Print_StringOnOff(u16 offset_x, u16 offset_y, u8 attribute, BOOL on_off)
{
	if(on_off == ON)
	{
		Print_StringWithSelectedMarkSize(offset_x, offset_y, menuStr_Space3, NULL, strlen(menuStr_Space3));
		Print_StringWithSelectedMark(offset_x, offset_y, menuStr_On, attribute, strlen(menuStr_On));
	}
	else
	{
		Print_StringWithSelectedMark(offset_x, offset_y, menuStr_Off, attribute, strlen(menuStr_Off));
	}
}

//--------------------------------------------------------------------------------------
static void Erase_AllMenuScreen(void)
{
	int line = 0;
	
	memset(lineBuffer, ASCII_SPACE, sizeof(lineBuffer)); 
	for(line = 0; line < LINES_IN_MENU; line++)	
	{
		MDINGAC_SetDrawXYMode(
				MENU_START_POSITION_Y + (line*CHAR_HEIGHT), 
				MENU_START_POSITION_X, 
				lineBuffer, 
				sizeof(lineBuffer), 
				0);
	}
}

//-----------------------------------------------------------------
// Draw allow to mark which item is selected
//-----------------------------------------------------------------
static void DrawSelectMark(u8 verticalItem)
{	
	static const u8 offset_x[MENU_PAGE_MAX] = {19,13,18,17,18,17,17,17}; //in characters
	static u8 previousLocationX;
	static u8 previousLocationY;
	u8 offset_y;
	sPosition_t position;

	offset_y = LINE1_OFFSET_Y + (2 * (verticalItem-1));

	// erase previous mark
	position.pos_x = MENU_START_POSITION_X + (previousLocationX * CHAR_WIDTH_S);
	position.pos_y = MENU_START_POSITION_Y + (previousLocationY * CHAR_HEIGHT);
	OSD_PrintString(position,menuStr_Space1, strlen(menuStr_Space1));
	// draw new mark
	position.pos_x = MENU_START_POSITION_X + (offset_x[currentPage] * CHAR_WIDTH_S);
	position.pos_y = MENU_START_POSITION_Y + (offset_y * CHAR_HEIGHT);
	OSD_PrintString(position, menuStr_ArrowL, strlen(menuStr_ArrowL));

	previousLocationX = offset_x[currentPage];
	previousLocationY = offset_y;
}

static void MainMenu_Entry(u8 itemY)
{
	const sLocationNString_t mainMenu[MAINMENU_ITEM_Y_MAX] =
	{
			{24, LINE0_OFFSET_Y, menuStr_MainMenu_Title},
			{22, LINE1_OFFSET_Y, menuStr_MainMenu_TimeDate},
			{22, LINE2_OFFSET_Y, menuStr_MainMenu_CameraTitle},
			{22, LINE3_OFFSET_Y, menuStr_MainMenu_AutoSeq},
			{22, LINE4_OFFSET_Y, menuStr_MainMenu_Display},
			{22, LINE5_OFFSET_Y, menuStr_MainMenu_Alarm},
			{22, LINE6_OFFSET_Y, menuStr_MainMenu_MotionDetection},
			{22, LINE7_OFFSET_Y, menuStr_MainMenu_DeviceInfo}
	};
	u8 index;

	currentPage = MENU_PAGE_MAIN;
	Erase_AllMenuScreen();
	requestEnterKeyProc = CLEAR;

	MDINOSD_SetBGBoxColor(RGB(0,0,0));		// set BG-BOX color

	MDINOSD_SetBGBoxArea(BGBOX_INDEX0, MENU_START_POSITION_X, MENU_START_POSITION_Y, MENU_WIDTH, MENU_HEIGHT);
	MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
	MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);

	DrawSelectMark(itemY);
	for(index = 0; index < MAINMENU_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				mainMenu[index].offset_x,
				mainMenu[index].offset_y,
				mainMenu[index].str,
				NULL, 0);
	}
}


//------------------------------------------------------------------
//   Time/Date Page Function
//------------------------------------------------------------------
const sLocationNString_t timeDateMenu[TIMEDATE_ITEM_Y_MAX] =
{
		{24, LINE0_OFFSET_Y, menuStr_TimeDate_Title},
		{17, LINE1_OFFSET_Y, menuStr_TimeDate_Time},
		{17, LINE2_OFFSET_Y, menuStr_TimeDate_Date},
		{17, LINE3_OFFSET_Y, menuStr_TimeDate_TimeDisplay},
		{17, LINE4_OFFSET_Y, menuStr_TimeDate_DateDisplay},
		{17, LINE5_OFFSET_Y, menuStr_TimeDate_DateFormat},
		{17, LINE6_OFFSET_Y, menuStr_TimeDate_YearFormat},
		{17, LINE7_OFFSET_Y, menuStr_TimeDate_TimeCorrection}
};

static void Print_StringTimeCorrect(u16 itemX,u8 attribute)
{
	sTimeCorrect_t timeCorrect;
	u8 offsetStr[2];
	u8 selectedMark[3];

	Read_NvItem_TimeCorrect(&timeCorrect);

	selectedMark[0] = (itemX & 0x01)?attribute:NULL;
	selectedMark[1] = (itemX & 0x02)?attribute:NULL;
	selectedMark[2] = (itemX & 0x04)?attribute:NULL;

	// +/-
	if(timeCorrect.timeCorrectDirection == DIRECTION_UP) // up --> '+'
	{
		Print_StringWithSelectedMarkSize(
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_x + strlen(menuStr_TimeDate_TimeCorrection),
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_y,
				(const u8*)"+", selectedMark[0], 1);
	}
	else if(timeCorrect.timeCorrectDirection == DIRECTION_DOWN) // down --> '-'
	{
		Print_StringWithSelectedMarkSize(
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_x + strlen(menuStr_TimeDate_TimeCorrection),
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_y,
				(const u8*)"-", selectedMark[0], 1);
	}
	// offset
	Int2Str(timeCorrect.timeCorrectOffset, offsetStr);
	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_x + strlen(menuStr_TimeDate_TimeCorrection) + 1,
			timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_y,
			offsetStr, selectedMark[1], sizeof(offsetStr));
	// unit
	if(timeCorrect.timeCorrectUint == TIME_UNIT_MONTH)
	{
		Print_StringWithSelectedMark(
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_x + strlen(menuStr_TimeDate_TimeCorrection) + 1 + sizeof(offsetStr),
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_y,
				menuStr_SecMonth,
				selectedMark[2], strlen(menuStr_SecMonth));
	}
	else if(timeCorrect.timeCorrectUint == TIME_UNIT_DAY)
	{
		Print_StringWithSelectedMarkSize(
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_x + strlen(menuStr_TimeDate_TimeCorrection) + 1 + sizeof(offsetStr),
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_y,
				menuStr_Space9, NULL, strlen(menuStr_Space9));
		Print_StringWithSelectedMark(
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_x + strlen(menuStr_TimeDate_TimeCorrection) + 1 + sizeof(offsetStr),
				timeDateMenu[TIMEDATE_ITEM_Y_TIME_CORRECTION].offset_y,
				menuStr_SecDay, selectedMark[2], strlen(menuStr_SecDay));
	}
}

static void Print_StringTime(u16 itemX, u8 attribute, sTimeDate_t time)
{
	u8 hourStr[2], minStr[2], secStr[2];
	u8 timeStr[TIME_STRING_LENGTH] = {0,};
	u8 selectedMark[3];
	char* pStr;

	selectedMark[0] = (itemX & 0x01)?attribute:NULL;
	selectedMark[1] = (itemX & 0x02)?attribute:NULL;
	selectedMark[2] = (itemX & 0x04)?attribute:NULL;

	Int2Str(time.hour, hourStr);
	Int2Str(time.min, minStr);
	Int2Str(time.sec, secStr);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_x + strlen(menuStr_TimeDate_Time),
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_y,
			hourStr, selectedMark[0], sizeof(hourStr));

	Print_StringWithSelectedMarkSize(
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_x + strlen(menuStr_TimeDate_Time) + 2,
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_y,
			":", NULL, 1);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_x + strlen(menuStr_TimeDate_Time) + sizeof(hourStr) + 1,
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_y,
			minStr, selectedMark[1], sizeof(minStr));

	Print_StringWithSelectedMarkSize(
		timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_x + strlen(menuStr_TimeDate_Time) + 5,
		timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_y,
		":", NULL, 1);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_x + strlen(menuStr_TimeDate_Time) + sizeof(hourStr) + sizeof(minStr) + 2,
			timeDateMenu[TIMEDATE_ITEM_Y_TIME].offset_y,
			secStr, selectedMark[2], sizeof(secStr));
}

static void Print_StringDate(u16 itemX,u8 attribute,sTimeDate_t date)
{
//	sTimeDate_t date;
	u8 yearStr[4] = {'2','0',}, monthStr[2], dayStr[2];
	u8 dateStr[DATE_STRING_LENGTH] = {'2','0',};
	char* pStr;
	u8 selectedMark[3];

	selectedMark[0] = (itemX & 0x0001)?attribute:NULL;
	selectedMark[1] = (itemX & 0x0002)?attribute:NULL;
	selectedMark[2] = (itemX & 0x0004)?attribute:NULL;

//	RTC_GetTime(&date);
	Int2Str(date.year + DEFAULT_YEAR, yearStr+2);
	Int2Str(date.month, monthStr);
	Int2Str(date.day, dayStr);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_x + strlen(menuStr_TimeDate_Date),
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_y,
			yearStr, selectedMark[0], 4);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_x + strlen(menuStr_TimeDate_Date) + 4,
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_y,
			"-", NULL, 1);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_x + strlen(menuStr_TimeDate_Date) + 5,
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_y,
			monthStr, selectedMark[1],2);

	Print_StringWithSelectedMarkSize(
		timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_x + strlen(menuStr_TimeDate_Date) + 7,
		timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_y,
		"-", NULL, 1);

	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_x + strlen(menuStr_TimeDate_Date) + 8,
			timeDateMenu[TIMEDATE_ITEM_Y_DATE].offset_y,
			dayStr, selectedMark[2], 2);
}  

static void Print_StringDateFormat(u8 attribute)
{
	eDateFormat_t format;

	Read_NvItem_DateFormat(&format);
	switch(format)
	{
		case DATE_FORMAT_YMD:	//ASIA
			Print_StringWithSelectedMarkSize(
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_x + strlen(menuStr_TimeDate_DateFormat),
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_y, menuStr_Asia, attribute, 4);
			break;

		case DATE_FORMAT_MDY:	//USA
			Print_StringWithSelectedMarkSize(
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_x + strlen(menuStr_TimeDate_DateFormat),
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_y,
					menuStr_Space4, NULL, strlen((char*)menuStr_Space4));
			Print_StringWithSelectedMarkSize(
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_x + strlen(menuStr_TimeDate_DateFormat),
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_y, menuStr_Usa, attribute, 3);
			break;

		case DATE_FORMAT_DMY:	//EURO
			Print_StringWithSelectedMarkSize(
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_x + strlen(menuStr_TimeDate_DateFormat),
					timeDateMenu[TIMEDATE_ITEM_Y_DATE_FORMAT].offset_y, menuStr_Euro, attribute, 4);
			break;
	}
}

static void Print_StringYearFormat(u8 attribute)
{
	u8 year4digit;
	u8 strNum;

	Read_NvItem_YearFormat(&year4digit);
	if(year4digit == FALSE)
	{
		strNum = '2';
	}
	else
	{
		strNum = '4';
	}
	Print_StringWithSelectedMark(
			timeDateMenu[TIMEDATE_ITEM_Y_YEAR_FORMAT].offset_x + strlen(menuStr_TimeDate_YearFormat),
			timeDateMenu[TIMEDATE_ITEM_Y_YEAR_FORMAT].offset_y, (const u8*)&strNum, attribute, 1);
	Print_StringWithSelectedMarkSize(
			timeDateMenu[TIMEDATE_ITEM_Y_YEAR_FORMAT].offset_x + strlen(menuStr_TimeDate_YearFormat) + 2,
			timeDateMenu[TIMEDATE_ITEM_Y_YEAR_FORMAT].offset_y, menuStr_Digit, NULL, strlen(menuStr_Digit));
}

static void TimeDatePage_UpdatePage(u16 itemX, u8 itemY, sTimeDate_t time)
{
	BOOL timeDisplay, dateDisplay;
	u8 attribute = (requestEnterKeyProc == SET)?UNDER_BAR:NULL;
	switch(itemY)
	{
		case TIMEDATE_ITEM_Y_TIME:
			Print_StringTime(itemX, attribute, time);
			break;

		case TIMEDATE_ITEM_Y_DATE :
			Print_StringDate(itemX, attribute, time);
			break;

		case TIMEDATE_ITEM_Y_TIME_DISPLAY:
			Read_NvItem_TimeDisplayOn(&timeDisplay);
			Print_StringOnOff(
			timeDateMenu[itemY].offset_x + strlen(timeDateMenu[itemY].str),
			timeDateMenu[itemY].offset_y,
			attribute, timeDisplay);
			break;

		case TIMEDATE_ITEM_Y_DATE_DISPALY :
			Read_NvItem_DateDisplayOn(&dateDisplay);
			Print_StringOnOff(timeDateMenu[itemY].offset_x + strlen(timeDateMenu[itemY].str),
			timeDateMenu[itemY].offset_y,
			attribute, dateDisplay);
			break;

		case TIMEDATE_ITEM_Y_DATE_FORMAT:
			Print_StringDateFormat(attribute);
			break;

		case TIMEDATE_ITEM_Y_YEAR_FORMAT:
			Print_StringYearFormat(attribute);
			break;

		case TIMEDATE_ITEM_Y_TIME_CORRECTION:
			Print_StringTimeCorrect(itemX, attribute);
			break;
	}
}

static void TimeDatePage_Entry(void)
{   
	u8 index = 0;
	BOOL timeDisplayOn, dateDisplayOn;
	sTimeDate_t time = {0,};

	currentPage = MENU_PAGE_TIME_DATE;
	Erase_AllMenuScreen();
	requestEnterKeyProc = CLEAR;
	RTC_GetTime(&time);

	DrawSelectMark(TIMEDATE_ITEM_Y_TIME);
	Read_NvItem_TimeDisplayOn(&timeDisplayOn);
	Read_NvItem_DateDisplayOn(&dateDisplayOn);
	// display items of this page
	for(index = 0; index < TIMEDATE_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(timeDateMenu[index].offset_x, timeDateMenu[index].offset_y, timeDateMenu[index].str, NULL, 0);
		TimeDatePage_UpdatePage(0, index, time);
	}
}

static void TimeDatePage_KeyHandler(eKeyData_t key)
{
	static u8 pos_x = 0;
	static u8 itemY = TIMEDATE_ITEM_Y_TIME;
	u16 itemX;
	BOOL inc_dec = DECREASE;
	static sTimeDate_t rtcTime;
	BOOL displayOn;
	eDateFormat_t dateFormat;
	BOOL yearFormat;
	sTimeCorrect_t timeCorrect;

	switch(key)
	{
		case KEY_UP :
			inc_dec = INCREASE;
		case KEY_DOWN :
			if(requestEnterKeyProc == SET)
			{
				switch(itemY)
				{
					case TIMEDATE_ITEM_Y_TIME:
						switch(pos_x)
						{
							case 0://hour
								IncreaseDecreaseCount(23, 0, inc_dec, &rtcTime.hour);
								break;
							case 1://min
								IncreaseDecreaseCount(59, 0, inc_dec, &rtcTime.min);
								break;
							case 2://sec
								IncreaseDecreaseCount(59, 0, inc_dec, &rtcTime.sec);
								break;
						}
						break;

					case TIMEDATE_ITEM_Y_DATE:
						switch(pos_x)
						{
							case 0://year
								IncreaseDecreaseCount(81, 0, inc_dec, &rtcTime.year); //2018~2099
								break;
							case 1://month
								IncreaseDecreaseCount(12, 1, inc_dec, &rtcTime.month);
								break;
							case 2://day
								IncreaseDecreaseCount(GetDaysInMonth(rtcTime.month, rtcTime.year), 1, inc_dec, &rtcTime.day);
								break;
						}
						break;

					case TIMEDATE_ITEM_Y_TIME_DISPLAY:
						Read_NvItem_TimeDisplayOn(&displayOn);
						Toggle(&displayOn);
						Write_NvItem_TimeDisplayOn(displayOn);
						break;

					case TIMEDATE_ITEM_Y_DATE_DISPALY:
						Read_NvItem_DateDisplayOn(&displayOn);
						Toggle(&displayOn);
						Write_NvItem_DateDisplayOn(displayOn);
						break;

					case TIMEDATE_ITEM_Y_DATE_FORMAT:
						Read_NvItem_DateFormat(&dateFormat);
						IncreaseDecreaseCount(2, 0, inc_dec, &dateFormat);
						Write_NvItem_DateFormat(dateFormat);
						break;

					case TIMEDATE_ITEM_Y_YEAR_FORMAT:
						Read_NvItem_YearFormat(&yearFormat);
						Toggle(&yearFormat);
						Write_NvItem_YearFormat(yearFormat);
						break;

					case TIMEDATE_ITEM_Y_TIME_CORRECTION:
						Read_NvItem_TimeCorrect(&timeCorrect);
						switch(pos_x)
						{
							case 0: //direction
								if(timeCorrect.timeCorrectDirection == DIRECTION_UP)
								{
									timeCorrect.timeCorrectDirection = DIRECTION_DOWN;
								}
								else if(timeCorrect.timeCorrectDirection == DIRECTION_DOWN)
								{
									timeCorrect.timeCorrectDirection = DIRECTION_UP;
								}
								break;
							case 1: //offset
								IncreaseDecreaseCount(59,0,inc_dec,&timeCorrect.timeCorrectOffset);
								break;
							case 2: //unit
								if(timeCorrect.timeCorrectUint == TIME_UNIT_DAY)
								{
									timeCorrect.timeCorrectUint = TIME_UNIT_MONTH;
								}
								else if(timeCorrect.timeCorrectUint == TIME_UNIT_MONTH)
								{
									timeCorrect.timeCorrectUint = TIME_UNIT_DAY;
								}
								break;
						}
						Write_NvItem_TimeCorrect(timeCorrect);
						break;
				}
				TimeDatePage_UpdatePage(ITEM_X(pos_x), itemY, rtcTime);
			}
			else
			{
				IncreaseDecreaseCount(7, 1, inc_dec, &itemY);
				DrawSelectMark(itemY);
				pos_x = 0;
			}
			break;

		case KEY_RIGHT :
			inc_dec = INCREASE;
		case KEY_LEFT  :
			if(requestEnterKeyProc)
			{
				Toggle(&requestEnterKeyProc);
				if((itemY == TIMEDATE_ITEM_Y_TIME) ||
						(itemY == TIMEDATE_ITEM_Y_DATE) ||
						(itemY == TIMEDATE_ITEM_Y_TIME_CORRECTION))
				{
					TimeDatePage_UpdatePage(ITEM_X(pos_x), itemY, rtcTime);
					IncreaseDecreaseCount(2, 0,inc_dec, &pos_x);
					requestEnterKeyProc = SET;
					TimeDatePage_UpdatePage(ITEM_X(pos_x), itemY, rtcTime);
				}
			}
			break;

		case KEY_ENTER :
			Toggle(&requestEnterKeyProc);
			if((itemY == TIMEDATE_ITEM_Y_TIME) || (itemY == TIMEDATE_ITEM_Y_DATE))
			{
				if(requestEnterKeyProc == SET)
				{
					RTC_GetTime(&rtcTime);
				}
				else
				{
					RTC_SetTime(&rtcTime);
				}
			}
			TimeDatePage_UpdatePage(ITEM_X(pos_x), itemY, rtcTime);
			break; 	

		case KEY_EXIT :
			if(requestEnterKeyProc == SET)
			{
				Toggle(&requestEnterKeyProc);
				if((itemY == TIMEDATE_ITEM_Y_TIME) || (itemY == TIMEDATE_ITEM_Y_DATE))
				{
					RTC_SetTime(&rtcTime);
				}
				TimeDatePage_UpdatePage(ITEM_X(pos_x), itemY, rtcTime);
			}
			else
			{
				itemY = TIMEDATE_ITEM_Y_TIME;
				pos_x = 0;
				MainMenu_Entry(currentPage);
			}
			break;
	}
}
   

//------------------------------------------------------------------
//	Camera Title Menu
//------------------------------------------------------------------
/*
static const u8* pTitlePosition_Str[TITLE_POSITION_MAX] =
{
		menuStr_TopLeft,
		menuStr_TopCenter,
		menuStr_TopRight,
		menuStr_BottomLeft,
		menuStr_BottomCenter,
		menuStr_BottomRight,
		menuStr_Center
};
*/

const sLocationNString_t cameraTitle[CAMERATITLE_ITEM_Y_MAX] =
{
		{25, LINE0_OFFSET_Y, menuStr_CamaraTitle_Title},
		{20, LINE1_OFFSET_Y, menuStr_CameraTitle_Ch1},
		{20, LINE2_OFFSET_Y, menuStr_CameraTitle_Ch2},
		{20, LINE3_OFFSET_Y, menuStr_CameraTitle_Ch3},
		{20, LINE4_OFFSET_Y, menuStr_CameraTitle_Ch4},
		{20, LINE5_OFFSET_Y, menuStr_CameraTitle_TitleDisplay}
//		{20, LINE6_OFFSET_Y, menuStr_CameraTitle_Position}
};

static void CameraTitlePage_UpdatePage(u8 itemY, u8 pos_x)
{
	BOOL titleOn;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX];// = {0,};
	//eTitlePosition_t titlePosition;
	u8* pChar;
	u8 attribute = (requestEnterKeyProc == SET)?UNDER_BAR:NULL;

	switch(itemY)
	{
		case CAMERATITLE_ITEM_Y_CH1:
		case CAMERATITLE_ITEM_Y_CH2:
		case CAMERATITLE_ITEM_Y_CH3:
		case CAMERATITLE_ITEM_Y_CH4:
			Read_NvItem_ChannelName(channel_name, (eChannel_t)(itemY - 1));
			// print full name
			Print_StringWithSelectedMarkSize(
					cameraTitle[itemY].offset_x + strlen(cameraTitle[itemY].str),
					cameraTitle[itemY].offset_y,
					(const u8*)channel_name,
					NULL, 0);
			// get selected character
			pChar = &channel_name[pos_x];
			// and print
			Print_StringWithSelectedMark(
					cameraTitle[itemY].offset_x + strlen(cameraTitle[itemY].str) + pos_x,
					cameraTitle[itemY].offset_y,
					(const u8*)pChar,
					attribute, 1);
			break;

		case CAMERATITLE_ITEM_Y_DISPLAY_ON:
			Read_NvItem_TitleDispalyOn(&titleOn);
			Print_StringOnOff(
					cameraTitle[itemY].offset_x + strlen(cameraTitle[itemY].str),
					cameraTitle[itemY].offset_y,
					attribute, titleOn);
			break;
/*
		case CAMERATITLE_ITEM_Y_POSITION:
			Read_NvItem_TitlePosition(&titlePosition);
			Print_StringWithSelectedMarkSize(
					cameraTitle[itemY].offset_x + strlen(cameraTitle[itemY].str),
					cameraTitle[itemY].offset_y,
					menuStr_Space13,
					NULL, strlen(menuStr_Space13));
			Print_StringWithSelectedMark(
					cameraTitle[itemY].offset_x + strlen(cameraTitle[itemY].str),
					cameraTitle[itemY].offset_y,
					pTitlePosition_Str[titlePosition],
					attribute,
					strlen(pTitlePosition_Str[titlePosition]));
			break;
*/
	}

}

static void CameraTitlePage_Entry(void)
{
	u8 index = 0;

	currentPage = MENU_PAGE_CAMERA_TITLE;
	Erase_AllMenuScreen();
	requestEnterKeyProc = CLEAR;

	DrawSelectMark(CAMERATITLE_ITEM_Y_CH1);
	for(index = 0; index < CAMERATITLE_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(cameraTitle[index].offset_x, cameraTitle[index].offset_y, cameraTitle[index].str, NULL, 0);
		CameraTitlePage_UpdatePage(index, 0);
	}
}

static void CameraTitlePage_KeyHandler(eKeyData_t key)
{
	static u8 pos_x = 0;
	static u8 itemY = CAMERATITLE_ITEM_Y_CH1;
	BOOL inc_dec = DECREASE;
	BOOL titleOn;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX];
//	eTitlePosition_t titlePosition;
	u8* pChar;

	switch(key)
	{
	  	case KEY_UP  :
	  		inc_dec = INCREASE;
	    case KEY_DOWN  :
	    	if(requestEnterKeyProc)
			{  
				switch(itemY)
				{
					case CAMERATITLE_ITEM_Y_CH1:
					case CAMERATITLE_ITEM_Y_CH2:
					case CAMERATITLE_ITEM_Y_CH3:
					case CAMERATITLE_ITEM_Y_CH4:
						Read_NvItem_ChannelName(channel_name, (eChannel_t)(itemY - 1));
						pChar = &channel_name[pos_x];
						IncreaseDecreaseCount(ASCII_TILDE, ASCII_SPACE, inc_dec, pChar);
						Write_NvItem_ChannelName(channel_name, (eChannel_t)(itemY - 1));
						break;

					case CAMERATITLE_ITEM_Y_DISPLAY_ON:
						Read_NvItem_TitleDispalyOn(&titleOn);
						Toggle(&titleOn);
						Write_NvItem_TitleDispalyOn(titleOn);
						break;
/*
					case CAMERATITLE_ITEM_Y_POSITION:
						Read_NvItem_TitlePosition(&titlePosition);
						IncreaseDecreaseCount(TITLE_POSITION_MAX, 0, inc_dec, &titlePosition);
						Write_NvItem_TitlePosition(titlePosition);
						break;*/
				}
				CameraTitlePage_UpdatePage(itemY, pos_x);
			}
			else
			{
				IncreaseDecreaseCount(CAMERATITLE_ITEM_Y_MAX - 1, 1,inc_dec, &itemY);
				DrawSelectMark(itemY);
			}
	  		break;

		case KEY_RIGHT :
			inc_dec = INCREASE;
		case KEY_LEFT  :
	    	if(requestEnterKeyProc)
			{
				if((itemY >= CAMERATITLE_ITEM_Y_CH1) && (itemY <= CAMERATITLE_ITEM_Y_CH4))
				{
					requestEnterKeyProc = CLEAR;
					CameraTitlePage_UpdatePage(itemY, pos_x);
	  				IncreaseDecreaseCount(CHANNEL_NEME_LENGTH_MAX - 1, 0, inc_dec,&pos_x);
	  				requestEnterKeyProc = SET;
	  				CameraTitlePage_UpdatePage(itemY, pos_x);
				}
			}
			else
			{
				DrawSelectMark(itemY);
			}
			break;
	  
		case KEY_ENTER:
			Toggle(&requestEnterKeyProc);
//			if(requestEnterKeyProc == SET)
//			{
//				pos_x = 0;
//			}
			CameraTitlePage_UpdatePage(itemY, pos_x);
			break; 	

		case KEY_EXIT :
	    	if(requestEnterKeyProc)
			{
				Toggle(&requestEnterKeyProc);
				CameraTitlePage_UpdatePage(itemY, 0);
				DrawSelectMark(itemY);
			}
			else 
			{
				itemY = CAMERATITLE_ITEM_Y_CH1;
				pos_x = 0;
				MainMenu_Entry(currentPage);
			}
			break; 	
	}
}  


//------------------------------------------------------------------
//	Auto Seq Page Function
//------------------------------------------------------------------
const sLocationNString_t autoSeqMenu[AUTOSEQ_ITEM_Y_MAX] =
{
	{24, LINE0_OFFSET_Y, menuStr_AutoSeq_Title},
	{20, LINE1_OFFSET_Y, menuStr_AutoSeq_DisplayTime_Ch1},
	{20, LINE2_OFFSET_Y, menuStr_AutoSeq_DisplayTime_Ch2},
	{20, LINE3_OFFSET_Y, menuStr_AutoSeq_DisplayTime_Ch3},
	{20, LINE4_OFFSET_Y, menuStr_AutoSeq_DisplayTime_Ch4},
	{20, LINE5_OFFSET_Y, menuStr_AutoSeq_NoVideoSkip}
};

static void AutoSeqPage_UpdatePage(u8 itemY)
{
	u8 displayTime[NUM_OF_CHANNEL];
	u8 displayTimeInStr[2];
	BOOL videoLossSkip;
	u8 attribute = (requestEnterKeyProc == SET)?UNDER_BAR:NULL;

	switch(itemY)
	{
    	case AUTOSEQ_ITEM_Y_CH1_DISPLAY_TIME :
    	case AUTOSEQ_ITEM_Y_CH2_DISPLAY_TIME:
    	case AUTOSEQ_ITEM_Y_CH3_DISPLAY_TIME:
    	case AUTOSEQ_ITEM_Y_CH4_DISPLAY_TIME:
    		Read_NvItem_AutoSeqTime(displayTime);
			if(displayTime[itemY-1] == 0)
			{
				// print OFF
				Print_StringWithSelectedMarkSize(
						autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
				        menuStr_Space6, 
				        NULL, strlen(menuStr_Space6));
				Print_StringWithSelectedMark(
						autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
						menuStr_Off,
						attribute, strlen(menuStr_Off));
			}
			else
			{
				Int2Str(displayTime[itemY-1], displayTimeInStr);
				Print_StringWithSelectedMarkSize(
						autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
						menuStr_Space3,
						NULL, strlen(menuStr_Space3));
				Print_StringWithSelectedMark(
						autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
						(const u8*)displayTimeInStr,
						attribute, sizeof(displayTimeInStr));
				Print_StringWithSelectedMarkSize(
						autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str) + sizeof(displayTimeInStr),
						autoSeqMenu[itemY].offset_y,
						menuStr_Sec,
						NULL, strlen(menuStr_Sec));
			}
    		break;

    	case AUTOSEQ_ITEM_Y_LOSS_SKIP :
    		Read_NvItem_AutoSeqLossSkip(&videoLossSkip);
    		if(videoLossSkip == ON) //skip
    		{
    			Print_StringWithSelectedMarkSize(
    					autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
    					menuStr_Space9,
    					NULL, strlen(menuStr_Space9));
    			Print_StringWithSelectedMarkSize(
    					autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
    					menuStr_Skip,
    					attribute, strlen(menuStr_Skip));
    		}
    		else //show
    		{
    			Print_StringWithSelectedMarkSize(
    					autoSeqMenu[itemY].offset_x + strlen(autoSeqMenu[itemY].str),
						autoSeqMenu[itemY].offset_y,
    					menuStr_Show,
    					attribute, strlen(menuStr_Show));
    		}
    		break;
 	}
}

static void AutoSeqPage_Entry()
{
	u8 index = 0;

	currentPage = MENU_PAGE_AUTO_SEQ;
	Erase_AllMenuScreen();
	requestEnterKeyProc = CLEAR;
	DrawSelectMark(AUTOSEQ_ITEM_Y_CH1_DISPLAY_TIME);

	for(index = 0; index < AUTOSEQ_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				autoSeqMenu[index].offset_x,
				autoSeqMenu[index].offset_y,
				autoSeqMenu[index].str,
				NULL, 0);
		AutoSeqPage_UpdatePage(index);
	}
}

static void AutoSeqPage_KeyHandler(eKeyData_t key)
{
	static u8 itemY = AUTOSEQ_ITEM_Y_CH1_DISPLAY_TIME;
	u8 inc_dec = DECREASE;
	static u8 autoSeqTime[NUM_OF_CHANNEL];
	BOOL autoSeqLossSkip;

	switch(key)
	{
		case KEY_UP  :
			inc_dec = INCREASE;
		case KEY_DOWN  :
			if(requestEnterKeyProc)
			{
				switch(itemY)
				{
					case AUTOSEQ_ITEM_Y_CH1_DISPLAY_TIME:
					case AUTOSEQ_ITEM_Y_CH2_DISPLAY_TIME:
					case AUTOSEQ_ITEM_Y_CH3_DISPLAY_TIME:
					case AUTOSEQ_ITEM_Y_CH4_DISPLAY_TIME:
						Read_NvItem_AutoSeqTime(autoSeqTime);
						IncreaseDecreaseCount(60,0,inc_dec, &autoSeqTime[itemY-1]);
						Write_NvItem_AutoSeqTime(autoSeqTime);
						break;
					case AUTOSEQ_ITEM_Y_LOSS_SKIP:
						Read_NvItem_AutoSeqLossSkip(&autoSeqLossSkip);
						Toggle(&autoSeqLossSkip);
						Write_NvItem_AutoSeqLossSkip(autoSeqLossSkip);
						break;
				}
				AutoSeqPage_UpdatePage(itemY);
			}
			else
			{
				IncreaseDecreaseCount(5, 1, inc_dec, &itemY);
				DrawSelectMark(itemY);
			}
			break;

		case KEY_ENTER :
			Toggle(&requestEnterKeyProc);
			AutoSeqPage_UpdatePage(itemY);
			break;

		case KEY_EXIT :
			if(requestEnterKeyProc == SET)
			{
				requestEnterKeyProc = CLEAR;
				AutoSeqPage_UpdatePage(itemY);
				DrawSelectMark(itemY);
			}
			else
			{
				itemY = AUTOSEQ_ITEM_Y_CH1_DISPLAY_TIME;
				MainMenu_Entry(currentPage);
			}
			break;
	}
}   

//------------------------------------------------------------------
// Display Page Function
//------------------------------------------------------------------
const sLocationNString_t displayMenu[DISPLAY_ITEM_Y_MAX] =
{
	{24, LINE0_OFFSET_Y, menuStr_Display_Title},
	{20, LINE1_OFFSET_Y, menuStr_Display_Resolution},
	{20, LINE2_OFFSET_Y, menuStr_Display_OsdDisplay},
	{20, LINE3_OFFSET_Y, menuStr_Display_BorderLine},
	{20, LINE4_OFFSET_Y, menuStr_Display_SplitMode}
};
static BOOL splitModeSelecting = FALSE;

static u8* Get_String_SplitMode(splitMode)
{
	u8* pStr;

	switch(splitMode)
	{
		case DISPLAY_MODE_QUAD_A:
			pStr = (u8*)menuStr_SplitMode_QuadA;
			break;
		case DISPLAY_MODE_QUAD_B:
			pStr = (u8*)menuStr_SplitMode_QuadB;
			break;
		case DISPLAY_MODE_QUAD_C:
			pStr = (u8*)menuStr_SplitMode_QuadC;
			break;
		case DISPLAY_MODE_QUAD_D:
			pStr = (u8*)menuStr_SplitMode_QuadD;
			break;
		case DISPLAY_MODE_QUAD_E:
			pStr = (u8*)menuStr_SplitMode_QuadE;
			break;
		case DISPLAY_MODE_3SPLIT_A:
			pStr = (u8*)menuStr_SplitMode_3SplitA;
			break;
		case DISPLAY_MODE_3SPLIT_B:
			pStr = (u8*)menuStr_SplitMode_3SplitB;
			break;
		case DISPLAY_MODE_3SPLIT_C:
			pStr = (u8*)menuStr_SplitMode_3SplitC;
			break;
		case DISPLAY_MODE_3SPLIT_D:
			pStr = (u8*)menuStr_SplitMode_3SplitD;
			break;
		case DISPLAY_MODE_2SPLIT:
			pStr = (u8*)menuStr_SplitMode_2Split;
			break;
	}

	return pStr;
}

static void DisplayPage_DisplaySplitMode(const u8* pStr)
{
	eSplitMode_t splitMode = Get_SystemSplitMode();
	sPosition_t position;

	Erase_AllMenuScreen();
	MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
	splitModeSelecting = TRUE;

	DisplayMode_SplitScreen(splitMode);
        OSD_DrawBorderLine();

	position.pos_x = (DISPLAY_WIDTH - strlen(pStr))/2;
	position.pos_y = 100;
	OSD_PrintString(position, menuStr_Space8, strlen(menuStr_Space8));
	OSD_PrintString(position, pStr, strlen(pStr));
}

static void DisplayPage_UpdatePageOption(u8 itemY)
{
	eResolution_t resolution;
	BOOL osdOn;
	BOOL borderLineOn;
	eSplitMode_t splitMode;
	u8* pStr_SplitMode;
	u8 attribute = (requestEnterKeyProc == SET)?UNDER_BAR:NULL;

	switch(itemY)
	{
		case DISPLAY_ITEM_Y_RESOLUTION:
			Read_NvItem_Resolution(&resolution);
			switch(resolution)
			{
				case RESOLUTION_1920_1080_60P:
					Print_StringWithSelectedMark(
							displayMenu[itemY].offset_x + strlen(displayMenu[itemY].str),
							displayMenu[itemY].offset_y,
							menuStr_Resolution1920X1080_60P,
							attribute,
							strlen(menuStr_Resolution1920X1080_60P));
					break;
				case RESOLUTION_1920_1080_50P:
					Print_StringWithSelectedMark(
							displayMenu[itemY].offset_x + strlen(displayMenu[itemY].str),
							displayMenu[itemY].offset_y,
							menuStr_Resolution1920X1080_50P,
							attribute,
							strlen(menuStr_Resolution1920X1080_50P));
					break;
			}
			break;

		case DISPLAY_ITEM_Y_OSD_DISPLAY:
			Read_NvItem_OsdOn(&osdOn);
			Print_StringOnOff(
					displayMenu[itemY].offset_x + strlen(displayMenu[itemY].str),
					displayMenu[itemY].offset_y,
					attribute, osdOn);
			break;

		case DISPLAY_ITEM_Y_BORDER_LINE_DISPLAY:
			Read_NvItem_BorderLineDisplay(&borderLineOn);
			Print_StringOnOff(
					displayMenu[itemY].offset_x + strlen(displayMenu[itemY].str),
					displayMenu[itemY].offset_y,
					attribute, borderLineOn);
			break;

		case DISPLAY_ITEM_Y_SPLIT_MODE:
			pStr_SplitMode = Get_String_SplitMode(Get_SystemSplitMode());
			if(requestEnterKeyProc == CLEAR)
			{
				Print_StringWithSelectedMark(
						displayMenu[itemY].offset_x + strlen(displayMenu[itemY].str),
						displayMenu[itemY].offset_y,
						menuStr_Space8,
						NULL,
						strlen(menuStr_Space8));
				Print_StringWithSelectedMark(
						displayMenu[itemY].offset_x + strlen(displayMenu[itemY].str),
						displayMenu[itemY].offset_y,
						(const u8*)pStr_SplitMode,
						attribute,
						strlen(pStr_SplitMode));
			}
			else
			{
				DisplayPage_DisplaySplitMode((const u8*)pStr_SplitMode);
			}
			break;
	}
}

static void DisplayPage_Entry(void)
{
	u8 index;

	currentPage = MENU_PAGE_DISPLAY;
	Erase_AllMenuScreen();
	requestEnterKeyProc = CLEAR;

	DrawSelectMark(DISPLAY_ITEM_Y_RESOLUTION);
	for(index = 0; index < DISPLAY_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				displayMenu[index].offset_x,
				displayMenu[index].offset_y,
				displayMenu[index].str,
				NULL, 0);
		DisplayPage_UpdatePageOption(index);
	}
}

static void DisplayPage_RedrawPage(u8 itemY)
{
	u8 index;

	MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
	splitModeSelecting = FALSE;
	for(index = 0; index < DISPLAY_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				displayMenu[index].offset_x,
				displayMenu[index].offset_y,
				displayMenu[index].str,
				NULL, 0);
		DisplayPage_UpdatePageOption(index);
	}
	DrawSelectMark(itemY);
}

static void DisplayPage_KeyHandler(eKeyData_t key)
{
	static u8 itemY = DISPLAY_ITEM_Y_RESOLUTION;
	u8 inc_dec = DECREASE;
	eResolution_t resolution;
	BOOL osdOn;
	BOOL borderLineOn;
	eSplitMode_t splitMode;

	switch(key)
	{
		case KEY_UP:
			inc_dec = INCREASE;
		case KEY_DOWN:
			if(requestEnterKeyProc)
			{
				switch(itemY)
				{
					case DISPLAY_ITEM_Y_RESOLUTION:
						Read_NvItem_Resolution(&resolution);
						IncreaseDecreaseCount(RESOLUTION_MAX - 1, 0, inc_dec, &resolution);
						Write_NvItem_Resolution(resolution);
						break;
					case DISPLAY_ITEM_Y_OSD_DISPLAY:
						Read_NvItem_OsdOn(&osdOn);
						Toggle(&osdOn);
						Write_NvItem_OsdOn(osdOn);
						break;
					case DISPLAY_ITEM_Y_BORDER_LINE_DISPLAY:
						Read_NvItem_BorderLineDisplay(&borderLineOn);
						Toggle(&borderLineOn);
						Write_NvItem_BorderLineDisplay(borderLineOn);
						break;
					case DISPLAY_ITEM_Y_SPLIT_MODE:
						splitMode = Get_SystemSplitMode();
						IncreaseDecreaseCount(DISPLAY_MODE_MAX - 1, 0, inc_dec, &splitMode);
						Set_SystemSplitMode(splitMode);
						break;
				}
				DisplayPage_UpdatePageOption(itemY);
			}
			else
			{
				IncreaseDecreaseCount(DISPLAY_ITEM_Y_MAX - 1, 1, inc_dec, &itemY);
				DrawSelectMark(itemY);
			}
			break;

		case KEY_ENTER:
			Toggle(&requestEnterKeyProc);
			DisplayPage_UpdatePageOption(itemY);
			break;

		case KEY_EXIT:
			if(requestEnterKeyProc)
			{
				Toggle(&requestEnterKeyProc);
				if((splitModeSelecting == TRUE) && (requestEnterKeyProc == CLEAR))
				{
					DisplayPage_RedrawPage(itemY);
				}
				else
				{
					DisplayPage_UpdatePageOption(itemY);
				}
			}
			else
			{
				itemY = DISPLAY_ITEM_Y_RESOLUTION;
				splitModeSelecting = FALSE;
				MainMenu_Entry(currentPage);
			}
			break;
	}
}  

//------------------------------------------------------------------
//   Alarm/Remote Page Function
//------------------------------------------------------------------
const sLocationNString_t alarmRemoconMenu[ALARM_ITEM_Y_MAX] =
{
	{24, LINE0_OFFSET_Y, menuStr_Alarm_Title},
	{20, LINE1_OFFSET_Y, menuStr_Alarm_AlarmRemocon},
	{20, LINE2_OFFSET_Y, menuStr_Alarm_Channel1},
	{20, LINE3_OFFSET_Y, menuStr_Alarm_Channel2},
	{20, LINE4_OFFSET_Y, menuStr_Alarm_Channel3},
	{20, LINE5_OFFSET_Y, menuStr_Alarm_Channel4},
	{20, LINE6_OFFSET_Y, menuStr_Alarm_AlarmOutTime},
	{20, LINE7_OFFSET_Y, menuStr_Alarm_AlarmBuzzerTime},
	{20, LINE8_OFFSET_Y, menuStr_Alarm_VideoLossBuzzerTime},
	{20, LINE9_OFFSET_Y, menuStr_Alarm_RemoconId},
	{20, LINE10_OFFSET_Y, menuStr_Alarm_BaudRate}
};

static void Print_StringAlarmRemoconSelection(u8 attribute)
{
	BOOL alarmRemoconSelection;

	Read_NvItem_AlarmRemoconSelect(&alarmRemoconSelection);
	if(alarmRemoconSelection == ALARM_MODE)
	{
		Print_StringWithSelectedMarkSize(
				alarmRemoconMenu[ALARM_ITEM_Y_ALARM_REMOCON].offset_x + strlen(menuStr_Alarm_AlarmRemocon),
				alarmRemoconMenu[ALARM_ITEM_Y_ALARM_REMOCON].offset_y,
				menuStr_Space7,
				NULL, strlen(menuStr_Space7));
		Print_StringWithSelectedMark(
				alarmRemoconMenu[ALARM_ITEM_Y_ALARM_REMOCON].offset_x + strlen(menuStr_Alarm_AlarmRemocon),
				alarmRemoconMenu[ALARM_ITEM_Y_ALARM_REMOCON].offset_y,
				menuStr_Alarm,
				attribute, strlen(menuStr_Alarm));
	}
	else //remote key
	{
		Print_StringWithSelectedMark(
				alarmRemoconMenu[ALARM_ITEM_Y_ALARM_REMOCON].offset_x + strlen(menuStr_Alarm_AlarmRemocon),
				alarmRemoconMenu[ALARM_ITEM_Y_ALARM_REMOCON].offset_y,
				menuStr_Remocon,
				attribute, strlen(menuStr_Remocon));
	}
}

static void Print_StringAlarmOption(u16 offset_x, u16 offset_y, u8 attribute, eChannel_t channel)
{
	eAlarmOption_t alarmOption;

	Read_NvItem_AlarmOption(&alarmOption, channel);

	switch(alarmOption)
	{
		case ALARM_OPTION_NO: // normal open
			Print_StringWithSelectedMark(offset_x, offset_y, menuStr_NO, attribute, strlen(menuStr_NO));
			break;

		case ALARM_OPTION_NC: // normal close
			Print_StringWithSelectedMark(offset_x, offset_y, menuStr_NC, attribute, strlen(menuStr_NC));
			break;

		case ALARM_OPTION_OFF:
			Print_StringWithSelectedMark(offset_x, offset_y, menuStr_Off, attribute, strlen(menuStr_Off));
			break;
	}
}

static void AlarmRemoconPage_UpdatePageOption(u8 itemY)//, u8 pos_x)
{
//	u16 itemX = ITEM_X(pos_x);
	u8 nv_data;
	u8 str2digit[2];
	u8 attribute = (requestEnterKeyProc == SET)?UNDER_BAR:NULL;
	u8* baudrateStr;

 	switch(itemY)
 	{
		case ALARM_ITEM_Y_ALARM_REMOCON:
			Print_StringAlarmRemoconSelection(attribute);
			break;

 	  	case ALARM_ITEM_Y_CH1:
 	  	case ALARM_ITEM_Y_CH2:
 	  	case ALARM_ITEM_Y_CH3:
 	  	case ALARM_ITEM_Y_CH4:
 	  		Print_StringAlarmOption(
 	  				alarmRemoconMenu[itemY].offset_x + 15,//strlen(alarmRemoconMenu[itemY].str),
					alarmRemoconMenu[itemY].offset_y,
					attribute, itemY - 2);
 	  		break;

 	  	case ALARM_ITEM_Y_ALARMOUT_TIME:
			Read_NvItem_AlarmOutTime(&nv_data);
            if(nv_data != 0)
            {
				Int2Str(nv_data, str2digit);
                Print_StringWithSelectedMarkSize(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Space3,
						NULL, strlen(menuStr_Space3));
				Print_StringWithSelectedMark(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						(const u8*)str2digit,
						attribute, sizeof(str2digit));
				Print_StringWithSelectedMarkSize(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str) + sizeof(str2digit),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Sec,
						NULL, 0);
            }
            else // print OFF
            {
                Print_StringWithSelectedMarkSize(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Space6,
						NULL, strlen(menuStr_Space6));
                Print_StringWithSelectedMark(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Off,
						attribute, strlen(menuStr_Off));
            }
			break; 

 		case ALARM_ITEM_Y_ALARM_BUZZER_TIME :
			Read_NvItem_AlarmBuzzerTime(&nv_data);
			if(nv_data != 0)
			{
				Int2Str(nv_data, str2digit);
                Print_StringWithSelectedMarkSize(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Space3,
						NULL, strlen(menuStr_Space3));
				Print_StringWithSelectedMark(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						(const u8*)str2digit,
						attribute, sizeof(str2digit));
				Print_StringWithSelectedMarkSize(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str) + sizeof(str2digit),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Sec,
						NULL, 0);
			}
            else // print OFF
            {
                Print_StringWithSelectedMarkSize(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Space6,
						NULL, strlen(menuStr_Space6));
                Print_StringWithSelectedMark(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Off,
						attribute, strlen(menuStr_Off));
            }

			break;

 		case ALARM_ITEM_Y_LOSS_BUZZER_TIME:
 			Read_NvItem_VideoLossBuzzerTime(&nv_data);
 			if(nv_data != 0)
 			{
				Int2Str(nv_data, str2digit);
                Print_StringWithSelectedMarkSize(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Space3,
						NULL, strlen(menuStr_Space3));
				Print_StringWithSelectedMark(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						(const u8*)str2digit,
						attribute, sizeof(str2digit));
				Print_StringWithSelectedMarkSize(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str) + sizeof(str2digit),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Sec,
						NULL, 0);
 			}
 			else
 			{
                Print_StringWithSelectedMarkSize(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Space6,
						NULL, strlen(menuStr_Space6));
                Print_StringWithSelectedMark(
                		alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_Off,
						attribute, strlen(menuStr_Off));
 			}
			break; 

 		case ALARM_ITEM_Y_REMOCONID:
 			Read_NvItem_RemoconId(&nv_data);
 			if(nv_data != 0)
 			{
				Int2Str(nv_data, str2digit);
				Print_StringWithSelectedMark(
						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						(const u8*)str2digit,
						attribute, sizeof(str2digit));
 			}
 			else
 			{
 				Print_StringWithSelectedMark(
 						alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
						alarmRemoconMenu[itemY].offset_y,
						menuStr_None,
						attribute, strlen(menuStr_None));
 			}
 			break;

 		case ALARM_ITEM_Y_BAUDRATE:
 			Read_NvItem_SerialBaudrate((eBaudRate_t *)&nv_data);
 			switch(nv_data)
 			{
				case BAUDRATE_1200:
					baudrateStr = (u8*)menuStr_Baudrate1200;
					break;
				case BAUDRATE_2400:
					baudrateStr = (u8*)menuStr_Baudrate2400;
					break;
				case BAUDRATE_9600:
					baudrateStr = (u8*)menuStr_Baudrate9600;
					break;
 			}
 			Print_StringWithSelectedMark(
 					alarmRemoconMenu[itemY].offset_x + strlen(alarmRemoconMenu[itemY].str),
					alarmRemoconMenu[itemY].offset_y,
					(const u8*)baudrateStr,
					attribute, strlen(baudrateStr));

 			break;
 	}
}

static void AlarmRemoconPage_Entry(void)
{
	u8 index;

	currentPage = MENU_PAGE_ALARM_REMOCON;
	requestEnterKeyProc = CLEAR;
	Erase_AllMenuScreen();

	DrawSelectMark(ALARM_ITEM_Y_ALARM_REMOCON);
	for(index = 0; index < ALARM_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				alarmRemoconMenu[index].offset_x,
				alarmRemoconMenu[index].offset_y,
				alarmRemoconMenu[index].str,
				NULL, 0);
		AlarmRemoconPage_UpdatePageOption(index);//, 0);
	}
}

static void AlaramRemoconPage_KeyHandler(eKeyData_t key)
{
	static u8 itemY = ALARM_ITEM_Y_ALARM_REMOCON;
	//static u8 pos_x = 0;
	u8 itemX;
	u8 inc_dec = DECREASE;
	BOOL alarmRemoconSel;
	eAlarmOption_t alarmOption;
	u8 intData;
	eBaudRate_t baudrate;

	switch(key)
	{
		case KEY_UP :
			inc_dec = INCREASE;
    	case KEY_DOWN :
    		if(requestEnterKeyProc)
			{
    			switch(itemY)
    			{
    				case ALARM_ITEM_Y_ALARM_REMOCON:
    					Read_NvItem_AlarmRemoconSelect(&alarmRemoconSel);
    					Toggle(&alarmRemoconSel);
    					ChangeAlarmRemoteKeyMode(alarmRemoconSel);
    					Write_NvItem_AlarmRemoconSelect(alarmRemoconSel);
    					break;

    				case ALARM_ITEM_Y_CH1:
    				case ALARM_ITEM_Y_CH2:
    				case ALARM_ITEM_Y_CH3:
    				case ALARM_ITEM_Y_CH4:
    					Read_NvItem_AlarmOption(&alarmOption, itemY - 2);
    					IncreaseDecreaseCount(2, 0, inc_dec, (u8 *)&alarmOption);
    					Write_NvItem_AlarmOption(alarmOption, itemY - 2);
    					break;

    				case ALARM_ITEM_Y_ALARMOUT_TIME:
    					Read_NvItem_AlarmOutTime(&intData);
    					IncreaseDecreaseCount(99, 0, inc_dec, &intData);
    					Write_NvItem_AlarmOutTime(intData);
    					break;

    				case ALARM_ITEM_Y_ALARM_BUZZER_TIME:
    					Read_NvItem_AlarmBuzzerTime(&intData);
    					IncreaseDecreaseCount(99, 0, inc_dec, &intData);
    					Write_NvItem_AlarmBuzzerTime(intData);
    					break;

    				case ALARM_ITEM_Y_LOSS_BUZZER_TIME:
    					Read_NvItem_VideoLossBuzzerTime(&intData);
    					IncreaseDecreaseCount(99, 0, inc_dec, &intData);
    					Write_NvItem_VideoLossBuzzerTime(intData);
    					break;

    				case ALARM_ITEM_Y_REMOCONID:
    					Read_NvItem_RemoconId(&intData);
    					IncreaseDecreaseCount(REMOCON_ID_MAX, REMOCON_ID_NONE, inc_dec, &intData);
    					Write_NvItem_RemoconId(intData);
    					break;

    				case ALARM_ITEM_Y_BAUDRATE:
    					Read_NvItem_SerialBaudrate(&baudrate);
    					IncreaseDecreaseCount(2, 0, inc_dec, &baudrate);
    					Write_NvItem_SerialBaudrate(baudrate);
    					break;

    			}
    			AlarmRemoconPage_UpdatePageOption(itemY);
			}
			else 
			{
				IncreaseDecreaseCount(ALARM_ITEM_Y_MAX-1, 1, inc_dec, &itemY);
				DrawSelectMark(itemY);//,0);
			}
  			break;	

    	case KEY_RIGHT:
    		inc_dec = INCREASE;
    	case KEY_LEFT:
    		// To Do : video loss alarm set for each channel
    		break;

    	case KEY_ENTER :
    		Toggle(&requestEnterKeyProc);
    		AlarmRemoconPage_UpdatePageOption(itemY);
			break; 	

		case KEY_EXIT:
       		if(requestEnterKeyProc)
			{
       			Toggle(&requestEnterKeyProc);
       			AlarmRemoconPage_UpdatePageOption(itemY);
			}
			else 
			{
				ChangeBaudrate();
				itemY = ALARM_ITEM_Y_ALARM_REMOCON;
				MainMenu_Entry(currentPage);
			}
			break; 	
	}
}   

//------------------------------------------------------------------
//   Motion Detection Page Function
//------------------------------------------------------------------
static BOOL areaSelecting = FALSE;
const sLocationNString_t motionDetectionMenu[MOTION_ITEM_Y_MAX] =
{
	{24, LINE0_OFFSET_Y, menuStr_Motion_Title},
	{20, LINE1_OFFSET_Y, menuStr_Motion_Channel1},
	{20, LINE2_OFFSET_Y, menuStr_Motion_Channel2},
	{20, LINE3_OFFSET_Y, menuStr_Motion_Channel3},
	{20, LINE4_OFFSET_Y, menuStr_Motion_Channel4},
	{20, LINE5_OFFSET_Y, menuStr_Motion_Sensitivity},
	{20, LINE6_OFFSET_Y, menuStr_Motion_CallMode}
};

static void Print_StringSelectArea(u16 offset_x, u16 offset_y,BOOL active)
{
	u8 attribute = NULL;

	if(active)
	{
		attribute = UNDER_BAR;
	}
	Print_StringWithSelectedMark(offset_x, offset_y, menuStr_SelectArea, attribute, strlen(menuStr_SelectArea));
}

static void MotionDetectionPage_DrawCursor(u8 offset_x, u8 offset_y, BOOL active)
{
	sPosition_t position;
	const u8 cursorData = SELECTED_MARK;
	const u8 nullData = ASCII_SPACE;

	position.pos_x = (offset_x * BLOCK_WIDTH) + (BLOCK_WIDTH - CHAR_WIDTH_S)/2 - 1;
	position.pos_y = (offset_y * BLOCK_HEIGHT) + ((BLOCK_HEIGHT - CHAR_HEIGHT)/2 - 1) + CHAR_HEIGHT;
	if(active == TRUE)
	{
		OSD_PrintString(position, &cursorData, sizeof(cursorData));
	}
	else
	{
		OSD_PrintString(position, &nullData, sizeof(nullData));
	}
}

static void MotionDetectionPage_DrawBlockMark(u8 offset_x, u8 offset_y, BOOL active)
{
	sPosition_t position;

	position.pos_x = (offset_x * BLOCK_WIDTH) + (BLOCK_WIDTH - CHAR_WIDTH_S)/2 - 1;
	position.pos_y = (offset_y * BLOCK_HEIGHT) + ((BLOCK_HEIGHT - CHAR_HEIGHT)/2 - 1);
	if(active == TRUE)
	{
		OSD_PrintString(position, menuStr_SelectedMark, strlen(menuStr_SelectedMark));
	}
	else
	{
		OSD_PrintString(position, menuStr_UnselectedMart, strlen(menuStr_UnselectedMart));
	}
}

static void MotionDetectionPage_EraseAllBlockMark(void)
{
	sPosition_t position;
	u8 blockX, blockY;

	for(blockY = 0; blockY < ROWS_OF_BLOCKS; blockY++)
	{
		for(blockX = 0; blockX < COLUMMS_OF_BLOCKS; blockX++)
		{
			position.pos_x = (blockX * BLOCK_WIDTH) + (BLOCK_WIDTH - CHAR_WIDTH_S)/2 - 1;
			position.pos_y = (blockY * BLOCK_HEIGHT) + ((BLOCK_HEIGHT - CHAR_HEIGHT)/2 - 1);
			OSD_PrintString(position, menuStr_Space1, strlen(menuStr_Space1));
		}
	}
}

static void MotionDetectionPage_DrawSelectedArea(eChannel_t channel)
{
	u8 blockX, blockY;
	sPosition_t mark;
	u16 blocks[ROWS_OF_BLOCKS];
	u16 block_mask;

	// Clear screen
	Erase_AllMenuScreen();
	MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
	DisplayMode_FullScreen(channel);

	MotionDetectionPage_DrawCursor(0, 0, TRUE);
	Read_NvItem_MotionBlock(blocks, channel);
	for(blockY = 0; blockY < ROWS_OF_BLOCKS; blockY++)
	{
		for(blockX = 0; blockX < COLUMMS_OF_BLOCKS; blockX++)
		{
			MotionDetectionPage_DrawBlockMark(blockX, blockY, (blocks[blockY]>>blockX)&0x0001);
		}
	}
}

static void MotionDetectionPage_UpdatePage(u8 itemY, u8 itemX)
{
	u8 sensitivity;
	u8 str[2];
	u8 attribute = (requestEnterKeyProc==SET)?UNDER_BAR:NULL;
	BOOL motionOn;

	switch(itemY)
	{
		case MOTION_ITEM_Y_CH1:
		case MOTION_ITEM_Y_CH2:
		case MOTION_ITEM_Y_CH3:
		case MOTION_ITEM_Y_CH4:
			Read_NvItem_MotionDetectOnOff(&motionOn, itemY - 1);
			if(itemX == 0)
			{
				Print_StringOnOff(
						motionDetectionMenu[itemY].offset_x + 9,
						motionDetectionMenu[itemY].offset_y,
						attribute, motionOn);
				Print_StringSelectArea(
						motionDetectionMenu[itemY].offset_x + strlen(motionDetectionMenu[itemY].str),
						motionDetectionMenu[itemY].offset_y,
						FALSE);
			}
			else
			{
				Print_StringOnOff(
						motionDetectionMenu[itemY].offset_x + 9,
						motionDetectionMenu[itemY].offset_y,
						NULL, motionOn);
				Print_StringSelectArea(
						motionDetectionMenu[itemY].offset_x + strlen(motionDetectionMenu[itemY].str),
						motionDetectionMenu[itemY].offset_y,
						attribute);
			}
			break;

		case MOTION_ITEM_Y_SENSITIVITY:
			Read_NvItem_MotionSensitivity(&sensitivity);
			Int2Str(sensitivity, str);
			Print_StringWithSelectedMark(
					motionDetectionMenu[itemY].offset_x + strlen(motionDetectionMenu[itemY].str),
					motionDetectionMenu[itemY].offset_y,
					(const u8*)str,
					attribute, sizeof(str));
			break;

		case MOTION_ITEM_Y_MOTION_MODE:
			break;
	}
}

static void MotionDetectionPage_Entry(void)
{
	u8 index;

	currentPage = MENU_PAGE_MOTION;
	Erase_AllMenuScreen();
	requestEnterKeyProc = CLEAR;

	DrawSelectMark(MOTION_ITEM_Y_CH1);
	for(index = 0; index < MOTION_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				motionDetectionMenu[index].offset_x,
				motionDetectionMenu[index].offset_y,
				motionDetectionMenu[index].str,
				NULL, 0);
		MotionDetectionPage_UpdatePage(index, 0);
	}
}


static void MotionDetectionPage_KeyHandler(eKeyData_t key)
{
	static u8 itemY = MOTION_ITEM_Y_CH1;
	static u8 pos_x = 0;
	static u8 cursorX = 0;
	static u8 cursorY = 0;
	u8 inc_dec = DECREASE;
	u8 sensitivity;
	BOOL motionOn;
	u16 activeBlocks[ROWS_OF_BLOCKS];
	BOOL active;

	switch(key)
	{
		case KEY_UP :
			inc_dec = INCREASE;
		case KEY_DOWN :
			if(!areaSelecting)
			{
				if(requestEnterKeyProc)
				{
					switch(itemY)
					{
						case MOTION_ITEM_Y_CH1:
						case MOTION_ITEM_Y_CH2:
						case MOTION_ITEM_Y_CH3:
						case MOTION_ITEM_Y_CH4:
							Read_NvItem_MotionDetectOnOff(&motionOn, (eChannel_t)(itemY - 1));
							Toggle(&motionOn);
							Write_NvItem_MotionDetectOnOff(motionOn, (eChannel_t)(itemY - 1));
							break;

						case MOTION_ITEM_Y_SENSITIVITY:
							Read_NvItem_MotionSensitivity(&sensitivity);
							IncreaseDecreaseCount(99,1,inc_dec,&sensitivity);
							Write_NvItem_MotionSensitivity(sensitivity);
							//Set_MotionDetect_Sensitivity(sensitivity);
							break;

						case MOTION_ITEM_Y_MOTION_MODE:
							break;
					}
					MotionDetectionPage_UpdatePage(itemY, pos_x);
				}
				else
				{
					IncreaseDecreaseCount(6, 1, inc_dec, &itemY);
					DrawSelectMark(itemY);
				}
			}
			else
			{
				//selecting area
				MotionDetectionPage_DrawCursor(cursorX, cursorY, FALSE);
				IncreaseDecreaseCount(ROWS_OF_BLOCKS-1, 0, inc_dec, &cursorY);
				MotionDetectionPage_DrawCursor(cursorX, cursorY, TRUE);
			}
			break;

    	case KEY_RIGHT:
    		inc_dec = INCREASE;
    	case KEY_LEFT:
    		if(!areaSelecting)
    		{
				if(requestEnterKeyProc)
				{
					if((itemY >= MOTION_ITEM_Y_CH1) && (itemY <= MOTION_ITEM_Y_CH4))
					{
						requestEnterKeyProc = CLEAR;
						MotionDetectionPage_UpdatePage(itemY, pos_x);
						IncreaseDecreaseCount(1, 0, inc_dec,&pos_x);
						requestEnterKeyProc = SET;
						MotionDetectionPage_UpdatePage(itemY, pos_x);
					}
				}
				else
				{
					DrawSelectMark(itemY);
				}
    		}
    		else
    		{
				//selecting area
				MotionDetectionPage_DrawCursor(cursorX, cursorY, FALSE);
				IncreaseDecreaseCount(COLUMMS_OF_BLOCKS-1, 0, inc_dec, &cursorX);
				MotionDetectionPage_DrawCursor(cursorX, cursorY, TRUE);
    		}
    		break;

    	case KEY_ENTER:
    		if(!areaSelecting)
    		{
    			if(pos_x == 0)
    			{
					Toggle(&requestEnterKeyProc);
					MotionDetectionPage_UpdatePage(itemY, pos_x);
    			}
				else
				{
					areaSelecting = TRUE;
					MotionDetectionPage_DrawSelectedArea((eChannel_t)(itemY-1));
				}
    		}
    		else
    		{
    			// toggle active/deactive block
    			Read_NvItem_MotionBlock(activeBlocks, (eChannel_t)(itemY-1));
    			activeBlocks[cursorY] ^= (0x0001 << cursorX);
    			active = (activeBlocks[cursorY]>>cursorX) & 0x0001;
    			MotionDetectionPage_DrawBlockMark(cursorX, cursorY, active);
    			Write_NvItem_MotionBlock(activeBlocks, (eChannel_t)(itemY-1));
    		}
			break;

		case KEY_EXIT:
			if(areaSelecting)
			{
				//Set_MotionDetect_ActivatedArea((eChannel_t)(itemY-1));
				MotionDetectionPage_DrawCursor(cursorX, cursorY, FALSE);
				MotionDetectionPage_EraseAllBlockMark();
				Toggle(&areaSelecting);
				cursorX = 0;
				cursorY = 0;
				pos_x = 0;
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MotionDetectionPage_Entry();
				DrawSelectMark(itemY);
			}
			else
			{
				if(requestEnterKeyProc)
				{
					Toggle(&requestEnterKeyProc);
					MotionDetectionPage_UpdatePage(itemY, pos_x);
				}
				else
				{
					InitializeMotionDetect();
					pos_x = 0;
					itemY = MOTION_ITEM_Y_CH1;
					MainMenu_Entry(currentPage);
				}
			}
			break;
	}
}   

//------------------------------------------------------------------
//   DeviceInfo Page Function
//------------------------------------------------------------------

static void DeviceInfoPage_Entry(void)
{
	const sLocationNString_t deviceInfoMenu[DEVICEINFO_ITEM_Y_MAX] =
	{
		{24, LINE0_OFFSET_Y, menuStr_DeviceInfo_Title},
		{20, LINE1_OFFSET_Y, menuStr_DeviceInfo_Version}
	};
	u8 index;
	sVersion_t version;
	u8 strVersion[5] = "00.00";

	currentPage = MENU_PAGE_DEVICE_INFO;
	Erase_AllMenuScreen();

	Read_NvItem_FwVersion(&version);
	Int2Str(version.major, &strVersion[0]);
	Int2Str(version.minor, &strVersion[3]);
	if(strVersion[0] == ASCII_ZERO)
	{
		strVersion[0] = ASCII_SPACE;
	}

	// There is no selectable item
	for(index = 0; index < DEVICEINFO_ITEM_Y_MAX; index++)
	{
		Print_StringWithSelectedMarkSize(
				deviceInfoMenu[index].offset_x,
				deviceInfoMenu[index].offset_y,
				deviceInfoMenu[index].str,
				NULL, 0);
	}
	Print_StringWithSelectedMarkSize(
			deviceInfoMenu[DEVICEINFO_ITEM_Y_FW_VERSION].offset_x + strlen(menuStr_DeviceInfo_Version),
			LINE1_OFFSET_Y,
			strVersion,
			NULL,0);
}

void DeviceInfoPage_KeyHandler(eKeyData_t key)
{
	switch(key)
	{
		case KEY_EXIT :
			MainMenu_Entry(currentPage);
			break;
	}
}   

//------------------------------------------------------------------
//   Main Menu Page
//------------------------------------------------------------------
static void SelectMainMenu(u8 itemY)
{
 	switch(itemY)
 	{
  		case MAINMENU_ITEM_Y_TIME_DATE :
  			TimeDatePage_Entry();
  			break;
 		case MAINMENU_ITEM_Y_CAMERA_TITLE :
 			CameraTitlePage_Entry();
 			break;
  		case MAINMENU_ITEM_Y_AUTOSEQ:
  			AutoSeqPage_Entry();
  			break;
 		case MAINMENU_ITEM_Y_DISPLAY :
 			DisplayPage_Entry();
 			break;
 		case MAINMENU_ITEM_Y_ALARM_REMOCON :
 			AlarmRemoconPage_Entry();
 			break;
 		case MAINMENU_ITEM_Y_MOTION :
 			itemY = 0;
 			MotionDetectionPage_Entry();
 			break;
 		case MAINMENU_ITEM_Y_DEVICE_INFO :
 			DeviceInfoPage_Entry();
 			break;
	}
}

static void MainPage_KeyHandler(eKeyData_t key)
{
	static u8 itemY = MAINMENU_ITEM_Y_TIME_DATE;
	u8 inc_dec = DECREASE;

 	switch(key)
	{
		case KEY_UP :
			inc_dec = INCREASE;
		case KEY_DOWN :
			IncreaseDecreaseCount(MAINMENU_ITEM_Y_MAX - 1, 1, inc_dec, &itemY);
			DrawSelectMark(itemY);
			break;

		case KEY_ENTER :
			SelectMainMenu(itemY);
			requestEnterKeyProc = CLEAR;
	   		break;

		case KEY_EXIT :
			// Update NV
			itemY = MAINMENU_ITEM_Y_TIME_DATE;
			StoreNvDataToStorage();
			Erase_AllMenuScreen();
			ChangeSystemMode(SYSTEM_NORMAL_MODE);
			SetKeyMode(KEY_MODE_LONG);

			MDINOSD_SetBGBoxColor(RGB(255,255,255));
			OSD_DrawBorderLine();
			OSD_RefreshScreen();
			break;
	}
}

//-----------------------------------------------------------------
// 
//-----------------------------------------------------------------
void ChangeSystemMode(u8 mode)
{
	systemMode = mode;
}

u8 GetSystemMode(void)
{
	return systemMode;
}

void Enter_MainMenu(void)
{
	ChangeSystemMode(SYSTEM_SETUP_MODE);
	requestEnterKeyProc = CLEAR;
	SetKeyMode(KEY_MODE_REPEAT);
	Osd_ClearScreen();//OSD_EraseAll();
	MainMenu_Entry(MAINMENU_ITEM_Y_TIME_DATE);
}

//-----------------------------------------------------------------
void DisplayTimeInMenu(void)
{
	if((currentPage == MENU_PAGE_TIME_DATE) && (RTC_GetDisplayTimeStatus() == SET))
	{
		RTC_ChangeDisplayTimeStatus(CLEAR);
	}
}

//-----------------------------------------------------------------
void Menu_KeyProc(eKeyData_t key)
{
	switch(currentPage)
	{
		case MENU_PAGE_MAIN :
			MainPage_KeyHandler(key);
			break;
		case MENU_PAGE_TIME_DATE :
			TimeDatePage_KeyHandler(key);
			break;
		 case MENU_PAGE_CAMERA_TITLE:
			 CameraTitlePage_KeyHandler(key);
			 break;
		 case MENU_PAGE_AUTO_SEQ:
			 AutoSeqPage_KeyHandler(key);
			 break;
		 case MENU_PAGE_DISPLAY:
			 DisplayPage_KeyHandler(key);
			 break;
		 case MENU_PAGE_ALARM_REMOCON:
			 AlaramRemoconPage_KeyHandler(key);
			 break;
		 case MENU_PAGE_MOTION :
			 MotionDetectionPage_KeyHandler(key);
			 break;
		 case MENU_PAGE_DEVICE_INFO :
			 DeviceInfoPage_KeyHandler(key);
			 break;
		 default :
			 break;
	}
}
