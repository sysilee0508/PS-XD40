#ifndef __MAIN_H__
#define __MAIN_H__

#undef __9CH_DEVICE__
#define __4CH__

#define cSYSENV_vCORRECT_OFFSET 	 1
#define cSYSENV_bVECTOR				 2
#define cSYSENV_bCORRECT		     3
#define cSYSENV_vDATE_FORMAT		 4
#define cSYSENV_bTIME_ON			 5
#define cSYSENV_vTIME_Position		 6
#define cSYSENV_vCH_NAME			 7	
#define cSYSENV_bTITLE_ON			 120	
#define cSYSENV_vDWELL				 121	
#define cSYSENV_bLossAutoSkip        125 
#define cSYSENV_bOSD_Display		 126
#define cSYSENV_vOSD_Position		 127
#define cSYSENV_border_line			 128
#define cSYSENV_resolution			 129
#define cSYSENV_baud_rate		     130
#define cSYSENV_vREMOCON_ID			 131	
#define cSYSENV_vLoss_Time			 132
#define cSYSENV_vLoss_Display		 133
#define cSYSENV_b9Split_Mode		 134
#define cEEP_CHK					 200

#define FULL_1		0x00
#define FULL_2		0x01
#define FULL_3		0x02
#define FULL_4		0x03
#define FULL_5		0x04
#define FULL_6		0x05
#define FULL_7		0x06
#define FULL_8		0x07
#define FULL_9		0x08
#define SPLIT4_1	0x09
#define SPLIT4_2	0x0A
#define SPLIT9_1	0x0B
#define SPLIT9_2	0x0C
#define SPLIT9_3	0x0D
#define SPLIT9_4	0x0E
#define SPLIT9_5	0x0F
#define SPLIT9_6	0x10
#define SPLIT9_7	0x11
#define SPLIT9_8	0x12
#define SPLIT9_9	0x13

#define NORMAL_VIEW	0
#define MENU_VIEW	1

//-----------------------------------------------------------------------------
//  ���� �ý��� ���� ����
//-----------------------------------------------------------------------------
typedef struct
{
//	unsigned char output_resolution;		// ����� ��� �ػ�
//	unsigned int  output_h_size;			// ����ػ� ���� ũ��
//	unsigned int  output_v_size;			// ������ ���� ũ��

	unsigned char current_mode;				// ���� ���� (���� ���� ���, �޴����)
	unsigned char current_split_mode;		// ���� ���Ҹ�� ���� (FULL, 4����, 9����)
	
} sys_stat_t;


typedef struct
{
//	u8 bVideo_format;		// ���� �Է� ����(NTSC, PAL)
//	SCR_MODE vSCR_MODE; 	// ����� �ػ�

	u8 vCORRECT_OFFSET; 	// RTC �ð� ���� offset��
	u8 bVECTOR;				// RTC ���� +, - ����
	u8 bCORRECT;			// RTC �ð� ���� ���� day, mon
	u8 vDATE_FORMAT;		// �ƽþ�, �̱�, ���� ������� �ð� ǥ��
//	u8 bYEAR_FORMAT;		// ���� ǥ�ø� 4�ڸ��� ���� 2�ڸ��� ���� ����
	u8 bTIME_ON;			// �ð�ǥ�� ON/OFF
//	u8 vTIME_Size;      	// ȭ�鿡 ǥ�õǴ� �ð��� ���� ũ�� 
	u8 vTIME_Position;  	// ȭ�鿡 ǥ�õǴ� �ð��� ��ġ  

	u8 vCH_NAME[9][12]; 	// ä�θ� ����
	u8 bTITLE_ON;		 	// ä�θ� ǥ�� ON/OFF

	u8 vDWELL[3];	    	// SEQ Dwell time(FULL,4split,9split)
	u8 bLossAutoSkip;   	// �Է� ���� ä�� SEQ ���� ����

	u8 vResolution;			// ����� �ػ�
//	u8 vSPOT_type;			// SPOT type ���� FULL, QUAD ���� 
//	u8 vSPOT_mode_FULL[4]; 	// SPOT Ÿ���� FULL�϶� mode 
//	u8 vSPOT_mode_QUAD[4]; 	// SPOT Ÿ���� QUAD�϶� mode 
	u8 bOSD_Display;		// ȭ�鿡 ���� ǥ�� ON/OFF
//	u8 vOSD_Size;	    	// ���� ũ�� ���� 0:�� 1:�� 2:��
	u8 vOSD_Position;		// ���� ��ġ
	u8 border_line;			// �������� ����
	
//	u16 vMOTION_EN;			// ��� ON/OFF		
//	u8 bMotion_Mode;		// ��� ���(FULL, SPLIT)	
//	u16 vAlarm;				// �˶� 	
//	u8 vAlarm_Display_Time; // �˶� ��� ǥ�� ���ӽð�	

	u8 vREMOCON_ID;			// Serial Key ���� ID
	u8 baud_rate;			// baud_rate
//	u8 bLang_sel;			// ��� ����(0:����, 1:�ѱ�)
	u8 vLoss_Time;			// Video Loss �� ������ ���ӽð� 
	u8 vLoss_Display;		// Video Loss Display ǥ�� ON/OFF 

	u8 b9Split_Mode;    	// 9���� ��忡�� 9����,8���� ����


//    unsigned short crc;
}sys_env_t;


extern sys_stat_t sys_status;
extern sys_env_t sys_env;

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


extern BYTE sysenv_split_mode;

extern void NVP6158_init(void);
extern void NVP6158_VideoDetectionProc(void);
extern void vs4210_system_init();


#endif


