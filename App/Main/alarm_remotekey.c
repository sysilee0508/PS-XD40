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
#define ALARM_START						1
#define ALARM_STOP						0

#define UART_SOH						1
#define UART_STX						2
#define UART_ETX						3

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static BYTE alarm_remotekey_mode = ALARM_MODE;

static u32 alarmOutTimeCountInSec = 0;
static u8 alarmBuzzerCountIn500ms = 0;
static eChannel_t lastAlarmChannel = CHANNEL_QUAD;

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

static sVirtualKeys_t virtual_key_tabla[] =
{
	{KEY_NONE,			VIRTUAL_KEY_NONE},
	{KEY_FULL_CH1,		VIRTUAL_KEY_CH1},
	{KEY_FULL_CH2,		VIRTUAL_KEY_CH2},
	{KEY_FULL_CH3,		VIRTUAL_KEY_CH3},
	{KEY_FULL_CH4,		VIRTUAL_KEY_CH4},
	{KEY_4SPLIT,		VIRTUAL_KEY_4SPLIT},
	{KEY_FREEZE,		VIRTUAL_KEY_FREEZE},
	{KEY_AUTO_SEQ,		VIRTUAL_KEY_AUTO_SEQ},
	{KEY_MENU,			VIRTUAL_KEY_MENU}
};

//=============================================================================
//  Function Definition
//=============================================================================

//------------------------------------------------------------------------------
// Alarm (SPI function)
//------------------------------------------------------------------------------
static BYTE ReadSpiDataByte(void)
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
	SPI_CS_LOW;

	return spiDataByte;
}

//------------------------------------------------------------------------------
void CheckAlarm(void)
{
	u8 spiData = ReadSpiDataByte();
	u8 channel;
	eAlarmOption_t alarmOption;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		alarmInfo[channel].raw_data = spiData & spiDataMask[channel];
		Read_NvItem_AlarmOption(&alarmOption, channel);

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
				//buzzer & alarm output
				StartStopAlarm(ALARM_START);
				lastAlarmChannel = channel;
				//Occur key data (key_alarm) to display alarm screen
				UpdateKeyData(KEY_ALARM);
				SetKeyReady();
			}
		}
		alarmInfo[channel].previous_data = alarmInfo[channel].raw_data;
	}
}
//------------------------------------------------------------------------------
static void ClearAllAlarm(void)
{
	eChannel_t channel;

	for(channel = CHANNEL1; channel<NUM_OF_CHANNEL; channel++)
	{
		alarmInfo[channel].alarm_status = ALARM_CLEAR;
	}
	lastAlarmChannel = CHANNEL_QUAD;

	if(GetCurrentAutoSeq() == AUTO_SEQ_ALARM)
	{
		InitializeAutoSeq(AUTO_SEQ_NONE);
	}
}
//------------------------------------------------------------------------------
static void StartStopAlarm(BOOL start_stop)
{
	u8 alarmBuzzerTime;
	u8 alarmOutTime;

	Read_NvItem_AlarmBuzzerTime(&alarmBuzzerTime);
	Read_NvItem_AlarmOutTime(&alarmOutTime);

	if(start_stop == ALARM_START)
	{
		alarmBuzzerCountIn500ms = alarmBuzzerTime * 2;
		alarmOutTimeCountInSec = alarmOutTime;
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
u8 GetAlarmBuzzerCount(void)
{
	return alarmBuzzerCountIn500ms;
}
//
//void ClearAlarmBuzzerCount(void)
//{
//	alarmBuzzerCountIn500ms = 0;
//}
//------------------------------------------------------------------------------
void DecreaseAlarmBuzzerCount(void)
{
	alarmBuzzerCountIn500ms--;
}
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
eChannel_t GetLastAlarmChannel(void)
{
	return lastAlarmChannel;
}
//------------------------------------------------------------------------------
BOOL GetAlarmStatus(eChannel_t channel)
{
	return alarmInfo[channel].alarm_status;
}

//------------------------------------------------------------------------------
// Remote controller (UART function)
//------------------------------------------------------------------------------
static u8 GetRemotconId(void)
{
	u8 id;

	Read_NvItem_RemoconId(&id);
	return id;
}

void USART3_Init(void)
{
	USART_InitTypeDef USART_InitStructure;

	//USARTx configuration
	//USARTx configured as follow:
	//-Baudrate = 19200
	//-Word Length = 8bits
	//-One stop bit
	//-No parity
	//-Flow control None.
	//-Receive enabled

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength    = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits      = USART_StopBits_1;
	USART_InitStructure.USART_Parity        = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode          = USART_Mode_Rx;// | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	//Disable the USART3 at start
	if(GetAlarmRemoteKeyMode() == REMOTEKEY_MODE)
	{
		USART_Cmd(USART3, ENABLE);
	}
	else
	{
		USART_Cmd(USART3, DISABLE);
	}
}

void USART3_IRQHandler(void)
{
	static u8 errorCode = ERROR_NONE;
	u8 receivedData;
	u8 i;
	u8 remoconId = GetRemotconId();

	receivedData = USART_ReceiveData(USART3);
	// Clear the USART3 RX interrupt
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);

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
				errorCode = ERROR_INVALID_CONTROL;
			}
			break;

		case UART_STATE_HEADER:
			if(receivedData > REMOCON_ID_MAX)
			{
				errorCode = ERROR_INVALID_REMOCONID;
				uartProc_State = UART_STATE_SOH;
			}
			else if(receivedData != remoconId)
			{
				errorCode = ERROR_REMOCONID_MISMATCH;
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
				errorCode = ERROR_INVALID_CONTROL;
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

			for(i = 0; i < sizeof(virtual_key_tabla)/sizeof(sVirtualKeys_t); i++)
			{
				if(virtual_key_tabla[i].virtual_key == receivedData)
				{
					UpdateKeyData(virtual_key_tabla[i].keydata);
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
				errorCode = ERROR_INVALID_CONTROL;
				ClearKeyReady();
			}
			uartProc_State = UART_STATE_SOH;
			break;
	}
}

//------------------------------------------------------------------------------
void ChangeAlarmRemoteKeyMode(BYTE mode)
{
	if(mode == REMOTEKEY_MODE)
	{
		//enable uart
		USART_Cmd(USART3, ENABLE);
	}
	else
	{
		USART_Cmd(USART3, DISABLE);
	}
}

BYTE GetAlarmRemoteKeyMode(void)
{
	BYTE select;

	Read_NvItem_AlarmRemoconSelect(&select);

	return select;
}
//------------------------------------------------------------------------------
void AlarmRemoteKey_Proc(void)
{
	if(GetAlarmRemoteKeyMode() == ALARM_MODE)
	{
		CheckAlarm();
	}
}
