#ifndef __MAIN_H__
#define __MAIN_H__

#define SYSTEM_SETUP_MODE			1	//set-up menu
#define SYSTEM_NORMAL_MODE			0	//normal working

//#define ALARMOUT_REQUESTER_NONE		0x00
//#define ALARMOUT_REQUESTER_ALARM		0x01
//#define ALARMOUT_REQUESTER_MOTION	0x02

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

#define EVT_NONE 						0x00
#define EVT_NO_VIDOE					0x01
#define EVT_ALARM						0x02
#define EVT_MOTION						0x04
#define EVT_FREEZE						0x08

#if BD_NVP == NVP6158
extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);
#endif

extern void TurnOnOffAlarmLed(BOOL on_off);
//extern void TurnOnAlarmOut(u8 requester);
//extern void TurnOffAlarmOut(u8 requester);

#endif


