#include "common.h"

//global variables --> should be changed to static
static sSystemTick_t systemTick;

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 1ms
//-----------------------------------------------------------------------------
void TIM2_IRQHandler(void)
{
	static unsigned int cnt;

	TIM2->SR = TIM2->SR & 0xFFFE;			// clear TIM2 update interrupt flag

	systemTick.tickCount_1ms++;
	if(systemTick.tickCount_1ms/10 > 0)
	{
		systemTick.tickCount_10ms++;
		systemTick.tickCount_1ms %= 10;
	}
	if(systemTick.tickCount_10ms/10 > 0)
	{
		systemTick.tickCount_100ms++;
		systemTick.tickCount_10ms %= 10;
	}
	if(systemTick.tickCount_100ms/10 > 0)
	{
		systemTick.tickCount_1s++;
		systemTick.tickCount_100ms %= 10;
	}
}

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 10ms - This is to handle keys and LEDs
//-----------------------------------------------------------------------------
void TIM3_IRQHandler(void)
{
	static u8 count;
	u8 i;

	TIM3->SR = TIM3->SR & 0xFFFE;			// clear TIM2 update interrupt flag

	Key_Scan();
	Key_Led_Ctrl();
	Key_Check();

	// Check alarm every 20ms if alarm is enabled
	if((count%2==0) && (GetAlarmRemoteKeyMode() == ALARM_MODE))
	{
		CheckAlarm();
	}

	count = (++count)%2;
}

//-----------------------------------------------------------------------------
sSystemTick_t* GetSystemTime(void)
{
	return &systemTick;
}
