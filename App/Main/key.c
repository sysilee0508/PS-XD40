//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================
u8 key_raw_data = 0;
key_state_e key_state = KEY_STATE_LONG;
u8 key_flag = 0;
u8 key_data = 0;
u8 pre_key_data = 0;
//u16 led_state = 0xffff;
u8 bFreeze = 0;

//=============================================================================
//  Static Variable Declaration
//=============================================================================
static u8 led_state = 0xff;

//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
#ifdef __4CH__
const keycode_t keycode_table[] =
{
	KEYCODE_CAM1, 	// KEY_1 	//1111 1110 	//254
    KEYCODE_CAM2, 	// KEY_2 	//1111 1101 	//253
	KEYCODE_CAM3, 	// KEY_3 	//1111 1011 	//251
	KEYCODE_CAM4, 	// KEY_4 	//1111 0111 	//247
	KEYCODE_SPLIT, 	// KEY_2x2 	//1110 1111 	//239
	KEYCODE_FREEZE, 	// KEY_FRZ(KEY_MENU_ESC) //1101 1111 //223
	KEYCODE_SEQUENCE, 	// KEY_SEQ 	// 1011 1111 	//191
};

const u8 key_table_index[] = 
{
    0x01, // KEY_1
    0x02, // KEY_2
    0x03, // KEY_3
    0x04, // KEY_4

	0x15, // KEY_2x2
	0x19, // KEY_FRZ
	0x1a, // KEY_SEQ
};
#endif


#define KEYLED_ROW0_EN		KEY_ROW0_LOW; KEY_LED1_HIGH;
#define KEYLED_ROW0_DIS		KEY_ROW0_HIGH; KEY_LED1_LOW;
#define KEYLED_ROW1_EN		KEY_ROW1_LOW; KEY_LED0_HIGH;
#define KEYLED_ROW1_DIS		KEY_ROW1_HIGH; KEY_LED0_LOW;

//=============================================================================
//  Function Definition
//=============================================================================

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
	keycode_t key_code = KEYCODE_NONE;

//	KEY_LED1_5_HIGH;
//	KEY_LED2_6_HIGH;
//	KEY_LED3_7_HIGH;
//	KEY_LED4_HIGH;

	//Scan KROW0
	KEYLED_ROW0_EN;
	KEYLED_ROW1_DIS;
//
//	KEY_ROW0_LOW;
//	KEY_ROW1_HIGH;
//
//	KEY_LED0_LOW;	//pair with KROW1
//	KEY_LED1_HIGH;  //pair with KROW0
	
	KEY_DATA_INPUT_MODE;

	if(LOW == KEY_DATA1_5_INPUT)
		key_code = KEYCODE_CAM1;
	else if(LOW== KEY_DATA2_6_INPUT)
		key_code = KEYCODE_CAM2;
	else if(LOW == KEY_DATA3_7_INPUT)
		key_code = KEYCODE_CAM3;
	else if(LOW == KEY_DATA4_INPUT)
		key_code = KEYCODE_CAM4;

//	KEY_ROW0_HIGH;
//	KEY_ROW1_LOW;
//	KEY_LED0_LOW;
//	KEY_LED1_HIGH;

	//Scan KROW1
	KEYLED_ROW1_EN;
	KEYLED_ROW0_DIS;

	if(LOW == KEY_DATA1_5_INPUT)
		key_code = KEYCODE_SPLIT;
	else if(LOW== KEY_DATA2_6_INPUT)
		key_code = KEYCODE_FREEZE;
	else if(LOW == KEY_DATA3_7_INPUT)
		key_code = KEYCODE_SEQUENCE;

	key_raw_data = key_code;
	
	KEYLED_ROW1_DIS;
	KEYLED_ROW0_DIS;
//
//	//Set LED with active key
//	KEY_DATA_OUTPUT_MODE;
//	if((key_code & 0x0F) != 0x0F)
//	{
//		KEY_LED_ON(key_code);
//		KEY_LED1_HIGH;
//	}
//	else if((key_code & 0xF0) != 0xF0)
//	{
//		KEY_LED_ON((key_code >> 4)|0xF0);
//		KEY_LED0_HIGH;
//	}
	led_state = key_code;
	
	Key_Led_Ctrl(key_code);
}

void Key_Led_Ctrl(keycode_t led)
{
	static u8 stage = KEYLED_STAGE_LEFT;

	KEY_DATA_OUTPUT_MODE;

	KEY_LED1_5_HIGH;
	KEY_LED2_6_HIGH;
	KEY_LED3_7_HIGH;
	KEY_LED4_HIGH;

	if(stage == KEYLED_STAGE_LEFT)
	{
		KEY_LED0_LOW;
		if((led & 0x0F) != 0x0F)
		{
			KEY_LED_ON(led);
		}
		KEY_LED1_HIGH;

		// Change stage for the next time
		stage = KEYLED_STAGE_RIGHT;
	}
	else if(stage == KEYLED_STAGE_RIGHT)
	{
		KEY_LED1_LOW;
		if((led & 0xF0) != 0xF0)
		{
			KEY_LED_ON((led>>4)|0xF0);
			KEY_LED0_HIGH;
		}
		KEY_LED0_HIGH;
		// Change stage for the next time
		stage = KEYLED_STAGE_LEFT;
	}
}
#endif

//-----------------------------------------------------------------------------
//	KeyINPUT()�Լ����� ���� ���� ���¿� ���� �ٽ� ó���ϴ� �Լ�	(20ms���� ����)
//-----------------------------------------------------------------------------
void Key_Check(void)	
{
#ifdef __4CH__
	const u8 *pKey;	
	u8 Check_Value;	
#endif
	u8 Count;	
	u8 i;
	static u8 temp_key_flag = 0;
	static u8 key_repeat_flag = 0;
	static u8 cmp_num = 0;
	static u8 key_cnt = 0;
	static u16 temp_key_data = 0;

#ifdef __4CH__
	if(key_raw_data != KEYCODE_NONE)
	{	
        pKey = keycode_table;
        Check_Value = key_raw_data;
        Count = sizeof(keycode_table);
    }
#endif
	else 
	{
        if(temp_key_flag)
		{
	    	temp_key_flag = 0;
			key_flag = 1;
			key_data = (u8)temp_key_data;			
        }
		
		key_repeat_flag = 0;
		cmp_num = 3;//4;
		key_cnt = 0;
		return ; 
	}


//--------------------------------------
	i = 0;
	while(*pKey != Check_Value)		
	{
		if(Count==0)
		{	
			temp_key_flag = 0;
			key_repeat_flag = 0;
			cmp_num = 3;//4;
			key_cnt = 0;
			return ; 
		}
		pKey++;					
        i++;
		Count--;
	}

	if(key_table_index[i] != temp_key_data)
	{
		temp_key_data = key_table_index[i];
		key_cnt = 0;
		return ;
	}
//--------------------------------------
	key_cnt++;	

	if(key_cnt >= cmp_num)
	{
        // ���� Ű���� REPEAT ON �̸� 
        if(key_state == KEY_STATE_REPEAT)
        {
            key_data = (u8)temp_key_data;	
            cmp_num = 20;
            if(key_repeat_flag)
            {
                if( (key_data != LEFT_KEY) && (key_data != RIGHT_KEY) && (key_data != UP_KEY) && (key_data != DOWN_KEY)) 
                {
                    key_repeat_flag = 1; 
                    key_cnt = 0;
                        
                    return; 
                }
                cmp_num = 3;		
            }
            key_flag = 1;  								
            key_repeat_flag = 1; 				
            key_cnt = 0;

            //printf("REPEAT : 0x%x\r\n",key_data);
        }

        
        // ���� Ű���� LONG ON �̸�  
        else if(key_state == KEY_STATE_LONG)
		{
    		temp_key_flag = 1;
           	if(key_cnt >= 35)
            {
                if(!key_repeat_flag)  
        		{		
            		key_repeat_flag = 1; 
                    temp_key_flag = 0; 	
                    key_flag = 1; 
                    //key_data = (u8)temp_key_data + 0x80;
					//printf("LONG   : 0x%x\r\n",key_data);

					if(temp_key_data == KEY_FREEZE) key_data = KEY_MENU;     
					else if(temp_key_data == KEY_9SPLIT) key_data = KEY_9SPLIT+0x80;
					else key_data = (u8)temp_key_data;
                }
            }
        }
        
        // ���� Ű���� SHORT ON �̸�  
        else if((key_state == KEY_STATE_SHORT))
        {
            if(!key_repeat_flag)  
            {
                key_flag = 1; 
                key_repeat_flag = 1; 
                temp_key_flag = 0; 	
                key_data = (u8)temp_key_data;
            }
        }
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
			case KEY_FULL_CH5 : 
			case KEY_FULL_CH6 : 
			case KEY_FULL_CH7 : 
			case KEY_FULL_CH8 : 
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
					//if(pre_split_mode > FULL_9) //2015.5.23 �ּ�ó��
					{
						Erase_OSD();
					}
					bFreeze = 0;
					bAuto_Seq_Flag = 0;
					bMode_change_flag = 1;
					//InputSelect = VIDEO_SDI_2HD_POP;
					pre_split_mode = sys_status.current_split_mode = key_data-1;
					aux_display_flag = 0;
					//MDIN3xx_EnableAuxDisplay(&stVideo, OFF); 
#if 0 //Louis
					FullScreen(key_data-1);
#endif
					Set_border_line();
				}
				break;

			case KEY_FULL_CH9 : 
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
					//if(pre_split_mode > FULL_9) //2015.5.23 �ּ�ó��
					{
						Erase_OSD();
					}
					bFreeze = 0;
					bAuto_Seq_Flag = 0;
					bMode_change_flag = 1;
					//InputSelect = VIDEO_SDI_2HD_POP;
					pre_split_mode = sys_status.current_split_mode = FULL_9;
					Set_border_line();
					aux_display_flag = 1;
					//MDIN3xx_EnableAuxDisplay(&stVideo, ON);
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}
				break;
				
			case KEY_4SPLIT : 
#ifdef __4CH__				
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
#endif	
				{
#ifdef __4CH__				
					if(pre_split_mode != SPLIT4_1 || bAuto_Seq_Flag || bFreeze)
#endif
					{
						Erase_OSD();
					}
					bFreeze = 0;
					bAuto_Seq_Flag = 0;
					bMode_change_flag = 1;
					//InputSelect = VIDEO_SDI_2HD_POP;
#ifdef __4CH__				
					pre_split_mode = sys_status.current_split_mode = SPLIT4_1;
#endif
					aux_display_flag = 0;
					//MDIN3xx_EnableAuxDisplay(&stVideo, OFF); 
#if 0 //Louis
				    SGQ_4CH_INIT(change_mode[cmode]);
#endif
					Set_border_line();
				}
				break;

			case KEY_9SPLIT : 
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
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

			case KEY_FREEZE : 
				bAuto_Seq_Flag = 0;
				if(bFreeze == 0)	
				{
					bFreeze = 1;
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 1);	//main freeze On
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 1);	//aux freeze On
				}
				else 
				{
					bFreeze = 0;
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 0);	//main freeze Off
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 0);	//aux freeze Off
				}
				break;

			case KEY_SEQ : 
				if(sys_env.bLossAutoSkip)
				{
#ifdef __4CH__
					if((vVideo_Loss&0x0000000f) == 0x0000000f) break; 
#endif
				}

				if(bAuto_Seq_Flag == 0) Erase_OSD();
				//bMode_change_flag = 1;
				bFreeze = 0;
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 0);	//main freeze Off
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 0);	//aux freeze Off

				Auto_Seq_Init();
				break;

			case KEY_MENU : 
				bAuto_Seq_Flag = 0;
				bFreeze = 0;
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x040, 1, 1, 0);	//main freeze Off
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x142, 1, 1, 0);	//aux freeze Off
				SetUP();				
				break;
		}

		pre_key_data = key_data & 0x7f;
	}
}

