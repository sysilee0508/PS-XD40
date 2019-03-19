/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Audio information
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _RAPTOR3_AUDIO_H_
#define _RAPTOR3_AUDIO_H_

#include "video_decoder.h"

typedef struct NC_VD_AFE_AUDIO_STR
{
	unsigned char B0_0x02;
}NC_VD_AFE_AUDIO_STR;

typedef struct NC_VD_AUDIO_STR
{
	unsigned char ch;
	unsigned char devnum;

	NC_VD_AFE_AUDIO_STR AFE_SET;
	unsigned char Clk_Sel;
	unsigned char Bit_Rate;
	unsigned char AIGain_01;
	unsigned char AIGain_02;
	unsigned char AIGain_03;
	unsigned char AIGain_04;
	unsigned char MIGain_01;
	unsigned char AIGain_05;
	unsigned char AIGain_06;
	unsigned char AIGain_07;
	unsigned char AIGain_08;
	unsigned char AOGain;
	unsigned char MIXOutSel;
	unsigned char MUX_Setting;
	unsigned char DELAY_Setting;
	unsigned char LiveMute;

	unsigned char FilterEN;
	unsigned char AU_DClevel;
	unsigned char AU_Sysclk;
}NC_VD_AUDIO_STR;

typedef struct _NC_VD_AOC_ATTR
{
	unsigned char ch;
	unsigned char fmt;
	unsigned char sample_rate;
}NC_VD_AOC_ATTR;


void RAPTOR3_Audio_Default_set( void );



#endif /* _RAPTOR3_AUDIO_H_ */
