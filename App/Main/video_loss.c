#include "common.h"

#if BD_NVP == NVP6158
extern void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss);
#elif BD_NVP == NVP6168
#include "raptor4_fmt.h"
#include "NVP6168.h"

#endif

static u8 videoLossChannels = VIDEO_LOSS_CHANNEL_NONE; /* 1:Loss 0:Video */
static BOOL videoLossEvent = CLEAR;


//-----------------------------------------------------------------------------
//	Video Loss Check
//-----------------------------------------------------------------------------
#if BD_NVP == NVP6168
static u8 Get_VideoLossChannels(void)
{
	eChannel_t channel;
	//NC_VIVO_CH_FORMATDEF_E fmt;
	u8 lossChannels = 0;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		if(NVP6168_Current_VideoFormat_Get(channel) == NC_VIVO_CH_FORMATDEF_UNKNOWN)
		{
			lossChannels |= (0x01) << channel;
		}
	}

	return lossChannels;
}
#endif

void ScanVideoLossChannels(void)
{
	static u8 previousLossChannels = (u8)VIDEO_LOSS_CHANNEL_ALL;
	u8 lossChannels = 0x00;
	u8 changedChannels = 0x0F;

    	videoLossChannels = VIDEO_LOSS_CHANNEL_NONE;
#if BD_NVP == NVP6158
	NVP6158_Video_Loss_Check(&videoLossChannels);
#elif BD_NVP == NVP6168
	videoLossChannels = Get_VideoLossChannels();
#endif
    	lossChannels = (u8)videoLossChannels & 0x0F;
	changedChannels = previousLossChannels ^ lossChannels;
    	if(changedChannels != 0)
    	{
    		SetVideoLossEvent(SET);
   	}
}
//-----------------------------------------------------------------------------
BOOL IsVideoLossChannel(eChannel_t channel)
{
#if BD_NVP == NVP6158
	return ((videoLossChannels & VIDEO_LOSS_CHANNEL(channel)) == 0)?FALSE:TRUE;
#elif BD_NVP == NVP6168
	return (videoLossChannels & (0x01 << channel)) >> channel;
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
}
