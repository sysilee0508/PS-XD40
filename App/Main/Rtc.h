#ifndef __RTC__
#define __RTC__

//=============================================================================
//  
//=============================================================================
struct tm 
{
	BYTE tm_sec;   
	BYTE tm_min;   
	BYTE tm_hour;  

	BYTE tm_mday;  
	BYTE tm_mon;   
	BYTE tm_year;  
};

//=============================================================================
//  Function Prototype
//=============================================================================
void Change_RTC_Cnt(struct tm *time);
void Time_Read(void); 

//=============================================================================
//  Extern Grobal Variable 
//=============================================================================
extern BYTE rtc_year;
extern BYTE rtc_month;
extern BYTE rtc_day;
extern BYTE rtc_hour;
extern BYTE rtc_min;
extern BYTE rtc_sec;

extern BYTE rtc_sec_update_flag;
extern BYTE sec_flag;


#endif
