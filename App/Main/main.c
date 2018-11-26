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
volatile BOOL fZOOMMove, fCROPMove;
s8 Video_Out_Res_Val = VIDOUT_1920x1080p60;

u8 PIP_mode = 5;
u8 Pre_PIP_mode = 0xff;
BYTE sysenv_split_mode = 0;

static u8 alarmOutRequester = ALARMOUT_REQUESTER_NONE;

void TurnOnAlarmOut(u8 requester)
{
	alarmOutRequester |=  requester;
	ALARMOUT_LOW;
}

void TurnOffAlarmOut(u8 requester)
{
	alarmOutRequester &= (~requester);

	if(alarmOutRequester == ALARMOUT_REQUESTER_NONE)
	{
		ALARMOUT_HIGH;
	}
}

static void PlayBuzzer(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	u8 lossCount = GetVideoLossBuzzerCount();
    u8 alarmCount = GetAlarmBuzzerCount();
    u8 motionCount = GetMotionBuzzerCount();
	u8 buzzerCount;

	if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
	{
		buzzerCount = MAX(MAX(lossCount, alarmCount), motionCount);

		if(buzzerCount > 0)
		{
			if(buzzerCount%2)
				BUZZER_LOW;
			else
				BUZZER_HIGH;

			if(lossCount > 0)
			{
				DecreaseVideoLossBuzzerCount();
			}
			if(alarmCount > 0)
			{
				DecreaseAlarmBuzzerCount();
			}
			if(motionCount > 0)
			{
				DecreaseMotionBuzzerCount();
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
	DisplayMode_SplitScreen(Get_SystemSplitMode());

	UpdateKeyData(KEY_SPLIT);
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
		MotionDetectCheck();
		PlayBuzzer();

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


