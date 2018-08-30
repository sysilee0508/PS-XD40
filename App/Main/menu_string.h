#ifndef __MENU_STRING_H__
#define __MENU_STRING_H__

const u8 WHITE_SPACE[]		=" ";
const u8 strNULL[10] = {NULL,};
const u8 ArrowL[]				= {0x80,0x00};
const u8 strTimeFormatHMS[]		= "(HH:MM:SS)";
const u8 strDateFormatYMD[]		= "(YY/MM/DD)";	//asia
const u8 strDateFormatMDY[]		= "(MM/DD/YY)";	//us
const u8 strDateFormatDMY[]		= "(DD/MM/YY)"; //euro

// Main Menu Items
const BYTE MAIN_MENU[]			= "- MAIN MENU -";
const BYTE TIME_DATE[]			= "1. TIME/DATE";
const BYTE CAMERA_TITLE[]		= "2. CAMERA TITLE";
const BYTE AUTO_SEQUENCE[]		= "3. AUTO SEQUENCE";
const BYTE DISPLAY[]			= "4. DISPLAY";
const BYTE ALARM_SET[]			= "5. ALARM SET";
const BYTE MOTION_DETECTION[]	= "6. MOTION DETECTION";
const BYTE MISCELLANEOUS[]		= "7. MISCELLANEOUS";
// Time/Date Menu Items
const u8 cTIME_DATE1[]			= "- TIME/DATE -";
const u8 cTIME1[]				= "1. TIME :";
const u8 cDATE1[]				= "2. DATE :";
const u8 cTIME_DISPLAY1[]		= "3. TIME DISPLAY  :";
const u8 cDATE_FORMAT_1[]		= "4. DATE DISPLAY  :";
const u8 cTIME_CORRECT1[]		= "5. DATE FORMAT   :";
const u8 cTIME_OSD_SIZE1[]		= "6. YEAR FORMAT   :";
const u8 cTIME_OSD_LOC1[]		= "7. TIME CORRECT  :";
// Date Format Selection
const u8 cASIA[]				= "ASIA";
const u8 cUS[]					= "U.S ";
const u8 cEURO[]				= "EURO";



const BYTE cOFF6[] = "OFF  ";
const BYTE cSEC6[] = "SEC";
const BYTE cMIN6[] = "MIN";

#endif
