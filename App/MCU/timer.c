#include "common.h"

//global variables --> should be changed to static
static sSystemTick_t systemTick;

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 1ms
//-----------------------------------------------------------------------------
void TIM2_Init(void)
{
	TIM2->CR1 = 0x0005;				// up-counter enable
	TIM2->PSC = 639;
	TIM2->ARR = 99;					// 64MHz/(1+639)/(1+99) = 1000Hz
	TIM2->SR = 0x0000;				// clear TIM2 interrupt flags
	TIM2->DIER = 0x0001;			// enable TIM2 update interrupt
}

void TIM2_IRQHandler(void)
{
	TIM2->SR = TIM2->SR & 0xFFFE;			// clear TIM2 update interrupt flag

	systemTick.tickCount_1ms++;
	if(systemTick.tickCount_1ms%10 == 0)
	{
		systemTick.tickCount_10ms++;
	}
	if(systemTick.tickCount_1ms%100 == 0)
	{
		systemTick.tickCount_100ms++;
	}
	if(systemTick.tickCount_1ms%1000 == 0)
	{
		systemTick.tickCount_1s++;
	}
}

//-----------------------------------------------------------------------------
//	Process Timer interrupt with 10ms - This is to handle keys and LEDs
//-----------------------------------------------------------------------------
void TIM3_Init(void)
{
	TIM3->CR1 = 0x0005;				// up-counter enable
	TIM3->PSC = 639;
	TIM3->ARR = 999;					// 64MHz/(1+639)/(1+999) = 100Hz  --> 10ms
	TIM3->SR = 0x0000;				// clear TIM3 interrupt flags
	TIM3->DIER = 0x0001;			// enable TIM3 update interrupt
}

void TIM3_IRQHandler(void)
{
	static u8 count;

	TIM3->SR = TIM3->SR & 0xFFFE;			// clear TIM2 update interrupt flag

	Key_Scan();
	Key_Led_Ctrl();
	Key_Check();

	// Check alarm every 20ms if alarm is enabled
	if(count%2==0)
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
