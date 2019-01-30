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
#define	NV_STORAGE_START_ADDR			FLASH_ADDRESS_PAGE(150)//0x08080000

#define NVSTORAGE_START_CHECK			0xA5A5A5A5
#define NVSTORAGE_END_CHECK				0x5A5A5A5A

#define NV_VERSION_MAJOR				(uint8_t)1
#define NV_VERSION_MINOR				(uint8_t)0

#define FW_VERSION_MAJOR				(uint8_t)99	//engineer version starts 90
#define FW_VERSION_MINOR				(uint8_t)0

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
//-- system data ---------------------------------------------------------------
//	If you want to store any system data (item) in NV memory, it comes here
	NV_ITEM_DISPLAY_MODE,
//------------------------------------------------------------------------------
	NV_ITEM_END_CHECK,
	NV_ITEM_MAX
} eNvItems_t;

typedef struct
{
	uint8_t major;
	uint8_t minor;
} sVersion_t;

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

	sVersion_t				nvVersion;
	sVersion_t				fwVersion;
	eDisplayMode_t			displayMode;

	uint32_t				storageEndCheck;
} sNvData_t;

//=============================================================================
//  Function Prototype
//=============================================================================
extern void StoreNvDataToStorage(void);
extern void LoadNvDataFromStorage(void);
extern void InitializeNvData(void);

extern void Read_NvItem_FwVersion(sVersion_t* pData);
extern void Read_NvItem_DisplayMode(eDisplayMode_t* pData);
extern void Write_NvItem_DisplayMode(eDisplayMode_t data);

#endif
