#include "common.h"
#include "osd_string.h"
#if BD_NVP == NVP6158
#include "NVP6158.h"
#elif BD_NVP == NVP6168
//#include "NVP6168.h"
#include "raptor4_fmt.h"
#endif

#define MARGIN_X						5
#define MARGIN_Y						5

#define DATE_TIME_LENGTH				21 // "yyyy-mmm-dd hh:mm:ss "
#define DATE_LENGTH_4DIGIT				11 // "yyyy-mmm-dd"
#define DATE_LENGTH_2DIGIT				9 // "yy-mmm-dd"
#define TIME_LENGTH						8 // "hh:mm:ss"

//#define TITLE_LENGTH					5
#define VIDEO_FORMAT_LENGTH_MAX		20
#define VIDEO_LOSS_LENGTH				8

#define VIDEO_MODE_DISPLAY_TIME			7//seconds
static u8 displayingDateTimeLength = 0;

enum
{
	VIDEO_IN = 0,
	VIDEO_OUT
};


enum 
{
	QUAD = 0,
	QUAD_R,
	QUAD_L,
	QUAD_D,
	QUAD_U,
	QUAD_H,
	QUAD_X,
	NUM_OF_POS_QUAD,
	SPLIT_H = NUM_OF_POS_QUAD,		// 2 split H
	SPLIT_V,		// 2 split V
	NUM_OF_POS_SPLIT
};
enum
{
	PIP_A,
	PIP_B,
	PIP_C,
	PIP_D,
	NUM_OF_POS_PIP
};

static const sPosition_t titlePositionTable_Quad[NUM_OF_POS_QUAD][NUM_OF_CHANNEL] =
{
	//QUAD
	{{DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, 0}, {DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}},
	//Quad_R
	{{DISPLAY_WIDTH/3, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), (DISPLAY_HEIGHT/3)*2}},
	//quad_L
	{{(DISPLAY_WIDTH/3)*2, 0}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, (DISPLAY_HEIGHT/3)*2}},
	//quad_D
	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/3*2}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3*2}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/3*2}},
	//quad_U
	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, 0}, {DISPLAY_HALF_WIDTH, 0}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), 0}},
	//quad_H
	{{DISPLAY_HALF_WIDTH, 0}, {DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_QUAD_WIDTH/2, 0}, {DISPLAY_WIDTH - (DISPLAY_QUAD_WIDTH/2), 0}},
	//quad_X
	{{DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_HALF_WIDTH, 0}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT}},
};

static const sPosition_t indicatorPositionTable_Split[NUM_OF_POS_SPLIT][NUM_OF_CHANNEL] =
{
	//QUAD
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X , DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}, {DISPLAY_HALF_WIDTH+MARGIN_X, DISPLAY_HALF_HEIGHT+MARGIN_Y}},
	//Quad_R
	{{(DISPLAY_WIDTH/3)*2-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
			{(DISPLAY_WIDTH/3)*2+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
	//quad_L
	{{(DISPLAY_WIDTH/3)+MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
	//quad_D
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)*2-CHAR_HEIGHT-MARGIN_Y}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)*2+MARGIN_Y+CHAR_HEIGHT}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)*2+MARGIN_Y}},
	//quad_U
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HEIGHT/3+MARGIN_Y+CHAR_HEIGHT}, {DISPLAY_WIDTH/3-CHAR_WIDTH-MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y},
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y}, {(DISPLAY_WIDTH/3)*2+MARGIN_X, (DISPLAY_HEIGHT/3)-CHAR_HEIGHT-MARGIN_Y}},
	//quad_H
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2 , DISPLAY_HALF_HEIGHT - MARGIN_Y - CHAR_HEIGHT}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT + MARGIN_Y+CHAR_HEIGHT}, 
			{DISPLAY_QUAD_WIDTH -CHAR_WIDTH - MARGIN_X, DISPLAY_HALF_HEIGHT - CHAR_HEIGHT/2}, {DISPLAY_WIDTH -DISPLAY_QUAD_WIDTH + MARGIN_X, DISPLAY_HALF_HEIGHT - CHAR_HEIGHT/2}},
	//quad_X
	{{DISPLAY_HALF_WIDTH - MARGIN_X - CHAR_WIDTH, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH + MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, 
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_QUAD_HEIGHT - MARGIN_Y - CHAR_HEIGHT}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT+MARGIN_Y+CHAR_HEIGHT}},
	// 2split_H
	{{DISPLAY_HALF_WIDTH - MARGIN_X - CHAR_WIDTH, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH + MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, 
			{DISPLAY_HALF_WIDTH - MARGIN_X - CHAR_WIDTH, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}, {DISPLAY_HALF_WIDTH + MARGIN_X, DISPLAY_HALF_HEIGHT-CHAR_HEIGHT/2}},
	// 2split_v
	{{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2 , DISPLAY_HALF_HEIGHT - MARGIN_Y - CHAR_HEIGHT}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT + MARGIN_Y + CHAR_HEIGHT}, 
			{DISPLAY_HALF_WIDTH-CHAR_WIDTH/2 , DISPLAY_HALF_HEIGHT - MARGIN_Y - CHAR_HEIGHT}, {DISPLAY_HALF_WIDTH-CHAR_WIDTH/2, DISPLAY_HALF_HEIGHT + MARGIN_Y+CHAR_HEIGHT}}
};

// center postion of x, start postion of y
static const sPosition_t videoFormatPosition_Split[NUM_OF_POS_SPLIT][NUM_OF_CHANNEL] =
{
	//QUAD
	{{DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_QUAD_HEIGHT},
			{DISPLAY_QUAD_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT}},
	//Quad_R
	{{DISPLAY_WIDTH/3, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/6},
			{DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT - (DISPLAY_HEIGHT/6)}},
	//quad_L
	{{(DISPLAY_WIDTH/3)*2, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/6},
			{DISPLAY_WIDTH/6, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT - (DISPLAY_HEIGHT/6)}},
	//quad_D
	{{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/3}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT-DISPLAY_HEIGHT/6},
			{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT-DISPLAY_HEIGHT/6}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT-DISPLAY_HEIGHT/6}},
	//quad_U
	{{DISPLAY_HALF_WIDTH, (DISPLAY_HEIGHT/3)*2}, {DISPLAY_WIDTH/6, DISPLAY_HEIGHT/6},
			{DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT/6}, {DISPLAY_WIDTH - (DISPLAY_WIDTH/6), DISPLAY_HEIGHT/6}},
	//quad_H
	{{DISPLAY_HALF_WIDTH , DISPLAY_QUAD_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT}, 
			{DISPLAY_QUAD_WIDTH/2, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH-DISPLAY_QUAD_WIDTH/2, DISPLAY_HALF_HEIGHT}},
	//quad_X
	{{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_HALF_WIDTH + DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, 
			{DISPLAY_HALF_WIDTH, DISPLAY_QUAD_HEIGHT/2}, {DISPLAY_HALF_WIDTH, DISPLAY_HEIGHT-DISPLAY_QUAD_HEIGHT/2}},
	// 2 split_H
	{{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT},
			{DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}, {DISPLAY_WIDTH - DISPLAY_QUAD_WIDTH, DISPLAY_HALF_HEIGHT}},
	// 2 split_V
	{{DISPLAY_HALF_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT+DISPLAY_QUAD_HEIGHT},
			{DISPLAY_HALF_WIDTH, DISPLAY_QUAD_HEIGHT}, {DISPLAY_HALF_WIDTH, DISPLAY_HALF_HEIGHT+DISPLAY_QUAD_HEIGHT}}
};


static const sPosition_t videoFormatPosition_Full =
{
	DISPLAY_HALF_WIDTH, //x
	DISPLAY_HALF_HEIGHT	//y
};
static const sPosition_t videoLossPosition_Full =
{
	(DISPLAY_WIDTH - (VIDEO_LOSS_LENGTH*CHAR_WIDTH)) / 2,
	(DISPLAY_HEIGHT - CHAR_HEIGHT) / 2
};

static BOOL requestRefreshScreen = CLEAR;
static BOOL boarderLineUpdate = CLEAR;

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
	u8 titleLength;

	titleLength = length * CHAR_WIDTH;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			position.pos_x = (DISPLAY_WIDTH - titleLength) / 2;
			position.pos_y = MARGIN_Y;
			break;

		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
			position.pos_x = DISPLAY_WIDTH-(PIP_POSITION_MARGIN_1080+PIP_WINDOW_WIDTH_1080) + ((PIP_WINDOW_WIDTH_1080-titleLength)/2);
			position.pos_y = MARGIN_Y+PIP_POSITION_MARGIN_1080;
			break;

		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
			position.pos_x = PIP_POSITION_MARGIN_1080+ ((PIP_WINDOW_WIDTH_1080-titleLength)/2);
			position.pos_y = DISPLAY_HEIGHT -(PIP_WINDOW_HEIGHT_1080+ PIP_POSITION_MARGIN_1080) + MARGIN_Y;
			break;

		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
			position.pos_x = DISPLAY_WIDTH-(PIP_POSITION_MARGIN_1080+PIP_WINDOW_WIDTH_1080) + ((PIP_WINDOW_WIDTH_1080-titleLength)/2);
			position.pos_y = DISPLAY_HEIGHT -(PIP_WINDOW_HEIGHT_1080+ PIP_POSITION_MARGIN_1080) + MARGIN_Y;
			break;

		case DISPLAY_MODE_PIP_D2:
		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			position.pos_x = PIP_POSITION_MARGIN_1080+ ((PIP_WINDOW_WIDTH_1080-titleLength)/2);
			position.pos_y = MARGIN_Y+PIP_POSITION_MARGIN_1080;
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
			if((channel == CHANNEL1) || (channel == CHANNEL3))
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - titleLength) / 2;
			}
			else //ch2 or ch4
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - titleLength) / 2;
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

		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y;
			}
			else // ch3
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y + DISPLAY_HALF_HEIGHT;
			}
			break;

		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y;
			}
			else // ch3
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y + DISPLAY_HALF_HEIGHT;
			}
			break;

		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH)) / 2;
				position.pos_y = MARGIN_Y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y + DISPLAY_HALF_HEIGHT;
			}
			else
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y + DISPLAY_HALF_HEIGHT;
			}
			break;

		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH)) / 2;
				position.pos_y = MARGIN_Y + DISPLAY_HALF_HEIGHT;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y;
			}
			else
			{
				position.pos_x = DISPLAY_HALF_WIDTH + (DISPLAY_HALF_WIDTH - titleLength) / 2;
				position.pos_y = MARGIN_Y;
			}
			break;

		case DISPLAY_MODE_4SPLIT_QUAD:
			position.pos_x = titlePositionTable_Quad[QUAD][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD][channel].pos_y + MARGIN_Y;
			break;
			
		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
			position.pos_x = titlePositionTable_Quad[QUAD_R][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD_R][channel].pos_y + MARGIN_Y;
			break;
			
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
			position.pos_x = titlePositionTable_Quad[QUAD_L][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD_L][channel].pos_y + MARGIN_Y;
			break;
			
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
			position.pos_x = titlePositionTable_Quad[QUAD_D][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD_D][channel].pos_y + MARGIN_Y;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
			position.pos_x = titlePositionTable_Quad[QUAD_U][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD_U][channel].pos_y + MARGIN_Y;
			break;

		case DISPLAY_MODE_4SPLIT_H:
			position.pos_x = titlePositionTable_Quad[QUAD_H][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD_H][channel].pos_y + MARGIN_Y;
			break;

		case DISPLAY_MODE_4SPLIT_X:
			position.pos_x = titlePositionTable_Quad[QUAD_X][channel].pos_x - titleLength/2;
			position.pos_y = titlePositionTable_Quad[QUAD_X][channel].pos_y + MARGIN_Y;
			break;
			
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

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
			position.pos_x = indicatorPositionTable_Split[SPLIT_H][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[SPLIT_H][channel].pos_y;
			break;
		
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			position.pos_x = indicatorPositionTable_Split[SPLIT_V][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[SPLIT_V][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_QUAD:
			position.pos_x = indicatorPositionTable_Split[QUAD][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
			position.pos_x = indicatorPositionTable_Split[QUAD_R][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD_R][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
			position.pos_x = indicatorPositionTable_Split[QUAD_L][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD_L][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
			position.pos_x = indicatorPositionTable_Split[QUAD_D][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD_D][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
			position.pos_x = indicatorPositionTable_Split[QUAD_U][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD_U][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_H:
			position.pos_x = indicatorPositionTable_Split[QUAD_H][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD_H][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_X:
			position.pos_x = indicatorPositionTable_Split[QUAD_X][channel].pos_x;
			position.pos_y = indicatorPositionTable_Split[QUAD_X][channel].pos_y;
			break;

		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = indicatorPositionTable_Split[SPLIT_H][CHANNEL1].pos_x;
				position.pos_y = indicatorPositionTable_Split[SPLIT_H][CHANNEL1].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL2].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL2].pos_y;
			}
			else
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL4].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL4].pos_y;
			}
			break;
			
		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = indicatorPositionTable_Split[SPLIT_H][CHANNEL2].pos_x;
				position.pos_y = indicatorPositionTable_Split[SPLIT_H][CHANNEL2].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL1].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL1].pos_y;
			}
			else
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL3].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL3].pos_y;
			}
			break;
			
		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = indicatorPositionTable_Split[SPLIT_V][CHANNEL1].pos_x;
				position.pos_y = indicatorPositionTable_Split[SPLIT_V][CHANNEL1].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL3].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL3].pos_y;
			}
			else
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL4].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL4].pos_y;
			}
			break;
			
		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = indicatorPositionTable_Split[SPLIT_V][CHANNEL2].pos_x;
				position.pos_y = indicatorPositionTable_Split[SPLIT_V][CHANNEL2].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL1].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL1].pos_y;
			}
			else
			{
				position.pos_x = indicatorPositionTable_Split[QUAD][CHANNEL2].pos_x;
				position.pos_y = indicatorPositionTable_Split[QUAD][CHANNEL2].pos_y;
			}
			break;
			
		case DISPLAY_MODE_PIP_A2:
		case DISPLAY_MODE_PIP_A3:
		case DISPLAY_MODE_PIP_A4:
			if(channel == CHANNEL1)
			{
				// same with full screen
				position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
				position.pos_y = DISPLAY_HEIGHT - 2*CHAR_HEIGHT - MARGIN_Y;
			}
			else
			{
				position.pos_x = DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080/2 - CHAR_WIDTH/2;
				position.pos_y = PIP_POSITION_MARGIN_1080 + PIP_WINDOW_HEIGHT_1080- MARGIN_Y - CHAR_HEIGHT; 
			}
			break;
			
		case DISPLAY_MODE_PIP_B2:
		case DISPLAY_MODE_PIP_B3:
		case DISPLAY_MODE_PIP_B4:
			if(channel == CHANNEL1)
			{
				// same with full screen
				position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
				position.pos_y = DISPLAY_HEIGHT - 2*CHAR_HEIGHT - MARGIN_Y;
			}
			else
			{
				position.pos_x = PIP_POSITION_MARGIN + PIP_WINDOW_WIDTH_1080/2 - CHAR_WIDTH/2;
				position.pos_y = DISPLAY_HEIGHT - PIP_POSITION_MARGIN_1080- MARGIN_Y - CHAR_HEIGHT; 
			}
			break;
			
		case DISPLAY_MODE_PIP_C2:
		case DISPLAY_MODE_PIP_C3:
		case DISPLAY_MODE_PIP_C4:
			if(channel == CHANNEL1)
			{
				// same with full screen
				position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
				position.pos_y = DISPLAY_HEIGHT - 2*CHAR_HEIGHT - MARGIN_Y;
			}
			else
			{
				position.pos_x = DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080/2 - CHAR_WIDTH/2;
				position.pos_y = DISPLAY_HEIGHT - PIP_POSITION_MARGIN_1080 - MARGIN_Y - CHAR_HEIGHT; 
			}
			break;
			
		case DISPLAY_MODE_PIP_D2:
		case DISPLAY_MODE_PIP_D3:
		case DISPLAY_MODE_PIP_D4:
			if(channel == CHANNEL1)
			{
				// same with full screen
				position.pos_x = (DISPLAY_WIDTH - (length * CHAR_WIDTH))/2;
				position.pos_y = DISPLAY_HEIGHT - 2*CHAR_HEIGHT - MARGIN_Y;
			}
			else
			{
				position.pos_x = PIP_POSITION_MARGIN_1080 + PIP_WINDOW_WIDTH_1080/2 - CHAR_WIDTH/2;
				position.pos_y = PIP_POSITION_MARGIN_1080 + PIP_WINDOW_HEIGHT_1080 - MARGIN_Y - CHAR_HEIGHT; 
			}
			break;
	}
	return position;
}

static sPosition_t OSD_VideoModeStringPosition(eChannel_t channel, eDisplayMode_t displayMode, u8 videoDir)
{
	sPosition_t position = {0xFF00,0xFF00};

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
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
			position.pos_x = videoFormatPosition_Full.pos_x;
			position.pos_y = videoFormatPosition_Full.pos_y ;
			break;
			
		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
			position.pos_x = videoFormatPosition_Split[SPLIT_H][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[SPLIT_H][channel].pos_y;
			break;
		
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			position.pos_x = videoFormatPosition_Split[SPLIT_V][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[SPLIT_V][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_QUAD:
			position.pos_x = videoFormatPosition_Split[QUAD][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_R3SCALE:
		case DISPLAY_MODE_4SPLIT_R3CROP:
			position.pos_x = videoFormatPosition_Split[QUAD_R][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD_R][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_L3SCALE:
		case DISPLAY_MODE_4SPLIT_L3CROP:
			position.pos_x = videoFormatPosition_Split[QUAD_L][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD_L][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_D3SCALE:
		case DISPLAY_MODE_4SPLIT_D3CROP:
			position.pos_x = videoFormatPosition_Split[QUAD_D][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD_D][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_U3SCALE:
		case DISPLAY_MODE_4SPLIT_U3CROP:
			position.pos_x = videoFormatPosition_Split[QUAD_U][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD_U][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_H:
			position.pos_x = videoFormatPosition_Split[QUAD_H][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD_H][channel].pos_y;
			break;
			
		case DISPLAY_MODE_4SPLIT_X:
			position.pos_x = videoFormatPosition_Split[QUAD_X][channel].pos_x;
			position.pos_y = videoFormatPosition_Split[QUAD_X][channel].pos_y;
			break;

		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = videoFormatPosition_Split[SPLIT_H][CHANNEL1].pos_x;
				position.pos_y = videoFormatPosition_Split[SPLIT_H][CHANNEL1].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL2].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL2].pos_y;
			}
			else if(channel == CHANNEL3)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL4].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL4].pos_y;
			}
			break;
			
		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = videoFormatPosition_Split[SPLIT_H][CHANNEL2].pos_x;
				position.pos_y = videoFormatPosition_Split[SPLIT_H][CHANNEL2].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL1].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL1].pos_y;
			}
			else if(channel == CHANNEL3)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL3].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL3].pos_y;
			}
			break;
			
		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = videoFormatPosition_Split[SPLIT_V][CHANNEL1].pos_x;
				position.pos_y = videoFormatPosition_Split[SPLIT_V][CHANNEL1].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL3].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL3].pos_y;
			}
			else if(channel == CHANNEL3)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL4].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL4].pos_y;
			}
			break;
			
		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
			if(channel == CHANNEL1)
			{
				position.pos_x = videoFormatPosition_Split[SPLIT_V][CHANNEL2].pos_x;
				position.pos_y = videoFormatPosition_Split[SPLIT_V][CHANNEL2].pos_y;
			}
			else if(channel == CHANNEL2)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL1].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL1].pos_y;
			}
			else if(channel == CHANNEL3)
			{
				position.pos_x = videoFormatPosition_Split[QUAD][CHANNEL2].pos_x;
				position.pos_y = videoFormatPosition_Split[QUAD][CHANNEL2].pos_y;
			}
			break;
	}

	position.pos_x -= (VIDEO_FORMAT_LENGTH_MAX*CHAR_WIDTH)/2;
	if(videoDir == VIDEO_IN)
	{
		position.pos_y -= (CHAR_HEIGHT+MARGIN_Y);
	}
	else
	{
		position.pos_y += MARGIN_Y;
	}
	
	return position;

}

static u8 Get_NumOfDisplayChannels(eDisplayMode_t displayMode)
{
	u8 channels = NUM_OF_CHANNEL;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			channels = 1;
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_A:	
		case DISPLAY_MODE_2SPLIT_VCROP_A:
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

		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
			channels = 3;
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
	if(year4digit == FALSE) // 2digit
	{
		year[0] = ((rtcDate.year + DEFAULT_YEAR)/ 10) + ASCII_ZERO;
		year[1] = ((rtcDate.year + DEFAULT_YEAR)% 10) + ASCII_ZERO;
		dateLength = DATE_LENGTH_2DIGIT;
	}
	else // 4digit
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
		case AHD_1080P_30P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p30;
			break;
		case AHD_1080P_25P:
			inStr = (u8 *)osdStr_Format_In_AHD_1080p25;
			break;
		case TVI_FHD_30P:
			inStr = (u8 *)osdStr_Format_In_TVI_1080p30;
			break;
//		case TVI_FHD_60P:
//			inStr = (u8 *)osdStr_Format_In_TVI_1080p60;
//			break;
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
		case AHD_720P_60P:
		case AHD_720P_30P:
		case AHD_720P_30P_EX:
		case AHD_720P_30P_EX_Btype:
			inStr = (u8 *)osdStr_Format_In_AHD_720p30;
			break;
		case AHD_720P_50P:
		case AHD_720P_25P:
		case AHD_720P_25P_EX:
		case AHD_720P_25P_EX_Btype:
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

		case SD_H960_NT:
		case SD_H960_EX_NT:
		case SD_H960_2EX_NT:
		case SD_H960_2EX_Btype_NT:
			inStr = (u8 *)osdStr_Format_In_CVBS_NTSC;
			break;

		case SD_H960_PAL:
		case SD_H960_EX_PAL:
		case SD_H960_2EX_PAL:
		case SD_H960_2EX_Btype_PAL:
			inStr = (u8 *)osdStr_Format_In_CVBS_PAL;
			break;

		default:
			inStr = (u8 *)osdStr_Space20;
			break;
	}
	return inStr;
}

static u8* FindIndicator(eDisplayMode_t mode, eChannel_t channel)
{
	u8* pStr;
	BOOL videoLossDiplayOn;

	if(IS_FULL_MODE(mode) == TRUE)
	{
		if(GetAlarmStatus(channel) == ALARM_SET)
		{
			pStr = (u8*)osdStr_AlarmFull;
		}
		else if(Get_MotionDetectedStatus(channel))//motion
		{
			pStr = (u8*)osdStr_MotionFull;
		}
		else if(IsScreenFreeze())//freeze
		{
			pStr = (u8*)osdStr_FreezeFull;
		}
		else
		{
			pStr = (u8*)osdStr_Space6;
		}
	}
	else
	{
		Read_NvItem_VideoLossDisplayOn(&videoLossDiplayOn);
		if((IsVideoLossChannel(channel) == TRUE) && (videoLossDiplayOn == ON))
		{
			pStr = (u8*)osdStr_NoVideo;
		}
		else if(GetAlarmStatus(channel) == ALARM_SET)
		{
			pStr = (u8*)osdStr_Alarm;
		}
		else if(Get_MotionDetectedStatus(channel))
		{
			pStr = (u8*)osdStr_Motion;
		}
		else if(IsScreenFreeze())
		{
			pStr = (u8*)osdStr_Freeze;
		}
		else
		{
			pStr = (u8*)osdStr_Space1;
		}
	}
	
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

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			channel = ConvertDisplayMode2Channel(displayMode);
			position =  OSD_TitleStringPosition(channel, displayMode, strlen(osdStr_Space12));
			OSD_PrintString(position, osdStr_Space12, strlen(osdStr_Space12));
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
			position =  OSD_TitleStringPosition(CHANNEL1, DISPLAY_MODE_FULL_CH1, strlen(osdStr_Space12));
			OSD_PrintString(position, osdStr_Space12, strlen(osdStr_Space12));
			position.pos_x = 0;
			position.pos_y = 0;
			// sub channel
			position =  OSD_TitleStringPosition(CHANNEL2, displayMode, strlen(osdStr_Space12));
			OSD_PrintString(position, osdStr_Space12, strlen(osdStr_Space12));
			break;
			
		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			position =  OSD_TitleStringPosition(CHANNEL1, displayMode, strlen(osdStr_Space12));
			OSD_PrintString(position, osdStr_Space12, strlen(osdStr_Space12));

			position =  OSD_TitleStringPosition(CHANNEL2, displayMode, strlen(osdStr_Space12));
			OSD_PrintString(position, osdStr_Space12, strlen(osdStr_Space12));
			break;

		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
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
			for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(displayMode); channel++)
			{
				position =  OSD_TitleStringPosition(channel, displayMode, strlen(osdStr_Space12));
				OSD_PrintString(position, osdStr_Space12, strlen(osdStr_Space12));
			}
			break;
	}
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
	
	if(IS_FULL_MODE(displayMode) == TRUE)
	{
		position = OSD_IndicatorStringPosition(CHANNEL1, displayMode, strlen(osdStr_Space10));
		OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
	}
	else
	{
		for(channel = CHANNEL1; channel < Get_NumOfDisplayChannels(displayMode); channel++)
		{
			position = OSD_IndicatorStringPosition(channel, displayMode, strlen(osdStr_Space10));
			OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
		}
	}
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

static void OSD_EraseVideoMode(void)
{
	sPosition_t position;
	eChannel_t channel;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	
	if(IS_FULL_MODE(displayMode) || IS_PIP_MODE(displayMode))
	{
		position = OSD_VideoModeStringPosition(CHANNEL1, displayMode, VIDEO_IN);
		OSD_PrintString(position, osdStr_Space20, strlen(osdStr_Space20));
		position = OSD_VideoModeStringPosition(CHANNEL1, displayMode, VIDEO_OUT);
		OSD_PrintString(position, osdStr_Space20, strlen(osdStr_Space20));
	}
	else
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			position = OSD_VideoModeStringPosition(channel, displayMode, VIDEO_IN);
			OSD_PrintString(position, osdStr_Space20, strlen(osdStr_Space20));
			position = OSD_VideoModeStringPosition(channel, displayMode, VIDEO_OUT);
			OSD_PrintString(position, osdStr_Space20, strlen(osdStr_Space20));
		}
	}
}

//-----------------------------------------------------------------------------
// No Video
//-----------------------------------------------------------------------------
static void OSD_DisplayNoVideo(void)
{
	sPosition_t position;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	eChannel_t chan;
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
				OSD_PrintString(position, osdStr_Space10, strlen(osdStr_Space10));
			}
		}
	}
}

u8* pTestString;
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
//	u8 num_of_channels = 1;

/*
	if((preDisplayMode != displayMode) && (preDisplayMode != DISPLAY_MODE_MAX))
	{
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
		}
		preInVideo = NULL;
	}
	preDisplayMode = displayMode;
*/
//	if(IS_FULL_MODE(displayMode) == TRUE)	num_of_channels = 1;
//	else if((IS_PIP_MODE(displayMode) == TRUE) || (IS_2SPLIT_MODE(displayMode) == TRUE)	num_of_channels = 2;
//	else if(IS_4SPLIT_MODE(displayMode) == TRUE)	num_of_channels = 4;
//	else		num_of_channels = 3;

	if((IS_FULL_MODE(displayMode) == TRUE) || (IS_PIP_MODE(displayMode) == TRUE))
	{
		channel = ConvertDisplayMode2Channel(displayMode);
		inPosition = OSD_VideoModeStringPosition(channel, displayMode, VIDEO_IN);
		outPosition = OSD_VideoModeStringPosition(channel, displayMode, VIDEO_OUT);
		
		if((IsVideoLossChannel(channel)  == FALSE) && (GetAutoSeqOn() == FALSE))
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
				OSD_PrintString(inPosition, pInVideoStr, strlen(pInVideoStr));
				OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr));
			}

			if((TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn1s,1)) && (videoModeDisplayCount[channel] > 0))
			{
				videoModeDisplayCount[channel]--;
			}
		}
		else
		{
			videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
			
			pInVideoStr = (u8*)osdStr_Space20;
			pOutVideoStr = (u8*)osdStr_Space20;

			if((preInVideo != pInVideoStr) || (preOutVideo != pOutVideoStr))
			{
				OSD_PrintString(inPosition, pInVideoStr, strlen(pInVideoStr));
				OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr));
			}
			OSD_DisplayNoVideo();
		}
		preInVideo = pInVideoStr;
		preOutVideo = pOutVideoStr;

	}
	else	 // split mode
	{
		
		for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
		{
			inPosition =  OSD_VideoModeStringPosition(channel, displayMode, VIDEO_IN);
			outPosition = OSD_VideoModeStringPosition(channel, displayMode, VIDEO_OUT);
			
			if(IsVideoLossChannel(channel)  == FALSE)
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
				
				if((displayMode == DISPLAY_MODE_2SPLIT_HSCALE_A) || (displayMode == DISPLAY_MODE_2SPLIT_HCROP_A) ||
					(displayMode == DISPLAY_MODE_2SPLIT_VSCALE_A) || (displayMode == DISPLAY_MODE_2SPLIT_VCROP_A))
				{
					if((channel == CHANNEL1) || (channel == CHANNEL2))
					{
						OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
						OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
					}
				}
				else if((displayMode == DISPLAY_MODE_2SPLIT_HSCALE_B) || (displayMode == DISPLAY_MODE_2SPLIT_HCROP_B) ||
					(displayMode == DISPLAY_MODE_2SPLIT_VSCALE_B) || (displayMode == DISPLAY_MODE_2SPLIT_VCROP_B))
				{
					if((channel == CHANNEL3) || (channel == CHANNEL4))
					{
						OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
						OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
					}
				}
				else if(IS_4SPLIT_MODE(displayMode) == TRUE)
				{
					OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
					OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
				}
				else		// 3split
				{
					if(channel != CHANNEL4)
					{
						OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
						OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
					}
				}
				if((TIME_AFTER(currentSystemTime->tickCount_1s, previousSystemTimeIn1s,1)) && (videoModeDisplayCount[channel] > 0))
				{
					videoModeDisplayCount[channel]--;
				}
			}
			else
			{
				videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
				
				pInVideoStr = (u8*)osdStr_Space20;
				pOutVideoStr = (u8*)osdStr_Space20;
				if((displayMode == DISPLAY_MODE_2SPLIT_HSCALE_A) || (displayMode == DISPLAY_MODE_2SPLIT_HCROP_A) ||
					(displayMode == DISPLAY_MODE_2SPLIT_VSCALE_A) || (displayMode == DISPLAY_MODE_2SPLIT_VCROP_A))
				{
					if((channel == CHANNEL1) || (channel == CHANNEL2))
					{
						OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
						OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
					}
				}
				else if((displayMode == DISPLAY_MODE_2SPLIT_HSCALE_B) || (displayMode == DISPLAY_MODE_2SPLIT_HCROP_B) ||
					(displayMode == DISPLAY_MODE_2SPLIT_VSCALE_B) || (displayMode == DISPLAY_MODE_2SPLIT_VCROP_B))
				{
					if((channel == CHANNEL3) || (channel == CHANNEL4))
					{
						OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
						OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
					}
				}
				else if(IS_4SPLIT_MODE(displayMode) == TRUE)
				{
					OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
					OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
				}
				else		// 3split
				{
					if(channel != CHANNEL4)
					{
						OSD_PrintString(inPosition, pInVideoStr, strlen((const u8*)pInVideoStr));
						OSD_PrintString(outPosition, pOutVideoStr, strlen((const u8*)pOutVideoStr)); 
					}
				}

			}
		}
	}
	previousSystemTimeIn1s = currentSystemTime->tickCount_1s;
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
	static u8* preIndicator[4]= NULL;
	static eDisplayMode_t preMode;

	switch(displayMode)
	{
		case DISPLAY_MODE_FULL_CH1:
		case DISPLAY_MODE_FULL_CH2:
		case DISPLAY_MODE_FULL_CH3:
		case DISPLAY_MODE_FULL_CH4:
			channel = ConvertDisplayMode2Channel(displayMode);
			pIndicator = FindIndicator(displayMode, channel);
			//if((displayMode != preMode) || (pIndicator != preIndicator[0]))
			{
				position = OSD_IndicatorStringPosition(channel, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[0] = pIndicator;
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
			pIndicator = FindIndicator(DISPLAY_MODE_FULL_CH1, CHANNEL1);
			//if((displayMode != preMode) || (pIndicator != preIndicator[0]))
			{
				position = OSD_IndicatorStringPosition(CHANNEL1, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[0] = pIndicator;

			// sub channel
			channel = FindAuxChannel(displayMode, MDIN_ID_A);
			pIndicator = FindIndicator(displayMode, channel);		
			//if((displayMode != preMode) || (pIndicator != preIndicator[1]))
			{
				position = OSD_IndicatorStringPosition(channel, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[1] = pIndicator;
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
			for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
			{
				pIndicator = FindIndicator(displayMode, channel);
				//if((displayMode != preMode) || (pIndicator != preIndicator[channel]))
				{
					position = OSD_IndicatorStringPosition(channel, displayMode, strlen((const u8*)pIndicator));
					OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
				}
				//preIndicator[channel] = pIndicator;
			}
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_A:
		case DISPLAY_MODE_2SPLIT_HCROP_A:
		case DISPLAY_MODE_2SPLIT_VSCALE_A:
		case DISPLAY_MODE_2SPLIT_VCROP_A:
			// channel 1
			pIndicator = FindIndicator(displayMode, CHANNEL1);
			//if((displayMode != preMode) || (pIndicator != preIndicator[0]))
			{
				position = OSD_IndicatorStringPosition(CHANNEL1, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[0] = pIndicator;
			
			// channel 2
			pIndicator = FindIndicator(displayMode, CHANNEL2);
			//if((displayMode != preMode) || (pIndicator != preIndicator[1]))
			{
				position = OSD_IndicatorStringPosition(CHANNEL2, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[1] = pIndicator;
			break;

		case DISPLAY_MODE_2SPLIT_HSCALE_B:
		case DISPLAY_MODE_2SPLIT_HCROP_B:
		case DISPLAY_MODE_2SPLIT_VSCALE_B:
		case DISPLAY_MODE_2SPLIT_VCROP_B:
			// channel 3
			pIndicator = FindIndicator(displayMode, CHANNEL3);
			//if((displayMode != preMode) || (pIndicator != preIndicator[0]))
			{
				position = OSD_IndicatorStringPosition(CHANNEL3, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[0] = pIndicator;

			// channel 4
			pIndicator = FindIndicator(displayMode, CHANNEL4);
			//if((displayMode != preMode) || (pIndicator != preIndicator[1]))
			{
				position = OSD_IndicatorStringPosition(CHANNEL4, displayMode, strlen((const u8*)pIndicator));
				OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
			}
			//preIndicator[0] = pIndicator;
			break;
			
		case DISPLAY_MODE_3SPLIT_R2SCALE:
		case DISPLAY_MODE_3SPLIT_R2CROP:
		case DISPLAY_MODE_3SPLIT_L2SCALE:
		case DISPLAY_MODE_3SPLIT_L2CROP:
		case DISPLAY_MODE_3SPLIT_D2SCALE:
		case DISPLAY_MODE_3SPLIT_D2CROP:
		case DISPLAY_MODE_3SPLIT_U2SCALE:
		case DISPLAY_MODE_3SPLIT_U2CROP:
			for(channel = CHANNEL1; channel < NUM_OF_CHANNEL-1; channel++)
			{
				pIndicator = FindIndicator(displayMode, channel);
				//if((displayMode != preMode) || (pIndicator != preIndicator[channel]))
				{
					position = OSD_IndicatorStringPosition(channel, displayMode, strlen((const u8*)pIndicator));
					OSD_PrintString(position, (const u8*)pIndicator, strlen((const u8*)pIndicator));
				}
				//preIndicator[channel] = pIndicator;
			}
			break;
	}

	preMode = displayMode;
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
//	ConfigI2C(MDIN_ID_C);
	M380_ID = MDIN_ID_C;
	
	//OSD_SetFontGAC(SPRITE_INDEX0);
	MDINGAC_SetDrawXYMode(position.pos_y, position.pos_x, (PBYTE)pData, size, 0);
	//MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX0], ON);
}

void OSD_DisplayChannelName(void)
{
	eChannel_t channel, max_channel = NUM_OF_CHANNEL;
	sPosition_t positionValue;
	BOOL titleDisplayOn;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	u8 channel_name[CHANNEL_NEME_LENGTH_MAX+1] = {0,};

	Read_NvItem_TitleDispalyOn(&titleDisplayOn);

	if((titleDisplayOn == ON) && (requestRefreshScreen == SET))
	//if(titleDisplayOn == ON)
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
				channel = FindAuxChannel(displayMode, MDIN_ID_A);
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

			case DISPLAY_MODE_3SPLIT_R2SCALE:
			case DISPLAY_MODE_3SPLIT_R2CROP:
			case DISPLAY_MODE_3SPLIT_L2SCALE:
			case DISPLAY_MODE_3SPLIT_L2CROP:
			case DISPLAY_MODE_3SPLIT_D2SCALE:
			case DISPLAY_MODE_3SPLIT_D2CROP:
			case DISPLAY_MODE_3SPLIT_U2SCALE:
			case DISPLAY_MODE_3SPLIT_U2CROP:
				for(channel = CHANNEL1; channel < NUM_OF_CHANNEL-1; channel++)
				{
					// clean-up
					memset(channel_name, 0x00, CHANNEL_NEME_LENGTH_MAX+1);
					positionValue.pos_x = 0;
					positionValue.pos_y = 0;
					Read_NvItem_ChannelName(channel_name, channel);
					positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
					OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				}
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
				for(channel = CHANNEL1; channel < NUM_OF_CHANNEL; channel++)
				{
					// clean-up
					memset(channel_name, 0x00, CHANNEL_NEME_LENGTH_MAX+1);
					positionValue.pos_x = 0;
					positionValue.pos_y = 0;
					Read_NvItem_ChannelName(channel_name, channel);
					positionValue =  OSD_TitleStringPosition(channel, displayMode, strlen(channel_name));
					OSD_PrintString(positionValue, channel_name, strlen(channel_name));
				}
				break;
		}
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
		OSD_EraseVideoMode();
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
		//Delay_us(10);
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


void OSD_SetBoaderLine(void)
{
	boarderLineUpdate = SET;
}


//-----------------------------------------------------------------------------
void OSD_DrawBorderLine(void)	//DONE
{
	BOOL border_line;
	eDisplayMode_t displayMode = GetCurrentDisplayMode();
	//static eDisplayMode_t prevMode = 0xFF;
	//static BOOL prevBorder = ON;


	if(boarderLineUpdate == SET)
	{
		boarderLineUpdate = CLEAR;
		Read_NvItem_BorderLineDisplay(&border_line);

	//if((border_line == OFF) ||( (IS_FULL_MODE(displayMode) == TRUE) && (GetSystemMode() == SYSTEM_NORMAL_MODE))|| (prevMode != displayMode))
	{
		MDINOSD_EnableBGBox(BGBOX_INDEX0, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX1, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX2, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX3, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX4, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX5, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX6, OFF);
		MDINOSD_EnableBGBox(BGBOX_INDEX7, OFF);
	}

	if(border_line == ON)
	{
		//if(( (IS_FULL_MODE(displayMode) == FALSE) && (prevMode != displayMode)) || (forceUpdate == TRUE))
		{
			MDINOSD_SetBGBoxColor(WHITE(GetCurrentColorFormat()));
			switch(displayMode)
			{
				case DISPLAY_MODE_3SPLIT_R2SCALE:
				case DISPLAY_MODE_3SPLIT_R2CROP:
					MDINOSD_SetBGBoxArea(BGBOX_INDEX0, DISPLAY_HALF_WIDTH-1, DISPLAY_HALF_HEIGHT-1, DISPLAY_HALF_WIDTH, 2);	//h
					MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);	//v
					MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
					MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
					break;
					
				case DISPLAY_MODE_3SPLIT_L2SCALE:
				case DISPLAY_MODE_3SPLIT_L2CROP:
					MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_HALF_WIDTH, 2);	//h
					MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HEIGHT);
					MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
					MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
					break;
					
				case DISPLAY_MODE_3SPLIT_D2SCALE:
				case DISPLAY_MODE_3SPLIT_D2CROP:
					MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
					MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, DISPLAY_HALF_HEIGHT-1, 2, DISPLAY_HALF_HEIGHT);
					MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
					MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
					break;
					
				case DISPLAY_MODE_3SPLIT_U2SCALE:
				case DISPLAY_MODE_3SPLIT_U2CROP:
					MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 0, DISPLAY_HALF_HEIGHT-1, DISPLAY_WIDTH, 2);
					MDINOSD_SetBGBoxArea(BGBOX_INDEX1, DISPLAY_HALF_WIDTH-1, 0, 2, DISPLAY_HALF_HEIGHT);
					MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
					MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
					break;
					
					
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
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080, 
					PIP_POSITION_MARGIN_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080, 
					PIP_POSITION_MARGIN_1080 + PIP_WINDOW_HEIGHT_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080-1,		// 190905
					PIP_POSITION_MARGIN_1080, 
					1, PIP_WINDOW_HEIGHT_1080);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080, 
					PIP_POSITION_MARGIN_1080, 
					1, PIP_WINDOW_HEIGHT_1080);

				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
				break;

			case DISPLAY_MODE_PIP_B2:
			case DISPLAY_MODE_PIP_B3:
			case DISPLAY_MODE_PIP_B4:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
					PIP_POSITION_MARGIN_1080, 
					DISPLAY_HEIGHT-PIP_POSITION_MARGIN_1080 -PIP_WINDOW_HEIGHT_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
					PIP_POSITION_MARGIN_1080, 
					DISPLAY_HEIGHT - PIP_POSITION_MARGIN_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
					PIP_POSITION_MARGIN_1080-1, 		// 190905
					DISPLAY_HEIGHT-PIP_POSITION_MARGIN_1080 -PIP_WINDOW_HEIGHT_1080, 
					1, PIP_WINDOW_HEIGHT_1080);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
					PIP_POSITION_MARGIN_1080 + PIP_WINDOW_WIDTH_1080, 
					DISPLAY_HEIGHT- PIP_POSITION_MARGIN_1080 - PIP_WINDOW_HEIGHT_1080, 
					1, PIP_WINDOW_HEIGHT_1080);

				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
				break;

			case DISPLAY_MODE_PIP_C2:
			case DISPLAY_MODE_PIP_C3:
			case DISPLAY_MODE_PIP_C4:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080, 
					DISPLAY_HEIGHT-PIP_POSITION_MARGIN_1080 -PIP_WINDOW_HEIGHT_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080, 
					DISPLAY_HEIGHT - PIP_POSITION_MARGIN_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080 - PIP_WINDOW_WIDTH_1080-1,	// 190905
					DISPLAY_HEIGHT-PIP_POSITION_MARGIN_1080 - PIP_WINDOW_HEIGHT_1080, 
					1, PIP_WINDOW_HEIGHT_1080);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 
					DISPLAY_WIDTH - PIP_POSITION_MARGIN_1080, 
					DISPLAY_HEIGHT-PIP_POSITION_MARGIN_1080-PIP_WINDOW_HEIGHT_1080, 
					1, PIP_WINDOW_HEIGHT_1080);

				MDINOSD_EnableBGBox(BGBOX_INDEX0, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX1, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX2, ON);
				MDINOSD_EnableBGBox(BGBOX_INDEX3, ON);
				break;

			case DISPLAY_MODE_PIP_D2:
			case DISPLAY_MODE_PIP_D3:
			case DISPLAY_MODE_PIP_D4:
				MDINOSD_SetBGBoxArea(BGBOX_INDEX0, 	// top
					PIP_POSITION_MARGIN_1080, 
					PIP_POSITION_MARGIN_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX1, 	// bottom
					PIP_POSITION_MARGIN_1080, 
					PIP_POSITION_MARGIN_1080+ PIP_WINDOW_HEIGHT_1080, 
					PIP_WINDOW_WIDTH_1080, 1);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX2, 	// left
					PIP_POSITION_MARGIN_1080-1, 			// 190905
					PIP_POSITION_MARGIN_1080, 
					1, PIP_WINDOW_HEIGHT_1080);
				MDINOSD_SetBGBoxArea(BGBOX_INDEX3, 	// right
					PIP_POSITION_MARGIN_1080 + PIP_WINDOW_WIDTH_1080, 
					PIP_POSITION_MARGIN_1080, 
					1, PIP_WINDOW_HEIGHT_1080);

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
	//prevMode = displayMode;
	}
}

void ResetVideoModeDisplayTime(eChannel_t channel)
{
	videoModeDisplayCount[channel] = VIDEO_MODE_DISPLAY_TIME;
}
void ClearVideoModeDisplayTime(eChannel_t channel)
{
	videoModeDisplayCount[channel] = 0;
}

