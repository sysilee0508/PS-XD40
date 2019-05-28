#ifndef __MAIN_H__
#define __MAIN_H__

#define SYSTEM_SETUP_MODE			1	//set-up menu
#define SYSTEM_NORMAL_MODE			0	//normal working

#define ALARMOUT_REQUESTER_NONE		0x00
#define ALARMOUT_REQUESTER_ALARM		0x01
#define ALARMOUT_REQUESTER_MOTION	0x02

#define CHANNEL_NONE					0xFF
//-----------------------------------------------------------------------------
//  ���� �ý��� ���� ����
//-----------------------------------------------------------------------------
#define NUM_OF_CHANNEL 				4
typedef enum
{
	CHANNEL1 = 0,
	CHANNEL2,
	CHANNEL3,
	CHANNEL4
} eChannel_t;

typedef enum
{
	SPLIT_2H_SCALE = 0,
	SPLIT_2H_CROP,
	SPLIT_2V_SCALE,
	SPLIT_2V_CROP,
	SPLIT_QUAD,
	SPLIT_QUAD_R3_SCALE,
	SPLIT_QUAD_R3_CROP,
	SPLIT_QUAD_L3_SCALE,
	SPLIT_QUAD_L3_CROP,
	SPLIT_QUAD_D3_SCALE,
	SPLIT_QUAD_D3_CROP,
	SPLIT_QUAD_U3_SCALE,
	SPLIT_QUAD_U3_CROP,
	NUM_OF_SPLIT	//13
} eSplit_t;

typedef enum
{
	PIP_A2 = 0,
	PIP_A3,
	PIP_A4,
	PIP_B2,
	PIP_B3,
	PIP_B4,
	PIP_C2,
	PIP_C3,
	PIP_C4,
	PIP_D2,
	PIP_D3,
	PIP_D4,
	NUM_OF_PIP	//12
} ePip_t;
extern s8 videoOutResolution;

extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);

extern void TurnOnAlarmOut(u8 requester);
extern void TurnOffAlarmOut(u8 requester);

#endif


