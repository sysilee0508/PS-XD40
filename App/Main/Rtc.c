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


//=============================================================================
//  Constant Array Declaration (data table)
//=============================================================================
const BYTE month_normal_table[] =	//평년 
{
	31,28,31,30,31,30,31,31,30,31,30,31
};

const BYTE month_leap_table[] = 	//윤년
{
	31,29,31,30,31,30,31,31,30,31,30,31
};

const WORD year_table[] =	//100년 
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
//	16진수를 BCD코드로 변환
//-----------------------------------------------------------------------------
BYTE Hex2Bcd(BYTE hex_num) 
{
	return (((hex_num/10)<<4)|(hex_num%10));
}


//-----------------------------------------------------------------------------
//	일일값으로 된 년월일 값을 계산해서 각각으로 나눈다.  
//-----------------------------------------------------------------------------
void RTC_Cnt_Calc(WORD ymd_val)
{
	BYTE i;
	BYTE year_val = 0;
	BYTE month_val = 0;
	BYTE day_val = 0;
	WORD tmp_val = 0;


	tmp_val = ymd_val; 


	//년 찾기
	for(i=0;i<100;i++)
	{
		if(tmp_val < year_table[i])
		{
			year_val = i;
			break;
		}	

		tmp_val -= year_table[i];
	}


	//월,일 찾기
	if((year_val%4) == 0)	//윤년일 때
	{
		for(i=0;i<12;i++)
		{
			if(tmp_val < month_leap_table[i])
			{
				month_val = i+1;
				break;
			}	

			tmp_val -= month_leap_table[i];
		}	
	}
	else 
	{
		for(i=0;i<12;i++)
		{
			if(tmp_val < month_normal_table[i])
			{
				month_val = i+1;
				break;
			}	

			tmp_val -= month_normal_table[i];
		}	
	}

	day_val = tmp_val+1;
	

	rtc_year  = Hex2Bcd(year_val);
	rtc_month = Hex2Bcd(month_val);
	rtc_day   = Hex2Bcd(day_val);
}


//전체 시간값을 받아 초단위 값으로 계산해서 RTC 카운터에 넣는다. 
void Change_RTC_Cnt(struct tm *time)
{
	BYTE i;
	LONG tmp_val = 0;


	//년 더하기
	for(i=0;i<time->tm_year;i++)
	{
		tmp_val += year_table[i];
	}		

	//월 더하기
	if((time->tm_year%4) == 0)	//윤년일 때
	{
		for(i=0;i<time->tm_mon;i++)
		{
			tmp_val += month_leap_table[i];
		}		
	}
	else 
	{
		for(i=0;i<time->tm_mon;i++)
		{
			tmp_val += month_normal_table[i];
		}		
	}
		
	//월 더하기
	tmp_val += time->tm_mday;

	tmp_val *= 86400;

	//시 더하기
	tmp_val += (time->tm_hour*3600); 

	//분 더하기
	tmp_val += (time->tm_min*60); 

	//초 더하기
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

	//if((!bSETUP) || (vPAGE != 1) || (vITEM_Y != 0) || (!bENTER))
	{
		if(rtc_sec_update_flag)
		{
			rtc_sec_update_flag = 0;
			sec_flag = 1;

			date_value = RTC_GetCounter() / 86400; 
			time_value = RTC_GetCounter() % 86400;

			month_tmp = rtc_month;	//time correction 에서 사용
			day_tmp = rtc_day;		//time correction 에서 사용


			RTC_Cnt_Calc(date_value);

			
		    // Compute  hours
			rtc_hour = Hex2Bcd(time_value/3600);

			// Compute minutes
			rtc_min = Hex2Bcd((time_value%3600)/60);

		    // Compute seconds
			rtc_sec = Hex2Bcd((time_value%3600)%60);



			//Time correction-----------------------------------------------------
			if(sys_env.vCORRECT_OFFSET)    
			{
				if(sys_env.bCORRECT == 0) //Daliy 
				{
					if(day_tmp != rtc_day)
					{
						if(sys_env.bVECTOR) //1= + , 0= -
						{
						    RTC_WaitForLastTask();	
						    RTC_SetCounter(RTC_GetCounter()+sys_env.vCORRECT_OFFSET);
						    RTC_WaitForLastTask();	

							date_value = RTC_GetCounter() / 86400; 
							RTC_Cnt_Calc(date_value);
							time_value = RTC_GetCounter() % 86400;
							rtc_hour = Hex2Bcd(time_value/3600);
							rtc_min = Hex2Bcd((time_value%3600)/60);
							rtc_sec = Hex2Bcd((time_value%3600)%60);
						}
						else
						{
							sub_cnt = sys_env.vCORRECT_OFFSET;				
			 				sub_flag = 1;
						}	
					}
				}
				else //Monthly					
				{
					if(month_tmp != rtc_month)
					{
						if(sys_env.bVECTOR)
						{
						    RTC_WaitForLastTask();	
						    RTC_SetCounter(RTC_GetCounter()+sys_env.vCORRECT_OFFSET);
						    RTC_WaitForLastTask();	

							date_value = RTC_GetCounter() / 86400; 
							RTC_Cnt_Calc(date_value);
							time_value = RTC_GetCounter() % 86400;
							rtc_hour = Hex2Bcd(time_value/3600);
							rtc_min = Hex2Bcd((time_value%3600)/60);
							rtc_sec = Hex2Bcd((time_value%3600)%60);
						}		
						else
						{
							sub_cnt = sys_env.vCORRECT_OFFSET;				
			 				sub_flag = 1;	
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
				    RTC_SetCounter(RTC_GetCounter()-sys_env.vCORRECT_OFFSET);
				    RTC_WaitForLastTask();	

					date_value = RTC_GetCounter() / 86400; 
					RTC_Cnt_Calc(date_value);
					time_value = RTC_GetCounter() % 86400;
					rtc_hour = Hex2Bcd(time_value/3600);
					rtc_min = Hex2Bcd((time_value%3600)/60);
					rtc_sec = Hex2Bcd((time_value%3600)%60);

					sub_flag = 0;
				}
			}          
			//Time correction-----------------------------------------------------

			//if(bOSD_OFF&(!bSETUP)) TD_Display();
			//if(bOSD_OFF) TD_Display_Spot();
		}
	}
}




