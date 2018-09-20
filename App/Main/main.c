//*****************************************************************************
// 
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "video_loss.h"

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
sys_stat_t sys_status;
//sys_env_t sys_env;

volatile BOOL fZOOMMove, fCROPMove;
s8 Video1_In_Res_Val = 0x00;
s8 Video2_In_Res_Val = 0x00;
s8 Video_Out_Res_Val = VIDOUT_1920x1080p60;

//u8 SDI_IN_Display_Flag = 0;

u8 PIP_mode = 5;
u8 Pre_PIP_mode = 0xff;

BYTE sysenv_split_mode = 0;

u8 aux_display_flag = 0;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 20ms - This is to handle keys and LEDs
//-----------------------------------------------------------------------------
void TIM3_IRQHandler(void)
{
	TIM3->SR = TIM3->SR & 0xFFFE;			// clear TIM2 update interrupt flag

	Key_Scan();
	Key_Led_Ctrl();
	Key_Check();

}

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
	eKeyData_t	keydata;
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
	u8 remoconId;

	Data = USART_ReceiveData(USART3);

    // Clear the USART3 RX interrupt
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);

    ReadNvItem(NV_ITEM_REMOCON_ID, &remoconId, sizeof(remoconId));

	if(remoconId != 0)
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
			if(Data == remoconId)
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

		if(GetSystemMode() == SYSTEM_SETUP_MODE)//(systemMode)
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

		RTC_ChangeRtcTimeStatus(SET);

		// Wait until last write operation on RTC registers has finished 
		RTC_WaitForLastTask();
	}
}

//=============================================================================
//  main function
//=============================================================================
void main(void)
{
	eResolution_t videoOutResolution;

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
			InitializeNvData();

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

	// Load NV data from flash memory
	LoadNvDataFromStorage();

	// initialize Debug Serial
	USART3_Init();

	// I think we don't need this flag because we don't have aux display
	//aux_display_flag = SET;
	ReadNvItem(NV_ITEM_OUTPUT_RESOLUTION, &videoOutResolution, sizeof(videoOutResolution));

	if(videoOutResolution == RESOLUTION_1920_1080_60P)
	{
		Video_Out_Res_Val = VIDOUT_1920x1080p60;
	}
	else if(videoOutResolution == RESOLUTION_1920_1080_50P)
	{
		Video_Out_Res_Val = VIDOUT_1920x1080p50;
	}
	else
	{
		// Are there only 2 resolution options?
	}

	CreateVideoInstance();
	CreateOSDInstance();
	SetVideoOutputfrmt(Video_Out_Res_Val);
	Osd_Init_Erase();

	//VS4210 device initialization
	vs4210_system_init();
	Delay_ms(10);
	//NVP6158 device initialization
	NVP6158_init();

	Loss_Event_Flag = 0;
	Loss_Buzzer_Cnt = 0;

	InputSelect = VIDEO_DIGITAL_SDI;
//	InputSelect = VIDEO_SDI_2HD_POP;
	sys_status.current_split_mode = SPLITMODE_SPLIT4;

	UpdateKeyData(KEY_4SPLIT);
	SetKeyReady();
	sysenv_split_mode = 5; //OMG! what is 5?!

	OSD_DrawBorderLine();
	changedDisplayMode = SET;

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

		Key_Proc();
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

		OSD_Display();
//		printf("While end \n");
    }
}


