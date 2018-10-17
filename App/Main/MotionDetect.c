//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
typedef struct
{
	BOOL motiondetect_enabled;
	WORD activated_block[12];
	BYTE temporal_sensitivity;
	BOOL motion_detected;
} sMotionDetectInfo_t;
//=============================================================================
//  Static Variable Declaration
//=============================================================================
static sMotionDetectInfo_t motiondetectionInfo[NUM_OF_CHANNEL] = 
{
	{FALSE, {0, }, 0x60, FALSE },
	{FALSE, {0, }, 0x60, FALSE },
	{FALSE, {0, }, 0x60, FALSE },
	{FALSE, {0, }, 0x60, FALSE }
};
//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================
void Read_MotionDetect_OnOff(void)
{
	unsigned char channel_num;
	
	for(channel_num = 0; channel_num < 4; channel_num++)
	{
		Read_NvItem_MotionDetectOnOff(&motiondetectionInfo[channel_num].motiondetect_enabled, channel_num);
	}
}

void Write_MotionDetect_OnOff(BYTE ch, BOOL enabled)
{
	motiondetectionInfo[ch].motiondetect_enabled = enabled;
	Write_NvItem_MotionDetectOnOff(motiondetectionInfo[ch].motiondetect_enabled, ch);
}

BOOL Get_MotionDetect_OnOff(BYTE ch)
{
	return motiondetectionInfo[ch].motiondetect_enabled;
}

void Set_MotionDetect_OnOff(BYTE ch, BOOL enabled)
{
	motiondetectionInfo[ch].motiondetect_enabled = enabled;
}

void Read_MotionDetect_Sensitivity(BYTE ch)
{
	Read_NvItem_MotionDetectOnOff(&motiondetectionInfo[ch].temporal_sensitivity);
}

void Write_MotionDetect_Sensitivity(BYTE ch, BYTE value)
{
	motiondetectionInfo[ch].temporal_sensitivity = value;
	Write_NvItem_MotionDetectOnOff(motiondetectionInfo[ch].temporal_sensitivity);
}

BYTE Get_MotionDetect_Sensitivity(BYTE ch)
{
	return motiondetectionInfo[ch].temporal_sensitivity;
}

void Set_MotionDetect_Sensitivity(BYTE ch, BYTE value)
{
	
}

void MotionDetectCheck(void)
{
	unsigned char vMotion;
	unsigned char channel_num;
	vMotion = NVP6158_MotionDetect_Check();

	for(channel_num = 0; channel_num < 4; channel_num++)
	{
		motiondetectionInfo[channel_num].motion_detected = vMotion >> channel_num;
	}
	if (motiondetectionInfo[0].motion_detected)
	{
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
	}
	if (motiondetectionInfo[1].motion_detected)
	{
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
	}
	if (motiondetectionInfo[2].motion_detected)
	{
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
	}
	if (motiondetectionInfo[3].motion_detected)
	{
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_LOW;
	}
}
