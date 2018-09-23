#ifndef __RTC_H__
#define __RTC_H__

//=============================================================================
//  Defines
//=============================================================================
#define SECS_IN_MIN				60
#define MINS_IN_HOUR			60
#define HOURS_IN_DAY			24
#define SECS_IN_HOUR			3600//SECS_IN_MIN * MINS_IN_HOUR	//3600
#define SECS_IN_DAY				86400//SECS_IN_HOUR * HOURS_IN_DAY	//86400
#define DAYS_IN_YEAR 			365
#define DAYS_IN_LEAPYEAR		366
// DEFAUTL DATE : 2018-JAN-1
#define DEFAULT_YEAR			18
#define DEFAULT_MONTH			1
#define DEFAULT_DAY				1

//=============================================================================
//  Type definitions
//=============================================================================

typedef struct
{
	u8 sec;
	u8 min;
	u8 hour;
	u8 day;
	u8 month;
	u8 year;
} sTimeDate_t;

//=============================================================================
//  Function Prototype
//=============================================================================
extern void RTC_Configuration(void);
extern void RTC_IRQHandler(void);

extern void RTC_SetTime(sTimeDate_t *time);
extern void RTC_GetTime(sTimeDate_t* rtcTimeDate);
extern void RTC_CheckTime(void);
extern void RTC_SetDefaultDate(void);

extern void RTC_ChangeRtcTimeStatus(BOOL set);
extern BOOL RTC_GetRtcTimeStatus(void);
extern void RTC_ChangeDisplayTimeStatus(BOOL set);
extern BOOL RTC_GetDisplayTimeStatus(void);

extern u8 GetDaysInMonth(u8 month, u16 year);

#endif
