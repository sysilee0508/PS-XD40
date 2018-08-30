//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  MENU.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#ifndef		__MENU_H__
#define		__MENU_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------
#include "constants.h"

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Exported Variables
// ----------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
WORD GetMenuStatus(BYTE nID, BYTE sID);
void SetMenuStatus(BYTE nID, BYTE sID, BYTE val);
WORD GetOSDMenuID(void);
void SetOSDMenuID(WORD nID);
void SetOSDCombID(WORD nID);
void SetMenuDefaultStatus(void);

void OSD_LineClearRemainder(BYTE row, BYTE rem);

void Inc_Dec_Count(u8 Max,u8 Min,u8 Up_Flag,u8 *P);



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
#define LEFT_KEY   			KEY_FULL_CH1//1
#define UP_KEY     			KEY_FULL_CH2//2
#define DOWN_KEY   			KEY_FULL_CH3//3
#define RIGHT_KEY  			KEY_FULL_CH4//4
#define ENTER_KEY  			KEY_4SPLIT//0x15
#define EXIT_KEY   			KEY_FREEZE//0x19

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
#define MENU_WIDTH			792//800
#define MENU_HEIGHT			672//700
#define MENU_H_START 		(SCREEN_WIDTH/2)-(MENU_WIDTH/2)
#define MENU_V_START 		(SCREEN_HIGHT/2)-(MENU_HEIGHT/2)

#define CHAR_WIDTH_E		12
#define CHAR_WIDTH_K		24
#define CHAR_HEIGHT			24


extern u8 vOSD_B[2];
extern u8 bSETUP;
extern u8 bENTER;
extern const u8 tBIT_CHECK[8];
extern u16 vMOTION_AREA[16][12]; 
extern BYTE bSETUP_Change_flag;



void Data_Con(u8 DataC);

void Enter_SetUP(void);
//void MenuSelect(u8 Current);  
void MenuSelect(u8 Current, u8 offset);  

void SetupMenu(void);
void Page_Title(void);
void Setup_Process(void);
void tPAGE0_KEY(void);

void ON_OFF_DSP(u8 PX,u8 PY,u8 Position,u8 state);
void Time_Correct_Con(u8 Select,u8 Position);
//void RTC_TimeCon(u8 Select,u8 Position);
//void RTC_DateCon(u8 Select,u8 Position);
void RTC_TimeCon(u8 Select,u8 Position,u8 opt);
void RTC_DateCon(u8 Select,u8 Position,u8 opt);

void Data_Run(void);
void Data_Format(u8 Position);
void Year_Format(u8 Position);
void TimeDateSetup_TITLE(void);
void TimeDateSetup_Position(u8 Position);
void TimeDateSetup_KEY(void);

void PAGE2_TITLE(void);
void tPAGE2_KEY(void);

void Int2Char_OSD(u8 PX,u8 PY,u8 *P,u8 Position);
void Off_Check(u8 PX,u8 PY,u8 Chenal,u8 Position);
void PAGE3_TITLE(void);
void tPAGE3_Position(u8 Position);
void tPAGE3_KEY(void);

void Resolution_Dsp(u8 res, u8 Position);
void SPOT_type_Dsp(u8 type, u8 Position);
void SPOT_mode_Dsp(u8 PosY, u8 type, u8 mode, u8 Position);
void PAGE4_TITLE(void);
void tPAGE4_KEY(void);

void Remocon_Dsp(u8 Position);
void tPAGE5_Position(u8 Position);
void PAGE5_TITLE(void);
void tPAGE5_KEY(void);

void PAGE6_TITLE(void);
void tPAGE6_KEY(void);

void PAGE7_TITLE(void);
void tPAGE7_KEY(void);


#endif	
