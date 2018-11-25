//=============================================================================
//  Header Declaration
//=============================================================================
#include "NVP6158.h"
#include "common.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
typedef struct
{
	BOOL motion_detected;
	WORD previousActivatedBlock[ROWS_OF_BLOCKS];
} sMotionDetectInfo_t;

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static sMotionDetectInfo_t motiondetectionInfo[NUM_OF_CHANNEL] = 
{
	{FALSE, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}},
	{FALSE, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}},
	{FALSE, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}},
	{FALSE, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}}
};

static BYTE motionDetectionSensitivity = 49;
static BYTE motionBuzzerCountIn500ms = 0;

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

void Set_MotionDetect_OnOff(eChannel_t channel)
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
		if(memcmp(&activeBlocks[index], &motiondetectionInfo[channel].previousActivatedBlock[index], sizeof(WORD)) != 0)
		{
			changedBlocks = activeBlocks[index] ^ motiondetectionInfo[channel].previousActivatedBlock[index];
			for(bitIndex = 0; bitIndex < COLUMMS_OF_BLOCKS; bitIndex++)
			{
				if(0x0001 & (changedBlocks >> bitIndex))
				{
					motion.set_val = index * COLUMMS_OF_BLOCKS + bitIndex;//pixel(block) number
					motion_pixel_onoff_set(&motion);
				}
			}
			motiondetectionInfo[channel].previousActivatedBlock[index] = activeBlocks[index];
		}
	}
}

void MotionDetectCheck(void)
{
	eChannel_t channel;
	BYTE channel_mask;
	BYTE currentMotion = NVP6158_MotionDetect_Check();
	BYTE alarmBuzzerTime;
	static BYTE alarmOutTimeCountInSec = 0;
	static BYTE previousMotion = 0x00;
	static BOOL motionCleared = TRUE;
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s = 0;

	// buzzer
	if((previousMotion == 0) && (currentMotion > 0) && (alarmOutTimeCountInSec == 0))
	{
		// it means there is new motion detected channel
		Read_NvItem_AlarmBuzzerTime(&alarmBuzzerTime);
		motionBuzzerCountIn500ms =  alarmBuzzerTime * 2;
	}

	// photo coupler
	if(currentMotion > 0)
	{
		motionCleared = FALSE;
		TurnOnAlarmOut(ALARMOUT_REQUESTER_MOTION);
	}
	else if((motionCleared == FALSE) &&  (currentMotion == 0))
	{
		motionCleared = TRUE;
		Read_NvItem_AlarmOutTime(&alarmOutTimeCountInSec);
	}
	else
	{
		if((TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn1s,1)) && (alarmOutTimeCountInSec !=0))
		{
			alarmOutTimeCountInSec--;
		}
		else if(alarmOutTimeCountInSec == 0)
		{
			TurnOffAlarmOut(ALARMOUT_REQUESTER_MOTION);
		}
		previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
	}
	previousMotion = currentMotion;
}

BYTE GetMotionBuzzerCount(void)
{
	return motionBuzzerCountIn500ms;
}

void DecreaseMotionBuzzerCount(void)
{
	motionBuzzerCountIn500ms--;
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
