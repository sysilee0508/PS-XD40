#ifndef __NV_STORAGE_H__
#define __NV_STORAGE_H__

#include "main.h"
#include "mdintype.h"

//=============================================================================
//  Define
//=============================================================================
#define FLASH_PAGE_SIZE					0x800//2048 //page size is 2k
#define FLASH_START_ADDR				0x08000000
#define FLASH_END_ADDR					0x080FFFFF

#define FLASH_ADDRESS_PAGE(x)			x * FLASH_PAGE_SIZE + FLASH_START_ADDR
#define	NV_STORAGE_START_ADDR			FLASH_ADDRESS_PAGE(128)//0x080400000

#define NVSTORAGE_START_CHECK			0xA5A5A5A5
#define NVSTORAGE_END_CHECK				0x5A5A5A5A

#define NV_VERSION_MAJOR				(u16)0x0001
#define NV_VERSION_MINOR				(u16)0x0000
#define NV_VERSION						(NV_VERSION_MAJOR << 16) | NV_VERSION_MINOR

#define CHANNEL_NEME_LENGTH_MAX			12

#define NV_SUCCESS						TRUE
#define NV_FAIL							FALSE

//=============================================================================
//  Type definitions - enum, struct
//=============================================================================

typedef enum
{
	NV_ITEM_START_CHECK = 0,
	NV_ITEM_VERSION,
//-- user setting data (MENU) --------------------------------------------------
	NV_ITEM_TIME_CORRECT_OFFSET,
	NV_ITEM_TIME_CORRECT_DIRECTION,
	NV_ITEM_TIME_CORRECT_UNIT,
	NV_ITEM_DATE_FORMAT,
	NV_ITEM_TIME_ON,
	NV_ITEM_TIME_POSITION,
	NV_ITEM_CHANNEL_NAME,
	NV_ITEM_TITLE_DISPLAY_ON,
	NV_ITEM_AUTO_SEQ_TIME,
	NV_ITEM_AUTO_SEQ_LOSS_SKIP,
	NV_ITEM_OUTPUT_RESOLUTION,
	NV_ITEM_OSD_DISPLAY,
	NV_ITEM_OSD_POSITION,
	NV_ITEM_BORDER_LINE,
	NV_ITEM_USER_ALARM_OPTION,
	NV_ITEM_USER_ALARMOUT_TIME,
	NV_ITEM_USER_ALARM_BUZZER_TIME,
	NV_ITEM_VIDEO_LOSS_ALARM_ON,
	NV_ITEM_VIDEO_LOSS_BUZZER_TIME,
	NV_ITEM_VIDEO_LOSS_DISPLAY_ON,
	NV_ITEM_REMOCON_ID,
	NV_ITEM_ALARM_REMOCON_SELECT,
//-- system data ---------------------------------------------------------------
//	If you want to store any system data (item) in NV memory, it comes here
	NV_ITEM_DISPLAY_MODE,
//	NV_ITEM_INPUT_VIDEO_FORMAT,
//------------------------------------------------------------------------------
	NV_ITEM_END_CHECK,
	NV_ITEM_MAX
} eNvItems_t;

typedef enum
{
	DISPLAY_POSITION_LEFT_TOP,
	DISPLAY_POSITION_MIDDLE_TOP,
	DISPLAY_POSITION_RIGHT_TOP,
	DISPLAY_POSITION_LEFT_CENTER,
	DISPLAY_POSITION_MIDDLE_CENTER,
	DISPLAY_POSITION_RIGHT_CENTER,
	DISPLAY_POSITION_LEFT_BOTTOM,
	DISPLAY_POSITION_MIDDLE_BOTTOM,
	DISPLAY_POSITION_RIGHT_BOTTOM,
	DISPLAY_POSITION_MAX
} eDisplayPositon_t;

typedef enum
{
	DATE_FORMAT_YMD = 0,
	DATE_FORMAT_MDY,
	DATE_FORMAT_DMY,
	DATE_FORMAT_MAX
} eDateFormat_t;

typedef enum
{
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_MAX
} eDirection_t;

typedef enum
{
	TIME_UNIT_SEC = 0,
	TIME_UNIT_MIN,
	TIME_UNIT_HOUR,
	TIME_UNIT_DAY,
	TIME_UNIT_MONTH,
	TIME_UNIT_YEAR,
	TIME_UNIT_MAX
} eTimeUnit_t;

typedef enum
{
	DISPLAY_MODE_FULL_SCREEN,	// full screen mode for any channel
	DISPLAY_MODE_4SPLIT,		// 4 split screen
	DISPLAY_MODE_MAX
} eDisplayMode_t;

typedef enum
{
	RESOLUTION_1920_1080_50P,
	RESOLUTION_1920_1080_60P,
	RESOLUTION_MAX
} eResolution_t;

typedef enum
{
	ALARM_OPTION_OFF,
	ALARM_OPTION_NO,
	ALARM_OPTION_NC,
	ALARM_OPTION_MAX
} eAlarmOption_t;

//--------------------------------------------------------------------------------------
typedef struct
{
	const eNvItems_t	item;
	const size_t		size;
	BOOL		dirty;
} sNvItemInfo_t;

typedef struct
{
	uint32_t				storageStartCheck;
	uint32_t				version;

	uint8_t 				timeCorrecOffset;
	eDirection_t 			timeCorrectDirection;
	eTimeUnit_t 			timeCorrectUint;
	eDateFormat_t			dateFormat;
	BOOL			 		timeDisplayOn;
	eDisplayPositon_t		timeDisplayPosition;
	uint8_t 				channelName[NUM_OF_CHANNEL][CHANNEL_NEME_LENGTH_MAX];
	BOOL 					titleDisplayOn;
	uint8_t					autoSeqTime;
	BOOL					autoSeqLossSkip;
	eResolution_t 			outputResolution;
	BOOL					osdOn;
	eDisplayPositon_t 		osdPosition;
	BOOL					borderLineOn;
	eAlarmOption_t 			alarmOption[NUM_OF_CHANNEL];
	uint8_t 				alarmOutTime;
	uint8_t					alarmBuzzerTime;
	BOOL					videoLossAlarmOn[NUM_OF_CHANNEL];
	uint8_t					videoLossBuzzerTime;
	BOOL					videoLossDisplayOn;
	uint8_t 				remoconId;
	BOOL					alarm_remote_sel;

//	uint8_t baud_rate;			// baud_rate
	eDisplayMode_t			displayMode;

	uint32_t				storageEndCheck;
} sNvData_t;






//=============================================================================
//  Function Prototype
//=============================================================================
extern void StoreNvDataToStorage(void);
extern void LoadNvDataFromStorage(void);
extern BOOL	ReadNvItem(eNvItems_t item, void * pData);
extern BOOL WriteNvItem(eNvItems_t item, void * pData, size_t size);

#endif
