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
static eKeyMode_t key_mode = KEY_MODE_LONG;
static eKeyMode_t saved_key_mode = KEY_MODE_LONG;
static eKeyStatus_t key_status = KEY_STATUS_RELEASED;
static BOOL bKeyReady = CLEAR;
static eKeyData_t current_keydata = KEY_NONE;
static eKeyData_t scanKey = KEY_NONE;

//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================

#define GPIO_KEY_CH1			GPIO_Pin_13	//SW2
#define GPIO_KEY_CH2			GPIO_Pin_12	//SW1
#define GPIO_KEY_SPLIT			GPIO_Pin_14	//SW3
#define GPIO_ALL_KEYS			GPIO_KEY_CH1 | GPIO_KEY_CH2 | GPIO_KEY_SPLIT
#define GPIO_LED_CH1			GPIO_Pin_15
#define GPIO_LED_CH2			GPIO_Pin_9	
#define GPIO_LED_SPLIT			GPIO_Pin_3	
#define GPIO_ALL_LEDS			GPIO_LED_CH1 | GPIO_LED_CH2 | GPIO_LED_SPLIT

#define NUM_OF_KEYS				sizeof(key_table) //3

#define KEYCOUNT_SHORT			4
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
	if(RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_KEY_CH1))	//KEY_CH1
	{
		scanKey = KEY_FULL_CH1;
	}
	else if(RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_KEY_CH2))	//KEY_CH2
	{
		scanKey = KEY_FULL_CH2;
	}
	else if(RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_KEY_SPLIT))	//KEY_SPLIT
	{
		scanKey = KEY_SPLIT;
	}
	else
	{
		scanKey = KEY_NONE;
	}
}

static void Key_Led_Ctrl(eKeyData_t key)
{
	static eKeyData_t previousKey = KEY_NONE;

	if(previousKey != key)
	{
		previousKey = key;
		GPIO_ResetBits(GPIOB, GPIO_ALL_LEDS);
		switch(key)
		{
			case KEY_FULL_CH1:
				GPIO_SetBits(GPIOB, GPIO_LED_CH1);
				break;
			case KEY_FULL_CH2:
				GPIO_SetBits(GPIOB, GPIO_LED_CH2);
				break;
			case KEY_SPLIT:
				GPIO_SetBits(GPIOB, GPIO_LED_SPLIT);
				break;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Key_Check(void)	
{
	static u8 debounce_cnt = KEYCOUNT_SHORT;
	static u8 key_cnt = 0;
	static eKeyData_t processing_key_data = KEY_NONE;
	static BOOL bLongKey = CLEAR;

	if(scanKey != KEY_NONE)
	{
		UpdateKeyStatus(KEY_STATUS_PRESSED);
		if(scanKey != processing_key_data)
		{
			// this in new key
			processing_key_data = scanKey;
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
					Key_Led_Ctrl(key);
					OSD_EraseAllText();
					OSD_RefreshScreen();
					DisplayScreen((eChannel_t)(key - 1));
					OSD_DrawBorderLine();
				}
				break;
				
			case KEY_SPLIT : 
				// display current split mode
				if(previous_keydata != key)
				{
					Key_Led_Ctrl(key);
					OSD_EraseAllText();
					OSD_RefreshScreen();
					DisplayScreen(DISPLAY_MODE_SPLIT_A);
					OSD_DrawBorderLine();
				}
				break;

			case KEY_SPLIT_LONG:
				Key_Led_Ctrl(KEY_SPLIT);
				// display next split mode
				break;
		}
		previous_keydata = (eKeyData_t)(key & 0x1F); // clear long or special key mark
	}
}


