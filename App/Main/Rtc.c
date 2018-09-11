//=============================================================================
//  Header Declaration
//=============================================================================
#include "common.h"

//=============================================================================
//  Global Variable Declaration
//=============================================================================
BYTE rtc_year;
BYTE rtc_month;
BYTE rtc_day;
BYTE rtc_hour;
BYTE rtc_min;
BYTE rtc_sec;

BYTE sec_flag = 0;
BYTE rtc_sec_update_flag = 0;
DWORD time_value = 0;
WORD date_value = 0;

#define SECS_IN_MIN			60
#define MINS_IN_HOUR		60
#define HOURS_IN_DAY		24
#define SECS_IN_HOUR		SECS_IN_MIN * MINS_IN_HOUR	//3600
#define SECS_IN_DAY			SECS_IN_HOUR * HOURS_IN_DAY	//86400
#define MONTHS_OF_YEAR		12

#define MAX_YEAR_COUNT		100

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
BYTE Hex2Bcd(BYTE hex_num) 
{
	return (((hex_num/10)<<4)|(hex_num%10));
}


//-----------------------------------------------------------------------------
//	���ϰ����� �� ����� ���� ����ؼ� �������� ������.  
//-----------------------------------------------------------------------------
void RTC_Cnt_Calc(WORD ymd_val)
{
	BYTE index;
	BYTE year_val = 0;
	BYTE month_val = 0;
	BYTE day_val = 0;
	WORD tmp_val = 0;

	tmp_val = ymd_val; 

	for(index=0; index<MAX_YEAR_COUNT; index++)
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
		for(index=0;index<MONTHS_OF_YEAR;index++)
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

	rtc_year  = Hex2Bcd(year_val);
	rtc_month = Hex2Bcd(month_val);
	rtc_day   = Hex2Bcd(day_val);
}

//��ü �ð����� �޾� �ʴ��� ������ ����ؼ� RTC ī���Ϳ� �ִ´�. 
void Change_RTC_Cnt(struct tm *time)
{
	BYTE index;
	LONG tmp_val = 0;

	//�� ���ϱ�
	for(index=0;index<time->tm_year;index++)
	{
		tmp_val += year_table[index];
	}		

	//�� ���ϱ�
	if((time->tm_year%4) == 0)	//������ ��
	{
		for(index=0;index<time->tm_mon;index++)
		{
			tmp_val += month_leap_table[index];
		}		
	}
	else 
	{
		for(index=0;index<time->tm_mon;index++)
		{
			tmp_val += month_normal_table[index];
		}		
	}
		
	//�� ���ϱ�
	tmp_val += time->tm_mday;
	tmp_val *= SECS_IN_DAY;

	//�� ���ϱ�
	tmp_val += (time->tm_hour*SECS_IN_HOUR);

	//�� ���ϱ�
	tmp_val += (time->tm_min*SECS_IN_MIN);

	//�� ���ϱ�
	tmp_val += time->tm_sec; 

    RTC_WaitForLastTask();	
    RTC_SetCounter(tmp_val);
    RTC_WaitForLastTask();	
}

void Time_Read(void) 
{ 
	static WORD sub_cnt; 
	static BYTE sub_flag;
	BYTE month_tmp, day_tmp;
	sTimeCorrect_t timeCorrection;

	//if((!bSETUP) || (vPAGE != 1) || (vITEM_Y != 0) || (!bENTER))
	{
		if(rtc_sec_update_flag)
		{
			rtc_sec_update_flag = 0;
			sec_flag = 1;

			date_value = RTC_GetCounter() / SECS_IN_DAY;
			time_value = RTC_GetCounter() % SECS_IN_DAY;

			month_tmp = rtc_month;	//time correction ���� ���
			day_tmp = rtc_day;		//time correction ���� ���

			RTC_Cnt_Calc(date_value);
		    // Compute  hours
			rtc_hour = Hex2Bcd(time_value/SECS_IN_HOUR);
			// Compute minutes
			rtc_min = Hex2Bcd((time_value%SECS_IN_HOUR)/SECS_IN_MIN);
		    // Compute seconds
			rtc_sec = Hex2Bcd((time_value%SECS_IN_HOUR)%SECS_IN_MIN);

			//Time correction
			Read_NvItem_TimeCorrect(&timeCorrection);
			if(timeCorrection.timeCorrecOffset > 0)
			{
				if(timeCorrection.timeCorrectUint == TIME_UNIT_DAY) //Daliy
				{
					if(day_tmp != rtc_day)
					{
						if(timeCorrection.timeCorrectDirection == DIRECTION_UP) // + --> up / - --> down
						{
						    RTC_WaitForLastTask();	
						    RTC_SetCounter(RTC_GetCounter()+timeCorrection.timeCorrecOffset);
						    RTC_WaitForLastTask();	

							date_value = RTC_GetCounter() / SECS_IN_DAY;
							RTC_Cnt_Calc(date_value);
							time_value = RTC_GetCounter() % SECS_IN_DAY;
							rtc_hour = Hex2Bcd(time_value / SECS_IN_HOUR);
							rtc_min = Hex2Bcd((time_value % SECS_IN_HOUR)/SECS_IN_MIN);
							rtc_sec = Hex2Bcd((time_value % SECS_IN_HOUR)%SECS_IN_MIN);
						}
						else
						{
							sub_cnt = timeCorrection.timeCorrecOffset;
			 				sub_flag = SET;
						}	
					}
				}
				else if(timeCorrection.timeCorrectUint == TIME_UNIT_MONTH)//Monthly
				{
					if(month_tmp != rtc_month)
					{
						if(timeCorrection.timeCorrectDirection == DIRECTION_UP)
						{
						    RTC_WaitForLastTask();	
						    RTC_SetCounter(RTC_GetCounter()+timeCorrection.timeCorrecOffset);
						    RTC_WaitForLastTask();	

							date_value = RTC_GetCounter() / SECS_IN_DAY;
							RTC_Cnt_Calc(date_value);
							time_value = RTC_GetCounter() % SECS_IN_DAY;
							rtc_hour = Hex2Bcd(time_value/SECS_IN_HOUR);
							rtc_min = Hex2Bcd((time_value%SECS_IN_HOUR)/SECS_IN_MIN);
							rtc_sec = Hex2Bcd((time_value%SECS_IN_HOUR)%SECS_IN_MIN);
						}		
						else
						{
							sub_cnt = timeCorrection.timeCorrecOffset;
			 				sub_flag = SET;
						}
					}
				}		
			}

			if(sub_flag)
			{
				if(sub_cnt != 0)
				{
					sub_cnt--;
				}	
				else
				{
				    RTC_WaitForLastTask();	
				    RTC_SetCounter(RTC_GetCounter()-timeCorrection.timeCorrecOffset);
				    RTC_WaitForLastTask();	

					date_value = RTC_GetCounter() / SECS_IN_DAY;
					RTC_Cnt_Calc(date_value);
					time_value = RTC_GetCounter() % SECS_IN_DAY;
					rtc_hour = Hex2Bcd(time_value/SECS_IN_HOUR);
					rtc_min = Hex2Bcd((time_value%SECS_IN_HOUR)/SECS_IN_MIN);
					rtc_sec = Hex2Bcd((time_value%SECS_IN_HOUR)%SECS_IN_MIN);

					sub_flag = CLEAR;
				}
			}          
		}
	}
}
