/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Coaxial information
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include <raptor3_ioctl.h>
#include "coax.h"
#include "coax_cmd_def.h"
#include "raptor3_api_drv.h"

int sFmtDef[16] = {0, };

/*========================================================================================================================
 * Coaxial UP stream function
 *
 ========================================================================================================================*/

void NC_APP_VD_COAX_Tx_Init( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR CoaxTx_Init;

	sFmtDef[vd_dev*4 + vd_ch]	   = vivo_fmt;

	CoaxTx_Init.vd_dev             = vd_dev;
	CoaxTx_Init.ch                 = vd_ch%4;
	CoaxTx_Init.format_standard    = format_standard;
	CoaxTx_Init.format_resolution  = format_resolution;
	CoaxTx_Init.format_fps         = format_fps;
	CoaxTx_Init.vivo_fmt           = vivo_fmt;
	NC_VD_COAX_Tx_INIT(&CoaxTx_Init);
}

void NC_APP_VD_COAX_Tx_Command_Send_Set( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps, NC_COAX_CMD_DEF cmd )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR CoaxTx_CMD;

	CoaxTx_CMD.vd_dev          = vd_dev;
	CoaxTx_CMD.ch              = vd_ch%4;
	CoaxTx_CMD.format_standard = format_standard;
	CoaxTx_CMD.vivo_fmt        = vivo_fmt;
	CoaxTx_CMD.cmd             = cmd;
	NC_VD_COAX_Tx_CMD_SEND(&CoaxTx_CMD);
}

void NC_APP_VD_COAX_Tx_16Bit_Init( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR CoaxTx_Init;

	CoaxTx_Init.vd_dev             = vd_dev;
	CoaxTx_Init.ch                 = vd_ch%4;
	CoaxTx_Init.format_standard    = format_standard;
	CoaxTx_Init.format_resolution  = format_resolution;
	CoaxTx_Init.format_fps         = format_fps;
	CoaxTx_Init.vivo_fmt           = vivo_fmt;
	NC_VD_COAX_Tx_16Bit_INIT(&CoaxTx_Init);
}

void NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps, NC_COAX_CMD_DEF cmd )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR CoaxTx_CMD;

	CoaxTx_CMD.vd_dev          = vd_dev;
	CoaxTx_CMD.ch              = vd_ch%4;
	CoaxTx_CMD.format_standard = format_standard;
	CoaxTx_CMD.vivo_fmt        = vivo_fmt;
	CoaxTx_CMD.cmd             = cmd;
	if( format_standard == FMT_AHD20)
        {
	NC_VD_COAX_Tx_16Bit_CMD_SEND(&CoaxTx_CMD);
        }
	else if( format_standard == FMT_CVI)
	{
		NC_VD_COAX_Tx_CVI_NEW_CMD_SEND(&CoaxTx_CMD);
	}
	else
		printf("NC_APP_VD_COAX_Tx_16Bit_Command_Send_Set::Not Supported::%d\n", format_standard);
}

/*========================================================================================================================
 * Coaxial Down stream function
 ========================================================================================================================*/
void NC_APP_VD_COAX_Rx_Init( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR CoaxRx_Init;

	sFmtDef[vd_dev*4 + vd_ch]	   = vivo_fmt;

	CoaxRx_Init.vd_dev          = vd_dev;
	CoaxRx_Init.ch              = vd_ch%4;
	CoaxRx_Init.format_standard = format_standard;
	CoaxRx_Init.vivo_fmt        = vivo_fmt;
	NC_VD_COAX_Rx_INIT(&CoaxRx_Init);
}

void NC_APP_VD_COAX_Rx_DeInit( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR Coax_Rx_DeInit;

	Coax_Rx_DeInit.vd_dev = vd_dev;
	Coax_Rx_DeInit.ch = vd_ch%4;
	NC_VD_COAX_Rx_DEINIT(&Coax_Rx_DeInit);
}

void NC_APP_VD_COAX_Rx_Buffer_Clear( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR Coax_Clear;

	Coax_Clear.vd_dev = vd_dev;
	Coax_Clear.ch = vd_ch%4;
	NC_VD_COAX_Rx_BUF_CLEAR( &Coax_Clear );
}

void NC_APP_VD_COAX_Rx_Data_Get( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps, NC_VD_COAX_STR *ReadVal )
{
	NC_VIVO_CH_FORMATDEF vivo_fmt = NC_HI_VI_FindFormatDef( format_standard, format_resolution, format_fps );
	NC_VD_COAX_STR Coax_Rx_Data;
	int ii;
/*
	if( format_standard == FMT_TVI )
	{
		NC_VD_COAX_STR CoaxTx_CMD;

		CoaxTx_CMD.vd_dev          = vd_dev;
		CoaxTx_CMD.ch              = vd_ch%4;
		CoaxTx_CMD.format_standard = format_standard;
		CoaxTx_CMD.vivo_fmt        = vivo_fmt;
		CoaxTx_CMD.cmd             = COAX_CMD_TVI_DOWNSTREAM_REQUEST;
		NC_VD_COAX_Tx_CMD_SEND(&CoaxTx_CMD);
	}
*/
	Coax_Rx_Data.vd_dev			  = vd_dev;
	Coax_Rx_Data.ch               = vd_ch%4;
	Coax_Rx_Data.format_standard  = format_standard;
	Coax_Rx_Data.vivo_fmt         = vivo_fmt;
	NC_VD_COAX_Rx_DATA_READ( &Coax_Rx_Data );

	if( (format_standard == FMT_CVI) || (format_standard == FMT_TVI) )
	{
		for(ii=0; ii<5; ii++)
		{
			ReadVal->rx_data1[ii] = Coax_Rx_Data.rx_data1[ii];
			ReadVal->rx_data2[ii] = Coax_Rx_Data.rx_data2[ii];
			ReadVal->rx_data3[ii] = Coax_Rx_Data.rx_data3[ii];
			ReadVal->rx_data4[ii] = Coax_Rx_Data.rx_data4[ii];
			ReadVal->rx_data5[ii] = Coax_Rx_Data.rx_data5[ii];
			ReadVal->rx_data6[ii] = Coax_Rx_Data.rx_data6[ii];
		}
	}
	else
	{
		for(ii=0; ii<8; ii++)
		{
			ReadVal->rx_pelco_data[ii] = Coax_Rx_Data.rx_pelco_data[ii];
		}
	}

}
/*========================================================================================================================
 * COAX
 *
 ========================================================================================================================*/
void NC_APP_VD_COAX_Option_RT_NRT_Mode_Change_Set( int vd_dev, int vd_ch, int param )
{
	NC_VD_COAX_STR Tx_mode;

	Tx_mode.vd_dev = vd_dev;
	Tx_mode.ch     = vd_ch%4;
	Tx_mode.param  = param;

	Tx_mode.vivo_fmt = sFmtDef[vd_dev*4 + vd_ch];

	NC_VD_OPTION_RT_NRT_MODE_CHANGE_SET(&Tx_mode);
}

/*========================================================================================================================
 * COAX Test Parameter Write&Read
 * Only Test
 ========================================================================================================================*/
void NC_APP_VD_COAX_Tx_Test_Shot_Set( int vd_ch, NC_FORMAT_STANDARD format_standard, int Val )
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	Coax_TestVal.chip_num = 0;

	if( format_standard == FMT_SD)
	{
		Coax_TestVal.ch        = vd_ch%4;
		Coax_TestVal.bank      = 0x03;
		Coax_TestVal.data_addr = 0x0c;

		if(Val)
			Coax_TestVal.param = 0x01;
		else
			Coax_TestVal.param = 0x00;
	}
	else
	{
		Coax_TestVal.ch        = vd_ch%4;
		Coax_TestVal.bank      = 0x03;
		Coax_TestVal.data_addr = 0x09;

		if(Val)
			Coax_TestVal.param = 0x08;
		else
			Coax_TestVal.param = 0x00;
	}

	printf("bank(0x%02X)/addr(0x%02X)/param(0x%02X)\n", Coax_TestVal.bank, Coax_TestVal.data_addr, Coax_TestVal.param);
	NC_VD_COAX_TEST_DATA_SET(&Coax_TestVal);
}

void NC_APP_VD_COAX_Tx_Test_Init_Val_Set( int menu, int Val )
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	Coax_TestVal.chip_num = 0;
	Coax_TestVal.ch       = 0;

	if( menu == rx_src || menu == rx_slice_lev )
	{
		Coax_TestVal.bank = 0x05;
		if(menu == rx_src) Coax_TestVal.data_addr = 0x7c;
		else if(menu == rx_slice_lev) Coax_TestVal.data_addr = 0x7d;
		else printf("COAX_READ_UNKNOWN!!\n");

		Coax_TestVal.param = Val;
	}
	else
	{
		Coax_TestVal.bank = 0x03;
		if(menu == tx_baud) Coax_TestVal.data_addr = 0x00;
		else if(menu == tx_pel_baud) Coax_TestVal.data_addr = 0x02;
		else if(menu == tx_line_pos0) Coax_TestVal.data_addr = 0x03;
		else if(menu == tx_line_pos1) Coax_TestVal.data_addr = 0x04;
		else if(menu == tx_line_count) Coax_TestVal.data_addr = 0x05;
		else if(menu == tx_pel_line_pos0) Coax_TestVal.data_addr = 0x07;
		else if(menu == tx_pel_line_pos1) Coax_TestVal.data_addr = 0x08;
		else if(menu == tx_line_count_max) Coax_TestVal.data_addr = 0x0a;
		else if(menu == tx_mode) Coax_TestVal.data_addr = 0x0b;
		else if(menu == tx_sync_pos0) Coax_TestVal.data_addr = 0x0d;
		else if(menu == tx_sync_pos1) Coax_TestVal.data_addr = 0x0e;
		else if(menu == tx_even) Coax_TestVal.data_addr = 0x2f;
		else printf("COAX_READ_UNKNOWN!!\n");

		Coax_TestVal.param = Val;
	}
	NC_VD_COAX_TEST_DATA_SET(&Coax_TestVal);
}

void NC_APP_VD_COAX_Tx_Test_Init_Val_Get( int menu, int *Val, int *Addr )
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	NC_VD_COAX_TEST_Tx_INIT_DATA_GET(&Coax_TestVal);

	if(menu == rx_src)
	{
		*Addr = 0x7c;
		*Val  = Coax_TestVal.rx_src;
	}
	else if(menu == rx_slice_lev)
	{
		*Addr = 0x7d;
		*Val  = Coax_TestVal.rx_slice_lev;
	}
	else if(menu == tx_baud)
	{
		*Addr = 0x00;
		*Val  = Coax_TestVal.tx_baud;
	}
	else if(menu == tx_pel_baud)
	{
		*Addr = 0x02;
		*Val  = Coax_TestVal.tx_pel_baud;
	}
	else if(menu == tx_line_pos0)
	{
		*Addr = 0x03;
		*Val  = Coax_TestVal.tx_line_pos0;
	}
	else if(menu == tx_line_pos1)
	{
		*Addr = 0x04;
		*Val  = Coax_TestVal.tx_line_pos1;
	}
	else if(menu == tx_line_count)
	{
		*Addr = 0x05;
		*Val  = Coax_TestVal.tx_line_count;
	}
	else if(menu == tx_pel_line_pos0)
	{
		*Addr = 0x07;
		*Val  = Coax_TestVal.tx_pel_line_pos0;
	}
	else if(menu == tx_pel_line_pos1)
	{
		*Addr = 0x08;
		*Val  = Coax_TestVal.tx_pel_line_pos1;
	}
	else if(menu == tx_line_count_max)
	{
		*Addr = 0x0A;
		*Val  = Coax_TestVal.tx_line_count_max;
	}
	else if(menu == tx_mode)
	{
		*Addr = 0x0B;
		*Val  = Coax_TestVal.tx_mode;
	}
	else if(menu == tx_sync_pos0)
	{
		*Addr = 0x0D;
		*Val  = Coax_TestVal.tx_sync_pos0;
	}
	else if(menu == tx_sync_pos1)
	{
		*Addr = 0x0E;
		*Val  = Coax_TestVal.tx_sync_pos1;
	}
	else if(menu == tx_even)
	{
		*Addr = 0x2F;
		*Val  = Coax_TestVal.tx_even;
	}
	else printf("COAX_READ_UNKNOWN!!\n");
}

void NC_APP_VD_COAX_Rx_Test_EQ_Filter_Set(int vd_ch, int sel)
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	Coax_TestVal.ch        = vd_ch;
	Coax_TestVal.chip_num  = 0;
	Coax_TestVal.bank      = 0x05;
	Coax_TestVal.data_addr = 0x58;

	if(sel)
	{
		printf("EQ Filter ON!!\n");
		Coax_TestVal.param = 0xD3;
	}
	else
	{
		printf("EQ Filter OFF!!\n");
		Coax_TestVal.param = 0x03;
	}
	NC_VD_COAX_TEST_DATA_SET(&Coax_TestVal);
}

void NC_APP_VD_COAX_Bank_Dump(int slave_addr, unsigned char bank, int vd_ch, unsigned char *pData)
{
	NC_VD_COAX_BANK_DUMP_STR BankVal;

	BankVal.vd_dev = slave_addr;
	BankVal.bank = bank;

	NC_VD_COAX_BANK_DATA_GET(&BankVal);

	memcpy(pData, BankVal.rx_pelco_data, sizeof(BankVal.rx_pelco_data) );
}

void NC_APP_VD_COAX_Test_Val_Set(int slave_addr, int bank, int addr, int val )
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	Coax_TestVal.chip_num  = slave_addr;
	Coax_TestVal.bank      = bank;
	Coax_TestVal.data_addr = addr;
	Coax_TestVal.param     = val;
	NC_VD_COAX_TEST_DATA_SET(&Coax_TestVal);
}

void NC_APP_VD_SK_Test_Val_Set( unsigned char ChipNo, unsigned char bank, unsigned char addr, unsigned char val )
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	Coax_TestVal.chip_num  = ChipNo;
	Coax_TestVal.bank      = bank;
	Coax_TestVal.data_addr = addr;
	Coax_TestVal.param     = val;
	NC_VD_COAX_TEST_DATA_SET(&Coax_TestVal);
}

int NC_APP_VD_COAX_Test_Val_Get( int slave_addr, int bank, int addr )
{
	NC_VD_COAX_TEST_STR Coax_TestVal;

	Coax_TestVal.chip_num  = slave_addr;
	Coax_TestVal.bank      = bank;
	Coax_TestVal.data_addr = addr;
	NC_VD_COAX_TEST_DATA_READ(&Coax_TestVal);

	printf("NC_APP_VD_COAX_Test_Val_Get::%d\n", Coax_TestVal.param);
	return 	Coax_TestVal.param;
}

