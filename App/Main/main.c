//*****************************************************************************
// 
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include "common.h"

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
//static u8 alarmOutRequester = ALARMOUT_REQUESTER_NONE;
//
//void TurnOnAlarmOut(u8 requester)
//{
//	alarmOutRequester |=  requester;
//	ALARMOUT_LOW;
//}
//
//void TurnOffAlarmOut(u8 requester)
//{
//	alarmOutRequester &= (~requester);
//
//	if(alarmOutRequester == ALARMOUT_REQUESTER_NONE)
//	{
//		ALARMOUT_HIGH;
//	}
//}

//static void PlayBuzzer(void)
//{
//	sSystemTick_t* currentSystemTime = GetSystemTime();
//	static u32 previousSystemTimeIn100ms = 0;
//	u8 lossCount = GetVideoLossBuzzerCount();
//    u8 alarmCount = GetAlarmBuzzerCount();
//    u8 motionCount = GetMotionBuzzerCount();
//	u8 buzzerCount;
//
//	if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
//	{
//		buzzerCount = MAX(MAX(lossCount, alarmCount), motionCount);
//
//		if(buzzerCount > 0)
//		{
//			if(buzzerCount%2)
//				BUZZER_LOW;
//			else
//				BUZZER_HIGH;
//
//			if(lossCount > 0)
//			{
//				DecreaseVideoLossBuzzerCount();
//			}
//			if(alarmCount > 0)
//			{
//				DecreaseAlarmBuzzerCount();
//			}
//			if(motionCount > 0)
//			{
//				DecreaseMotionBuzzerCount();
//			}
//		}
//		else
//		{
//			BUZZER_LOW;
//		}
//		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
//	}
//}
//
//=============================================================================
//  main function
//=============================================================================
void main(void)
{
	eDisplayMode_t displayMode;
	
	// initialize STM32F103x
	MCU_init();
	// initialize interrupt
	IRQ_Init();
	TIM2_Init();
	TIM3_Init();
	// initialize RTC
	RTC_Configuration();

	// Load NV data from flash memory
	LoadNvDataFromStorage();

	//NVP6158 device initialization
	NVP6158_init();
	InitVideoLossCheck();
	Delay_ms(1);
	// Reset MDIN
	MDIN3XX_RST_LOW;
	Delay_ms(500);
	MDIN3XX_RST_HIGH;	

	CreateVideoInstance();
	CreateOSDInstance();
	Osd_ClearScreen();

	// Set key mode as long key mode
	SetKeyMode(KEY_MODE_LONG);
	Read_NvItem_DisplayMode(&displayMode);
	if(displayMode == DISPLAY_MODE_FULL_CH1)
	{
		UpdateKeyData(KEY_FULL_CH1);
	}
	else if(displayMode == DISPLAY_MODE_FULL_CH2)
	{
		UpdateKeyData(KEY_FULL_CH2);
	}
	else
	{
		UpdateKeyData(KEY_SPLIT);
	}
	SetKeyReady();
//	Key_Proc();

#ifdef MDIN_TEST_PATTERN
//	MDIN3xx_SetSrcTestPattern(&stVideo, MDIN_IN_TEST_H_COLOR);
	MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_COLOR);
#endif

	while(TRUE)
	{
		RTC_CheckTime();
		Key_Proc();
		NVP6158_VideoDetectionProc();
		Delay_ms(1);
		// check video loss
//		ScanVideoLossChannels();
		UpdateDisplayMode();
		// video process handler
		VideoProcessHandler();
		// delay for HDMI-Tx register !!
		MDINDLY_mSec(1);
		// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.
		VideoHTXCtrlHandler();
		OSD_Display();
		StoreNvDataToStorage();
    }
}
