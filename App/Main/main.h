#ifndef __MAIN_H__
#define __MAIN_H__

#define SYSTEM_SETUP_MODE			1	//set-up menu
#define SYSTEM_NORMAL_MODE			0	//normal working

#define ALARMOUT_REQUESTER_NONE		0x00
#define ALARMOUT_REQUESTER_ALARM	0x01
#define ALARMOUT_REQUESTER_MOTION	0x02

//-----------------------------------------------------------------------------
//  ���� �ý��� ���� ����
//-----------------------------------------------------------------------------
typedef enum
{
	CHANNEL1 = 0,
	CHANNEL2,
	CHANNEL3,
	CHANNEL4,
	NUM_OF_CHANNEL,
	CHANNEL_SPLIT
} eChannel_t;

extern s8 Video1_In_Res_Val;
extern s8 Video2_In_Res_Val;
extern s8 Video_Out_Res_Val;

extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);

extern void vs4210_system_init(void);
extern void vs4210_display_proc(void);

#endif


