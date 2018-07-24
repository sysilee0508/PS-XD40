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

//u32 led_state = 0xffffffff;
u16 led_state = 0xffff;

u8 bFreeze = 0;


//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
/*const u32 key_table[] = 
{
    0xfdffffff, // KEY_1
    0xfeffffff, // KEY_2
    0xff7fffff, // KEY_3
    0xffbfffff, // KEY_4
    0xffdfffff, // KEY_5
    0xffefffff, // KEY_6
    0xfff7ffff, // KEY_7
    0xfffbffff, // KEY_8
	0xfffdffff, // KEY_9

	0xffff7fff, // KEY_2x2
	0xffffbfff, // KEY_3x3
	0xffffdfff, // KEY_FRZ
	0xffffefff, // KEY_SEQ

	0xfffffdff, // KEY_MENU_ESC
};

const u8 key_table_index[] = 
{
    0x01, // KEY_1
    0x02, // KEY_2
    0x03, // KEY_3
    0x04, // KEY_4
    0x05, // KEY_5
    0x06, // KEY_6
    0x07, // KEY_7
    0x08, // KEY_8
    0x09, // KEY_9

	0x15, // KEY_2x2
	0x16, // KEY_3x3
	0x19, // KEY_FRZ
	0x1a, // KEY_SEQ

	0x1c, // KEY_MENU_ESC
};*/

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

#ifdef __9CH__
const u16 key_table[] = 
{
    0xfffd, // KEY_1
    0xfffb, // KEY_2
    0xfff7, // KEY_3
    0xffef, // KEY_4
    0xffdf, // KEY_5
    0xffbf, // KEY_6
    0xff7f, // KEY_7
    0xfffe, // KEY_8
	0xfeff, // KEY_9

	0xfdff, // KEY_2x2
	0xfbff, // KEY_FRZ // KEY_MENU_ESC
	0xf7ff, // KEY_SEQ
	0xefff, // KEY_3x3
};

const u8 key_table_index[] = 
{
    0x01, // KEY_1
    0x02, // KEY_2
    0x03, // KEY_3
    0x04, // KEY_4
    0x05, // KEY_5
    0x06, // KEY_6
    0x07, // KEY_7
    0x08, // KEY_8
    0x09, // KEY_9

	0x15, // KEY_2x2
	0x19, // KEY_FRZ
	0x1a, // KEY_SEQ
	0x16, // KEY_3x3
};
#endif


//=============================================================================
//  Function Definition
//=============================================================================
//-----------------------------------------------------------------------------
//  리모컨 입력 처리 함수 
//  MCU 포트 PC12를 통해 입력된 리모컨 신호를 
//  외부 인터럽트로 처리로 처리함.
//-----------------------------------------------------------------------------
/*void EXTI9_5_IRQHandler(void)
{
	#define PRE_LEAD	0 
	#define LEAD	    1 
	#define CODE	    2	

	static u8 data[4];
	static u8 bit_count = 0;
	static u8 state = PRE_LEAD;
	static u8 temp = 0;
	//	static u8 vDataCode = 0;
	static u8 OldDataCode = 0;
	static u8 PreRepeatCnt = 0;
//	static u8 long_key_flag = 0;

    if(EXTI_GetITStatus(EXTI_Line7) != RESET) 
	{
        EXTI_ClearITPendingBit(EXTI_Line7);

		switch(state)
	    { 
			case PRE_LEAD :   
	        	count_100us = 0;	
				state = LEAD;
				break;

			case LEAD :   
	        	if((count_100us >= 130) && (count_100us < 140))  // between falling edge (9.5ms+4.5ms)
	            {
					state = CODE;                                 // then, lead code!!!
	                bit_count = 0;   
	                temp = 0;
	                PreRepeatCnt = 0;
	            }
				else if((count_100us >= 105) && (count_100us < 125))
				{
					state = PRE_LEAD; 

					if(PreRepeatCnt > 4)
					{
						if( (OldDataCode >= 0x0F) && (OldDataCode <= 0x12) ) 
						{
							key_data = OldDataCode;
				
							key_flag = 1;
							//printf("IRDA = 0x%02X  -REPEAT\n",vKEY_VAL);
						}
						//else if(OldDataCode == ENTER_KEY) 
						//{
						//	if(bSETUP && (vPAGE == 7) && bSUB_ENTER_M1 && (!long_key_flag)) 
						//	{
						//		vKEY_VAL = OldDataCode+0x80;
					    //
						//		bKEY_FLAG = 1;
						//		long_key_flag = 1;
						//	}
						//}
					}
					else PreRepeatCnt++; 
				}
				break;
		
			case CODE : 											  
	        	if((count_100us < 15));                            // dummy time
	           	else if((count_100us >= 20) && (count_100us < 25)) // between falling edge (2ms~2.5ms)
			   		temp |= 0x80;                                  // then, code is one(1)!!!
			   	else  
				{ 
					state = LEAD;
	                break;
	            }
	            
			   	bit_count++;

	           	if(!(bit_count % 8))							 
				{ 
					data[(bit_count / 8) - 1] = temp; 
	                temp = 0;

	                if(bit_count >= 32)
				    { 
						//state = lead; 
						state = PRE_LEAD; 
	                    bit_count = 0;
		
						//----------------------
						if((data[2]+data[3]) != 0xff ) return; 
						//----------------------
						key_data = data[2];

						if( key_data <= 0x19 ) 
						{
							key_flag = 1;
							//printf("IRDA = 0x%02X \n",key_data);

							OldDataCode = key_data;
							//long_key_flag = 0;
						}
	                }
	            }

	            temp = temp >> 1;
	            break;

			default: break;
	    }

	    count_100us = 0;
    }
}
*/



//-----------------------------------------------------------------------------
//  현재 화면 상태에 따라 키보드의 LED를 어떤 것으로 켤 것인가를 처리하는 함수
//-----------------------------------------------------------------------------
void Key_LED_Set(void)
{
    static u32 timeout = 0;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 4; // 10ms * 4 = 40ms

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
		
	    if(bFreeze) led_state &= key_table[5];
	    if(bAuto_Seq_Flag) led_state &= key_table[6];
#endif

#ifdef __9CH__
		if(sys_status.current_split_mode <= FULL_9) 
		{
			led_state = key_table[sys_status.current_split_mode];
		}	
		else if(sys_status.current_split_mode <= SPLIT4_2)
		{
			led_state = key_table[9];
		}
		else if(sys_status.current_split_mode >= SPLIT9_1)
		{
			led_state = key_table[12];
		}
		/*else if(bSETUP) 
		{
			led_state = key_table[13]; 
		}*/
		
	    if(bFreeze) led_state &= key_table[10];
	    if(bAuto_Seq_Flag) led_state &= key_table[11];
#endif
	}
}


//-----------------------------------------------------------------------------
//	키보드의 상태를 스캔하고 LED를 켜는 함수	(20ms마다 실행)
//-----------------------------------------------------------------------------
/*void Key_Input(void)
{
    static u32 timeout = 0;
	unsigned char i;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 2; // 10ms * 2 = 20ms

	
	//--------------------------------------------------
	//  키보드 상태를 스캔
	//--------------------------------------------------
	KEY_CS_LOW;	
	KEY_CLK_HIGH;
	

	//...HC164에 0x7fffff(01111111 11111111 11111111B)를 넣는다.
	KEY_MOSI_HIGH;	
	for(i=0;i<23;i++)
	{
		KEY_CLK_LOW;		
		//NOP();
		KEY_CLK_HIGH;	
	}

	KEY_MOSI_LOW;		
	KEY_CLK_LOW;			
	//NOP();
	KEY_CLK_HIGH;		


	key_raw_data = 0xffffffff;

    //...HC164의 각 비트의 상태를 읽어 온다. 
	KEY_MOSI_HIGH;		
    for(i=0;i<24;i++)
	{
		key_raw_data = key_raw_data >> 1;

		if(KEY_MISO_INPUT != 0) key_raw_data |= 0x80000000; //MISO          
		else key_raw_data &= 0x7fffffff;

		KEY_CLK_LOW;		    
		//NOP();
		KEY_CLK_HIGH;		
	}

//    printf("key scan = 0x%8x\r\n",key_raw_data);


	//--------------------------------------------------
	//  키보드의 LED를 켬
	//--------------------------------------------------
  
	for(i=0;i<24;i++)
	{
        if((led_state << i) & 0x80000000) KEY_MOSI_HIGH;  
		else KEY_MOSI_LOW;      

		KEY_CLK_LOW;		    
		//NOP();
		KEY_CLK_HIGH;		
	}

	KEY_CS_HIGH;  
}
*/
#ifdef __4CH__
void Key_Input(void)
{
    static u32 timeout = 0;
	u8 key_temp = 0;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 2; // 10ms * 2 = 20ms

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

	if(KEY_DATA1_5_INPUT) key_temp = 0x01;
	else key_temp = 0x00;
	if(KEY_DATA2_6_INPUT) key_temp |= 0x02;
	else key_temp &= 0xfd;
	if(KEY_DATA3_7_INPUT) key_temp |= 0x04;
	else key_temp &= 0xfb;
	if(KEY_DATA4_INPUT) key_temp |= 0x08;
	else key_temp &= 0xf7;
	
	//KEY_LED0EN_HIGH;
	KEY_EN1_HIGH;
	KEY_EN2_LOW;
	
	if(KEY_DATA1_5_INPUT) key_temp |= 0x10;
	else key_temp &= 0xef;
	if(KEY_DATA2_6_INPUT) key_temp |= 0x20;
	else key_temp &= 0xdf;
	if(KEY_DATA3_7_INPUT) key_temp |= 0x40;
	else key_temp &= 0xbf;
	if(KEY_DATA4_INPUT) key_temp |= 0x80;
	else key_temp &= 0x7f;

	KEY_EN2_HIGH;
	KEY_EN_HIZ_MODE;

	key_raw_data = key_temp;	
	//printf("key_temp = 0x%x\r\n",key_temp);	


	KEY_DATA_OUTPUT_MODE;

/*	if((sys_status.current_split_mode <= FULL_4) && (!bFreeze) && (!bAuto_Seq_Flag)) 
	{
		if(led_state & 0x01) KEY_LED1_5_HIGH;
		else  KEY_LED1_5_LOW;
		if(led_state & 0x02) KEY_LED2_6_HIGH;
		else  KEY_LED2_6_LOW;
		if(led_state & 0x04) KEY_LED3_7_HIGH;
		else  KEY_LED3_7_LOW;
		if(led_state & 0x08) KEY_LED4_HIGH;
		else  KEY_LED4_LOW;

		KEY_LED1EN_HIGH;
	}	
	else
	{
		if(led_state & 0x10) KEY_LED1_5_HIGH;
		else  KEY_LED1_5_LOW;
		if(led_state & 0x20) KEY_LED2_6_HIGH;
		else  KEY_LED2_6_LOW;
		if(led_state & 0x40) KEY_LED3_7_HIGH;
		else  KEY_LED3_7_LOW;

		KEY_LED2EN_HIGH;
	}
*/
}

void Key_Led_Ctrl(void)
{
	static u8 toggle = 0;

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

#ifdef __9CH__
void Key_Input(void)
{
    static u32 timeout = 0;
	unsigned char i;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 2; // 10ms * 2 = 20ms

	
	//--------------------------------------------------
	//  키보드 상태를 스캔
	//--------------------------------------------------
	KEY_CS_LOW;	
	KEY_CLK_HIGH;
	

	//...HC164에 0x7fffff(01111111 11111111 11111111B)를 넣는다.
	KEY_MOSI_HIGH;	
	for(i=0;i<15;i++)
	{
		KEY_CLK_LOW;		
		//NOP();
		KEY_CLK_HIGH;	
	}

	KEY_MOSI_LOW;		
	KEY_CLK_LOW;			
	//NOP();
	KEY_CLK_HIGH;		


	key_raw_data = 0xffff;

    //...HC164의 각 비트의 상태를 읽어 온다. 
	KEY_MOSI_HIGH;		
    for(i=0;i<16;i++)
	{
		key_raw_data = key_raw_data >> 1;

		if(KEY_MISO_INPUT != 0) key_raw_data |= 0x8000; //MISO          
		else key_raw_data &= 0x7fff;

		KEY_CLK_LOW;		    
		//NOP();
		KEY_CLK_HIGH;		
	}

//    printf("key scan = 0x%4x\r\n",key_raw_data);


	//--------------------------------------------------
	//  키보드의 LED를 켬
	//--------------------------------------------------
  
	for(i=0;i<16;i++)
	{
        if((led_state << i) & 0x8000) KEY_MOSI_HIGH;  
		else KEY_MOSI_LOW;      

		KEY_CLK_LOW;		    
		//NOP();
		KEY_CLK_HIGH;		
	}

	KEY_CS_HIGH;  
}
#endif

//-----------------------------------------------------------------------------
//	KeyINPUT()함수에서 읽은 값을 상태에 따라 다시 처리하는 함수	(20ms마다 실행)
//-----------------------------------------------------------------------------
void Key_Check(void)	
{
    static u32 timeout = 0;
#ifdef __4CH__
	const u8 *pKey;	
	u8 Check_Value;	
#endif
#ifdef __9CH__
//	const u32 *pKey;	
	const u16 *pKey;	
//	u32 Check_Value;	
	u16 Check_Value;	
#endif
	u8 Count;	
	u8 i;
	static u8 temp_key_flag = 0;
	static u8 key_repeat_flag = 0;
	static u8 cmp_num = 0;
	static u8 key_cnt = 0;
//	static u32 temp_key_data = 0;
	static u16 temp_key_data = 0;


    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 2; // 10ms * 2 = 20ms

#ifdef __4CH__
	if(key_raw_data != 0xff)	
	{	
        pKey = key_table;			
        Check_Value = key_raw_data;
        Count = sizeof(key_table);
    }
#endif

//	if(key_raw_data != 0xffffffff)	
#ifdef __9CH__
	if(key_raw_data != 0xffff)	
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

			//printf("LONG  : 0x%x\r\n",key_data);
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
        // 현재 키값이 REPEAT ON 이면 
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

        
        // 현재 키값이 LONG ON 이면  
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
        
        // 현재 키값이 SHORT ON 이면  
        else if((key_stat == STAT_KEY_SHORT))
        {
            if(!key_repeat_flag)  
            {
                key_flag = 1; 
                key_repeat_flag = 1; 
                temp_key_flag = 0; 	
                key_data = (u8)temp_key_data;
                
                //printf("SHORT  : 0x%x\r\n",key_data);
            }
        }
    }

//    if(key_data == KEY_1) key_stat = 0;     	// test
//    else if(key_data == KEY_2) key_stat = 1;	// test    
//    else if(key_data == KEY_3) key_stat = 2; // test     
}




u16 dump_buf[0x400];

void HexDump(u16 *addr, unsigned long len)
{
 	u16	*s=addr; 
	u16	*endPtr=(u16 *)((unsigned long)addr+(len*2));
 	unsigned long	i, remainder=len%16;
 		
 	printf("         00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F \n");

 	// print out 16 byte blocks.
 	while (s+16<=endPtr)
	{
 		// offset 출력.
 		printf("0x%04lx  ", (long)(s-addr));
 		
 		// 16 words 단위로 내용 출력.
 		for (i=0; i<16; i++)
		{
 			printf("%04x ", s[i]);
 		}
/* 		printf(" ");
 		
 		for (i=0; i<16; i++)
		{
 			if (s[i]>=32 && s[i]<=125)printf("%c", s[i]);
 			else printf(".");
 		}
*/ 		s += 16;
 		printf("\n");
 	}
 	
 	// Print out remainder.
 	if (remainder)
	{
 		// offset 출력.
 		printf("0x%04lx  ", (long)(s-addr));
 		
 		// 16 words 단위로 출력하고 남은 것 출력.
 		for (i=0; i<remainder; i++)
		{
 			printf("%04x ", s[i]);
 		}
/* 		for (i=0; i<(16-remainder); i++)
		{
 			printf("   ");
 		}
 
 		printf(" ");
 		for (i=0; i<remainder; i++)
		{
 			if (s[i]>=32 && s[i]<=125) printf("%c", s[i]);
 			else	printf(".");
 		}
 		for (i=0; i<(16-remainder); i++){
 			printf(" ");
 		}
 		printf("\n");
*/	}
 	return;
}	// HexDump.


u8 pre_special_mode = LEFT_TOP;
u8 pre_split_mode = 0;
void Key_Proc(void)
{
	int i;
	
	if(key_flag)
	{
		key_flag = 0;

		switch(key_data)
		{
/*			case KEY_FULL_CH1 : 
					if(pre_split_mode != SPLIT9_2 || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bMode_change_flag = 1;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_2;
					pre_special_mode = LEFT_TOP;
					demoSpecialSplit(0xff,LEFT_TOP); 
					aux_display_flag = 0;
					Set_border_line();
#ifdef __9CH__
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
#endif
			break;
			
			case KEY_FULL_CH2 : 
					if(pre_split_mode != SPLIT9_3 || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bMode_change_flag = 1;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_3;
					pre_special_mode = RIGHT_TOP;
					demoSpecialSplit(0xff,RIGHT_TOP); 
					aux_display_flag = 0;
					Set_border_line();
#ifdef __9CH__
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
#endif
			break;

			case KEY_FULL_CH3 : 
					if(pre_split_mode != SPLIT9_4 || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bMode_change_flag = 1;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_4;
					pre_special_mode = LEFT_BOT;
					demoSpecialSplit(0xff,LEFT_BOT); 
					aux_display_flag = 0;
					Set_border_line();
#ifdef __9CH__
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
#endif
			break;

			case KEY_FULL_CH4 : 
					if(pre_split_mode != SPLIT9_5 || bAuto_Seq_Flag)
					{
						Erase_OSD();
					}
					bMode_change_flag = 1;
					pre_split_mode = sys_status.current_split_mode = SPLIT9_5;
					pre_special_mode = RIGHT_BOT;
					demoSpecialSplit(0xff,RIGHT_BOT); 
					aux_display_flag = 0;
					Set_border_line();
#ifdef __9CH__
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
#endif
			break;
*/
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
					//if(pre_split_mode > FULL_9) //2015.5.23 주석처리
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
#ifdef __9CH__
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
#endif
				}
				break;

			case KEY_FULL_CH9 : 
				if(pre_key_data != key_data /*|| SDIRX_change_flag	Louis block*/)
				{
					//if(pre_split_mode > FULL_9) //2015.5.23 주석처리
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
#ifdef __9CH__				
					//if(pre_split_mode != SPLIT4_1 && pre_split_mode != SPLIT4_2) //2015.5.23 주석처리
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
#ifdef __9CH__				
					if(sys_status.current_split_mode == SPLIT4_1)
					{
						pre_split_mode = sys_status.current_split_mode = SPLIT4_2;
					}
					else 
					{
						pre_split_mode = sys_status.current_split_mode = SPLIT4_1;
					}
#endif
					aux_display_flag = 0;
					//MDIN3xx_EnableAuxDisplay(&stVideo, OFF); 
#if 0 //Louis
				    SGQ_4CH_INIT(change_mode[cmode]);
#endif
					Set_border_line();
#ifdef __9CH__
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
#endif
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

/*				memset(dump_buf, 0, sizeof(dump_buf));		
				for(i=0;i<=0xff;i++)
				{
					MDINHIF_RegRead(MDIN_HOST_ID, i, &dump_buf[i]);
					//printf("dump_buf[%d] = 0x%x\r\n",i,dump_buf[i]);
				}
				HexDump(dump_buf, 0x0ff+1);

				printf("\n\r\n\r\n\r");
				
				memset(dump_buf, 0, sizeof(dump_buf));		
				for(i=0;i<=0x3ff;i++)
				{
					MDINHIF_RegRead(MDIN_LOCAL_ID, i, &dump_buf[i]);
					//printf("dump_buf[%d] = 0x%x\r\n",i,dump_buf[i]);
				}
				HexDump(dump_buf, 0x3ff+1);

				break;
*/
			case KEY_SEQ : 
				if(sys_env.bLossAutoSkip)
				{
#ifdef __4CH__
					if((vVideo_Loss&0x0000000f) == 0x0000000f) break; 
#endif

#ifdef __9CH__
					if((vVideo_Loss&0x000001ff) == 0x000001ff) break; 
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

