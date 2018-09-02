#ifndef __NV_STORAGE_H__
#define __NV_STORAGE_H__

#include "mdintype.h"


#define cSYSENV_vCORRECT_OFFSET 	1
#define cSYSENV_bVECTOR				2
#define cSYSENV_bCORRECT		    3
#define cSYSENV_vDATE_FORMAT		4
#define cSYSENV_bTIME_ON			5
#define cSYSENV_vTIME_Position		6
#define cSYSENV_vCH_NAME			7
#define cSYSENV_bTITLE_ON			120
#define cSYSENV_vDWELL				121
#define cSYSENV_bLossAutoSkip       	125
#define cSYSENV_bOSD_Display		126
#define cSYSENV_vOSD_Position		127
#define cSYSENV_border_line			128
#define cSYSENV_resolution			129
#define cSYSENV_baud_rate		    	130
#define cSYSENV_vREMOCON_ID		131
#define cSYSENV_vLoss_Time			132
#define cSYSENV_vLoss_Display		133
#define cSYSENV_b9Split_Mode		134
#define cSYSENV_vAlarm				135
#define cSYSENV_vAlarm_Display_Time	136
#define cSYSENV_alarm_remote_sel		137
#define cEEP_CHK					200


#define NVSTORAGE_START_CHECKSUM		0xA5A5A5A5
#define NVSTORAGE_END_CHECKSUM			0x5A5A5A5A

#define CHANNEL_NEME_LENGTH_MAX			12


typedef enum
{
	NV_STORAGE_START_CHECK = 0,
//------------------------------------------------------------------------------
	NV_STORAGE_TIME_CORRECT_OFFSET, //cSYSENV_vCORRECT_OFFSET
	NV_STORAGE_TIME_CORRECT_DIRECTION,
	NV_STORAGE_TIME_CORRECT_UNIT,
	NV_STORAGE_DATE_FORMAT,
	NV_STORAGE_TIME_ON,
	NV_STORAGE_TIME_POSITION,
	NV_STORAGE_CHANNEL_NAME_1,
	NV_STORAGE_CHANNEL_NAME_2,
	NV_STORAGE_CHANNEL_NAME_3,
	NV_STORAGE_CHANNEL_NAME_4,
	NV_STORAGE_TITLE_DISPLAY,
	NV_STORAGE_AUTO_SKIP,
	NV_STORAGE_OSD_DISPLAY,
	NV_STORAGE_OSD_POSITION,
	NV_STORAGE_BORDER_LINE,
	NV_STORAGE_RESOLUTION,
	NV_STORAGE_BAUD_RATE,
	NV_STORAGE_REMOCON_ID,
	NV_STORAGE_LOSS_TIME,
	NV_STORAGE_LOSS_DISPLAY,
	NV_STORAGE_CHANNEL_ALARM_1,
	NV_STORAGE_CHANNEL_ALARM_2,
	NV_STORAGE_CHANNEL_ALARM_3,
	NV_STORAGE_CHANNEL_ALARM_4,
	NV_STORAGE_ALARMOUT_TIME,
	NV_STORAGE_ALARM_BUZZER_TIME,
	NV_STORAGE_ALARM_REMOCON_SELECT,
//------------------------------------------------------------------------------
	NV_STORAGE_END_CHECK,
	NV_STORAGE_MAX
} eNvStorageItems_t;

typedef struct
{
	eNvStorageItems_t	item;
	u8					size;
} sNvStorageItemInfo_t;

typedef enum
{
	TIME_POSISION_CENTER,
	TIME_POSISION_LEFT,
	TIME_POSISION_RIGHT,
} eTimeDisplayPositon_t;

typedef enum
{
	DATE_FORMAT_YMD = 0,
	DATE_FORMAT_MDY,
	DATE_FORMAT_DMY,
	DATE_FORMAT_MAX
} eDateFormat_t;

#define PLUS			UP
#define MINUS			DOWN

typedef struct
{
	u8 						timeCorrecOffset;
	BOOL 					timeCorrectDirection;	// +,-
	u8 						timeCorrectUint;		// SEC/DAY
	eDateFormat_t			dateFormat;
	BOOL			 		timeDisplyOn;			// �ð�ǥ�� ON/OFF
	eTimeDisplayPositon_t	timeDisplayPosition;  	// ȭ�鿡 ǥ�õǴ� �ð��� ��ġ

	u8 						channelName[NUM_OF_CHANNEL][CHANNEL_NEME_LENGTH_MAX];
	u8 bTITLE_ON;		 	// ä�θ� ǥ�� ON/OFF

//	u8 vDWELL[3];	    	// SEQ Dwell time(FULL,4split,9split)
	u8 bLossAutoSkip;   	// �Է� ���� ä�� SEQ ���� ����

	u8 vResolution;			// ����� �ػ�
	u8 bOSD_Display;		// ȭ�鿡 ���� ǥ�� ON/OFF
	u8 vOSD_Position;		// ���� ��ġ
	u8 border_line;			// �������� ����

	u8 vAlarm;
	u8 vAlarm_Display_Time; // �˶� ��� ǥ�� ���ӽð�

	u8 vREMOCON_ID;			// Serial Key ���� ID
	u8 baud_rate;			// baud_rate
	u8 vLoss_Time;			// Video Loss �� ������ ���ӽð�
	u8 vLoss_Display;		// Video Loss Display ǥ�� ON/OFF

	u8 b9Split_Mode;    	// 9���� ��忡�� 9����,8���� ����
	u8 alarm_remote_sel;

//    unsigned short crc;
}sys_env_t;


const struct nv_storage_items [NV_STORAGE_MAX] =
{
//		 item(name)								size
		{NV_STORAGE_START_CHECK,				sizeof(u32)},
		{NV_STORAGE_TIME_CORRECT_OFFSET,		sizeof(u8)},
		{NV_STORAGE_VECTOR,						sizeof(u8)},
		{NV_STORAGE_CORRECT,					sizeof(u8)},
		{NV_STORAGE_DATE_FORMAT,				sizeof(u8)},
		{NV_STORAGE_TIME_ON,					sizeof(BOOL)},
		{NV_STORAGE_TIME_POSITION,				sizeof(eTimeDisplayPositon_t)},
		{NV_STORAGE_CHANNEL_NAME_1,				CAMERA_NEME_LENGTH_MAX},
		{NV_STORAGE_CHANNEL_NAME_2,				CAMERA_NEME_LENGTH_MAX},
		{NV_STORAGE_CHANNEL_NAME_3,				CAMERA_NEME_LENGTH_MAX},
		{NV_STORAGE_CHANNEL_NAME_4,				CAMERA_NEME_LENGTH_MAX},
		{NV_STORAGE_TITLE_DISPLAY,				sizeof(BOOL)},
		{NV_STORAGE_AUTO_SKIP					,
		{NV_STORAGE_OSD_DISPLAY,
		{NV_STORAGE_OSD_POSITION,
		{NV_STORAGE_BORDER_LINE,
		{NV_STORAGE_RESOLUTION,
		{NV_STORAGE_BAUD_RATE,
		{NV_STORAGE_REMOCON_ID,
		{NV_STORAGE_LOSS_TIME,
		{NV_STORAGE_LOSS_DISPLAY,
		{NV_STORAGE_CHANNEL_ALARM_1,
		{NV_STORAGE_CHANNEL_ALARM_2,
		{NV_STORAGE_CHANNEL_ALARM_3,
		{NV_STORAGE_CHANNEL_ALARM_4,
		{NV_STORAGE_ALARMOUT_TIME,
		{NV_STORAGE_ALARM_BUZZER_TIME,
		{NV_STORAGE_ALARM_REMOCON_SELECT,

} sNvStorage_t;



extern void write_eeprom_all(void);
extern void read_eeprom_all(void);



#endif
