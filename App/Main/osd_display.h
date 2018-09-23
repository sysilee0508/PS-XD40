// Created By kukuri & sysilee

#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__


//-----------------------------------------------------------------
// Define & Macro
//-----------------------------------------------------------------
#define DISPLAY_WIDTH_1920X1080				1920
#define DISPLAY_HEIGHT_1920x1080			1080

#define DISPLAY_WIDTH 						DISPLAY_WIDTH_1920X1080
#define DISPLAY_HEIGHT						DISPLAY_HEIGHT_1920x1080

#define DISPLAY_HALF_WIDTH 					(DISPLAY_WIDTH/2)	//960
#define DISPLAY_HALF_HEIGHT					(DISPLAY_HEIGHT/2)	//540
#define DISPLAY_QUAD_WIDTH					(DISPLAY_WIDTH/4)	//480
#define DISPLAY_QUAD_HEIGHT					(DISPLAY_HEIGHT/4)	//270


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
extern void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size);
extern void OSD_Display(void);
extern void OSD_Display_Time(void);
extern void OSD_Display_Time_NOW(void);
extern void OSD_Display_ChannelName(void);
extern void OSD_Display_State(void);

extern void OSD_DrawBorderLine(void);
#endif
