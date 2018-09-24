#include "common.h"
#include "Sequence.h"
#include "video_loss.h"


#define SKIP_CHANNEL		0xFF

u8 bAuto_Seq_Flag = CLEAR;
static u8 vAuto_Seq_Cnt[NUM_OF_CHANNEL] = 0;
static u8 vAuto_Seq_Index = 0;

static eChannel_t auto_display_channels[NUM_OF_CHANNEL] = {CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4};

static void InitAutoDispalyChannels(void)
{
	eChannel_t index;

	for(index = CHANNEL1; index < NUM_OF_CHANNEL; index++)
	{
		auto_display_channels[index] = index;
	}
}

void Auto_Seq_Init(void)
{
//	u8 Index_flag = 0;
	eChannel_t startChannel;
	eSplitmode_t splitMode;
	BOOL videoLossChannelSkip;
	BOOL osdDisplayOn;
	BOOL titleDisplayOn;
	u8 videoLossChannel;
	eChannel_t iChannel;

	// Read NV Data
	Read_NvItem_AutoSeqLossSkip(&videoLossChannelSkip);
	Read_NvItem_OsdOn(&osdDisplayOn);
	Read_NvItem_TitleDispalyOn(&titleDisplayOn);
	Read_NvItem_AutoSeqTime(vAuto_Seq_Cnt);

	//Find video loss channels
	if(ON == videoLossChannelSkip)
	{
		videoLossChannel = GetVideoLossChannels();
		if(videoLossChannel & VIDEO_LOSS_CHANNEL_ALL)
		{
			memset(auto_display_channels, SKIP_CHANNEL, sizeof(auto_display_channels));
		}
		else if(videoLossChannel & VIDEO_LOSS_CHANNEL_NONE)
		{
			InitAutoDispalyChannels();
		}
		else
		{
			for(iChannel = CHANNEL1; iChannel < NUM_OF_CHANNEL; iChannel++)
			{
				if((videoLossChannel & VIDEO_LOSS_CHANNEL(iChannel)) == VIDEO_LOSS_CHANNEL(iChannel))
				{
					auto_display_channels[iChannel] = SKIP_CHANNEL;
				}
			}

		}
	}

	// Set auto sequence start channel
	splitMode = DISPLAY_MODE_FULL_SCREEN;
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
	{
		startChannel = sys_status.current_split_mode; //current displaying channel
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
	{
		startChannel = CHANNEL1;
	}

//I don't know what is vAuto_Seq_Index??????
//	if(bAuto_Seq_Flag == CLEAR)
//	{
//		vAuto_Seq_Index = 0;
//	}
//
//	if(bAuto_Seq_Flag == SET)
//	{
//		if(vAuto_Seq_Index < CHANNEL4)
//			vAuto_Seq_Index++;
//		else
//			vAuto_Seq_Index = CHANNEL1;
//	}
	
//	if(videoLossChannelSkip == ON)
//	{
//		do
//		{
//			if(vVideo_Loss&(0x00000001<<vAuto_Seq_Index))
//			{
//				vAuto_Seq_Index = (++vAuto_Seq_Index) % NUM_OF_CHANNEL;
////						if(vAuto_Seq_Index < SPLITMODE_FULL_CH4)
////							vAuto_Seq_Index++;
////						else
////							vAuto_Seq_Index = SPLITMODE_FULL_CH1;
//			}
//			else // all channel has video signal
//				Index_flag = 1;
//
//		}
//		while(Index_flag == 0);
//	}

	//Full 1ä�� ��ȯ
//	splitMode = FULL_SCREEN_MODE;
//	startChannel = CHANNEL1 + vAuto_Seq_Index;

	sys_status.current_split_mode = startChannel;
	OSD_DrawBorderLine();
//	DEMO_SetPIPViewWIND(0);	// update pip/pop window

	if((ON == osdDisplayOn) && (ON == titleDisplayOn))
	{
		Osd_EraseAllText();
		OSD_DisplayChannelName();
	}

	bAuto_Seq_Flag = SET;
}


void Auto_Seq_Cnt(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s;

	if(TIME_AFTER(currentSystemTime->tickCount_1s,previousSystemTimeIn1s,1))
	{
		if(vAuto_Seq_Cnt[CHANNEL1] > 0)
		{
			vAuto_Seq_Cnt[CHANNEL1]--;
		}
		previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
	}
}


void Auto_Sequence(void)
{
	static eChannel_t previousStartChannel = 0xff;
	eChannel_t startChannel;
	u8 Index_flag = 0;
	u8 vMODE;
	BOOL videoLossChannelSkip;
	BOOL osdDisplayOn;
	BOOL titleDisplayOn;

	Read_NvItem_AutoSeqLossSkip(&videoLossChannelSkip);
	Read_NvItem_OsdOn(&osdDisplayOn);
	Read_NvItem_TitleDispalyOn(&titleDisplayOn);
	
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
	{
		vMODE = DISPLAY_MODE_FULL_SCREEN;
		startChannel = (eChannel_t)sys_status.current_split_mode;
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
	{
		vMODE = DISPLAY_MODE_4SPLIT;
		startChannel = CHANNEL1;
	}
	if(bAuto_Seq_Flag == SET)
	{
		if(videoLossChannelSkip == ON)
		{
			if(GetVideoLossChannels() == VIDEO_LOSS_CHANNEL_ALL)
				return;
		}
		
		if(vAuto_Seq_Cnt[0] == 0)
		{
			switch(vMODE)
			{
				case DISPLAY_MODE_FULL_SCREEN:  
				{
					Read_NvItem_AutoSeqTime(vAuto_Seq_Cnt);
					if(vAuto_Seq_Index < SPLITMODE_FULL_CH4)
						vAuto_Seq_Index++;
					else
						vAuto_Seq_Index = SPLITMODE_FULL_CH1;

					if(videoLossChannelSkip == ON)
					{
						do
						{
							if(IsVideoLossChannel(vAuto_Seq_Index) == TRUE)
							{
								if(vAuto_Seq_Index < SPLITMODE_FULL_CH4)
									vAuto_Seq_Index++;
								else
									vAuto_Seq_Index = SPLITMODE_FULL_CH1;
							}
							else
								Index_flag = 1;
						
						}
						while(Index_flag == 0); 
					}	

					//ä�� ��ȯ
					vMODE = DISPLAY_MODE_FULL_SCREEN; 
					startChannel = SPLITMODE_FULL_CH1+vAuto_Seq_Index;

					if(startChannel < CHANNEL4)
					{
						sys_status.current_split_mode = startChannel;
#if 0 //Louis
						FullScreen(startChannel);
#endif
						//Set_border_line();
//						aux_display_flag = 0;
//						if(previousStartChannel == CHANNEL4)
//							DEMO_SetPIPViewWIND(0);	// update pip/pop window
					}
					else 
					{
						sys_status.current_split_mode = startChannel;
						//Set_border_line();
//						aux_display_flag = 1;
//						DEMO_SetPIPViewWIND(0);	// update pip/pop window

					}

					if((ON == osdDisplayOn) && (ON == titleDisplayOn))
					{
						Osd_EraseAllText();
						OSD_DisplayChannelName();
					}
				}
				break;
			}
		}
	}

	//if(vPre_vMODE != vMODE) bMode_change_flag = 1;
	//if(vPre_vStart_CH != vStart_CH) bMode_change_flag = 1;  

	//vPre_vMODE = vMODE;		
	previousStartChannel = startChannel;
}

