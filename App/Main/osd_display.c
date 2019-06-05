#include "common.h"
#include "osd_string.h"
#include "NVP6158.h"

#define MARGIN_X						5
#define MARGIN_Y						5

#define DATE_TIME_LENGTH				21 // "yyyy-mmm-dd hh:mm:ss "
#define DATE_LENGTH_4DIGIT				11 // "yyyy-mmm-dd"
#define DATE_LENGTH_2DIGIT				9 // "yy-mmm-dd"
#define TIME_LENGTH						8 // "hh:mm:ss"

//#define TITLE_LENGTH					5
#define VIDEO_FORMAT_LENGTH_MAX		20
#define VIDEO_LOSS_LENGTH				8

#define VIDEO_MODE_DISPLAY_TIME			10//seconds
static u8 displayingDateTimeLength = 0;

static const sPosition_t titlePositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
{
	//QUAD_A
	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}},
	//Quad_B
	{{DISPLAY_WIDTH/3, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), (DISPLAY_HEIGHT/3)*2}},
	//quad_c
	{{(DISPLAY_WIDTH/3)*2, 0}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, (DISPLAY_HEIGHT/3)*2}},
	//quad_d
	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3*2}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3*2}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3*2}},
	//quad_e
	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}},
	//3split_a
	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
	//3split_b
	{{DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
	//3split_c
	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {0, 0}},
	//3split_d
	{{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH,0}, {0, 0}},
	//split
	{{DISPLAY_QUAD_WIDTH,0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {0, 0}, {0, 0}}
};

static const sPosition_t indicatorPositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
{
	//QUAD_A
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X , DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}},
	//Quad_B
	{{(DISPLAY_WIDTH/3)*2-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
			{(DISPLAY_WIDTH/3)*2+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
	//quad_c
	{{(DISPLAY_WIDTH/3)+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
	//quad_d
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)*2-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)*2+MARGIN_Y+CHAR_HEIGHT}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
	//quad_e
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HEIGHT/3+MARGIN_Y+CHAR_HEIGHT}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y}},
	//3split_a
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {0, 0}},
	//3split_b
	{{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {0, 0}},
	//3split_c
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {0, 0}},
	//3split_d
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT+MARGIN_Y+CHAR_HEIGHT}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {0, 0}},
	//split
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {0, 0}, {0, 0}}
};

static const sPosition_t videoPositionTable_Split[DISPLAY_MODE_MAX][NUM_OF_CHANNEL] =
{
	//QUAD_A
	{{DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT},
			{DISPLAY_QUAD_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT}},
	//Quad_B
	{{DISPLAY_WIDTH/3, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/6},
			{DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT - (DISPLAY_HEIGHT/6)}},
	//quad_c
	{{(DISPLAY_WIDTH/3)*2, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/6},
			{DISPLAY_WIDTH/6, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT - (DISPLAY_HEIGHT/6)}},
	//quad_d
	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT-DISPLAY_HEIGHT/6},
			{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT-DISPLAY_HEIGHT/6}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT-DISPLAY_HEIGHT/6}},
	//quad_e
	{{DISPLAY_HALF_WIDTH, (DISPLAY_HEIGHT/3)*2}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/6},
			{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/6}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/6}},
	//3split_a
	{{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT},
			{DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT-DISPLAY_QUAD_HEIGHT}, {0, 0}},
	//3split_b
	{{DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT},
			{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT-DISPLAY_QUAD_HEIGHT}, {0, 0}},
	//3split_c
	{{DISPLAY_HALF_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT+DISPLAY_QUAD_HEIGHT},
			{DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT+DISPLAY_QUAD_HEIGHT}, {0, 0}},
	//3split_d
	{{DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT+DISPLAY_QUAD_HEIGHT}, {DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT},
			{DISPLAY_HALF_WIDTH+DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT}, {0, 0}},
	//split
	{{DISPLAY_QUAD_WIDTH,DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},
			{0, 0}, {0, 0}}
};


static const sPosition_t videoInFormatPosition_Full =
{
	(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, //x
	DISPLAY_HALF_HEIGHT - CHAR_HEIGHT - MARGIN_Y	//y
};
static const sPosition_t videoOutFormatPosition_Full =
{
	(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, //x
	DISPLAY_HALF_HEIGHT + MARGIN_Y	//y
};
static const sPosition_t videoLossPosition_Full =
{
	(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2,
	(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2
};

static const sPosition_t videoInFormatPosition_Split[NUM_OF_SPLIT][NUM_OF_CHANNEL] =
{
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT - MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}},
};

static const sPosition_t videoOutFormatPosition_Split[NUM_OF_SPLIT][NUM_OF_CHANNEL] =
{
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT+MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2, DISPLAY_HALF_HEIGHT+MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT+MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_QUAD_HEIGHT+MARGIN_Y},
			{(DISPLAY_WIDTH - (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT+MARGIN_Y}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH))/2,DISPLAY_HALF_HEIGHT+MARGIN_Y},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2,DISPLAY_HALF_HEIGHT+MARGIN_Y}},
};

static const sPosition_t videoLossPosition_Split[NUM_OF_SPLIT+NUM_OF_PIP][NUM_OF_CHANNEL] =
{
	// SPLIT -------------------------------------------------------------------------------------------------------------//
	{{(DISPLAY_HALF_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HALF_HEIGHT - CHAR_HEIGHT) / 2},
			{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HALF_HEIGHT - CHAR_HEIGHT) / 2},
			{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, DISPLAY_HEIGHT - DISPLAY_QUAD_HEIGHT - CHAR_HEIGHT/2}},
	{{(DISPLAY_HALF_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2,(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2}},
	// PIP --------------------------------------------------------------------------------------------------------------//
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-PIP_POSITION_MARGIN-PIP_WINDOW_WIDTH/2-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2, PIP_POSITION_MARGIN+(PIP_WINDOW_HEIGHT-CHAR_HEIGHT)/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{PIP_POSITION_MARGIN+(PIP_WINDOW_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT/2-CHAR_HEIGHT/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{DISPLAY_WIDTH-PIP_POSITION_MARGIN-PIP_WINDOW_WIDTH/2-(VIDEO_LOSS_LENGTH*CHAR_WIDTH)/2, DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT/2-CHAR_HEIGHT/2}},
	{{(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2, (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2},
			{PIP_POSITION_MARGIN+(PIP_WINDOW_WIDTH-(VIDEO_LOSS_LENGTH*CHAR_WIDTH))/2, PIP_POSITION_MARGIN+(PIP_WINDOW_HEIGHT-CHAR_HEIGHT)/2}},
};


static BOOL requestRefreshScreen = CLEAR;

static u8 videoModeDisplayCount[NUM_OF_CHANNEL] = 
{
	VIDEO_MODE_DISPLAY_TIME*2,
	VIDEO_MODE_DISPLAY_TIME*2, 
	VIDEO_MODE_DISPLAY_TIME*2, 
	VIDEO_MODE_DISPLAY_TIME*2
};

static sPosition_t OSD_TitleStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH)) / 2;
			position.pos_y = MARGIN_Y;
			break;

		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
			position.pos_x = DISPLAY_WIDTH-(PIP_POSITION_MARGIN+PIP_WINDOW_WIDTH) + ((PIP_WINDOW_WIDTH-(length*CHAR_WIDTH))/2);
			position.pos_y = MARGIN_Y+PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
			position.pos_x = PIP_POSITION_MARGIN + ((PIP_WINDOW_WIDTH-(length*CHAR_WIDTH))/2);
			position.pos_y = DISPLAY_HEIGHT -(PIP_WINDOW_HEIGHT + PIP_POSITION_MARGIN) + MARGIN_Y;
			break;

		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
			position.pos_x = DISPLAY_WIDTH-(PIP_POSITION_MARGIN+PIP_WINDOW_WIDTH) + ((PIP_WINDOW_WIDTH-(length*CHAR_WIDTH))/2);
			position.pos_y = DISPLAY_HEIGHT -(PIP_WINDOW_HEIGHT + PIP_POSITION_MARGIN) + MARGIN_Y;
			break;

		case DISPLAY_MODE_PIP_D2:
		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			position.pos_x = PIP_POSITION_MARGIN + ((PIP_WINDOW_WIDTH-(length*CHAR_WIDTH))/2);
			position.pos_y = MARGIN_Y+PIP_POSITION_MARGIN;
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
			if((channel == CHANNEL1) || (channel == CHANNEL3))
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - (length * CHAR_WIDTH)) / 2;
			}
			else //ch2 or ch4
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - (length * CHAR_WIDTH)) / 2;
			}
			position.pos_y = MARGIN_Y;
			break;
			
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			if((channel == CHANNEL1) || (channel == CHANNEL3))
			{
				position.pos_y = MARGIN_Y;
			}
			else //ch2 or ch4
			{
				position.pos_y = MARGIN_Y + DISPLAY_HALF_HEIGHT;
			}
			position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH)) / 2;
			break;

			//DISPLAY_MODE_4SPLIT_QUAD,
			//DISPLAY_MODE_4SPLIT_R3SCALE,
			//DISPLAY_MODE_4SPLIT_R3CROP,
			//DISPLAY_MODE_4SPLIT_L3SCALE,
			//DISPLAY_MODE_4SPLIT_L3CROP,
			//DISPLAY_MODE_4SPLIT_D3SCALE,
			//DISPLAY_MODE_4SPLIT_D3CROP,
			//DISPLAY_MODE_4SPLIT_U3SCALE,
			//DISPLAY_MODE_4SPLIT_U3CROP,
			//DISPLAY_MODE_4SPLIT_H,
			//DISPLAY_MODE_4SPLIT_X,
			
		default:
			break;
	}

	return position;
}

static sPosition_t OSD_IndicatorStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
			position.pos_y = DISPLAY_HEIGHT - 2*CHAR_HEIGHT - MARGIN_Y;
			break;

	/*
		case DISPLAY_MODE_SPLIT:
			position.pos_x = indicatorPositionTable_Split[splitMode][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[splitMode][channel].pos_y;
			break;
	*/
	}
	return position;
}

static sPosition_t OSD_VideoModeStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 length)
{
	sPosition_t position;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
			position.pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;
			break;

	/*
		case DISPLAY_MODE_SPLIT:
			position.pos_x = videoPositionTable_Split[splitMode][channel].pos_x - ((length * CHAR_WIDTH)/2);
			position.pos_y = videoPositionTable_Split[splitMode][channel].pos_y - CHAR_HEIGHT/2;
			break;
	*/
	}
	return position;

}

static u8 Get_NumOfDisplayChannels(eDisplayMode_t displayMode)
{
	u8 channels = NUM_OF_CHANNEL;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:		// full screen - channel 1
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			channels = 1;
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_A:	// ch 1,2
		case DISPLAY_MODE_2SPLIT_VCROP_A:		// ch 1,2
		case DISPLAY_MODE_2SPLIT_HSCALE_B:	
		case DISPLAY_MODE_2SPLIT_HCROP_B:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
		case DISPLAY_MODE_PIP_D2:
		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			channels = 2;
			break;

		case DISPLAY_MODE_4SPLIT_QUAD:
		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
		case DISPLAY_MODE_4SPLIT_H:
		case DISPLAY_MODE_4SPLIT_X:
			channels = 4;
			break;
	}
	return channels;
}

static u8 CreateDateString(u8 *pDateStr)
{
  static sTimeDate_t rtcDate;
	eDateFormat_t dateFormat;
	BOOL year4digit;
	u8 year[5];
	u8 month[3];
	u8 day[2];
	u8 dateLength;

	RTC_GetTime(&rtcDate);
	Read_NvItem_DateFormat(&dateFormat);
	Read_NvItem_YearFormat(&year4digit);

	memset(year, NULL, sizeof(year));
	if(year4digit == FALSE) //2digit
	{
		year[0] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
		year[1] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
		dateLength = DATE_LENGTH_2DIGIT;
	}
	else //4 digit
	{
		year[0] = 2 + ASCII_ZERO;
		year[1] = ASCII_ZERO;
		year[2] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
		year[3] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
		dateLength = DATE_LENGTH_4DIGIT;
	}
	strncpy(month, pStrMonthName[rtcDate.month - 1], sizeof(month));
	day[0] = (rtcDate.day / 10) + ASCII_ZERO;
	day[1] = (rtcDate.day % 10) + ASCII_ZERO;


	switch(dateFormat)
	{
		case DATE_FORMAT_YMD: //ASIA
			strncpy(pDateStr, year, strlen(year));
			pDateStr += strlen(year);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, month, sizeof(month));
			pDateStr += sizeof(month);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, day, sizeof(day));
			break;

		case DATE_FORMAT_MDY: //US
			strncpy(pDateStr, month, sizeof(month));
			pDateStr += sizeof(month);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, day, sizeof(day));
			pDateStr += sizeof(day);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, year, strlen(year));
			break;

		case DATE_FORMAT_DMY: //EURO
			strncpy(pDateStr, day, sizeof(day));
			pDateStr += sizeof(day);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, month, sizeof(month));
			pDateStr += sizeof(month);
			strncpy(pDateStr, "-", 1);
			pDateStr++;
			strncpy(pDateStr, year, strlen(year));
			break;
	}
	return dateLength;
}

static u8 CreateTimeString(u8 *pTimeStr)
{
	sTimeDate_t rtcTime;

	RTC_GetTime(&rtcTime);
    *pTimeStr = ((rtcTime.hour / 10)+ ASCII_ZERO);
    *(++pTimeStr) = ((rtcTime.hour % 10)+ ASCII_ZERO);
    *(++pTimeStr) = ':';
    *(++pTimeStr) = ((rtcTime.min / 10)+ ASCII_ZERO);
    *(++pTimeStr) = ((rtcTime.min % 10)+ ASCII_ZERO);
    *(++pTimeStr) = ':';
    *(++pTimeStr) = ((rtcTime.sec / 10)+ ASCII_ZERO);
    *(++pTimeStr) = ((rtcTime.sec % 10)+ ASCII_ZERO);

	return TIME_LENGTH;
}

static u8* GetOutVideoFormatString(void)
{
	u8* outStr;

	switch(GetOutVideoFormat(MDIN_ID_C))
	{
		case VIDOUT_1920x1080p60:
			outStr = (u8 *)osdStr_Format_Out_1080p60;
			break;

		case VIDOUT_1920x1080p30:
			outStr = (u8 *)osdStr_Format_Out_1080p30;
			break;

		case VIDOUT_1920x1080p50:
			outStr = (u8 *)osdStr_Format_Out_1080p50;
			break;

		case VIDOUT_1920x1080p25:
			outStr = (u8 *)osdStr_Format_Out_1080p25;
			break;
		default:
			outStr = (u8 *)osdStr_Space20;
			break;

	}
	
	return outStr;
}

static u8* GetInVideoFormatString(eChannel_t channel)
{
	u8* inStr;

	switch(GetInputVideoFormat(channel))
	{
		// 1080p
		case AHD20_1080P_60P:
		case AHD20_1080P_30P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p30;
			break;
		case AHD20_1080P_50P:
		case AHD20_1080P_25P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p25;
			break;
		case TVI_FHD_30P:
			inStr = (u8 *)osdStr_Format_In_TVI_1080p30;
			break;
		case TVI_FHD_25P:
			inStr = (u8 *)osdStr_Format_In_TVI_1080p25;
			break;
		case CVI_FHD_30P:
			inStr = (u8 *)osdStr_Format_In_CVI_1080p30;
			break;
		case CVI_FHD_25P:
			inStr = (u8 *)osdStr_Format_In_CVI_1080p25;
			break;
		// 720p
		case AHD20_720P_60P:
		case AHD20_720P_30P:
		case AHD20_720P_30P_EX:
		case AHD20_720P_30P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p30;
			break;
		case AHD20_720P_50P:
		case AHD20_720P_25P:
		case AHD20_720P_25P_EX:
		case AHD20_720P_25P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p25;
			break;
		case TVI_HD_60P:
		case TVI_HD_30P:
		case TVI_HD_30P_EX:
		case TVI_HD_B_30P:
		case TVI_HD_B_30P_EX:
			inStr = (u8 *)osdStr_Format_In_TVI_720p30;
			break;
		case TVI_HD_50P:
		case TVI_HD_25P:
		case TVI_HD_25P_EX:
		case TVI_HD_B_25P:
		case TVI_HD_B_25P_EX:
			inStr = (u8 *)osdStr_Format_In_TVI_720p25;
			break;
		case CVI_HD_60P:
		case CVI_HD_30P:
		case CVI_HD_30P_EX:
			inStr = (u8 *)osdStr_Format_In_CVI_720p30;
			break;
		case CVI_HD_50P:
		case CVI_HD_25P:
		case CVI_HD_25P_EX:
			inStr = (u8 *)osdStr_Format_In_CVI_720p25;
			break;

		case AHD20_SD_H960_NT:
		//case AHD20_SD_SH720_NT:
		//case AHD20_SD_H1280_NT:
		//case AHD20_SD_H1440_NT:
		case AHD20_SD_H960_EX_NT:
		case AHD20_SD_H960_2EX_NT:
		case AHD20_SD_H960_2EX_Btype_NT:
			inStr = (u8 *)osdStr_Format_In_CVBS_NTSC;
			break;

		case AHD20_SD_H960_PAL:
		//case AHD20_SD_SH720_PAL:
		//case AHD20_SD_H1280_PAL:
		//case AHD20_SD_H1440_PAL:
		case AHD20_SD_H960_EX_PAL:
		case AHD20_SD_H960_2EX_PAL:
		case AHD20_SD_H960_2EX_Btype_PAL:
			inStr = (u8 *)osdStr_Format_In_CVBS_PAL;
			break;

		default:
			inStr = (u8 *)osdStr_Space20;
			break;
	}
	return inStr;
}

static sPosition_t OSD_GetAutoPosition(u8 strLength)
{
	sPosition_t position;
	BOOL timeOn;
	BOOL dateOn;
	BOOL year4digit;
	u8 dateTimeLength = 0;

	Read_NvItem_YearFormat(&year4digit);
	Read_NvItem_DateDisplayOn(&dateOn);
	Read_NvItem_TimeDisplayOn(&timeOn);

	if((dateOn == ON) && (year4digit == TRUE))
	{
		dateTimeLength = DATE_LENGTH_4DIGIT + 2;
	}
	else if((dateOn == ON) && (year4digit == FALSE))
	{
		dateTimeLength = DATE_LENGTH_2DIGIT + 2;
	}

	if(timeOn == ON)
	{
		dateTimeLength += TIME_LENGTH;
	}

	// x
	if(dateTimeLength > 0)
	{
		position.pos_x = (DISPLAY_WIDTH + (MIN(DATE_TIME_LENGTH, dateTimeLength) * CHAR_WIDTH)) / 2 ;
	}
	else
	{
		position.pos_x = (DISPLAY_WIDTH - (strLength * CHAR_WIDTH)) / 2;
	}
	// y
	position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;

	return position;
}

//-----------------------------------------------------------------------------
// Erase
//-----------------------------------------------------------------------------
static void OSD_EraseChannelName(void)
{
	eChannel_t channel;
	sPosition_t position;
	eDisplayMode_t displayMode;
	eChannel_t max_channel = NUM_OF_CHANNEL;
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX+1] = {0,};

	displayMode = GetCurrentDisplayMode();

	if(IS_FULL_MODE(displayMode))
	{
		channel = ConvertDisplayMode2Channel(displayMode);
		Read_NvItem_ChannelName(channel_name, channel);
		position =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
		OSD_PrintString(position, osdStr_Space12, strlen(channel_name));
	}
	/*
	else
	{
		max_channel = Get_NumOfDisplayChannels(displayMode);
		for(channel = CHANNEL1; channel < max_channel; channel++)
		{
			Read_NvItem_ChannelName(channel_name, channel);
			position =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
			OSD_PrintString(position, osdStr_Space12, strlen(channel_name));
		}
	}
	*/
}

static void OSD_EraseTimeDate(void)
{
	sPosition_t position;
	u8 strSpaces[DATE_TIME_LENGTH];

	position.pos_x = (DISPLAY_WIDTH - (DATE_TIME_LENGTH*CHAR_WIDTH))/2 - 12;
	position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;

	memset(strSpaces, ASCII_SPACE, sizeof(strSpaces));
	OSD_PrintString(position, strSpaces, displayingDateTimeLength);
	displayingDateTimeLength = 0;
}

static void OSD_EraseIndicator(void)
{
	eChannel_t channel;
	sPosition_t position;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();

	
	if(IS_FULL_MODE(displayMode))
	{
		channel = ConvertDisplayMode2Channel(displayMode);
		position = OSD_IndicatorStringPosition(channel, displayMode, strlen(osdStr_Space10));
		OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
	}
/*
	else
	{
		for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(); channel++)
		{
			position = OSD_IndicatorStringPosition(channel, displayMode, strlen(osdStr_Space1));
			OSD_PrintString(position, osdStr_Space1, strlen(osdStr_Space1));
		}
	}
*/
}

static void OSD_EraseAuto(void)
{
	sPosition_t position;
	BOOL timeOn;

	Read_NvItem_TimeDisplayOn(&timeOn);
	position = OSD_GetAutoPosition(strlen(osdStr_Space4));

	if(timeOn == ON)
	{
		position.pos_x += (2 * CHAR_WIDTH);
	}
	OSD_PrintString(position, osdStr_Space6, strlen(osdStr_Space4));
}

static void OSD_EraseNoVideo(void)
{
	sPosition_t position;

	if(IS_FULL_MODE(GetCurrentDisplayMode()))
	{
		position.pos_x = (DISPLAY_WIDTH - (strlen(osdStr_Space10)*CHAR_WIDTH))/2;
		position.pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;
		OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
	}
}

//-----------------------------------------------------------------------------
// No Video
//-----------------------------------------------------------------------------
static void OSD_DisplayNoVideo(void)
{
	sPosition_t position;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	BOOL videoLossDiplayOn;

	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);

	if((GetVideoLossEvent() == SET) || (requestRefreshScreen == SET))
	{
		SetVideoLossEvent(CLEAR);

		if(IS_FULL_MODE(displayMode))
		{
			position.pos_x = (DISPLAY_WIDTH - (strlen(osdStr_NoVideoFull)*CHAR_WIDTH))/2;
			position.pos_y = (DISPLAY_HEIGHT - CHAR_HEIGHT)/2;

			if(IsVideoLossChannel(ConvertDisplayMode2Channel(displayMode)) == TRUE)
			{
				if(videoLossDiplayOn == ON)
				{
					OSD_PrintString(position, osdStr_NoVideoFull, strlen(osdStr_NoVideoFull));
				}
			}
			else
			{
				//OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
			}
		}
	}
}


void OSD_DisplayVideoMode(void)
{
	eChannel_t channel;
	sPosition_t inPosition, outPosition;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	u8* pInVideoStr = NULL;
	u8* pOutVideoStr = NULL;
	sSystemTick_t* currentSystemTime = GetSystemTime();
	static u32 previousSystemTimeIn1s = 0;
	static eDisplayMode_t preDisplayMode = DISPLAY_MODE_MAX; 
	static u8* preInVideo = NULL;
	static u8* preOutVideo = NULL;

	if((preDisplayMode != displayMode) && (preDisplayMode != DISPLAY_MODE_MAX))
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
		}
		preInVideo = NULL;
	}
	preDisplayMode = displayMode;

	if(IS_FULL_MODE(displayMode))
	{
		channel = ConvertDisplayMode2Channel(displayMode);

		if(IsVideoLossChannel(channel) != TRUE)
		{
			if(videoModeDisplayCount[channel] > 0)
			{
				pInVideoStr = GetInVideoFormatString(channel);
				pOutVideoStr = GetOutVideoFormatString();
			}
			else
			{
				pInVideoStr = (u8*)osdStr_Space20;
				pOutVideoStr = (u8*)osdStr_Space20;
			}
			
			if(preInVideo != pInVideoStr)
			{
				OSD_EraseNoVideo();
				OSD_PrintString(videoInFormatPosition_Full, pInVideoStr, strlen(pInVideoStr));
				OSD_PrintString(videoOutFormatPosition_Full, pOutVideoStr, strlen((const u8*)pOutVideoStr));
			}

			if((TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn1s,1)) && (videoModeDisplayCount[channel] > 0))
			{
				//videoModeDisplayCount[channel]--;
			}
		}
		else
		{
			videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
			
			pInVideoStr = (u8*)osdStr_Space20;
			pOutVideoStr = (u8*)osdStr_Space20;

			if((preInVideo != pInVideoStr) || (preOutVideo != pOutVideoStr))
			{
				OSD_PrintString(videoInFormatPosition_Full, pInVideoStr, strlen(pInVideoStr));
				OSD_PrintString(videoOutFormatPosition_Full, pOutVideoStr, strlen((const u8*)pOutVideoStr));
				
				OSD_DisplayNoVideo();
			}
		}
		preInVideo = pInVideoStr;
		preOutVideo = pOutVideoStr;

	}
/*
	else
	{
		for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(); channel++)
		{
			if(IsVideoLossChannel(channel) != TRUE)
			{
				if(videoModeDisplayCount[channel] > 0)
				{
					pInVideoStr = GetInVideoFormatString(channel);
				}
				else
				{
					pInVideoStr = (u8*)osdStr_Space10;
				}
				position =  OSD_VideoModeStringPosition(channel, displayMode, strlen(pInVideoStr));
				OSD_PrintString(position, pInVideoStr, strlen(pInVideoStr));
				
				if((TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn1s,1)) && (videoModeDisplayCount[channel] > 0))
				{
					videoModeDisplayCount[channel]--;
				}
			}
			else
			{
				videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
			}
		}
	}
	previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
*/
}

//-----------------------------------------------------------------------------
// auto
//-----------------------------------------------------------------------------
static void OSD_DisplayAuto(void)
{
	BOOL autoOn;
	sPosition_t position;
	BOOL timeOn;
	static BOOL previousAutoSeqOn = CLEAR;
	u8* pStr;

	autoOn = (GetCurrentAutoSeq() == AUTO_SEQ_NORMAL)?SET:CLEAR;

	if((previousAutoSeqOn !=  autoOn) || (requestRefreshScreen == SET))
	{
		previousAutoSeqOn = autoOn;

		position = OSD_GetAutoPosition(sizeof(osdStr_AUTO));
		Read_NvItem_TimeDisplayOn(&timeOn);
		if(timeOn == ON)
		{
			position.pos_x += (2 * CHAR_WIDTH);
		}

		if(autoOn == SET)
		{
			pStr = (u8 *)osdStr_AUTO;
		}
		else
		{
			pStr = (u8 *)osdStr_Space4;
		}
		OSD_PrintString(position, pStr, strlen(pStr));
	}
}

//-----------------------------------------------------------------------------
// Indicator - Freeze, Alarm, Motion
//-----------------------------------------------------------------------------
static void OSD_DisplayIndicator(void)
{
	sPosition_t position;
	eChannel_t channel;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	BOOL videoLossDiplayOn;
	u8* pIndicator;

	Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
	if(IS_FULL_MODE(displayMode))
	{
		channel = ConvertDisplayMode2Channel(displayMode);
		if(GetAlarmStatus(channel) == ALARM_SET)
		{
			pIndicator = (u8*)osdStr_AlarmFull;
		}
		else if(Get_MotionDetectedStatus(channel))//motion
		{
			pIndicator = (u8*)osdStr_MotionFull;
		}
		else if(IsScreenFreeze())//freeze
		{
			pIndicator = (u8*)osdStr_FreezeFull;
		}
		else
		{
			pIndicator = (u8*)osdStr_Space6;
		}
		position = OSD_IndicatorStringPosition(channel, displayMode, strlen((const u8*)pIndicator));
		OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
	}
/*
	else//Split
	{
		for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(); channel++)
		{
			if((IsVideoLossChannel(channel) == TRUE) && (videoLossDiplayOn == ON))
			{
				pIndicator = (u8*)osdStr_NoVideo;
			}
			else if(GetAlarmStatus(channel) == ALARM_SET)
			{
				pIndicator = (u8*)osdStr_Alarm;
			}
			else if(Get_MotionDetectedStatus(channel))
			{
				pIndicator = (u8*)osdStr_Motion;
			}
			else if(IsScreenFreeze())
			{
				pIndicator = (u8*)osdStr_Freeze;
			}
			else
			{
				pIndicator = (u8*)osdStr_Space1;
			}
			position = OSD_IndicatorStringPosition(channel, DISPLAY_MODE_SPLIT, strlen((const u8*)pIndicator));
			OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
		}
	}
*/
}

//-----------------------------------------------------------------------------
// Date & Time
//-----------------------------------------------------------------------------
static void OSD_DisplayDateTime(void)
{
	BOOL timeDisplayOn;
	BOOL dateDisplayOn;
	sPosition_t position;
	u8 dateTimeString[DATE_TIME_LENGTH+1];
	u8* pStr;

	Read_NvItem_DateDisplayOn(&dateDisplayOn);
	Read_NvItem_TimeDisplayOn(&timeDisplayOn);

	pStr = &dateTimeString[0];
	memset(pStr, NULL, sizeof(dateTimeString));

	if(RTC_GetDisplayTimeStatus() == SET)
	{
		RTC_ChangeDisplayTimeStatus(CLEAR);

		if((dateDisplayOn == ON) && (timeDisplayOn == ON))
		{
			pStr += CreateDateString(pStr);
			strcpy(pStr, osdStr_Space2);
			pStr += strlen(osdStr_Space2);
			CreateTimeString(pStr);
		}
		else if((dateDisplayOn == ON) && (timeDisplayOn == OFF))
		{
			CreateDateString(pStr);
		}
		else if((dateDisplayOn == OFF) && (timeDisplayOn == ON))
		{
			CreateTimeString(pStr);
		}
		displayingDateTimeLength = strlen(dateTimeString);

		if(displayingDateTimeLength > 0)
		{
			position.pos_x = (DISPLAY_WIDTH - (strlen(dateTimeString)*CHAR_WIDTH))/2 - 12;
			position.pos_y = DISPLAY_HEIGHT - CHAR_HEIGHT - MARGIN_Y;
			OSD_PrintString(position, dateTimeString, displayingDateTimeLength);
		}
	}
}


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void OSD_RefreshScreen(void)
{
	requestRefreshScreen = SET;
}

void OSD_PrintString(sPosition_t position, const u8 *pData, u16 size)
{
	ConfigI2C(MDIN_ID_C);
	
	OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

void OSD_DisplayChannelName(void)
{
	eChannel_t channel, max_channel = NUM_OF_CHANNEL;
	sPosition_t positionValue;
	BOOL titleDisplayOn;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX+1] = {0,};

	Read_NvItem_TitleDispalyOn(&titleDisplayOn);

	if(titleDisplayOn == ON)
	{
		switch(displayMode)
		{
			case DISPLAY_MODE_FULL_CH1:
			case DISPLAY_MODE_FULL_CH2:
			case DISPLAY_MODE_FULL_CH3:
			case DISPLAY_MODE_FULL_CH4:
				channel = ConvertDisplayMode2Channel(displayMode);
				Read_NvItem_ChannelName(channel_name, channel);
				positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				break;

			case DISPLAY_MODE_PIP_A2:
			case DISPLAY_MODE_PIP_A3:
			case DISPLAY_MODE_PIP_A4:
			case DISPLAY_MODE_PIP_B2:
			case DISPLAY_MODE_PIP_B3:
			case DISPLAY_MODE_PIP_B4:
			case DISPLAY_MODE_PIP_C2:
			case DISPLAY_MODE_PIP_C3:
			case DISPLAY_MODE_PIP_C4:
			case DISPLAY_MODE_PIP_D2:
			case DISPLAY_MODE_PIP_D3:
			case DISPLAY_MODE_PIP_D4:
				// channel 1
				Read_NvItem_ChannelName(channel_name, CHANNEL1);
				positionValue =  OSD_TitleStringPosition(CHANNEL1, DISPLAY_MODE_FULL_CH1, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				// clean-up
				memset(channel_name, 0x00, CHANNEL_NEME_LENGTH_MAX+1);
				positionValue.pos_x = 0;
				positionValue.pos_y = 0;
				// sub channel
				channel = FineSubChannelForPIP(displayMode);
				Read_NvItem_ChannelName(channel_name, channel);
				positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				break;

			case DISPLAY_MODE_2SPLIT_HSCALE_A:
			case DISPLAY_MODE_2SPLIT_HCROP_A:
			case DISPLAY_MODE_2SPLIT_VSCALE_A:
			case DISPLAY_MODE_2SPLIT_VCROP_A:
				// channel 1
				Read_NvItem_ChannelName(channel_name, CHANNEL1);
				positionValue =  OSD_TitleStringPosition(CHANNEL1, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				// clean-up
				memset(channel_name, 0x00, CHANNEL_NEME_LENGTH_MAX+1);
				positionValue.pos_x = 0;
				positionValue.pos_y = 0;
				// channel 2
				Read_NvItem_ChannelName(channel_name, CHANNEL2);
				positionValue =  OSD_TitleStringPosition(CHANNEL2, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				break;
				
			case DISPLAY_MODE_2SPLIT_HSCALE_B:
			case DISPLAY_MODE_2SPLIT_HCROP_B:
			case DISPLAY_MODE_2SPLIT_VSCALE_B:
			case DISPLAY_MODE_2SPLIT_VCROP_B:
				// channel 3
				Read_NvItem_ChannelName(channel_name, CHANNEL3);
				positionValue =  OSD_TitleStringPosition(CHANNEL3, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				// clean-up
				memset(channel_name, 0x00, CHANNEL_NEME_LENGTH_MAX+1);
				positionValue.pos_x = 0;
				positionValue.pos_y = 0;
				// channel 4
				Read_NvItem_ChannelName(channel_name, CHANNEL4);
				positionValue =  OSD_TitleStringPosition(CHANNEL4, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				break;

			//DISPLAY_MODE_4SPLIT_QUAD,
			//DISPLAY_MODE_4SPLIT_R3SCALE,
			//DISPLAY_MODE_4SPLIT_R3CROP,
			//DISPLAY_MODE_4SPLIT_L3SCALE,
			//DISPLAY_MODE_4SPLIT_L3CROP,
			//DISPLAY_MODE_4SPLIT_D3SCALE,
			//DISPLAY_MODE_4SPLIT_D3CROP,
			//DISPLAY_MODE_4SPLIT_U3SCALE,
			//DISPLAY_MODE_4SPLIT_U3CROP,
			//DISPLAY_MODE_4SPLIT_H,
			//DISPLAY_MODE_4SPLIT_X,

		}

		#if 0
		if(IS_FULL_MODE(displayMode))
		{
			channel = ConvertDisplayMode2Channel(displayMode);
			Read_NvItem_ChannelName(channel_name, channel);
			positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
			OSD_PrintString(positionValue, channel_name, strlen(channel_name));
		}
		else if(IS_PIP_MODE(displayMode))
		{
			// channel 1
			Read_NvItem_ChannelName(channel_name, CHANNEL1);
			positionValue =  OSD_TitleStringPosition(channel, DISPLAY_MODE_FULL_CH1, strlen(channel_name));
			OSD_PrintString(positionValue, channel_name, strlen(channel_name));
			// sub channel
			channel = FineSubChannelForPIP(displayMode);
			Read_NvItem_ChannelName(channel_name, channel);
			positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
			OSD_PrintString(positionValue, channel_name, strlen(channel_name));
		}
		else
		{
			// 2-split
			// 4-split
		}
		#endif
		/*
		else
		{
			max_channel = Get_NumOfDisplayChannels();
			for(channel = CHANNEL1; channel < max_channel; channel++)
			{
				Read_NvItem_ChannelName(channel_name, channel);
				positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
				OSD_PrintString(positionValue, channel_name, strlen(channel_name));
			}
		}
		*/
	}
}


//-----------------------------------------------------------------------------
void Osd_ClearScreen(void)
{
	u16 i;
	BYTE pSTR[DISPLAY_WIDTH/CHAR_WIDTH];

	memset(pSTR, ASCII_SPACE, sizeof(pSTR));

	OSD_SetFontGAC(SPRITE_INDEX0);

	for(i = 0; i < DISPLAY_HEIGHT/CHAR_HEIGHT; i++)
	{
		MDINGAC_SetDrawXYMode(i*CHAR_HEIGHT, 0, pSTR, sizeof(pSTR), 0);
	}
}
//-----------------------------------------------------------------------------
void OSD_EraseAllText(void)
{
	BOOL titleOn;
	BOOL timeOn;
	BOOL dateOn;
	BOOL osdOn;

	Read_NvItem_OsdOn(&osdOn);
	Read_NvItem_TitleDispalyOn(&titleOn);
	Read_NvItem_TimeDisplayOn(&timeOn);
	Read_NvItem_DateDisplayOn(&dateOn);

	if(osdOn == TRUE)
	{
		// erase freeze or auto
		if(titleOn == TRUE)
		{
			//erase title
			OSD_EraseChannelName();
		}
		if((dateOn == TRUE) || (timeOn == TRUE))
		{
			// erase time and/or date
			OSD_EraseTimeDate();
		}
		OSD_EraseNoVideo();
		OSD_EraseAuto();
		OSD_EraseIndicator();
	}
}

//-----------------------------------------------------------------------------
void OSD_Display(void)
{
	BOOL osdDisplayOn;

	Read_NvItem_OsdOn(&osdDisplayOn);

	if((osdDisplayOn == ON) && (GetSystemMode() == SYSTEM_NORMAL_MODE))
	{
		OSD_DisplayDateTime();
		OSD_DisplayChannelName();
		OSD_DisplayVideoMode();
		//OSD_DisplayNoVideo();
		OSD_DisplayAuto();
		OSD_DisplayIndicator();
	}
	else if(GetSystemMode() == SYSTEM_SETUP_MODE)
	{
		DisplayTimeInMenu();
	}

	requestRefreshScreen = CLEAR;
}


//-----------------------------------------------------------------------------
void OSD_DrawBorderLine(void)
{
	BOOL border_line;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();

	Read_NvItem_BorderLineDisplay(&border_line);

	MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
	MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);

	if(border_line == ON)
	{
		ConfigI2C(MDIN_ID_C);
		MDINOSD_SetBGBoxColor(RGB(255,255,255));
		switch(displayMode)
		{
			case DISPLAY_MODE_4SPLIT_QUAD:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				break;

			case DISPLAY_MODE_4SPLIT_R3SCALE:
			case DISPLAY_MODE_4SPLIT_R3CROP:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_WIDTH - (DISPLAY_WIDTH/3)-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_WIDTH - (DISPLAY_WIDTH/3), DISPLAY_HEIGHT / 3 - 1, DISPLAY_WIDTH/3, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, DISPLAY_WIDTH - (DISPLAY_WIDTH/3), DISPLAY_HEIGHT / 3 * 2, DISPLAY_WIDTH/3, 2);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				break;

			case DISPLAY_MODE_4SPLIT_L3SCALE:
			case DISPLAY_MODE_4SPLIT_L3CROP:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, (DISPLAY_WIDTH/3)-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 0, DISPLAY_HEIGHT/3-1, DISPLAY_WIDTH/3, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 0, (DISPLAY_HEIGHT/3) * 2, DISPLAY_WIDTH/3, 2);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				break;

			case DISPLAY_MODE_4SPLIT_D3SCALE:
			case DISPLAY_MODE_4SPLIT_D3CROP:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, (DISPLAY_HEIGHT/3)*2, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_WIDTH/3, (DISPLAY_HEIGHT/3)*2, 2, DISPLAY_HEIGHT/3);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, (DISPLAY_WIDTH/3) * 2, (DISPLAY_HEIGHT/3)*2, 2, DISPLAY_HEIGHT/3);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				break;

			case DISPLAY_MODE_4SPLIT_U3SCALE:
			case DISPLAY_MODE_4SPLIT_U3CROP:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, (DISPLAY_HEIGHT/3)-1, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_WIDTH/3, 0, 2, DISPLAY_HEIGHT/3);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, (DISPLAY_WIDTH/3) * 2, 0, 2, DISPLAY_HEIGHT/3);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				break;

			case DISPLAY_MODE_4SPLIT_H:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_QUAD_WIDTH-1, DISPLAY_HALF_HEIGHT-1, DISPLAY_HALF_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_QUAD_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				break;

			case DISPLAY_MODE_4SPLIT_X:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_QUAD_HEIGHT-1, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 0, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT-1, DISPLAY_WIDTH, 2);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, DISPLAY_HALF_WIDTH-1, DISPLAY_QUAD_HEIGHT-1, 2, DISPLAY_HALF_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				break;
				
			case DISPLAY_MODE_2SPLIT_HSCALE_A:
			case DISPLAY_MODE_2SPLIT_HCROP_A:
			case DISPLAY_MODE_2SPLIT_HSCALE_B:
			case DISPLAY_MODE_2SPLIT_HCROP_B:
				MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				break;

			case DISPLAY_MODE_2SPLIT_VSCALE_A:
			case DISPLAY_MODE_2SPLIT_VCROP_A:
			case DISPLAY_MODE_2SPLIT_VSCALE_B:
			case DISPLAY_MODE_2SPLIT_VCROP_B:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				break;

		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN + PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				PIP_POSITION_MARGIN+PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT - PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN - PIP_WINDOW_WIDTH, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				DISPLAY_WIDTH - PIP_POSITION_MARGIN, 
				DISPLAY_HEIGHT-PIP_POSITION_MARGIN-PIP_WINDOW_HEIGHT, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;

		case DISPLAY_MODE_PIP_D2:
		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
				PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
				PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN + PIP_WINDOW_HEIGHT, 
				PIP_WINDOW_WIDTH, 1);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
				PIP_POSITION_MARGIN, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);
			MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
				PIP_POSITION_MARGIN + PIP_WINDOW_WIDTH, 
				PIP_POSITION_MARGIN, 
				1, PIP_WINDOW_HEIGHT);

			MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
			MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
			break;
                                
			default :
				MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
				MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
                break;
		}
	}
}
