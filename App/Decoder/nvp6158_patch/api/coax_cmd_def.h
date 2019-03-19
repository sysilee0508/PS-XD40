/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Coaxial CMD information
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _COAX_CMD_DEF_
#define _COAX_CMD_DEF_

typedef enum NC_COAX_CMD_DEF
{
	COAX_CMD_UNKNOWN = 0,
	COAX_CMD_IRIS_INC,
	COAX_CMD_IRIS_DEC,
	COAX_CMD_FOCUS_INC,
	COAX_CMD_FOCUS_DEC,
	COAX_CMD_ZOOM_INC,
	COAX_CMD_ZOOM_DEC,
	COAX_CMD_OSD_ON,
	COAX_CMD_PTZ_UP,
	COAX_CMD_PTZ_DOWN,
	COAX_CMD_PTZ_LEFT,
	COAX_CMD_PTZ_RIGHT,
	COAX_CMD_OSD_ENTER,
	COAX_CMD_SPECIAL_FW,
	COAX_CMD_SPECIAL_CAMEQ,
	COAX_CMD_SPECIAL_FPS,
	COAX_CMD_SPECIAL_MOTION,
	COAX_CMD_SPECIAL_3DNR_REQUEST,
	COAX_CMD_TVI_DOWNSTREAM_REQUEST,
	COAX_CMD_TVI_1M_A_B_TYPE_CHANGE_A,
	COAX_CMD_TVI_1M_A_B_TYPE_CHANGE_B,

	COAX_CMD_MAX,

} NC_COAX_CMD_DEF;

typedef enum NC_COAX_READ_DEF
{
	COAX_READ_UNKNOWN = 0,
	rx_src,
	rx_slice_lev,
	tx_baud,
	tx_pel_baud,
	tx_line_pos0,
	tx_line_pos1,
	tx_line_count,
	tx_pel_line_pos0,
	tx_pel_line_pos1,
	tx_line_count_max,
	tx_mode,
	tx_sync_pos0,
	tx_sync_pos1,
	tx_even,

	COAX_RAED_MAX,

} NC_COAX_READ_DEF;

#if 0
typedef enum NC_COAX_RX_READ_DEF
{
	COAX_RX_READ_UNKNOWN = 0,
	COAX_CH1_rx_line1,
	COAX_CH1_rx_line2,
	COAX_CH1_rx_line3,
	COAX_CH1_rx_line4,
	COAX_CH1_rx_line5,
	COAX_CH1_rx_line6,

	COAX_CH2_rx_line1,
	COAX_CH2_rx_line2,
	COAX_CH2_rx_line3,
	COAX_CH2_rx_line4,
	COAX_CH2_rx_line5,
	COAX_CH2_rx_line6,

	COAX_CH3_rx_line1,
	COAX_CH3_rx_line2,
	COAX_CH3_rx_line3,
	COAX_CH3_rx_line4,
	COAX_CH3_rx_line5,
	COAX_CH3_rx_line6,

	COAX_CH4_rx_line1,
	COAX_CH4_rx_line2,
	COAX_CH4_rx_line3,
	COAX_CH4_rx_line4,
	COAX_CH4_rx_line5,
	COAX_CH4_rx_line6,

	COAX_RX_RAED_MAX,

} NC_COAX_RX_READ_DEF;
#endif

#endif
