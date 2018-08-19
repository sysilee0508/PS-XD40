//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	MENU.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "..\main\common.h"
#include "..\main\key.h"
// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
#define		INC			1
#define		DEC			0

#if CPU_ALIGN_BIG_ENDIAN == 1
#define		pID(x)		pID[((x)? 1:0)]
#else
#define		pID(x)		pID[((x)? 0:1)]
#endif

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

// 3dnrlevel
//static ROMDATA FLOAT defNRMul[]	= {1.667f};
// Sharpness
//static ROMDATA FLOAT defEnhanceMul[]	= {0.788f};
// Contrast, Brightness, Saturation, Hue, 
//static ROMDATA FLOAT defPictureMul[]	= {0.393f, 0.393f, 0.393f, 0.393f};
// H-Offset, V-Offset
//static ROMDATA FLOAT defEVideoMul[]	= {0.393f, 0.393f};		   //by hungry 2012.03.14

static WORD OSDMenuID, OSDCombID;
//static BYTE fMenuUpdate, fMenuGraphic;
BYTE fMenuUpdate;			  //by hungry 2012.03.06
//static BYTE fMenuGraphic;	  //by hungry 2012.03.06

static BYTE demoIPCMode[sizeof(defIPCModeSize)];
static BYTE demoNRMode[sizeof(defNRModeSize)];
static BYTE demoEnhance[sizeof(defEnhanceSize)];
static BYTE demoInOutMode[sizeof(defInOutModeSize)];
static BYTE demoPicture[sizeof(defPictureSize)];
static BYTE demoSetUp[sizeof(defSetUpSize)];
static BYTE demoFilter[sizeof(defFilterSize)];
//static BYTE demoEVideo[sizeof(defEVideoSize)];   //by hungry 2012.03.14
static BYTE demoAudio[sizeof(defAudioSize)];	   //by hungry 2012.03.14


// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//static void OSD_LineClearRemainder(BYTE row, BYTE rem)
void OSD_LineClearRemainder(BYTE row, BYTE rem)
{
	BYTE j, pSTR[32];

	if (rem==32) return;
	//for (j=0; j<32-rem; j++) pSTR[j] = 0x00;	// clear remainder
	for (j=0; j<32-rem; j++) pSTR[j] = 0x20;	// clear remainder
	MDINGAC_SetDrawXYMode(row, rem, &pSTR[0], 32-rem, 0);
}



//--------------------------------------------------------------------------------------------------
void SetOSDMenuID(WORD nID)
{
	OSDMenuID = nID;
	fMenuUpdate = ON;
}

//--------------------------------------------------------------------------------------------------
WORD GetOSDMenuID(void)
{
	return OSDMenuID;
}

//--------------------------------------------------------------------------------------------------
void SetOSDCombID(WORD nID)
{
	OSDCombID = nID;
}

//--------------------------------------------------------------------------------------------------
WORD GetMenuStatus(BYTE nID, BYTE sID)
{
	if (nID==1) return demoIPCMode[sID-1];
	if (nID==2) return demoNRMode[sID-1];
	if (nID==3) return demoEnhance[sID-1];
	if (nID==4) return demoInOutMode[sID-1];
	if (nID==5) return demoPicture[sID-1];
	if (nID==6) return demoSetUp[sID-1];
	if (nID==7) return demoFilter[sID-1];
//	if (nID==8) return demoEVideo[sID-1];		 //by hungry 2012.03.14
	if (nID==8) return demoAudio[sID-1];		 //by hungry 2012.03.14
	return 0;
}

//--------------------------------------------------------------------------------------------------
void SetMenuStatus(BYTE nID, BYTE sID, BYTE val)
{
	if (nID==1) demoIPCMode[sID-1] = val;
	if (nID==2) demoNRMode[sID-1] = val;
	if (nID==3) demoEnhance[sID-1] = val;
	if (nID==4) demoInOutMode[sID-1] = val;
	if (nID==5) demoPicture[sID-1] = val;
	if (nID==6) demoSetUp[sID-1] = val;
	if (nID==7) demoFilter[sID-1] = val;
//	if (nID==8) demoEVideo[sID-1] = val;		   //by hungry 2012.03.14
	if (nID==8) demoAudio[sID-1] = val;		   //by hungry 2012.03.14
}


//--------------------------------------------------------------------------------------------------------------------------
void SetMenuDefaultStatus(void)
{
	memcpy(demoIPCMode, defIPCMode, sizeof(defIPCMode));
	memcpy(demoNRMode, defNRMode, sizeof(defNRMode));
	memcpy(demoEnhance, defEnhance, sizeof(defEnhance));
	memcpy(demoInOutMode, defInOutMode, sizeof(defInOutMode));
	memcpy(demoPicture, defPicture, sizeof(defPicture));
	memcpy(demoSetUp, defSetUp, sizeof(defSetUp));
	memcpy(demoFilter, defFilter, sizeof(defFilter));
//	memcpy(demoEVideo, defEVideo, sizeof(defEVideo));		//by hungry 2012.03.14
	memcpy(demoAudio, defAudio, sizeof(defAudio));		//by hungry 2012.03.14
//	OSDMenuID = OSDCombID = 0;
}




//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
BYTE bSETUP_Change_flag = 0;

u8 vPAGE = 0;
u8 vITEM_X = 0;
u8 vITEM_Y = 0;
u8 bENTER = 0;
u8 bSETUP = 0;

u8 vOSD_B[2];


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
#define UNDER_BAR		0x01

#define D_P_HOUR	0x01
#define D_P_MIN		0x02
#define D_P_SEC		0x04

#define D_P_YEAR	0x01
#define D_P_MON     0x02
#define D_P_DAY     0x04


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
const u8 tBIT_CHECK[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const u16 tBIT_CHECK16[16]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
const u8 BLAN[]=" ";
const u8 ArrowL[]= {0x80,0x00};

//-----------------------------------------------------------------
// Menu ȭ��ǥ ��ġ��    
//-----------------------------------------------------------------
const u8 tMenu_P[8][25] = 
{{ 5,7,9,11,13,15,17,19},
 { 5,7,9,11,13,15,17,19,21},

 { 5,7,9,11,13,15,17,19,21,23},

 { 6,8,10,12},

 { 6,8,10,12,14,16,18},

 { 6,8,10,12,14,16},

 { 6,8,10,12,14,16},

 { 6,8,10,12,14,16},
};


//-----------------------------------------------------------------
//    
//-----------------------------------------------------------------
const u8 tMenu_S[8] =  {19,13,18,20,18,17,17,17};


//-----------------------------------------------------------------
//    
//-----------------------------------------------------------------
void Data_Con(u8 DataC)
{
  	vOSD_B[0] = ((DataC>>4)+0x30);
  	vOSD_B[1] = ((DataC&0x0f)+0x30);
}

//-----------------------------------------------------------------
//    
//-----------------------------------------------------------------
BYTE Bcd2Hex(BYTE bcd) 
{
	return ((bcd>>4)*10)+(bcd&0X0F);
}

void Dec_Hex_Function(u8 *P) 
{
	*P=((*P>>4)*10)+(*P&0X0F);
}

void Hex_Dec_Function(u8 *P) 
{
	*P=(((*P/10)<<4)|(*P%10));
}

void Inc_Dec_Count(u8 Max,u8 Min,u8 Up_Flag,u8 *P)

{
	if(Up_Flag)
	{
		if(*P<Max)
		(*P)++;
		else *P = Min;}
	else 
	{
		if(*P>Min)
			(*P)--;
		else
			*P = Max;}
}

void INC_Dec_Hex(u8 Max,u8 Min,u8 Up_Flag,u8 *P) // 16������ ��ȯ �� Inc_Dec_Count�� ���� �� �ٽ� 10������ ��ȯ�Ѵ�.
{
	Dec_Hex_Function(P);
	Inc_Dec_Count(Max,Min,Up_Flag,P);
	Hex_Dec_Function(P);
}


//--------------------------------------------------------------------------------------
//	(����) ���ڿ��� ���� ��ŭ ���ڿ��� ǥ����.  
//--------------------------------------------------------------------------------------
void Print_Str(u16 PosX, u16 PosY, const u8 *FontData)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

void Print_Char(u16 PosX, u16 PosY, u8 *FontData, u16 SIZE)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, SIZE, 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void CodeWriteChar(u16 PosX, u16 PosY, const u8 *FontData, u8 Attrib, u16 SIZE)
{
	u16 i;
	BYTE ch_buf[MENU_WIDTH/12];
	
	Print_Str(MENU_H_START+(PosX*CHAR_WIDTH_E), MENU_V_START+(PosY*CHAR_HEIGHT), FontData);

	if(Attrib != NULL)
	{
		for(i=0;i<SIZE;i++) ch_buf[i] = 0x7f;
	}	
	else 
	{
		for(i=0;i<SIZE;i++) ch_buf[i] = 0x20;
	}

	Print_Char(MENU_H_START+(PosX*CHAR_WIDTH_E), MENU_V_START+((PosY+1)*CHAR_HEIGHT), ch_buf, SIZE);
}

void WriteChar(u16 PosX, u16 PosY, u8 *FontData, u8 Attrib, u16 SIZE)
{
	u16 i;
	BYTE ch_buf[MENU_WIDTH/12];

	Print_Char(MENU_H_START+(PosX*CHAR_WIDTH_E), MENU_V_START+(PosY*CHAR_HEIGHT), FontData, SIZE);

	if(Attrib != NULL)
	{
		for(i=0;i<SIZE;i++) ch_buf[i] = 0x7f;
	}	
	else 
	{
		for(i=0;i<SIZE;i++) ch_buf[i] = 0x20;
	}

	Print_Char(MENU_H_START+(PosX*CHAR_WIDTH_E), MENU_V_START+((PosY+1)*CHAR_HEIGHT), ch_buf, SIZE);
}


//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void Erase_Menu_OSD(void)
{
	int i = 0;
	BYTE pSTR[MENU_WIDTH/12];
	
	memset(pSTR, 0x20, sizeof(pSTR)); 

	for(i=0; i<=(MENU_HEIGHT/24); i++)	
	{
		MDINGAC_SetDrawXYMode(MENU_V_START+i*CHAR_HEIGHT, MENU_H_START, pSTR, sizeof(pSTR), 0);
	}
}



//-----------------------------------------------------------------
// 
//-----------------------------------------------------------------
void Enter_SetUP(void)
{
	bSETUP = 1;
	bENTER = 0;

	SetKeyMode(KEY_MODE_REPEAT);
  
	vITEM_X = 0;
	vITEM_Y = 0;

	Erase_OSD();

	SetupMenu();
}


//-----------------------------------------------------------------
// 
//-----------------------------------------------------------------
void MenuSelect(u8 Current, u8 offset)  
{	
	static u8 pre_H_loc;
	static u8 pre_V_loc;

  	Print_Str(MENU_H_START+(pre_H_loc*CHAR_WIDTH_E),MENU_V_START+(pre_V_loc*CHAR_HEIGHT),BLAN);
  	Print_Str(MENU_H_START+((tMenu_S[vPAGE]+offset)*CHAR_WIDTH_E),MENU_V_START+(tMenu_P[vPAGE][(Current)]*CHAR_HEIGHT),ArrowL);

	pre_H_loc = tMenu_S[vPAGE] + offset;
	pre_V_loc = tMenu_P[vPAGE][(Current)];
}



//------------------------------------------------------------------
//   PAGE0 Function
//------------------------------------------------------------------
const BYTE cMAIN_MENU[]		= "- MAIN MENU -";
const BYTE cTIME_DATE[]		= "1. TIME/DATE";
const BYTE cCAMERA_TITLE[]		= "2. CAMERA TITLE";
const BYTE cAUTO_SEQUENCE[]	= "3. AUTO SEQUENCE";
const BYTE cDISPLAY[]			= "4. DISPLAY";
const BYTE cALARM_SET[]        = "5. ALARM SET";
const BYTE cMOTION_DETECTION[]	= "6. MOTION DETECTION";
const BYTE cMISCELLANEOUS[]	= "7. MISCELLANEOUS";
  
void SetupMenu(void)
{   
	vPAGE = 0;

	Erase_Menu_OSD();                          

	//------------------------------------------------
	// �޴� ��׶��带 ������ �ڽ��� ��Ÿ����.
	//------------------------------------------------
	//MDINOSD_SetBGBoxColor(RGB(128,16,128));		// set BG-BOX color //������
	MDINOSD_SetBGBoxColor(RGB(0,0,0));		// set BG-BOX color //������

	MDINOSD_SetBGBoxArea(BGBOX_INDEX0, MENU_H_START, MENU_V_START, MENU_WIDTH, MENU_HEIGHT);
	MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
	MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
	//------------------------------------------------

	MenuSelect(vITEM_Y,0);
	CodeWriteChar(24,2,cMAIN_MENU,NULL, 0);
	CodeWriteChar(22,5,cTIME_DATE,NULL, 0);
	CodeWriteChar(22,7,cCAMERA_TITLE,NULL, 0);
	CodeWriteChar(22,9,cAUTO_SEQUENCE,NULL, 0);
	CodeWriteChar(22,11,cDISPLAY,NULL, 0);
	CodeWriteChar(22,13,cALARM_SET,NULL, 0);
	CodeWriteChar(22,15,cMOTION_DETECTION,NULL, 0);
	CodeWriteChar(22,17,cMISCELLANEOUS,NULL, 0);
}

void Page_Title(void)
{
 	switch(vITEM_Y)
 	{
  		case 1 : PAGE1_TITLE(); break;
 		case 2 : PAGE2_TITLE(); break;
  		case 3 : PAGE3_TITLE(); break;
 		case 4 : PAGE4_TITLE(); break;
 		case 5 : PAGE5_TITLE(); break;
 		case 6 :  vITEM_Y = 0; PAGE6_TITLE(); break;
 		case 7 : PAGE7_TITLE(); break;

	}
}
void Setup_Process(void)
{
	if(IsKeyReady())
	{
		ClearKeyReady();

		switch(vPAGE)
		{
     	 case 0 : tPAGE0_KEY(); break;
     	 case 1 : tPAGE1_KEY(); break;
      	 case 2 : tPAGE2_KEY(); break;
      	 case 3 : tPAGE3_KEY(); break;
      	 case 4 : tPAGE4_KEY(); break;
      	 case 5 : tPAGE5_KEY(); break;
      	 case 6 : tPAGE6_KEY(); break;
      	 case 7 : tPAGE7_KEY(); break;
 		}
	}
	else
	{
		if(vPAGE ==1)
		{
			if(sec_flag)
			{
	    		sec_flag = 0;
		   		if((vITEM_Y==0)&(bENTER))
		   		{
          		 	if(vITEM_X == 0){RTC_TimeCon(D_P_HOUR,UNDER_BAR,0);RTC_TimeCon(D_P_MIN,NULL,0);RTC_TimeCon(D_P_SEC,NULL,0);}
              		if(vITEM_X == 1){RTC_TimeCon(D_P_HOUR,NULL,0);RTC_TimeCon(D_P_MIN,UNDER_BAR,0);RTC_TimeCon(D_P_SEC,NULL,0);}
              		if(vITEM_X == 2){RTC_TimeCon(D_P_HOUR,NULL,0);RTC_TimeCon(D_P_MIN,NULL,0);RTC_TimeCon(D_P_SEC,UNDER_BAR,0);}
		  		}
          		else 
				{
					RTC_TimeCon(D_P_HOUR|D_P_MIN|D_P_SEC,NULL,0); 
					RTC_DateCon(D_P_YEAR|D_P_MON|D_P_DAY,NULL,0);
          		}	
			}
		}
	}
}

void tPAGE0_KEY(void)
{
	u8 State=0;		

 	switch(GetCurrentKey())
	{
		case UP_KEY :
			State=0xff;
		case DOWN_KEY : 
			Inc_Dec_Count(6,0,~State,&vITEM_Y);
			MenuSelect(vITEM_Y,0);
			break;              

		case ENTER_KEY : 
			vITEM_Y++;
			Page_Title();
			bENTER = 0;
			vITEM_X = 0;
			vITEM_Y = 0;
	   		break; 	

		case EXIT_KEY :	
			if(bSETUP_Change_flag) 
			{
				bSETUP_Change_flag = 0;
				write_eeprom_all();
			}	

			Erase_Menu_OSD();                          
			bSETUP = 0;
			//bMode_Change = 1;
			SetKeyMode(KEY_MODE_LONG);//key_mode = KEY_MODE_LONG;

			MDINOSD_SetBGBoxColor(RGB(255,255,255));	// set BG-BOX color //���
			Set_border_line();
			bMode_change_flag = 1;
			break;
	}
}


//------------------------------------------------------------------
//   PAGE1 Function
//------------------------------------------------------------------
const u8 cON1[]	= "ON ";
const u8 cOFF1[]	= "OFF";
void ON_OFF_DSP(u8 PX,u8 PY,u8 Position,u8 state)
{
	if(state)CodeWriteChar(PX,PY,cON1,Position,3);
	else CodeWriteChar(PX,PY,cOFF1,Position,3);
}

const u8 cPLUS1[]  = "+";
const u8 cMINUS1[] = "-";
const u8 cMON[]	= "MON";
const u8 cDAY[]	= "DAY";
void Time_Correct_Con(u8 Select,u8 Position)
{
	if(Select & 0x01)
	{
		if(sys_env.bVECTOR)	CodeWriteChar(35,13,cPLUS1,Position,1);
		else CodeWriteChar(35,13,cMINUS1,Position,1);	
	}
	if(Select & 0x02)
	{
		Data_Con(sys_env.vCORRECT_OFFSET);
		WriteChar(36,13,vOSD_B,Position,2);
	}
	
	if(Select & 0x04)
	{
		if(sys_env.bCORRECT) CodeWriteChar(42,13,cMON,Position,3);
		else CodeWriteChar(42,13,cDAY,Position,3);
	}
}
	
const u8 cSEMICOL1[]	= ":";
void RTC_TimeCon(u8 Select,u8 Position,u8 opt)
{
	static u8 old_rtc_hour = 0xff, old_rtc_min = 0xff, old_rtc_sec = 0xff;      

	if(Select & D_P_HOUR)
	{
		if(opt == 1 || old_rtc_hour != rtc_hour)
    	{
			old_rtc_hour = rtc_hour;
			
			Data_Con(rtc_hour);
			WriteChar(27,5,vOSD_B,Position,2);
			CodeWriteChar(29,5,cSEMICOL1,NULL,0);
		}	
	}
	if(Select & D_P_MIN)
	{
		if(opt == 1 || old_rtc_min != rtc_min)
    	{
			old_rtc_min = rtc_min;
			
			Data_Con(rtc_min);
			WriteChar(30,5,vOSD_B,Position,2);
			CodeWriteChar(32,5,cSEMICOL1,NULL,0);
		}
	}
	if(Select & D_P_SEC)
	{
		if(opt == 1 || old_rtc_sec != rtc_sec)
    	{
			old_rtc_sec = rtc_sec;
			
			Data_Con(rtc_sec);
			WriteChar(33,5,vOSD_B,Position,2);
		}
	}
}

//const u8 tYEAR_Position[3][3] ={ {19,23,23},{22,17,20},{25,20,17} };
const u8 tYEAR_Position[3][3] ={ {27,31,31},{30,25,28},{33,28,25} };
const u8 c20_1[]	= "20";
void RTC_DateCon(u8 Select,u8 Position,u8 opt)
{
	u8 Year_P,Mon_P,Sec_P;
	static u8 old_rtc_year = 0xff, old_rtc_month = 0xff, old_rtc_day = 0xff;      

	if(Select & D_P_YEAR)
	{
		if(opt == 1 || old_rtc_year != rtc_year)
		{
			old_rtc_year = rtc_year;
			
			Year_P = tYEAR_Position[0][sys_env.vDATE_FORMAT];

			if(sys_env.vDATE_FORMAT)Year_P += 2;
			CodeWriteChar(Year_P-2,7,c20_1,Position,2);
	    	Data_Con(rtc_year);
			WriteChar(Year_P,7,vOSD_B,Position,2);
		}
	}
	if(Select & D_P_MON)
	{
		if(opt == 1 || old_rtc_month != rtc_month)
		{
			old_rtc_month = rtc_month;
			
			Mon_P = tYEAR_Position[1][sys_env.vDATE_FORMAT];
	    	Data_Con(rtc_month);
			WriteChar(Mon_P,7,vOSD_B,Position,2);
		}
	}
	if(Select & D_P_DAY)
	{
		if(opt == 1 || old_rtc_day != rtc_day)
		{
			old_rtc_day = rtc_day;

			Sec_P = tYEAR_Position[2][sys_env.vDATE_FORMAT];
	    	Data_Con(rtc_day);
			WriteChar(Sec_P,7,vOSD_B,Position,2);
		}
	}
}  

const u8 cDATE_FORMAT1[]	= "-  -    ";
const u8 cDATE_FORMAT2[]	= "    -  -";
void Data_Run(void)
{
	if(sys_env.vDATE_FORMAT)CodeWriteChar(27,7,cDATE_FORMAT1,NULL,0);
	else CodeWriteChar(25,7,cDATE_FORMAT2,NULL,0);
	RTC_DateCon(D_P_YEAR|D_P_MON|D_P_DAY,NULL,1);
}


const u8 cASIA[]	= "ASIA";
const u8 cUS[]		= "U.S ";
const u8 cEURO[]	= "EURO";
const u8 cYMD[]	= "(YY-MM-DD)";
const u8 cMDY[]	= "(MM-DD-YY)";
const u8 cDMY[]	= "(DD-MM-YY)";
void Data_Format(u8 Position)
{
	if(sys_env.vDATE_FORMAT==0){CodeWriteChar(35,11,cASIA,Position,4);CodeWriteChar(35,7,cYMD,NULL,0);}
	if(sys_env.vDATE_FORMAT==1){CodeWriteChar(35,11,cUS,Position,4);CodeWriteChar(35,7,cMDY,NULL,0);}
	if(sys_env.vDATE_FORMAT==2){CodeWriteChar(35,11,cEURO,Position,4);CodeWriteChar(35,7,cDMY,NULL,0);}
}


const u8 cTIME_OSD_NORMAL1[] = "NORMAL";
const u8 cTIME_OSD_15X1[]    = "1.5X  ";
const u8 cTIME_OSD_20X1[]    = "2X    ";
void TIME_SIZE_Dsp(u8 res, u8 Position)
{
	switch(res)
	{
		case 0: CodeWriteChar(40,15,cTIME_OSD_NORMAL1,Position,6);break; 
		case 1: CodeWriteChar(40,15,cTIME_OSD_15X1,Position,6);break; 
		case 2: CodeWriteChar(40,15,cTIME_OSD_20X1,Position,6);break; 
	}
}

const u8 cTIME_OSD_LEFT1[]   = "LEFT  ";
const u8 cTIME_OSD_CENTER1[] = "CENTER";
const u8 cTIME_OSD_RIGHTX1[] = "RIGHT ";
void TIME_LOC_Dsp(u8 res, u8 Position)
{
	switch(res)
	{
		case 0: CodeWriteChar(44,15,cTIME_OSD_LEFT1,Position,6);break; 
		case 1: CodeWriteChar(44,15,cTIME_OSD_CENTER1,Position,6);break; 
		case 2: CodeWriteChar(44,15,cTIME_OSD_RIGHTX1,Position,6);break; 
	}
}




const u8 cTIME_DATE1[]		= "- TIME/DATE -";
const u8 cTIME1[]			= "1. TIME:";
const u8 cDATE1[]			= "2. DATE:";
const u8 cTIME_DISPLAY1[] 	= "3. TIME/DATE DISPLAY:";
const u8 cDATE_FORMAT_1[]  	= "4. DATE FORMAT  :";           
const u8 cTIME_CORRECT1[] 	= "5. TIME CORRECT :    SEC/ ";
const u8 cTIME_OSD_SIZE1[] 	= "6. TIME/DATE OSD SIZE:";         
const u8 cTIME_OSD_LOC1[] 	= "6. TIME/DATE OSD LOCATION:";
const u8 cHMS[]				= "(HH:MM:SS)";
void PAGE1_TITLE(void)
{   
	vPAGE = 1;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);
	CodeWriteChar(24,2,cTIME_DATE1,NULL,0);

    CodeWriteChar(17,5,cTIME1,NULL,0);
	RTC_TimeCon(D_P_HOUR|D_P_MIN|D_P_SEC,NULL,1);
	CodeWriteChar(35,5,cHMS,NULL,0);

    CodeWriteChar(17,7,cDATE1,NULL,0);
	Data_Run();

    CodeWriteChar(17,9,cTIME_DISPLAY1,NULL,0);
	ON_OFF_DSP(39,9,0,sys_env.bTIME_ON);

    CodeWriteChar(17,11,cDATE_FORMAT_1,NULL,0);
	Data_Format(NULL);

    CodeWriteChar(17,13,cTIME_CORRECT1,NULL,0);
	Time_Correct_Con(0x01|0x02|0x04,NULL);

//    CodeWriteChar(17,15,cTIME_OSD_SIZE1,NULL,0);
	//TIME_SIZE_Dsp(sys_env.vTIME_Size,NULL);

    CodeWriteChar(17,15,cTIME_OSD_LOC1,NULL,0);
	TIME_LOC_Dsp(sys_env.vTIME_Position,NULL);
  	
}

const u8 tRTC_POSITION[3][3]={{0x01,0x02,0x04},{0x02,0x04,0x02},{0x04,0x01,0x01}};
void tPAGE1_Position(u8 Position)
{
	u8 Select;

 	switch(vITEM_Y)
 	{
 	    case 0 : Select = tBIT_CHECK[vITEM_X];
				 RTC_TimeCon(Select,Position,1);
				 break;
 	    case 1 : Select = tRTC_POSITION[vITEM_X][sys_env.vDATE_FORMAT]; 
				 RTC_DateCon(Select,Position,1);
				 break;
 	    case 2 : ON_OFF_DSP(39,9,Position,sys_env.bTIME_ON);break;
 	    case 3 : Data_Format(Position);Data_Run();break;
 		case 4 : Select = tBIT_CHECK[vITEM_X];
				 Time_Correct_Con(Select,Position);break;	
// 		case 5 : TIME_SIZE_Dsp(sys_env.vTIME_Size,Position);break;	
 		case 5 : TIME_LOC_Dsp(sys_env.vTIME_Position,Position);break;	
	}
}

void tPAGE1_KEY(void)
{
	u8 State=0;		
	struct tm time_tm;

	switch(GetCurrentKey())
	{
		case UP_KEY :
			State=0xff;
		case DOWN_KEY : 
			if(bENTER)
			{
				bSETUP_Change_flag = 1;

				if(vITEM_Y==0)
				{
					if(vITEM_X==0) INC_Dec_Hex(23,0,State,&rtc_hour);
					if(vITEM_X==1) INC_Dec_Hex(59,0,State,&rtc_min);	 		 					
					if(vITEM_X==2) INC_Dec_Hex(59,0,State,&rtc_sec);		 		 									

					time_tm.tm_year = Bcd2Hex(rtc_year);
				    time_tm.tm_mon  = Bcd2Hex(rtc_month)-1;
				    time_tm.tm_mday = Bcd2Hex(rtc_day)-1;
				    time_tm.tm_hour = Bcd2Hex(rtc_hour);
				    time_tm.tm_min  = Bcd2Hex(rtc_min);
				    time_tm.tm_sec  = Bcd2Hex(rtc_sec);
				    Change_RTC_Cnt(&time_tm);
				}
				else if(vITEM_Y==1)
				{
					if(vITEM_X==0)
					{
						if(sys_env.vDATE_FORMAT==0) INC_Dec_Hex(99,00,State,&rtc_year);
						if(sys_env.vDATE_FORMAT==1) INC_Dec_Hex(12,01,State,&rtc_month);
						if(sys_env.vDATE_FORMAT==2) INC_Dec_Hex(31,01,State,&rtc_day);
					}
					else if(vITEM_X==1)
					{
						if(sys_env.vDATE_FORMAT==0) INC_Dec_Hex(12,01,State,&rtc_month);
						if(sys_env.vDATE_FORMAT==1) INC_Dec_Hex(31,01,State,&rtc_day);
						if(sys_env.vDATE_FORMAT==2) INC_Dec_Hex(12,01,State,&rtc_month);
					}
					else if(vITEM_X==2)
					{
						if(sys_env.vDATE_FORMAT==0) INC_Dec_Hex(31,01,State,&rtc_day);
						if(sys_env.vDATE_FORMAT==1) INC_Dec_Hex(50,00,State,&rtc_year);
						if(sys_env.vDATE_FORMAT==2) INC_Dec_Hex(50,00,State,&rtc_year);
					}

					time_tm.tm_year = Bcd2Hex(rtc_year);
				    time_tm.tm_mon  = Bcd2Hex(rtc_month)-1;
				    time_tm.tm_mday = Bcd2Hex(rtc_day)-1;
				    time_tm.tm_hour = Bcd2Hex(rtc_hour);
				    time_tm.tm_min  = Bcd2Hex(rtc_min);
				    time_tm.tm_sec  = Bcd2Hex(rtc_sec);
				    Change_RTC_Cnt(&time_tm);
				}		
				else if(vITEM_Y==2)
				{
					INC_Dec_Hex(1,0,State,&sys_env.bTIME_ON);
					EEP_buf[cSYSENV_bTIME_ON] = sys_env.bTIME_ON;
				}
				else if(vITEM_Y==3)
				{
					Inc_Dec_Count(2,0,State,&sys_env.vDATE_FORMAT);
					EEP_buf[cSYSENV_vDATE_FORMAT] = sys_env.vDATE_FORMAT;
				}
				else if(vITEM_Y==4)
				{
	               	if(vITEM_X==0) {INC_Dec_Hex(1,0,State,&sys_env.bVECTOR); EEP_buf[cSYSENV_bVECTOR] = sys_env.bVECTOR;}	 		 					
					if(vITEM_X==1) {INC_Dec_Hex(59,0,State,&sys_env.vCORRECT_OFFSET); EEP_buf[cSYSENV_vCORRECT_OFFSET] = sys_env.vCORRECT_OFFSET;}	 		 						
				    if(vITEM_X==2) {INC_Dec_Hex(1,0,State,&sys_env.bCORRECT); EEP_buf[cSYSENV_bCORRECT] = sys_env.bCORRECT;}	 		 					 
				}
/*				else if(vITEM_Y==5)
				{
					Inc_Dec_Count(2,0,State,&sys_env.vTIME_Size);
					if(sys_env.vOSD_Position < 3) OSG_block_fill(0,scrsize.cy-80, 1920,80, 0xffff);
					else OSG_block_fill(0,0, 1920,80, 0xffff);
					if(sys_env.bTIME_ON) OSG_Display_Time_NOW();

				}
*/				else if(vITEM_Y==5)
				{
					Inc_Dec_Count(2,0,State,&sys_env.vTIME_Position);
					//if(sys_env.vOSD_Position < 3) OSG_block_fill(0,scrsize.cy-80, 1920,80, 0xffff);
					//else OSG_block_fill(0,0, 1920,80, 0xffff);
					//if(sys_env.bTIME_ON) OSG_Display_Time_NOW();
					EEP_buf[cSYSENV_vTIME_Position] = sys_env.vTIME_Position;
				}
				
				tPAGE1_Position(UNDER_BAR);
			}
			else
			{
				if(!vITEM_Y)RTC_DateCon(D_P_YEAR|D_P_MON|D_P_DAY,NULL,0);
				Inc_Dec_Count(5,0,~State,&vITEM_Y);
				MenuSelect(vITEM_Y,0);
				if(!vITEM_Y)RTC_DateCon(D_P_YEAR|D_P_MON|D_P_DAY,NULL,0);
			}
			break;

		case RIGHT_KEY :
			State=0xff;
		case LEFT_KEY  : 
			if(bENTER)
			{
			  	if(vITEM_Y==0)
				{
					tPAGE1_Position(NULL);
					Inc_Dec_Count(2,0,State,&vITEM_X);
					tPAGE1_Position(UNDER_BAR);
				}
			  	else if(vITEM_Y==1)
				{
					tPAGE1_Position(NULL);
					Inc_Dec_Count(2,0,State,&vITEM_X);
					tPAGE1_Position(UNDER_BAR);
				}
				else if(vITEM_Y==4)
				{
					tPAGE1_Position(NULL);
					Inc_Dec_Count(2,0,State,&vITEM_X);
					tPAGE1_Position(UNDER_BAR);
				}
			}
			break;

		case ENTER_KEY :
	       if(bENTER)
			{
				tPAGE1_Position(NULL);
				vITEM_X	= 0;
	       	bENTER = 0; 
			}
	       else 
			{
				tPAGE1_Position(UNDER_BAR);
				vITEM_X = 0;
	       	bENTER = 1; 
			}
			break; 	

		case EXIT_KEY : 
	       if(bENTER)
			{
				tPAGE1_Position(NULL);
				vITEM_X	= 0;
	       	bENTER = 0; 
			}
	       else 
			{
				vITEM_Y	= vPAGE - 1; 
				SetupMenu();
			}
			break;
	}
}
   

//------------------------------------------------------------------
//	PAGE2 Function
//------------------------------------------------------------------
const u8 cCAMERA_TITLE2[]	= "- CAMERA -";
const u8 cCH01_2[]			= " 1. CH01_NAME: ";
const u8 cCH02_2[]			= " 2. CH02_NAME: ";
const u8 cCH03_2[]			= " 3. CH03_NAME: ";
const u8 cCH04_2[]			= " 4. CH04_NAME: ";
const u8 cCH05_2[]			= " 5. CH05_NAME: ";
const u8 cCH06_2[]			= " 6. CH06_NAME: ";
const u8 cCH07_2[]			= " 7. CH07_NAME: ";
const u8 cCH08_2[]			= " 8. CH08_NAME: ";
const u8 cCH09_2[]			= " 9. CH09_NAME: ";

#ifdef __4CH__
const u8 cTITLE_DISPLAY2[]	= " 5. TITLE DISPLAY:";
#endif


void PAGE2_TITLE(void)
{
	vPAGE = 2;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);

	CodeWriteChar(27,2,cCAMERA_TITLE2,NULL,0);

#ifdef __4CH__
    CodeWriteChar(20,5,cCH01_2,NULL,0);
	WriteChar(35,5,&sys_env.vCH_NAME[0][0],NULL,12);

    CodeWriteChar(20,7,cCH02_2,NULL,0);
	WriteChar(35,7,&sys_env.vCH_NAME[1][0],NULL,12);

    CodeWriteChar(20,9,cCH03_2,NULL,0);
	WriteChar(35,9,&sys_env.vCH_NAME[2][0],NULL,12);

    CodeWriteChar(20,11,cCH04_2,NULL,0);
	WriteChar(35,11,&sys_env.vCH_NAME[3][0],NULL,12);

    CodeWriteChar(20,13,cTITLE_DISPLAY2,NULL,0);
	ON_OFF_DSP(41,13,NULL,sys_env.bTITLE_ON);	
#endif
}

void tPAGE2_KEY(void)
{
	u8 State=0;		
	u8 vFONT;

	switch(GetCurrentKey())
	{
	  	case UP_KEY  :
	  		State=0xff;
	    case DOWN_KEY  : 
	    	if(bENTER)
			{  
				bSETUP_Change_flag = 1;
#ifdef __4CH__
				if(vITEM_Y < 4)
				{
					vFONT = sys_env.vCH_NAME[vITEM_Y][vITEM_X];
					Inc_Dec_Count(0x7e,0x20,State,&vFONT);
					sys_env.vCH_NAME[vITEM_Y][vITEM_X] = vFONT;						
					EEP_buf[cSYSENV_vCH_NAME+vITEM_X+(vITEM_Y*12)] = sys_env.vCH_NAME[vITEM_Y][vITEM_X];

					WriteChar(35+vITEM_X,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X],UNDER_BAR,1);
				}				
			  	else if(vITEM_Y==4)
				{
					INC_Dec_Hex(1,0,State,&sys_env.bTITLE_ON);
					EEP_buf[cSYSENV_bTITLE_ON] = sys_env.bTITLE_ON;
					ON_OFF_DSP(41,13,UNDER_BAR,sys_env.bTITLE_ON);	
				}
#endif
			}
			else
			{
#ifdef __4CH__
				Inc_Dec_Count(4,0,~State,&vITEM_Y);
				MenuSelect(vITEM_Y,0);	
#endif
			}
	  		break;

		case RIGHT_KEY :
			State=0xff;
		case LEFT_KEY  : 
	    	if(bENTER)
			{
#ifdef __4CH__
				if(vITEM_Y < 4)
				{
	  				Inc_Dec_Count(11,0,State,&vITEM_X);
					WriteChar(35+vITEM_X,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X],UNDER_BAR,1);

					if(vITEM_X == 0)
					{
						WriteChar(35+vITEM_X+11,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X+11],NULL,1);
						WriteChar(35+vITEM_X+1,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X+1],NULL,1);
					}
					else if(vITEM_X == 11)
					{
						WriteChar(35+vITEM_X-11,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X-11],NULL,1);
						WriteChar(35+vITEM_X-1,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X-1],NULL,1);
					}
					else 
					{
						WriteChar(35+vITEM_X-1,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X-1],NULL,1);
						WriteChar(35+vITEM_X+1,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X+1],NULL,1);
					}
				}
#endif
			}
			else
			{
				MenuSelect(vITEM_Y,0);	
			}
			break;
	  
		case ENTER_KEY :
			if(bENTER) 
			{
#ifdef __4CH__
				bENTER = 0;
				if(vITEM_Y < 4)
				{
					WriteChar(35+vITEM_X,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X],NULL,1);

				}
				else if(vITEM_Y==4)
				{	
					ON_OFF_DSP(41,13,NULL,sys_env.bTITLE_ON); return;
				}
#endif
			}
	        else 
			{
#ifdef __4CH__
				bENTER = 1;
				vITEM_X = 0;

				if(vITEM_Y < 4)
				{
					WriteChar(35+vITEM_X,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X],UNDER_BAR,1);
				}
				else if(vITEM_Y==4)
				{	
					ON_OFF_DSP(41,13,UNDER_BAR,sys_env.bTITLE_ON); return;
				}
#endif
			}
			break; 	

		case EXIT_KEY : 
	    	if(bENTER)
			{
#ifdef __4CH__
				bENTER = 0;
				if(vITEM_Y < 4)
				{
					WriteChar(35+vITEM_X,5+(vITEM_Y*2),&sys_env.vCH_NAME[vITEM_Y][vITEM_X],NULL,1);
				}
				else if(vITEM_Y==4)
					ON_OFF_DSP(41,13,NULL,sys_env.bTITLE_ON);	     
				else 
					PAGE1_TITLE();	

				MenuSelect(vITEM_Y,0);	
#endif
			}
			else 
			{
				vITEM_Y = vPAGE - 1;
				SetupMenu();
			}
			break; 	
	}
}  


//------------------------------------------------------------------
//	PAGE3 Function
//------------------------------------------------------------------
#define SEQ_FULL	0
#define SEQ_4SPLIT	1

void Hex_Dec_OSD(u8 PX,u8 PY,u8 *P,u8 Position)
{
	vOSD_B[0] = (*P/10)+'0'; 
	vOSD_B[1] = (*P%10)+'0';
   WriteChar(PX,PY,vOSD_B,Position,2);
}


const u8 cOFF3[] = "OFF  ";
const u8 cSEC3[] = "SEC";
void Off_Check(u8 PX,u8 PY,u8 Split_mode,u8 Position)
{
	if(sys_env.vDWELL[Split_mode]==0) CodeWriteChar(PX,PY,cOFF3,Position,5);
	else
	{ 	
		Hex_Dec_OSD(PX,PY,&sys_env.vDWELL[Split_mode],Position); 
	   	CodeWriteChar(PX+2,PY,cSEC3,Position,3);
	}
}


const u8 cAUTO_SEQUENCE3[] = "- AUTO SEQUENCE -";
const u8 cFULL_3[]		   = "1. FULL   : ";
#ifdef __4CH__
const u8 cLOSS_AUTO_SKIP3[] = "2. LOSS AUTO SKIP: ";
#endif

void PAGE3_TITLE()
{
	vPAGE = 3;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);

	CodeWriteChar(23,2,cAUTO_SEQUENCE3,NULL,0);

    CodeWriteChar(23,6,cFULL_3,NULL,0);
	Off_Check(35,6,SEQ_FULL,NULL);

#ifdef __4CH__
    CodeWriteChar(23,8,cLOSS_AUTO_SKIP3,NULL,0);
	ON_OFF_DSP(42,8,NULL,sys_env.bLossAutoSkip);
#endif
}


void tPAGE3_Position(u8 Position)
{
	switch(vITEM_Y)
	{
    	case 0 : Off_Check(35,6,CH1,Position); break; 
#ifdef __4CH__
    	case 1 : ON_OFF_DSP(42,8,Position,sys_env.bLossAutoSkip); break; 
#endif
 	}
}

void tPAGE3_KEY(void)
{
	u8 State=0;		

	switch(GetCurrentKey()){
  	case UP_KEY  :
  		State=0xff;
    case DOWN_KEY  : 
		if(bENTER)
		{
			bSETUP_Change_flag = 1;

			switch(vITEM_Y)
 			{
#ifdef __4CH__
      			case 0 : Inc_Dec_Count(60,0,State,&sys_env.vDWELL[vITEM_Y]); EEP_buf[cSYSENV_vDWELL+vITEM_Y] = sys_env.vDWELL[vITEM_Y]; break;
       			case 1 : Inc_Dec_Count(1,0,State,&sys_env.bLossAutoSkip); EEP_buf[cSYSENV_bLossAutoSkip] = sys_env.bLossAutoSkip; break;
#endif
			}
			
			tPAGE3_Position(UNDER_BAR);
		}
		else
		{
#ifdef __4CH__
			Inc_Dec_Count(1,0,~State,&vITEM_Y);
#endif
			MenuSelect(vITEM_Y,0);
		}
  		break;

    case ENTER_KEY :
       if(bENTER)
		{
			bENTER = 0;
			tPAGE3_Position(NULL);
		}
		else 
		{
			bENTER = 1;
			tPAGE3_Position(UNDER_BAR);
		}
		break; 	

    case EXIT_KEY : 
		if(bENTER)
		{
			bENTER = 0;
			tPAGE3_Position(NULL);		
		}
		else 
		{
			vITEM_Y = vPAGE -1;
			SetupMenu();
		}
		break; 	
	}
}   

//------------------------------------------------------------------
//   PAGE4 Function
//------------------------------------------------------------------
//#define SPOT_FULL	0
//#define SPOT_QUAD	1

const u8 c1920X1080_60P[] = "1920x1080 60p";
const u8 c1920X1080_50P[] = "1920x1080 50p";
void Resolution_Dsp(u8 res, u8 Position)
{
	switch(res)
	{
		case 0: CodeWriteChar(37,6,c1920X1080_60P,Position,13);break; 
		case 1: CodeWriteChar(37,6,c1920X1080_50P,Position,13);break; 
	}
}

/*const u8 cOSD1[] = "NORMAL";
const u8 cOSD15[]= "1.5X  ";
const u8 cOSD2[] = "2X    ";
const u8 cOSD_AUTO[] = "AUTO  ";
void OSD_SIZE_Dsp(u8 res, u8 Position)
{
	switch(res)
	{
		case 0: CodeWriteChar(35,10,cOSD1,Position,6);break; 
		case 1: CodeWriteChar(35,10,cOSD15,Position,6);break; 
		case 2: CodeWriteChar(35,10,cOSD2,Position,6);break; 
		case 3: CodeWriteChar(35,10,cOSD_AUTO,Position,6);break; 
	}
}
*/
const u8 cOSD_POS_A[] = "A";
const u8 cOSD_POS_B[] = "B";
const u8 cOSD_POS_C[] = "C";
const u8 cOSD_POS_D[] = "D";
const u8 cOSD_POS_E[] = "E";
const u8 cOSD_POS_F[] = "F";
const u8 cOSD_POS_G[] = "G";
void OSD_Position_Dsp(u8 res, u8 Position)
{
	switch(res)
	{
		case 0: CodeWriteChar(39,10,cOSD_POS_A,Position,1);break; 
		case 1: CodeWriteChar(39,10,cOSD_POS_B,Position,1);break; 
		case 2: CodeWriteChar(39,10,cOSD_POS_C,Position,1);break; 
		case 3: CodeWriteChar(39,10,cOSD_POS_D,Position,1);break; 
		case 4: CodeWriteChar(39,10,cOSD_POS_E,Position,1);break; 
		case 5: CodeWriteChar(39,10,cOSD_POS_F,Position,1);break; 
		case 6: CodeWriteChar(39,10,cOSD_POS_G,Position,1);break; 
	}
}

const u8 c9SPLIT_9MODE[]  = "9SPLIT  ";
const u8 c9SPLIT_8MODE1[] = "8SPLIT-1";
const u8 c9SPLIT_8MODE2[] = "8SPLIT-2";
const u8 c9SPLIT_8MODE3[] = "8SPLIT-3";
const u8 c9SPLIT_8MODE4[] = "8SPLIT-4";
const u8 c9SPLIT_6MODE1[] = "6SPLIT-1";
const u8 c9SPLIT_6MODE2[] = "6SPLIT-2";
const u8 c9SPLIT_6MODE3[] = "6SPLIT-3";
const u8 c9SPLIT_6MODE4[] = "6SPLIT-4";
void Mode_9Split(u8 sel, u8 Position)
{
	switch(sel)
	{
		case 0: CodeWriteChar(38,14,c9SPLIT_9MODE,Position,6);break; 
		case 1: CodeWriteChar(38,14,c9SPLIT_8MODE1,Position,6);break; 
		case 2: CodeWriteChar(38,14,c9SPLIT_8MODE2,Position,6);break; 
		case 3: CodeWriteChar(38,14,c9SPLIT_8MODE3,Position,6);break; 
		case 4: CodeWriteChar(38,14,c9SPLIT_8MODE4,Position,6);break; 
		case 5: CodeWriteChar(38,14,c9SPLIT_6MODE1,Position,6);break; 
		case 6: CodeWriteChar(38,14,c9SPLIT_6MODE2,Position,6);break; 
		case 7: CodeWriteChar(38,14,c9SPLIT_6MODE3,Position,6);break; 
		case 8: CodeWriteChar(38,14,c9SPLIT_6MODE4,Position,6);break; 
	}
}

const u8 cDISPLAY4[]			 = "- DISPLAY -";
const u8 cRESOLUTION4[] 	  	= "1. RESOLUTION: ";
const u8 cOSD_DISPLAY_4[]	    = "2. OSD DISPLAY: ";
const u8 cOSD_POSTION_4[]	    = "3. OSD POSITION: ";
const u8 cBORDER_LINE_4[]	    = "4. BORDER LINE: ";
const u8 c9SPLIT_MODE_4[]	    = "5. 9SPLIT MODE: ";

void PAGE4_TITLE(void)
{
	vPAGE = 4;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);

	CodeWriteChar(24,2,cDISPLAY4,NULL,0);

    CodeWriteChar(22,6,cRESOLUTION4,NULL,0);
	Resolution_Dsp(sys_env.vResolution,NULL);

    CodeWriteChar(22,8,cOSD_DISPLAY_4,NULL,0);
	ON_OFF_DSP(38,8,NULL,sys_env.bOSD_Display);	

//    CodeWriteChar(22,10,cOSD_SIZE_4,NULL,0);

    CodeWriteChar(22,10,cOSD_POSTION_4,NULL,0);
	OSD_Position_Dsp(sys_env.vOSD_Position,NULL);

//	OSD_SIZE_Dsp(sys_env.vOSD_Size,NULL);
//	Mode_9Split(sys_env.b9Split_Mode,NULL);

    CodeWriteChar(22,12,cBORDER_LINE_4,NULL,0);
	ON_OFF_DSP(38,12,NULL,sys_env.border_line);

}


void tPAGE4_KEY(void)
{
	u8 State=0;

	switch(GetCurrentKey()){
	case UP_KEY : State=0xff;
    case DOWN_KEY : 
		if(bENTER)
		{
			bSETUP_Change_flag = 1;

			if(vITEM_Y==0)					
			{
				Inc_Dec_Count(1,0,State,&sys_env.vResolution);
				Resolution_Dsp(sys_env.vResolution,UNDER_BAR);
				EEP_buf[cSYSENV_resolution] = sys_env.vResolution;
			}
			else if(vITEM_Y==1)
			{
				Inc_Dec_Count(1,0,State,&sys_env.bOSD_Display);
				EEP_buf[cSYSENV_bOSD_Display] = sys_env.bOSD_Display;
				ON_OFF_DSP(38,8,UNDER_BAR,sys_env.bOSD_Display);	
			}
			else if(vITEM_Y==2)					
			{
#ifdef __4CH__
				Inc_Dec_Count(6,0,State,&sys_env.vOSD_Position);
#endif
				EEP_buf[cSYSENV_vOSD_Position] = sys_env.vOSD_Position;

				OSD_Position_Dsp(sys_env.vOSD_Position,UNDER_BAR);
			}
			else if(vITEM_Y==3)					
			{
				Inc_Dec_Count(1,0,State,&sys_env.border_line);
				EEP_buf[cSYSENV_border_line] = sys_env.border_line;
				ON_OFF_DSP(38,12,UNDER_BAR,sys_env.border_line);
			}
			else if(vITEM_Y==4)					
			{
				Inc_Dec_Count(8,0,State,&sys_env.b9Split_Mode);
				Mode_9Split(sys_env.b9Split_Mode,UNDER_BAR);

				EEP_buf[cSYSENV_b9Split_Mode] = sys_env.b9Split_Mode;

				if(sys_env.b9Split_Mode == 0)
				{
					sys_status.current_split_mode = SPLITMODE_SPLIT9_1;
					aux_display_flag = 1;
#if 0 //Louis
				    SGQ_9CH_INIT(change_mode[cmode]);
#endif
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}
				else if((sys_env.b9Split_Mode > 0) && (sys_env.b9Split_Mode < 5)) 
				{
					bMode_change_flag = 1;
					sys_status.current_split_mode = SPLITMODE_SPLIT9_1+sys_env.b9Split_Mode;
					pre_special_mode = LEFT_TOP+sys_env.b9Split_Mode-1;
#if 0 //Louis
					SGQ_16CH_INIT(change_mode[cmode]);
#endif
					aux_display_flag = 1;
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}
				else
				{
					bMode_change_flag = 1;
					sys_status.current_split_mode = SPLITMODE_SPLIT9_1+sys_env.b9Split_Mode;
					pre_special_mode = LEFT_TOP+sys_env.b9Split_Mode-1;
#if 0 //Louis
					SGQ_9CH_INIT(change_mode[cmode]);
#endif
					aux_display_flag = 1;
					DEMO_SetPIPViewWIND(0);	// update pip/pop window
				}
			}		
		}
		else		
		{ 
#ifdef __4CH__
			Inc_Dec_Count(3,0,~State,&vITEM_Y);
#endif
			MenuSelect(vITEM_Y,0);
		}
  		break;
		
    case ENTER_KEY :
       if(bENTER)
		{
			bENTER = 0;
			if(vITEM_Y==0)					
			{
				Resolution_Dsp(sys_env.vResolution,NULL);
			}
			else if(vITEM_Y==1)
			{
				ON_OFF_DSP(38,8,NULL,sys_env.bOSD_Display);	
			}
/*			else if(vITEM_Y==2)					
			{
				OSD_SIZE_Dsp(sys_env.vOSD_Size,NULL);
			}
*/			else if(vITEM_Y==2)					
			{
				OSD_Position_Dsp(sys_env.vOSD_Position,NULL);
			}
			else if(vITEM_Y==3)					
			{
				ON_OFF_DSP(38,12,NULL,sys_env.border_line);
			}
			else if(vITEM_Y==4)					
			{
				Mode_9Split(sys_env.b9Split_Mode,NULL);
			}
		}
		else 
		{
			bENTER = 1;
			if(vITEM_Y==0)					
			{
				Resolution_Dsp(sys_env.vResolution,UNDER_BAR);
			}
			else if(vITEM_Y==1)
			{
				ON_OFF_DSP(38,8,UNDER_BAR,sys_env.bOSD_Display);	
			}
/*			else if(vITEM_Y==2)					
			{
				OSD_SIZE_Dsp(sys_env.vOSD_Size,UNDER_BAR);
			}
*/			else if(vITEM_Y==2)					
			{
				OSD_Position_Dsp(sys_env.vOSD_Position,UNDER_BAR);
			}
			else if(vITEM_Y==3)					
			{
				ON_OFF_DSP(38,12,UNDER_BAR,sys_env.border_line);
			}
			else if(vITEM_Y==4)					
			{
				Mode_9Split(sys_env.b9Split_Mode,UNDER_BAR);
			}
		}
		break; 	

	case EXIT_KEY : 
    	if(bENTER)
		{
			bENTER = 0;
			if(vITEM_Y==0)					
			{
				Resolution_Dsp(sys_env.vResolution,NULL);
			}
			else if(vITEM_Y==1)
			{
				ON_OFF_DSP(38,8,NULL,sys_env.bOSD_Display);	
			}
/*			else if(vITEM_Y==2)					
			{
				OSD_SIZE_Dsp(sys_env.vOSD_Size,NULL);
			}
*/			else if(vITEM_Y==2)					
			{
				OSD_Position_Dsp(sys_env.vOSD_Position,NULL);
			}
			else if(vITEM_Y==3)					
			{
				ON_OFF_DSP(38,12,NULL,sys_env.border_line);
			}
			else if(vITEM_Y==4)					
			{
				Mode_9Split(sys_env.b9Split_Mode,NULL);
			}
		}
		else 
		{
			vITEM_Y = vPAGE - 1;
			SetupMenu();
		}
		break; 	
	}
}  

//------------------------------------------------------------------
//   PAGE5 Function
//------------------------------------------------------------------
#if 0
const u8 cON7[]	     = "ON ";
const u8 cOFF7[]	 = "OFF";
const u8 cOFF_BLK7[] = "   ";
void Remocon_Dsp(u8 Position)
{
	if(!sys_env.vREMOCON_ID)CodeWriteChar(34,6,cOFF7,Position,3);
	else
	{ 
		CodeWriteChar(34,6,cOFF_BLK7,NULL,3);
		Hex_Dec_OSD(34,6,&sys_env.vREMOCON_ID,Position);
	}
}

void Loss_Time_Dsp(u8 Position)
{
	if(!sys_env.vLoss_Time)CodeWriteChar(46,10,cOFF7,Position,3);
	else
	{ 
		CodeWriteChar(46,10,cOFF_BLK7,NULL,3);
		Hex_Dec_OSD(46,10,&sys_env.vLoss_Time,Position);
	}
}

const u8 cbaud_1200[]  = "1200BPS ";
const u8 cbaud_2400[]  = "2400BPS ";
const u8 cbaud_4800[]  = "4800BPS ";
const u8 cbaud_9600[]  = "9600BPS ";
const u8 cbaud_19200[] = "19200BPS";
void Baud_Rate_Dsp(u8 Position)
{
	if(sys_env.baud_rate == 0)CodeWriteChar(33,8,cbaud_1200,Position,8);
	else if(sys_env.baud_rate == 1)CodeWriteChar(33,8,cbaud_2400,Position,8);
	else if(sys_env.baud_rate == 2)CodeWriteChar(33,8,cbaud_4800,Position,8);
	else if(sys_env.baud_rate == 3)CodeWriteChar(33,8,cbaud_9600,Position,8);
	else if(sys_env.baud_rate == 4)CodeWriteChar(33,8,cbaud_19200,Position,8);
}
#endif

void tPAGE5_Position(u8 Position)
{
#if 0
	if(vITEM_Y==0) Remocon_Dsp(Position);
	else if(vITEM_Y==1) Baud_Rate_Dsp(Position);
	else if(vITEM_Y==2) Loss_Time_Dsp(Position);
	else if(vITEM_Y==3) ON_OFF_DSP(42,12,Position,sys_env.vLoss_Display);
#endif
}

const BYTE cALARM6[] = "- ALARM -";
const BYTE cCH1_6[] = "1. CH1 :     /           ";
const BYTE cCH2_6[] = "2. CH2 :     /           ";
const BYTE cCH3_6[] = "3. CH3 :     /           ";
const BYTE cCH4_6[] = "4. CH4 :     /           ";
const BYTE cALARM_OUT_TIME6[] = "5. ALARM OUT TIME :";
const BYTE cALARM_BUZZER6[] = "6. ALARM BUZZER :";
const BYTE cVIDEO_LOSS_BUZZER6[] = "7. VIDEO LOSS BUZZER :";


void PAGE5_TITLE(void)
{
	vPAGE = 5;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);

	CodeWriteChar(23,2,cALARM6,NULL,0);

    CodeWriteChar(19,6,cCH1_6,NULL,0);
	//Remocon_Dsp(NULL);

    CodeWriteChar(19,8,cCH2_6,NULL,0);
	//Baud_Rate_Dsp(NULL);

    CodeWriteChar(19,10,cCH3_6,NULL,0);
	//Loss_Time_Dsp(NULL);

    CodeWriteChar(19,12,cCH4_6,NULL,0);
	//ON_OFF_DSP(42,12,NULL,sys_env.vLoss_Display);

    CodeWriteChar(19,14,cALARM_OUT_TIME6,NULL,0);
    CodeWriteChar(19,16,cALARM_BUZZER6,NULL,0);
    CodeWriteChar(19,18,cVIDEO_LOSS_BUZZER6,NULL,0);
}


void tPAGE5_KEY(void)
{
	u8 State=0;		

	switch(GetCurrentKey()){
#if 0
  	case UP_KEY : State=0xff;
    case DOWN_KEY : 
		if(bENTER)
		{
			bSETUP_Change_flag = 1;

       	 	if(vITEM_Y==0)
			{
				Inc_Dec_Count(99,0,State,&sys_env.vREMOCON_ID);
				EEP_buf[cSYSENV_vREMOCON_ID] = sys_env.vREMOCON_ID;
			}
   	     	else if(vITEM_Y==1)
			{
				Inc_Dec_Count(4,0,State,&sys_env.baud_rate);
				EEP_buf[cSYSENV_baud_rate] = sys_env.baud_rate;

				USART3_Init();					// initialize Serial
			}
   	     	else if(vITEM_Y==2)
			{
				Inc_Dec_Count(60,0,State,&sys_env.vLoss_Time);
				EEP_buf[cSYSENV_vLoss_Time] = sys_env.vLoss_Time;
			}
   	     	else if(vITEM_Y==3)
			{
				Inc_Dec_Count(1,0,State,&sys_env.vLoss_Display);
				EEP_buf[cSYSENV_vLoss_Display] = sys_env.vLoss_Display;
			}
			
			tPAGE5_Position(UNDER_BAR);
		}
		else
		{
			Inc_Dec_Count(3,0,~State,&vITEM_Y);
			MenuSelect(vITEM_Y,0);
		}
  		break;

    case ENTER_KEY :
       if(bENTER)
		{
			bENTER = 0;
			tPAGE5_Position(NULL);
		}
		else 
		{
			bENTER = 1;
			tPAGE5_Position(UNDER_BAR);
		}
		break; 	
#endif
	case EXIT_KEY : 
       if(bENTER)
		{
			bENTER = 0;
			tPAGE5_Position(NULL);
		}
		else 
		{
           vITEM_Y = vPAGE -1;   
			SetupMenu();
		}
		break; 	
	}
}   

//------------------------------------------------------------------
//   PAGE6 Function
//------------------------------------------------------------------
#if 0
const u8 cON7[]	     = "ON ";
const u8 cOFF7[]	 = "OFF";
const u8 cOFF_BLK7[] = "   ";
void Remocon_Dsp(u8 Position)
{
	if(!sys_env.vREMOCON_ID)CodeWriteChar(34,6,cOFF7,Position,3);
	else
	{ 
		CodeWriteChar(34,6,cOFF_BLK7,NULL,3);
		Hex_Dec_OSD(34,6,&sys_env.vREMOCON_ID,Position);
	}
}

void Loss_Time_Dsp(u8 Position)
{
	if(!sys_env.vLoss_Time)CodeWriteChar(46,10,cOFF7,Position,3);
	else
	{ 
		CodeWriteChar(46,10,cOFF_BLK7,NULL,3);
		Hex_Dec_OSD(46,10,&sys_env.vLoss_Time,Position);
	}
}

const u8 cbaud_1200[]  = "1200BPS ";
const u8 cbaud_2400[]  = "2400BPS ";
const u8 cbaud_4800[]  = "4800BPS ";
const u8 cbaud_9600[]  = "9600BPS ";
const u8 cbaud_19200[] = "19200BPS";
void Baud_Rate_Dsp(u8 Position)
{
	if(sys_env.baud_rate == 0)CodeWriteChar(33,8,cbaud_1200,Position,8);
	else if(sys_env.baud_rate == 1)CodeWriteChar(33,8,cbaud_2400,Position,8);
	else if(sys_env.baud_rate == 2)CodeWriteChar(33,8,cbaud_4800,Position,8);
	else if(sys_env.baud_rate == 3)CodeWriteChar(33,8,cbaud_9600,Position,8);
	else if(sys_env.baud_rate == 4)CodeWriteChar(33,8,cbaud_19200,Position,8);
}
#endif

void tPAGE6_Position(u8 Position)
{
#if 0
	if(vITEM_Y==0) Remocon_Dsp(Position);
	else if(vITEM_Y==1) Baud_Rate_Dsp(Position);
	else if(vITEM_Y==2) Loss_Time_Dsp(Position);
	else if(vITEM_Y==3) ON_OFF_DSP(42,12,Position,sys_env.vLoss_Display);
#endif
}

const BYTE cMOTION5[]		= "- MOTION -";
const BYTE cCH1_5[]			= "1. CH1  :       /";
const BYTE cCH2_5[]			= "2. CH2  :       /";
const BYTE cCH3_5[]			= "3. CH3  :       /";
const BYTE cCH4_5[]			= "4. CH4  :       /";
const BYTE cSENSITIVITY5[]	= "5. SENSITIVITY :";
const BYTE cCALL_MODE5[]	= "6. CALL MODE   :";
const BYTE cAREA_SAVE[]	    = "SAVING AREA...  ";


void PAGE6_TITLE(void)
{
	vPAGE = 6;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);

	CodeWriteChar(23,2,cMOTION5,NULL,0);

    CodeWriteChar(19,6,cCH1_5,NULL,0);
	//Remocon_Dsp(NULL);

    CodeWriteChar(19,8,cCH2_5,NULL,0);
	//Baud_Rate_Dsp(NULL);

    CodeWriteChar(19,10,cCH3_5,NULL,0);
	//Loss_Time_Dsp(NULL);

    CodeWriteChar(19,12,cCH4_5,NULL,0);
	//ON_OFF_DSP(42,12,NULL,sys_env.vLoss_Display);

    CodeWriteChar(19,14,cSENSITIVITY5,NULL,0);
    CodeWriteChar(19,16,cCALL_MODE5,NULL,0);
    CodeWriteChar(19,18,cAREA_SAVE,NULL,0);
}


void tPAGE6_KEY(void)
{
	u8 State=0;		

	switch(GetCurrentKey()){
#if 0
  	case UP_KEY : State=0xff;
    case DOWN_KEY : 
		if(bENTER)
		{
			bSETUP_Change_flag = 1;

       	 	if(vITEM_Y==0)
			{
				Inc_Dec_Count(99,0,State,&sys_env.vREMOCON_ID);
				EEP_buf[cSYSENV_vREMOCON_ID] = sys_env.vREMOCON_ID;
			}
   	     	else if(vITEM_Y==1)
			{
				Inc_Dec_Count(4,0,State,&sys_env.baud_rate);
				EEP_buf[cSYSENV_baud_rate] = sys_env.baud_rate;

				USART3_Init();					// initialize Serial
			}
   	     	else if(vITEM_Y==2)
			{
				Inc_Dec_Count(60,0,State,&sys_env.vLoss_Time);
				EEP_buf[cSYSENV_vLoss_Time] = sys_env.vLoss_Time;
			}
   	     	else if(vITEM_Y==3)
			{
				Inc_Dec_Count(1,0,State,&sys_env.vLoss_Display);
				EEP_buf[cSYSENV_vLoss_Display] = sys_env.vLoss_Display;
			}
			
			tPAGE5_Position(UNDER_BAR);
		}
		else
		{
			Inc_Dec_Count(3,0,~State,&vITEM_Y);
			MenuSelect(vITEM_Y,0);
		}
  		break;

    case ENTER_KEY :
       if(bENTER)
		{
			bENTER = 0;
			tPAGE5_Position(NULL);
		}
		else 
		{
			bENTER = 1;
			tPAGE5_Position(UNDER_BAR);
		}
		break; 	
#endif
	case EXIT_KEY : 
       if(bENTER)
		{
			bENTER = 0;
			tPAGE5_Position(NULL);
		}
		else 
		{
           vITEM_Y = vPAGE -1;   
			SetupMenu();
		}
		break; 	
	}

}   

//------------------------------------------------------------------
//   PAGE7 Function
//------------------------------------------------------------------
const u8 cON7[]	     = "ON ";
const u8 cOFF7[]	 = "OFF";
const u8 cOFF_BLK7[] = "   ";
void Remocon_Dsp(u8 Position)
{
	if(!sys_env.vREMOCON_ID)CodeWriteChar(34,6,cOFF7,Position,3);
	else
	{ 
		CodeWriteChar(34,6,cOFF_BLK7,NULL,3);
		Hex_Dec_OSD(34,6,&sys_env.vREMOCON_ID,Position);
	}
}

void Loss_Time_Dsp(u8 Position)
{
	if(!sys_env.vLoss_Time)CodeWriteChar(46,10,cOFF7,Position,3);
	else
	{ 
		CodeWriteChar(46,10,cOFF_BLK7,NULL,3);
		Hex_Dec_OSD(46,10,&sys_env.vLoss_Time,Position);
	}
}

const u8 cbaud_1200[]  = "1200BPS ";
const u8 cbaud_2400[]  = "2400BPS ";
const u8 cbaud_4800[]  = "4800BPS ";
const u8 cbaud_9600[]  = "9600BPS ";
const u8 cbaud_19200[] = "19200BPS";
void Baud_Rate_Dsp(u8 Position)
{
	if(sys_env.baud_rate == 0)CodeWriteChar(33,8,cbaud_1200,Position,8);
	else if(sys_env.baud_rate == 1)CodeWriteChar(33,8,cbaud_2400,Position,8);
	else if(sys_env.baud_rate == 2)CodeWriteChar(33,8,cbaud_4800,Position,8);
	else if(sys_env.baud_rate == 3)CodeWriteChar(33,8,cbaud_9600,Position,8);
	else if(sys_env.baud_rate == 4)CodeWriteChar(33,8,cbaud_19200,Position,8);
}


void tPAGE7_Position(u8 Position)
{
	if(vITEM_Y==0) Remocon_Dsp(Position);
	else if(vITEM_Y==1) Baud_Rate_Dsp(Position);
	else if(vITEM_Y==2) Loss_Time_Dsp(Position);
	else if(vITEM_Y==3) ON_OFF_DSP(42,12,Position,sys_env.vLoss_Display);
}

const u8 cMISC7[]            = "- MISCELLANEOUS -";
const u8 cPREMOCON_ID7[]     =  "1. REMOCON ID:";
const u8 cBAUDRATE_ID7[]     =  "2. BAUD RATE:";
const u8 cLOSS_TIME_ID7[]    =  "3. VIDEO LOSS BUZZER TIME:";
const u8 cLOSS_DISPLAY_ID7[] =  "4. VIDEO LOSS DISPLAY:";

const u8 cSW_VER_7[]         = "F/W: v1.1.1";


void PAGE7_TITLE(void)
{
	vPAGE = 7;
	Erase_Menu_OSD();                          

	MenuSelect(0,0);

	CodeWriteChar(23,2,cMISC7,NULL,0);

    CodeWriteChar(19,6,cPREMOCON_ID7,NULL,0);
	Remocon_Dsp(NULL);

    CodeWriteChar(19,8,cBAUDRATE_ID7,NULL,0);
	Baud_Rate_Dsp(NULL);

    CodeWriteChar(19,10,cLOSS_TIME_ID7,NULL,0);
	Loss_Time_Dsp(NULL);

    CodeWriteChar(19,12,cLOSS_DISPLAY_ID7,NULL,0);
	ON_OFF_DSP(42,12,NULL,sys_env.vLoss_Display);

    CodeWriteChar(19,18,cSW_VER_7,NULL,0);
}


void tPAGE7_KEY(void)
{
	u8 State=0;		

	switch(GetCurrentKey()){
  	case UP_KEY : State=0xff;
    case DOWN_KEY : 
		if(bENTER)
		{
			bSETUP_Change_flag = 1;

       	 	if(vITEM_Y==0)
			{
				Inc_Dec_Count(99,0,State,&sys_env.vREMOCON_ID);
				EEP_buf[cSYSENV_vREMOCON_ID] = sys_env.vREMOCON_ID;
			}
   	     	else if(vITEM_Y==1)
			{
				Inc_Dec_Count(4,0,State,&sys_env.baud_rate);
				EEP_buf[cSYSENV_baud_rate] = sys_env.baud_rate;

				USART3_Init();					// initialize Serial
			}
   	     	else if(vITEM_Y==2)
			{
				Inc_Dec_Count(60,0,State,&sys_env.vLoss_Time);
				EEP_buf[cSYSENV_vLoss_Time] = sys_env.vLoss_Time;
			}
   	     	else if(vITEM_Y==3)
			{
				Inc_Dec_Count(1,0,State,&sys_env.vLoss_Display);
				EEP_buf[cSYSENV_vLoss_Display] = sys_env.vLoss_Display;
			}
			
			tPAGE5_Position(UNDER_BAR);
		}
		else
		{
			Inc_Dec_Count(3,0,~State,&vITEM_Y);
			MenuSelect(vITEM_Y,0);
		}
  		break;

    case ENTER_KEY :
       if(bENTER)
		{
			bENTER = 0;
			tPAGE5_Position(NULL);
		}
		else 
		{
			bENTER = 1;
			tPAGE5_Position(UNDER_BAR);
		}
		break; 	
	case EXIT_KEY : 
       if(bENTER)
		{
			bENTER = 0;
			tPAGE5_Position(NULL);
		}
		else 
		{
           vITEM_Y = vPAGE -1;   
			SetupMenu();
		}
		break; 	
	}
}   

