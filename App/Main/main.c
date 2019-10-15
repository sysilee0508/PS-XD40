//*****************************************************************************
// 
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "NVP6168.h"

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

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

	// Reset MDIN
	MDIN3XX_RST_LOW;
	Delay_ms(500);
	MDIN3XX_RST_HIGH;	
	
	// Load NV data from flash memory
	LoadNvDataFromStorage();

#if BD_NVP == NVP6158
	//NVP6158 device initialization
	NVP6158_init();
#elif BD_NVP == NVP6168
	NVP6168_Init();
#endif
	InitVideoLossCheck();
	Delay_ms(1);

	CreateVideoInstance();
	CreateOSDInstance();
	Osd_ClearScreen();

	// Set key mode as long key mode
	//SetKeyMode(KEY_MODE_LONG);
	SetKeyMode(KEY_MODE_SHORT);
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
#if BD_NVP == NVP6158
		NVP6158_VideoDetectionProc();
#elif BD_NVP == NVP6168
		NVP6168_AutoDetection_Proc();
#endif

		Delay_ms(10);
		// check video loss
		ScanVideoLossChannels();
		Delay_ms(1);
		
		UpdateDisplayMode();
		// video process handler
		VideoProcessHandler();
		// delay for HDMI-Tx register !!
		Delay_ms(1);
		// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.
		VideoHTXCtrlHandler();
		OSD_Display();
		StoreNvDataToStorage();
    }
}
