//=============================================================================
//  Header Declaration
//=============================================================================
#include "common.h"

#define SECS_IN_MIN			60
#define MINS_IN_HOUR		60
#define HOURS_IN_DAY		24
#define SECS_IN_HOUR		SECS_IN_MIN * MINS_IN_HOUR	//3600
#define SECS_IN_DAY			SECS_IN_HOUR * HOURS_IN_DAY	//86400
#define MONTHS_OF_YEAR		12

#define MAX_YEAR_COUNT		100

//=============================================================================
//  Global Variable Declaration
//=============================================================================
static BOOL updateDisplayTime = CLEAR;
static BOOL updatedRTCTime = CLEAR;
DWORD time_value = 0;
WORD date_value = 0;

static sTimeDateBCD_t rtcTimeDate;

//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
const BYTE month_normal_table[MONTHS_OF_YEAR] =
{
	31,28,31,30,31,30,31,31,30,31,30,31
};

const BYTE month_leap_table[MONTHS_OF_YEAR] =
{
	31,29,31,30,31,30,31,31,30,31,30,31
};

const WORD year_table[MAX_YEAR_COUNT] =
{
	366,365,365,365, //2000~2003
	366,365,365,365, //2004~2007
	366,365,365,365, //2008~2011
	366,365,365,365, //2012~2015
	366,365,365,365, //2016~2019

	366,365,365,365, //2020~2023
	366,365,365,365, //2024~2027
	366,365,365,365, //2028~2031
	366,365,365,365, //2032~2035
	366,365,365,365, //2036~2039

	366,365,365,365, //2040~2043
	366,365,365,365, //2044~2047
	366,365,365,365, //2048~2051
	366,365,365,365, //2052~2055
	366,365,365,365, //2056~2059

	366,365,365,365, //2060~2063
	366,365,365,365, //2064~2067
	366,365,365,365, //2068~2071
	366,365,365,365, //2072~2075
	366,365,365,365, //2076~2079

	366,365,365,365, //2080~2083
	366,365,365,365, //2084~2087
	366,365,365,365, //2088~2091
	366,365,365,365, //2092~2095
	366,365,365,365, //2096~2099
};


//=============================================================================
//  Function Definition
//=============================================================================
//-----------------------------------------------------------------------------
//	16������ BCD�ڵ�� ��ȯ
//-----------------------------------------------------------------------------
static BYTE Hex2Bcd(BYTE hex_num)
{
	return (((hex_num/10)<<4)|(hex_num%10));
}


//-----------------------------------------------------------------------------
//	���ϰ����� �� ����� ���� ����ؼ� �������� ������.  
//-----------------------------------------------------------------------------
static void RTC_CalculateDate(WORD ymd_val)
{
	BYTE index;
	BYTE year_val = 0;
	BYTE month_val = 0;
	BYTE day_val = 0;
	WORD tmp_val = 0;


	tmp_val = ymd_val; 

	for(index = 0; index < MAX_YEAR_COUNT; index++)
	{
		if(tmp_val < year_table[index])
		{
			year_val = index;
			break;
		}	
		tmp_val -= year_table[index];
	}
	// Is it leap year
	if((year_val % 4) == 0)
	{
		for(index = 0;index < MONTHS_OF_YEAR; index++)
		{
			if(tmp_val < month_leap_table[index])
			{
				month_val = index+1;
				break;
			}	
			tmp_val -= month_leap_table[index];
		}	
	}
	else 
	{
		for(index=0;index<MONTHS_OF_YEAR;index++)
		{
			if(tmp_val < month_normal_table[index])
			{
				month_val = index+1;
				break;
			}	
			tmp_val -= month_normal_table[index];
		}	
	}
	day_val = tmp_val+1;

	rtcTimeDate.year  = Hex2Bcd(year_val);
	rtcTimeDate.month = Hex2Bcd(month_val);
	rtcTimeDate.day   = Hex2Bcd(day_val);
}

void RTC_ChangeCount(sTimeDate_t *time)
{
	BYTE index;
	DWORD rtcCount = 0;

	for(index = 0; index < time->year; index++)
	{
		rtcCount += year_table[index];
	}		

	if((time->year % 4) == 0) //leap year
	{
		for(index = 0; index < time->month ; index++)
		{
			rtcCount += month_leap_table[index];
		}		
	}
	else 
	{
		for(index = 0; index < time->month; index++)
		{
			rtcCount += month_normal_table[index];
		}		
	}
		
	rtcCount += time->day;
	rtcCount *= SECS_IN_DAY;
	rtcCount += (time->hour*SECS_IN_HOUR);
	rtcCount += (time->min*SECS_IN_MIN);
	rtcCount += time->sec;

    RTC_WaitForLastTask();	
    RTC_SetCounter(rtcCount);
    RTC_WaitForLastTask();	
}

void Time_Read(void) 
{ 
//	static WORD sub_cnt;
//	static BYTE sub_flag;
	u32 rtcCount = RTC_GetCounter();
	BYTE month_tmp, day_tmp;
	sTimeCorrect_t timeCorrection;

	if(RTC_GetRtcTimeStatus() == SET)
	{
		RTC_ChangeRtcTimeStatus(CLEAR);
		RTC_ChangeDisplayTimeStatus(SET);//updateDisplayTime = SET;

		date_value = rtcCount / SECS_IN_DAY;
		time_value = rtcCount % SECS_IN_DAY;

		month_tmp = rtcTimeDate.month;
		day_tmp = rtcTimeDate.day;
		RTC_CalculateDate(date_value);

	    // Compute  time
		rtcTimeDate.hour = Hex2Bcd(time_value/SECS_IN_HOUR);
		rtcTimeDate.min = Hex2Bcd((time_value%SECS_IN_HOUR)/SECS_IN_MIN);
		rtcTimeDate.sec = Hex2Bcd((time_value%SECS_IN_HOUR)%SECS_IN_MIN);

		//Time correction
		Read_NvItem_TimeCorrect(&timeCorrection);
		if(timeCorrection.timeCorrecOffset > 0)
		{
			if(((timeCorrection.timeCorrectUint == TIME_UNIT_DAY) && (day_tmp != rtcTimeDate.day)) ||
				((timeCorrection.timeCorrectUint == TIME_UNIT_MONTH) &&	(month_tmp != rtcTimeDate.month)))
			{
				if(timeCorrection.timeCorrectDirection == DIRECTION_UP) // + --> up / - --> down
				{
					rtcCount += +timeCorrection.timeCorrecOffset;
				}
				else if(timeCorrection.timeCorrectDirection == DIRECTION_DOWN)// -
				{
					if(rtcCount > timeCorrection.timeCorrecOffset)
					{
						rtcCount -= timeCorrection.timeCorrecOffset;
					}
					else
					{
						rtcCount = 0;
					}
				}	
			    RTC_WaitForLastTask();
			    RTC_SetCounter(rtcCount);
			    RTC_WaitForLastTask();

				date_value = rtcCount / SECS_IN_DAY;
				RTC_CalculateDate(date_value);
				time_value = rtcCount % SECS_IN_DAY;
				rtcTimeDate.hour = Hex2Bcd(time_value / SECS_IN_HOUR);
				rtcTimeDate.min = Hex2Bcd((time_value % SECS_IN_HOUR)/SECS_IN_MIN);
				rtcTimeDate.sec = Hex2Bcd((time_value % SECS_IN_HOUR)%SECS_IN_MIN);
			}
		}
	}
}

void GetTimeDateInBCD(sTimeDateBCD_t* pData)
{
	memcpy(pData, &rtcTimeDate, sizeof(rtcTimeDate));
}

void RTC_ChangeRtcTimeStatus(BOOL set)
{
	updatedRTCTime = set;
}

BOOL RTC_GetRtcTimeStatus(void)
{
	return updatedRTCTime;
}

void RTC_ChangeDisplayTimeStatus(BOOL set)
{
	updateDisplayTime = set;
}

BOOL RTC_GetDisplayTimeStatus(void)
{
	return updateDisplayTime;
}
