#ifndef __KEY_H__
#define __KEY_H__

//=============================================================================
//  MACRO
//=============================================================================
#define KEY_LED_ON(led)		(GPIOB->BRR = (~led) << 12)

//=============================================================================
//  define
//=============================================================================
//#define DISP_OFF_KEY	0x0D
//#define FRONT_KEY		0x02
//#define BACK_KEY		0x04
//#define LEFT_CAM_KEY	0x05
//#define RIGHT_CAM_KEY	0x06
//#define AUX1_KEY		0x07
//#define AUX2_KEY		0x08
//#define CAM1_KEY		0x09
//#define CAM2_KEY		0x0A
//#define SPLIT_KEY		0x0B
//#define MENU_KEY		0x0E

// Key Codes
#define KEYCODE_NONE		0xFF	//1111 1111
// Left 4keys
#define KEYCODE_CAM1		0xFE	//1111 1110
#define KEYCODE_CAM2        0xFD	//1111 1101
#define KEYCODE_CAM3		0xFB	//1111 1011
#define KEYCODE_CAM4		0xF7	//1111 0111
// Right 3keys
#define KEYCODE_SPLIT		0xEF	//1110 1111
#define KEYCODE_FREEZE		0xDF	//1101 1111
#define KEYCODE_SEQUENCE	0xBF	//1011 1111

#define KEYLED_STAGE_LEFT	0		// krow0 && led1
#define	KEYLED_STAGE_RIGHT	1		// krow1 && led0

#define KEY_FULL_CH1	0x01
#define KEY_FULL_CH2	0x02
#define KEY_FULL_CH3	0x03
#define KEY_FULL_CH4	0x04
#define KEY_FULL_CH5	0x05
#define KEY_FULL_CH6	0x06
#define KEY_FULL_CH7	0x07
#define KEY_FULL_CH8	0x08
#define KEY_FULL_CH9	0x09
#define KEY_4SPLIT		0x15
#define KEY_FREEZE		0x19
#define KEY_SEQ			0x1a
#define KEY_9SPLIT		0x16
#define KEY_MENU		0x1c// KEY_FREEZE+0x80

//=============================================================================
//  typedef
//=============================================================================
typedef enum key_state
{
	KEY_STATE_SHORT = 0x00,
	KEY_STATE_LONG,
	KEY_STATE_REPEAT,
	KEY_STATE_MAX
} key_state_e;

typedef u8 keycode_t;
//
//#define STAT_KEY_SHORT      0x00
//#define KEY_STATE_LONG       0x01
//#define KEY_STATE_REPEAT     0x02


//#define LED_STAT_4SPLIT		0xfeffffff
//#define LED_STAT_9SPLIT		0xfdffffff
//#define LED_STAT_FREEZE		0xfbffffff
//#define LED_STAT_SEQ		0xf7ffffff

//#define LED_STAT_16SPLIT	0xffffffff
//#define LED_STAT_20SPLIT	0xffffffff
//
//#define QUEUE_SIZE	10
//#define NEXT(index)	((index+1)%QUEUE_SIZE)
//
//typedef struct Queue
//{
//	int buf[QUEUE_SIZE];
//	int front;
//	int rear;
//}Queue;

//=============================================================================
//  Function Prototype
//=============================================================================
//extern void EXTI9_5_IRQHandler(void);
extern void Key_LED_Set(void);
extern void Key_Scan(void);
extern void Key_Check(void);
extern void Key_Proc(void);
extern void Key_Led_Ctrl(keycode_t led);


//=============================================================================
//  Extern Grobal Variable 
//=============================================================================
extern key_state_e key_state;
extern u8 key_flag;
extern u8 key_data;
extern u8 pre_key_data;

//extern u16 led_state;


extern u8 bFreeze;

extern u8 pre_special_mode;
extern u8 pre_split_mode;

#endif
