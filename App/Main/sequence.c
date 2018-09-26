#include "common.h"

#define SKIP_CHANNEL				0xFF
#define NO_SKIP_CHANNEL                     0x00

static u8 autoSeqOn = CLEAR;
static u8 displayTime[NUM_OF_CHANNEL] = 0;
static eChannel_t displayChannel = 0xFF;
static u8 oldSkipChannels = NO_SKIP_CHANNEL;

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

//-----------------------------------------------------------------------------
// Update display time of each channels and display first screen
// no video channel's display time will be set as 0xFF
//-----------------------------------------------------------------------------
void InitializeAutoSeq(void)
{
	BOOL skipOn;
	u8 videoLossChannel;
	eChannel_t iChannel;
	eDisplayMode_t displayMode;

	// Read NV Data
	Read_NvItem_AutoSeqLossSkip(&skipOn);
	Read_NvItem_AutoSeqTime(displayTime);
	Read_NvItem_DisplayMode(&displayMode);

	UpdateAutoSeqDisplayTime();
//	//Find video loss channels
//	if(ON == skipOn)
//	{
//		videoLossChannel = GetVideoLossChannels();
//		for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
//		{
//			if(IsVideoLossChannel(iChannel) == TRUE)
//			{
//				displayTime[iChannel] = SKIP_CHANNEL;
//			}
//		}
//	}

	// Set auto sequence start channel
	if(displayMode == DISPLAY_MODE_FULL_SCREEN)
	{
		if(autoSeqOn == SET)
		{
			// move to next channel
			displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
		}
		else
		{
			//displayChannel = sys_status.current_split_mode; //current displaying channel
			Read_NvItem_DisplayChannel(&displayChannel);
		}
		while((displayTime[displayChannel] == 0) || (displayTime[displayChannel] == SKIP_CHANNEL))
		{
			// move to next channel
			displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
		}
	}
	else if(displayMode == DISPLAY_MODE_4SPLIT)
	{
		displayChannel = CHANNEL1;
	}
	
	// check displayChannel is valuable. If not, move to next channel
	while((displayTime[displayChannel] == 0) || ((displayTime[displayChannel] == SKIP_CHANNEL) && (ON == skipOn)))
	{
		// move to next channel
		displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
	}

	Osd_EraseAllText();

//	sys_status.current_split_mode = (eSplitmode_t)displayChannel;
	Write_NvItem_DisplayChannel(displayChannel);
	// update display mode as full screen
	Write_NvItem_DisplayMode(DISPLAY_MODE_FULL_SCREEN);
	// set autoSeqOn
	ChangeAutoSeqOn(SET);

	OSD_DrawBorderLine();
	OSD_Display();

	// TO DO : Update displaying Channel here in full screen mode
	//Display_FullScreen(displayChannel);
}

void UpdateAutoSeqCount(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s = 0;
	u8 autoSeqTime[NUM_OF_CHANNEL];

	if(TIME_AFTER(currentSystemTime->tickCount_1s,previousSystemTimeIn1s,1))
	{
		if((autoSeqOn == SET) && (displayTime[displayChannel] != SKIP_CHANNEL))
		{
			if(displayTime[displayChannel] > 0)
			{
				displayTime[displayChannel]--;
			}
			else
			{
				Read_NvItem_AutoSeqTime(autoSeqTime);
				displayTime[displayChannel] = autoSeqTime[displayChannel];
				// move to next channel
				do
				{
					displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
				} while((displayTime[displayChannel] == 0) || (displayTime[displayChannel] == SKIP_CHANNEL));
			}
		}
		previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
	}
}

void DisplayAutoSeqChannel(void)
{
	eChannel_t currentChannel;

	Read_NvItem_DisplayChannel(&currentChannel);
	if((currentChannel != displayChannel) && (autoSeqOn == SET))
	{
		//TO DO : display new channel
//		Display_FullScreen(displayChannel);
		// update current channel
		Write_NvItem_DisplayChannel(displayChannel);
		//sys_status.current_split_mode = (eSplitmode_t)displayChannel;
		// Update OSD
		changedDisplayMode = SET;
		Osd_EraseAllText();
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
        }
}
