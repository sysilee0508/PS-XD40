//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================
BOOL screenFreezeOn = CLEAR;
//u8 pre_special_mode = LEFT_TOP;
BOOL forceFreezeOn = CLEAR;

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static keycode_t current_keycode = KEYCODE_NONE;
static keycode_t frontKeyCode = KEYCODE_NONE;
static keycode_t backKeyCode = KEYCODE_NONE;
static keycode_t led_keycode = KEYCODE_SPLIT;
static eKeyMode_t key_mode = KEY_MODE_LONG;
static eKeyMode_t saved_key_mode = KEY_MODE_LONG;
static eKeyStatus_t key_status = KEY_STATUS_RELEASED;
static BOOL bKeyReady = CLEAR;
static eKeyData_t current_keydata = KEY_NONE;


//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
const static keycode_t keycode_table[] =
{
	KEYCODE_CH1,		//0xFE	1111 1110	254
	KEYCODE_CH2,		//0xFD 	1111 1101	253
	KEYCODE_CH3,		//0xFB 	1111 1011 	251
	KEYCODE_CH4,		//0xF7 	1111 0111 	247

	KEYCODE_SPLIT,		//0xEF 	1110 1111 	239
	KEYCODE_FREEZE,		//0xDF 	1101 1111 	223
	KEYCODE_SEQUENCE,	//0xBF 	1011 1111 	191
	KEYCODE_NONE,		//0x7F
};

const static eKeyData_t key_table[] =
{
	KEY_FULL_CH1,
	KEY_FULL_CH2,
	KEY_FULL_CH3,
	KEY_FULL_CH4,
//-------------------------
	KEY_SPLIT,
	KEY_FREEZE,
	KEY_AUTO_SEQ,
};

#define NUM_OF_KEYS				sizeof(key_table) //7

#define KEYCOUNT_SHORT			4
#define KEYCOUNT_REPEAT			40	//400ms
#define KEYCOUNT_LONG			80	//800ms

#define VALID_LONG_KEY(key)		(key == KEY_FREEZE)?TRUE:FALSE

#define VALID_REPEAT_KEY(key)	\
	((key == KEY_FULL_CH1) || (key == KEY_FULL_CH2) || \
	 (key == KEY_FULL_CH3) || (key == KEY_FULL_CH4))?TRUE:FALSE

#define VALID_MENU_KEY(key)		\
	((key == KEY_LEFT) || (key == KEY_RIGHT) || \
	 (key == KEY_UP) || (key == KEY_DOWN) || \
	 (key == KEY_ENTER) || (key == KEY_EXIT))?TRUE:FALSE
//=============================================================================
//  Function Definition
//=============================================================================
static void AllButtonLedOff(void)
{
	led_keycode = KEYCODE_NONE;
}

void TurnOnSelectedLed(BYTE selectedLed)
{
	switch(selectedLed)
	{
		case LED_CH1:
			led_keycode = KEYCODE_CH1;
			break;
		case LED_CH2:
			led_keycode = KEYCODE_CH2;
			break;
		case LED_CH3:
			led_keycode = KEYCODE_CH3;
			break;
		case LED_CH4:
			led_keycode = KEYCODE_CH4;
			break;
		case LED_SPLIT:
			led_keycode = KEYCODE_SPLIT;
			break;
		case LED_SEQUENCE:
			led_keycode = KEYCODE_SEQUENCE;
			break;
	}
}

static void TurnOnOffFreezeLed(BOOL on_off)
{
	if(on_off == ON)
	{
		led_keycode &= KEYCODE_FREEZE;
	}
	else
	{
		//led_keycode = ~(led_keycode ^ KEYCODE_FREEZE);
		led_keycode |= (~KEYCODE_FREEZE);
	}
}
//-----------------------------------------------------------------------------
// Interface
//-----------------------------------------------------------------------------
void SetKeyMode(eKeyMode_t mode)
{
	saved_key_mode = mode;
	if(GetKeyStatus() == KEY_STATUS_RELEASED)
	{
		key_mode = mode;
	}
}

eKeyMode_t GetKeyMode(void)
{
	return key_mode;
}
//-----------------------------------------------------------------------------
keycode_t GetKeyCode(eKeyData_t key)
{
	keycode_t code = KEYCODE_NONE;
	u8 i;

	for(i=0; i<NUM_OF_KEYS; i++)
	{
		if(key == key_table[i])
		{
			break;
		}
	}

	if(i < NUM_OF_KEYS)
	{
		code = keycode_table[i];
	}

	return code;
}
void SetCurrentKeyCode(keycode_t keycode)
{
	current_keycode = keycode;
}
//-----------------------------------------------------------------------------
void UpdateKeyStatus(eKeyStatus_t status)
{
	key_status = status; 
}
eKeyStatus_t GetKeyStatus(void)
{
	return key_status;
}
//-----------------------------------------------------------------------------
void SetKeyReady(void)
{
	bKeyReady = SET;
}
void ClearKeyReady(void)
{
	bKeyReady = CLEAR;
}
BOOL IsKeyReady(void)
{
	return bKeyReady;
}
//-----------------------------------------------------------------------------
void UpdateKeyData(eKeyData_t key)
{
	current_keydata = key;
}
eKeyData_t GetCurrentKey(void)
{
	return current_keydata;
}
//-----------------------------------------------------------------------------
BOOL IsScreenFreeze(void)
{
	return screenFreezeOn;
}

void SetInitialKey(void)
{
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	eKeyData_t key = KEY_NONE;
	BYTE selectedLed;
	BOOL seqOn;
	eChannel_t iChannel;
	u8 initialTime[NUM_OF_CHANNEL] = {0,};

	Read_NvItem_AutoOn(&seqOn);
	if(seqOn == ON)
	{
		key = KEY_AUTO_SEQ;
		selectedLed = LED_SEQUENCE;
	}
	else
	{
		if(IS_FULL_MODE(displayMode))
		{
			key = (eKeyData_t)displayMode+1;
			selectedLed = ConvertDisplayMode2Channel(displayMode);
			initialTime[selectedLed] = VIDEO_MODE_DISPLAY_TIME+3;
		}
		else
		{
			key = KEY_SPLIT;
			selectedLed = LED_SPLIT;
			switch(displayMode)
			{
				case DISPLAY_MODE_2SPLIT_HSCALE_A:
				case DISPLAY_MODE_2SPLIT_HCROP_A:
				case DISPLAY_MODE_2SPLIT_VSCALE_A:
				case DISPLAY_MODE_2SPLIT_VCROP_A:
					initialTime[CHANNEL1] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL2] = VIDEO_MODE_DISPLAY_TIME+3;
					break;
					
				case DISPLAY_MODE_2SPLIT_HSCALE_B:
				case DISPLAY_MODE_2SPLIT_HCROP_B:
				case DISPLAY_MODE_2SPLIT_VSCALE_B:
				case DISPLAY_MODE_2SPLIT_VCROP_B:
					initialTime[CHANNEL3] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL4] = VIDEO_MODE_DISPLAY_TIME+3;
					break;

				case DISPLAY_MODE_3SPLIT_R2SCALE:
				case DISPLAY_MODE_3SPLIT_R2CROP:
				case DISPLAY_MODE_3SPLIT_L2SCALE:
				case DISPLAY_MODE_3SPLIT_L2CROP:
				case DISPLAY_MODE_3SPLIT_D2SCALE:
				case DISPLAY_MODE_3SPLIT_D2CROP:
				case DISPLAY_MODE_3SPLIT_U2SCALE:
				case DISPLAY_MODE_3SPLIT_U2CROP:
					initialTime[CHANNEL1] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL2] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL3] = VIDEO_MODE_DISPLAY_TIME+3;
					break;

				case DISPLAY_MODE_4SPLIT_QUAD:
				case DISPLAY_MODE_4SPLIT_R3SCALE:
				case DISPLAY_MODE_4SPLIT_R3CROP:
				case DISPLAY_MODE_4SPLIT_L3SCALE:
				case DISPLAY_MODE_4SPLIT_L3CROP:
				case DISPLAY_MODE_4SPLIT_D3SCALE:
				case DISPLAY_MODE_4SPLIT_D3CROP:
				case DISPLAY_MODE_4SPLIT_U3SCALE:
				case DISPLAY_MODE_4SPLIT_U3CROP:
				case DISPLAY_MODE_4SPLIT_H:
				case DISPLAY_MODE_4SPLIT_X:
					initialTime[CHANNEL1] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL2] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL3] = VIDEO_MODE_DISPLAY_TIME+3;
					initialTime[CHANNEL4] = VIDEO_MODE_DISPLAY_TIME+3;
					break;
			}
		}
	}
	
	UpdateKeyData(key);
	TurnOnSelectedLed(selectedLed);
	SetKeyReady();

	for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
	{
		if(initialTime[iChannel] == 0)
			initialTime[iChannel] = VIDEO_MODE_DISPLAY_TIME;
		
		ResetVideoModeDisplayTime(iChannel, initialTime[iChannel] );
	}
}

//-----------------------------------------------------------------------------
//  Key Functions
//-----------------------------------------------------------------------------
void Key_Scan_ParallelKey(void)
{
	BOOL paralleKeyOn;

	Read_NvItem_AlarmRemoconSelect(&paralleKeyOn);
	if((paralleKeyOn == REMOTEKEY_MODE) && (frontKeyCode == KEYCODE_NONE) && (CheckAlarmRemoteEnable()==TRUE))
	{
		backKeyCode = (keycode_t)ReadSpiDataByte();
		current_keycode = backKeyCode;
	}
	else
	{
		backKeyCode = KEYCODE_NONE;
	}
}

void Key_Scan(void)
{
	keycode_t key_code = KEYCODE_NONE;
	keycode_t tempKey = KEYCODE_NONE;

	if(backKeyCode == KEYCODE_NONE)
	{
		// All key columns are HIGH and LED rows are LOW
		KEY_LED1_5_HIGH;
		KEY_LED2_6_HIGH;
		KEY_LED3_7_HIGH;
		KEY_LED4_HIGH;

		KEY_LED0_LOW;
		KEY_LED1_LOW;

		KEY_DATA_INPUT_MODE;

		//Scan KROW0
		KEY_ROW1_HIGH;
		KEY_ROW0_LOW;

		Delay_us(10);

		if(LOW == KEY_DATA1_5_INPUT)
			key_code = KEYCODE_CH1;
		else if(LOW== KEY_DATA2_6_INPUT)
			key_code = KEYCODE_CH2;
		else if(LOW == KEY_DATA3_7_INPUT)
			key_code = KEYCODE_CH3;
		else if(LOW == KEY_DATA4_INPUT)
			key_code = KEYCODE_CH4;

		//Scan KROW1
		KEY_ROW0_HIGH;
		KEY_ROW1_LOW;

		Delay_us(10);

		if(LOW == KEY_DATA1_5_INPUT)
			key_code = KEYCODE_SPLIT;
		else if(LOW== KEY_DATA2_6_INPUT)
			key_code = KEYCODE_FREEZE;
		else if(LOW == KEY_DATA3_7_INPUT)
			key_code = KEYCODE_SEQUENCE;

		KEY_ROW1_HIGH;

		if((key_code != KEYCODE_NONE) && (frontKeyCode != key_code))
		{
			if(key_code == KEYCODE_FREEZE)
			{
				tempKey = ~(led_keycode ^ key_code);
				led_keycode = tempKey;
			}
			else
			{
				led_keycode = key_code;
			}
			UpdateKeyStatus(KEY_STATUS_PRESSED);
		}
		else if(key_code == KEYCODE_NONE)
		{
			UpdateKeyStatus(KEY_STATUS_RELEASED);
		}
		
		frontKeyCode = key_code;
		// Update current_keycode
		current_keycode = key_code;
	}
}

void Key_Led_Ctrl(void)
{
	static u8 stage = KEYLED_STAGE_LEFT;

	if(GetSystemMode() == SYSTEM_SETUP_MODE)
	{
		led_keycode = KEYCODE_NONE;
	}

	if(led_keycode != KEYCODE_NONE)
	{
		KEY_DATA_OUTPUT_MODE;

		if(stage == KEYLED_STAGE_LEFT)
		{
			KEY_LED0_LOW;
			if((led_keycode & 0x0F) != 0x0F)
			{
				KEY_LED_ON(led_keycode);
			}
			KEY_LED1_HIGH;
		}
		else if(stage == KEYLED_STAGE_RIGHT)
		{
			KEY_LED1_LOW;
			if((led_keycode & 0xF0) != 0xF0)
			{
				KEY_LED_ON((u32)((led_keycode>>4)|0xFFFFFFF0));
			}
			KEY_LED0_HIGH;
		}
	}
	// Change stage for the next time
	stage = (++stage) % KEYLED_STAGE_MAX;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Key_Check(void)	
{
	const keycode_t *pKeyCode;

	static u8 debounce_cnt = KEYCOUNT_SHORT;
	static u8 key_cnt = 0;
	static eKeyData_t processing_key_data = KEY_NONE;
	static BOOL bLongKey = CLEAR;
	static BOOL bRepeatKey = CLEAR;

	u8 i;

	if(current_keycode != KEYCODE_NONE)
	{
		pKeyCode = keycode_table;
		UpdateKeyStatus(KEY_STATUS_PRESSED);

		// Find index of key code table
		for(i=0; i< NUM_OF_KEYS; i++)
		{
			if(*(pKeyCode+i) == current_keycode)
			{
				break;
			}
		}

		if(i < NUM_OF_KEYS)
		{
			if(key_table[i] != processing_key_data)
			{
				processing_key_data = key_table[i];
				key_cnt = 0;
			}
			else
			{
				key_cnt++;
			}

			if(key_cnt >= debounce_cnt)
			{
				switch (GetKeyMode())
				{
				case KEY_MODE_SHORT:
					if(CLEAR == bRepeatKey)
					{
						bRepeatKey = SET;
						bLongKey = CLEAR;
						UpdateKeyData(processing_key_data);
						SetKeyReady();
					}
					break;

				case KEY_MODE_REPEAT:
					UpdateKeyData(processing_key_data);//current_keydata = processing_key_data;
					debounce_cnt = KEYCOUNT_REPEAT;
					if(SET == bRepeatKey)
					{
						if(FALSE == VALID_REPEAT_KEY(processing_key_data))
						{
							bRepeatKey = SET;
							key_cnt = 0;
							return;
						}
						debounce_cnt = KEYCOUNT_SHORT;
					}
					SetKeyReady();
					bRepeatKey = SET;
					key_cnt = 0;
					break;

				case KEY_MODE_LONG:
					bLongKey = SET;
					if((VALID_LONG_KEY(processing_key_data)) && (key_cnt > KEYCOUNT_LONG))
					{
						bRepeatKey = SET;

						UpdateKeyData((eKeyData_t)(processing_key_data | KEY_LONG));
						SetKeyReady();
					}
					else
					{
						UpdateKeyData(processing_key_data);
					}
					break;

				case KEY_MODE_MAX:
				default:
					// Do nothing
					break;
				}
			}
		}
		else //reset all flags and count.
		{
			bLongKey = CLEAR;
			bRepeatKey = CLEAR;
			debounce_cnt = KEYCOUNT_SHORT;
			key_cnt = 0;
			UpdateKeyStatus(KEY_STATUS_RELEASED);
		}
	}
	else
	{
		if(SET == bLongKey)
		{
			bLongKey = CLEAR;
			SetKeyReady();
		}
		bRepeatKey = CLEAR;
		debounce_cnt = KEYCOUNT_SHORT;
		key_cnt = 0;
		UpdateKeyStatus(KEY_STATUS_RELEASED);
		if(GetKeyMode() != saved_key_mode)
		{
			SetKeyMode(saved_key_mode);
		}
	}
}

void Key_Proc(void)
{
	static eKeyData_t previous_keydata = KEY_NONE;
	eKeyData_t key = GetCurrentKey();
	BOOL autoSeq_skipNoVideoChannel;
	eChannel_t channel; 
	BOOL prevKeyUpdated = TRUE;
	//static BOOL seqOn = OFF;

	if(IsKeyReady()==TRUE)
	{
		ClearKeyReady();

		if(GetSystemMode() == SYSTEM_SETUP_MODE)
		{
			key |= KEY_SPECIAL;
		}

		switch(key)
		{
			case KEY_FULL_CH1 : 
			case KEY_FULL_CH2 : 
			case KEY_FULL_CH3 : 
			case KEY_FULL_CH4 : 
				if(forceFreezeOn == SET)
				{
					UpdateKeyData(key);
					SetKeyReady();
					prevKeyUpdated = FALSE;
				}
				else
				{
					for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
					{
						OSD_ClearEvent(channel, EVT_ALARM);
						OSD_ClearEvent(channel, EVT_MOTION);
						OSD_ClearEvent(channel, EVT_FREEZE);
						AlarmOutState(ALARM_OUT_CLEAR);
						if(screenFreezeOn == SET)
						{
							screenFreezeOn = CLEAR;
						}
					}
					// If key is changed...
					if(previous_keydata != key)
					{
						forceFreezeOn = SET;
						//OSD_EraseAllText();
						OSD_EraseIndicator();
						InitializeAutoSeq(AUTO_SEQ_NONE);
						OSD_RefreshScreen();

						if(IS_FULL_MODE(GetCurrentDisplayMode()) == TRUE)
						{
							channel = ConvertDisplayMode2Channel(GetCurrentDisplayMode());
							if(GetVideoModeDisplayTime(channel) < VIDEO_MODE_DISPLAY_TIME)
							{
								ClearVideoModeDisplayTime(ConvertDisplayMode2Channel(GetCurrentDisplayMode()));
							}
						}
						
						channel = (eChannel_t)(key-1);
						DisplayScreen((eDisplayMode_t)channel);
						SetInputChanged();
						OSD_SetBoaderLine();//OSD_DrawBorderLine();
					}
				}
				break;
				
			case KEY_SPLIT : 
				if(forceFreezeOn == SET)
				{
					UpdateKeyData(key);
					SetKeyReady();
					prevKeyUpdated = FALSE;
				}
				else
				{
					for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
					{
						OSD_ClearEvent(channel, EVT_ALARM);
						OSD_ClearEvent(channel, EVT_MOTION);
						AlarmOutState(ALARM_OUT_CLEAR);
					}
					
					if(GetCurrentDisplayMode() != GetSystemSplitMode())
					{
						forceFreezeOn = SET;
						if((screenFreezeOn == SET) || (previous_keydata == KEY_FREEZE))
						{
							screenFreezeOn = CLEAR;
							for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
							{
								OSD_ClearEvent(channel, EVT_FREEZE);
							}
							M380_ID = MDIN_ID_C;
							MDIN3xx_EnableMainFreeze(MDIN_ID_C, OFF);	//main freeze Off
							MDIN3xx_EnableAuxFreeze(&stVideo[M380_ID], OFF);

							if(IS_FULL_MODE(GetCurrentDisplayMode()) == TRUE)
							{
								//OSD_EraseAllText();
								OSD_EraseIndicator();
								InitializeAutoSeq(AUTO_SEQ_NONE);
								OSD_RefreshScreen();
								DisplayScreen(GetSystemSplitMode());
								SetInputChanged();
								OSD_SetBoaderLine();//OSD_DrawBorderLine();
							}
						}
						else
						{
							//OSD_EraseAllText();
							OSD_EraseIndicator();
							InitializeAutoSeq(AUTO_SEQ_NONE);
							OSD_RefreshScreen();
							DisplayScreen(GetSystemSplitMode());
							SetInputChanged();
							OSD_SetBoaderLine();//OSD_DrawBorderLine();
						}
					}
				}
				break;

			case KEY_FREEZE :
				if(forceFreezeOn == SET)
				{
					UpdateKeyData(key);
					SetKeyReady();
					prevKeyUpdated = FALSE;
				}
				else
				{
					for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
					{
						OSD_ClearEvent(channel, EVT_ALARM);
						OSD_ClearEvent(channel, EVT_MOTION);
						AlarmOutState(ALARM_OUT_CLEAR);
					}
					//InitializeAutoSeq(AUTO_SEQ_NONE);
					M380_ID = MDIN_ID_C;
					if(screenFreezeOn == CLEAR)
					{
						screenFreezeOn = SET;
						TurnOnOffFreezeLed(ON);
						if(GetAutoSeqOn() == ON)
						{
							PauseAutoSeq();
						}
						MDIN3xx_EnableMainFreeze(MDIN_ID_C, ON);	//main freeze On
						MDIN3xx_EnableAuxFreeze(&stVideo[M380_ID], ON);
						for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
						{
							OSD_SetEvent(channel, EVT_FREEZE);
						}
					}
					else
					{
						TurnOnOffFreezeLed(OFF);
						for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
						{
							OSD_ClearEvent(channel, EVT_FREEZE);
						}

						MDIN3xx_EnableMainFreeze(MDIN_ID_C, OFF);	//main freeze Off
						MDIN3xx_EnableAuxFreeze(&stVideo[M380_ID], OFF);
						screenFreezeOn = CLEAR;

						if(GetAutoSeqOn() == ON)
						{
							ResumeAutoSeq();
						}

					}
				}
				break;

			case KEY_AUTO_SEQ :
				if(forceFreezeOn == SET)
				{
					UpdateKeyData(key);
					SetKeyReady();
					prevKeyUpdated = FALSE;
				}
				else
				{
					for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
					{
						OSD_ClearEvent(channel, EVT_ALARM);
						OSD_ClearEvent(channel, EVT_MOTION);
						OSD_ClearEvent(channel, EVT_FREEZE);
						AlarmOutState(ALARM_OUT_CLEAR);
					}
					Read_NvItem_AutoSeqLossSkip(&autoSeq_skipNoVideoChannel);
					if((OFF == autoSeq_skipNoVideoChannel) || (GetVideoLossChannels() != VIDEO_LOSS_CHANNEL_ALL))
					{
						forceFreezeOn = SET;
						if(screenFreezeOn == SET)
						{
							screenFreezeOn = CLEAR;
						}
						OSD_EraseIndicator();
						InitializeAutoSeq(AUTO_SEQ_NORMAL);
						//OSD_RefreshScreen();
					}
				}
				break;

			case KEY_MENU :
				for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
				{
					OSD_ClearEvent(channel, EVT_ALARM);
					OSD_ClearEvent(channel, EVT_MOTION);
					OSD_ClearEvent(channel, EVT_FREEZE);
					AlarmOutState(ALARM_OUT_CLEAR);
				}
				AllButtonLedOff();
				InitializeAutoSeq(AUTO_SEQ_NONE);
				if(screenFreezeOn == SET)
				{
					screenFreezeOn = CLEAR;
					M380_ID = MDIN_ID_C;
					MDIN3xx_EnableMainFreeze(MDIN_ID_C, OFF);	//main freeze Off
					MDIN3xx_EnableAuxFreeze(&stVideo[M380_ID], OFF);
				}
				Enter_MainMenu();
				break;

			case KEY_ALARM :
				// Sound out beep for configured time(in sec)
				InitializeAutoSeq(AUTO_SEQ_NONE);
				if(screenFreezeOn == SET)
				{
					screenFreezeOn = CLEAR;
					for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
					{
						OSD_ClearEvent(channel, EVT_FREEZE);
					}
					M380_ID = MDIN_ID_C;
					MDIN3xx_EnableMainFreeze(MDIN_ID_C, OFF);	//main freeze Off
					MDIN3xx_EnableAuxFreeze(&stVideo[M380_ID], OFF);
				}
				forceFreezeOn = SET;
				if(GetTotalAlarmChannels() == 1)
				{
					channel = GetLastAlarmChannel();
					if(GetCurrentDisplayMode() != (DISPLAY_MODE_FULL_CH1 + channel))
					{
						OSD_EraseIndicator();
						DisplayScreen((eDisplayMode_t)channel);
						SetInputChanged();
					}
				}
				else
				{
					if(GetCurrentDisplayMode() != DISPLAY_MODE_4SPLIT_QUAD)
					{
						OSD_EraseIndicator();
						DisplayScreen(DISPLAY_MODE_4SPLIT_QUAD);
						SetInputChanged();
					}
				}
				OSD_RefreshScreen();
				OSD_SetBoaderLine();//OSD_DrawBorderLine();
				break;

			case KEY_UP:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
			case KEY_ENTER:
			case KEY_EXIT:
				Menu_KeyProc(key);
				break;
		}

		if(prevKeyUpdated == TRUE)
		{
			previous_keydata = (eKeyData_t)(key & 0x3F); // clear long or special key mark
			//previous_keydata = key;
		}
	}
}


