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
//		item(name),							size of data,								dirty
		{NV_ITEM_START_CHECK,				sizeof(uint32_t),							CLEAR},
		{NV_ITEM_NV_VERSION,				sizeof(uint16_t),							CLEAR},
		{NV_ITEM_FW_VERSION,				sizeof(uint16_t),							CLEAR},
		{NV_ITEM_TIME_CORRECT,				sizeof(sTimeCorrect_t),						CLEAR},
		{NV_ITEM_DATE_FORMAT,				sizeof(eDateFormat_t),						CLEAR},
		{NV_ITEM_YEAR_FORMAT, 				sizeof(uint8_t),							CLEAR},
		{NV_ITEM_TIME_ON,					sizeof(BOOL),								CLEAR},
		{NV_ITEM_DATE_ON,					sizeof(BOOL),								CLEAR},
		{NV_ITEM_CHANNEL_NAME,				(NUM_OF_CHANNEL * (CHANNEL_NEME_LENGTH_MAX+1)),	CLEAR},
		{NV_ITEM_TITLE_DISPLAY_ON,			sizeof(BOOL),								CLEAR},
		{NV_ITEM_AUTO_SEQ_TIME,				NUM_OF_CHANNEL,								CLEAR},
		{NV_ITEM_AUTO_SEQ_LOSS_SKIP,		sizeof(BOOL),								CLEAR},
//		{NV_ITEM_OUTPUT_RESOLUTION,			sizeof(eResolution_t),						CLEAR},
		{NV_ITEM_OSD_DISPLAY,				sizeof(BOOL),								CLEAR},
		{NV_ITEM_BORDER_LINE,				sizeof(BOOL),								CLEAR},
		{NV_ITEM_USER_ALARM_OPTION, 		sizeof(eAlarmOption_t),						CLEAR},
		{NV_ITEM_USER_ALARMOUT_TIME,		sizeof(uint8_t),							CLEAR},
		{NV_ITEM_USER_ALARM_BUZZER_TIME,	sizeof(uint8_t),							CLEAR},
		{NV_ITEM_VIDEO_LOSS_ALARM_ON,		NUM_OF_CHANNEL,								CLEAR},
		{NV_ITEM_VIDEO_LOSS_BUZZER_TIME,	sizeof(uint8_t),							CLEAR},
		{NV_ITEM_VIDEO_LOSS_DISPLAY_ON,		sizeof(BOOL),								CLEAR},
		{NV_ITEM_REMOCON_ID,				sizeof(uint8_t),							CLEAR},
		{NV_ITEM_ALARM_REMOCON_SELECT,		sizeof(BOOL),								CLEAR},
		{NV_ITEM_MOTION_DETECT_ON,			sizeof(BOOL),								CLEAR},
		{NV_ITEM_MOTION_SENSITIVITY,		sizeof(uint8_t),							CLEAR},
		{NV_ITEM_MOTION_DETECT_BLOCK,       sizeof(uint16_t)*NUM_OF_CHANNEL*ROWS_OF_BLOCKS, CLEAR},
		{NV_ITEM_SERIAL_BAUDRATE,			sizeof(eBaudRate_t),						CLEAR},
		{NV_ITEM_DISPLAY_MODE, 				sizeof(eDisplayMode_t),							CLEAR},
		{NV_ITEM_SPLIT_MODE,				sizeof(eDisplayMode_t),						CLEAR},
		//{NV_ITEM_DISPLAY_CHANNEL,			sizeof(eChannel_t),							CLEAR},
		{NV_ITEM_END_CHECK,					sizeof(uint32_t),							CLEAR}
};

static sSystemStatusData_t systemStatus;

//-----------------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------------
static void CreateDefaultChannelTitle(void)
{
	uint8_t index;

	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{
		strncpy((char *)&(nv_data.data.channelName[index]), "CAM", 3);
		nv_data.data.channelName[index][3] = '1'+index;
	}
}

static BOOL CheckNvStorage(void)
{
	BOOL result = FALSE;

	if((nv_data.data.storageStartCheck == NVSTORAGE_START_CHECK) &&
		(nv_data.data.storageEndCheck == NVSTORAGE_END_CHECK) &&
		(nv_data.data.nvVersion.major == NV_VERSION_MAJOR) &&
		(nv_data.data.nvVersion.minor == NV_VERSION_MINOR))
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

static void ClearNvBufferDirty(void)
{
	uint8_t index;

	for(index = 0; index < NV_ITEM_MAX; index++)
	{
		nvInfo[index].dirty = CLEAR;
	}
}

static void LoadDefaultNvData(void)
{
	uint8_t index;
	//clear buffer
	memset(nv_data.buffer, 0x00, FLASH_PAGE_SIZE);

	// Make default nv data
	nv_data.data.storageStartCheck = NVSTORAGE_START_CHECK;
	nv_data.data.storageEndCheck = NVSTORAGE_END_CHECK;

	nv_data.data.nvVersion.major = NV_VERSION_MAJOR;
	nv_data.data.nvVersion.minor = NV_VERSION_MINOR;
	nv_data.data.fwVersion.major = FW_VERSION_MAJOR;
	nv_data.data.fwVersion.minor = FW_VERSION_MINOR;
       
	nv_data.data.timeCorrection.timeCorrectOffset = 0;
	nv_data.data.timeCorrection.timeCorrectDirection = DIRECTION_UP;
	nv_data.data.timeCorrection.timeCorrectUint = TIME_UNIT_DAY;
	nv_data.data.dateFormat = DATE_FORMAT_YMD;
	nv_data.data.yearFormat = 1; //4 // 4 digit
	nv_data.data.timeDisplayOn = ON;
	nv_data.data.dateDisplayOn = ON;       
	CreateDefaultChannelTitle();
	nv_data.data.titleDisplayOn = ON;
	for(index = CHANNEL1; index < NUM_OF_CHANNEL; index++)
	{
		nv_data.data.autoSeqTime[index] = 7;
	}
	nv_data.data.autoSeqLossSkip = ON;
	//nv_data.data.outputResolution = RESOLUTION_1920_1080_60P;
	nv_data.data.osdOn = ON;
	nv_data.data.borderLineOn = ON;
	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{
		nv_data.data.alarmOption[index] = ALARM_OPTION_NO;
	}
	nv_data.data.alarmOutTime = 5;
	nv_data.data.alarmBuzzerTime = 3;
	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{
		nv_data.data.videoLossAlarmOn[index] = OFF;
	}
	nv_data.data.videoLossBuzzerTime = 3;
	nv_data.data.videoLossDisplayOn = ON;

	nv_data.data.remoconId = REMOCON_ID_NONE;
	nv_data.data.alarm_remote_sel = ALARM_MODE;
	nv_data.data.baudrate = BAUDRATE_9600;
	
	for(index = 0; index < NUM_OF_CHANNEL; index++)
	{	
		nv_data.data.motionDetect_On[index] = OFF;
	}
	nv_data.data.motionSensitivity = 49;
	memset(nv_data.data.motionBlocks, 0x00, sizeof(nv_data.data.motionBlocks));
	nv_data.data.motionIndication = OFF;

	nv_data.data.displayMode = DISPLAY_MODE_FULL_CH1;
	nv_data.data.splitMode = DISPLAY_MODE_2SPLIT_HSCALE_A;
	//nv_data.data.currentChannel = (eChannel_t)CHANNEL_SPLIT;

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
	uint16_t index;

	if(CheckNvBufferDirty() == SET)
	{
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP| FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ErasePage(NV_STORAGE_START_ADDR);
		for(index = 0; index < (FLASH_PAGE_SIZE/sizeof(uint32_t)); index++)
		{
			FLASH_ProgramWord(NV_STORAGE_START_ADDR+(index*sizeof(uint32_t)),*(pData+index));
		}
		FLASH_Lock();
		ClearNvBufferDirty();
	}
}

//--------------------------------------------------------------------------------
void LoadNvDataFromStorage(void)
{
	memcpy((void *)nv_data.buffer, (void *)(NV_STORAGE_START_ADDR), FLASH_PAGE_SIZE);

	if(CheckNvStorage() == FALSE)
	{
		LoadDefaultNvData();
		RTC_SetDefaultDate();
	}
}

//--------------------------------------------------------------------------------
void InitializeNvData(void)
{
	LoadDefaultNvData();
    RTC_SetDefaultDate();
}

// read or write each NV Item------------------------------------------------------
BOOL ReadNvItem(eNvItems_t item, void * pData, size_t size)
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
			memcpy(pData, &nv_data.buffer[offset], size);
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

// read/write specific item ----------------------------------------------------
void Read_NvItem_FwVersion(sVersion_t* pData)
{
	pData->major = nv_data.data.fwVersion.major;
       pData->minor = nv_data.data.fwVersion.minor;
}

void Read_NvItem_SplitMode(eDisplayMode_t* pData)
{
	*pData = nv_data.data.splitMode;
}
void Write_NvItem_SplitMode(eDisplayMode_t data)
{
	if(data < DISPLAY_MODE_2SPLIT_HSCALE_A)
	{
		data = DISPLAY_MODE_2SPLIT_HSCALE_A;
	}
	else if(data >= DISPLAY_MODE_MAX)
	{
		data = DISPLAY_MODE_PIP_D4;
	}
	nv_data.data.splitMode = data;
	nvInfo[NV_ITEM_SPLIT_MODE].dirty = SET;
}
/*
void Read_NvItem_DisplayChannel(eChannel_t* pData)
{
	*pData = nv_data.data.currentChannel;
}
void Write_NvItem_DisplayChannel(eChannel_t data)
{
	if(data <= NUM_OF_CHANNEL)
	{
		nv_data.data.currentChannel = data;
		nvInfo[NV_ITEM_DISPLAY_CHANNEL].dirty = SET;
	}
}
*/
void Read_NvItem_DisplayMode(eDisplayMode_t* pData)
{
	*pData = nv_data.data.displayMode;
}
void Write_NvItem_DisplayMode(eDisplayMode_t data)
{
	if(data >= DISPLAY_MODE_MAX)
	{
		data = DISPLAY_MODE_FULL_CH1;
	}
	nv_data.data.displayMode = data;
	nvInfo[NV_ITEM_DISPLAY_MODE].dirty = SET;
}

void Read_NvItem_TimeCorrect(sTimeCorrect_t *pData)
{
	*pData = nv_data.data.timeCorrection;
}
void Write_NvItem_TimeCorrect(sTimeCorrect_t data)
{
	nv_data.data.timeCorrection = data;
	nvInfo[NV_ITEM_TIME_CORRECT].dirty = SET;
}

void Read_NvItem_VideoLossBuzzerTime(uint8_t *pData)
{
	*pData = nv_data.data.videoLossBuzzerTime;
}
void Write_NvItem_VideoLossBuzzerTime(uint8_t data)
{
	nv_data.data.videoLossBuzzerTime = data;
	nvInfo[NV_ITEM_VIDEO_LOSS_BUZZER_TIME].dirty = SET;
}

void Read_NvItem_AutoSeqTime(uint8_t* pData)
{
    memcpy(pData, nv_data.data.autoSeqTime, nvInfo[NV_ITEM_AUTO_SEQ_TIME].size);
}
void Write_NvItem_AutoSeqTime(uint8_t* pData)
{
	memcpy(nv_data.data.autoSeqTime, pData, nvInfo[NV_ITEM_AUTO_SEQ_TIME].size);
	nvInfo[NV_ITEM_AUTO_SEQ_TIME].dirty = SET;
}

void Read_NvItem_AutoSeqLossSkip(BOOL* pData)
{
	*pData = nv_data.data.autoSeqLossSkip;
}
void Write_NvItem_AutoSeqLossSkip(BOOL data)
{
	nv_data.data.autoSeqLossSkip = data;
	nvInfo[NV_ITEM_AUTO_SEQ_LOSS_SKIP].dirty = SET;
}

void Read_NvItem_TitleDispalyOn(BOOL *pData)
{
	*pData = nv_data.data.titleDisplayOn;
}
void Write_NvItem_TitleDispalyOn(BOOL data)
{
	nv_data.data.titleDisplayOn = data;
	nvInfo[NV_ITEM_TITLE_DISPLAY_ON].dirty = SET;
}

void Read_NvItem_OsdOn(BOOL* pData)
{
	*pData = nv_data.data.osdOn;
}
void Write_NvItem_OsdOn(BOOL data)
{
	nv_data.data.osdOn = data;
	nvInfo[NV_ITEM_OSD_DISPLAY].dirty = SET;
}

void Read_NvItem_TimeDisplayOn(BOOL* pData)
{
	*pData = nv_data.data.timeDisplayOn;
}
void Write_NvItem_TimeDisplayOn(BOOL data)
{
	nv_data.data.timeDisplayOn = data;
	nvInfo[NV_ITEM_TIME_ON].dirty = SET;
}

void Read_NvItem_DateDisplayOn(BOOL* pData)
{
	*pData = nv_data.data.dateDisplayOn;
}
void Write_NvItem_DateDisplayOn(BOOL data)
{
	nv_data.data.dateDisplayOn = data;
	nvInfo[NV_ITEM_DATE_ON].dirty = SET;
}

void Read_NvItem_ChannelName(uint8_t* pData, eChannel_t channel)
{
	//*pData = nv_data.data.channelName[channel];
	strncpy((char *)pData, (const char*)(nv_data.data.channelName[channel]), CHANNEL_NEME_LENGTH_MAX);
}
void Write_NvItem_ChannelName(uint8_t* pData, eChannel_t channel)
{
	if(channel < NUM_OF_CHANNEL)
	{
		strncpy((char *)(nv_data.data.channelName[channel]), (const char *)pData, CHANNEL_NEME_LENGTH_MAX);
		nvInfo[NV_ITEM_CHANNEL_NAME].dirty = SET;
	}
}

void Read_NvItem_DateFormat(eDateFormat_t* pData)
{
	*pData = nv_data.data.dateFormat;
}
void Write_NvItem_DateFormat(eDateFormat_t data)
{
	if(data < DATE_FORMAT_MAX)
	{
		nv_data.data.dateFormat = data;
		nvInfo[NV_ITEM_DATE_FORMAT].dirty = SET;
	}
}

void Read_NvItem_YearFormat(BOOL* pData)
{
	*pData = nv_data.data.yearFormat;
}
void Write_NvItem_YearFormat(BOOL data)
{
	nv_data.data.yearFormat = data;
	nvInfo[NV_ITEM_YEAR_FORMAT].dirty = SET;
}

void Read_NvItem_VideoLossDisplayOn(BOOL* pData)
{
	*pData = nv_data.data.videoLossDisplayOn;
}
void Write_NvItem_VideoLossDisplayOn(BOOL data)
{
	nv_data.data.videoLossDisplayOn = data;
	nvInfo[NV_ITEM_VIDEO_LOSS_DISPLAY_ON].dirty = SET;
}

/*
void Read_NvItem_Resolution(eResolution_t *pData)
{
	*pData = nv_data.data.outputResolution;
}
void Write_NvItem_Resolution(eResolution_t data)
{
	if(data < RESOLUTION_MAX)
	{
		nv_data.data.outputResolution = data;
		nvInfo[NV_ITEM_OUTPUT_RESOLUTION].dirty = SET;

		UpdateVideoResolution(data);
	}
}
*/

void Read_NvItem_BorderLineDisplay(BOOL *pData)
{
	*pData = nv_data.data.borderLineOn;
}
void Write_NvItem_BorderLineDisplay(BOOL data)
{
	nv_data.data.borderLineOn = data;
	nvInfo[NV_ITEM_BORDER_LINE].dirty = SET;
}

void Read_NvItem_AlarmRemoconSelect(BOOL *pData)
{
	*pData = nv_data.data.alarm_remote_sel;
}
void Write_NvItem_AlarmRemoconSelect(BOOL data)
{
	nv_data.data.alarm_remote_sel = data;
	nvInfo[NV_ITEM_ALARM_REMOCON_SELECT].dirty = SET;
}

void Read_NvItem_AlarmOutTime(uint8_t *pData)
{
	*pData = nv_data.data.alarmOutTime;
}
void Write_NvItem_AlarmOutTime(uint8_t data)
{
	nv_data.data.alarmOutTime = data;
	nvInfo[NV_ITEM_USER_ALARMOUT_TIME].dirty = SET;
}

void Read_NvItem_AlarmBuzzerTime(uint8_t *pData)
{
	*pData = nv_data.data.alarmBuzzerTime;
}
void Write_NvItem_AlarmBuzzerTime(uint8_t data)
{
	nv_data.data.alarmBuzzerTime = data;
	nvInfo[NV_ITEM_USER_ALARM_BUZZER_TIME].dirty = SET;
}

void Read_NvItem_AlarmOption(eAlarmOption_t* pData, eChannel_t channel)
{
	if(channel < NUM_OF_CHANNEL)
	{
		*pData = nv_data.data.alarmOption[channel];
	}
}
void Write_NvItem_AlarmOption(eAlarmOption_t data, eChannel_t channel)
{
	if(channel < NUM_OF_CHANNEL)
	{
		nv_data.data.alarmOption[channel] = data;
		nvInfo[NV_ITEM_USER_ALARM_OPTION].dirty = SET;
	}
}

void Read_NvItem_RemoconId(uint8_t *pData)
{
	*pData = nv_data.data.remoconId;
}
void Write_NvItem_RemoconId(uint8_t data)
{
	nv_data.data.remoconId = data;
	nvInfo[NV_ITEM_REMOCON_ID].dirty = SET;
}

void Read_NvItem_MotionDetectOnOff(uint8_t *pData, eChannel_t channel)
{
	if(channel < NUM_OF_CHANNEL)
	{
		*pData = nv_data.data.motionDetect_On[channel];
	}	
}
void Write_NvItem_MotionDetectOnOff(uint8_t data, eChannel_t channel)
{
	if(channel < NUM_OF_CHANNEL)
	{
		nv_data.data.motionDetect_On[channel] = data;
		nvInfo[NV_ITEM_MOTION_DETECT_ON].dirty = SET;
	}	
}

void Read_NvItem_SerialBaudrate(eBaudRate_t *pData)
{
	*pData = nv_data.data.baudrate;
}
void Write_NvItem_SerialBaudrate(eBaudRate_t data)
{
	nv_data.data.baudrate = data;
	nvInfo[NV_ITEM_SERIAL_BAUDRATE].dirty = SET;
}

void Read_NvItem_MotionSensitivity(uint8_t *pData)
{
	*pData = nv_data.data.motionSensitivity;
}
void Write_NvItem_MotionSensitivity(uint8_t data)
{
	nv_data.data.motionSensitivity = data;
	nvInfo[NV_ITEM_MOTION_SENSITIVITY].dirty = SET;
}

void Read_NvItem_MotionBlock(uint16_t *pData, eChannel_t channel)
{
	memcpy(pData, &nv_data.data.motionBlocks[channel][0], sizeof(uint16_t)*ROWS_OF_BLOCKS);
}
void Write_NvItem_MotionBlock(uint16_t *pData, eChannel_t channel)
{
	memcpy(&nv_data.data.motionBlocks[channel][0], pData, sizeof(uint16_t)*ROWS_OF_BLOCKS);
	nvInfo[NV_ITEM_MOTION_DETECT_BLOCK].dirty = SET;
}

void Read_NvItem_MotionIndication(BOOL *pData)
{
	*pData = nv_data.data.motionIndication;
}
void Write_NvItem_MotionIndication(BOOL data)
{
	nv_data.data.motionIndication = data;
	nvInfo[NV_ITME_MOTION_INDICATION].dirty = SET;
}

//--------------------------------------------------------------------------------
void GetSystemStatus(sSystemStatusData_t* pStatus)
{
	memcpy(pStatus, &systemStatus, sizeof(sSystemStatusData_t));
}

void SetSystemStatus(sSystemStatusData_t* pStatus)
{
	memcpy(&systemStatus, pStatus, sizeof(sSystemStatusData_t));
}
