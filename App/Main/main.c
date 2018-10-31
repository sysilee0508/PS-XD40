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
//sys_stat_t sys_status;
//sys_env_t sys_env;

volatile BOOL fZOOMMove, fCROPMove;
s8 Video1_In_Res_Val = 0x00;
s8 Video2_In_Res_Val = 0x00;
s8 Video_Out_Res_Val = VIDOUT_1920x1080p60;

//u8 SDI_IN_Display_Flag = 0;

u8 PIP_mode = 5;
u8 Pre_PIP_mode = 0xff;

BYTE sysenv_split_mode = 0;

//u8 aux_display_flag = 0;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

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
static void PlayBuzzer(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	u8 lossCount;	
    u8 alarmCount;
	u8 buzzerCount;
	if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
	{
		lossCount = GetVideoLossBuzzerCount();
		alarmCount = GetAlarmBuzzerCount();
		buzzerCount = MAX(lossCount, alarmCount);

		if(buzzerCount > 0)
		{
			if(buzzerCount%2)
				BUZZER_LOW;
			else
				BUZZER_HIGH;

			if(lossCount > 0)
			{
				DecreaseVideoLossBuzzerCount();//videoLossBuzzerCount--;
			}
			if(alarmCount > 0)
			{
				DecreaseAlarmBuzzerCount();
			}
		}
		else
		{
			BUZZER_LOW;
		}
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
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
	TIM2_Init();
	TIM3_Init();
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
	Osd_ClearScreen();

	//VS4210 device initialization
	vs4210_system_init();
	Delay_ms(10);
	//NVP6158 device initialization
	NVP6158_init();
	InitVideoLossCheck();
	InitializeMotionDetect();

	InputSelect = VIDEO_DIGITAL_SDI;
//	InputSelect = VIDEO_SDI_2HD_POP;
	Set_DisplayMode_Quad();

	UpdateKeyData(KEY_4SPLIT);
	SetKeyReady();
	sysenv_split_mode = 5; //OMG! what is 5?!

	OSD_DrawBorderLine();
	OSD_RefreshScreen();

#if 0 //for verify video input source test
	MDIN3xx_SetSrcTestPattern(&stVideo, MDIN_IN_TEST_H_COLOR);
#endif

	while(TRUE)
    { 
		NVP6158_VideoDetectionProc();
		Set_DisplayoutputMode_table();
		
		ScanVideoLossChannels();
		CheckAlarmClearCondition();
		PlayBuzzer();
		
		MotionDetectCheck();

		UpdateAutoSeqCount();
		DisplayAutoSeqChannel();

		Key_Proc();
		RTC_CheckTime();
		
		// video process handler
		VideoProcessHandler();

		// delay for HDMI-Tx register !!
		MDINDLY_mSec(1);

		if((InputSelect == VIDEO_DIGITAL_SDI) && (sysenv_split_mode != Pre_PIP_mode))
		{
			Pre_PIP_mode = sysenv_split_mode;
			DEMO_SetPIPDisplay(sysenv_split_mode);
		}

		//vs4210_display_proc();

		// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.
		VideoHTXCtrlHandler();

		OSD_Display();

		StoreNvDataToStorage();
//		printf("While end \n");
    }
}


