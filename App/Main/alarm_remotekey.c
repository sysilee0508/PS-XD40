//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"
#include "alarm_remotekey.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
//#define ALARM_DATA_MASK				0x0F //LSB 4 bits
//#define ALARM_CH(ch)					0x01 << ch
#define ALARM_DEBOUNCE_MAX_COUNT		5

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static BYTE alarm_remotekey_mode = ALARM_MODE;
//static BYTE previous_alarm_data;
//static BYTE current_alarm_data;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================
static sAlarmInfo_t alarmInfo[NUM_OF_CHANNEL] =
{
//	option				status			raw_data		previous_data		debounce_count
	{ALARM_OPTION_OFF, 	ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_OPTION_OFF, 	ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_OPTION_OFF, 	ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_OPTION_OFF, 	ALARM_CLEAR,	0xFF,			0xFF,				0}
};


static u8 spiDataMask[NUM_OF_CHANNEL] = { 0x01, 0x02, 0x04, 0x08 };

static u8 spiFakeData[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFF, 0xFF};

//=============================================================================
//  Function Definition
//=============================================================================

//static BYTE ReadDataBit(void)
//{
//	BYTE bit;
//
//	SPI_CS_HIGH;
//	SPI_CLK_LOW;
//	SPI_DELAY;
//	SPI_CLK_HIGH;
//	SPI_DELAY;
//	bit = SPI_MISO_DATA;
//	SPI_CLK_LOW;
//	//SPI_CS_LOW;
//	//SPI_DELAY;
//
//	return bit;
//}

static BYTE ReadSpiDataByte(void)
{
	//u8 i;
	BYTE spiDataByte = 0x00;
	//static BYTE temp;
#if 0
	SPI_CS_LOW;
	
	for(i = 0; i < 8; i++)
	{
		SPI_CLK_LOW;
		//SPI_DELAY;
		spiDataByte <<= 1;
		if(SPI_MISO_DATA == 1)
		{
			spiDataByte |= 0x01;
		}
		//SPI_CLK_LOW;
		SPI_DELAY;
		SPI_CLK_HIGH;
		SPI_DELAY;
		//temp = SPI_MISO_DATA;
		//spiDataByte |= SPI_MISO_DATA<<i;
	}
	SPI_CS_HIGH;
#else
	u8 fakeBufSize = sizeof(spiFakeData);
	static u8 index = 0;

	index = index % fakeBufSize;
	spiDataByte = spiFakeData[index];
	index++;
#endif

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
	alarmInfo[channel].option = option;
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
				alarmInfo[channel].debounce_count = 0;
				break;

			case ALARM_OPTION_NO:
				if((alarmInfo[channel].raw_data != alarmInfo[channel].previous_data) ||
					(alarmInfo[channel].raw_data == spiDataMask[channel]))
				{
					alarmInfo[channel].debounce_count = 0;
				}
				else
				{
					alarmInfo[channel].debounce_count++;
				}
				break;

			case ALARM_OPTION_NC:
				if((alarmInfo[channel].raw_data != alarmInfo[channel].previous_data) ||
					(alarmInfo[channel].raw_data == LOW))
				{
					alarmInfo[channel].debounce_count = 0;
				}
				else
				{
					alarmInfo[channel].debounce_count++;
				}
				break;

			default:
				alarmInfo[channel].debounce_count = 0;
				break;
		}

		if(alarmInfo[channel].debounce_count > ALARM_DEBOUNCE_MAX_COUNT)
		{
			alarmInfo[channel].alarm_status = ALARM_SET;
			alarmInfo[channel].debounce_count = 0;
		}

		alarmInfo[channel].previous_data = alarmInfo[channel].raw_data;
	}

}
