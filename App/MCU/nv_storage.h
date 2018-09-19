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

#define NV_VERSION_MAJOR				(uint8_t)0x01
#define NV_VERSION_MINOR				(uint8_t)0x00
#define NV_VERSION						(NV_VERSION_MAJOR << 8) | NV_VERSION_MINOR

#define FW_VERSION_MAJOR				(uint8_t)0x10	// develop version uses MSB 4bit, release version uses LSB 8bit
#define FW_VERSION_MINOR				(uint8_t)0x00
#define FW_VERSION						(FW_VERSION_MAJOR << 8) | FW_VERSION_MINOR

#define NV_SUCCESS						TRUE
#define NV_FAIL							FALSE

#define CHANNEL_NEME_LENGTH_MAX			12

//=============================================================================
//  Type definitions - enum, struct
//=============================================================================

typedef enum
{
	NV_ITEM_START_CHECK = 0,
	NV_ITEM_NV_VERSION,
	NV_ITEM_FW_VERSION,
//-- user setting data (MENU) --------------------------------------------------
	NV_ITEM_TIME_CORRECT,
	NV_ITEM_DATE_FORMAT,
	NV_ITEM_YEAR_FORMAT,
	NV_ITEM_TIME_ON,
	NV_ITEM_DATE_ON,
//	NV_ITEM_TIME_POSITION,
	NV_ITEM_CHANNEL_NAME,
	NV_ITEM_TITLE_DISPLAY_ON,
	NV_ITEM_TITLE_POSITION,
	NV_ITEM_AUTO_SEQ_TIME,
	NV_ITEM_AUTO_SEQ_LOSS_SKIP,
	NV_ITEM_OUTPUT_RESOLUTION,
	NV_ITEM_OSD_DISPLAY,
	NV_ITEM_BORDER_LINE,
	NV_ITEM_USER_ALARM_OPTION,
	NV_ITEM_USER_ALARMOUT_TIME,
	NV_ITEM_USER_ALARM_BUZZER_TIME,
	NV_ITEM_VIDEO_LOSS_ALARM_ON,
	NV_ITEM_VIDEO_LOSS_BUZZER_TIME,
	NV_ITEM_VIDEO_LOSS_DISPLAY_ON,
	NV_ITEM_REMOCON_ID,
	NV_ITEM_ALARM_REMOCON_SELECT,
	NV_ITEM_MOTION_SENSITIVITY,
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
	TITLE_POSITION_TOP_LEFT,
	TITLE_POSITION_TOP_CENTER,
	TITLE_POSITION_TOP_RIGHT,
	TITLE_POSITION_BOTTOM_LEFT,
	TITLE_POSITION_BOTTOM_CENTER,
	TITLE_POSITION_BOTTOM_RIGHT,
	TITLE_POSITION_4SPILIT_CENTER,//CH1&CH2 --> center_bottom  / CH3&CH4 --> center_top
	TITLE_POSITION_MAX
} eTitlePosition_t;

//typedef enum
//{
//	TIME_POSITION_LEFT,
//	TIME_POSITION_CENTER,
//	TIME_POSITION_RIGHT,
//	TIME_POSITION_MAX
//} eTimePosition_t;

typedef enum
{
	DATE_FORMAT_YMD = 0, //ASIA
	DATE_FORMAT_MDY,	 // US
	DATE_FORMAT_DMY,	 //EURO
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
	RESOLUTION_1920_1080_60P,
	RESOLUTION_1920_1080_50P,
	RESOLUTION_MAX
} eResolution_t;

typedef enum
{
	ALARM_OPTION_OFF,
	ALARM_OPTION_NO,
	ALARM_OPTION_NC,
	ALARM_OPTION_MAX
} eAlarmOption_t;

typedef struct
{
	uint8_t 				timeCorrecOffset;
	eDirection_t 			timeCorrectDirection;
	eTimeUnit_t 			timeCorrectUint;
} sTimeCorrect_t;

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
	uint16_t				nvVersion;
	uint16_t				fwVersion;

	sTimeCorrect_t			timeCorrection;
	eDateFormat_t			dateFormat;
	uint8_t					yearFormat;
	BOOL			 		timeDisplayOn;
	BOOL					dateDisplayOn;
//	eTimePosition_t			timeDisplayPosition;
	uint8_t 				channelName[NUM_OF_CHANNEL][CHANNEL_NEME_LENGTH_MAX];
	BOOL 					titleDisplayOn;
	eTitlePosition_t 		titlePosition;
	uint8_t					autoSeqTime[NUM_OF_CHANNEL];
	BOOL					autoSeqLossSkip;
	eResolution_t 			outputResolution;
	BOOL					osdOn;
	BOOL					borderLineOn;
	eAlarmOption_t 			alarmOption[NUM_OF_CHANNEL];
	uint8_t 				alarmOutTime;
	uint8_t					alarmBuzzerTime;
	BOOL					videoLossAlarmOn[NUM_OF_CHANNEL];
	uint8_t					videoLossBuzzerTime;
	BOOL					videoLossDisplayOn;
	uint8_t 				remoconId;
	BOOL					alarm_remote_sel; //0 : alarm, 1: remocon
	uint8_t					motionSensitivity;

//	uint8_t baud_rate;			// baud_rate
	eDisplayMode_t			displayMode;

	uint32_t				storageEndCheck;
} sNvData_t;

//=============================================================================
//  Function Prototype
//=============================================================================
extern void StoreNvDataToStorage(void);
extern void LoadNvDataFromStorage(void);
extern void InitializeNvData(void);

extern BOOL	ReadNvItem(eNvItems_t item, void * pData, size_t size);
extern BOOL WriteNvItem(eNvItems_t item, void * pData, size_t size);

extern void Read_NvItem_TimeCorrect(sTimeCorrect_t *pData);
extern void Write_NvItem_TimeCorrect(sTimeCorrect_t data);
extern void Read_NvItem_VideoLossBuzzerTime(uint8_t *pData);
extern void Write_NvItem_VideoLossBuzzerTime(uint8_t data);
extern void Read_NvItem_AutoSeqTime(u8* pData);
extern void Write_NvItem_AutoSeqTime(u8* data);
extern void Read_NvItem_AutoSeqLossSkip(BOOL* pData);
extern void Write_NvItem_AutoSeqLossSkip(BOOL data);
extern void Read_NvItem_OsdOn(BOOL* pData);
extern void Write_NvItem_OsdOn(BOOL data);
extern void Read_NvItem_TitleDispalyOn(BOOL *pData);
extern void Write_NvItem_TitleDispalyOn(BOOL data);
extern void Read_NvItem_TitlePosition(eTitlePosition_t *pData);
extern void Write_NvItem_TitlePosition(eTitlePosition_t data);
extern void Read_NvItem_TimeDisplayOn(BOOL* pData);
extern void Write_NvItem_TimeDisplayOn(BOOL data);
extern void Read_NvItem_DateDisplayOn(BOOL* pData);
extern void Write_NvItem_DateDisplayOn(BOOL data);
//extern void Read_NvItem_TimePosition(eTimePosition_t *pData);
//extern void Write_NvItem_TimePosition(eTimePosition_t data);
extern void Read_NvItem_ChannelName(uint8_t* pData, eChannel_t channel);
extern void Write_NvItem_ChannelName(uint8_t* pData, eChannel_t channel);
extern void Read_NvItem_DateFormat(eDateFormat_t* pData);
extern void Write_NvItem_DateFormat(eDateFormat_t data);
extern void Read_NvItem_YearFormat(BOOL* pData);
extern void Write_NvItem_YearFormat(BOOL data);
extern void Read_NvItem_VideoLossDisplayOn(BOOL* pData);
extern void Write_NvItem_VideoLossDisplayOn(BOOL data);
extern void Read_NvItem_Resolution(eResolution_t *pData);
extern void Write_NvItem_Resolution(eResolution_t data);
extern void Read_NvItem_BorderLineDisplay(BOOL *pData);
extern void Write_NvItem_BorderLineDisplay(BOOL data);
extern void Read_NvItem_AlarmRemoconSelect(BOOL *pData);
extern void Write_NvItem_AlarmRemoconSelect(BOOL data);
extern void Read_NvItem_AlarmBuzzerTime(uint8_t *pData);
extern void Write_NvItem_AlarmBuzzerTime(uint8_t data);
extern void Read_NvItem_VideoLossBuzzerTime(uint8_t *pData);
extern void Write_NvItem_VideoLossBuzzerTime(uint8_t data);
extern void Read_NvItem_AlarmOption(eAlarmOption_t* pData, eChannel_t channel);
extern void Write_NvItem_AlarmOption(eAlarmOption_t data, eChannel_t channel);
extern void Read_NvItem_RemoconId(uint8_t *pData);
extern void Write_NvItem_RemoconId(uint8_t data);
extern void Read_NvItem_MotionSensitivity(uint8_t *pData);
extern void Write_NvItem_MotionSensitivity(uint8_t data);

#endif
