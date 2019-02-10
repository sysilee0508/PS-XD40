#ifndef __KEY_H__
#define __KEY_H__

//=============================================================================
//  MACRO
//=============================================================================

//=============================================================================
//  define
//=============================================================================
#define KEY_LONG			0x80

//=============================================================================
//  typedef
//=============================================================================
typedef enum KeyMode
{
	KEY_MODE_SHORT = 0x00,
	KEY_MODE_LONG,
	KEY_MODE_MAX
} eKeyMode_t;

typedef enum
{
	KEY_NONE 			= 0x00,
//----Short Keys-------------------------------------	
	KEY_FULL_CH1		= 0x01,
	KEY_FULL_CH2,
	KEY_SPLIT,
//----Long Keys--------------------------------------
	KEY_SPLIT_LONG		= KEY_SPLIT | KEY_LONG,	//0x99
	KEY_MAX				= 0xFF
} eKeyData_t;

typedef enum
{
	KEY_STATUS_PRESSED = 0,
	KEY_STATUS_RELEASED
} eKeyStatus_t;

typedef u8 keycode_t;


//=============================================================================
//  Function Prototype
//=============================================================================
extern void Key_Scan(void);
extern void Key_Check(void);
extern void Key_Proc(void);
//extern void Key_Led_Ctrl(void);

// Interface
extern void SetKeyMode(eKeyMode_t mode);
extern eKeyMode_t GetKeyMode(void);
extern void UpdateKeyStatus(eKeyStatus_t status);
extern eKeyStatus_t GetKeyStatus(void);
extern void SetKeyReady(void);
extern void ClearKeyReady(void);
extern BOOL IsKeyReady(void);
extern void UpdateKeyData(eKeyData_t key);
extern eKeyData_t GetCurrentKey(void);
extern BOOL IsScreenFreeze(void);
#endif
