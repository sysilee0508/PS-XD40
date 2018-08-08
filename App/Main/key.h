#ifndef __KEY_H__
#define __KEY_H__


//=============================================================================
//  define
//=============================================================================
#define DISP_OFF_KEY	0x0D
#define FRONT_KEY		0x02
#define BACK_KEY		0x04
#define LEFT_CAM_KEY	0x05
#define RIGHT_CAM_KEY	0x06
#define AUX1_KEY		0x07
#define AUX2_KEY		0x08
#define CAM1_KEY		0x09
#define CAM2_KEY		0x0A
#define SPLIT_KEY		0x0B
#define MENU_KEY		0x0E


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



#define STAT_KEY_SHORT      0x00
#define STAT_KEY_LONG       0x01
#define STAT_KEY_REPEAT     0x02


#define LED_STAT_4SPLIT		0xfeffffff
#define LED_STAT_9SPLIT		0xfdffffff
#define LED_STAT_FREEZE		0xfbffffff
#define LED_STAT_SEQ		0xf7ffffff

#define LED_STAT_16SPLIT	0xffffffff
#define LED_STAT_20SPLIT	0xffffffff

#define QUEUE_SIZE	10
#define NEXT(index)	((index+1)%QUEUE_SIZE)

typedef struct Queue
{
	int buf[QUEUE_SIZE];
	int front;
	int rear;
}Queue;

//=============================================================================
//  Function Prototype
//=============================================================================
void EXTI9_5_IRQHandler(void);
void Key_LED_Set(void);
void Key_Input(void);
void Key_Check(void);	
void Key_Proc(void);
void Key_Led_Ctrl(void);


//=============================================================================
//  Extern Grobal Variable 
//=============================================================================
extern u8 key_stat;
extern u8 key_flag;
extern u8 key_data;
extern u8 pre_key_data;

//extern u32 led_state;
extern u16 led_state;


extern u8 bFreeze;

extern u8 pre_special_mode;
extern u8 pre_split_mode;

#endif
