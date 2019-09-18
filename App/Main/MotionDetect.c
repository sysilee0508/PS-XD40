//=============================================================================
//  Header Declaration
//=============================================================================
#include "common.h"

#if BD_NVP == NVP6158
#include "NVP6158.h"
#include "motion.h"
#elif BD_NVP == NVP6168
#include "NVP6168.h"
#include "nc_sdk_define.h"

extern NC_CH_E NVP_Ch[4];

extern int DECODER_Motion_Set( int Chn, int SelectItem, int Val );
#endif

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

void Set_MotionDetect_Configue(eChannel_t channel)
{
#if BD_NVP == NVP6158
	motion_mode motion;
#endif
	BOOL motionOn;

	Read_NvItem_MotionDetectOnOff(&motionOn, channel);
#if BD_NVP == NVP6158
	motion.ch = channel;
	motion.fmtdef = NVP6158_Current_Video_Format_Check(channel);
	motion.set_val = motionOn;
	motion_onoff_set(&motion);
#elif BD_NVP == NVP6168
	DECODER_Motion_Set(NVP_Ch[channel], VD_MOTION_SET_ON_OFF, motionOn);
#endif
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
#if BD_NVP == NVP6158
	motion_mode motion;
#endif

	motionDetectionSensitivity = ConvertSensitivity(value);

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
#if BD_NVP == NVP6158
		motion.set_val = motionDetectionSensitivity;
		motion.ch = channel;
		motion.fmtdef = NVP6158_Current_Video_Format_Check(channel);
		motion_tsen_set(&motion);
#elif BD_NVP ==NVP6168
		DECODER_Motion_Set(channel, VD_MOTION_SET_TSEN, motionDetectionSensitivity);
#endif
	}
}

void Set_MotionDetect_ActivatedArea(eChannel_t channel)
{
	WORD activeBlocks[ROWS_OF_BLOCKS];
	WORD changedBlocks;
	BYTE index, bitIndex;
#if BD_NVP == NVP6158
	motion_mode motion;

	motion.ch = channel;
	motion.fmtdef = NVP6158_Current_Video_Format_Check(channel);
#elif BD_NVP == NVP6168
	BYTE value;
#endif

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
#if BD_NVP == NVP6158
					motion.set_val = index * COLUMMS_OF_BLOCKS + bitIndex;//pixel(block) number
					motion_pixel_onoff_set(&motion);
#elif BD_NVP == NVP6168
					value = index * COLUMMS_OF_BLOCKS + bitIndex;
					DECODER_Motion_Set(NVP_Ch[channel], VD_MOTION_SET_EACH_BLOCK_ON_OFF, value);
#endif
				}
			}
			motiondetectionInfo[channel].previousActivatedBlock[index] = activeBlocks[index];
		}
	}
}

//BOOL Set_MotionDetect_Indicator(void)
//{
//	BOOL indication;
//
//	Read_NvItem_MotionIndication(&indication);
//	return indication;
//}

void MotionDetectCheck(void)
{
	eChannel_t channel;
	BYTE channel_mask;
	BYTE currentMotion = 0;// = Get_MotionDetectedStatus();
	BYTE alarmBuzzerTime;
	static BYTE alarmOutTimeCountInSec = 0;
	static BYTE previousMotion = 0x00;
	static BOOL motionCleared = TRUE;
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s = 0;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		currentMotion |= (Get_MotionDetectedStatus(channel) << channel);
	}
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

BOOL Get_MotionDetectedStatus(eChannel_t channel)
{
	BYTE motion;
#if BD_NVP == NVP6158
	motion = NVP6158_MotionDetect_Check();
#elif BD_NVP == NVP6168
	motion = DECODER_Motion_Get(NVP_Ch[channel], VD_MOTION_GET_DETECT_INFO, 0);
#endif
	return ((motion & (0x01 << NVP_Ch[channel])) == 0)?FALSE:TRUE;
}

BYTE GetMotionBuzzerCount(void)
{
	return motionBuzzerCountIn500ms;
}

void DecreaseMotionBuzzerCount(void)
{
	motionBuzzerCountIn500ms--;
}

BOOL Get_MotionIndication(void)
{
	BOOL indication;

	Read_NvItem_MotionIndication(&indication);
	return indication;
}

void InitializeMotionDetect(void)
{
	eChannel_t channel;
	BYTE sensitivity;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		Set_MotionDetect_Configue(channel);
		Set_MotionDetect_ActivatedArea(channel);
	}
	//Set_MotionDetect_Indicator();
	Read_NvItem_MotionSensitivity(&sensitivity);
	Set_MotionDetect_Sensitivity(sensitivity);
}
