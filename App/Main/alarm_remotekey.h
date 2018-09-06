#ifndef __ALARM_REMOTEKEY_H__
#define __ALARM_REMOTEKEY_H__

//=============================================================================
//  MACRO
//=============================================================================

#define SPI_DELAY					Delay_us(1)
//=============================================================================
//  define
//=============================================================================
#define ALARM_MODE					0
#define REMOTEKEY_MODE				1

#define ALARM_SET					1
#define ALARM_CLEAR					0

#define SPI_MISO_HIGH				0x00000001
#define SPI_MISO_LOW				0x00000000

//=============================================================================
//  enum
//=============================================================================
typedef enum
{
	ALARM_OPTION_OFF = 0,
	ALARM_OPTION_NO,	//active low
	ALARM_OPTION_NC,	//active high
	ALARM_OPTION_MAX
} eAlarmOption_t;

//=============================================================================
//  struct
//=============================================================================
typedef struct
{
	eAlarmOption_t option;
	BOOL alarm_status;
	BOOL raw_data;
	BOOL previous_data;
	u8 check_count;
} sAlarmInfo_t;

//=============================================================================
//  typedef
//=============================================================================

//=============================================================================
//  Extern Grobal Variable
//=============================================================================

//=============================================================================
//  Function Prototype
//=============================================================================
extern BYTE GetAlarmRemoteKeyMode(void);
extern void ChangeAlarmRemoteKeyMode(BYTE mode);
extern eAlarmOption_t GetAlarmOption(eChannel_t channel);
extern void SetAlarmOption(eChannel_t channel, eAlarmOption_t option);
extern void StartStopAlarm(BOOL start_stop);
extern void CheckAlarm(void);
extern void CheckAlarmClearCondition(void);
extern void ClearAllAlarm(void);
#endif
