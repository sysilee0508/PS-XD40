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
const u8 menuStr_Space30[] = "                              ";

// Options
const u8 menuStr_Asia[] = "ASIA (yyyy-mm-dd)";
const u8 menuStr_Usa[] =   "USA  (mm-dd-yyyy)";
const u8 menuStr_Euro[] = "EURO (dd-mm-yyyy)";

const u8 menuStr_Skip[] = "SKIP(ON)";
const u8 menuStr_Show[] = "INCLUDE(OFF)";

const u8 menuStr_Resolution1920X1080_60P[] = "1920 X 1080 60(50)P";
const u8 menuStr_Resolution1920X1080_30P[] = "1920 X 1080 30(25)P";

const u8 menuStr_DisplayMode_2SplitHScale_A[] = "11. 2Split H-Scale Ch1&Ch2";
const u8 menuStr_DisplayMode_2SplitHCrop_A[] = "12. 2Split H-Crop Ch1&Ch2";
const u8 menuStr_DisplayMode_2SplitVScale_A[] = "13. 2Split V-Scale Ch1&Ch2";
const u8 menuStr_DisplayMode_2SplitVCrop_A[] = "14. 2Split V-Crop Ch1&Ch2";
const u8 menuStr_DisplayMode_2SplitHScale_B[] = "15. 2Split H-Scale Ch3&Ch4";
const u8 menuStr_DisplayMode_2SplitHCrop_B[] = "16. 2Split H-Crop Ch3&Ch4";
const u8 menuStr_DisplayMode_2SplitVScale_B[] = "17. 2Split V-Scale Ch3&Ch4";
const u8 menuStr_DisplayMode_2SplitVCrop_B[] = "18. 2Split V-Crop Ch3&Ch4";

const u8 menuStr_DisplayMode_3Split_RScale[] = "19. 3Split Left Scale";
const u8 menuStr_DisplayMode_3Split_RCrop[] = "20. 3Split Left Crop";
const u8 menuStr_DisplayMode_3Split_LScale[] = "21. 3Split Right Scale";
const u8 menuStr_DisplayMode_3Split_LCrop[] = "22. 3Split Right Crop";
const u8 menuStr_DisplayMode_3Split_DScale[] = "23. 3Split Top Scale";
const u8 menuStr_DisplayMode_3Split_DCrop[] = "24. 3Split Top Crop";
const u8 menuStr_DisplayMode_3Split_UScale[] = "25. 3Split Bottom Scale";
const u8 menuStr_DisplayMode_3Split_UCrop[] = "26. 3Split Bottom Crop";

const u8 menuStr_DisplayMode_4Split_Quad[] = "27. QUAD";
const u8 menuStr_DisplayMode_4Split_RScale[] = "1. 4Split Left Scale";
const u8 menuStr_DisplayMode_4Split_RCrop[] = "2. 4Split Left Crop";
const u8 menuStr_DisplayMode_4Split_LScale[] = "3. 4Split Right Scale";
const u8 menuStr_DisplayMode_4Split_LCrop[] = "4. 4Split Right Crop";
const u8 menuStr_DisplayMode_4Split_DScale[] = "5. 4Split Top Scale";
const u8 menuStr_DisplayMode_4Split_DCrop[] = "6. 4Split Top Crop";
const u8 menuStr_DisplayMode_4Split_UScale[] = "7. 4Split Bottom Scale";
const u8 menuStr_DisplayMode_4Split_UCrop[] = "8. 4Split Bottom Crop";
const u8 menuStr_DisplayMode_4Split_H[] = "9. 4Split-H";
const u8 menuStr_DisplayMode_4Split_X[] = "10. 4Split-X";
#if 0
const u8 menuStr_DisplayMode_PIP_A2[] = "PIP Right-Top Ch2";
const u8 menuStr_DisplayMode_PIP_A3[] = "PIP Right-Top Ch3";
const u8 menuStr_DisplayMode_PIP_A4[] = "PIP Right-Top Ch4";
const u8 menuStr_DisplayMode_PIP_B2[] = "PIP Left-Bottom Ch2";
const u8 menuStr_DisplayMode_PIP_B3[] = "PIP Left-Bottom Ch3";
const u8 menuStr_DisplayMode_PIP_B4[] = "PIP Left-Bottom Ch4";
const u8 menuStr_DisplayMode_PIP_C2[] = "PIP Right-Bottom Ch2";
const u8 menuStr_DisplayMode_PIP_C3[] = "PIP Right-Bottom Ch3";
const u8 menuStr_DisplayMode_PIP_C4[] = "PIP Right-Bottom Ch4";
const u8 menuStr_DisplayMode_PIP_D2[] = "PIP Left-Top Ch2";
const u8 menuStr_DisplayMode_PIP_D3[] = "PIP Left-Top Ch3";
const u8 menuStr_DisplayMode_PIP_D4[] = "PIP Left-Top Ch4";
#endif
const u8* pDisplayMode_Name[DISPLAY_MODE_MAX] = 
{
	NULL,
	NULL, 
	NULL,
	NULL,
	menuStr_DisplayMode_2SplitHScale_A,
	menuStr_DisplayMode_2SplitHCrop_A,
	menuStr_DisplayMode_2SplitVScale_A,
	menuStr_DisplayMode_2SplitVCrop_A,
	menuStr_DisplayMode_2SplitHScale_B,	
	menuStr_DisplayMode_2SplitHCrop_B,
	menuStr_DisplayMode_2SplitVScale_B,
	menuStr_DisplayMode_2SplitVCrop_B,
	menuStr_DisplayMode_3Split_RScale,
	menuStr_DisplayMode_3Split_RCrop,
	menuStr_DisplayMode_3Split_LScale,
	menuStr_DisplayMode_3Split_LCrop,
	menuStr_DisplayMode_3Split_DScale,
	menuStr_DisplayMode_3Split_DCrop,
	menuStr_DisplayMode_3Split_UScale,
	menuStr_DisplayMode_3Split_UCrop,
	menuStr_DisplayMode_4Split_Quad,
	menuStr_DisplayMode_4Split_RScale,
	menuStr_DisplayMode_4Split_RCrop,
	menuStr_DisplayMode_4Split_LScale,
	menuStr_DisplayMode_4Split_LCrop,
	menuStr_DisplayMode_4Split_DScale,
	menuStr_DisplayMode_4Split_DCrop,
	menuStr_DisplayMode_4Split_UScale,
	menuStr_DisplayMode_4Split_UCrop,
	menuStr_DisplayMode_4Split_H,
	menuStr_DisplayMode_4Split_X
#if 0	
	menuStr_DisplayMode_PIP_A2,
	menuStr_DisplayMode_PIP_A3,
	menuStr_DisplayMode_PIP_A4,
	menuStr_DisplayMode_PIP_B2,
	menuStr_DisplayMode_PIP_B3,
	menuStr_DisplayMode_PIP_B4,
	menuStr_DisplayMode_PIP_C2,
	menuStr_DisplayMode_PIP_C3,
	menuStr_DisplayMode_PIP_C4,
	menuStr_DisplayMode_PIP_D2,
	menuStr_DisplayMode_PIP_D3,
	menuStr_DisplayMode_PIP_D4
#endif	
};

const u8 menuStr_AuxVGA[] = "VGA";
const u8 menuStr_AuxCVBS[] = "CVBS/TVI";

const u8 menuStr_Alarm[] =   "ALARM";
const u8 menuStr_Remocon[] = "REMOTE";

const u8 menuStr_NO[] = "N.O";
const u8 menuStr_NC[] = "N.C";

const u8 menuStr_Digit[] = "DIGIT";

const u8 menuStr_Baudrate1200[] = "1200";
const u8 menuStr_Baudrate2400[] = "2400";
const u8 menuStr_Baudrate9600[] = "9600";

const u8 menuStr_SelectArea[] = "SELECT AREA";

const u8 menuStr_PIP_Position_LT[] = "Left-Top";
const u8 menuStr_PIP_Position_RT[] = "Right-Top";
const u8 menuStr_PIP_Position_LB[] = "Left-Bottom";
const u8 menuStr_PIP_Position_RB[] = "Right-Bottom";

const u8 menuStr_Full[] = "FULL";
const u8 menuStr_Pip[] = "PIP";

// Main Menu
const u8 menuStr_MainMenu_Title[] =           "-- MAIN MENU --";
const u8 menuStr_MainMenu_TimeDate[] =        "1. TIME / DATE";
const u8 menuStr_MainMenu_CameraTitle[] =     "2. CAMERA TITLE";
const u8 menuStr_MainMenu_AutoSeq[] =         "3. AUTO SEQUENCE OPTION";
const u8 menuStr_MainMenu_Display[] =         "4. DISPALY OPTION";
const u8 menuStr_MainMenu_MotionDetection[] = "5. MOTION DETECTION OPTION";
const u8 menuStr_MainMenu_Alarm[] =           "6. ALARM / REMOTE OPTION";
const u8 menuStr_MainMenu_FW_Version[] =      "** F/W VERSION : v";

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
const u8 menuStr_CameraTitle_VideoLossBuzzerTime[] = "6. VIDEO LOSS BUZZER TIME : ";

// Auto Seq
const u8 menuStr_AutoSeq_Title[]       = "-- AUTO SEQUENCE -- ";
const u8 menuStr_AutoSeq_DisplayTime_Ch1[] = "1. CH1 : ";
const u8 menuStr_AutoSeq_DisplayTime_Ch2[] = "2. CH2 : ";
const u8 menuStr_AutoSeq_DisplayTime_Ch3[] = "3. CH3 : ";
const u8 menuStr_AutoSeq_DisplayTime_Ch4[] = "4. CH4 : ";
const u8 menuStr_AutoSeq_NoVideoSkip[] =     "5. NO VIDEO CHANNEL SKIP : "; // SKIP(ON) / SHOW(OFF)
const u8 menuStr_AutoSeq_Mode[] = "6. SEQUENCE MODE : ";
const u8 menuStr_AutoSeq_PipPosition[] = "7. PIP POSITION : ";

// Display
const u8 menuStr_Display_Title[] =      "-- DISPLAY OPTION --";
const u8 menuStr_Display_Resolution[] = "1. RESOLUTION : ";
const u8 menuStr_Display_OsdDisplay[] = "2. OSD DISPLAY : ";
const u8 menuStr_Display_BorderLine[] = "3. BORDER LINE DISPLAY : ";
const u8 menuStr_Display_SplitMode[] = "4. SPLIT MODE : ";
const u8 menuStr_Display_VideoOutFormat[] = "5. AUX VIDEO OUT : ";

// Alarm
const u8 menuStr_Alarm_Title[] =               "-- ALARM / REMOTE OPTION --";
const u8 menuStr_Alarm_AlarmRemocon[] =        "1. ALARM/REMOTE SELECT : "; //ALARM(default) / REMOCON
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
const u8 menuStr_Motion_Channel1[] =    "1. CH 1:     / ";
const u8 menuStr_Motion_Channel2[] =    "2. CH 2:     / ";
const u8 menuStr_Motion_Channel3[] =    "3. CH 3:     / ";
const u8 menuStr_Motion_Channel4[] =    "4. CH 4:     / ";
const u8 menuStr_Motion_Sensitivity[] = "5. MOTION SENSITIVITY (1~99) : ";
const u8 menuStr_Motion_Indication[] =  "6. MOTION INDICATION : ";


#endif
