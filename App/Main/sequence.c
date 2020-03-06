#include "common.h"

#define SKIP_CHANNEL				0xFF
#define NO_SKIP_CHANNEL				0x00
#define DEFAULT_DISPLAY_TIME		3

static eAutoSeqType_t autoSeqStatus = AUTO_SEQ_NONE;
static u8 autoSeqOn = OFF;
static u8 displayTime[NUM_OF_CHANNEL] = 0;
static eChannel_t displayChannel = 0xFF;
static u8 oldSkipChannels = NO_SKIP_CHANNEL;

static BOOL currentMode = SEQ_MODE_FULL;
static ePipPosition_t pipPos = PIP_POSITION_MAX;

//-----------------------------------------------------------------------------
// Update display time of each channels and display first screen
// no video channel's display time will be set as 0xFF
//-----------------------------------------------------------------------------
static void InitializeAutoSeq_Normal(void)
{
	BOOL skipOn;
	u8 videoLossChannel;
	eChannel_t iChannel;
	eDisplayMode_t displayMode;

//	MDIN3xx_EnableMainFreeze(MDIN_ID_C, ON);
	
	autoSeqStatus = AUTO_SEQ_NORMAL;

	Read_NvItem_AutoSeqLossSkip(&skipOn);
	Read_NvItem_AutoSeqTime(displayTime);
	displayMode = GetCurrentDisplayMode();

	UpdateAutoSeqDisplayTime();
	//Find video loss channels
	if(ON == skipOn)
	{
		videoLossChannel = GetVideoLossChannels();
		for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
		{
			if(IsVideoLossChannel(iChannel) == TRUE)
			{
				displayTime[iChannel] = SKIP_CHANNEL;
			}
		}
	}
	// Set auto sequence start channel
	if(IS_FULL_MODE(displayMode))
	{
		if(GetAutoSeqOn() == SET)
		{
			// move to next channel
			displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
		}
		else
		{
			displayChannel = ConvertDisplayMode2Channel(displayMode);
		}
	}
	else //if(displayMode >= DISPLAY_MODE_QUAD_A)	*/
	{
		displayChannel = CHANNEL1;
	}
	
	// check displayChannel is valuable. If not, move to next channel
	while((displayTime[displayChannel] == 0) || ((displayTime[displayChannel] == SKIP_CHANNEL) && (ON == skipOn)))
	{
		// move to next channel
		displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
	}

	OSD_EraseAllText();
	// set autoSeqOn
	ChangeAutoSeqOn(ON);
	// update display mode as full screen
	DisplayScreen((eDisplayMode_t)displayChannel);
	SetInputChanged();
	OSD_SetBoaderLine();//OSD_DrawBorderLine();
	OSD_Display();

//	Delay_ms(100);
//	MDIN3xx_EnableMainFreeze(MDIN_ID_C, OFF);
}


//-----------------------------------------------------------------------------
// Update display time of each channels and display first screen
// no video channel's display time will be set as 0xFF
//-----------------------------------------------------------------------------
static void InitializeAutoSeq_Normal_Pip(void)
{
	BOOL skipOn;
	u8 videoLossChannel;
	eChannel_t iChannel;
	eDisplayMode_t displayMode;

	autoSeqStatus = AUTO_SEQ_NORMAL;

	Read_NvItem_AutoSeqLossSkip(&skipOn);
	Read_NvItem_AutoSeqTime(displayTime);
	displayMode = GetCurrentDisplayMode();

	UpdateAutoSeqDisplayTime();
	
	//Find video loss channels
	if(ON == skipOn)
	{
		videoLossChannel = GetVideoLossChannels();
		for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
		{
			if(IsVideoLossChannel(iChannel) == TRUE)
			{
				displayTime[iChannel] = SKIP_CHANNEL;
			}
		}
	}
	displayTime[CHANNEL1] = 0; // channel 1 should be skipped because it is not sequencial channel

	// Set auto sequence start channel
	if(IS_PIP_MODE(displayMode))
	{
		if(GetAutoSeqOn() == SET)
		{
			// move to next channel
			displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
		}
		else
		{
			displayChannel = CHANNEL2;
		}
	}
	else //if(displayMode >= DISPLAY_MODE_QUAD_A)	*/
	{
		displayChannel = CHANNEL2;
	}

	// check displayChannel is valuable. If not, move to next channel
	while((displayTime[displayChannel] == 0) || ((displayTime[displayChannel] == SKIP_CHANNEL) && (ON == skipOn)))
	{
		// move to next channel
		displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
	}

	OSD_EraseAllText();
	// set autoSeqOn
	ChangeAutoSeqOn(ON);
	// display channel in PIP mode 

	if((displayChannel > CHANNEL1) && (displayChannel < NUM_OF_CHANNEL))
	{
		displayMode = (eDisplayMode_t)(displayChannel - CHANNEL2);
	}
	else
	{
		displayMode = DISPLAY_MODE_FULL_CH1;	//0
	}
	
	switch(pipPos)
	{
		case PIP_POSITION_LT:	//D
			DisplayScreen(DISPLAY_MODE_PIP_D2 + displayMode);
			break;

		case PIP_POSITION_RT:	// A
			DisplayScreen(DISPLAY_MODE_PIP_A2 + displayMode);
			break;
			
		case PIP_POSITION_LB:	// B
			DisplayScreen(DISPLAY_MODE_PIP_B2 + displayMode);
			break;

		case PIP_POSITION_RB:	// C
			DisplayScreen(DISPLAY_MODE_PIP_C2 + displayMode);
			break;
	}
	
	SetInputChanged();
	OSD_SetBoaderLine();//OSD_DrawBorderLine();
	OSD_Display();
}

#if 0
static void InitializeAutoSeq_Alarm(void)
{
	eChannel_t channel;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();

	memset(displayTime, 0x00, sizeof(displayTime));
	if(GetLastAlarmChannel() < NUM_OF_CHANNEL)
	{
		autoSeqStatus = AUTO_SEQ_ALARM;
		// the last alarm channel should be start channel
		displayChannel = GetLastAlarmChannel();
		displayTime[displayChannel] = DEFAULT_DISPLAY_TIME;

		if(displayChannel != displayMode)
		{
			OSD_EraseAllText();
			DisplayScreen((eDisplayMode_t)displayChannel);
			SetInputChanged();
			OSD_SetBoaderLine();//OSD_DrawBorderLine();
			OSD_Display();
		}
	}
}
#endif
//-----------------------------------------------------------------------------
void InitializeAutoSeq(eAutoSeqType_t type)
{
	switch(type)
	{
		case AUTO_SEQ_NORMAL:
			Read_NvItem_AutoSeqMode(&currentMode);
			if(currentMode == SEQ_MODE_FULL)
			{
				InitializeAutoSeq_Normal();
			}
			else
			{
				Read_NvItem_AutoSeq_Position(&pipPos);
				InitializeAutoSeq_Normal_Pip();
			}
			break;

		case AUTO_SEQ_ALARM:
			ChangeAutoSeqOn(OFF);
			//InitializeAutoSeq_Alarm();
			break;

		case AUTO_SEQ_NONE:
			ChangeAutoSeqOn(OFF);
			autoSeqStatus = AUTO_SEQ_NONE;
			memset(displayTime, 0x00, sizeof(displayTime));
			break;
	}
}

//-----------------------------------------------------------------------------
// this function should be called when new video loss event is occurred.
//-----------------------------------------------------------------------------
void UpdateAutoSeqDisplayTime(void)
{
	BOOL skipOn;
	u8 skipChannels;
	eChannel_t iChannel;
	u8 changedChannels;
	u8 timeInSecs[NUM_OF_CHANNEL] = {0,};

	Read_NvItem_AutoSeqLossSkip(&skipOn);
	Read_NvItem_AutoSeqTime(timeInSecs);

	if(ON == skipOn)
	{
		skipChannels = GetVideoLossChannels();
		changedChannels = (oldSkipChannels ^ skipChannels);

		for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
		{
			if(changedChannels & (0x01 << iChannel))
			{
				if(IsVideoLossChannel(iChannel) == TRUE)
				{
					displayTime[iChannel] = SKIP_CHANNEL;
				}
				else
				{
					displayTime[iChannel] = timeInSecs[iChannel];
				}
			}
		}
		oldSkipChannels = skipChannels;
	}
	else
	{
		oldSkipChannels = NO_SKIP_CHANNEL;
	}
}

void UpdateAutoSeqCount(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s = 0;
	u8 autoSeqTime[NUM_OF_CHANNEL];

	if(TIME_AFTER(currentSystemTime->tickCount_1s,previousSystemTimeIn1s,1))
	{
		if(((autoSeqStatus > AUTO_SEQ_NONE) && (autoSeqStatus < AUTO_SEQ_MAX)) && (displayTime[displayChannel] != SKIP_CHANNEL))
		{
			if(displayTime[displayChannel] > 0)
			{
				displayTime[displayChannel]--;
			}
			else
			{
				if(autoSeqStatus == AUTO_SEQ_NORMAL)
				{
					Read_NvItem_AutoSeqTime(autoSeqTime);
					displayTime[displayChannel] = autoSeqTime[displayChannel];
					// move to next channel
					do
					{
						displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
					} while((displayTime[displayChannel] == 0) || (displayTime[displayChannel] == SKIP_CHANNEL));
				}
				/*
				else if(autoSeqStatus == AUTO_SEQ_ALARM)
				{
					do
					{
						displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
					} while(GetAlarmStatus(displayChannel) != ALARM_SET);
					displayTime[displayChannel] = DEFAULT_DISPLAY_TIME;
				}
				*/
			}
		}
		previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
	}
}

void DisplayAutoSeqChannel(void)
{
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	eChannel_t currentChannel;

	//if((currentMode == SEQ_MODE_FULL) || (autoSeqStatus == AUTO_SEQ_ALARM))
	if(currentMode == SEQ_MODE_FULL)
	{
		if(IS_FULL_MODE(displayMode) == TRUE)
			currentChannel = ConvertDisplayMode2Channel(displayMode);
		else
			currentChannel = CHANNEL1;

		if((currentChannel != displayChannel) &&
				((autoSeqStatus > AUTO_SEQ_NONE) && (autoSeqStatus < AUTO_SEQ_MAX)))
		{
			OSD_EraseAllText();
			// update current channel
			DisplayScreen((eDisplayMode_t)displayChannel);
			SetInputChanged();
			// Update OSD
			OSD_RefreshScreen();
			forceFreezeOn = SET;
		}
	}
	else // pip mode
	{
		if(IS_PIP_MODE(displayMode) == TRUE)
		{
			currentChannel = FindAuxChannel(displayMode, MDIN_ID_A);
		}
		else
		{
			currentChannel = CHANNEL2;
		}

		if((currentChannel != displayChannel) &&
				((autoSeqStatus > AUTO_SEQ_NONE) && (autoSeqStatus < AUTO_SEQ_MAX)))
		{
			//OSD_EraseAllText();

			// update current channel
			switch(pipPos)
			{
				case PIP_POSITION_LT:	//D
					DisplayScreen(DISPLAY_MODE_PIP_D2 + displayChannel - CHANNEL2);
					break;

				case PIP_POSITION_RT:	// A
					DisplayScreen(DISPLAY_MODE_PIP_A2 + displayChannel - CHANNEL2);
					break;
					
				case PIP_POSITION_LB:	// B
					DisplayScreen(DISPLAY_MODE_PIP_B2 + displayChannel - CHANNEL2);
					break;

				case PIP_POSITION_RB:	// C
					DisplayScreen(DISPLAY_MODE_PIP_C2 + displayChannel - CHANNEL2);
					break;
			}
			SetInputChanged();
			// Update OSD
			OSD_RefreshScreen();
			forceFreezeOn = SET;
		}
	}
}

BOOL GetAutoSeqOn(void)
{
	return autoSeqOn;
}

void ChangeAutoSeqOn(BOOL on_off)
{
	autoSeqOn = on_off;
	if(on_off == OFF)
	{
		oldSkipChannels = NO_SKIP_CHANNEL;
		autoSeqStatus = AUTO_SEQ_NONE;
	}
	Write_NvItem_AutoOn(autoSeqOn);
}

eAutoSeqType_t GetCurrentAutoSeq(void)
{
	return autoSeqStatus;
}
