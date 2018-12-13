/*
 * This file contains all character strings of OSD.
*/
#ifndef __OSD_STRING_H__
#define __OSD_STRING_H__

#define ASCII_SPACE					0x20
#define ASCII_ZERO					0x30
#define ASCII_UNDERBAR				0x5F

const u8 osdStr_Space1[] = " ";
const u8 osdStr_Space2[] = "  ";
const u8 osdStr_Space3[] = "   ";
const u8 osdStr_Space4[] = "    ";
const u8 osdStr_Space5[] = "     ";
const u8 osdStr_Space6[] = "      ";
const u8 osdStr_Space7[] = "       ";
const u8 osdStr_Space8[] = "        ";
const u8 osdStr_Space9[] = "         ";
const u8 osdStr_Space10[] = "          ";
const u8 osdStr_Space12[] = "            ";
//-----------------------------------------------------------------------------
const u8 osdStr_AUTO[] = "AUTO";
const u8 osdStr_NoVideoFull[]= "VIDEO LOSS";
const u8 osdStr_NoVideo[] = "L";
const u8 osdStr_AlarmFull[] = "ALARM";
const u8 osdStr_Alarm[] = "A";
const u8 osdStr_MotionFull[] = "MOTION";
const u8 osdStr_Motion[] = "M";
const u8 osdStr_FreezeFull[] = "FREEZE";
const u8 osdStr_Freeze[] = "F";
//-----------------------------------------------------------------------------
const u8 osdStr_JAN[] = "JAN";
const u8 osdStr_FEB[] = "FEB";
const u8 osdStr_MAR[] = "MAR";
const u8 osdStr_APR[] = "APR";
const u8 osdStr_MAY[] = "MAY";
const u8 osdStr_JUN[] = "JUN";
const u8 osdStr_JUL[] = "JUL";
const u8 osdStr_AUG[] = "AUG";
const u8 osdStr_SEP[] = "SEP";
const u8 osdStr_OCT[] = "OCT";
const u8 osdStr_NOV[] = "NOV";
const u8 osdStr_DEC[] = "DEC";

const u8* pStrMonthName[12] =
{
		osdStr_JAN,
		osdStr_FEB,
		osdStr_MAR,
		osdStr_APR,
		osdStr_MAY,
		osdStr_JUN,
		osdStr_JUL,
		osdStr_AUG,
		osdStr_SEP,
		osdStr_OCT,
		osdStr_NOV,
		osdStr_DEC
};


#endif
