#include "common.h"

BYTE bMode_change_flag = 0;

struct osd_location 
{
	u8 state;
	u16 loc_x;
	u16 loc_y;
	u8 length;
};

struct osd_location osd_ch_name_location_buf[9];
struct osd_location osd_video_lose_location_buf[9];
struct osd_location osd_freeze_autoseq_location_buf[2];

//-----------------------------------------------------------------------------
// 4���� OSG ��ġ��
//-----------------------------------------------------------------------------
const u16 tbl_OSG_SPLIT4_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  480,   0,   960,   0,    0, 540,   480, 540,    960, 540,	//CH01
     960,   0, 1440,   0,  1920,   0,  960, 540,  1440, 540,   1920, 540,	//CH02
       0, 540,  480, 540,   960, 540,    0,1080,   480,1080,    960,1080,	//CH03
     960, 540, 1440, 540,  1920, 540,  960,1080,  1440,1080,   1920,1080,	//CH04
};

#ifdef __9CH_DEVICE__
//-----------------------------------------------------------------------------
// 9���� OSG ��ġ��
//-----------------------------------------------------------------------------
const u16 tbl_OSG_SPLIT9_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  320,   0,   640,   0,    0, 360,   320, 360,    640, 360,	//CH01
     640,   0,  960,   0,  1280,   0,  640, 360,   960, 360,   1280, 360,	//CH02
    1280,   0, 1600,   0,  1920,   0, 1280, 360,  1600, 360,   1920, 360,	//CH03

       0, 360,  320, 360,   640, 360,    0, 720,   320, 720,    640, 720,	//CH04
     640, 360,  960, 360,  1280, 360,  640, 720,   960, 720,   1280, 720,	//CH05
    1280, 360, 1600, 360,  1920, 360, 1280, 720,  1600, 720,   1920, 720,	//CH06

       0, 720,  320, 720,   640, 720,    0,1080,   320,1080,    640,1080,	//CH07
     640, 720,  960, 720,  1280, 720,  640,1080,   960,1080,   1280,1080,	//CH08
    1280, 720, 1600, 720,  1920, 720, 1280,1080,  1600,1080,   1920,1080,	//CH09
};

//-----------------------------------------------------------------------------
// 8���� OSG ��ġ��
//-----------------------------------------------------------------------------
/*const u16 tbl_OSG_SPLIT8_1_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  720,   0,  1440,   0,    0, 810,   720, 810,   1440, 810,	//CH01

       0, 810,  240, 810,   480, 810,    0,1080,   240,1080,    480,1080,	//CH02
     480, 810,  720, 810,   960, 810,  480,1080,   720,1080,    960,1080,	//CH03
     960, 810, 1200, 810,  1440, 810,  960,1080,  1200,1080,   1440,1080,	//CH04

    1440,   0, 1680,   0,  1920,   0, 1440, 270,  1680, 270,   1920, 270,	//CH05
    1440, 270, 1680, 270,  1920, 270, 1440, 540,  1680, 540,   1920, 540,	//CH06
    1440, 540, 1680, 540,  1920, 540, 1440, 810,  1680, 810,   1920, 810,	//CH07
    1440, 810, 1680, 810,  1920, 810, 1440,1080,  1680,1080,   1920,1080,	//CH08
};

const u16 tbl_OSG_SPLIT8_2_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
     480,   0, 1200,   0,  1920,   0,  480, 810,  1200, 810,   1920, 810,	//CH01

       0,   0,  240,   0,   480,   0,    0, 270,   240, 270,    480, 270,	//CH02
       0, 270,  240, 270,   480, 270,    0, 540,   240, 540,    480, 540,	//CH03
       0, 540,  240, 540,   480, 540,    0, 810,   240, 810,    480, 810,	//CH04
       0, 810,  240, 810,   480, 810,    0,1080,   240,1080,    480,1080,	//CH05

     480, 810,  720, 810,   960, 810,  480,1080,   720,1080,    960,1080,	//CH06
     960, 810, 1200, 810,  1440, 810,  960,1080,  1200,1080,   1440,1080,	//CH07
    1440, 810, 1680, 810,  1920, 810, 1440,1080,  1680,1080,   1920,1080,	//CH08
};

const u16 tbl_OSG_SPLIT8_3_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0, 270,  720, 270,  1440, 270,    0,1080,   720,1080,   1440,1080,	//CH01

       0,   0,  240,   0,   480,   0,    0, 270,   240, 270,    480, 270,	//CH02
     480,   0,  720,   0,   960,   0,  480, 270,   720, 270,    960, 270,	//CH03
     960,   0, 1200,   0,  1440,   0,  960, 270,  1200, 270,   1440, 270,	//CH04

    1440,   0, 1680,   0,  1920,   0, 1440, 270,  1680, 270,   1920, 270,	//CH05
    1440, 270, 1680, 270,  1920, 270, 1440, 540,  1680, 540,   1920, 540,	//CH06
    1440, 540, 1680, 540,  1920, 540, 1440, 810,  1680, 810,   1920, 810,	//CH07
    1440, 810, 1680, 810,  1920, 810, 1440,1080,  1680,1080,   1920,1080,	//CH08
};

const u16 tbl_OSG_SPLIT8_4_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
     480, 270, 1200, 270,  1920, 270,  480,1080,  1200,1080,   1920,1080,	//CH01

       0,   0,  240,   0,   480,   0,    0, 270,   240, 270,    480, 270,	//CH02
     480,   0,  720,   0,   960,   0,  480, 270,   720, 270,    960, 270,	//CH03
     960,   0, 1200,   0,  1440,   0,  960, 270,  1200, 270,   1440, 270,	//CH04
    1440,   0, 1680,   0,  1920,   0, 1440, 270,  1680, 270,   1920, 270,	//CH05

       0, 270,  240, 270,   480, 270,    0, 540,   240, 540,    480, 540,	//CH06
       0, 540,  240, 540,   480, 540,    0, 810,   240, 810,    480, 810,	//CH07
       0, 810,  240, 810,   480, 810,    0,1080,   240,1080,    480,1080,	//CH08
};*/

const u16 tbl_OSG_SPLIT8_1_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0, 810,  240, 810,   480, 810,    0,1080,   240,1080,    480,1080,	//CH01
     480, 810,  720, 810,   960, 810,  480,1080,   720,1080,    960,1080,	//CH02
     960, 810, 1200, 810,  1440, 810,  960,1080,  1200,1080,   1440,1080,	//CH03
    1440,   0, 1680,   0,  1920,   0, 1440, 270,  1680, 270,   1920, 270,	//CH04
    1440, 270, 1680, 270,  1920, 270, 1440, 540,  1680, 540,   1920, 540,	//CH05
    1440, 540, 1680, 540,  1920, 540, 1440, 810,  1680, 810,   1920, 810,	//CH06
    1440, 810, 1680, 810,  1920, 810, 1440,1080,  1680,1080,   1920,1080,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
       0,   0,  720,   0,  1440,   0,    0, 810,   720, 810,   1440, 810,	//CH09
};

const u16 tbl_OSG_SPLIT8_2_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  240,   0,   480,   0,    0, 270,   240, 270,    480, 270,	//CH01
       0, 270,  240, 270,   480, 270,    0, 540,   240, 540,    480, 540,	//CH02
       0, 540,  240, 540,   480, 540,    0, 810,   240, 810,    480, 810,	//CH03
       0, 810,  240, 810,   480, 810,    0,1080,   240,1080,    480,1080,	//CH04
     480, 810,  720, 810,   960, 810,  480,1080,   720,1080,    960,1080,	//CH05
     960, 810, 1200, 810,  1440, 810,  960,1080,  1200,1080,   1440,1080,	//CH06
    1440, 810, 1680, 810,  1920, 810, 1440,1080,  1680,1080,   1920,1080,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
     480,   0, 1200,   0,  1920,   0,  480, 810,  1200, 810,   1920, 810,	//CH09
};

const u16 tbl_OSG_SPLIT8_3_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  240,   0,   480,   0,    0, 270,   240, 270,    480, 270,	//CH01
     480,   0,  720,   0,   960,   0,  480, 270,   720, 270,    960, 270,	//CH02
     960,   0, 1200,   0,  1440,   0,  960, 270,  1200, 270,   1440, 270,	//CH03
    1440,   0, 1680,   0,  1920,   0, 1440, 270,  1680, 270,   1920, 270,	//CH04
    1440, 270, 1680, 270,  1920, 270, 1440, 540,  1680, 540,   1920, 540,	//CH05
    1440, 540, 1680, 540,  1920, 540, 1440, 810,  1680, 810,   1920, 810,	//CH06
    1440, 810, 1680, 810,  1920, 810, 1440,1080,  1680,1080,   1920,1080,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
       0, 270,  720, 270,  1440, 270,    0,1080,   720,1080,   1440,1080,	//CH09
};

const u16 tbl_OSG_SPLIT8_4_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  240,   0,   480,   0,    0, 270,   240, 270,    480, 270,	//CH01
     480,   0,  720,   0,   960,   0,  480, 270,   720, 270,    960, 270,	//CH02
     960,   0, 1200,   0,  1440,   0,  960, 270,  1200, 270,   1440, 270,	//CH03
    1440,   0, 1680,   0,  1920,   0, 1440, 270,  1680, 270,   1920, 270,	//CH04
       0, 270,  240, 270,   480, 270,    0, 540,   240, 540,    480, 540,	//CH05
       0, 540,  240, 540,   480, 540,    0, 810,   240, 810,    480, 810,	//CH06
       0, 810,  240, 810,   480, 810,    0,1080,   240,1080,    480,1080,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
     480, 270, 1200, 270,  1920, 270,  480,1080,  1200,1080,   1920,1080,	//CH09
};

//-----------------------------------------------------------------------------
// 6���� OSG ��ġ��
//-----------------------------------------------------------------------------
const u16 tbl_OSG_SPLIT6_1_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0, 720,  320, 720,   640, 720,    0,1080,   320,1080,    640,1080,	//CH01
     640, 720,  960, 720,  1280, 720,  640,1080,   960,1080,   1280,1080,	//CH02
    1280,   0, 1600,   0,  1920,   0, 1280, 360,  1600, 360,   1920, 360,	//CH03
    1280, 360, 1600, 360,  1920, 360, 1280, 720,  1600, 720,   1920, 720,	//CH04
    1280, 720, 1600, 720,  1920, 720, 1280,1080,  1600,1080,   1920,1080,	//CH05
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH06
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
       0,   0,  640,   0,  1280,   0,    0, 720,   640, 720,   1280, 720,	//CH09
};

const u16 tbl_OSG_SPLIT6_2_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  320,   0,   640,   0,    0, 360,   320, 360,    640, 360,	//CH01
       0, 360,  320, 360,   640, 360,    0, 720,   320, 720,    640, 720,	//CH02
       0, 720,  320, 720,   640, 720,    0,1080,   320,1080,    640,1080,	//CH03
     640, 720,  960, 720,  1280, 720,  640,1080,   960,1080,   1280,1080,	//CH04
    1280, 720, 1600, 720,  1920, 720, 1280,1080,  1600,1080,   1920,1080,	//CH05
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH06
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
     640,   0, 1280,   0,  1920,   0,  640, 720,  1280, 720,   1920, 720,	//CH09
};

const u16 tbl_OSG_SPLIT6_3_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  320,   0,   640,   0,    0, 360,   320, 360,    640, 360,	//CH01
     640,   0,  960,   0,  1280,   0,  640, 360,   960, 360,   1280, 360,	//CH02
    1280,   0, 1600,   0,  1920,   0, 1280, 360,  1600, 360,   1920, 360,	//CH03
    1280, 360, 1600, 360,  1920, 360, 1280, 720,  1600, 720,   1920, 720,	//CH04
    1280, 720, 1600, 720,  1920, 720, 1280,1080,  1600,1080,   1920,1080,	//CH05
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH06
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
       0, 360,  640, 360,  1280, 360,    0,1080,   640,1080,   1280,1080,	//CH09
};

const u16 tbl_OSG_SPLIT6_4_POSITION_1920x1080[] =			 
{
//  TopLeft    TopCenter  TopRight    BottomLeft  BottomCenter BottomRight	
       0,   0,  320,   0,   640,   0,    0, 360,   320, 360,    640, 360,	//CH01
     640,   0,  960,   0,  1280,   0,  640, 360,   960, 360,   1280, 360,	//CH02
    1280,   0, 1600,   0,  1920,   0, 1280, 360,  1600, 360,   1920, 360,	//CH03
       0, 360,  320, 360,   640, 360,    0, 720,   320, 720,    640, 720,	//CH04
       0, 720,  320, 720,   640, 720,    0,1080,   320,1080,    640,1080,	//CH05
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH06
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH07
       0,   0,    0,   0,     0,   0,    0,   0,     0,   0,      0,   0,	//CH08
     640, 360, 1280, 360,  1920, 360,  640,1080,  1280,1080,   1920,1080,	//CH09
};
#endif //__9CH_DEVICE__
BYTE str_Blank[] = "            ";
//-----------------------------------------------------------------------------
const u8 str_Freeze[] 		= "FREEZE  ";
const u8 str_Freeze2[] 		= "FREEZE";
const u8 str_Freeze3[] 		= "  FREEZE";
const u8 str_Freeze_BLK[] 	= "        ";
const u8 str_Freeze_BLK2[] 	= "      ";
//-----------------------------------------------------------------------------
const u8 str_AUTO[] = "AUTO  ";
const u8 str_AUTO2[] = "AUTO";
const u8 str_AUTO3[] = "  AUTO";
const u8 str_AUTO_BLK[] = "      ";
const u8 str_AUTO_BLK2[] = "    ";

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Osd_Init_Erase(void)
{
	int i;
	BYTE pSTR[1920/12];
	BYTE pSTR2[720/12];
	
	memset(pSTR, 0x20, sizeof(pSTR)); 
	memset(pSTR2, 0x20, sizeof(pSTR2)); 

	//OSD_SetSprite_layer0();
	
	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i=0;i<(1080/24);i++) 
	{
		MDINGAC_SetDrawXYMode(i*CHAR_HEIGHT, 0, pSTR, sizeof(pSTR), 0);
	}

	memset(osd_ch_name_location_buf, 0, sizeof(osd_ch_name_location_buf));
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

void OSG_Display_Time_Erase(void);
void OSD_Str_Loss_Erase(void);
void OSD_Str_Freeze_autoseq_Erase(void);

void Erase_OSD(void)
{
	int i = 0;

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i=0; i<NUM_OF_CHANNEL; i++)
	{
		if(osd_ch_name_location_buf[i].state == 1)
		{
			MDINGAC_SetDrawXYMode(osd_ch_name_location_buf[i].loc_y,
								osd_ch_name_location_buf[i].loc_x,
								str_Blank,
								osd_ch_name_location_buf[i].length,
								0);
		}
	}
	OSD_Str_Loss_Erase();
	if(!bAuto_Seq_Flag) OSD_Str_Freeze_autoseq_Erase();
	if(bSETUP) OSG_Display_Time_Erase();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Print_OSD_Str(u16 PosX, u16 PosY, const u8 *FontData, u8 ch)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_ch_name_location_buf[ch].state = 1;
	//osd_ch_name_location_buf[ch].loc_x = PosX-1;
	osd_ch_name_location_buf[ch].loc_x = PosX;
	osd_ch_name_location_buf[ch].loc_y = PosY;
	//osd_ch_name_location_buf[ch].length = strlen(FontData)+2;
	osd_ch_name_location_buf[ch].length = strlen(FontData);
}

void Print_OSD_Str2(u16 PosX, u16 PosY, const u8 *FontData)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

void Print_OSD_Str_Loss(u16 PosX, u16 PosY, const u8 *FontData, u8 ch)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_video_lose_location_buf[ch].state = 1;
	//osd_video_lose_location_buf[ch].loc_x = PosX-1;
	osd_video_lose_location_buf[ch].loc_x = PosX;
	osd_video_lose_location_buf[ch].loc_y = PosY;
	//osd_video_lose_location_buf[ch].length = strlen(FontData)+2;
	osd_video_lose_location_buf[ch].length = strlen(FontData);
}

BYTE space2[] = "          ";
void OSD_Str_Loss_Erase(void)
{
	u8 i = 0;
	
	for(i=0; i<NUM_OF_CHANNEL; i++)
	{
		if(osd_video_lose_location_buf[i].state == 1)
		{
			MDINGAC_SetDrawXYMode(osd_video_lose_location_buf[i].loc_y,
					osd_video_lose_location_buf[i].loc_x,
					str_Blank,
					osd_video_lose_location_buf[i].length,
					0);
		}
	}
}

void Print_OSD_Str_Freeze_Autoseq(u16 PosX, u16 PosY, const u8 *FontData)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_freeze_autoseq_location_buf[0].state = 1;
	//osd_freeze_autoseq_location_buf[0].loc_x = PosX-1;
	osd_freeze_autoseq_location_buf[0].loc_x = PosX;
	osd_freeze_autoseq_location_buf[0].loc_y = PosY;
	//osd_freeze_autoseq_location_buf[0].length = strlen(FontData)+2;
	osd_freeze_autoseq_location_buf[0].length = strlen(FontData);
}

BYTE space3[] = "        ";
void OSD_Str_Freeze_autoseq_Erase(void)
{
	//u8 i = 0;
	
	//for(i=0; i<1; i++)
	{
		if(osd_freeze_autoseq_location_buf[0].state == 1)
		{
			MDINGAC_SetDrawXYMode(osd_freeze_autoseq_location_buf[0].loc_y,
					osd_freeze_autoseq_location_buf[0].loc_x,
					str_Blank,
					osd_freeze_autoseq_location_buf[0].length,
					0);
		}
	}
}


void Print_OSD_Char(u16 PosX, u16 PosY, u8 *FontData, u16 SIZE, u8 ch)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, SIZE, 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_ch_name_location_buf[ch].state = 1;
	//osd_ch_name_location_buf[ch].loc_x = PosX-1;
	osd_ch_name_location_buf[ch].loc_x = PosX;
	osd_ch_name_location_buf[ch].loc_y = PosY;
	//osd_ch_name_location_buf[ch].length = SIZE+2;
	osd_ch_name_location_buf[ch].length = SIZE;
}

void Print_OSD_Char_Time(u16 PosX, u16 PosY, u8 *FontData, u16 SIZE)
{  
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, SIZE, 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSG_Display(void)
{
	if(sys_env.bOSD_Display)
	{
		if(bMode_change_flag && (!bSETUP))
		{
			//if(sys_env.bTITLE_ON) Erase_OSD();
			if(sys_env.bTITLE_ON) OSG_Display_CH_name();
			if(sys_env.bTIME_ON) OSG_Display_Time_NOW();
		}

		if(!bSETUP) OSG_Display_State();

		if(sys_env.bTIME_ON && (!bSETUP)) OSG_Display_Time();
	}

	bMode_change_flag = CLEAR;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
u8 Str_len(u8 *Ch_name)
{
	u8 i,j=12;
	u8 *Ch_name_Ptr = Ch_name;

	Ch_name_Ptr+=11;
	for(i=0;i<12;i++)
	{
		if((*Ch_name_Ptr) != ' ') break;	
		else Ch_name_Ptr--; 

		j--;
	}

	return j;	
}


ST_POINT Ext_HV_Pos(u8 CH, u8 OSD_Pos)
{
	ST_POINT Pos_Val;
	u8 vMODE;
	u8 vStart_CH; 
	
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)//SPLITMODE_FULL_CH9)
	{
		vMODE = FULL_SCREEN_MODE;
		vStart_CH = sys_status.current_split_mode;
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_1)
	{
		vMODE = SPLIT_4_MODE;
		vStart_CH = 0;
	}
#ifdef __9CH_DEVICE__
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_2)
	{
		vMODE = SPLIT_4_MODE;
		vStart_CH = 4;
	}
	else if(sys_status.current_split_mode >= SPLITMODE_SPLIT9_1)
	{
		vMODE = SPLIT_9_MODE;
		vStart_CH = 0;
	}
#endif
	switch(OSD_Pos)
	{
		case 0: 
		{
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					Pos_Val.x = 2;
					Pos_Val.y = 0;
				break;
					
				case SPLIT_4_MODE:
					Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[CH*12]+2;
					Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[(CH*12)+1];
				break;
#ifdef __9CH_DEVICE__
				case SPLIT_9_MODE:
					if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
					{
						Pos_Val.x = tbl_OSG_SPLIT9_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT9_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[CH*12]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[(CH*12)+1];
					}
				break;
#endif
			}
		}
		break;
		
		case 1:
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					Pos_Val.x = (1920/2)-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24)/2);
					Pos_Val.y = 0;
				break;
					
				case SPLIT_4_MODE:
					Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
					Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[2+(CH*12)+1];
				break;
#ifdef __9CH_DEVICE__
				case SPLIT_9_MODE:
					if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
					{
						Pos_Val.x = tbl_OSG_SPLIT9_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT9_POSITION_1920x1080[2+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[2+(CH*12)+1];
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[2+(CH*12)+1];
					} 
				break;
#endif
			}
		break;
		
		case 2:
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					//Pos_Val.x = 1920-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24);
					Pos_Val.x = 1920-2-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24)/2);
					Pos_Val.y = 0;
				break;
					
				case SPLIT_4_MODE:
					Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
					Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[4+(CH*12)+1];
				break;
#ifdef __9CH_DEVICE__
				case SPLIT_9_MODE:
					if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
					{
						Pos_Val.x = tbl_OSG_SPLIT9_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT9_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[4+(CH*12)+1];
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[4+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[4+(CH*12)+1];
					}
				break;
#endif
			}
		break;
		
		case 3:
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					Pos_Val.x = 2;
					//Pos_Val.y = 1080-48;
					Pos_Val.y = 1080-24;
				break;
					
				case SPLIT_4_MODE:
					Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[6+(CH*12)]+2;
					Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[6+(CH*12)+1]-24;
				break;
#ifdef __9CH_DEVICE__
				case SPLIT_9_MODE:
					if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
					{
						Pos_Val.x = tbl_OSG_SPLIT9_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT9_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[6+(CH*12)]+2;
						Pos_Val.y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[6+(CH*12)+1]-24;
					}
				break;
#endif
			}
		break;

		case 4:
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					Pos_Val.x = (1920/2)-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24)/2);
					//Pos_Val.y = 1080-48;
					Pos_Val.y = 1080-24;
				break;
					
				case SPLIT_4_MODE:
					Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
					Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[8+(CH*12)+1]-24;
				break;
#ifdef __9CH_DEVICE__
				case SPLIT_9_MODE:
					if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
					{
						Pos_Val.x = tbl_OSG_SPLIT9_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT9_POSITION_1920x1080[8+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[8+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[8+(CH*12)+1]-24;
					} 
				break;
#endif
			}
		break;

		case 5:
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					//Pos_Val.x = 1920-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24);
					Pos_Val.x = 1920-2-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24)/2);
					//Pos_Val.y = 1080-48;
					Pos_Val.y = 1080-24;
				break;
					
				case SPLIT_4_MODE:
					Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
					Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[10+(CH*12)+1]-24;
				break;
#ifdef __9CH_DEVICE__
				case SPLIT_9_MODE:
					if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
					{
						Pos_Val.x = tbl_OSG_SPLIT9_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT9_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
					{
						Pos_Val.x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
					else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
					{
						Pos_Val.x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[10+(CH*12)]-2-(Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12);
						Pos_Val.y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[10+(CH*12)+1]-24;
					}
				break;
#endif
			}
		break;

		case 6:
			switch(vMODE)
			{
				case FULL_SCREEN_MODE:
					Pos_Val.x = (1920/2)-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*24)/2);
					Pos_Val.y = 0;
				break;

				case SPLIT_4_MODE:
					if(CH >= 2)
					{
						Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[2+(CH*12)+1];
					}
					else 
					{
						CH += 2;
						Pos_Val.x = tbl_OSG_SPLIT4_POSITION_1920x1080[2+(CH*12)]-((Str_len(&sys_env.vCH_NAME[vStart_CH+CH][0])*12)/2);
						Pos_Val.y = tbl_OSG_SPLIT4_POSITION_1920x1080[2+(CH*12)+1]-24;
					}
				break;
			}
		break;
	}
	return Pos_Val;
}

void OSG_Display_CH_name(void)
{
	u8 cnt;
	ST_POINT Pos;
	u8 vMAX_Ch; 
	u8 vStart_CH; 
	u8 vMODE;
	
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH9)
	{
		vMODE = FULL_SCREEN_MODE;
		vMAX_Ch = 1;
		vStart_CH = sys_status.current_split_mode;
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_1)
	{
		vMODE = SPLIT_4_MODE;
		vMAX_Ch = 4;
		vStart_CH = 0;
	}
#ifdef __9CH_DEVICE__
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_2)
	{
		vMODE = SPLIT_4_MODE;
		vMAX_Ch = 4;
		vStart_CH = 4;
	}
	else if(sys_status.current_split_mode >= SPLITMODE_SPLIT9_1)
	{
		vMODE = SPLIT_9_MODE;
		vMAX_Ch = 9;
		vStart_CH = 0;
	}
#endif
	for(cnt=0;cnt<vMAX_Ch;cnt++)
	{
		Pos = Ext_HV_Pos(cnt, sys_env.vOSD_Position);

		if(vMODE == FULL_SCREEN_MODE)
		{
			Print_OSD_Char(Pos.x, Pos.y, sys_env.vCH_NAME[vStart_CH+cnt], Str_len(sys_env.vCH_NAME[vStart_CH+cnt]), vStart_CH+cnt);
		}
		else if(vMODE == SPLIT_4_MODE)
		{
			Print_OSD_Char(Pos.x, Pos.y, sys_env.vCH_NAME[vStart_CH+cnt], Str_len(sys_env.vCH_NAME[vStart_CH+cnt]), vStart_CH+cnt);
		}
#ifdef __9CH_DEVICE__
		else if(vMODE == SPLIT_9_MODE)
		{
			if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
				Print_OSD_Char(Pos.x, Pos.y, sys_env.vCH_NAME[vStart_CH+cnt], Str_len(sys_env.vCH_NAME[vStart_CH+cnt]), vStart_CH+cnt);
			else if(sys_status.current_split_mode > SPLITMODE_SPLIT9_1 && sys_status.current_split_mode < SPLITMODE_SPLIT9_6) 
			{
				if(cnt != 7)
					Print_OSD_Char(Pos.x, Pos.y, sys_env.vCH_NAME[vStart_CH+cnt], Str_len(sys_env.vCH_NAME[vStart_CH+cnt]), vStart_CH+cnt);
			}
			else 
			{
				if(cnt < 5 || cnt == 8)
					Print_OSD_Char(Pos.x, Pos.y, sys_env.vCH_NAME[vStart_CH+cnt], Str_len(sys_env.vCH_NAME[vStart_CH+cnt]), vStart_CH+cnt);
			}
		}
#endif
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSG_Display_Freeze(void)
{
	static u16 tblFreezeDisplayLoc_X[3][3] =
	{
		0,
	};
	static BOOL previous_freezeMode = CLEAR;
	BOOL current_freezeMode = IsScreenFreeze();
	u16 PosX;
	u16 PosY;


	if(previous_freezeMode != current_freezeMode)
	{
		previous_freezeMode = current_freezeMode;

		if(sys_env.bTIME_ON) 
		{
			tblFreezeDisplayLoc_X[0][0] = 6+(20*12);
			tblFreezeDisplayLoc_X[0][1] = 9+(20*18);
			tblFreezeDisplayLoc_X[0][2] = 12+(20*24);
			tblFreezeDisplayLoc_X[1][0] = (1920/2)-120+(20*12);
			tblFreezeDisplayLoc_X[1][1] = (1920/2)-180+(20*18);
			tblFreezeDisplayLoc_X[1][2] = (1920/2)-240+(20*24);
			tblFreezeDisplayLoc_X[2][0] = 1920-246-(8*12);
			tblFreezeDisplayLoc_X[2][1] = 1920-369-(8*18);
			tblFreezeDisplayLoc_X[2][2] = 1920-492-(8*24);

			PosX = tblFreezeDisplayLoc_X[sys_env.vTIME_Position][0];
			if(sys_env.vOSD_Position < 3 || sys_env.vOSD_Position == 6) PosY = 1080-24-4;
			else PosY = 4;

			if(current_freezeMode)
			{
				if(sys_env.vTIME_Position < 2)
				{
					Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_Freeze3);
				}
				else
				{
					Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_Freeze);
				}
			}
			else
			{
				Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_Freeze_BLK);
			}
		}
		else 
		{
			tblFreezeDisplayLoc_X[0][0] = 6;
			tblFreezeDisplayLoc_X[0][1] = 9;
			tblFreezeDisplayLoc_X[0][2] = 12;
			tblFreezeDisplayLoc_X[1][0] = (1920/2)-(3*12);
			tblFreezeDisplayLoc_X[1][1] = (1920/2)-(3*18);
			tblFreezeDisplayLoc_X[1][2] = (1920/2)-(3*24);
			tblFreezeDisplayLoc_X[2][0] = 1920-(6*12)-6;
			tblFreezeDisplayLoc_X[2][1] = 1920-(6*18)-9;
			tblFreezeDisplayLoc_X[2][2] = 1920-(6*24)-12;

			PosX = tblFreezeDisplayLoc_X[sys_env.vTIME_Position][0];
			if(sys_env.vOSD_Position < 3) PosY = 1080-24-4;
			else PosY = 4;

			if(current_freezeMode) Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_Freeze2);
			else Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_Freeze_BLK2);				
		}
	}	
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSG_Display_AUTO(void)
{
	static u16 tblAUTODisplayLoc_X[3][3] =
	{
		0,
	};
	static u8 Pre_bAuto_Seq_Flag = CLEAR;
	BOOL current_freezeMode = IsScreenFreeze();
	u16 PosX;
	u16 PosY = 1080-24-4;

	if(Pre_bAuto_Seq_Flag != bAuto_Seq_Flag)
	{
		Pre_bAuto_Seq_Flag = bAuto_Seq_Flag;
		
		if(sys_env.bTIME_ON) 
		{
			tblAUTODisplayLoc_X[0][0] = 6+(20*12);
			tblAUTODisplayLoc_X[0][1] = 9+(20*18);
			tblAUTODisplayLoc_X[0][2] = 12+(20*24);
			tblAUTODisplayLoc_X[1][0] = (1920/2)-120+(20*12);
			tblAUTODisplayLoc_X[1][1] = (1920/2)-180+(20*18);
			tblAUTODisplayLoc_X[1][2] = (1920/2)-240+(20*24);
			tblAUTODisplayLoc_X[2][0] = 1920-246-(6*12);
			tblAUTODisplayLoc_X[2][1] = 1920-369-(6*18);
			tblAUTODisplayLoc_X[2][2] = 1920-492-(6*24);

			PosX = tblAUTODisplayLoc_X[sys_env.vTIME_Position][0];
			//if(sys_env.vOSD_Position < 3) PosY = 1080-24-4;
			if(sys_env.vOSD_Position < 3 || sys_env.vOSD_Position == 6) PosY = 1080-24-4;
			else PosY = 4;

			if(bAuto_Seq_Flag) 
			{
				if(sys_env.vTIME_Position < 2) Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_AUTO3);			
				else Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_AUTO);			
			}
			else 
			{	
				if(!current_freezeMode) Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_AUTO_BLK);
			}
		}
		else 
		{
			tblAUTODisplayLoc_X[0][0] = 6;
			tblAUTODisplayLoc_X[0][1] = 9;
			tblAUTODisplayLoc_X[0][2] = 12;
			tblAUTODisplayLoc_X[1][0] = (1920/2)-(2*12);
			tblAUTODisplayLoc_X[1][1] = (1920/2)-(2*18);
			tblAUTODisplayLoc_X[1][2] = (1920/2)-(2*24);
			tblAUTODisplayLoc_X[2][0] = 1920-(4*12)-6;
			tblAUTODisplayLoc_X[2][1] = 1920-(4*18)-9;
			tblAUTODisplayLoc_X[2][2] = 1920-(4*24)-12;

			PosX = tblAUTODisplayLoc_X[sys_env.vTIME_Position][0];
			if(sys_env.vOSD_Position < 3) PosY = 1080-24-4;
			else PosY = 4;

			if(bAuto_Seq_Flag) Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_AUTO2);			
			else 
			{	
				if(!current_freezeMode) Print_OSD_Str_Freeze_Autoseq(PosX, PosY, str_AUTO_BLK2);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Video Loss ǥ��
//-----------------------------------------------------------------------------
const u8 str_NO_VIDEO[]= "VIDEO LOSS";
const u8 str_NO_VIDEO_Blk[]= "          ";
void OSG_Display_Video_Loss(void)
{
	ST_POINT Pos_Val[20];
	u8 CH = 0;

	u8 vMAX_Ch; 
	u8 vStart_CH; 
	u8 vMODE;
	
	if(sys_env.vLoss_Display & (Loss_Event_Flag || bMode_change_flag))
	{

		if(sys_status.current_split_mode <= SPLITMODE_FULL_CH9)
		{
			vMODE = FULL_SCREEN_MODE;
			vMAX_Ch = 1;
			vStart_CH = sys_status.current_split_mode;
		}
		else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_1)
		{
			vMODE = SPLIT_4_MODE;
			vMAX_Ch = 4;
			vStart_CH = 0;
		}
#ifdef __9CH_DEVICE__
		else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_2)
		{
			vMODE = SPLIT_4_MODE;
			vMAX_Ch = 4;
			vStart_CH = 4;
		}
		else if(sys_status.current_split_mode >= SPLITMODE_SPLIT9_1)
		{
			vMODE = SPLIT_9_MODE;
			vMAX_Ch = 9;
			vStart_CH = 0;
		}
#endif

		Loss_Event_Flag = 0;
	
		if(vMODE == FULL_SCREEN_MODE)
		{
			Pos_Val[vStart_CH].x = (1920/2)-(12*5);
			Pos_Val[vStart_CH].y = (1080/2)-12;
		}
		else if(vMODE == SPLIT_4_MODE)
		{
			for(CH=vStart_CH;CH<(vStart_CH+vMAX_Ch);CH++)
			{
				Pos_Val[CH].x = tbl_OSG_SPLIT4_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
				Pos_Val[CH].y = tbl_OSG_SPLIT4_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+270-12;
			}
		}
#ifdef __9CH_DEVICE__
		else if(vMODE == SPLIT_9_MODE)
		{
			for(CH=vStart_CH;CH<(vStart_CH+vMAX_Ch);CH++)
			{
				if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT9_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					Pos_Val[CH].y = tbl_OSG_SPLIT9_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+180-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT8_1_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+405-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT8_1_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+135-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT8_2_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+405-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT8_2_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+135-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT8_3_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+405-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT8_3_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+135-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT8_4_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+405-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT8_4_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+135-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT6_1_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+360-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT6_1_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+180-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT6_2_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+360-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT6_2_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+180-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT6_3_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+360-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT6_3_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+180-12;
				}
				else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
				{
					Pos_Val[CH].x = tbl_OSG_SPLIT6_4_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
					if(CH == 8) Pos_Val[CH].y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+360-12;
					else Pos_Val[CH].y = tbl_OSG_SPLIT6_4_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+180-12;
				}
			}
		}
#endif		
		for(CH=vStart_CH;CH<(vStart_CH+vMAX_Ch);CH++)
		{
			if(vVideo_Loss & (0x01<<CH))
			{
				if(sys_status.current_split_mode <= SPLITMODE_SPLIT9_1)
				{
					Print_OSD_Str_Loss(Pos_Val[CH].x, Pos_Val[CH].y, str_NO_VIDEO, CH);
				}
				else if((sys_status.current_split_mode >= SPLITMODE_SPLIT9_2) &&
						(sys_status.current_split_mode <= SPLITMODE_SPLIT9_5) &&
						(CH != 7))
				{
					Print_OSD_Str_Loss(Pos_Val[CH].x, Pos_Val[CH].y, str_NO_VIDEO, CH);
				}
				else if((sys_status.current_split_mode >= SPLITMODE_SPLIT9_6) &&
						(sys_status.current_split_mode <= SPLITMODE_SPLIT9_9) &&
						(!((CH >= 5) && (CH <= 7))))
				{
					Print_OSD_Str_Loss(Pos_Val[CH].x, Pos_Val[CH].y, str_NO_VIDEO, CH);
				}
				
			}
			else 
			{
				if(sys_status.current_split_mode <= SPLITMODE_SPLIT9_1)
				{
					Print_OSD_Str_Loss(Pos_Val[CH].x, Pos_Val[CH].y, str_NO_VIDEO_Blk, CH);
				}
				else if((sys_status.current_split_mode >= SPLITMODE_SPLIT9_2) &&
						(sys_status.current_split_mode <= SPLITMODE_SPLIT9_5) &&
						(CH != 7))
				{	Print_OSD_Str_Loss(Pos_Val[CH].x, Pos_Val[CH].y, str_NO_VIDEO_Blk, CH);
				}
				else if((sys_status.current_split_mode >= SPLITMODE_SPLIT9_6) &&
						(sys_status.current_split_mode <= SPLITMODE_SPLIT9_9) &&
						(!((CH >= 5) && (CH <= 7))))
				{
					Print_OSD_Str_Loss(Pos_Val[CH].x, Pos_Val[CH].y, str_NO_VIDEO_Blk, CH);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSG_Display_State(void)
{
	OSG_Display_Freeze();		// Freeze ǥ�� 
	
	OSG_Display_AUTO();    		// Auto ǥ��

	OSG_Display_Video_Loss();	//Video Loss ǥ��
}


//------------------------------------------------------------------------------------------
// ���� LIVE�� �ð�(RTC���� �о�� BCD������)�� buf�� ��ġ�� �°� �־� ���´�.
//------------------------------------------------------------------------------------------
void TimeCon(u8 *buf)
{
    buf[0] = ((rtc_hour>>4)+0x30);
    buf[1] = ((rtc_hour&0x0f)+0x30);
    buf[2] = ':';
    buf[3] = ((rtc_min>>4)+0x30);
    buf[4] = ((rtc_min&0x0f)+0x30);
    buf[5] = ':';
    buf[6] = ((rtc_sec>>4)+0x30);
	buf[7] = ((rtc_sec&0x0f)+0x30);
}


//------------------------------------------------------------------------------------------
// ���� LIVE�� ��¥(RTC���� �о�� BCD������)�� buf�� ��ġ�� �°� �־� ���´�.
//------------------------------------------------------------------------------------------
#define ASIA	0
#define USA		1
#define EURO	2
void DateCon(u8 *buf)
{
   	if(sys_env.vDATE_FORMAT == USA)
	{                         
      	buf[0] = ((rtc_month>>4)+0x30);
       	buf[1] = ((rtc_month&0x0f)+0x30);
      	buf[2] = '-';
      	buf[3] = ((rtc_day>>4)+0x30);
       	buf[4] = ((rtc_day&0x0f)+0x30);
       	buf[5] = '-';
       	buf[6] = '2';
      	buf[7] = '0'; 
      	buf[8] = ((rtc_year>>4)+0x30);
      	buf[9] = ((rtc_year&0x0f)+0x30);
    }
	else if(sys_env.vDATE_FORMAT == EURO)
	{        
       	buf[0] = ((rtc_day>>4)+0x30);
       	buf[1] = ((rtc_day&0x0f)+0x30);
      	buf[2] = '-';
       	buf[3] = ((rtc_month>>4)+0x30);
      	buf[4] = ((rtc_month&0x0f)+0x30);
       	buf[5] = '-';       
      	buf[6] = '2';
      	buf[7] = '0'; 
       	buf[8] = ((rtc_year>>4)+0x30);
      	buf[9] = ((rtc_year&0x0f)+0x30);
	}
	else if(sys_env.vDATE_FORMAT == ASIA)
	{        
		buf[0] = '2';
       	buf[1] = '0'; 
       	buf[2] = ((rtc_year>>4)+0x30);
       	buf[3] = ((rtc_year&0x0f)+0x30);
       	buf[4] = '-';
       	buf[5] = ((rtc_month>>4)+0x30);
      	buf[6] = ((rtc_month&0x0f)+0x30);
       	buf[7] = '-';
      	buf[8] = ((rtc_day>>4)+0x30);
      	buf[9] = ((rtc_day&0x0f)+0x30);
   	}     
} 


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSG_Display_Time(void)
{
	static u16 tblTimeDisplayLoc_X[3][3] =
	{
		0,
	};

	u16 PosX;
	u16 PosY;
	u8 str_buf[20];


	if(sec_flag)
	{
		sec_flag = 0;       

		tblTimeDisplayLoc_X[0][0] = 6;
		tblTimeDisplayLoc_X[0][1] = 9;
		tblTimeDisplayLoc_X[0][2] = 12;
		tblTimeDisplayLoc_X[1][0] = (1920/2)-120-12;
		tblTimeDisplayLoc_X[1][1] = (1920/2)-180-12;
		tblTimeDisplayLoc_X[1][2] = (1920/2)-240-12;
		tblTimeDisplayLoc_X[2][0] = 1920-246;
		tblTimeDisplayLoc_X[2][1] = 1920-369;
		tblTimeDisplayLoc_X[2][2] = 1920-492;
		
		//if(sys_env.vOSD_Position < 3) PosY = 1080-24-4;
		if(sys_env.vOSD_Position < 3 || sys_env.vOSD_Position == 6) PosY = 1080-24-4;
		else PosY = 4;
		
		PosX = tblTimeDisplayLoc_X[sys_env.vTIME_Position][0];

		DateCon(&str_buf[0]);
		TimeCon(&str_buf[12]);
		str_buf[10] = ' ';
		str_buf[11] = ' ';
		Print_OSD_Char_Time(PosX, PosY, str_buf, 20);			
	}
}

void OSG_Display_Time_NOW(void)
{
	static u16 tblTimeDisplayLoc_X[3][3] =
	{
		0,
	};

	u16 PosX;
	u16 PosY;
	u8 str_buf[20];

	tblTimeDisplayLoc_X[0][0] = 6;
	tblTimeDisplayLoc_X[0][1] = 9;
	tblTimeDisplayLoc_X[0][2] = 12;
	tblTimeDisplayLoc_X[1][0] = (1920/2)-120-12;
	tblTimeDisplayLoc_X[1][1] = (1920/2)-180-12;
	tblTimeDisplayLoc_X[1][2] = (1920/2)-240-12;
	tblTimeDisplayLoc_X[2][0] = 1920-246;
	tblTimeDisplayLoc_X[2][1] = 1920-369;
	tblTimeDisplayLoc_X[2][2] = 1920-492;

	//if(sys_env.vOSD_Position < 3) PosY = 1080-24-4;
	if(sys_env.vOSD_Position < 3 || sys_env.vOSD_Position == 6) PosY = 1080-24-4;
	else PosY = 4;

	PosX = tblTimeDisplayLoc_X[sys_env.vTIME_Position][0];

	DateCon(&str_buf[0]);
	TimeCon(&str_buf[12]);
	str_buf[10] = ' ';
	str_buf[11] = ' ';
	Print_OSD_Char_Time(PosX, PosY, str_buf, 20);			
}

void OSG_Display_Time_Erase(void)
{
	static u16 tblTimeDisplayLoc_X[3][3] =
	{
		0,
	};

	u16 PosX;
	u16 PosY;
	u8 str_buf[22];

	tblTimeDisplayLoc_X[0][0] = 6;
	tblTimeDisplayLoc_X[0][1] = 9;
	tblTimeDisplayLoc_X[0][2] = 12;
	tblTimeDisplayLoc_X[1][0] = (1920/2)-120-12;
	tblTimeDisplayLoc_X[1][1] = (1920/2)-180-12;
	tblTimeDisplayLoc_X[1][2] = (1920/2)-240-12;
	tblTimeDisplayLoc_X[2][0] = 1920-246;
	tblTimeDisplayLoc_X[2][1] = 1920-369;
	tblTimeDisplayLoc_X[2][2] = 1920-492;

	//if(sys_env.vOSD_Position < 3) PosY = 1080-24-4;
	if(sys_env.vOSD_Position < 3 || sys_env.vOSD_Position == 6) PosY = 1080-24-4;
	else PosY = 4;

	PosX = tblTimeDisplayLoc_X[sys_env.vTIME_Position][0];

	memset(str_buf, ' ', 22);
	Print_OSD_Char_Time(PosX-1, PosY, str_buf, 22);			
}




