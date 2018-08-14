//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================
BOOL key_flag = CLEAR;
keydata_e key_data = KEY_NONE;
keydata_e pre_key_data = KEY_NONE;
//u16 led_state = 0xffff;
BOOL bFreeze = CLEAR;

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static keycode_t current_keycode = KEYCODE_NONE;
//static keycode_t led_state = KEYCODE_NONE;
static key_mode_e key_mode = KEY_MODE_LONG;

//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
const static keycode_t keycode_table[] =
{
	KEYCODE_CAM1, 		//0xFE	1111 1110	254
 	KEYCODE_CAM2, 		//0xFD 	1111 1101	253
	KEYCODE_CAM3, 		//0xFB 	1111 1011 	251
	KEYCODE_CAM4, 		//0xF7 	1111 0111 	247

	KEYCODE_SPLIT, 		//0xEF 	1110 1111 	239
	KEYCODE_FREEZE, 	//0xDF 	1101 1111 	223
	KEYCODE_SEQUENCE, 	//0xBF 	1011 1111 	191
	KEYCODE_NONE		//0x7f	0111 1111	127
};

const static keydata_e key_table[] =
{
	KEY_FULL_CH1,
	KEY_FULL_CH2,
	KEY_FULL_CH3,
	KEY_FULL_CH4,
//-------------------------
	KEY_4SPLIT,
	KEY_FREEZE,
	KEY_AUTO_SEQ,
};

#define NUM_OF_KEYS			sizeof(key_table) //7

#define KEYCOUNT_SHORT		3
#define KEYCOUNT_REPEAT		20
#define KEYCOUNT_LONG		35
//
//#define KEYLED_ROW0_EN		KEY_ROW0_LOW; KEY_LED1_HIGH;
//#define KEYLED_ROW0_DIS		KEY_ROW0_HIGH; KEY_LED1_LOW;
//#define KEYLED_ROW1_EN		KEY_ROW1_LOW; KEY_LED0_HIGH;
//#define KEYLED_ROW1_DIS		KEY_ROW1_HIGH; KEY_LED0_LOW;

//=============================================================================
//  Function Definition
//=============================================================================

//-----------------------------------------------------------------------------
// Interface
//-----------------------------------------------------------------------------
void SetKeyMode(key_mode_e mode)
{
	key_mode = mode;
}

key_mode_e GetKeyMode(void)
{
	return key_mode;
}
//-----------------------------------------------------------------------------
keycode_t GetKeyCode(keydata_e key)
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

//-----------------------------------------------------------------------------
//  ���� ȭ�� ���¿� ���� Ű������ LED�� � ������ �� ���ΰ��� ó���ϴ� �Լ�
//-----------------------------------------------------------------------------
void Key_LED_Set(void)
{
#if 0
	//led_state = 0xff;
	
	if(bSETUP == 0)
	{
#ifdef __4CH__
		if(sys_status.current_split_mode <= FULL_4) 
		{
			led_state = keycode_table[sys_status.current_split_mode];
		}	
		else if(sys_status.current_split_mode <= SPLIT4_1)
		{
			led_state = keycode_table[4];
		}
		
	    if(bFreeze)
			led_state &= keycode_table[5];
	    if(bAuto_Seq_Flag) 
			led_state &= keycode_table[6];
#endif
	}
#endif
}

#ifdef __4CH__
void Key_Scan(void)
{
//	static keycode_t active_key_code = KEYCODE_NONE;
	keycode_t key_code = KEYCODE_NONE;

	//Scan KROW0
	KEY_ROW1_HIGH;
	KEY_ROW0_LOW;
	
	KEY_DATA_INPUT_MODE;
       
	if(LOW == KEY_DATA1_5_INPUT)
		key_code = KEYCODE_CAM1;
	else if(LOW== KEY_DATA2_6_INPUT)
		key_code = KEYCODE_CAM2;
	else if(LOW == KEY_DATA3_7_INPUT)
		key_code = KEYCODE_CAM3;
	else if(LOW == KEY_DATA4_INPUT)
		key_code = KEYCODE_CAM4;

	//Scan KROW1
	KEY_ROW0_HIGH;
	KEY_ROW1_LOW;

	if(LOW == KEY_DATA1_5_INPUT)
		key_code = KEYCODE_SPLIT;
	else if(LOW== KEY_DATA2_6_INPUT)
		key_code = KEYCODE_FREEZE;
	else if(LOW == KEY_DATA3_7_INPUT)
		key_code = KEYCODE_SEQUENCE;

	KEY_ROW1_HIGH;
	//KEY_EN_HIZ_MODE;
	
//	if(key_code != KEYCODE_NONE)
//	{
//		active_key_code = key_code;
//	}
//	led_state = active_key_code;

	// Update current_keycode
	current_keycode = key_code;
}

void Key_Led_Ctrl(void)
{
	static u8 stage = KEYLED_STAGE_LEFT;
	keycode_t leds = GetKeyCode(key_data);

	if(leds != KEYCODE_NONE)
	{
		KEY_DATA_OUTPUT_MODE;

		// If key data is changed, turn off all leds
		if(key_data != pre_key_data)
		{
			KEY_LED0_LOW;
			KEY_LED1_LOW;

			KEY_LED1_5_HIGH;
			KEY_LED2_6_HIGH;
			KEY_LED3_7_HIGH;
			KEY_LED4_HIGH;
		}

		if(stage == KEYLED_STAGE_LEFT)
		{
			KEY_LED0_LOW;
			if((leds & 0x0F) != 0x0F)
			{
				KEY_LED_ON(leds);
			}
			KEY_LED1_HIGH;
		}
		else if(stage == KEYLED_STAGE_RIGHT)
		{
			KEY_LED1_LOW;
			if((leds & 0xF0) != 0xF0)
			{
				KEY_LED_ON((u32)((leds>>4)|0xFFFFFFF0));
			}
			KEY_LED0_HIGH;
		}
	}
	// Change stage for the next time
	stage = (++stage) % KEYLED_STAGE_MAX;
}
#endif

//-----------------------------------------------------------------------------
//	KeyINPUT()�Լ����� ���� ���� ���¿� ���� �ٽ� ó���ϴ� �Լ�	(20ms���� ����)
//-----------------------------------------------------------------------------
void Key_Check(void)	
{
	const keycode_t *pKeyCode;
	static BOOL bLongKey = CLEAR;
	static BOOL bRepeatKey = CLEAR;
	static u8 debounce_cnt = KEYCOUNT_SHORT;
	static u8 key_cnt = 0;
	static keydata_e temp_key_data = 0;
	u8 i;

	if(current_keycode == KEYCODE_NONE)
	{
        if(SET == bLongKey)
		{
	    	bLongKey = CLEAR;
			key_flag = SET;
			key_data = temp_key_data;
        }
		
		bRepeatKey = CLEAR;
		debounce_cnt = KEYCOUNT_SHORT;
		key_cnt = 0;
		return ; 
	}
	else
	{
		pKeyCode = keycode_table;
	}

	// Find index of key code table
	for(i=0; i< sizeof(keycode_table); i++)
	{
		if(*(pKeyCode+i) == current_keycode)
		{
			break;
		}
	}

	if(i < sizeof(keycode_table))
	{
		if(key_table[i] != temp_key_data)
		{
			temp_key_data = key_table[i];
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
					key_flag = SET;
					bRepeatKey = SET;
					bLongKey = CLEAR;
					key_data = temp_key_data;
				}
				break;

			case KEY_MODE_REPEAT:
				key_data = temp_key_data;
				debounce_cnt = KEYCOUNT_REPEAT;
				if(SET == bRepeatKey)
				{
					if( (key_data != LEFT_KEY) && (key_data != RIGHT_KEY) && (key_data != UP_KEY) && (key_data != DOWN_KEY))
					{
						bRepeatKey = SET;
						key_cnt = 0;

						return;
					}
					debounce_cnt = KEYCOUNT_SHORT;
				}
				key_flag = SET;
				bRepeatKey = SET;
				key_cnt = 0;
				break;

			case KEY_MODE_LONG:
				bLongKey = SET;
				if((key_cnt >= KEYCOUNT_LONG) && (CLEAR == bRepeatKey))
				{
					bRepeatKey = SET;
					bLongKey = CLEAR;
					key_flag = SET;

					if(temp_key_data == KEY_FREEZE)
						key_data = KEY_MENU;
#ifdef __9CH_DEVICE__
					else if(temp_key_data == KEY_9SPLIT)
						key_data = KEY_9SPLIT | KEY_LONG;
#endif //__9CH_DEVICE__
					else
						key_data = temp_key_data;
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
		bLongKey = RESET;
		bRepeatKey = RESET;
		debounce_cnt = KEYCOUNT_SHORT;
		key_cnt = 0;
	}
}

u8 pre_special_mode = LEFT_TOP;
u8 pre_split_mode = 0;
void Key_Proc(void)
{
	if(key_flag)
	{
		key_flag = 0;

		switch(key_data)
		{
			case KEY_FULL_CH1 : 
			case KEY_FULL_CH2 : 
			case KEY_FULL_CH3 : 
			case KEY_FULL_CH4 : 
#ifdef __9CH_DEVICE__
			case KEY_FULL_CH5 : 
			case KEY_FULL_CH6 : 
			case KEY_FULL_CH7 : 
			case KEY_FULL_CH8 : 
			case KEY_FULL_CH9 :
#endif
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
					//if(pre_split_mode > FULL_9) //2015.5.23 �ּ�ó��
					{
						Erase_OSD();
					}
					bFreeze = RESET;
					bAuto_Seq_Flag = RESET;
					bMode_change_flag = SET;
					//InputSelect = VIDEO_SDI_2HD_POP;
					pre_split_mode = sys_status.current_split_mode = key_data-1;
#ifdef __9CH_DEVICE__
					if(key_data == KEY_FULL_CH9)
					{
						Set_border_line();
						aux_display_flag = SET;
						//MDIN3xx_EnableAuxDisplay(&stVideo, ON);
						DEMO_SetPIPViewWIND(0);	// update pip/pop window
					}
					else
#endif
					{
						aux_display_flag = RESET;
						//MDIN3xx_EnableAuxDisplay(&stVideo, OFF);
						//FullScreen(key_data-1); //blocked by Louis
						Set_border_line();
					}
				}
				break;
				
			case KEY_4SPLIT : 
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
					if(pre_split_mode != SPLIT4_1 || bAuto_Seq_Flag || bFreeze)
					{
						Erase_OSD();
					}
					bFreeze = CLEAR;
					bAuto_Seq_Flag = CLEAR;
					bMode_change_flag = SET;
					//InputSelect = VIDEO_SDI_2HD_POP;
					pre_split_mode = sys_status.current_split_mode = SPLIT4_1;
					aux_display_flag = RESET;
					//MDIN3xx_EnableAuxDisplay(&stVideo, OFF); 
#if 0 //Louis
				    SGQ_4CH_INIT(change_mode[cmode]);
#endif
					Set_border_line();
				}
				break;

#ifdef __9CH_DEVICE__ // blocked by kukuri
			case KEY_9SPLIT : 
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
					if(sys_env.b9Split_Mode == 0)
					{
						if(pre_split_mode != SPLIT9_1 || bAuto_Seq_Flag)
						{
							Erase_OSD();
						}
						bFreeze = RESET;
						bAuto_Seq_Flag = RESET;
						bMode_change_flag = SET;
						//InputSelect = VIDEO_SDI_2HD_POP;
						pre_split_mode = sys_status.current_split_mode = SPLIT9_1;
						aux_display_flag = SET;
						//MDIN3xx_EnableAuxDisplay(&stVideo, ON);
#if 0 //Louis
					    SGQ_9CH_INIT(change_mode[cmode]);
#endif
						Set_border_line();
						DEMO_SetPIPViewWIND(0);	// update pip/pop window
					}
					else if((sys_env.b9Split_Mode > 0) && (sys_env.b9Split_Mode < 5))  
					{
						if(pre_split_mode != (SPLIT9_1+sys_env.b9Split_Mode) || bAuto_Seq_Flag)
						{
							Erase_OSD();
						}
						bFreeze = 0;
						bAuto_Seq_Flag = 0;
						bMode_change_flag = 1;
						pre_split_mode = sys_status.current_split_mode = SPLIT9_1+sys_env.b9Split_Mode;
						pre_special_mode = LEFT_TOP+sys_env.b9Split_Mode-1;
#if 0 //Louis
						SGQ_16CH_INIT(change_mode[cmode]);
#endif
						//aux_display_flag = 0;
						aux_display_flag = 1;
						Set_border_line();
						DEMO_SetPIPViewWIND(0);	// update pip/pop window
					}
					else 
					{
						if(pre_split_mode != (SPLIT9_1+sys_env.b9Split_Mode) || bAuto_Seq_Flag)
						{
							Erase_OSD();
						}
						bFreeze = 0;
						bAuto_Seq_Flag = 0;
						bMode_change_flag = 1;
						pre_split_mode = sys_status.current_split_mode = SPLIT9_1+sys_env.b9Split_Mode;
						pre_special_mode = LEFT_TOP+sys_env.b9Split_Mode-1;
#if 0 //Louis
						SGQ_9CH_INIT(change_mode[cmode]);
#endif
						//aux_display_flag = 0;
						aux_display_flag = 1;
						Set_border_line();
						DEMO_SetPIPViewWIND(0);	// update pip/pop window
					}
				}
				break;

			case KEY_9SPLIT + 0x80 : 
				
				Inc_Dec_Count(8,0,1,&sys_env.b9Split_Mode);
				
				if(sys_env.b9Split_Mode == 0)
				{
					if(pre_split_mode != SPLIT9_1 || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bFreeze = 0;
					bAuto_Seq_Flag = 0;
					bMode_change_flag = 1;
					//InputSelect = VIDEO_SDI_2HD_POP;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_1;
					aux_display_flag = 1;
					//MDIN3xx_EnableAuxDisplay(&stVideo, ON);
#if 0 //Louis
				    SGQ_9CH_INIT(change_mode[cmode]);
#endif
					Set_border_line();
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}
				else if((sys_env.b9Split_Mode > 0) && (sys_env.b9Split_Mode < 5))  
				{
					if(pre_split_mode != (SPLIT9_1+sys_env.b9Split_Mode) || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bFreeze = 0;
					bAuto_Seq_Flag = 0;
					bMode_change_flag = 1;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_1+sys_env.b9Split_Mode;
					pre_special_mode = LEFT_TOP+sys_env.b9Split_Mode-1;
#if 0 //Louis
					SGQ_16CH_INIT(change_mode[cmode]);
#endif
					//aux_display_flag = 0;
					aux_display_flag = 1;
					Set_border_line();
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}
				else 
				{
					if(pre_split_mode != (SPLIT9_1+sys_env.b9Split_Mode) || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bFreeze = 0;
					bAuto_Seq_Flag = 0;
					bMode_change_flag = 1;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_1+sys_env.b9Split_Mode;
					pre_special_mode = LEFT_TOP+sys_env.b9Split_Mode-1;
#if 0 //Louis
					SGQ_9CH_INIT(change_mode[cmode]);
#endif
					//aux_display_flag = 0;
					aux_display_flag = 1;
					Set_border_line();
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}

				EEP_buf[cSYSENV_b9Split_Mode] = sys_env.b9Split_Mode;
				write_eeprom_all();

				break;
#endif // __9CH_DEVICE__
			case KEY_FREEZE : 
				bAuto_Seq_Flag = CLEAR;
				if(bFreeze == CLEAR)
				{
					bFreeze = SET;
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 1);	//main freeze On
					// don't need aut display for now Right?....by kukuri
					//MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 1);	//aux freeze On
				}
				else 
				{
					bFreeze = CLEAR;
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 0);	//main freeze Off
					// don't need aut display for now Right?....by kukuri
					//MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 0);	//aux freeze Off
				}
				break;

			case KEY_AUTO_SEQ :
				if(sys_env.bLossAutoSkip)
				{
					if((vVideo_Loss&0x0000000f) == 0x0000000f)  // what the magic number!!!!
						break;
				}

				if(bAuto_Seq_Flag == CLEAR)
				{
					Erase_OSD();
				}
				//bMode_change_flag = SET;
				bFreeze = CLEAR;
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 0);	//main freeze Off
				// don't need aut display for now Right?....by kukuri
				//MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 0);	//aux freeze Off

				Auto_Seq_Init();
				break;

			case KEY_MENU : 
				bAuto_Seq_Flag = CLEAR;
				bFreeze = SET;
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 0);	//main freeze Off
				// don't need aut display for now Right?....by kukuri
				//MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 0);	//aux freeze Off
				Enter_SetUP();
				break;
		}
		pre_key_data = key_data & 0x7f;
	}
}


