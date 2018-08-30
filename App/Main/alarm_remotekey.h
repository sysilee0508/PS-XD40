#ifndef __ALARM_REMOTEKEY_H__
#define __ALARM_REMOTEKEY_H__

//=============================================================================
//  MACRO
//=============================================================================
#define SPI_CS_HIGH					GPIOC->BSRR |= GPIO_Pin_3
#define SPI_CS_LOW					GPIOC->BRR |= GPIO_Pin_3
#define SPI_CLK_HIGH				GPIOC->BSRR |= GPIO_Pin_2
#define SPI_CLK_LOW					GPIOC->BRR |= GPIO_Pin_2
#define SPI_MISO_DATA				GPIOC->IDR & GPIO_Pin_0

#define SPI_DELAY					Delay_us(1)
//=============================================================================
//  define
//=============================================================================
#define ALARM_MODE					0
#define REMOTEKEY_MODE				1

#define ALARM_SET					1
#define ALARM_CLEAR					0

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
	u8 debounce_count;
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
extern void CheckAlarm(eChannel_t channel);
#endif
