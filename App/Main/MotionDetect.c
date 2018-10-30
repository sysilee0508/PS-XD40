//=============================================================================
//  Header Declaration
//=============================================================================
#include "common.h"
#include "NVP6158.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
typedef struct
{
	WORD previousActivatedBlock[ROWS_OF_BLOCKS];
	BOOL motion_detected;
} sMotionDetectInfo_t;

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static sMotionDetectInfo_t motiondetectionInfo[NUM_OF_CHANNEL] = 
{
	{{0, }, FALSE },
	{{0, }, FALSE },
	{{0, }, FALSE },
	{{0, }, FALSE }
};

static BYTE motionDetectionSensitivity = 0x60;
//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================

BOOL Get_MotionDetect_OnOff(eChannel_t channel)
{
	BOOL motionOn;
	Read_NvItem_MotionDetectOnOff(&motionOn, channel);
	return motionOn;
}

void Set_MotionDetect_OnOff(eChannel_t channel, BOOL enabled)
{
	motion_mode motion;
	BOOL motionOn;

	Read_NvItem_MotionDetectOnOff(&motionOn, channel);
	motion.ch = channel;
	motion.fmtdef = NVP6158_Current_Video_Format_Check(channel);
	motion.set_val = motionOn;

	motion_onoff_set(&motion);
}

static BYTE ConvertSensitivity(BYTE nvData)
{
	BYTE result = 0;

	if(nvData > 0)
	{
		result = (nvData * 25) / 10;
	}

	return result;
}

BYTE Get_MotionDetect_Sensitivity(void)
{
	return motionDetectionSensitivity;
}

void Set_MotionDetect_Sensitivity(BYTE value)
{
	eChannel_t channel;
	motion_mode motion;

	motionDetectionSensitivity = ConvertSensitivity(value);
	motion.set_val = motionDetectionSensitivity;
	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		motion.ch = channel;
		motion.fmtdef = NVP6158_Current_Video_Format_Check(channel);
		motion_tsen_set(&motion);
	}
}

void Set_MotionDetect_ActivatedArea(eChannel_t channel)
{
	WORD activeBlocks[ROWS_OF_BLOCKS];
	WORD changedBlocks;
	BYTE index, bitIndex;
	motion_mode motion;

	motion.ch = channel;
	motion.fmtdef = NVP6158_Current_Video_Format_Check(channel);

	Read_NvItem_MotionBlock(activeBlocks, channel);
	
	for(index = 0; index < ROWS_OF_BLOCKS; index++)
	{
		if(memcmp(&activeBlocks[index], &motiondetectionInfo.previousActivatedBlock[index], sizeof(WORD)) != 0)
		{
			changedBlocks = activeBlocks[index] ^ motiondetectionInfo.previousActivatedBlock[index];
			for(bitIndex = 0; bitIndex < sizeof(WORD); bitIndex++)
			{
				if(0x0001 & (changedBlocks >> bitIndex))
				{
					motion.set_val = index * sizeof(WORD) + bitIndex;//pixel(block) number
					motion_pixel_onoff_set(&motion);
				}
			}
			motiondetectionInfo.previousActivatedBlock[index] = activeBlocks[index];
		}
	}
}

void MotionDetectCheck(void)
{
	unsigned char vMotion;
	unsigned char channel_num;
	vMotion = NVP6158_MotionDetect_Check();

	for(channel_num = 0; channel_num < NUM_OF_CHANNEL; channel_num++)
	{
		motiondetectionInfo[channel_num].motion_detected = vMotion >> channel_num;
	}
	if (motiondetectionInfo[CHANNEL1].motion_detected)
	{
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
	}
	if (motiondetectionInfo[CHANNEL2].motion_detected)
	{
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
		Delay_ms(100);
		BUZZER_HIGH;
		Delay_ms(100);
		BUZZER_LOW;
	}
	if (motiondetectionInfo[CHANNEL3].motion_detected)
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
	if (motiondetectionInfo[CHANNEL4].motion_detected)
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

void InitializeMotionDetect(void)
{
	eChannel_t channel;
	BYTE sensitivity;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		Set_MotionDetect_OnOff(channel);
		Set_MotionDetect_ActivatedArea(channel);
	}
	Read_NvItem_MotionSensitivity(&sensitivity);
	Set_MotionDetect_Sensitivity(sensitivity);
}
