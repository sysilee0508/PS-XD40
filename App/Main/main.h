#ifndef __MAIN_H__
#define __MAIN_H__

//#define __9CH__
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
//  각종 시스템 상태 변수
//-----------------------------------------------------------------------------
typedef struct
{
//	unsigned char output_resolution;		// 모니터 출력 해상도
//	unsigned int  output_h_size;			// 출력해상도 가로 크기
//	unsigned int  output_v_size;			// 출력행상도 세로 크기

	unsigned char current_mode;				// 현재 상태 (보통 분할 모드, 메뉴모드)
	unsigned char current_split_mode;		// 현재 분할모드 상태 (FULL, 4분할, 9분할)
	
} sys_stat_t;


typedef struct
{
//	u8 bVideo_format;		// 비디오 입력 포맷(NTSC, PAL)
//	SCR_MODE vSCR_MODE; 	// 모니터 해상도

	u8 vCORRECT_OFFSET; 	// RTC 시간 보정 offset값
	u8 bVECTOR;				// RTC 보정 +, - 설정
	u8 bCORRECT;			// RTC 시간 보정 기준 day, mon
	u8 vDATE_FORMAT;		// 아시아, 미국, 유럽 방식으로 시간 표시
//	u8 bYEAR_FORMAT;		// 연도 표시를 4자리로 할지 2자리로 할지 설정
	u8 bTIME_ON;			// 시간표시 ON/OFF
//	u8 vTIME_Size;      	// 화면에 표시되는 시간의 글자 크기 
	u8 vTIME_Position;  	// 화면에 표시되는 시간의 위치  

	u8 vCH_NAME[9][12]; 	// 채널명 저장
	u8 bTITLE_ON;		 	// 채널명 표시 ON/OFF

	u8 vDWELL[3];	    	// SEQ Dwell time(FULL,4split,9split)
	u8 bLossAutoSkip;   	// 입력 없는 채널 SEQ 제외 설정

	u8 vResolution;			// 모니터 해상도
//	u8 vSPOT_type;			// SPOT type 지정 FULL, QUAD 선택 
//	u8 vSPOT_mode_FULL[4]; 	// SPOT 타입이 FULL일때 mode 
//	u8 vSPOT_mode_QUAD[4]; 	// SPOT 타입이 QUAD일때 mode 
	u8 bOSD_Display;		// 화면에 글자 표시 ON/OFF
//	u8 vOSD_Size;	    	// 글자 크기 지정 0:소 1:중 2:대
	u8 vOSD_Position;		// 글자 위치
	u8 border_line;			// 보더라인 유무
	
//	u16 vMOTION_EN;			// 모션 ON/OFF		
//	u8 bMotion_Mode;		// 모션 모드(FULL, SPLIT)	
//	u16 vAlarm;				// 알람 	
//	u8 vAlarm_Display_Time; // 알람 출력 표시 지속시간	

	u8 vREMOCON_ID;			// Serial Key 구분 ID
	u8 baud_rate;			// baud_rate
//	u8 bLang_sel;			// 언어 종류(0:영어, 1:한글)
	u8 vLoss_Time;			// Video Loss 시 부저음 지속시간 
	u8 vLoss_Display;		// Video Loss Display 표시 ON/OFF 

	u8 b9Split_Mode;    	// 9분할 모드에서 9분할,8분할 선택


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


