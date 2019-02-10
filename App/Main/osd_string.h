/*
 * This file contains all character strings of OSD.
*/
#ifndef __OSD_STRING_H__
#define __OSD_STRING_H__

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
const u8 osdSTr_Space20[] = "                    ";
//-----------------------------------------------------------------------------

const u8 osdStr_Title_CAM1[] = "CAM 1";
const u8 osdStr_Title_CAM2[] = "CAM 2";
const u8 osdStr_Title_SplitA[] = "TWO-A";
const u8 osdStr_Title_SplitB[] = "TWO-B";
const u8 osdStr_Title_SplitC[] = "TWO-C";
const u8 osdStr_Title_SplitD[] = "TWO-D";
const u8 osdStr_Title_SplitE[] = "TWO-E";
const u8 osdStr_Title_PipA[] = "PIP-A";
const u8 osdStr_Title_PipB[] = "PIP-B";
const u8 osdStr_Title_PipC[] = "PIP-C";
const u8 osdStr_Title_PipD[] = "PIP-D";
const u8* osdStr_Title[DISPLAY_MODE_MAX] =
{
		osdStr_Title_CAM1,		//DISPLAY_MODE_FULL_CH1,
		osdStr_Title_CAM2,		//DISPLAY_MODE_FULL_CH2,
		osdStr_Title_SplitA,	//DISPLAY_MODE_SPLIT_A,
		osdStr_Title_SplitB,	//DISPLAY_MODE_SPLIT_B,
		osdStr_Title_SplitC,	//DISPLAY_MODE_SPLIT_C,
		osdStr_Title_SplitD,	//DISPLAY_MODE_SPLIT_D,
		osdStr_Title_SplitE,	//DISPLAY_MODE_SPLIT_E,
		osdStr_Title_PipA,		//DISPLAY_MODE_PIP_A,
		osdStr_Title_PipB,		//DISPLAY_MODE_PIP_B,
		osdStr_Title_PipC,		//DISPLAY_MODE_PIP_C,
		osdStr_Title_PipD		//DISPLAY_MODE_PIP_D,
};

//-----------------------------------------------------------------------------
// format display postion (full screen): x=840 y=450,486
const u8 osdStr_Format_In_CVBS_NTSC[] =   "Input  : CVBS NTSC";
const u8 osdStr_Format_In_CVBS_PAL[] =    "Input  : CVBS PAL";
const u8 osdStr_Format_In_AHD_1080p30[] = "Input  : AHD 1080p30";
const u8 osdStr_Format_In_AHD_1080p25[] = "Input  : AHD 1080p25";
const u8 osdStr_Format_In_TVI_1080p30[] = "Input  : TVI 1080p30";
const u8 osdStr_Format_In_TVI_1080p25[] = "Input  : TVI 1080p25";
const u8 osdStr_Format_In_CVI_1080p30[] = "Input  : CVI 1080p30";
const u8 osdStr_Format_In_CVI_1080p25[] = "Input  : CVI 1080p25";
const u8 osdStr_Format_In_AHD_720p30[] =  "Input  : AHD 720p30";
const u8 osdStr_Format_In_AHD_720p25[] =  "Input  : AHD 720p25";
const u8 osdStr_Format_In_TVI_720p30[] =  "Input  : TVI 720p30";
const u8 osdStr_Format_In_TVI_720p25[] =  "Input  : TVI 720p25";
const u8 osdStr_Format_In_CVI_720p30[] =  "Input  : CVI 720p30";
const u8 osdStr_Format_In_CVI_720p25[] =  "Input  : CVI 720p25";
const u8 osdStr_Format_Out_1080p60[] =    "Output : 1080p60";
const u8 osdStr_Format_Out_1080p50[] =    "Output : 1080p50";

const u8 osdStr_NoVideo[] = "NO VIDEO";

//const u8 osdStr_JAN[] = "JAN";
//const u8 osdStr_FEB[] = "FEB";
//const u8 osdStr_MAR[] = "MAR";
//const u8 osdStr_APR[] = "APR";
//const u8 osdStr_MAY[] = "MAY";
//const u8 osdStr_JUN[] = "JUN";
//const u8 osdStr_JUL[] = "JUL";
//const u8 osdStr_AUG[] = "AUG";
//const u8 osdStr_SEP[] = "SEP";
//const u8 osdStr_OCT[] = "OCT";
//const u8 osdStr_NOV[] = "NOV";
//const u8 osdStr_DEC[] = "DEC";
//
//const u8* pStrMonthName[12] =
//{
//		osdStr_JAN,
//		osdStr_FEB,
//		osdStr_MAR,
//		osdStr_APR,
//		osdStr_MAY,
//		osdStr_JUN,
//		osdStr_JUL,
//		osdStr_AUG,
//		osdStr_SEP,
//		osdStr_OCT,
//		osdStr_NOV,
//		osdStr_DEC
//};


#endif
