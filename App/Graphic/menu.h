// Created By kukuri & sysilee

#ifndef __MENU_H__
#define	__MENU_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------
#include "constants.h"

// -----------------------------------------------------------------------------

//-----------------------------------------------------------------
// Struct/Union Types and define
//-----------------------------------------------------------------

#define MENU_WIDTH						792//800
#define MENU_HEIGHT						672//700
#define MENU_START_POSITION_X 			(DISPLAY_WIDTH - MENU_WIDTH) / 2
#define MENU_START_POSITION_Y 			(DISPLAY_HEIGHT - MENU_HEIGHT) / 2

// ----------------------------------------------------------------------
// Exported Variables
// ----------------------------------------------------------------------
extern BOOL readyForSetupMode;// = FALSE;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
extern WORD GetMenuStatus(BYTE nID, BYTE sID);
extern void SetMenuStatus(BYTE nID, BYTE sID, BYTE val);
//extern WORD GetOSDMenuID(void);
extern void SetOSDMenuID(WORD nID);
//extern void SetOSDCombID(WORD nID);
//extern void SetMenuDefaultStatus(void);

extern void Enter_MainMenu(void);
extern void Menu_KeyProc(eKeyData_t key);

extern void ChangeSystemMode(u8 mode);
extern u8 GetSystemMode(void);
extern void DisplayTimeInMenu(void);

#endif	
