// Created By kukuri & sysilee

#ifndef __OSD_DISPLAY__
#define __OSD_DISPLAY__


//-----------------------------------------------------------------
// Define & Macro
//-----------------------------------------------------------------

#define CHAR_WIDTH							12
#define CHAR_HEIGHT							24

#define ASCII_SPACE							0x20
#define ASCII_ZERO							0x30

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

//-----------------------------------------------------------------
// Function declaration
//-----------------------------------------------------------------
extern void Osd_ClearScreen(void);
extern void OSD_EraseAllText(void);
//extern void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size);
extern void OSD_Display(void);
//extern void OSD_Display_Time(void);
//extern void OSD_Display_Time_NOW(void);
extern void OSD_DisplayTitle(void);

extern void OSD_DrawBorderLine(void);
extern void OSD_RefreshScreen(void);
#endif
