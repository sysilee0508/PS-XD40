#include "common.h"

#define SKIP_CHANNEL		0xFF

static u8 autoSeqOn = CLEAR;
static u8 displayTime[NUM_OF_CHANNEL] = 0;
static eChannel_t displayChannel = CHANNEL1;

//static eChannel_t auto_display_channels[NUM_OF_CHANNEL] = {CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4};
//
//static void InitAutoDispalyChannels(void)
//{
//	eChannel_t index;
//
//	for(index = CHANNEL1; index < NUM_OF_CHANNEL; index++)
//	{
//		auto_display_channels[index] = index;
//	}
//}

//-----------------------------------------------------------------------------
// this function should be called when new video loss event is occurred.
//-----------------------------------------------------------------------------
void UpdateSkipChannels(void)
{

}

//-----------------------------------------------------------------------------
// Update display time of each channels and display first screen
// no video channel's display time will be set as 0xFF
//-----------------------------------------------------------------------------
void InitializeAutoSeq(void)
{
	u8 Index_flag = 0;
	BOOL skipOn;
	u8 videoLossChannel;
	eChannel_t iChannel;

	// Read NV Data
	Read_NvItem_AutoSeqLossSkip(&skipOn);

	//Find video loss channels
	if(ON == skipOn)
	{
		videoLossChannel = GetVideoLossChannels();
		if(videoLossChannel & VIDEO_LOSS_CHANNEL_ALL)
		{
			memset(displayTime, SKIP_CHANNEL, NUM_OF_CHANNEL);
		}
		else
		{
			Read_NvItem_AutoSeqTime(displayTime);
			for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
			{
				if(IsVideoLossChannel(iChannel) == TRUE)
				{
					displayTime[iChannel] = SKIP_CHANNEL;
				}
			}
		}
	}

	// Set auto sequence start channel
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
	{
		if(autoSeqOn == SET)
		{
			// move to next channel
			displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
		}
		else
		{
			displayChannel = sys_status.current_split_mode; //current displaying channel
			while((displayTime[displayChannel] == 0) || (displayTime[displayChannel] == SKIP_CHANNEL))
			{
				// move to next channel
				displayChannel = (++displayChannel) % NUM_OF_CHANNEL;
			}
		}
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
	{
		displayChannel = CHANNEL1;
	}
	sys_status.current_split_mode = (eSplitmode_t)displayChannel;
	// update display mode as full screen
	Write_NvItem_DisplayMode(DISPLAY_MODE_FULL_SCREEN);
	
	if(autoSeqOn == CLEAR)
	{
		autoSeqOn = SET;
	}

	// TO DO : Update displaying Channel here in full screen mode
//	Display_FullScreen(displayChannel);
	// Update OSD
	OSD_DrawBorderLine();
	Osd_EraseAllText();
	OSD_Display();
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
	if((sys_status.current_split_mode != displayChannel) && (autoSeqOn == SET))
	{
		//TO DO : display new channel
//		Display_FullScreen(displayChannel);
		// update current channel
		sys_status.current_split_mode = (eSplitmode_t)displayChannel;
		// Update OSD
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
}

//
//void Auto_Sequence(void)
//{
//	static eChannel_t previousStartChannel = 0xff;
//	eChannel_t startChannel;
//	u8 Index_flag = 0;
//	u8 vMODE;
//	BOOL videoLossChannelSkip;
//	BOOL osdDisplayOn;
//	BOOL titleDisplayOn;
//
//	Read_NvItem_AutoSeqLossSkip(&videoLossChannelSkip);
//	Read_NvItem_OsdOn(&osdDisplayOn);
//	Read_NvItem_TitleDispalyOn(&titleDisplayOn);
//
//	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
//	{
//		vMODE = DISPLAY_MODE_FULL_SCREEN;
//		startChannel = (eChannel_t)sys_status.current_split_mode;
//	}
//	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
//	{
//		vMODE = DISPLAY_MODE_4SPLIT;
//		startChannel = CHANNEL1;
//	}
//	if(autoSeqOn == SET)
//	{
//		if(videoLossChannelSkip == ON)
//		{
//			if(GetVideoLossChannels() == VIDEO_LOSS_CHANNEL_ALL)
//				return;
//		}
//
//		if(displayTime[0] == 0)
//		{
//			switch(vMODE)
//			{
//				case DISPLAY_MODE_FULL_SCREEN:
//				{
//					Read_NvItem_AutoSeqTime(displayTime);
//					if(displayChannel < SPLITMODE_FULL_CH4)
//						displayChannel++;
//					else
//						displayChannel = SPLITMODE_FULL_CH1;
//
//					if(videoLossChannelSkip == ON)
//					{
//						do
//						{
//							if(IsVideoLossChannel(displayChannel) == TRUE)
//							{
//								if(displayChannel < SPLITMODE_FULL_CH4)
//									displayChannel++;
//								else
//									displayChannel = SPLITMODE_FULL_CH1;
//							}
//							else
//								Index_flag = 1;
//
//						}
//						while(Index_flag == 0);
//					}
//
//					//ä�� ��ȯ
//					vMODE = DISPLAY_MODE_FULL_SCREEN;
//					startChannel = SPLITMODE_FULL_CH1+displayChannel;
//
//					if(startChannel < CHANNEL4)
//					{
//						sys_status.current_split_mode = startChannel;
//#if 0 //Louis
//						FullScreen(startChannel);
//#endif
//						//Set_border_line();
////						aux_display_flag = 0;
////						if(previousStartChannel == CHANNEL4)
////							DEMO_SetPIPViewWIND(0);	// update pip/pop window
//					}
//					else
//					{
//						sys_status.current_split_mode = startChannel;
//						//Set_border_line();
////						aux_display_flag = 1;
////						DEMO_SetPIPViewWIND(0);	// update pip/pop window
//
//					}
//
//					if((ON == osdDisplayOn) && (ON == titleDisplayOn))
//					{
//						Osd_EraseAllText();
//						OSD_DisplayChannelName();
//					}
//				}
//				break;
//			}
//		}
//	}
//
//	//if(vPre_vMODE != vMODE) bMode_change_flag = 1;
//	//if(vPre_vStart_CH != vStart_CH) bMode_change_flag = 1;
//
//	//vPre_vMODE = vMODE;
//	previousStartChannel = startChannel;
//}
//
