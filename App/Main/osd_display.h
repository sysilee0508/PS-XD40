#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__

#define FULL_1CH 0
#define SPLIT_04 1
#define SPLIT_09 2


typedef struct tagStPoint 
{
	s16		x;
	s16		y;
} ST_POINT;


void Osd_Init_Erase(void);
void Erase_OSD(void);

void OSG_Display(void);
void OSG_Display_Time(void);
void OSG_Display_Time_NOW(void);
void OSG_Display_CH_name(void);
void OSG_Display_State(void);



extern BYTE bMode_change_flag;

#endif
