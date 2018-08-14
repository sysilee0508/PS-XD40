//*****************************************************************************
// 
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include "common.h"

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
sys_stat_t sys_status;
sys_env_t sys_env;



volatile BOOL fUSBXferMode, fZOOMMove, fCROPMove;
s8 Video1_In_Res_Val = 0x00;
s8 Video2_In_Res_Val = 0x00;
s8 Video_Out_Res_Val = VIDOUT_1920x1080p60;

u8 SDI_IN_Display_Flag = 0;

u8 PIP_mode = 5;
u8 Pre_PIP_mode = 0xff;

BYTE sysenv_split_mode = 0;

u8 aux_display_flag = 0;


const unsigned char change_mode[4] = {0x4, 0x2, 0x7, 0x4};
int cmode = 0;


// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
extern BYTE fMenuUpdate;			  				//by hungry 2012.03.06
//extern BYTE Key1stCode;		   						//by hungry 2012.03.15
//extern BOOL fButtonScan;   							//by hungry 2012.03.15


u32 tick_10ms = 0;

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 1ms
//-----------------------------------------------------------------------------
void TIM2_IRQHandler(void)
{
	static unsigned int cnt;
	
    TIM2->SR = TIM2->SR & 0xFFFE;			// clear TIM2 update interrupt flag


	cnt++;
	if(cnt > 10)
	{
		cnt = 0;
		tick_10ms++;
	}
//
//#ifdef __4CH__
//	Key_Led_Ctrl();
//#endif
}

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 20ms - This is to handle keys and LEDs
//-----------------------------------------------------------------------------
void TIM3_IRQHandler(void)
{
	static unsigned int cnt;

    TIM3->SR = TIM3->SR & 0xFFFE;			// clear TIM2 update interrupt flag

	Key_Scan();
	Key_Check();
//	Key_LED_Set();
#ifdef __4CH__
//	Key_Led_Ctrl();
#endif
}

//-----------------------------------------------------------------------------
//	USART3 Rx Interrupt
//-----------------------------------------------------------------------------
#ifdef __4CH__
u8 rs232_key_table[] =
{	
	0x00,	//0x00
	0x80,	//0x01
	0x81,	//0x02
	0x82,	//0x03
	0x83,	//0x04
	0x00,	//0x05
	0x00,	//0x06
	0x00,	//0x07
	0x00,	//0x08
	0x00,	//0x09	
	0x00,	//0x0a	
	0x00,	//0x0b	
	0x00,	//0x0c	
	0x00,	//0x0d	
	0x00,	//0x0e	
	0x00,	//0x0f	
	0x00,	//0x10	
	0x00,	//0x11	
	0x00,	//0x12	
	0x00,	//0x13	
	0x00,	//0x14	
	0x84,	//0x15	
	0x00,	//0x16	
	0x00,	//0x17	
	0x00,	//0x18	
	0x9a,	//0x19	
	0x9b,	//0x1a	
	0x00,	//0x1b	
	0x8a,	//0x1c	
};	
#endif

void USART3_IRQHandler(void)
{
	static u8 bSOH_FLAG = 0;
	static u8 bHEADER_FLAG = 0;
	static u8 bSTX_FLAG = 0;
	static u8 bETX_FLAG = 0;
	u8 Data;
	u8 i;

	Data = USART_ReceiveData(USART3);

    //if (USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
    {
        // Clear the USART3 RX interrupt
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);

		if(sys_env.vREMOCON_ID != 0)
		{	
	 		if(!bSOH_FLAG)
			{
	   		  	if(Data == 1) 
					bSOH_FLAG = 1;
				else 
				{
					bSOH_FLAG = 0;
					bHEADER_FLAG = 0;
					bSTX_FLAG = 0;
					bETX_FLAG = 0;
				}
				return;
			}
			else if(!bHEADER_FLAG)  
			{
				if(Data == sys_env.vREMOCON_ID)
					bHEADER_FLAG = 1;
				else 
				{
					bSOH_FLAG = 0;
					bHEADER_FLAG = 0;
					bSTX_FLAG = 0;
					bETX_FLAG = 0;
				}
				return;
			}
		}

		if(!bSTX_FLAG)
		{
			if(Data == 2)
				bSTX_FLAG = 1;
			else 
			{
				bSOH_FLAG = 0;
				bHEADER_FLAG = 0;
				bSTX_FLAG = 0;
				bETX_FLAG = 0;
			}
		}
		else if(bETX_FLAG)
		{
			if(Data == 3) key_flag = 1;
			bSOH_FLAG = 0;
			bHEADER_FLAG = 0;
			bSTX_FLAG = 0;
			bETX_FLAG = 0;
		}
		else 
		{
			bETX_FLAG = 1;
#ifdef __4CH__
			if((Data >= 0x90) && (Data != 0x9a) && (Data != 0x9b))
				Data -= 0x10;
			if(bSETUP)
			{
				if(Data == 0x8a)
					Data = 0x9a; //KEY_MENU -> KEY_FREEZE
			}
#endif
			for(i = 0; i < sizeof(rs232_key_table); i++) 
			{
				if(rs232_key_table[i] == Data)
				{
					if(i == 0x1d)
						key_data = KEY_9SPLIT+0x80;
					else
						key_data = i;
					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//	Process RTC Interrupt
//-----------------------------------------------------------------------------
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		// Clear the RTC Second interrupt
		RTC_ClearITPendingBit(RTC_IT_SEC);

		rtc_sec_update_flag = 1;

		// Wait until last write operation on RTC registers has finished 
		RTC_WaitForLastTask();
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Load_Data(void)
{
	BYTE i;

	read_eeprom_all();

	sys_env.vCORRECT_OFFSET = EEP_buf[cSYSENV_vCORRECT_OFFSET];
	sys_env.bVECTOR = EEP_buf[cSYSENV_bVECTOR];
	sys_env.bCORRECT = EEP_buf[cSYSENV_bCORRECT];
	sys_env.vDATE_FORMAT = EEP_buf[cSYSENV_vDATE_FORMAT];
	sys_env.bTIME_ON = EEP_buf[cSYSENV_bTIME_ON];
	sys_env.vTIME_Position = EEP_buf[cSYSENV_vTIME_Position];

	for(i = 0; i < 9; i++)
	{
		sys_env.vCH_NAME[i][0] = EEP_buf[cSYSENV_vCH_NAME+0+(i*12)];
		sys_env.vCH_NAME[i][1] = EEP_buf[cSYSENV_vCH_NAME+1+(i*12)];
		sys_env.vCH_NAME[i][2] = EEP_buf[cSYSENV_vCH_NAME+2+(i*12)];
		sys_env.vCH_NAME[i][3] = EEP_buf[cSYSENV_vCH_NAME+3+(i*12)];
		sys_env.vCH_NAME[i][4] = EEP_buf[cSYSENV_vCH_NAME+4+(i*12)];
		sys_env.vCH_NAME[i][5] = EEP_buf[cSYSENV_vCH_NAME+5+(i*12)];
		sys_env.vCH_NAME[i][6] = EEP_buf[cSYSENV_vCH_NAME+6+(i*12)];
		sys_env.vCH_NAME[i][7] = EEP_buf[cSYSENV_vCH_NAME+7+(i*12)];
		sys_env.vCH_NAME[i][8] = EEP_buf[cSYSENV_vCH_NAME+8+(i*12)];
		sys_env.vCH_NAME[i][9] = EEP_buf[cSYSENV_vCH_NAME+9+(i*12)];
		sys_env.vCH_NAME[i][10] = EEP_buf[cSYSENV_vCH_NAME+10+(i*12)];
		sys_env.vCH_NAME[i][11] = EEP_buf[cSYSENV_vCH_NAME+11+(i*12)];
	}	

	sys_env.bTITLE_ON = EEP_buf[cSYSENV_bTITLE_ON];

	for(i=0; i<4; i++)
	{
		sys_env.vDWELL[i] = EEP_buf[cSYSENV_vDWELL+i];
	}	

	sys_env.bLossAutoSkip = EEP_buf[cSYSENV_bLossAutoSkip];
	sys_env.bOSD_Display = EEP_buf[cSYSENV_bOSD_Display];
	sys_env.vOSD_Position = EEP_buf[cSYSENV_vOSD_Position];
	sys_env.border_line = EEP_buf[cSYSENV_border_line];
	//sys_env.vAlarm = (WORD)(EEP_buf[cSYSENV_vAlarm+1]<<8) | (WORD)(EEP_buf[cSYSENV_vAlarm]);
	//sys_env.vAlarm_Display_Time = EEP_buf[cSYSENV_vAlarm_Display_Time];
	sys_env.vREMOCON_ID = EEP_buf[cSYSENV_vREMOCON_ID];
	sys_env.vLoss_Time = EEP_buf[cSYSENV_vLoss_Time];
	sys_env.vLoss_Display = EEP_buf[cSYSENV_vLoss_Display];

	sys_env.vResolution = EEP_buf[cSYSENV_resolution];
	sys_env.baud_rate = EEP_buf[cSYSENV_baud_rate];
	sys_env.b9Split_Mode = EEP_buf[cSYSENV_b9Split_Mode];
}

void Data_Load(void)
{
	BYTE i;

#ifdef __4CH__
	if(EEP_buf[cEEP_CHK] == 0xa5)Load_Data();
#endif
	else 
	{
		memset(EEP_buf, 0, 2048);

		EEP_buf[cSYSENV_vCORRECT_OFFSET] = 0;
		EEP_buf[cSYSENV_bVECTOR] = 1;
		EEP_buf[cSYSENV_bCORRECT] = 0;
		EEP_buf[cSYSENV_vDATE_FORMAT] = 0;
		EEP_buf[cSYSENV_bTIME_ON] = 1;
		EEP_buf[cSYSENV_vTIME_Position] = 1;
		
		for(i=0; i<9; i++)
		{
			EEP_buf[cSYSENV_vCH_NAME+0+(i*12)] = 'C';
			EEP_buf[cSYSENV_vCH_NAME+1+(i*12)] = 'A';
			EEP_buf[cSYSENV_vCH_NAME+2+(i*12)] = 'M';
			EEP_buf[cSYSENV_vCH_NAME+3+(i*12)] = '1'+i;
			EEP_buf[cSYSENV_vCH_NAME+4+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+5+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+6+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+7+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+8+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+9+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+10+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+11+(i*12)] = ' ';
		}

		EEP_buf[cSYSENV_bTITLE_ON] = 1;

		for(i=0; i<4; i++)
		{
			EEP_buf[cSYSENV_vDWELL+i] = 3;
		}

		EEP_buf[cSYSENV_bLossAutoSkip] = 1;
		EEP_buf[cSYSENV_bOSD_Display] = 1;
#ifdef __4CH__
		EEP_buf[cSYSENV_vOSD_Position] = 6;
#endif

		EEP_buf[cSYSENV_border_line] = 1;
		//EEP_buf[cSYSENV_vAlarm] = 0;
		//EEP_buf[cSYSENV_vAlarm+1] = 0;
		//EEP_buf[cSYSENV_vAlarm_Display_Time] = 0;
		EEP_buf[cSYSENV_vREMOCON_ID] = 0;
		EEP_buf[cSYSENV_vLoss_Time] = 0;
		EEP_buf[cSYSENV_vLoss_Display] = 0;

		EEP_buf[cSYSENV_resolution] = 0;
		EEP_buf[cSYSENV_baud_rate] = 3;
		EEP_buf[cSYSENV_b9Split_Mode] = 0;

#ifdef __4CH__
		EEP_buf[cEEP_CHK] = 0xa5;
#endif

		write_eeprom_all();

		Load_Data();
	}
}

//-----------------------------------------------------------------------------
//	Video Loss Check
//-----------------------------------------------------------------------------
u32 vVideo_Loss = 0; //1:Loss 0:Video
u8 ch9_loss = 0;
u8 Loss_Event_Flag = 0;
u8 Loss_Buzzer_Cnt = 0;

void Loss_Buzzer(void)
{
	static u32 timeout= 0;

	if(!TIME_AFTER(tick_10ms,timeout))
		return;

	timeout = tick_10ms + 50; // 10ms * 50 = 500ms

	if(Loss_Buzzer_Cnt)
	{
		if(Loss_Buzzer_Cnt%2) BUZZER_LOW;
		else BUZZER_HIGH;

		if(Loss_Buzzer_Cnt > 0) Loss_Buzzer_Cnt--;

		if(Loss_Buzzer_Cnt == 0) BUZZER_LOW;
	}
}

void Loss_Check(void)
{
	static u32 Pre_Video_Loss = 0;

	vVideo_Loss = 0;

#if 0 //Louis
	//���� ��ȣ�� ������ 1
	if(SGQ_READ_CH_FLOCK(0) == 0) vVideo_Loss |= 0x00000001;
	if(SGQ_READ_CH_FLOCK(1) == 0) vVideo_Loss |= 0x00000002;
	if(SGQ_READ_CH_FLOCK(2) == 0) vVideo_Loss |= 0x00000004;
	if(SGQ_READ_CH_FLOCK(3) == 0) vVideo_Loss |= 0x00000008;

	if(SGQ_READ_CH_FLOCK(4) == 0) vVideo_Loss |= 0x00000010;
	if(SGQ_READ_CH_FLOCK(5) == 0) vVideo_Loss |= 0x00000020;
	if(SGQ_READ_CH_FLOCK(6) == 0) vVideo_Loss |= 0x00000040;
	if(SGQ_READ_CH_FLOCK(7) == 0) vVideo_Loss |= 0x00000080;
#endif
	if(ch9_loss == 1) vVideo_Loss |= 0x00000100;


	if(Pre_Video_Loss != vVideo_Loss) 
	{
		Loss_Event_Flag = 1;
	}

	if(Pre_Video_Loss < vVideo_Loss) 
	{
		Loss_Buzzer_Cnt = sys_env.vLoss_Time*2;
	}
	
	Pre_Video_Loss = vVideo_Loss;			

}

void Video_Loss_Check(void)
{
    static u32 timeout = 0;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 50; // 10ms * 50 = 500ms

	Loss_Check();
}


//=============================================================================
//  main function
//=============================================================================
void main(void)
{
	int i;

	// initialize STM32F103x
	MCU_init();
	// initialize interrupt
	IRQ_Init();
	// initialize RTC
	RTC_Configuration();

	MDIN3XX_RST_LOW;
	Delay_ms(500);
	MDIN3XX_RST_HIGH;	
	
	/* Reset key check(Press and hold the menu button during turning on the power to initialize the EEPROM) */
	SetKeyMode(KEY_MODE_SHORT);//key_mode = KEY_MODE_SHORT;
	for(i = 0; i < 10; i++)
	{
		Key_Scan();
		Key_Check();
		Delay_ms(20);
	}
	
	if(key_flag == 1) 
	{
		key_flag = 0;
		if(key_data == KEY_MENU || key_data == KEY_FREEZE)
		{
			EEP_buf[cEEP_CHK] = 0;
			write_eeprom_all();

			BUZZER_HIGH;
			Delay_ms(100);
			BUZZER_LOW;
			Delay_ms(100);
			BUZZER_HIGH;
			Delay_ms(100);
			BUZZER_LOW;
			Delay_ms(100);
			BUZZER_HIGH;
			Delay_ms(100);
			BUZZER_LOW;
		}
	}
	//--------------------------------------------

	SetKeyMode(KEY_MODE_LONG);//key_mode = KEY_MODE_LONG;

	read_eeprom_all();
	Data_Load();
	// initialize Debug Serial
	USART3_Init();

	aux_display_flag = 1;

	if(sys_env.vResolution == 0) Video_Out_Res_Val = VIDOUT_1920x1080p60;
	else if(sys_env.vResolution == 1) Video_Out_Res_Val = VIDOUT_1920x1080p50; 

	CreateVideoInstance();
	CreateOSDInstance();
	DEMOKIT_Outputfrmt(Video_Out_Res_Val);
	Osd_Init_Erase();

	//VS4210 device initialization
	vs4210_system_init();
	Delay_ms(10);
	//NVP6158 device initialization
    NVP6158_init();

	Loss_Check();
	Delay_ms(100);

	Loss_Check();
	Delay_ms(100);
	Loss_Event_Flag = 0;
	Loss_Buzzer_Cnt = 0;

#ifdef __4CH__
	InputSelect = VIDEO_DIGITAL_SDI;
//	InputSelect = VIDEO_SDI_2HD_POP;
	sys_status.current_split_mode = SPLIT4_1;

	key_data = KEY_4SPLIT;
	key_flag = SET;
	sysenv_split_mode = 5; //OMG! what is 5?!
#endif

	Set_border_line();
	bMode_change_flag = SET;

#if 0 //for verify video input source test
	MDIN3xx_SetSrcTestPattern(&stVideo, MDIN_IN_TEST_H_COLOR);
#endif

	while(TRUE)
    { 
//		printf("While start \n");

		NVP6158_VideoDetectionProc();

		Video_Loss_Check();
		Loss_Buzzer();

		Auto_Seq_Cnt();
		Auto_Sequence();

		if(SET == bSETUP)
		{
			Setup_Process();
		}
		else
		{
			Key_Proc();
		}

		Time_Read();

		// video process handler
		VideoProcessHandler();

		// delay for HDMI-Tx register !!
		MDINDLY_mSec(1);

		if(InputSelect == VIDEO_DIGITAL_SDI)
			if(sysenv_split_mode != Pre_PIP_mode)
		{
			Pre_PIP_mode = sysenv_split_mode;
			
			DEMO_SetPIPDisplay(sysenv_split_mode);
		}
		// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.
		VideoHTXCtrlHandler();

		OSG_Display();
//		printf("While end \n");
    }
}


