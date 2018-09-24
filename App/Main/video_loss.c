#include "common.h"
#include "video_loss.h"

extern void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss);
//-----------------------------------------------------------------------------
//	Video Loss Check
//-----------------------------------------------------------------------------
u32 vVideo_Loss = 0; /* 1:Loss 0:Video */
u8 Loss_Event_Flag = 0;
u32 videoLossBuzzerCount = 0;
u32 alarmBuzzerCountIn500ms = 0;


void Loss_Buzzer(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;
	u32 buzzerCount = MAX(videoLossBuzzerCount, alarmBuzzerCountIn500ms);

	if(TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn100ms,5))
	{
		if(buzzerCount > 0)
		{
			if(buzzerCount%2)
				BUZZER_LOW;
			else
				BUZZER_HIGH;

			if(videoLossBuzzerCount > 0)
			{
				videoLossBuzzerCount--;
			}
			if(alarmBuzzerCountIn500ms > 0)
			{
				alarmBuzzerCountIn500ms--;
			}
		}
		else
		{
			BUZZER_LOW;
		}
		previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
	}
}

void Loss_Check(void)
{
	static u32 Pre_Video_Loss = 0xFFFFFFFF;

	vVideo_Loss = 0;

	NVP6158_Video_Loss_Check(&vVideo_Loss);

	if(Pre_Video_Loss != vVideo_Loss)
	{
		Loss_Event_Flag = 1;
	}

	if(Pre_Video_Loss < vVideo_Loss)
	{
		Read_NvItem_VideoLossBuzzerTime((u8*)&videoLossBuzzerCount);
		videoLossBuzzerCount *= 2;
	}

	Pre_Video_Loss = vVideo_Loss;
}

void Video_Loss_Check(void)
{
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn100ms = 0;

    if(TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn100ms,5))
    {
    	Loss_Check();
    	previousSystemTimeIn100ms = currentSystemTime->tickCount_100ms;
    }
}


u8 Find_Video_Loss_Channel(void)
{
	return (u8)(vVideo_Loss & 0x0000000F);
}
