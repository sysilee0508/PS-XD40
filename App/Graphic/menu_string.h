/*
 * This file contains all character strings for menu.
*/
#ifndef __MENU_STRING_H__
#define __MENU_STRING_H__

// Common
const BYTE menuStr_On[] = "ON";
const BYTE menuStr_Off[] = "OFF";
const BYTE menuStr_Sec[] = "SEC";
const BYTE menuStr_Min[] = "MIN";
const BYTE menuStr_Day[] = "DAY";
const BYTE menuStr_Month[] = "MONTH";

const BYTE menuStr_ArrowL[]= {0x80,0x00};

const BYTE menuStr_Space1[] = " ";
const BYTE menuStr_Space2[] = "  ";
const BYTE menuStr_Space3[] = "   ";
const BYTE menuStr_Space4[] = "    ";
const BYTE menuStr_Space5[] = "     ";
const BYTE menuStr_Space6[] = "      ";
const BYTE menuStr_Space7[] = "       ";
const BYTE menuStr_Space8[] = "        ";
const BYTE menuStr_Space9[] = "         ";
const BYTE menuStr_Space10[] = "          ";
const BYTE menuStr_Space13[] = "             ";

// Options
const BYTE menuStr_Left[] = "LEFT";
const BYTE menuStr_Center[] = "CENTER";
const BYTE menuStr_Right[] = "RIGHT";
const BYTE menuStr_TopLeft[] = "TOP-LEFT";
const BYTE menuStr_TopCenter[] = "TOP-CENTER";
const BYTE menuStr_TopRight[] = "TOP-RIGHT";
const BYTE menuStr_BottomLeft[] = "BOTTOM-LEFT";
const BYTE menuStr_BottomCenter[] = "BOTTOM-CENTER";
const BYTE menuStr_BottomRight[] = "BOTTOM-RIGHT";

const BYTE menuStr_Asia[] = "ASIA (yyyy-mm-dd)";
const BYTE menuStr_Usa[] = "USA (mm-dd-yyyy)";
const BYTE menuStr_Euro[] = "EURO (dd-mm-yyyy)";

const BYTE menuStr_Skip[] = "SKIP(ON)";
const BYTE menuStr_Show[] = "SHOW(OFF)";

const BYTE menuStr_Resolution1920X1080_60P[] = "1920 X 1080 60P";
const BYTE menuStr_Resolution1920X1080_50P[] = "1920 X 1080 50P";

const BYTE menuStr_Alarm[] =   "ALARM";
const BYTE menuStr_Remocon[] = "REMOCON";

const BYTE menuStr_NO[] = "N.O";
const BYTE menuStr_NC[] = "N.C";

// Main Menu
const BYTE menuStr_MainMenu_Title[] =           "-- MAIN MENU --";
const BYTE menuStr_MainMenu_TimeDate[] =        "1. TIME / DATE";
const BYTE menuStr_MainMenu_CameraTitle[] =     "2. CAMERA TITLE";
const BYTE menuStr_MainMenu_AutoSeq[] =         "3. AUTO SEQUENCE OPTION";
const BYTE menuStr_MainMenu_Display[] =         "4. DISPALY OPTION";
const BYTE menuStr_MainMenu_Alarm[] =           "5. ALARM / REMOCON OPTION";
const BYTE menuStr_MainMenu_MotionDetection[] = "6. MOTION DETECTION OPTION";
const BYTE menuStr_MainMenu_DeviceInfo[] =      "7. DEVICE INFO";

// Time/Date
const BYTE menuStr_TimeDate_Title[] =          "-- TIME / DATE --";
const BYTE menuStr_TimeDate_Time[] =           "1. TIME : hh:mm:ss (HH:MM:SS)";
const BYTE menuStr_TimeDate_Date[] =           "2. DATE : yyyy-mm-dd (YYYY-MM-DD)";
const BYTE menuStr_TimeDate_TimeDisplay[] =    "3. TIME DISPLAY : ";
const BYTE menuStr_TimeDate_DateDisplay[] =    "4. DATE DISPALY : ";
const BYTE menuStr_TimeDate_DateFormat[] =     "5. DATE FORMAT : ";
const BYTE menuStr_TimeDate_YearFormat[] =     "6. YEAR FORMAT : X DIGIT";
const BYTE menuStr_TimeDate_TimeCorrection[] = "7. TIME CORRECTION : +00 SEC/DAY";

// Camera Title
const BYTE menuStr_CamaraTitle_Title[] =        "-- CAMERAR TITLE --";
const BYTE menuStr_CameraTitle_Ch1[] =          "1. CH1 TITLE : ";
const BYTE menuStr_CameraTitle_Ch2[] =          "2. CH2 TITLE : ";
const BYTE menuStr_CameraTitle_Ch3[] =          "3. CH3 TITLE : ";
const BYTE menuStr_CameraTitle_Ch4[] =          "4. CH4 TITLE : ";
const BYTE menuStr_CameraTitle_TitleDisplay[] = "5. TITLE DISPLAY : ";
const BYTE menuStr_CameraTitle_Position[] =     "6. TITLE POSITION : ";

// Auto Seq
const BYTE menuStr_AutoSeq_Title[]       = "-- AUTO SEQUENCE -- ";
const BYTE menuStr_AutoSeq_DisplayTime_Ch1[] = "1. CH1 : ";
const BYTE menuStr_AutoSeq_DisplayTime_Ch2[] = "2. CH2 : ";
const BYTE menuStr_AutoSeq_DisplayTime_Ch3[] = "3. CH3 : ";
const BYTE menuStr_AutoSeq_DisplayTime_Ch4[] = "4. CH4 : ";
const BYTE menuStr_AutoSeq_NoVideoSkip[] =     "5. NO VIDEO CHANNEL SKIP : "; // SKIP(ON) / SHOW(OFF)

// Display
const BYTE menuStr_Display_Title[] =      "-- DISPLAY OPTION --";
const BYTE menuStr_Display_Resolution[] = "1. RESOLUTION : ";
const BYTE menuStr_Display_OsdDisplay[] = "2. OSD DISPLAY : ";
const BYTE menuStr_Display_BorderLine[] = "3. BORDER LINE DISPLAY : ";

// Alarm
const BYTE menuStr_Alarm_Title[] =               "-- ALARM / REMOCON OPTION --";
const BYTE menuStr_Alarm_AlarmRemocon[] =        "1. ALARM/REMOCON SELECT : "; //ALARM(default) / REMOCON
const BYTE menuStr_Alarm_Channel1[] =            "2. CH1 ALARM :     / VIDEO LOSS : ";
const BYTE menuStr_Alarm_Channel2[] =            "3. CH2 ALARM :     / VIDEO LOSS : ";
const BYTE menuStr_Alarm_Channel3[] =            "4. CH3 ALARM :     / VIDEO LOSS : ";
const BYTE menuStr_Alarm_Channel4[] =            "5. CH4 ALARM :     / VIDEO LOSS : ";
const BYTE menuStr_Alarm_AlarmOutTime[] =        "6. ALARM OUT TIME :";
const BYTE menuStr_Alarm_AlarmBuzzerTime[] =     "7. ALARM BUZZER TIME : ";
const BYTE menuStr_Alarm_VideoLossBuzzerTime[] = "8. VIDEO LOSS BUZZER TIME : ";
const BYTE menuStr_Alarm_RemoconId[] =           "9. REMOCON ID : ";

// Motion Detection
const BYTE menuStr_Motion_Title[] =       "-- MOTION DETECTION OPTION --";
const BYTE menuStr_Motion_Channel1[] =    "1. CH 1:     / AREA";
const BYTE menuStr_Motion_Channel2[] =    "2. CH 2:     / AREA";
const BYTE menuStr_Motion_Channel3[] =    "3. CH 3:     / AREA";
const BYTE menuStr_Motion_Channel4[] =    "4. CH 4:     / AREA";
const BYTE menuStr_Motion_Sensitivity[] = "5. MOTION SENSITIVITY : ";
const BYTE menuStr_Motion_CallMode[] =    "6. MOTION MODE : ";
//const BYTE menuStr_Motion_Saving[] = "SAVING AREA...";

// Miscellaneous
const BYTE menuStr_DeviceInfo_Title[] =   "-- DEVICE INFO --";
const BYTE menuStr_DeviceInfo_Version[] = "1. F/W VERSION : v";
//const BYTE menuStr_Miscellaneous_
//const BYTE menuStr_Miscellaneous_

#endif
