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
#define KEYCODE_NONE		0x7F	//0111 1111
// Left 4keys
#define KEYCODE_CH1			0xFE	//1111 1110
#define KEYCODE_CH2			0xFD	//1111 1101
#define KEYCODE_CH3			0xFB	//1111 1011
#define KEYCODE_CH4			0xF7	//1111 0111
// Right 3keys
#define KEYCODE_SPLIT		0xEF	//1110 1111
#define KEYCODE_FREEZE		0xDF	//1101 1111
#define KEYCODE_SEQUENCE	0xBF	//1011 1111

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
} key_mode_e;

typedef enum
{
	KEY_NONE 			= 0x00,
//----Short Keys-------------------------------------	
	KEY_FULL_CH1		= 0x01,
	KEY_FULL_CH2,
	KEY_FULL_CH3,
	KEY_FULL_CH4,
#ifdef __9CH_DEVICE__
	KEY_FULL_CH5,
	KEY_FULL_CH6,
	KEY_FULL_CH7,
	KEY_FULL_CH8,
	KEY_FULL_CH9,
#endif
	KEY_4SPLIT			= 0x15,
	KEY_9SPLIT			= 0x16,
	KEY_FREEZE			= 0x19,
	KEY_AUTO_SEQ		= 0x1A,
//----Long Keys--------------------------------------
	KEY_FREEZE_LONG		= KEY_FREEZE | KEY_LONG,	//0x99
	KEY_MENU			= KEY_FREEZE_LONG,
//---------------------------------------------------
	KEY_MAX				= 0xFF
} keydata_e;

typedef enum
{
	KEY_STATUS_PRESSED = 0,
	KEY_STATUS_RELEASED
} keystatus_e;

typedef u8 keycode_t;

enum
{
	KEYLED_STAGE_LEFT = 0,
	KEYLED_STAGE_RIGHT,
	KEYLED_STAGE_MAX
};

//=============================================================================
//  Extern Grobal Variable
//=============================================================================
//extern key_mode_e key_mode;
//extern keydata_e current_keydata;

//extern BOOL bScreenFreeze;

extern u8 pre_special_mode;

//=============================================================================
//  Function Prototype
//=============================================================================
extern void Key_Scan(void);
extern void Key_Check(void);
extern void Key_Proc(void);
extern void Key_Led_Ctrl(void);

// Interface
extern void SetKeyMode(key_mode_e mode);
extern key_mode_e GetKeyMode(void);
extern keycode_t GetKeyCode(keydata_e key);
extern void UpdateKeyStatus(keystatus_e status);
extern keystatus_e GetKeyStatus(void);
extern void SetKeyReady(void);
extern void ClearKeyReady(void);
extern BOOL IsKeyReady(void);
extern void UpdateKeyData(keydata_e key);
extern keydata_e GetCurrentKey(void);
extern BOOL IsScreenFreeze(void);
#endif
