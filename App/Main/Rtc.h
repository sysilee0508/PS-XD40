#ifndef __RTC__
#define __RTC__

//=============================================================================
//  
//=============================================================================
typedef struct
{
	BYTE sec;
	BYTE min;
	BYTE hour;

	BYTE day;
	BYTE month;
	BYTE year;
} sTimeDate_t;

// BCD Type
typedef struct
{
	BYTE sec;
	BYTE min;
	BYTE hour;
	BYTE day;
	BYTE month;
	BYTE year;
} sTimeDateBCD_t;

//=============================================================================
//  Function Prototype
//=============================================================================
extern void RTC_ChangeCount(sTimeDate_t *time);
extern void Time_Read(void);
extern void GetTimeDateInBCD(sTimeDateBCD_t* pData);
extern void RTC_ChangeRtcTimeStatus(BOOL set);
extern BOOL RTC_GetRtcTimeStatus(void);
extern void RTC_ChangeDisplayTimeStatus(BOOL set);
extern BOOL RTC_GetDisplayTimeStatus(void);

#endif
