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

#define ALARM_CLEAR					0
#define ALARM_DEBOUNCE				1
#define ALARM_SET					2

#define SPI_MISO_HIGH				0x00000001
#define SPI_MISO_LOW				0x00000000

#define REMOCON_ID_NONE 				0
#define REMOCON_ID_MAX					16

#define ALARM_OUT_CLEAR					0
#define ALARM_OUT_READY					1
#define ALARM_OUT_SET						2

//=============================================================================
//  enum
//=============================================================================
enum
{
	VIRTUAL_KEY_NONE     = 0x00,
	VIRTUAL_KEY_CH1	     = 0x80,
	VIRTUAL_KEY_CH2      = 0x81,
	VIRTUAL_KEY_CH3      = 0x82,
	VIRTUAL_KEY_CH4      = 0x83,
	VIRTUAL_KEY_SPLIT    = 0x84,
	VIRTUAL_KEY_MENU     = 0x8A,
	VIRTUAL_KEY_FREEZE   = 0x9A,
	VIRTUAL_KEY_AUTO_SEQ = 0x9B
};

enum
{
	UART_STATE_SOH = 0,
	UART_STATE_HEADER,
	UART_STATE_STX,
	UART_STATE_CODE,
	UART_STATE_ETX,
	UART_STATE_MAX
};

enum
{
	ERROR_NONE = 0,				// no error
	ERROR_INVALID_CONTROL, 		// soh, stx, etx error
	ERROR_REMOCONID_MISMATCH,	// remocon id is not matched
	ERROR_INVALID_REMOCONID,
	ERROR_INVALID_DATA			// data(code) is invalid
};
//=============================================================================
//  struct
//=============================================================================
typedef struct
{
	u8 alarm_status;
	BOOL raw_data;
	BOOL previous_data;
	u8 check_count;
	BOOL alarm_motion;
} sAlarmInfo_t;

//=============================================================================
//  typedef
//=============================================================================
typedef struct
{
	eKeyData_t	keydata;
	u8			virtual_key;
} sVirtualKeys_t;

//=============================================================================
//  Extern Grobal Variable
//=============================================================================

//=============================================================================
//  Function Prototype
//=============================================================================
extern BOOL CheckAlarmRemoteEnable(void);
extern BYTE ReadSpiDataByte(void);
extern void ChangeAlarmRemoteKeyMode(BYTE mode);
extern void CountDown_AlarmOutTimer(void);
extern void UpdateLastAlarmChannel(eChannel_t channel);
extern eChannel_t GetLastAlarmChannel(void);
extern eChannel_t FindAlarmChannel(void);
extern void AlarmOutState(eChannel_t alarmChannel, u8 state);
extern u8 GetAlarmStatus(eChannel_t channel);
extern void SetAlarmMotionStatus(eChannel_t channel, BOOL motion);
extern BOOL GetAlarmMotionStatus(eChannel_t channel);
extern void CheckAlarm(void);
extern void ChangeBaudrate(void);
extern u8 GetAlarmBuzzerCount(void);
extern void DecreaseAlarmBuzzerCount(void);
#endif
