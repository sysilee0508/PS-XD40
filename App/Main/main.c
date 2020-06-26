//*****************************************************************************
// 
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "video_loss.h"
#include "NVP6168.h"

#undef MDIN_TEST_PATTERN
//#define MDIN_TEST_PATTERN
extern BYTE	TPIAddr;
extern BOOL forceFreezeOn;

extern void InitRegisterSet();
// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
//static u8 alarmOutRequester = ALARMOUT_REQUESTER_NONE;

void TurnOnOffAlarmLed(BOOL on_off)
{
	if(on_off == ON)
	{
		ALARMOUT_LOW;
	}
	else
	{
		ALARMOUT_HIGH;
	}
}

#if 0
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
#endif

static void PlayBuzzer(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	u8 lossCount = GetVideoLossBuzzerCount();
	u8 alarmCount = GetAlarmBuzzerCount();
	//u8 motionCount = GetMotionBuzzerCount();
	u8 buzzerCount;
	static u8 playCount = 0;

	if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
	{
		buzzerCount = MAX(lossCount, alarmCount);

		if(buzzerCount > 0)
		{
			if(playCount%2)
			{
				
				BUZZER_LOW;
			}
			else
			{
				BUZZER_HIGH;
			}

			playCount++;

			if(lossCount > 0)
			{
				DecreaseVideoLossBuzzerCount();
			}
			if(alarmCount > 0)
			{
				DecreaseAlarmBuzzerCount();
			}
			//if(motionCount > 0)
			//{
			//	DecreaseMotionBuzzerCount();
			//}
		}
		else
		{
			BUZZER_LOW;
			playCount = 0;
		}
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
	}
}

static void ForceFreeze(BOOL on_off)
{
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	eDisplayMode_t prevMode = GetPrevDisplayMode();
	switch(on_off)
	{
		case ON:
			if((IS_PIP_MODE(displayMode) == TRUE) && (IS_PIP_MODE(prevMode) == TRUE))
			{
				MDIN3xx_EnableAuxFreeze(&stVideo[MDIN_ID_C], ON);
			}
			else
			{
				MDIN3xx_EnableMainFreeze(MDIN_ID_C, ON);
				MDIN3xx_EnableAuxFreeze(&stVideo[MDIN_ID_C], ON);
			}
			break;

		case OFF:
			if(IsScreenFreeze() == FALSE)
			{
				if((IS_PIP_MODE(displayMode) == TRUE) && (IS_PIP_MODE(prevMode) == TRUE))
				{
					MDIN3xx_EnableAuxFreeze(&stVideo[MDIN_ID_C], OFF);
				}
				else
				{
					MDIN3xx_EnableMainFreeze(MDIN_ID_C, OFF);
					MDIN3xx_EnableAuxFreeze(&stVideo[MDIN_ID_C], OFF);
				}
			}
			break;
	}
}

//=============================================================================
//  main function
//=============================================================================
 
 void main(void)
{
	//eResolution_t outRes;
	//static u8 fCnt = 0;
	
	I2C_SET_CHANNEL(I2C_MAIN);
	// initialize STM32F103x
	MCU_init();
	// initialize interrupt
	IRQ_Init();
	TIM2_Init();
	TIM3_Init();
	// initialize RTC
	RTC_Configuration();

	MDIN3XX_RST_LOW;
	Delay_ms(500);
	MDIN3XX_RST_HIGH;	

	// Set key mode as short key mode in order to check reset key 
	SetKeyMode(KEY_MODE_SHORT);

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

#if BD_NVP == NVP6158
	//NVP6158 device initialization
	NVP6158_init();
#elif BD_NVP == NVP6168
	NVP6168_Init();
#endif
	InitVideoLossCheck();
	InitializeMotionDetect();

#if BD_NVP == NVP6158
	NVP6158_VideoDetectionProc();
#elif BD_NVP == NVP6168
	NVP6168_AutoDetection_Proc();
#endif
	Delay_ms(10);

	// initialize Debug Serial
	USART3_Init();

	CreateVideoInstance();
	CreateOSDInstance();
	Osd_ClearScreen();

	// Init TP2912
	InitRegisterSet();
	
	SetInitialKey();

#ifdef MDIN_TEST_PATTERN
//	I2C_SET_CHANNEL(I2C_MAIN);
//	I2C_SET_CHANNEL(I2C_SUB);
	//SELECT_MDIN(MDIN_I2C_H);
	M380_ID = MDIN_ID_D;
	MDIN3xx_SetSrcTestPattern(&stVideo[MDIN_ID_D], MDIN_IN_TEST_H_COLOR);
//	MDIN3xx_SetSrcTestPattern(&stVideo_C, MDIN_IN_TEST_H_COLOR);
//	MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_COLOR);
#endif
	while(TRUE)
	{
		RTC_CheckTime();
		Key_Proc();
		
		UpdateAutoSeqCount();
		DisplayAutoSeqChannel();
		
		if(forceFreezeOn == SET)
		{
			ForceFreeze(ON);
		}
		
#if BD_NVP == NVP6158
		NVP6158_VideoDetectionProc();
#elif BD_NVP == NVP6168
		NVP6168_AutoDetection_Proc();
#endif
		Delay_ms(10);
		
		ScanVideoLossChannels();
		MotionDetectCheck();
		CountDown_AlarmOutTimer();
		PlayBuzzer();

		UpdateDisplayMode();

		// video process handler
		VideoProcessHandler();
		// delay for HDMI-Tx register !!
		MDINDLY_mSec(1);
		// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.
		VideoHTXCtrlHandler();

		if(forceFreezeOn == SET)
		{
			forceFreezeOn = CLEAR;
			ForceFreeze(OFF);
			OSD_EraseAllText();
		}
		else
		{
			OSD_Display();
			OSD_DrawBorderLine();
		}
		StoreNvDataToStorage();
    }
}


