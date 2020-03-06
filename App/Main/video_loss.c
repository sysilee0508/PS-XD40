#include "common.h"

#if BD_NVP == NVP6158
//#define VIDEO_LOSS_CHANNEL(x)				0x01 << x
#define VIDEO_LOSS_CHANNEL(x)				0x01 << (3-x)
extern void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss);
#elif BD_NVP == NVP6168
#include "raptor4_fmt.h"
#include "NVP6168.h"

extern NC_CH_E NVP_Ch[4];
#endif

static u8 videoLossChannels = VIDEO_LOSS_CHANNEL_NONE; /* 1:Loss 0:Video */
static BOOL videoLossEvent = CLEAR;
static u8 videoLossBuzzerCount = 0;

//-----------------------------------------------------------------------------
//	Video Loss Check
//-----------------------------------------------------------------------------

static u8 Get_VideoLossChannels(void)
{
	eChannel_t channel;
	//_VIVO_CH_FORMATDEF_E fmt;
	u8 lossChannels = 0;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		if(NVP6168_Current_VideoFormat_Get(channel) == NC_VIVO_CH_FORMATDEF_UNKNOWN)
		{
			//lossChannels |= (0x01) << NVP_Ch[channel];
			lossChannels |= (0x01) << channel;
 		}
 		else
 		{
 			OSD_ClearEvent(channel, EVT_NO_VIDOE);
 		}
 	}

	return lossChannels;
}


void ScanVideoLossChannels(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	static u8 previousLossChannels = VIDEO_LOSS_CHANNEL_ALL;
	u8 lossChannels = 0x00;
	u8 changedChannels = 0x0F;
	u8 newLoss = 0x00;
	eChannel_t channel;
	u8 index = 0;

	if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
	{
		videoLossChannels = VIDEO_LOSS_CHANNEL_NONE;
#if BD_NVP == NVP6158
		NVP6158_Video_Loss_Check(&videoLossChannels);
#elif BD_NVP == NVP6168
		videoLossChannels = Get_VideoLossChannels();
#endif
		lossChannels = videoLossChannels & 0x0F;
		changedChannels = previousLossChannels ^ lossChannels;
		if(changedChannels != 0)
		{
			SetVideoLossEvent(SET);
			newLoss = changedChannels & lossChannels;
			if(newLoss != (u8)VIDEO_LOSS_CHANNEL_NONE)
			{
				// There is new loss channel..
				// Should reset the buzzer count for video loss
				Read_NvItem_VideoLossBuzzerTime((u8*)&videoLossBuzzerCount);
				videoLossBuzzerCount *= 2;

				// set osd event of no video
				// fine new loss channel 
				for(index = 0; index < 4; index++)
				{
					if((newLoss >> index) & 0x01)
					{
						channel = (eChannel_t)index;
						OSD_SetEvent(channel, EVT_NO_VIDOE);
					}
				}
			}
			else
			{
				videoLossBuzzerCount = 0;
			}
			
			if(GetCurrentAutoSeq() == AUTO_SEQ_NORMAL)
			{
				UpdateAutoSeqDisplayTime();
			}
		}
		//OSD_DisplayVideoMode();
		previousLossChannels = lossChannels;
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
	}
}
//-----------------------------------------------------------------------------
BOOL IsVideoLossChannel(eChannel_t channel)
{
#if BD_NVP == NVP6158
	return ((videoLossChannels & VIDEO_LOSS_CHANNEL(channel)) == 0)?FALSE:TRUE;

#elif BD_NVP == NVP6168

 	BOOL ret = FALSE;
	BYTE format = GetInputVideoFormat(channel);

	if((format == NC_VIVO_CH_FORMATDEF_UNKNOWN) || (format == NC_VI_SIGNAL_ON))
	{
		ret = TRUE;
	}

	return ret;
	
#endif
}
//-----------------------------------------------------------------------------
u8 GetVideoLossChannels(void)
{
	return (videoLossChannels & 0x0F);
}
//-----------------------------------------------------------------------------
void SetVideoLossEvent(BOOL event)
{
	videoLossEvent = event;
}
//-----------------------------------------------------------------------------
BOOL GetVideoLossEvent(void)
{
	return videoLossEvent;
}
//-----------------------------------------------------------------------------
void InitVideoLossCheck(void)
{
	SetVideoLossEvent(CLEAR);
	videoLossBuzzerCount = 0;
}
//-----------------------------------------------------------------------------
u8 GetVideoLossBuzzerCount(void)
{
	return videoLossBuzzerCount;
}

void ClearVideoLossBuzzerCount(void)
{
	videoLossBuzzerCount = 0;
}

void DecreaseVideoLossBuzzerCount(void)
{
	videoLossBuzzerCount--;
}
