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
		{NV_ITEM_DISPLAY_MODE, 				sizeof(eDisplayMode_t),							CLEAR},
		{NV_ITEM_END_CHECK,					sizeof(uint32_t),							CLEAR}
};

//-----------------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------------
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
       
	nv_data.data.displayMode = DISPLAY_MODE_FULL_CH1;

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

void Read_NvItem_DisplayMode(eDisplayMode_t* pData)
{
	*pData = nv_data.data.displayMode;
}
void Write_NvItem_DisplayMode(eDisplayMode_t data)
{
	nv_data.data.displayMode = data;
	nvInfo[NV_ITEM_DISPLAY_MODE].dirty = SET;
}
