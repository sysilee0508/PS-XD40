#ifndef __MAIN_H__
#define __MAIN_H__

//-----------------------------------------------------------------------------
//  ���� �ý��� ���� ����
//-----------------------------------------------------------------------------
typedef enum
{
	CHANNEL1 = 0,
	CHANNEL2,
	NUM_OF_CHANNEL,
} eChannel_t;

typedef enum
{
	SPLIT_A = 0,
	SPLIT_B,
	SPLIT_C,
	SPLIT_D,
	SPLIT_E,
	NUM_OF_SPLIT
} eSplit_t;

typedef enum
{
	PIP_A = 0,
	PIP_B,
	PIP_C,
	PIP_D,
	NUM_OF_PIF
} ePip_t;
extern s8 videoOutResolution;

extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);

extern void TurnOnAlarmOut(u8 requester);
extern void TurnOffAlarmOut(u8 requester);

#endif


