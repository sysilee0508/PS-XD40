/*
 * This file contains all character strings for menu.
*/
#ifndef __MENU_STRING_H__
#define __MENU_STRING_H__

// Common
const u8 menuStr_On[] = "ON";
const u8 menuStr_Off[] = "OFF";
const u8 menuStr_Sec[] = "SEC";
const u8 menuStr_Min[] = "MIN";
const u8 menuStr_SecDay[] = "SEC/DAY";
const u8 menuStr_SecMonth[] = "SEC/MONTH";

const u8 menuStr_ArrowL[]= {0x80,0x00};
const u8 menuStr_SelectedMark[] = "O";
const u8 menuStr_UnselectedMart[] = "X";

const u8 menuStr_None[] = "--";
const u8 menuStr_Space1[] = " ";
const u8 menuStr_Space2[] = "  ";
const u8 menuStr_Space3[] = "   ";
const u8 menuStr_Space4[] = "    ";
const u8 menuStr_Space5[] = "     ";
const u8 menuStr_Space6[] = "      ";
const u8 menuStr_Space7[] = "       ";
const u8 menuStr_Space8[] = "        ";
const u8 menuStr_Space9[] = "         ";
const u8 menuStr_Space10[] = "          ";
const u8 menuStr_Space13[] = "             ";

// Options
const u8 menuStr_Left[] = "LEFT";
const u8 menuStr_Center[] = "CENTER";
const u8 menuStr_Right[] = "RIGHT";
const u8 menuStr_TopLeft[] = "TOP-LEFT";
const u8 menuStr_TopCenter[] = "TOP-CENTER";
const u8 menuStr_TopRight[] = "TOP-RIGHT";
const u8 menuStr_BottomLeft[] = "BOTTOM-LEFT";
const u8 menuStr_BottomCenter[] = "BOTTOM-CENTER";
const u8 menuStr_BottomRight[] = "BOTTOM-RIGHT";

const u8 menuStr_Asia[] = "ASIA (yyyy-mm-dd)";
const u8 menuStr_Usa[] =   "USA  (mm-dd-yyyy)";
const u8 menuStr_Euro[] = "EURO (dd-mm-yyyy)";

const u8 menuStr_Skip[] = "SKIP(ON)";
const u8 menuStr_Show[] = "SHOW(OFF)";

const u8 menuStr_Resolution1920X1080_60P[] = "1920 X 1080 60P";
const u8 menuStr_Resolution1920X1080_50P[] = "1920 X 1080 50P";

const u8 menuStr_Alarm[] =   "ALARM";
const u8 menuStr_Remocon[] = "SERIAL";

const u8 menuStr_NO[] = "N.O";
const u8 menuStr_NC[] = "N.C";

const u8 menuStr_Digit[] = "DIGIT";

const u8 menuStr_Baudrate1200[] = "1200";
const u8 menuStr_Baudrate2400[] = "2400";
const u8 menuStr_Baudrate9600[] = "9600";

// Main Menu
const u8 menuStr_MainMenu_Title[] =           "-- MAIN MENU --";
const u8 menuStr_MainMenu_TimeDate[] =        "1. TIME / DATE";
const u8 menuStr_MainMenu_CameraTitle[] =     "2. CAMERA TITLE";
const u8 menuStr_MainMenu_AutoSeq[] =         "3. AUTO SEQUENCE OPTION";
const u8 menuStr_MainMenu_Display[] =         "4. DISPALY OPTION";
const u8 menuStr_MainMenu_Alarm[] =           "5. ALARM / SERIAL OPTION";
const u8 menuStr_MainMenu_MotionDetection[] = "6. MOTION DETECTION OPTION";
const u8 menuStr_MainMenu_DeviceInfo[] =      "7. DEVICE INFO";

// Time/Date
const u8 menuStr_TimeDate_Title[] =          "-- TIME / DATE --";
const u8 menuStr_TimeDate_Time[] =           "1. TIME : ";//hh:mm:ss (HH:MM:SS)";
const u8 menuStr_TimeDate_Date[] =           "2. DATE : ";//yyyy-mm-dd (YYYY-MM-DD)";
const u8 menuStr_TimeDate_TimeDisplay[] =    "3. TIME DISPLAY : ";
const u8 menuStr_TimeDate_DateDisplay[] =    "4. DATE DISPALY : ";
const u8 menuStr_TimeDate_DateFormat[] =     "5. DATE FORMAT : ";
const u8 menuStr_TimeDate_YearFormat[] =     "6. YEAR FORMAT : ";//DIGIT";
const u8 menuStr_TimeDate_TimeCorrection[] = "7. TIME CORRECTION : ";

// Camera Title
const u8 menuStr_CamaraTitle_Title[] =        "-- CAMERAR TITLE --";
const u8 menuStr_CameraTitle_Ch1[] =          "1. CH1 TITLE : ";
const u8 menuStr_CameraTitle_Ch2[] =          "2. CH2 TITLE : ";
const u8 menuStr_CameraTitle_Ch3[] =          "3. CH3 TITLE : ";
const u8 menuStr_CameraTitle_Ch4[] =          "4. CH4 TITLE : ";
const u8 menuStr_CameraTitle_TitleDisplay[] = "5. TITLE DISPLAY : ";
const u8 menuStr_CameraTitle_Position[] =     "6. TITLE POSITION : ";

// Auto Seq
const u8 menuStr_AutoSeq_Title[]       = "-- AUTO SEQUENCE -- ";
const u8 menuStr_AutoSeq_DisplayTime_Ch1[] = "1. CH1 : ";
const u8 menuStr_AutoSeq_DisplayTime_Ch2[] = "2. CH2 : ";
const u8 menuStr_AutoSeq_DisplayTime_Ch3[] = "3. CH3 : ";
const u8 menuStr_AutoSeq_DisplayTime_Ch4[] = "4. CH4 : ";
const u8 menuStr_AutoSeq_NoVideoSkip[] =     "5. NO VIDEO CHANNEL SKIP : "; // SKIP(ON) / SHOW(OFF)

// Display
const u8 menuStr_Display_Title[] =      "-- DISPLAY OPTION --";
const u8 menuStr_Display_Resolution[] = "1. RESOLUTION : ";
const u8 menuStr_Display_OsdDisplay[] = "2. OSD DISPLAY : ";
const u8 menuStr_Display_BorderLine[] = "3. BORDER LINE DISPLAY : ";

// Alarm
const u8 menuStr_Alarm_Title[] =               "-- ALARM / SERIAL OPTION --";
const u8 menuStr_Alarm_AlarmRemocon[] =        "1. ALARM/SERIAL SELECT : "; //ALARM(default) / REMOCON
const u8 menuStr_Alarm_Channel1[] =            "2. CH1 ALARM :     / VIDEO LOSS : ";
const u8 menuStr_Alarm_Channel2[] =            "3. CH2 ALARM :     / VIDEO LOSS : ";
const u8 menuStr_Alarm_Channel3[] =            "4. CH3 ALARM :     / VIDEO LOSS : ";
const u8 menuStr_Alarm_Channel4[] =            "5. CH4 ALARM :     / VIDEO LOSS : ";
const u8 menuStr_Alarm_AlarmOutTime[] =        "6. ALARM OUT TIME : ";
const u8 menuStr_Alarm_AlarmBuzzerTime[] =     "7. ALARM BUZZER TIME : ";
const u8 menuStr_Alarm_VideoLossBuzzerTime[] = "8. VIDEO LOSS BUZZER TIME : ";
const u8 menuStr_Alarm_RemoconId[] =           "9. SERIAL ID : ";
const u8 menuStr_Alarm_BaudRate[] =            "10. SERIAL BAUDRATE(bps) : ";

// Motion Detection
const u8 menuStr_Motion_Title[] =       "-- MOTION DETECTION OPTION --";
const u8 menuStr_Motion_Channel1[] =    "1. CH 1:     / AREA";
const u8 menuStr_Motion_Channel2[] =    "2. CH 2:     / AREA";
const u8 menuStr_Motion_Channel3[] =    "3. CH 3:     / AREA";
const u8 menuStr_Motion_Channel4[] =    "4. CH 4:     / AREA";
const u8 menuStr_Motion_Sensitivity[] = "5. MOTION SENSITIVITY (1~99) : ";
const u8 menuStr_Motion_CallMode[] =    "6. MOTION MODE : ";

// Device Info
const u8 menuStr_DeviceInfo_Title[] =   "-- DEVICE INFO --";
const u8 menuStr_DeviceInfo_Version[] = "1. F/W VERSION : v";

#endif
