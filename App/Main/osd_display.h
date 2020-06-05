// Created By kukuri & sysilee

#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__


//-----------------------------------------------------------------
// Define & Macro
//-----------------------------------------------------------------
#define CHAR_WIDTH							12
#define CHAR_HEIGHT							24

#define ASCII_SPACE					0x20
#define ASCII_ZERO					0x30

#define VIDEO_MODE_DISPLAY_TIME			3//seconds

//-----------------------------------------------------------------
// type definitions - struct/enum/etc
//-----------------------------------------------------------------
typedef struct
{
	u16 pos_x;
	u16 pos_y;
} sPosition_t;

//-----------------------------------------------------------------
// exported variable declaration
//-----------------------------------------------------------------
//extern BYTE requestRefreshScreen;

//-----------------------------------------------------------------
// Function declaration
//-----------------------------------------------------------------
extern void Osd_ClearScreen(void);
extern void OSD_EraseIndicator(void);
extern void OSD_EraseAllText(void);
extern void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size);
extern void OSD_Display(void);
extern void OSD_Display_Time(void);
extern void OSD_Display_Time_NOW(void);
extern void OSD_DisplayChannelName(void);
extern void OSD_DisplayVideoMode(void);
extern void OSD_Display_State(void);
extern void OSD_DisplayIndicator(void);
extern void OSD_EraseNoVideo(void);

extern void OSD_SetBoaderLine(void);
extern void OSD_DrawBorderLine(void);
extern void OSD_RefreshScreen(void);
extern void OSD_SetEvent(eChannel_t channel, u8 evt);
extern void OSD_ClearEvent(eChannel_t channel, u8 evt);
extern u8 OSD_GetEvent(eChannel_t channel);

extern void ResetVideoModeDisplayTime(eChannel_t channel, u8 time);
extern void ClearVideoModeDisplayTime(eChannel_t channel);
extern u8 GetVideoModeDisplayTime(eChannel_t channel);

#endif
