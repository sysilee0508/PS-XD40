#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__
//
//enum
//{
//	DISPLAY_MODE_FULL_SCREEN = 0,
//	DISPLAY_MODE_4SPLIT,
//#ifdef __9CH_DEVICE__
//	SPLIT_9_MODE
//#endif
//};


#define CHANNEL_NEME_LENGTH_MAX			12

//
//typedef struct tagStPoint
//{
//	s16		x;
//	s16		y;
//} ST_POINT;


void Osd_Init_Erase(void);
void Erase_OSD(void);

void OSD_Display(void);
void OSD_Display_Time(void);
void OSD_Display_Time_NOW(void);
void OSD_Display_CH_name(void);
void OSD_Display_State(void);



extern BYTE bMode_change_flag;

#endif
