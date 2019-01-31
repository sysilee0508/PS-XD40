//=============================================================================
//  Header Declaration
//=============================================================================
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================
static BOOL updateDisplayTime = CLEAR;
static BOOL updatedRTCTime = CLEAR;

//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================
void RTC_Configuration(void)
{
	// Enable PWR and BKP clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	// Allow access to BKP Domain
	PWR_BackupAccessCmd(ENABLE);

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		// Reset Backup Domain
		BKP_DeInit();		//RTC related register reset (RTC related registers are not reset in system reset)
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}

	// Enable LSE
	RCC_LSEConfig(RCC_LSE_ON);

	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	// Select the RTC Clock Source
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	// Enable the RTC Clock
	RCC_RTCCLKCmd(ENABLE);

	// RTC configuration
	// Wait for RTC APB registers synchronization
	RTC_WaitForSynchro();
	// Wait until last write operation on RTC registers has finished
	RTC_WaitForLastTask();

	// Set the RTC time base to 1s
	RTC_SetPrescaler(32767); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
	// Wait until last write operation on RTC registers has finished
	RTC_WaitForLastTask();

	// Enable the RTC Second
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	// Wait until last write operation on RTC registers has finished
	RTC_WaitForLastTask();
}

//-----------------------------------------------------------------------------
//	Process RTC Interrupt
//-----------------------------------------------------------------------------
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		// Clear the RTC Second interrupt
		RTC_ClearITPendingBit(RTC_IT_SEC);
		RTC_ChangeRtcTimeStatus(SET);
		// Wait until last write operation on RTC registers has finished
		RTC_WaitForLastTask();
	}
}

static BOOL IsLeapYear(u16 year)
{
	BOOL leapYear = FALSE;

	year += DEFAULT_YEAR;

	if((((year % 4) == 0) && ((year % 100 ) != 0))||((year % 400) == 0))
	{
		leapYear = TRUE;
	}

	return leapYear;
}
//----------------------------------------------------------------------------
u8 GetDaysInMonth(u8 month, u16 year)
{
	u8 days;

	if((month == 4) || (month == 6) || (month == 9) || (month == 11))
	{
		days = 30;
	}
	else if((month == 2) && (IsLeapYear(year) == TRUE))
	{
		days = 29;
	}
	else if((month == 2) && (IsLeapYear(year) == FALSE))
	{
		days = 28;
	}
	else
	{
		days = 31;
	}

	return days;
}

static void RTC_CheckBoundaryCondition(sTimeDate_t* newTimeDate)
{
	if(newTimeDate->sec >= SECS_IN_MIN)
	{
		newTimeDate->sec -= SECS_IN_MIN;
		newTimeDate->min++;
	}
	if(newTimeDate->min >= MINS_IN_HOUR)
	{
		newTimeDate->min -= MINS_IN_HOUR;
		newTimeDate->hour++;
	}
	if(newTimeDate->hour >= HOURS_IN_DAY)
	{
		newTimeDate->hour -= HOURS_IN_DAY;
		newTimeDate->day++;
	}
	if(newTimeDate->day > GetDaysInMonth(newTimeDate->month, newTimeDate->year))
	{
		newTimeDate->day -= GetDaysInMonth(newTimeDate->month, newTimeDate->year);
		newTimeDate->month++;
	}
	else if(newTimeDate->day == 0)
	{
		newTimeDate->day = 1;
	}
	if(newTimeDate->month > 12)
	{
		newTimeDate->month -= 12;
		newTimeDate->year++;
	}
	else if(newTimeDate->month == 0)
	{
		newTimeDate->month = 1;
	}
}
//----------------------------------------------------------------------------
static void RTC_CalculateTimeDate(u32 count, sTimeDate_t* pTimeDate)
{
	u32 days = 0;
	u32 secs = 0;
	u8 index = 0;;
	sTimeDate_t timeDate = {0,};

	days = (count / SECS_IN_DAY);//SECS_IN_DAY; //days
	secs = (count % SECS_IN_DAY); // secs

	index = 0;
	timeDate.day = DEFAULT_DAY;
	timeDate.month = DEFAULT_MONTH;
	timeDate.year = 0;
	// Calculate year
	while(days > DAYS_IN_YEAR)
	{
		timeDate.year++;
		if(IsLeapYear(index) == TRUE)
		{
			days -= DAYS_IN_LEAPYEAR;
		}
		else
		{
			days -= DAYS_IN_YEAR;
		}
	}
	// Calculate month
	if(days == DAYS_IN_YEAR)
	{
		timeDate.year++;
	}
	else
	{
		while(days > GetDaysInMonth(timeDate.month, timeDate.year))
		{
			days -= GetDaysInMonth(timeDate.month, timeDate.year);
			timeDate.month++;
		}
		timeDate.day = days + DEFAULT_DAY;
	}

    // Calculate hours, minutes, seconds
    timeDate.hour = secs / SECS_IN_HOUR;
    timeDate.min = (secs % SECS_IN_HOUR) / SECS_IN_MIN ;
    timeDate.sec = (secs % SECS_IN_HOUR) % SECS_IN_MIN ;

	memcpy(pTimeDate, &timeDate, sizeof(timeDate));
}
//----------------------------------------------------------------------------
void RTC_GetTime(sTimeDate_t* rtcTimeDate)
{ 
	u32 rtcCount = RTC_GetCounter();
//	sTimeCorrect_t timeCorrection;
	sTimeDate_t timeDate;
	static sTimeDate_t oldTimeDate;

	if(RTC_GetRtcTimeStatus() == SET)
	{
		RTC_ChangeRtcTimeStatus(CLEAR);
		RTC_ChangeDisplayTimeStatus(SET);

		RTC_CalculateTimeDate(rtcCount, &timeDate);
		//Time correction
//		Read_NvItem_TimeCorrect(&timeCorrection);
//		if(timeCorrection.timeCorrectOffset > 0)
//		{
//			if(((timeCorrection.timeCorrectUint == TIME_UNIT_DAY) && (oldTimeDate.day != timeDate.day)) ||
//				((timeCorrection.timeCorrectUint == TIME_UNIT_MONTH) &&	(oldTimeDate.month != timeDate.month)))
//			{
//				if(timeCorrection.timeCorrectDirection == DIRECTION_UP) // + --> up / - --> down
//				{
//					rtcCount += timeCorrection.timeCorrectOffset;
//				}
//				else if(timeCorrection.timeCorrectDirection == DIRECTION_DOWN)// -
//				{
//					if(rtcCount > timeCorrection.timeCorrectOffset)
//					{
//						rtcCount -= timeCorrection.timeCorrectOffset;
//					}
//					else
//					{
//						rtcCount = 0;
//					}
//				}
//			    RTC_WaitForLastTask();
//			    RTC_SetCounter(rtcCount);
//			    RTC_WaitForLastTask();
//
//			    RTC_CalculateTimeDate(rtcCount, &timeDate);
//			}
//		}
		// boundary condition
		RTC_CheckBoundaryCondition(&timeDate);

		memcpy(rtcTimeDate, &timeDate, sizeof(timeDate));
		oldTimeDate = timeDate;
	}
	else
	{
		memcpy(rtcTimeDate, &oldTimeDate, sizeof(timeDate));
	}
}

void RTC_SetTime(sTimeDate_t* newTimeDate)
{
	u32 count;
	u16 days = 0;
	u8 index;

	// boundary condition
	RTC_CheckBoundaryCondition(newTimeDate);

	//day
	if(newTimeDate->day > DEFAULT_DAY)
	{
		days = newTimeDate->day - DEFAULT_DAY;
	}
	//month
	if(newTimeDate->month > DEFAULT_MONTH)
	{
		for(index = DEFAULT_MONTH; index < newTimeDate->month; index++)
		{
			days += GetDaysInMonth(index, newTimeDate->year);
		}
	}
	//year
	if(newTimeDate->year > 0)
	{
		for(index = 0; index < newTimeDate->year; index++)
		{
			if(IsLeapYear(index) == TRUE)
			{
				days += DAYS_IN_LEAPYEAR;
			}
			else
			{
				days += DAYS_IN_YEAR;
			}
		}
	}
	//time
	count = (days * SECS_IN_DAY) +
			(newTimeDate->hour * SECS_IN_HOUR) +
			(newTimeDate->min * SECS_IN_MIN) +
			(newTimeDate->sec);
	//set rtc
    RTC_WaitForLastTask();
    RTC_SetCounter(count);
    RTC_WaitForLastTask();
}

//----------------------------------------------------------------------------
void RTC_ChangeRtcTimeStatus(BOOL set)
{
	updatedRTCTime = set;
}
//----------------------------------------------------------------------------
BOOL RTC_GetRtcTimeStatus(void)
{
	return updatedRTCTime;
}
//----------------------------------------------------------------------------
void RTC_ChangeDisplayTimeStatus(BOOL set)
{
	updateDisplayTime = set;
}
//----------------------------------------------------------------------------
BOOL RTC_GetDisplayTimeStatus(void)
{
	return updateDisplayTime;
}
//----------------------------------------------------------------------------
void RTC_CheckTime(void)
{
	sTimeDate_t timeDate;

	RTC_GetTime(&timeDate);
}
//----------------------------------------------------------------------------
// Please call this function when NV is initialized.
void RTC_SetDefaultDate(void)
{
	sTimeDate_t timeDate;

	memset(&timeDate, 0x00, sizeof(timeDate));

	timeDate.year = 0; // default year
	timeDate.month = DEFAULT_MONTH;
	timeDate.day = DEFAULT_DAY;

	RTC_SetTime(&timeDate);
}
