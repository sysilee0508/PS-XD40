#include "nv_storage.h"

#include "..\main\common.h"
#include "stm32f10x_flash.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Flash Programming (like EEPROM)
//-----------------------------------------------------------------------------------
#define ProgramTimeout        	((uint32_t)0x00002000)
#define FLASH_START_ADDR			0x08000000
#define	EEPROM_ADDR				0x08040000 //256K  why here?
BYTE EEP_buf[2048];

void write_eeprom_all(void)
{
	DWORD *pEEP_buf;
	WORD i;

	pEEP_buf = (DWORD *)EEP_buf;

	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP| FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(EEPROM_ADDR);

	for(i = 0; i < 512; i++)
	{
		FLASH_ProgramWord(EEPROM_ADDR+(i*4),pEEP_buf[i]);
	}
}

void read_eeprom_all(void)
{
	vu32 Addr;
	DWORD *pEEP_buf;
	WORD i;

	pEEP_buf = (DWORD *)EEP_buf;

	Addr = EEPROM_ADDR;

	for(i = 0; i < 512; i++)
	{
		*pEEP_buf = *((vu32 *)Addr);
		Addr += 4;
		pEEP_buf++;
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Load_Data(void)
{
	BYTE i;

	read_eeprom_all();

	sys_env.vCORRECT_OFFSET = EEP_buf[cSYSENV_vCORRECT_OFFSET];
	sys_env.bVECTOR = EEP_buf[cSYSENV_bVECTOR];
	sys_env.bCORRECT = EEP_buf[cSYSENV_bCORRECT];
	sys_env.vDATE_FORMAT = EEP_buf[cSYSENV_vDATE_FORMAT];
	sys_env.bTIME_ON = EEP_buf[cSYSENV_bTIME_ON];
	sys_env.vTIME_Position = EEP_buf[cSYSENV_vTIME_Position];

	for(i = 0; i < 9; i++)
	{
		sys_env.vCH_NAME[i][0] = EEP_buf[cSYSENV_vCH_NAME+0+(i*12)];
		sys_env.vCH_NAME[i][1] = EEP_buf[cSYSENV_vCH_NAME+1+(i*12)];
		sys_env.vCH_NAME[i][2] = EEP_buf[cSYSENV_vCH_NAME+2+(i*12)];
		sys_env.vCH_NAME[i][3] = EEP_buf[cSYSENV_vCH_NAME+3+(i*12)];
		sys_env.vCH_NAME[i][4] = EEP_buf[cSYSENV_vCH_NAME+4+(i*12)];
		sys_env.vCH_NAME[i][5] = EEP_buf[cSYSENV_vCH_NAME+5+(i*12)];
		sys_env.vCH_NAME[i][6] = EEP_buf[cSYSENV_vCH_NAME+6+(i*12)];
		sys_env.vCH_NAME[i][7] = EEP_buf[cSYSENV_vCH_NAME+7+(i*12)];
		sys_env.vCH_NAME[i][8] = EEP_buf[cSYSENV_vCH_NAME+8+(i*12)];
		sys_env.vCH_NAME[i][9] = EEP_buf[cSYSENV_vCH_NAME+9+(i*12)];
		sys_env.vCH_NAME[i][10] = EEP_buf[cSYSENV_vCH_NAME+10+(i*12)];
		sys_env.vCH_NAME[i][11] = EEP_buf[cSYSENV_vCH_NAME+11+(i*12)];
	}

	sys_env.bTITLE_ON = EEP_buf[cSYSENV_bTITLE_ON];

	for(i=0; i<4; i++)
	{
		sys_env.vDWELL[i] = EEP_buf[cSYSENV_vDWELL+i];
	}

	sys_env.bLossAutoSkip = EEP_buf[cSYSENV_bLossAutoSkip];
	sys_env.bOSD_Display = EEP_buf[cSYSENV_bOSD_Display];
	sys_env.vOSD_Position = EEP_buf[cSYSENV_vOSD_Position];
	sys_env.border_line = EEP_buf[cSYSENV_border_line];
	sys_env.vAlarm = EEP_buf[cSYSENV_vAlarm];
	sys_env.vAlarm_Display_Time = EEP_buf[cSYSENV_vAlarm_Display_Time];
	sys_env.vREMOCON_ID = EEP_buf[cSYSENV_vREMOCON_ID];
	sys_env.vLoss_Time = EEP_buf[cSYSENV_vLoss_Time];
	sys_env.vLoss_Display = EEP_buf[cSYSENV_vLoss_Display];

	sys_env.vResolution = EEP_buf[cSYSENV_resolution];
	sys_env.baud_rate = EEP_buf[cSYSENV_baud_rate];
	sys_env.b9Split_Mode = EEP_buf[cSYSENV_b9Split_Mode];
	sys_env.alarm_remote_sel= EEP_buf[cSYSENV_alarm_remote_sel];
}

void Data_Load(void)
{
	BYTE i;

#ifdef __4CH__
	if(EEP_buf[cEEP_CHK] == 0xa5)Load_Data();
#endif
	else
	{
		memset(EEP_buf, 0, 2048);

		EEP_buf[cSYSENV_vCORRECT_OFFSET] = 0;
		EEP_buf[cSYSENV_bVECTOR] = 1;
		EEP_buf[cSYSENV_bCORRECT] = 0;
		EEP_buf[cSYSENV_vDATE_FORMAT] = 0;
		EEP_buf[cSYSENV_bTIME_ON] = 1;
		EEP_buf[cSYSENV_vTIME_Position] = 1;

		for(i=0; i<9; i++)
		{
			EEP_buf[cSYSENV_vCH_NAME+0+(i*12)] = 'C';
			EEP_buf[cSYSENV_vCH_NAME+1+(i*12)] = 'A';
			EEP_buf[cSYSENV_vCH_NAME+2+(i*12)] = 'M';
			EEP_buf[cSYSENV_vCH_NAME+3+(i*12)] = '1'+i;
			EEP_buf[cSYSENV_vCH_NAME+4+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+5+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+6+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+7+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+8+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+9+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+10+(i*12)] = ' ';
			EEP_buf[cSYSENV_vCH_NAME+11+(i*12)] = ' ';
		}

		EEP_buf[cSYSENV_bTITLE_ON] = 1;

		for(i=0; i<4; i++)
		{
			EEP_buf[cSYSENV_vDWELL+i] = 3;
		}

		EEP_buf[cSYSENV_bLossAutoSkip] = 1;
		EEP_buf[cSYSENV_bOSD_Display] = 1;
#ifdef __4CH__
		EEP_buf[cSYSENV_vOSD_Position] = 6;
#endif

		EEP_buf[cSYSENV_border_line] = 1;
		EEP_buf[cSYSENV_vAlarm] = 0;
		EEP_buf[cSYSENV_vAlarm_Display_Time] = 0;
		EEP_buf[cSYSENV_vREMOCON_ID] = 0;
		EEP_buf[cSYSENV_vLoss_Time] = 3;
		EEP_buf[cSYSENV_vLoss_Display] = 0xff;

		EEP_buf[cSYSENV_resolution] = 0;
		EEP_buf[cSYSENV_baud_rate] = 3;
		EEP_buf[cSYSENV_b9Split_Mode] = 0;
		EEP_buf[cSYSENV_alarm_remote_sel] = 0xff;
#ifdef __4CH__
		EEP_buf[cEEP_CHK] = 0xa5;
#endif

		write_eeprom_all();

		Load_Data();
	}
}


//---------------------------------------------------------------------------------------------
