#ifndef __MAIN_H__
#define __MAIN_H__

#undef __9CH_DEVICE__
#define __4CH__

#define SYSTEM_SETUP_MODE			1	//set-up menu
#define SYSTEM_NORMAL_MODE			0	//normal working

typedef enum
{
	CHANNEL1 = 0,
	CHANNEL2,
	CHANNEL3,
	CHANNEL4,
	NUM_OF_CHANNEL
} eChannel_t;

//-----------------------------------------------------------------------------
//  ���� �ý��� ���� ����
//-----------------------------------------------------------------------------
typedef enum
{
	SPLITMODE_FULL_CH1 = 0x00,
	SPLITMODE_FULL_CH2,
	SPLITMODE_FULL_CH3,
	SPLITMODE_FULL_CH4,
	SPLITMODE_SPLIT4,
	SPLITMODE_MAX
} eSplitmode_t;

typedef struct
{
	unsigned char current_mode;				// ���� ���� (���� ���� ���, �޴����)
	eSplitmode_t current_split_mode;		// ���� ���Ҹ�� ���� (FULL, 4����, 9����)
	
} sys_stat_t;


extern sys_stat_t sys_status;

//extern BYTE sysenv_out_resolution;

extern u32 tick_10ms;
extern s8 Video1_In_Res_Val;
extern s8 Video2_In_Res_Val;

extern s8 Video_Out_Res_Val;

extern const unsigned char change_mode[4];

extern unsigned long count_100us;

extern int cmode;

extern u8 aux_display_flag;

extern u32 vVideo_Loss;
extern u8 ch9_loss;
extern u8 Loss_Event_Flag;
extern u8 Loss_Buzzer_Cnt;


//extern BYTE sysenv_split_mode;

extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);

extern void vs4210_system_init(void);
extern void vs4210_display_proc(void);

#endif


