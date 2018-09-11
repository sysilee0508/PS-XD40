#include "common.h"

#define DISPLAY_WIDTH_1920X1080			1920
#define DISPLAY_HEIGHT_1920x1080		1080
#define NUM_OF_POSITION					6
#define MARGIN_X						2
#define MARGIN_Y						4

typedef struct
{
	u16 pos_x;
	u16 pos_y;
} sPosition_t;

struct osd_location 
{
	u8 state;
	u8 length;
	sPosition_t location;
};

struct osd_location osd_ch_name_location_buf[NUM_OF_CHANNEL];
struct osd_location osd_video_lose_location_buf[NUM_OF_CHANNEL];
struct osd_location osd_freeze_autoseq_location_buf;

const sPosition_t tbl_OSD_SPLIT4_POSITION_1920x1080[NUM_OF_CHANNEL][NUM_OF_POSITION] =
{
//  TopLeft		TopCenter		TopRight    	BottomLeft		BottomCenter	BottomRight
	{{0, 0},  	{480, 0},		{960, 0},		{0, 540},		{480, 540},		{960, 540}},	//CH01
	{{960, 0}, 	{1440, 0},		{1920, 0},		{960, 540},		{1440, 540},	{1920, 540}},	//CH02
	{{0, 540},  {480, 540},		{960, 540},		{0,1080},		{480,1080},		{960,1080}},	//CH03
	{{960, 540}, {1440, 540},  	{1920, 540},	{960,1080},		{1440,1080},	{1920,1080}}	//CH04
};

BYTE bMode_change_flag = CLEAR;
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
// static Functions
//-----------------------------------------------------------------------------
//static void Print_OSD_Str(u16 PosX, u16 PosY, const u8 *FontData, u8 ch)
//{
//	OSD_SetFontGAC(SPRITE_INDEX0);
//
//	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);
//
//	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
//
//	osd_ch_name_location_buf[ch].state = 1;
//	//osd_ch_name_location_buf[ch].loc_x = PosX-1;
//	osd_ch_name_location_buf[ch].loc_x = PosX;
//	osd_ch_name_location_buf[ch].loc_y = PosY;
//	//osd_ch_name_location_buf[ch].length = strlen(FontData)+2;
//	osd_ch_name_location_buf[ch].length = strlen(FontData);
//}

//static void Print_OSD_Str2(u16 PosX, u16 PosY, const u8 *FontData)
//{
//	OSD_SetFontGAC(SPRITE_INDEX0);
//
//	MDINGAC_SetDrawXYMode(PosY, PosX, (PBYTE)FontData, strlen(FontData), 0);
//
//	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
//}

static void Print_OSD_Str_Loss(sPosition_t position, const u8 *pData, u8 ch)
{
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, strlen(pData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_video_lose_location_buf[ch].state = ON;//1	//state? 1??
	osd_video_lose_location_buf[ch].length = strlen(pData);
	osd_video_lose_location_buf[ch].location = position;
}

static void Print_OSD_Str_Freeze_Autoseq(sPosition_t position, const u8 *pData)
{
	OSD_SetFontGAC(SPRITE_INDEX0);

	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, strlen(pData), 0);

	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_freeze_autoseq_location_buf.state = ON;//1;
	osd_freeze_autoseq_location_buf.length = strlen(pData);
	osd_freeze_autoseq_location_buf.location = position;
}

static void Print_OSD_Char(sPosition_t position, u8 *pData, u16 size, u8 ch)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);

	osd_ch_name_location_buf[ch].state = ON;
	osd_ch_name_location_buf[ch].length = size;
	osd_ch_name_location_buf[ch].location = position;
}

static void Print_OSD_Char_Time(sPosition_t position, u8 *pData, u16 size)
{
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

static void OSD_Display_Time_Erase(void)
{
//	static u16 tblTimeDisplayLoc_X[3][3] = // kukuri...I think there is no reason we should make this array as static
	u16 tblTimeDisplayLoc_X[3][3] =
	{
		6,               								9,               								12,
		(DISPLAY_WIDTH_1920X1080/2)-120-CHAR_WIDTH_E,	(DISPLAY_WIDTH_1920X1080/2)-180-CHAR_WIDTH_E,	(DISPLAY_WIDTH_1920X1080/2)-240-CHAR_WIDTH_E,
		DISPLAY_WIDTH_1920X1080-246,					DISPLAY_WIDTH_1920X1080-369,        			DISPLAY_WIDTH_1920X1080-492
	};
	sPosition_t position;
//	u16 PosX;
//	u16 PosY;
	u8 str_buf[22];
	eDisplayPositon_t timePosition;

	// get title position
	Read_NvItem_TimePosition(&timePosition);

//	tblTimeDisplayLoc_X[0][0] = 6;
//	tblTimeDisplayLoc_X[0][1] = 9;
//	tblTimeDisplayLoc_X[0][2] = 12;
//	tblTimeDisplayLoc_X[1][0] = (1920/2)-120-12;
//	tblTimeDisplayLoc_X[1][1] = (1920/2)-180-12;
//	tblTimeDisplayLoc_X[1][2] = (1920/2)-240-12;
//	tblTimeDisplayLoc_X[2][0] = 1920-246;
//	tblTimeDisplayLoc_X[2][1] = 1920-369;
//	tblTimeDisplayLoc_X[2][2] = 1920-492;

	if(timePosition == DISPLAY_POSITION_LEFT_BOTTOM ||
	   timePosition == DISPLAY_POSITION_CENTER_BOTTOM ||
	   timePosition == DISPLAY_POSITION_RIGHT_BOTTOM ||
	   timePosition == DISPLAY_POSITION_CENTER_4SPILIT)
	{
		position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT - MARGIN_Y;
	}
	else
	{
		position.pos_y = MARGIN_Y;
	}

	position.pos_x = tblTimeDisplayLoc_X[timePosition][0];

	memset(str_buf, ' ', sizeof(str_buf));
	Print_OSD_Char_Time(position, str_buf, sizeof(str_buf));
}


static void OSD_Str_Loss_Erase(void)
{
	eChannel_t channel;

	for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
	{
		if(osd_video_lose_location_buf[channel].state == ON)
		{
			MDINGAC_SetDrawXYMode(osd_video_lose_location_buf[channel].location.pos_y,
					osd_video_lose_location_buf[channel].location.pos_x,
					str_Blank,
					osd_video_lose_location_buf[channel].length,
					0);
		}
	}
}

static void OSD_Str_Freeze_autoseq_Erase(void)
{
	if(osd_freeze_autoseq_location_buf.state == ON)
	{
		MDINGAC_SetDrawXYMode(osd_freeze_autoseq_location_buf.location.pos_y,
				osd_freeze_autoseq_location_buf.location.pos_x,
				str_Blank,
				osd_freeze_autoseq_location_buf.length,
				0);
	}
}

static sPosition_t OSD_TitleStringPosition(eChannel_t channel, eDisplayPositon_t titlePosition, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	ReadNvItem_ChannelName(channel_name, channel);
	switch(titlePosition)
	{
		case DISPLAY_POSITION_LEFT_TOP://0:
		{
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = MARGIN_X;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x + MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
				break;
			}
		}
		break;

		case DISPLAY_POSITION_CENTER_TOP://1:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = (DISPLAY_WIDTH_1920X1080 - (strlen(channel_name) * CHAR_WIDTH_K)) / 2;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E)/2;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
				break;

			}
		break;

		case DISPLAY_POSITION_RIGHT_TOP://2:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = DISPLAY_WIDTH_1920X1080 - MARGIN_X - (length * CHAR_WIDTH_K);
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) - MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
				break;

			}
		break;

		case DISPLAY_POSITION_LEFT_BOTTOM://3:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = MARGIN_X;
					position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x + MARGIN_X;
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
		break;

		case DISPLAY_POSITION_CENTER_BOTTOM: //4
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = (DISPLAY_WIDTH_1920X1080 - (length * CHAR_WIDTH_K)) / 2;
					position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) / 2);
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
		break;

		case DISPLAY_POSITION_RIGHT_BOTTOM://5:
			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN:
					position.pos_x = DISPLAY_WIDTH_1920X1080 - MARGIN_X - (length * CHAR_WIDTH_K);
					position.pos_y = DISPLAY_HEIGHT_1920x1080 - CHAR_HEIGHT;
				break;

				case DISPLAY_MODE_4SPLIT:
					position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - MARGIN_X - (length * CHAR_WIDTH_E);
					position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
				break;
			}
		break;

		case DISPLAY_POSITION_CENTER_4SPILIT://6:

			switch(displayMode)
			{
				case DISPLAY_MODE_FULL_SCREEN: //center-top
					position.pos_x = (DISPLAY_WIDTH_1920X1080 - (length * CHAR_WIDTH_K)) / 2;
					position.pos_y = 0;
				break;

				case DISPLAY_MODE_4SPLIT:
					if(channel == CHANNEL1 || channel == CHANNEL2) //center-bottom
					{
						position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E) / 2);
						position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y - CHAR_HEIGHT;
					}
					else if(channel == CHANNEL3 || channel == CHANNEL4) //center-top
					{
						position.pos_x = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_x - (length * CHAR_WIDTH_E)/2;
						position.pos_y = tbl_OSD_SPLIT4_POSITION_1920x1080[channel][titlePosition].pos_y;
					}
				break;
			}
		break;
	}
	return position;
}


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void Osd_Init_Erase(void)
{
	u16 i;
	BYTE pSTR[DISPLAY_WIDTH_1920X1080/CHAR_WIDTH_E];
	
	memset(pSTR, ' ', sizeof(pSTR));

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < DISPLAY_HEIGHT_1920x1080/CHAR_HEIGHT; i++)
	{
		MDINGAC_SetDrawXYMode(i*CHAR_HEIGHT, 0, pSTR, sizeof(pSTR), 0);
	}

	// Initialize channel name buffer
	memset(osd_ch_name_location_buf, 0, sizeof(osd_ch_name_location_buf));
}

void Erase_OSD(void)
{
	u8 i = 0;

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < NUM_OF_CHANNEL; i++)
	{
		if(osd_ch_name_location_buf[i].state == ON)
		{
			MDINGAC_SetDrawXYMode(osd_ch_name_location_buf[i].location.pos_y,
								osd_ch_name_location_buf[i].location.pos_x,
								str_Blank,
								osd_ch_name_location_buf[i].length,
								0);
		}
	}
	OSD_Str_Loss_Erase();
	if(!bAuto_Seq_Flag)
	{
		OSD_Str_Freeze_autoseq_Erase();
	}
	if(bSETUP)
	{
		OSD_Display_Time_Erase();
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void OSD_Display(void)
{
	BOOL osdDisplayOn;
	BOOL titleDisplayOn;
	BOOL timeDisplayOn;

	Read_NvItem_OsdOn(&osdDisplayOn);
	Read_NvItem_TitleDispalyOn(&titleDisplayOn);
	Read_NvItem_TimeDisplayOn(&timeDisplayOn);

	if(osdDisplayOn == ON)
	{
		if(bMode_change_flag && (!bSETUP))
		{
			if(titleDisplayOn == ON)
				OSD_Display_CH_name();
			if(timeDisplayOn == ON)
				OSD_Display_Time_NOW();
		}
		if(!bSETUP)
			OSD_Display_State();

		if((timeDisplayOn == ON) && (!bSETUP))
			OSD_Display_Time();
	}
	bMode_change_flag = CLEAR;
}

void OSD_Display_CH_name(void)
{
	eChannel_t channel;
	sPosition_t positionValue;
	eDisplayPositon_t osdPosition;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX] = {0,};

	Read_NvItem_TitlePosition(&osdPosition);
	if(sys_status.current_split_mode <= SPLITMODE_FULL_CH4)
	{
		channel = (eChannel_t)sys_status.current_split_mode;
		Read_NvItem_ChannelName(channel_name, channel);
		positionValue =  OSD_TitleStringPosition(channel, osdPosition, DISPLAY_MODE_FULL_SCREEN, strlen(channel_name));
		Print_OSD_Char(positionValue.pos_x, positionValue.pos_y, channel_name, strlen(channel_name), channel);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4)
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			Read_NvItem_ChannelName(channel_name, channel);
			positionValue =  OSD_TitleStringPosition(channel, osdPosition, DISPLAY_MODE_4SPLIT, strlen(channel_name));
			Print_OSD_Char(positionValue.pos_x, positionValue.pos_y, channel_name, strlen(channel_name), channel);
		}
	}
}

//-----------------------------------------------------------------------------
void OSD_Display_Freeze(void)
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
void OSD_Display_AUTO(void)
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
void OSD_Display_Video_Loss(void)
{
	sPosition_t Pos_Val[20];
	u8 CH = 0;

	u8 vMAX_Ch; 
	u8 vStart_CH; 
	u8 vMODE;
	
	if(sys_env.vLoss_Display & (Loss_Event_Flag || bMode_change_flag))
	{

		if(sys_status.current_split_mode <= SPLITMODE_FULL_CH9)
		{
			vMODE = DISPLAY_MODE_FULL_SCREEN;
			vMAX_Ch = 1;
			vStart_CH = sys_status.current_split_mode;
		}
		else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_1)
		{
			vMODE = DISPLAY_MODE_4SPLIT;
			vMAX_Ch = 4;
			vStart_CH = 0;
		}

		Loss_Event_Flag = 0;
	
		if(vMODE == DISPLAY_MODE_FULL_SCREEN)
		{
			Pos_Val[vStart_CH].x = (1920/2)-(12*5);
			Pos_Val[vStart_CH].y = (1080/2)-12;
		}
		else if(vMODE == DISPLAY_MODE_4SPLIT)
		{
			for(CH=vStart_CH;CH<(vStart_CH+vMAX_Ch);CH++)
			{
				Pos_Val[CH].x = tbl_OSD_SPLIT4_POSITION_1920x1080[((CH-vStart_CH)*12)+2]-(12*5);
				Pos_Val[CH].y = tbl_OSD_SPLIT4_POSITION_1920x1080[((CH-vStart_CH)*12)+3]+270-12;
			}
		}
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
void OSD_Display_State(void)
{
	OSD_Display_Freeze();		// Freeze ǥ�� 
	
	OSD_Display_AUTO();    		// Auto ǥ��

	OSD_Display_Video_Loss();	//Video Loss ǥ��
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
void OSD_Display_Time(void)
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

void OSD_Display_Time_NOW(void)
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
