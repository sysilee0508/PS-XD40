#ifndef __KEY_H__
#define __KEY_H__

//=============================================================================
//  MACRO
//=============================================================================
#define KEY_LED_ON(led)		(GPIOB->BRR = (~led) << 12)

//=============================================================================
//  define
//=============================================================================
// Key Codes
#define KEYCODE_NONE		0xFF	//0111 1111
//#define KEYCODE_NONE_BACK	0xFF
// Left 4keys
#define KEYCODE_CH1			0xFE	//1111 1110
#define KEYCODE_CH2			0xFD	//1111 1101
#define KEYCODE_CH3			0xFB	//1111 1011
#define KEYCODE_CH4			0xF7	//1111 0111
// Right 3keys
#define KEYCODE_SPLIT		0xEF	//1110 1111
#define KEYCODE_FREEZE		0xDF	//1101 1111
#define KEYCODE_SEQUENCE	0xBF	//1011 1111

#define KEY_SPECIAL			0x40
#define KEY_LONG			0x80

#define LEFT_TOP			0x0
//=============================================================================
//  typedef
//=============================================================================
typedef enum KeyMode
{
	KEY_MODE_SHORT = 0x00,
	KEY_MODE_LONG,
	KEY_MODE_REPEAT,
	KEY_MODE_MAX
} eKeyMode_t;

typedef enum
{
	KEY_NONE 			= 0x00,
//----Short Keys-------------------------------------	
	KEY_FULL_CH1		= 0x01,
	KEY_FULL_CH2,
	KEY_FULL_CH3,
	KEY_FULL_CH4,
	KEY_SPLIT			= 0x15,
	KEY_FREEZE			= 0x19,
	KEY_AUTO_SEQ		= 0x1A,
//----Function (Virtual) Keys-----------------------
	KEY_ALARM			= 0x21,
//----Menu Keys-------------------------------------
	KEY_LEFT			= KEY_FULL_CH1 | KEY_SPECIAL,
	KEY_UP				= KEY_FULL_CH2 | KEY_SPECIAL,
	KEY_DOWN			= KEY_FULL_CH3 | KEY_SPECIAL,
	KEY_RIGHT			= KEY_FULL_CH4 | KEY_SPECIAL,
	KEY_ENTER			= KEY_SPLIT | KEY_SPECIAL,
	KEY_EXIT			= KEY_FREEZE | KEY_SPECIAL,
//----Long Keys--------------------------------------
	KEY_FREEZE_LONG		= KEY_FREEZE | KEY_LONG,	//0x99
	KEY_MENU			= KEY_FREEZE_LONG,
	KEY_MAX				= 0xFF
} eKeyData_t;

typedef enum
{
	KEY_STATUS_PRESSED = 0,
	KEY_STATUS_RELEASED
} eKeyStatus_t;

typedef u8 keycode_t;

enum
{
	KEYLED_STAGE_LEFT = 0,
	KEYLED_STAGE_RIGHT,
	KEYLED_STAGE_MAX
};

enum
{
	LED_CH1 = 0,
	LED_CH2,
	LED_CH3,
	LED_CH4,
	LED_SPLIT,
	LED_FREEZE,
	LED_SEQUENCE,
	LED_NONE = 0xFF
};

//=============================================================================
//  Extern Grobal Variable
//=============================================================================
extern u8 pre_special_mode;
extern BOOL forceFreezeOn;

//=============================================================================
//  Function Prototype
//=============================================================================
extern void Key_Scan(void);
extern void Key_Scan_ParallelKey(void);
extern void Key_Check(void);
extern void Key_Proc(void);
extern void Key_Led_Ctrl(void);

// Interface
extern void TurnOnSelectedLed(BYTE selectedLed);
extern void SetKeyMode(eKeyMode_t mode);
extern eKeyMode_t GetKeyMode(void);
extern keycode_t GetKeyCode(eKeyData_t key);
extern void SetCurrentKeyCode(keycode_t keycode);
extern void UpdateKeyStatus(eKeyStatus_t status);
extern eKeyStatus_t GetKeyStatus(void);
extern void SetKeyReady(void);
extern void ClearKeyReady(void);
extern BOOL IsKeyReady(void);
extern void UpdateKeyData(eKeyData_t key);
extern eKeyData_t GetCurrentKey(void);
extern BOOL IsScreenFreeze(void);
extern void SetInitialKey(void);
#endif
