#include "common.h"
#include "video_loss.h"

extern void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss);
//-----------------------------------------------------------------------------
//	Video Loss Check
//-----------------------------------------------------------------------------
u32 vVideo_Loss = 0; //1:Loss 0:Video
u8 Loss_Event_Flag = 0;
u8 Loss_Buzzer_Cnt = 0;


void Loss_Buzzer(void)
{
	static u32 timeout= 0;

	if(!TIME_AFTER(tick_10ms,timeout))
		return;

	timeout = tick_10ms + 50; // 10ms * 50 = 500ms

	if(Loss_Buzzer_Cnt)
	{
		if(Loss_Buzzer_Cnt%2) BUZZER_LOW;
		else BUZZER_HIGH;

		if(Loss_Buzzer_Cnt > 0) Loss_Buzzer_Cnt--;

		if(Loss_Buzzer_Cnt == 0) BUZZER_LOW;
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
		Read_NvItem_VideoLossBuzzerTime(&Loss_Buzzer_Cnt);
		Loss_Buzzer_Cnt *= 2;
	}

	Pre_Video_Loss = vVideo_Loss;
}

void Video_Loss_Check(void)
{
    static u32 timeout = 0;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 50; // 10ms * 50 = 500ms

	Loss_Check();
}


u8 Find_Video_Loss_Channel(void)
{
	return (u8)(vVideo_Loss & 0x0000000F);
}
