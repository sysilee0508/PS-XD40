#include "common.h"

#define SKIP_CHANNEL				0xFF
#define NO_SKIP_CHANNEL				0x00
#define DEFAULT_DISPLAY_TIME		3

static eAutoSeqType_t autoSeqStatus = AUTO_SEQ_NONE;
static u8 autoSeqOn = CLEAR;
static u8 displayTime[NUM_OF_CHANNEL] = 0;
static eChannel_t displayChannel = 0xFF;
static u8 oldSkipChannels = NO_SKIP_CHANNEL;

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

	autoSeqStatus = AUTO_SEQ_NORMAL;

	Read_NvItem_AutoSeqLossSkip(&skipOn);
	Read_NvItem_AutoSeqTime(displayTime);
	//displayMode = Get_SystemDisplayMode();

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
/*
	// Set auto sequence start channel
	if(displayMode == DISPLAY_MODE_FULL)
	{
		if(GetAutoSeqOn() == SET)
		{
			// move to next channel
			displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
		}
		else
		{
			displayChannel = Get_SystemDisplayChannel();
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
	// update display mode as full screen
	//DisplayMode_FullScreen(displayChannel);
	// set autoSeqOn
	ChangeAutoSeqOn(SET);
	OSD_DrawBorderLine();
	OSD_Display();
}

static void InitializeAutoSeq_Alarm(void)
{
	eChannel_t channel;

	memset(displayTime, 0x00, sizeof(displayTime));
	if(GetLastAlarmChannel() < NUM_OF_CHANNEL)
	{
		autoSeqStatus = AUTO_SEQ_ALARM;
		// the last alarm channel should be start channel
		displayChannel = GetLastAlarmChannel();
		displayTime[displayChannel] = DEFAULT_DISPLAY_TIME;

		//DisplayMode_FullScreen(displayChannel);
	}
}

//static void InitializeAutoSeq_Motion(void)
//{
//	autoSeqStatus = AUTO_SEQ_MOTION;
//}

//-----------------------------------------------------------------------------
void InitializeAutoSeq(eAutoSeqType_t type)
{
	switch(type)
	{
		case AUTO_SEQ_NORMAL:
			InitializeAutoSeq_Normal();
			break;

		case AUTO_SEQ_ALARM:
			ChangeAutoSeqOn(CLEAR);
			InitializeAutoSeq_Alarm();
			break;

		case AUTO_SEQ_NONE:
			ChangeAutoSeqOn(CLEAR);
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
				else if(autoSeqStatus == AUTO_SEQ_ALARM)
				{
					do
					{
						displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
					} while(GetAlarmStatus(displayChannel) == CLEAR);
					displayTime[displayChannel] = DEFAULT_DISPLAY_TIME;
				}
			}
		}
		previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
	}
}

void DisplayAutoSeqChannel(void)
{
	eChannel_t currentChannel;// = Get_SystemDisplayChannel();

	if((currentChannel != displayChannel) &&
			((autoSeqStatus > AUTO_SEQ_NONE) && (autoSeqStatus < AUTO_SEQ_MAX)))
	{
		OSD_EraseAllText();
		// update current channel
		//DisplayMode_FullScreen(displayChannel);
		// Update OSD
		OSD_RefreshScreen();
		OSD_Display();
	}
}

BOOL GetAutoSeqOn(void)
{
	return autoSeqOn;
}

void ChangeAutoSeqOn(BOOL set)
{
	autoSeqOn = set;
	if(set == CLEAR)
	{
		oldSkipChannels = NO_SKIP_CHANNEL;
		autoSeqStatus = AUTO_SEQ_NONE;
	}
}

eAutoSeqType_t GetCurrentAutoSeq(void)
{
	return autoSeqStatus;
}
