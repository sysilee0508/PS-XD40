// Created By kukuri & sysilee

#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__


//-----------------------------------------------------------------
// Define & Macro
//-----------------------------------------------------------------
#define DISPLAY_WIDTH_1920X1080				1920
#define DISPLAY_HEIGHT_1920x1080			1080

//-----------------------------------------------------------------
// type definitions - struct/enum/etc
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// exported variable declaration
//-----------------------------------------------------------------
extern BYTE changedDisplayMode;


//-----------------------------------------------------------------
// Function declaration
//-----------------------------------------------------------------
extern void Osd_Init_Erase(void);
extern void OSD_EraseAll(void);

extern void OSD_Display(void);
extern void OSD_Display_Time(void);
extern void OSD_Display_Time_NOW(void);
extern void OSD_Display_ChannelName(void);
extern void OSD_Display_State(void);

#endif
