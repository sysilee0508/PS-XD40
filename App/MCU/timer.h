#ifndef __TIMER_H__
#define __TIMER_H__

// Macros --------------------------------------------------
#define TIME_AFTER(current,previous,timegap)		((current-previous)%timegap == 0)?TRUE:FALSE

// Typedef -------------------------------------------------
typedef struct
{
	u8 tickCount_1ms;
	u8 tickCount_10ms;
	u8 tickCount_100ms;
	u32 tickCount_1s;
} sSystemTick_t;

// Function Declaration -------------------------------------
extern sSystemTick_t* GetSystemTime(void);

#endif
