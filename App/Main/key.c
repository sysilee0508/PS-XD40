//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static keycode_t current_keycode = KEYCODE_NONE;
static keycode_t frontKeyCode = KEYCODE_NONE;
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
	KEYCODE_SPLIT,		//0xEF 	1110 1111 	239
	KEYCODE_NONE,		//0x7F
};

const static eKeyData_t key_table[] =
{
	KEY_FULL_CH1,
	KEY_FULL_CH2,
	KEY_SPLIT
};

#define NUM_OF_KEYS				sizeof(key_table) //3

#define KEYCOUNT_SHORT			4
#define KEYCOUNT_REPEAT			40	//400ms
#define KEYCOUNT_LONG			80	//800ms

#define VALID_LONG_KEY(key)		(key == KEY_SPLIT)?TRUE:FALSE

//=============================================================================
//  Function Definition
//=============================================================================

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
//  Key Functions
//-----------------------------------------------------------------------------
void Key_Scan(void)
{
/*
	keycode_t key_code = KEYCODE_NONE;
	keycode_t tempKey = KEYCODE_NONE;

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
*/
}

void Key_Led_Ctrl(void)
{
	/*
	static u8 stage = KEYLED_STAGE_LEFT;

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
	*/
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
					bLongKey = CLEAR;
					UpdateKeyData(processing_key_data);
					SetKeyReady();
					break;

				case KEY_MODE_LONG:
					bLongKey = SET;
					if((VALID_LONG_KEY(processing_key_data)) && (key_cnt > KEYCOUNT_LONG))
					{
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

	if(IsKeyReady()==TRUE)
	{
		ClearKeyReady();

		switch(key)
		{
			case KEY_FULL_CH1 : 
			case KEY_FULL_CH2 : 
				// If key is changed...
				if(previous_keydata != key)
				{
					OSD_EraseAllText();
					OSD_RefreshScreen();
					DisplayMode_FullScreen((eChannel_t)(key - 1));
					OSD_DrawBorderLine();
				}
				break;
				
			case KEY_SPLIT : 
				if(previous_keydata != key)
				{
					OSD_EraseAllText();
					OSD_RefreshScreen();
					DisplayMode_SplitScreen(Get_SystemSplitMode());
					OSD_DrawBorderLine();
				}
				break;

			case KEY_SPLIT_LONG:
				break;
		}
		previous_keydata = (eKeyData_t)(key & 0x1F); // clear long or special key mark
	}
}


