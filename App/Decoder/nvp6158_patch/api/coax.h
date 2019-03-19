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
#ifndef _RAPTOR3_COAX_
#define _RAPTOR3_COAX_

#include <raptor3_fmt.h>
#include "coax_cmd_def.h"

/*=============================================================
 * Coaxial UP Stream Initialize Structure[APP -> DRV]
 ==============================================================*/
typedef struct NC_VD_COAX_STR{
	 char *name;
	 unsigned char ch;
	 unsigned char vd_dev;
	 unsigned char param;
	 NC_FORMAT_STANDARD format_standard;
	 NC_FORMAT_RESOLUTION format_resolution;
	 NC_FORMAT_FPS format_fps;
	 NC_VIVO_CH_FORMATDEF vivo_fmt;
	 NC_COAX_CMD_DEF cmd;

	 unsigned char rx_pelco_data[8];
	 unsigned char rx_data1[8];
	 unsigned char rx_data2[8];
	 unsigned char rx_data3[8];
	 unsigned char rx_data4[8];
	 unsigned char rx_data5[8];
	 unsigned char rx_data6[8];

}NC_VD_COAX_STR;

/*=============================================================
 * Coaxial Test Structure[APP <-> DRV]
 ==============================================================*/
typedef struct NC_VD_COAX_TEST_STR{
	 unsigned char ch;
	 unsigned char chip_num;
	 unsigned char bank;
	 unsigned char data_addr;
	 unsigned char param;

	 unsigned char rx_src;             //B5/6/7/8 0x7C
	 unsigned char rx_slice_lev;       //B5/6/7/8 0x7D
	 unsigned char tx_baud;            //B3/4 0x00/80
	 unsigned char tx_pel_baud;        //B3/4 0x02/82
	 unsigned char tx_line_pos0;       //B3/4 0x03/83
	 unsigned char tx_line_pos1;       //B3/4 0x04/84
	 unsigned char tx_pel_line_pos0;   //B3/4 0x07/87
	 unsigned char tx_pel_line_pos1;   //B3/4 0x08/88
	 unsigned char tx_line_count;      //B3/4 0x05/85
	 unsigned char tx_line_count_max;  //B3/4 0x0A/8A
	 unsigned char tx_mode;            //B3/4 0x0B/8B
	 unsigned char tx_sync_pos0;       //B3/4 0x0D/8D
	 unsigned char tx_sync_pos1;       //B3/4 0x0E/8E
	 unsigned char tx_even;            //B3/4 0x2F/AF
	 unsigned char tx_zero_length;     //B3/4 0x0C/
}NC_VD_COAX_TEST_STR;

typedef struct NC_VD_COAX_BANK_DUMP_STR{
	 unsigned char ch;
	 unsigned char vd_dev;
	 unsigned char bank;

	 unsigned char rx_pelco_data[256];

}NC_VD_COAX_BANK_DUMP_STR;


// Coaxial UP Stream Function
void NC_APP_VD_COAX_Tx_Init( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps );
void NC_APP_VD_COAX_Tx_Command_Send_Set( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps, NC_COAX_CMD_DEF cmd );

// Coaxial Down Stream Function
void NC_APP_VD_COAX_Rx_Init( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps );
void NC_APP_VD_COAX_Rx_DeInit( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps );
void NC_APP_VD_COAX_Rx_Buffer_Clear( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps );
void NC_APP_VD_COAX_Rx_Data_Get( int vd_dev, int vd_ch, NC_FORMAT_STANDARD format_standard, NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps, NC_VD_COAX_STR *ReadVal );

// Coaxial Test Function
void NC_APP_VD_COAX_Tx_Test_Shot_Set( int vd_ch, NC_FORMAT_STANDARD format_standard, int Val );
void NC_APP_VD_COAX_Tx_Test_Init_Val_Set( int menu, int Val );
void NC_APP_VD_COAX_Tx_Test_Init_Val_Get( int menu, int *Val, int *Addr );
void NC_APP_VD_COAX_Rx_Test_EQ_Filter_Set(int vd_ch, int sel);

void NC_APP_VD_COAX_Test_Val_Set(int slave_addr, int bank, int addr, int val );
int NC_APP_VD_COAX_Test_Val_Get( int slave_addr, int bank, int addr );

#endif
