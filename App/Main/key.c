//=============================================================================
//  Header Declaration
//=============================================================================
#include <stdio.h>
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================
//u32 key_raw_data = 0;
u16 key_raw_data = 0;
u8 key_stat = STAT_KEY_LONG;
u8 key_flag = 0;
u8 key_data = 0;
u8 pre_key_data = 0;
u16 led_state = 0xffff;
u8 bFreeze = 0;

//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
#ifdef __4CH__
const u8 key_table[] = 
{
    0xfe, // KEY_1
    0xfd, // KEY_2
    0xfb, // KEY_3
    0xf7, // KEY_4
	0xef, // KEY_2x2
	0xdf, // KEY_FRZ // KEY_MENU_ESC
	0xbf, // KEY_SEQ
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


//=============================================================================
//  Function Definition
//=============================================================================

//-----------------------------------------------------------------------------
//  ���� ȭ�� ���¿� ���� Ű������ LED�� � ������ �� ���ΰ��� ó���ϴ� �Լ�
//-----------------------------------------------------------------------------
void Key_LED_Set(void)
{
	static u32 timeout = 0;

//	if(!TIME_AFTER(tick_10ms,timeout))
//		return;
//
//	timeout = tick_10ms + 4; // 10ms * 4 = 40ms

	led_state = 0xffff;
	
	if(bSETUP == 0)
	{
#ifdef __4CH__
		if(sys_status.current_split_mode <= FULL_4) 
		{
			led_state = key_table[sys_status.current_split_mode];
		}	
		else if(sys_status.current_split_mode <= SPLIT4_1)
		{
			led_state = key_table[4];
		}
		
	    if(bFreeze)
			led_state &= key_table[5];
	    if(bAuto_Seq_Flag) 
			led_state &= key_table[6];
#endif
	}
}

#ifdef __4CH__
void Key_Input(void)
{
	static u32 timeout = 0;
	u8 key_temp = 0;

//	if(!TIME_AFTER(tick_10ms,timeout))
//		return;
//
//	timeout = tick_10ms + 2; // 10ms * 2 = 20ms

	KEY_EN_OUT_MODE;
	KEY_EN1_HIGH;
	KEY_EN2_HIGH;

	KEY_LED0EN_LOW;
	KEY_LED1EN_LOW;

	KEY_LED1_5_HIGH;
	KEY_LED2_6_HIGH;
	KEY_LED3_7_HIGH;
	KEY_LED4_HIGH; 
	
	KEY_DATA_INPUT_MODE;
	KEY_EN1_LOW;

	if(KEY_DATA1_5_INPUT)
		key_temp = 0x01;
	else
		key_temp = 0x00;
	if(KEY_DATA2_6_INPUT)
		key_temp |= 0x02;
	else
		key_temp &= 0xfd;
	if(KEY_DATA3_7_INPUT)
		key_temp |= 0x04;
	else
		key_temp &= 0xfb;
	if(KEY_DATA4_INPUT)
		key_temp |= 0x08;
	else
		key_temp &= 0xf7;
	
	//KEY_LED0EN_HIGH;
	KEY_EN1_HIGH;
	KEY_EN2_LOW;
	
	if(KEY_DATA1_5_INPUT)
		key_temp |= 0x10;
	else
		key_temp &= 0xef;
	if(KEY_DATA2_6_INPUT)
		key_temp |= 0x20;
	else
		key_temp &= 0xdf;
	if(KEY_DATA3_7_INPUT)
		key_temp |= 0x40;
	else
		key_temp &= 0xbf;
	if(KEY_DATA4_INPUT)
		key_temp |= 0x80;
	else
		key_temp &= 0x7f;

	KEY_EN2_HIGH;
	KEY_EN_HIZ_MODE;

	key_raw_data = key_temp;	

	KEY_DATA_OUTPUT_MODE;
}

void Key_Led_Ctrl(void)
{
//	static u32 timeout = 0;
	static u8 toggle = 0;
#if 0
	if(!TIME_AFTER(tick_10ms,timeout))
		return;

	timeout = tick_10ms + 2; // 10ms * 2 = 20ms
#endif
	if(toggle == 0)
	{
		toggle = 1;
		KEY_LED0EN_LOW;

		if(led_state & 0x01) 
			KEY_LED1_5_HIGH;
		else
			KEY_LED1_5_LOW;
		if(led_state & 0x02) 
			KEY_LED2_6_HIGH;
		else
			KEY_LED2_6_LOW;
		if(led_state & 0x04)
			KEY_LED3_7_HIGH;
		else
			KEY_LED3_7_LOW;
		if(led_state & 0x08)
			KEY_LED4_HIGH;
		else
			KEY_LED4_LOW;

		KEY_LED1EN_HIGH;

	}
	else //if(toggle == 1)
	{
		toggle = 0;
		KEY_LED1EN_LOW;

		if(led_state & 0x10)
			KEY_LED1_5_HIGH;
		else
			KEY_LED1_5_LOW;
		if(led_state & 0x20)
			KEY_LED2_6_HIGH;
		else
			KEY_LED2_6_LOW;
		if(led_state & 0x40)
			KEY_LED3_7_HIGH;
		else
			KEY_LED3_7_LOW;
		
		KEY_LED0EN_HIGH;
	}
}
#endif

//-----------------------------------------------------------------------------
//	KeyINPUT()�Լ����� ���� ���� ���¿� ���� �ٽ� ó���ϴ� �Լ�	(20ms���� ����)
//-----------------------------------------------------------------------------
void Key_Check(void)	
{
    static u32 timeout = 0;
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

//    if(!TIME_AFTER(tick_10ms,timeout))
//        return;
//
//    timeout = tick_10ms + 2; // 10ms * 2 = 20ms

#ifdef __4CH__
	if(key_raw_data != 0xff)	
	{	
        pKey = key_table;			
        Check_Value = key_raw_data;
        Count = sizeof(key_table);
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
        if(key_stat == STAT_KEY_REPEAT)
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
        else if(key_stat == STAT_KEY_LONG)  
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
        else if((key_stat == STAT_KEY_SHORT))
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

