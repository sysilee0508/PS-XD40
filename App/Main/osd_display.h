#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__

enum
{
	FULL_SCREEN_MODE = 0,
	SPLIT_4_MODE,
#ifdef __9CH_DEVICE__
	SPLIT_9_MODE
#endif
};

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
