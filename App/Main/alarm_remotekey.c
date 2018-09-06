//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
#define ALARM_DEBOUNCE_COUNT_MAX		5
//#define ALL_ALARM_STATE
#define ALARM_START						1
#define ALARM_STOP						0

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static BYTE alarm_remotekey_mode = ALARM_MODE;
static u32 alarmOutTimeCountInSec = 0;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================
static sAlarmInfo_t alarmInfo[NUM_OF_CHANNEL] =
{
//	option				status			raw_data		previous_data		check_count
	{ALARM_OPTION_NO, 	ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_OPTION_NO, 	ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_OPTION_NO, 	ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_OPTION_NO, 	ALARM_CLEAR,	0xFF,			0xFF,				0}
};

static u8 spiDataMask[NUM_OF_CHANNEL] = { 0x01, 0x02, 0x04, 0x08 };

//=============================================================================
//  Function Definition
//=============================================================================

static BYTE ReadSpiDataByte(void)
{
	u8 index;
	BYTE spiDataByte = 0x00;

	//	SPI_MISO_INPUT_MODE();
	SPI_CS_HIGH;
	for(index = 0; index < 8; index++)
	{
		spiDataByte <<= 1;
		if(SPI_MISO_DATA == SPI_MISO_HIGH)
		{
			spiDataByte |= 0x01;
		}
		SPI_CLK_LOW;
		SPI_DELAY;
		SPI_CLK_HIGH;
	}
	SPI_CS_LOW;

	return spiDataByte;
}

//------------------------------------------------------------------------------
BYTE GetAlarmRemoteKeyMode(void)
{
	return alarm_remotekey_mode;
}

void ChangeAlarmRemoteKeyMode(BYTE mode)
{
	alarm_remotekey_mode = mode;
}
//------------------------------------------------------------------------------
eAlarmOption_t GetAlarmOption(eChannel_t channel)
{
	return alarmInfo[channel].option;
}

void SetAlarmOption(eChannel_t channel, eAlarmOption_t option)
{
	if(option != alarmInfo[channel].option)
	{
		alarmInfo[channel].alarm_status = ALARM_CLEAR;
		alarmInfo[channel].option = option;
	}
}

void StartStopAlarm(BOOL start_stop)
{
	if(start_stop == ALARM_START)
	{
		alarmBuzzerCountIn500ms = sys_env.vAlarmBuzzerTime * 2;
		alarmOutTimeCountInSec = sys_env.vAlarmOutTime;
		ALARMOUT_LOW;
	}
	else
	{
		//alarmBuzzerCountIn500ms = 0;
		alarmOutTimeCountInSec = 0;
		ClearAllAlarm();
		ALARMOUT_HIGH;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CheckAlarm(void)//eChannel_t channel)
{
	u8 spiData = ReadSpiDataByte();
	u8 channel;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		alarmInfo[channel].raw_data = spiData & spiDataMask[channel];

		switch(alarmInfo[channel].option)
		{
			case ALARM_OPTION_OFF:
				alarmInfo[channel].alarm_status = ALARM_CLEAR;
				alarmInfo[channel].check_count = 0;
				break;

			case ALARM_OPTION_NO:
				if((alarmInfo[channel].raw_data != alarmInfo[channel].previous_data) ||
					(alarmInfo[channel].raw_data == spiDataMask[channel]))
				{
					alarmInfo[channel].check_count = 0;
				}
				else
				{
					alarmInfo[channel].check_count++;
				}
				break;

			case ALARM_OPTION_NC:
				if((alarmInfo[channel].raw_data != alarmInfo[channel].previous_data) ||
					(alarmInfo[channel].raw_data == LOW))
				{
					alarmInfo[channel].check_count = 0;
				}
				else
				{
					alarmInfo[channel].check_count++;
				}
				break;

			default:
				alarmInfo[channel].check_count = 0;
				break;
		}

		if(alarmInfo[channel].check_count > ALARM_DEBOUNCE_COUNT_MAX)
		{
			if(alarmInfo[channel].alarm_status == ALARM_CLEAR)
			{
				alarmInfo[channel].alarm_status = ALARM_SET;
				alarmInfo[channel].check_count = 0;
				//Occur key data (key_alarm) to display alarm screen
				UpdateKeyData(KEY_ALARM);
				SetKeyReady();
				//buzzer & alarm output
				StartStopAlarm(ALARM_START);
			}
		}

		alarmInfo[channel].previous_data = alarmInfo[channel].raw_data;
	}

}

void CheckAlarmClearCondition(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s = 0;

	if((TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn1s,1)) && (alarmOutTimeCountInSec !=0))
	{
		alarmOutTimeCountInSec--;
	}

	if(alarmOutTimeCountInSec == 0)
	{
		StartStopAlarm(ALARM_STOP);
	}
	previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
}

void ClearAllAlarm(void)
{
	eChannel_t channel;

	for(channel = CHANNEL1; channel<NUM_OF_CHANNEL; channel++)
	{
		alarmInfo[channel].alarm_status = ALARM_CLEAR;
	}
}
