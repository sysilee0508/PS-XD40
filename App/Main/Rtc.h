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
extern void RTC_SetRtcUpdated(BOOL set);
extern BOOL RTC_IsRtcUpdatd(void);
//=============================================================================
//  Extern Grobal Variable 
//=============================================================================

extern BYTE sec_flag;


#endif
