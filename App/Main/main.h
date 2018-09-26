#ifndef __MAIN_H__
#define __MAIN_H__

#undef __9CH_DEVICE__
#define __4CH__

#define SYSTEM_SETUP_MODE			1	//set-up menu
#define SYSTEM_NORMAL_MODE			0	//normal working

//-----------------------------------------------------------------------------
//  ���� �ý��� ���� ����
//-----------------------------------------------------------------------------
typedef enum
{
	CHANNEL1 = 0,
	CHANNEL2,
	CHANNEL3,
	CHANNEL4,
	NUM_OF_CHANNEL
} eChannel_t;

#define CHANNEL_QUAD		NUM_OF_CHANNEL

typedef enum
{
	SPLITMODE_FULL_CH1 = 0x00,
	SPLITMODE_FULL_CH2,
	SPLITMODE_FULL_CH3,
	SPLITMODE_FULL_CH4,
	SPLITMODE_SPLIT4,
	SPLITMODE_MAX
} eSplitmode_t;

extern s8 Video1_In_Res_Val;
extern s8 Video2_In_Res_Val;
extern s8 Video_Out_Res_Val;

extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);

extern void vs4210_system_init(void);
extern void vs4210_display_proc(void);

#endif


