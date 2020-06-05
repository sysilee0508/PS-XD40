//=============================================================================
//  Header Declaration
//=============================================================================
#include "stm32f10x_usart.h"
#include "common.h"

//=============================================================================
//  Define & MACRO
//=============================================================================
#define ALARM_DEBOUNCE_COUNT_MAX		5
//#define ALL_ALARM_STATE
//#define ALARM_START						1
//#define ALARM_STOP							0

#define ALARM_OUT_CLEAR					0
#define ALARM_OUT_READY					1
#define ALARM_OUT_SET						2

#define PARALLELKEY_DEBOUNCE_COUNT_MAX	2

#define UART_SOH						1
#define UART_STX						2
#define UART_ETX						3

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static BOOL alarmRemoteEnable = TRUE;

static u32 alarmOutTimeCountIn500ms = 0;
static u8 alarmBuzzerCountIn500ms = 0;
static eChannel_t lastAlarmChannel = CHANNEL_NONE;
static u8 totalAlarmChannel = 0;

//static u8 alarmOutStatus = ALARM_OUT_CLEAR;

static u8 uartProc_State = UART_STATE_SOH;

//=============================================================================
//  Array Declaration (data table)
//=============================================================================
static sAlarmInfo_t alarmInfo[NUM_OF_CHANNEL] =
{
//	status			raw_data		previous_data		check_count
	{ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_CLEAR,	0xFF,			0xFF,				0},
	{ALARM_CLEAR,	0xFF,			0xFF,				0}
};
static u8 spiDataMask[NUM_OF_CHANNEL] = { 0x01, 0x02, 0x04, 0x08 };

static sVirtualKeys_t virtual_key_table[] =
{
	{KEY_NONE,			VIRTUAL_KEY_NONE},
	{KEY_FULL_CH1,		VIRTUAL_KEY_CH1},
	{KEY_FULL_CH2,		VIRTUAL_KEY_CH2},
	{KEY_FULL_CH3,		VIRTUAL_KEY_CH3},
	{KEY_FULL_CH4,		VIRTUAL_KEY_CH4},
	{KEY_SPLIT,			VIRTUAL_KEY_SPLIT},
	{KEY_FREEZE,		VIRTUAL_KEY_FREEZE},
	{KEY_AUTO_SEQ,		VIRTUAL_KEY_AUTO_SEQ},
	{KEY_MENU,			VIRTUAL_KEY_MENU}
};

//=============================================================================
//  Function Definition
//=============================================================================
BOOL CheckAlarmRemoteEnable(void)
{
	return alarmRemoteEnable;
}
//------------------------------------------------------------------------------
// Alarm (SPI function)
//------------------------------------------------------------------------------
BYTE ReadSpiDataByte(void)
{
	u8 index;
	BYTE spiDataByte = 0x00;

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
	alarmRemoteEnable = (SPI_MISO_DATA == SPI_MISO_LOW)?TRUE:FALSE;
	SPI_CS_LOW;

	return spiDataByte;
}

//------------------------------------------------------------------------------
void CheckAlarm(void)
{
	u8 spiData = ReadSpiDataByte();
	u8 channel;
	eAlarmOption_t alarmOption;
	BOOL alarmSelect;

	Read_NvItem_AlarmRemoconSelect(&alarmSelect);
	if((alarmSelect == ALARM_MODE) && (CheckAlarmRemoteEnable() == TRUE))
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			alarmInfo[channel].raw_data = spiData & spiDataMask[channel];
			Read_NvItem_AlarmOption(&alarmOption, (eChannel_t)channel);

			switch(alarmOption)
			{
				case ALARM_OPTION_OFF:
					alarmInfo[channel].alarm_status = ALARM_CLEAR;
					alarmInfo[channel].check_count = 0;
					break;

				case ALARM_OPTION_NO:
					if((alarmInfo[channel].raw_data != alarmInfo[channel].previous_data) ||
						(alarmInfo[channel].raw_data == spiDataMask[channel])) //high
					{
						if(alarmInfo[channel].alarm_status == ALARM_SET)
						{
							AlarmOutState(channel, ALARM_OUT_SET);
						}
						alarmInfo[channel].alarm_status = ALARM_CLEAR;
						alarmInfo[channel].check_count = 0;
					}
					else
					{
						if(alarmInfo[channel].alarm_status != ALARM_SET)
						{
							alarmInfo[channel].check_count++;
							alarmInfo[channel].alarm_status = ALARM_DEBOUNCE;
						}
					}
					break;

				case ALARM_OPTION_NC:
					if((alarmInfo[channel].raw_data != alarmInfo[channel].previous_data) ||
						(alarmInfo[channel].raw_data == LOW))
					{
						if(alarmInfo[channel].alarm_status == ALARM_SET)
						{
							AlarmOutState(channel, ALARM_OUT_SET);
						}
						alarmInfo[channel].alarm_status = ALARM_CLEAR;
						alarmInfo[channel].check_count = 0;
					}
					else
					{
						if(alarmInfo[channel].alarm_status != ALARM_SET)
						{
							alarmInfo[channel].check_count++;
							alarmInfo[channel].alarm_status = ALARM_DEBOUNCE;
						}
					}
					break;

				default:
					alarmInfo[channel].check_count = 0;
					break;
			}

			if(alarmInfo[channel].check_count > ALARM_DEBOUNCE_COUNT_MAX)
			{
				if(alarmInfo[channel].alarm_status == ALARM_DEBOUNCE)
				{
					alarmInfo[channel].alarm_status = ALARM_SET;
					OSD_SetEvent(channel, EVT_ALARM);
					alarmInfo[channel].check_count = 0;
					//buzzer & alarm output
					//StartStopAlarm(ALARM_START);
					AlarmOutState(channel, ALARM_OUT_READY);
					//UpdateLastAlarmChannel(channel);
					//Occur key data (key_alarm) to display alarm screen
					UpdateKeyData(KEY_ALARM);
					SetKeyReady();
				}
			}
			alarmInfo[channel].previous_data = alarmInfo[channel].raw_data;
		}
	}
}
//------------------------------------------------------------------------------
static void ClearAllAlarm(void)
{
	eChannel_t channel;

	for(channel = CHANNEL1; channel<NUM_OF_CHANNEL; channel++)
	{
		alarmInfo[channel].alarm_status = ALARM_CLEAR;
		alarmInfo[channel].check_count = 0;
	}
	lastAlarmChannel = CHANNEL_NONE;
}

//------------------------------------------------------------------------------
void AlarmOutState(eChannel_t alarmChannel, u8 state)
{
	u8 alarmBuzzerTime;
	u8 alarmOutTime;
	static eChannel_t prevChannel = 0xFF;

	Read_NvItem_AlarmBuzzerTime(&alarmBuzzerTime);
	Read_NvItem_AlarmOutTime(&alarmOutTime);

	switch(state)
	{
		case ALARM_OUT_CLEAR:
			prevChannel = 0xFF;
			totalAlarmChannel = 0;
			alarmOutTimeCountIn500ms = 0;
			TurnOnOffAlarmLed(OFF);
			break;

		case ALARM_OUT_READY:
			if(prevChannel != alarmChannel)
			{
				totalAlarmChannel++;
				prevChannel = alarmChannel;
			}
			if((alarmBuzzerCountIn500ms == 0) && (alarmOutTimeCountIn500ms == 0))
			{
				alarmBuzzerCountIn500ms = alarmBuzzerTime * 2;
			}
			TurnOnOffAlarmLed(ON);
			break;

		case ALARM_OUT_SET:
			alarmOutTimeCountIn500ms = alarmOutTime * 2;
			TurnOnOffAlarmLed(ON);
			break;
	}
//	alarmOutStatus = state;
}

//------------------------------------------------------------------------------
u8 GetAlarmBuzzerCount(void)
{
	return alarmBuzzerCountIn500ms;
}

//------------------------------------------------------------------------------
void DecreaseAlarmBuzzerCount(void)
{
	alarmBuzzerCountIn500ms--;
}
//------------------------------------------------------------------------------
//void CheckAlarmClearCondition(void)
void CountDown_AlarmOutTimer(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	eChannel_t channel;
	u8 alarmStatus = ALARM_CLEAR;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		if( alarmInfo[channel].alarm_status == ALARM_SET)
		{
			alarmStatus = ALARM_SET;
			break;
		}
	}

	if((TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5)) && (alarmOutTimeCountIn500ms !=0))
	{
		alarmOutTimeCountIn500ms--;
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
	}

 	if(alarmOutTimeCountIn500ms == 0) 
 	{
 		if(alarmStatus != ALARM_SET)
 		{
	 		AlarmOutState(channel, ALARM_OUT_CLEAR);
		}	
	}
}

#if 0
//------------------------------------------------------------------------------
void UpdateLastAlarmChannel(eChannel_t channel)
{
	lastAlarmChannel = channel;
}
//------------------------------------------------------------------------------
eChannel_t GetLastAlarmChannel(void)
{
	return lastAlarmChannel;
}
#else
eChannel_t FindAlarmChannel(void)
{
	eChannel_t iChannel;

	for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
	{
		if((GetAlarmStatus(iChannel) == ALARM_SET) ||(GetAlarmMotionStatus(iChannel) == SET))
		{
			break; //total++;
		}
	}

	return iChannel;
}
#endif

//------------------------------------------------------------------------------
u8 GetAlarmStatus(eChannel_t channel)
{
	return alarmInfo[channel].alarm_status;
}

void SetAlarmMotionStatus(eChannel_t channel, BOOL motion)
{
	alarmInfo[channel].alarm_motion = motion;
}

BOOL GetAlarmMotionStatus(eChannel_t channel)
{
	return alarmInfo[channel].alarm_motion;
}

//------------------------------------------------------------------------------
u8 GetTotalAlarmChannels(void)
{
#if 0
	eChannel_t iChannel;
	u8 total = 0;

	for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
	{
		if((GetAlarmStatus(iChannel) == ALARM_SET) ||(GetAlarmMotionStatus(iChannel) == SET))
		{
			total++;
		}
	}
	return total;
#else
	return totalAlarmChannel;
#endif
}

//------------------------------------------------------------------------------
void ChangeAlarmRemoteKeyMode(BYTE mode)
{
	if(mode == REMOTEKEY_MODE)
	{
		ClearAllAlarm();
	}
}

//------------------------------------------------------------------------------
// RS232 Communication
//------------------------------------------------------------------------------
static u32 Get_BaudRate(void)
{
	eBaudRate_t rate;
	u32 baudrate = 9600;

	Read_NvItem_SerialBaudrate(&rate);
	switch(rate)
	{
		case BAUDRATE_1200:
			baudrate = 1200;
			break;
		case BAUDRATE_2400:
			baudrate = 2400;
			break;
		case BAUDRATE_9600:
			baudrate = 9600;
			break;
	}
	return baudrate;
}

void USART3_Init(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = Get_BaudRate();
	USART_InitStructure.USART_Mode = USART_Mode_Rx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void)
{
	u16 receivedData;
	u8 i;
	u8 remoconId;

	Read_NvItem_RemoconId(&remoconId);

	if((remoconId == 0) && (uartProc_State == UART_STATE_SOH))
	{
		uartProc_State = UART_STATE_STX;
	}
	else if(remoconId != 0)
	{
		remoconId += 0x9F;
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		receivedData = USART_ReceiveData(USART3);
		// Clear the USART3 RX interrupt
		switch(uartProc_State)
		{
			case UART_STATE_SOH:
				if(receivedData == UART_SOH)
				{
					// valid data is received. move to next step
					uartProc_State = UART_STATE_HEADER;
				}
				else
				{
					// invalid data. do nothing
				}
				break;

			case UART_STATE_HEADER:
				if(receivedData != remoconId)
				{
					uartProc_State = UART_STATE_SOH;
				}
				else
				{
					// move to next step
					uartProc_State = UART_STATE_STX;
				}
				break;

			case UART_STATE_STX:
				if(receivedData == UART_STX)
				{
					uartProc_State = UART_STATE_CODE;
				}
				else
				{
					uartProc_State = UART_STATE_SOH;
				}
				break;

			case UART_STATE_CODE:
				uartProc_State = UART_STATE_ETX;

				if((receivedData >= 0x90) && (receivedData != VIRTUAL_KEY_FREEZE) && (receivedData != VIRTUAL_KEY_AUTO_SEQ))
				{
					receivedData -= 0x10;
				}

				if(GetSystemMode() == SYSTEM_SETUP_MODE)
				{
					if(receivedData == VIRTUAL_KEY_MENU)
					{
						receivedData = VIRTUAL_KEY_FREEZE; //KEY_MENU -> KEY_FREEZE
					}
				}

				for(i = 0; i < sizeof(virtual_key_table)/sizeof(sVirtualKeys_t); i++)
				{
					if(virtual_key_table[i].virtual_key == receivedData)
					{
						UpdateKeyData(virtual_key_table[i].keydata);
						break;
					}
				}
				break;

			case UART_STATE_ETX:
				if(receivedData == UART_ETX)
				{
					SetKeyReady();
				}
				else
				{
					ClearKeyReady();
				}
				uartProc_State = UART_STATE_SOH;
				break;
		}
	}
}

void ChangeBaudrate(void)
{
	USART_Cmd(USART3, DISABLE);
	USART_DeInit(USART3);

	USART3_Init();
}

