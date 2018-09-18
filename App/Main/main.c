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

//-----------------------------------------------------------------------------
//	USART3 Rx Interrupt
//-----------------------------------------------------------------------------

//u8 rs232_key_table[] =
//{
//	0x00,	//0x00
//	0x80,	//0x01
//	0x81,	//0x02
//	0x82,	//0x03
//	0x83,	//0x04
//	0x00,	//0x05
//	0x00,	//0x06
//	0x00,	//0x07
//	0x00,	//0x08
//	0x00,	//0x09
//	0x00,	//0x0a
//	0x00,	//0x0b
//	0x00,	//0x0c
//	0x00,	//0x0d
//	0x00,	//0x0e
//	0x00,	//0x0f
//	0x00,	//0x10
//	0x00,	//0x11
//	0x00,	//0x12
//	0x00,	//0x13
//	0x00,	//0x14
//	0x84,	//0x15
//	0x00,	//0x16
//	0x00,	//0x17
//	0x00,	//0x18
//	0x9a,	//0x19
//	0x9b,	//0x1a
//	0x00,	//0x1b
//	0x8a,	//0x99
//};
typedef struct
{
	keydata_e	keydata;
	u8			virtual_key;
} VirtualKeys_t;

enum
{
	VIRTUAL_KEY_NONE = 0x00,
	VIRTUAL_KEY_CH1	 = 0x80,
	VIRTUAL_KEY_CH2,
	VIRTUAL_KEY_CH3,
	VIRTUAL_KEY_CH4,
	VIRTUAL_KEY_4SPLIT,
	VIRTUAL_KEY_MENU = 0x8A,
	VIRTUAL_KEY_FREEZE = 0x9A,
	VIRTUAL_KEY_AUTO_SEQ
};

VirtualKeys_t virtual_key_tabla[] =
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

void USART3_IRQHandler(void)
{
	static u8 bSOH_FLAG = 0;
	static u8 bHEADER_FLAG = 0;
	static u8 bSTX_FLAG = 0;
	static u8 bETX_FLAG = 0;
	u8 Data;
	u8 i;

	Data = USART_ReceiveData(USART3);

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
		if(Data == 3)
			SetKeyReady();
		bSOH_FLAG = 0;
		bHEADER_FLAG = 0;
		bSTX_FLAG = 0;
		bETX_FLAG = 0;
	}
	else
	{
		bETX_FLAG = 1;
		if((Data >= 0x90) && (Data != VIRTUAL_KEY_FREEZE) && (Data != VIRTUAL_KEY_AUTO_SEQ))
		{
			Data -= 0x10;
		}

		if(bSETUP)
		{
			if(Data == VIRTUAL_KEY_MENU)
			{
				Data = VIRTUAL_KEY_FREEZE; //KEY_MENU -> KEY_FREEZE
			}
		}

		for(i = 0; i < sizeof(virtual_key_tabla)/sizeof(VirtualKeys_t); i++)
		{
			if(virtual_key_tabla[i].virtual_key == Data)
			{
				UpdateKeyData(virtual_key_tabla[i].keydata);
				break;
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
#define EEP_VALID_CODE		0xA5
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

	for(i = 0; i < NUM_OF_CHANNEL; i++)
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
	sys_env.vAlarm = EEP_buf[cSYSENV_vAlarm];
	sys_env.vAlarmOutTime = EEP_buf[cSYSENV_vAlarm_Display_Time];
	sys_env.vAlarmBuzzerTime = EEP_buf[cSYSENV_vAlarm_Buzzer_Time];
	sys_env.vREMOCON_ID = EEP_buf[cSYSENV_vREMOCON_ID];
	sys_env.vLoss_Time = EEP_buf[cSYSENV_vLoss_Time];
	sys_env.vLoss_Display = EEP_buf[cSYSENV_vLoss_Display];

	sys_env.vResolution = EEP_buf[cSYSENV_resolution];
	sys_env.baud_rate = EEP_buf[cSYSENV_baud_rate];
	sys_env.b9Split_Mode = EEP_buf[cSYSENV_b9Split_Mode];
	sys_env.alarm_remote_sel= EEP_buf[cSYSENV_alarm_remote_sel];
}

void Data_Load(void)
{
	BYTE i;

	if(EEP_buf[cEEP_CHK] == EEP_VALID_CODE)
	{
		Load_Data();
	}
	else 
	{
		memset(EEP_buf, 0, 2048);

		EEP_buf[cSYSENV_vCORRECT_OFFSET] = 0;
		EEP_buf[cSYSENV_bVECTOR] = 1;
		EEP_buf[cSYSENV_bCORRECT] = 0;
		EEP_buf[cSYSENV_vDATE_FORMAT] = 0;
		EEP_buf[cSYSENV_bTIME_ON] = 1;
		EEP_buf[cSYSENV_vTIME_Position] = 1;
		
		for(i=0; i<NUM_OF_CHANNEL; i++)
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
		EEP_buf[cSYSENV_vOSD_Position] = 6;

		EEP_buf[cSYSENV_border_line] = 1;
		EEP_buf[cSYSENV_vAlarm] = 0;
		EEP_buf[cSYSENV_vAlarm_Display_Time] = 0;
		EEP_buf[cSYSENV_vAlarm_Buzzer_Time] = 5;
		EEP_buf[cSYSENV_vREMOCON_ID] = 0;
		EEP_buf[cSYSENV_vLoss_Time] = 3;
		EEP_buf[cSYSENV_vLoss_Display] = 0xff;

		EEP_buf[cSYSENV_resolution] = 0;
		EEP_buf[cSYSENV_baud_rate] = 3;
		EEP_buf[cSYSENV_b9Split_Mode] = 0;
		EEP_buf[cSYSENV_alarm_remote_sel] = 0xff;
#ifdef __4CH__
		EEP_buf[cEEP_CHK] = EEP_VALID_CODE;
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
u32 videoLossBuzzerCount = 0;
u32 alarmBuzzerCountIn500ms = 0;


void CheckBuzzer(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms;
	u32 buzzerCount = MAX(videoLossBuzzerCount, alarmBuzzerCountIn500ms);

	if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
	{
		//execute below lines every 500ms
		if(buzzerCount > 0)
		{
			if(buzzerCount%2)
				BUZZER_LOW;
			else
				BUZZER_HIGH;
			if(videoLossBuzzerCount > 0)
			{
				videoLossBuzzerCount--;
			}
			if(alarmBuzzerCountIn500ms > 0)
			{
				alarmBuzzerCountIn500ms--;
			}
		}
		else
		{
			BUZZER_LOW;
		}
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
	}
}

void Loss_Check(void)
{
	static u32 Pre_Video_Loss = 0xFFFFFFFF;

	vVideo_Loss = 0;

	NVP6158_Video_Loss_Check(&vVideo_Loss);

	if(Pre_Video_Loss != vVideo_Loss) 
	{
		Loss_Event_Flag = 1;
	}

	if(Pre_Video_Loss < vVideo_Loss) 
	{
		videoLossBuzzerCount = sys_env.vLoss_Time*2;
	}
	
	Pre_Video_Loss = vVideo_Loss;			

}

void Video_Loss_Check(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms;

	if(TIME_AFTER(currentSystemTime->tickCount_100ms,previousSystemTimeIn100ms,5))
	{
		Loss_Check();
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
	}

}


//=============================================================================
//  main function
//=============================================================================
void main(void)
{
	// initialize STM32F103x
	MCU_init();
	// initialize interrupt
	IRQ_Init();
	// initialize RTC
	RTC_Configuration();

	// Set key mode as short key mode in order to check reset key 
	SetKeyMode(KEY_MODE_SHORT);

	MDIN3XX_RST_LOW;
	Delay_ms(500);
	MDIN3XX_RST_HIGH;	

	// Need additional 500us to check reset key
	// Reset Key Function : Press and hold the menu button during turning on the power to initialize the EEPROM
	Delay_ms(500);
	if(IsKeyReady()) 
	{
		ClearKeyReady();
		if(GetCurrentKey() == KEY_MENU || GetCurrentKey() == KEY_FREEZE)
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
	// Set key mode as long key mode 
	SetKeyMode(KEY_MODE_LONG);

	read_eeprom_all();
	Data_Load();
	// initialize Debug Serial
	USART3_Init();

	// I think we don't need this flag because we don't have aux display
	//aux_display_flag = SET;

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

	Loss_Event_Flag = 0;
	videoLossBuzzerCount = 0;

#ifdef __4CH__
	InputSelect = VIDEO_DIGITAL_SDI;
//	InputSelect = VIDEO_SDI_2HD_POP;
	sys_status.current_split_mode = SPLITMODE_SPLIT4_1;

	UpdateKeyData(KEY_4SPLIT);
	SetKeyReady();
	sysenv_split_mode = 5; //OMG! what is 5?!
#endif

	Set_border_line();
	bMode_change_flag = SET;

#if 0 //for verify video input source test
	MDIN3xx_SetSrcTestPattern(&stVideo, MDIN_IN_TEST_H_COLOR);
#endif

	while(TRUE)
    { 
		NVP6158_VideoDetectionProc();

		Video_Loss_Check();
		CheckAlarmClearCondition();
		CheckBuzzer();

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
		vs4210_display_proc();

		// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.
		VideoHTXCtrlHandler();

		OSG_Display();
    }
}


