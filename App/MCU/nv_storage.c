#include "common.h"
#include "stm32f10x_flash.h"
#include "nv_storage.h"
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static union uNvData
{
	BYTE buffer[FLASH_PAGE_SIZE];
	sNvData_t data;
} nv_data;

static sNvItemInfo_t nvInfo[NV_ITEM_MAX] =
{
//		item(name),						size of data,								dirty
		{NV_ITEM_START_CHECK,			sizeof(uint32_t),							CLEAR},
		{NV_ITEM_VERSION,				sizeof(uint32_t),							CLEAR},
		{NV_ITEM_TIME_CORRECT_OFFSET,	sizeof(uint8_t),							CLEAR},
		{NV_ITEM_TIME_CORRECT_DIRECTION,sizeof(eDirection_t),						CLEAR},
		{NV_ITEM_TIME_CORRECT_UNIT,		sizeof(eTimeUnit_t),						CLEAR},
		{NV_ITEM_DATE_FORMAT,			sizeof(eDateFormat_t),						CLEAR},
		{NV_ITEM_TIME_ON,				sizeof(BOOL),								CLEAR},
		{NV_ITEM_TIME_POSITION,			sizeof(eDisplayPositon_t),					CLEAR},
		{NV_ITEM_CHANNEL_NAME,			(NUM_OF_CHANNEL * CHANNEL_NEME_LENGTH_MAX),	CLEAR},
		{NV_ITEM_TITLE_DISPLAY_ON,		sizeof(BOOL),								CLEAR},
		{NV_ITEM_AUTO_SEQ_TIME,			sizeof(uint8_t),							CLEAR},
		{NV_ITEM_AUTO_SEQ_LOSS_SKIP,	sizeof(BOOL),								CLEAR},
		{NV_ITEM_OUTPUT_RESOLUTION,		sizeof(eResolution_t),						CLEAR},
		{NV_ITEM_OSD_DISPLAY,			sizeof(BOOL),								CLEAR},
		{NV_ITEM_OSD_POSITION,			sizeof(eDisplayPositon_t),					CLEAR},
		{NV_ITEM_BORDER_LINE,			sizeof(BOOL),								CLEAR},
		{NV_ITEM_USER_ALARM_OPTION, 	sizeof(eAlarmOption_t),						CLEAR},
		{NV_ITEM_USER_ALARMOUT_TIME,	sizeof(uint8_t),							CLEAR},
		{NV_ITEM_USER_ALARM_BUZZER_TIME,sizeof(uint8_t),							CLEAR},
		{NV_ITEM_VIDEO_LOSS_ALARM_ON,	NUM_OF_CHANNEL,								CLEAR},
		{NV_ITEM_VIDEO_LOSS_BUZZER_TIME,sizeof(uint8_t),							CLEAR},
		{NV_ITEM_VIDEO_LOSS_DISPLAY_ON,	sizeof(BOOL),								CLEAR},
		{NV_ITEM_REMOCON_ID,			sizeof(uint8_t),							CLEAR},
		{NV_ITEM_ALARM_REMOCON_SELECT,	sizeof(BOOL),								CLEAR},
		{NV_ITEM_DISPLAY_MODE,			sizeof(eDisplayMode_t),						CLEAR},
		{NV_ITEM_END_CHECK,				sizeof(uint32_t),							CLEAR}
};

//-----------------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------------
static void CreateDefaultChannelTitle(void)
{
	uint8_t index;

	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{
		strncpy(nv_data.data.channelName[index], "CAM", 3);
		nv_data.data.channelName[index][3] = '1'+i;
	}
}

static BOOL CheckNvStorage(void)
{
	BOOL result = FALSE;

	if((nv_data.data.storageStartCheck == NVSTORAGE_START_CHECK) &&
		(nv_data.data.storageEndCheck == NVSTORAGE_END_CHECK) &&
		(nv_data.data.version == NV_VERSION))
	{
		result = TRUE;
	}

	return result;
}

static BOOL CheckNvBufferDirty(void)
{
	uint8_t index;
	BOOL dirty = CLEAR;

	for(index = 0; index < NV_ITEM_MAX; index++)
	{
		dirty |= nvInfo[index].dirty;
	}

	return dirty;
}

static void LoadDefaultNvData(void)
{
	uint8_t index;
	//clear buffer
	memset(nv_data.buffer, 0x00, FLASH_PAGE_SIZE);

	// Make default nv data
	nv_data.data.storageStartCheck = NVSTORAGE_START_CHECK;
	nv_data.data.storageEndCheck = NVSTORAGE_END_CHECK;

	nv_data.data.version = NV_VERSION;

	nv_data.data.timeCorrecOffset = 0;
	nv_data.data.timeCorrectDirection = DIRECTION_UP;
	nv_data.data.timeCorrectUint = TIME_UNIT_SEC;
	nv_data.data.dateFormat = DATE_FORMAT_YMD;
	nv_data.data.timeDisplayOn = ON;
	nv_data.data.timeDisplayPosition = DISPLAY_POSITION_MIDDLE_BOTTOM;
	CreateDefaultChannelTitle();
//	nv_data.data.channelName[0] = "CAM1";
//	nv_data.data.channelName[1] = "CAM2";
//	nv_data.data.channelName[2] = "CAM3";
//	nv_data.data.channelName[3] = "CAM4";
	nv_data.data.titleDisplayOn = ON;
	nv_data.data.autoSeqTime = 3;
	nv_data.data.autoSeqLossSkip = ON;
	nv_data.data.outputResolution = RESOLUTION_1920_1080_60P;
	nv_data.data.osdOn = ON;
	nv_data.data.osdPosition = DISPLAY_POSITION_LEFT_BOTTOM;
	nv_data.data.borderLineOn = ON;
	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{
		nv_data.data.alarmOption[index] = ALARM_OPTION_NO;
	}
	nv_data.data.alarmOutTime = 3;
	nv_data.data.alarmBuzzerTime = 3;
	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{
		nv_data.data.videoLossAlarmOn[index] = OFF;
	}
	nv_data.data.videoLossBuzzerTime = 3;
	nv_data.data.videoLossDisplayOn = ON;
	nv_data.data.remoconId = 0;
	nv_data.data.alarm_remote_sel = 0;
	nv_data.data.displayMode = DISPLAY_MODE_4SPLIT;

	// set anyone of nv items dirty in order to write nv data to flash
	nvInfo[NV_ITEM_END_CHECK].dirty = SET;
	// Write nv data to Flash
	StoreNvDataToStorage();
}

//-----------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------

// Store/Load NV data to/from flash memory (storage)------------------------------
void StoreNvDataToStorage(void)
{
	uint32_t *pData = (uint32_t *)nv_data.buffer;
	uint8_t index;

	if(CheckNvBufferDirty() == SET)
	{
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP| FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ErasePage(NV_STORAGE_START_ADDR);
		for(index = 0; index < FLASH_PAGE_SIZE/sizeof(uint32_t); index++)
		{
			FLASH_ProgramWord(NV_STORAGE_START_ADDR+(index*sizeof(uint32_t)),*(pData+index));
		}
		FLASH_Lock();
	}
}

//--------------------------------------------------------------------------------
void LoadNvDataFromStorage(void)
{
	memcpy((void *)nv_data.buffer, (void *)NV_STORAGE_START_ADDR, FLASH_PAGE_SIZE);

	if(CheckNvStorage() == FALSE)
	{
		LoadDefaultNvData();
	}
}

// read or write each NV Item------------------------------------------------------
BOOL ReadNvItem(eNvItems_t item, void * pData)
{
	uint8_t	index;
	uint16_t offset = 0;
	BOOL result = NV_FAIL;

	if(TRUE == CheckNvStorage())
	{
		for(index = 0; index < NV_ITEM_MAX; index++)
		{
			if(nvInfo[index].item < item)
			{
				offset += nvInfo[index].size;
			}
			else if(nvInfo[index].item == item)
			{
				break;
			}
		}
		if(index < NV_ITEM_MAX )
		{
			*pData = nv_data.buffer[offset];
			result = NV_SUCCESS;
		}
	}

	return result;
}

BOOL WriteNvItem(eNvItems_t item, void * pData, size_t size)
{
	uint8_t	index;
	uint16_t offset = 0;
	BOOL result = NV_FAIL;

	if(TRUE == CheckNvStorage())
	{
		for(index = 0; index < NV_ITEM_MAX; index++)
		{
			if(nvInfo[index].item < item)
			{
				offset += nvInfo[index].size;
			}
			else if(nvInfo[index].item == item)
			{
				break;
			}
		}
		if(index < NV_ITEM_MAX )
		{
			memcpy(&nv_data.buffer[offset], pData, size);
			nvInfo[index].dirty = SET;
			result = NV_SUCCESS;
		}
	}

	return result;
}
