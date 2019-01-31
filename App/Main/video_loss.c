#include "common.h"

extern void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss);

static u32 videoLossChannels = VIDEO_LOSS_CHANNEL_NONE; /* 1:Loss 0:Video */
static BOOL videoLossEvent = CLEAR;
static u8 videoLossBuzzerCount = 0;


//-----------------------------------------------------------------------------
//	Video Loss Check
//-----------------------------------------------------------------------------
void ScanVideoLossChannels(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	static u8 previousLossChannels = (u8)VIDEO_LOSS_CHANNEL_ALL;
	u8 lossChannels = 0x00;
	u8 changedChannels = 0x0F;

    if(TIME_AFTER(currentSystemTime->tickCount_100ms, previousSystemTimeIn100ms,5))
    {
    	videoLossChannels = VIDEO_LOSS_CHANNEL_NONE;
    	NVP6158_Video_Loss_Check(&videoLossChannels);
    	lossChannels = (u8)videoLossChannels & 0x0F;
		changedChannels = previousLossChannels ^ lossChannels;
    	if(changedChannels != 0)
    	{
    		SetVideoLossEvent(SET);
    		if((changedChannels & lossChannels) != (u8)VIDEO_LOSS_CHANNEL_NONE)
    		{
    			// There is new loss channel..
    			// Should reset the buzzer count for video loss
    			Read_NvItem_VideoLossBuzzerTime((u8*)&videoLossBuzzerCount);
    			videoLossBuzzerCount *= 2;
    		}
//    		if(GetCurrentAutoSeq() == AUTO_SEQ_NORMAL)
//    		{
//    			UpdateAutoSeqDisplayTime();
//    		}
    	}
    	previousLossChannels = lossChannels;
    	previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
    }
}
//-----------------------------------------------------------------------------
BOOL IsVideoLossChannel(eChannel_t channel)
{
	return (((u8)videoLossChannels & VIDEO_LOSS_CHANNEL(channel)) == 0)?FALSE:TRUE;
}
//-----------------------------------------------------------------------------
u8 GetVideoLossChannels(void)
{
	return (u8)(videoLossChannels & 0x0000000F);
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
