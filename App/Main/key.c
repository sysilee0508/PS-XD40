//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
#include "NVP6168.h"
#endif

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

#define GPIO_KEY_SPLIT			GPIO_Pin_13	//SW2
#define GPIO_KEY_CH1			GPIO_Pin_12	//SW1
#define GPIO_KEY_CH2			GPIO_Pin_14	//SW3
#define GPIO_KEY_LEFT			GPIO_KEY_SPLIT | GPIO_KEY_CH1
#define	GPIO_KEY_RIGHT			GPIO_KEY_SPLIT | GPIO_KEY_CH2
#define GPIO_ALL_KEYS			GPIO_KEY_CH1 | GPIO_KEY_CH2 | GPIO_KEY_SPLIT

#define GPIO_LED_SPLIT			GPIO_Pin_15	//sw2
#define GPIO_LED_CH1			GPIO_Pin_9	//sw1
#define GPIO_LED_CH2			GPIO_Pin_3	//sw3
#define GPIO_ALL_LEDS			GPIO_LED_CH1 | GPIO_LED_CH2 | GPIO_LED_SPLIT

#define NUM_OF_KEYS				sizeof(key_table) //3

#define KEYCOUNT_SHORT			4
#define KEYCOUNT_LONG			80	//800ms

#define VALID_LONG_KEY(key)		(key == KEY_SPLIT)?TRUE:FALSE

#define	KEY_PRESSED				0
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
	u16 readData;

	readData = (((GPIO_ReadInputData(GPIOB)) & ((u16)GPIO_ALL_KEYS)) ^ ((u16)GPIO_ALL_KEYS));
	switch(readData)
	{
		case GPIO_KEY_CH1:
			scanKey = KEY_FULL_CH1;
			break;

		case GPIO_KEY_CH2:
			scanKey = KEY_FULL_CH2;
			break;

		case GPIO_KEY_SPLIT:
			scanKey = KEY_SPLIT;
			break;

		case GPIO_KEY_LEFT:
			scanKey = KEY_LEFT;
			break;

		case GPIO_KEY_RIGHT:
			scanKey = KEY_RIGHT;
			break;

		default:
			scanKey = KEY_NONE;
			break;
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
			case KEY_LEFT:
				GPIO_SetBits(GPIOB, GPIO_LED_CH1|GPIO_LED_SPLIT);
				break;
			case KEY_RIGHT:
				GPIO_SetBits(GPIOB, GPIO_LED_CH2|GPIO_LED_SPLIT);
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
	static u8 split = (u8)SPLIT_A;
	eKeyData_t key = GetCurrentKey();
	BOOL requestOsd = FALSE;

	if(IsKeyReady()==TRUE)
	{
		ClearKeyReady();

		if(key == KEY_FULL_CH2)
		{
#if BD_NVP == NVP6158
			NVP6158_Set_VportMap(VPORT_MAP1);
#elif BD_NVP == NVP6168
			NVP6168_VO_Port_Set(VPORT_MAP1);
#endif
		}
		else
		{
#if BD_NVP == NVP6158
			NVP6158_Set_VportMap(VPORT_MAP0);
#elif BD_NVP == NVP6168
			NVP6168_VO_Port_Set(VPORT_MAP0);
#endif
		}
		
		switch(key)
		{
			case KEY_FULL_CH1 : 
			case KEY_FULL_CH2 : 
				// If key is changed...
				if(previous_keydata != key)
				{
					DisplayScreen((eDisplayMode_t)(key - 1));
					requestOsd = TRUE;
				}
				break;
			case KEY_SPLIT :
				// If key is changed...
				if(previous_keydata != key)
				{
					if((previous_keydata == KEY_FULL_CH1 ) ||(previous_keydata == KEY_FULL_CH2))
					{
						DisplayScreen((eDisplayMode_t)split+DISPLAY_MODE_SPLIT_A);
						requestOsd = TRUE;
					}
					else if(previous_keydata == KEY_NONE)
					{
						DisplayScreen(GetCurrentDisplayMode());
						split = GetCurrentDisplayMode() - DISPLAY_MODE_SPLIT_A;
						requestOsd = TRUE;
					}
				}
				break;
				
			case KEY_LEFT:
			case KEY_RIGHT:
				if((previous_keydata != KEY_FULL_CH1 ) &&(previous_keydata != KEY_FULL_CH2))
				{
					if(key == KEY_RIGHT)
					{
						split = ++split % (NUM_OF_SPLIT+NUM_OF_PIP);
					}
					else if(key == KEY_LEFT)
					{
						if(split > SPLIT_A)
						{
							--split;
						}
						else
						{
							split = NUM_OF_SPLIT+NUM_OF_PIP-1;
						}
					}
				}
				DisplayScreen((eDisplayMode_t)split+DISPLAY_MODE_SPLIT_A);
				requestOsd = TRUE;
				break;
/*
			case KEY_SPLIT_LONG:
				if(previous_keydata == KEY_SPLIT)
				{
					split = ++split % NUM_OF_SPLIT;
				}
				else
				{
					Key_Led_Ctrl(KEY_SPLIT);
				}
				OSD_EraseAllText();
				OSD_RefreshScreen();
				DisplayScreen(split+DISPLAY_MODE_SPLIT_A);
				OSD_DrawBorderLine();
				break;
*/
		}

		if(requestOsd == TRUE)
		{
			Key_Led_Ctrl(key);
			OSD_EraseAllText();
			OSD_RefreshScreen();
			SetInputChanged();
			OSD_DrawBorderLine();
		}
		previous_keydata = (eKeyData_t)(key & 0x1F); // clear long or special key mark
	}
}
