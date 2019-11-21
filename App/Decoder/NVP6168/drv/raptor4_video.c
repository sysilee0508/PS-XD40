/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: video_input.c
 *  Description	:
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/


/* ----------------------------------------------------------------------------------
 * 1. Header file include -----------------------------------------------------------
 * --------------------------------------------------------------------------------*/
#include "raptor4_common.h"
#include "raptor4_table.h"
#include "raptor4_function.h"


/* ----------------------------------------------------------------------------------
 * 2. Define ------------------------------------------------------------------------
 * --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
 * 3. Define variable ---------------------------------------------------------------
 * --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
 * 4. External variable & function --------------------------------------------------
 * --------------------------------------------------------------------------------*/
extern NC_U8 g_nc_drv_i2c_addr[4];


/* ----------------------------------------------------------------------------------
 * 5. Function prototype ------------------------------------------------------------
 * --------------------------------------------------------------------------------*/
NC_U32 internal_nc_drv_video_distance_hsync_distortion_get( NC_U8 dev, NC_U8 chn, NC_U32 *Sam1Frame, NC_U32 *Sam8Frame );
void   internal_nc_drv_video_sd_format_set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt);
void   internal_nc_drv_video_input_format_set(NC_U8 dev, NC_U8 chn, NC_VO_WORK_MODE_E mux_mode, NC_VIDEO_FMT_INIT_TABLE_S *stTableVideo);
void   internal_nc_drv_video_output_hide_set( NC_U8 Dev, NC_U8 Chn, NC_U8 Val );

// kukuri
extern void NVP6168_OutPort_Set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt);


void nc_drv_video_setting_info_get( void *pParam )
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 info_chn = pVdInfo->Chn;

	pVdInfo->VideoFormat      = nc_drv_common_info_video_fmt_def_get(info_chn);
	pVdInfo->VideoCable       = nc_drv_common_info_cable_get(info_chn);
	pVdInfo->FormatStandard   = nc_drv_common_info_video_fmt_standard_get(info_chn);
	pVdInfo->FormatResolution = nc_drv_common_info_video_fmt_resolusion_get(info_chn);
	pVdInfo->FormatFps        = nc_drv_common_info_video_fmt_fps_get(info_chn);
	pVdInfo->EqStage          = nc_drv_common_info_eq_stage_get(info_chn);
	nc_drv_common_info_video_fmt_string_get(info_chn, pVdInfo->FmtStr);
}

NC_VIVO_CH_FORMATDEF_E nc_drv_video_h_v_cnt_check_get(NC_U8 dev, NC_U8 chn)
{
	NC_VIVO_CH_FORMATDEF_E fmt = NC_VIVO_CH_FORMATDEF_UNKNOWN;

	int H_CNT = 0;
	int V_CNT = 0;
	int AHD_VBI = 0;
	int CVI_VBI = 0;
	int SyncMax = 0;

	NC_U8 HighReg = 0;
	NC_U8 LowReg  = 0;

	/*********************************************************************
	 * H, V Count Check
	 *********************************************************************/
	//printk("============= Signal Information =============\n");
	NC_DEVICE_DRIVER_BANK_SET(dev, 0x05 + chn);
	LowReg  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xF2);
	HighReg = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xF3);
	H_CNT = (HighReg<<8)|LowReg;
	//printk("H_Cnt[%x + %x = %x/%d]\n", HighReg, LowReg, H_CNT, H_CNT);

	LowReg  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xF4);
	HighReg = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xF5);
	V_CNT = (HighReg<<8)|LowReg;
	//printk("V_Cnt[%x + %x = %x/%d]\n", HighReg, LowReg, V_CNT, V_CNT);

	/*********************************************************************
	 * LE Pulse Check
	 *********************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, 0x13);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2B, chn);
	LowReg  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xa4);
	HighReg = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xa5);
	AHD_VBI = (HighReg<<8)|LowReg;
	//printk("AHD_VBI[%x + %x = %x/%d]\n", HighReg, LowReg, AHD_VBI, AHD_VBI);

	LowReg  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xa6);
	HighReg = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xa7);
	CVI_VBI = (HighReg<<8)|LowReg;
	//printk("CVI_VBI[%x + %x = %x/%d]\n", HighReg, LowReg, CVI_VBI, CVI_VBI);

	LowReg  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xaa);
	HighReg = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xab);
	SyncMax = (HighReg<<8)|LowReg;
	//printk("SyncMax[%x + %x = %x/%d]\n", HighReg, LowReg, SyncMax, SyncMax);
	//printk("==============================================\n");

	if(H_CNT == 0 && V_CNT == 0)
	{
		//printk("[%s::%d]NC_VI_NO_SIGNAL\n", __func__, __LINE__);
		return NC_VI_SIGNAL_OFF;
	}
	else
	{
		//printk("[%s::%d]NC_VI_SIGNAL_ON\n", __func__, __LINE__);
		return NC_VI_SIGNAL_ON;
	}

	return fmt;

}

void nc_drv_video_input_vfc_status_get( void *pParam )
{
	NC_S32 ret = 0;
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 info_chn = pVdInfo->Chn;
	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;

	NC_U8 cur_vfc = 0;
	NC_U8 pre_vfc = 0;
	NC_U8 fmt_change = 0; // 0 : Not Change, 1 : Change

//	NC_VIVO_CH_FORMATDEF_E fmt_h_v_signal = NC_VIVO_CH_FORMATDEF_UNKNOWN;

	// common - device&channel number check
	if( (ret = nc_drv_common_total_chn_count_get(info_chn)) != 0 )
	{
		//printk("[%s]nc_drv_common_total_chn_count_get error!!TotalChn>>>%d\n", __func__, ret);
		return;
	}

	// Check if Coax firmware is being updated
	if(nc_drv_common_info_coax_fw_update_status_get(info_chn))
	{
		pVdInfo->FmtChange   = 0;
		return;
	}

	NC_DEVICE_DRIVER_BANK_SET(dev, 0x05 + chn);
	cur_vfc = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xF0);
	pre_vfc = nc_drv_common_info_vfc_get( info_chn );

	/**********************************************************
	 * No Video Status
	 * 0xFF, 0xF0, 0x0F
	 **********************************************************/
	if( cur_vfc == 0xFF || (cur_vfc&0xF0) == 0xF0 || (cur_vfc&0x0F) == 0x0F )
	{
		/****************************************************************
		 * 1. No Video Sequence
		 ****************************************************************/
		if( pre_vfc != 0xFF )
		{
			/**************************************************************************************
			 * DATA_OUT_MODE :: 0001  :  Background color output, Black Pattern
			 **************************************************************************************/
			NC_U8 mask = 0;
			NC_U8 output = 0x0F;
			NC_DEVICE_DRIVER_BANK_SET(dev, 0x00);
			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7a + (chn/2));
			if(chn%2)
			{
				// Channel 1, 3, 5, 7, 9, 11, 13, 15
				output = (output<<4)|mask;
			}
			else
			{
				// Channel 0, 2, 4, 6, 8, 10, 12, 14
				output = output|mask;
			}
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7a + (chn/2), output); // black screen
			// kukuri 
			// addr 7A and 7B - sholud be 0xFF
			// addr 78 79 (background color) --> black

			/* On -> No video -----------*/
			NC_DEVICE_DRIVER_BANK_SET(dev, 0x05 + chn);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xB8, 0xB8);

			nc_drv_common_info_chn_alive_set(info_chn, 0);
			fmt_change = 1;
			//printk("[NC_DRV_VFC]NoVideo >>> Chn::%d[pre(0x%02X)->cur(0x%02X)]\n", info_chn, pre_vfc, cur_vfc);
		}

		cur_vfc = 0xFF;
	}
	else
	{
		/****************************************************************
		 * 2. On Video Sequence
		 ****************************************************************/
		if( pre_vfc != cur_vfc )
		{

			/* Sequence 2.1 : No -> On video -----------------------*/
			/* Sequence 2.2 : On -> On video Format Changed --------*/
			NC_U8 mask = 0;
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_13);
			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x70);
			mask = mask | (0x01<<chn);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x70, mask);  // FORMAT_SET_DONE

			msleep(100);

			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x71);
			mask = mask | (0x01<<chn);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x71, mask);  // ACC_GET_DONE

			// On Video Format Change
			fmt_change = 1;
			//printk("[NC_DRV_VFC]OnVideo >>> Chn::%d[pre(0x%02X)->cur(0x%02X)]\n", info_chn, pre_vfc, cur_vfc);
		}
	}

	/***************************************************************************************
	 * Decoder channel information save
	 * 1. Save - VFC
	 * 2. Save - Video Format Define_enum, Standard_enum, Resolution_enum, Fps_enum
	 * 3. Save - Video Format String
	 * 4. Save - Sleep Time
	 * 5. Save - Video Cable - Currently fixed
	 ***************************************************************************************/
	nc_drv_common_info_video_format_set( info_chn, cur_vfc );
	nc_drv_common_info_cable_set(info_chn, CABLE_3C2V);

	/***************************************************************************************
	 * Driver -> Application information send
	 * 1. Format Change : 1 - Changed  /  0 - Not Changed
	 * 2. VFC Value
	 * 3. Format Define_enum
	 * 4. Format Character String
	 ***************************************************************************************/
	pVdInfo->FmtChange   = fmt_change;
	pVdInfo->Vfc 	     = cur_vfc;
	pVdInfo->VideoFormat = nc_drv_common_info_video_fmt_def_get(info_chn);
	nc_drv_common_info_video_fmt_string_get(info_chn, pVdInfo->FmtStr);

	if(fmt_change)
	{
		/****************************************************************
		 * For 3M RT or higher format, BT1120 setting is required.
		 * Post automatically separates Y and C.
		 ****************************************************************/
		NC_VIVO_CH_FORMATDEF_E fmt = pVdInfo->VideoFormat;
		if( fmt == AHD_3M_25P || fmt == AHD_3M_30P || fmt == AHD_4M_25P || fmt == AHD_4M_30P || fmt == AHD_8M_12_5P || fmt == AHD_8M_15P ||\
			fmt == AHD_5M_20P || fmt == AHD_5_3M_20P ||\
			fmt == CVI_4M_25P || fmt == CVI_4M_30P || fmt == CVI_8M_12_5P || fmt == CVI_8M_15P ||\
			fmt == TVI_4M_25P || fmt == TVI_4M_30P || fmt == TVI_5M_20P   || fmt == TVI_8M_12_5P || fmt == TVI_8M_15P )
		{
			NC_U8 mask = 0;

			// 1. Post YC Merge
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x69);
			mask = mask|(0x01<<4);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x69, mask);

			// 2. TM Mux_YC Merge
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xed);
			mask = mask|(0x01<<chn);   // set 1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xed, mask);

	//		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc+chn, 0x66);
		}
		else
		{
			NC_U8 mask = 0;

			// 1. Post YC Merge
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x69);
			mask = mask&0xEF; // Set 0[4] 0xEF 1110 1111
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x69, mask);

			// 2. TM Mux_YC Merge
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xed);
			mask = mask&~(0x01<<chn);	// set 0[3:0]
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xed, mask);

	//		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc+chn, 0x46);
		}
	}
}

NC_U8 nc_drv_int_eq_stage_max_get(NC_U8 chn, NC_U8 *pSrc, NC_U8 Size)
{
	int ii = 0;
	NC_U8 max = 0;
	NC_U8 max_eq_stage = 0;

	for(ii=0; ii<Size; ii++)
	{
//		printk("chn[%d] pSrc[%d]::%d\n", chn, ii, pSrc[ii]);
		if(max <= pSrc[ii])
		{
			max = pSrc[ii];
			max_eq_stage = ii;
		}
	}

	return max_eq_stage;
}

void nc_drv_video_input_eq_stage_get( void *pParam )
{
	NC_S32 ret = 0;

	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 info_chn = pVdInfo->Chn;
	NC_U8 dev 	   = pVdInfo->Chn/4;
	NC_U8 chn 	   = pVdInfo->Chn%4;

	NC_U8  eq_stage 	 = 0;
	NC_S32 sam_check_cnt = 0;
	NC_S32 sam_check_sel = 5;

	NC_U8  Stage_max 	 = 0;
	NC_U32 sam_value[10] = { 0, };
	NC_U8  Stage[10] 	 = { 0, };

	int ii = 0;

	NC_U32 pre_sam_update = 0;
	NC_U32 time_out_cnt   = 0;
	NC_U32 sam_avg 		  = 0;

	NC_VIVO_CH_FORMATDEF_E fmt  = nc_drv_common_info_video_fmt_def_get(info_chn);
	NC_CABLE_E cable 	 	    = nc_drv_common_info_cable_get(info_chn);
	NC_VIDEO_FMT_INIT_TABLE_S *stTableVideo;


	// common - device&channel number check
	if( (ret = nc_drv_common_total_chn_count_get(info_chn)) != 0 )
	{
		//printk("[%s]nc_drv_common_total_chn_count_get error!!TotalChn>>>%d\n", __func__, ret);
		return;
	}

	/***************************************************************************************
	 * 2. hsync_distortion get
	 ***************************************************************************************/
	while(1)
	{
		NC_U32 sam_value_cur = 0;

		sam_value_cur = internal_nc_drv_video_distance_hsync_distortion_get(dev, chn, 0, 0);
		if( (sam_value_cur > 0) && ( pre_sam_update != sam_value_cur))
		{
			NC_U8 sam_update = 0;

			sam_value[sam_check_cnt++] = sam_value_cur;
//			printk("[%s::%d]%d::%x\n", __FILE__, __LINE__, sam_check_cnt, sam_value[sam_check_cnt]);
//			sam_check_cnt++;

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_13);
			sam_update = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7a);
			sam_update = sam_update|(0x01<<chn);	// Set 1[3:0]
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7a, sam_update);

			sam_update = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7a);
			sam_update = sam_update&~(0x01<<chn);	// set 0[3:0]
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7a, sam_update);

			pre_sam_update = sam_value_cur;
		}
		else
		{
			time_out_cnt++;
			if(time_out_cnt > 70)
			{
				//printk("[%s::%d]Chn%d - EQ Stage Get time_out_cnt Error!!(%d / %x)\n", __func__, __LINE__, info_chn, sam_value_cur, sam_value_cur);
				return;
			}
		}

		if(sam_check_cnt >= sam_check_sel)
		{
#if 0
			int ii = 0;
			for(ii=0; ii<20; ii++)
			{
				sam_avg += sam_value[ii];
			}
			sam_avg = sam_avg/10;
//			sam_avg = (sam_value[0] + sam_value[1] + sam_value[2])/3;
#endif
			break;
		}

		msleep( 10 );
	}

	if( cable == CABLE_3C2V )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else if( cable == CABLE_UTP )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else
	{
		//printk("[%s::%d]Video Cable Error!!\n", __FILE__, __LINE__);
		return;
	}

	for(ii=0; ii<sam_check_sel; ii++)
	{
		for( eq_stage=0; eq_stage<10; eq_stage++ )
		{
			if(sam_value[ii] >= stTableVideo->nc_table_eq_distance_value[eq_stage])
			{
//				printk("[%s::%d]Chn%d - %d_sam_value[,%x] > Stage[%d]\n", __func__, __LINE__, info_chn, ii, sam_value[ii], eq_stage);
				Stage[eq_stage]++;
				break;
			}
		}
	}

	Stage_max = nc_drv_int_eq_stage_max_get(chn, Stage, 10);

//	printk("Chn%d - Video EqStage Apply::%d\n", info_chn, Stage_max);
	pVdInfo->EqStage = Stage_max;

	// Test Purpose
	pVdInfo->SamVal = sam_avg;

	/********************************************************************************
	 * Save Common Information
	 ********************************************************************************/
	nc_drv_common_info_eq_stage_set(info_chn, Stage_max);
}

void nc_drv_video_input_eq_stage_set( void *pParam )
{
	NC_S32 ret = 0;
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 info_chn = pVdInfo->Chn;
	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;

	NC_VIVO_CH_FORMATDEF_E fmt   = 0;
	NC_CABLE_E cable 			 = 0;
	NC_U8 eq_stage 			   	 = 0;

	NC_U8 mask = 0;
	NC_U8 output = 0x01;

	NC_VIDEO_FMT_INIT_TABLE_S *stTableVideo;

	// common - device&channel number check
	if( (ret = nc_drv_common_total_chn_count_get(info_chn)) != 0 )
	{
		//printk("[%s]nc_drv_common_total_chn_count_get error!!TotalChn>>>%d\n", __func__, ret);
		return;
	}

	if(nc_drv_common_info_chn_alive_get(pVdInfo->Chn) != 1)
	{
		//printk("[%s]Video input is not set yet.!! chn(%d)\n", __func__, chn);
		return;
	}

	cable 	 = nc_drv_common_info_cable_get(info_chn);
	fmt      = nc_drv_common_info_video_fmt_def_get(info_chn);
	eq_stage = pVdInfo->EqStage;


	if( cable == CABLE_3C2V )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else if( cable == CABLE_UTP )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else
	{
		//printk("\n");
		return;
	}

	//printk("[%s]info_chn(%d), Dev(%d), Chn(%d), Fmt(%d::%s), EqStage(%d), Cable(%d)\n", __func__, info_chn, dev, chn, fmt, stTableVideo->name, eq_stage, cable );

	/**************************************************************************************
	 * eq_base_set_value
	 **************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5+chn);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x01, stTableVideo->nc_table_eq_base_5x01_bypass[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x58, stTableVideo->nc_table_eq_base_5x58_band_sel[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x5c, stTableVideo->nc_table_eq_base_5x5c_gain_sel[eq_stage]);

	NC_DEVICE_DRIVER_BANK_SET(dev,  (chn < 2 ? BANK_A : BANK_B));
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3d + (chn%2 * 0x80), stTableVideo->nc_table_eq_base_ax3d_deq_on[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3c + (chn%2 * 0x80), stTableVideo->nc_table_eq_base_ax3c_deq_a_sel[eq_stage]);

	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_9);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x80 + (chn * 0x20), stTableVideo->nc_table_eq_base_9x80_deq_b_sel[eq_stage]);

	/**************************************************************************************
	 * eq_coeff_set_value
	 **************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev,  (chn < 2 ? BANK_A : BANK_B));
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x30 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax30_deq_01[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x31 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax31_deq_02[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x32 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax32_deq_03[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x33 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax33_deq_04[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x34 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax34_deq_05[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x35 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax35_deq_06[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x36 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax36_deq_07[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x37 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax37_deq_08[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x38 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax38_deq_09[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x39 + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax39_deq_10[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3a + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax3a_deq_11[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3b + (chn%2 * 0x80), stTableVideo->nc_table_eq_coeff_ax3b_deq_12[eq_stage]);

	/**************************************************************************************
	 * eq_color_set_value
	 **************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x10 + chn,   stTableVideo->nc_table_eq_color_0x10_contrast[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x18 + chn,   stTableVideo->nc_table_eq_color_0x18_h_peaking[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x21 + chn*4, stTableVideo->nc_table_eq_color_0x21_c_filter[eq_stage]);

	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x40 + chn, stTableVideo->nc_table_eq_color_0x40_hue[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x44 + chn, stTableVideo->nc_table_eq_color_0x44_u_gain[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x48 + chn, stTableVideo->nc_table_eq_color_0x48_v_gain[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x4c + chn, stTableVideo->nc_table_eq_color_0x4c_u_offset[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x50 + chn, stTableVideo->nc_table_eq_color_0x50_v_offset[eq_stage]);

	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x20, stTableVideo->nc_table_eq_color_5x20_black_level[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x27, stTableVideo->nc_table_eq_color_5x27_acc_ref[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x28, stTableVideo->nc_table_eq_color_5x28_cti_delay[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2b, stTableVideo->nc_table_eq_color_5x2b_sub_saturation[eq_stage]);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xd5, stTableVideo->nc_table_eq_color_5xd5_c_option[eq_stage]);

	/**************************************************************************************
	 * eq_timing_a_set_value
	 **************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x58 + chn, stTableVideo->nc_table_eq_timing_0x58_h_delay_a[eq_stage]);

	/**************************************************************************************
	 * DATA_OUT_MODE :: 0001  :  Y_(001~254), Cb_(001~254), Cr_(001~254)
	 **************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, 0x00);
	mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7a + (chn/2));
	if(chn%2)
	{
		// Channel 1, 3, 5, 7, 9, 11, 13, 15
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7a + (chn/2));
		mask = mask&0x0F;
		output = (output<<4)|mask;
	}
	else
	{
		// Channel 0, 2, 4, 6, 8, 10, 12, 14
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7a + (chn/2));
		mask = mask&0xF0;
		output = output|mask;
	}
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7a + (chn/2), output);


	nc_drv_common_info_eq_stage_set(chn, eq_stage);

}

void nc_drv_video_input_set(void *pParam)
{
	NC_S32 ret = 0;
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 info_chn 	= pVdInfo->Chn;
	NC_U8 dev 		= pVdInfo->Chn/4;
	NC_U8 chn 		= pVdInfo->Chn%4;

	NC_VIDEO_FMT_INIT_TABLE_S *stTableVideo;

	NC_VIVO_CH_FORMATDEF_E fmt   = nc_drv_common_info_video_fmt_def_get(info_chn);
	NC_FORMAT_STANDARD_E fmt_std = nc_drv_common_info_video_fmt_standard_get(info_chn);
	NC_CABLE_E cable 	 	     = nc_drv_common_info_cable_get(info_chn);

	NC_VO_WORK_MODE_E mux_mode = nc_drv_common_info_vo_mode_get(chn);

	// common - device&channel number check
	if( (ret = nc_drv_common_total_chn_count_get(info_chn)) != 0 )
	{
		//printk("[%s]nc_drv_common_total_chn_count_get error!!TotalChn(%d), Chn(%d)\n", __func__, ret, info_chn);
		return;
	}

	if( cable == CABLE_3C2V )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else if( cable == CABLE_UTP )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else
	{
		//printk("\n");
		return;
	}

	if(stTableVideo == NULL)
	{
		//printk("[%s::%d]Can not find Video Table!! Chn(%d)\n", __func__, __LINE__, chn);
		return;
	}
	else if( fmt == AHD_8M_CIF_25P || fmt == AHD_8M_CIF_30P ||\
			 fmt == CVI_HD_25P     || fmt == CVI_HD_30P     ||\
			 fmt == TVI_HD_B_25P   || fmt == TVI_HD_B_30P   || fmt == TVI_960P_25P   || fmt == TVI_960P_30P )
	{
		//printk("[%s::%d]It is not yet ready Video Format!! Chn(%d), fmt(%s)\n", __func__, __LINE__, chn, stTableVideo->name);
		return;
	}

	/****************************************************************
	 * Common Information
	 ****************************************************************/
	nc_drv_common_info_chn_alive_set(info_chn, 1);

	/****************************************************************
	 * video format setting
	 ****************************************************************/
	internal_nc_drv_video_input_format_set(dev, chn, mux_mode, stTableVideo);

	/****************************************************************
	 * video eq_stage default setting -> stage 0
	 * setting sequence : video setting -> eq_stage 0 setting -> eq_stage get -> eq_stage set
	 ****************************************************************/
	pVdInfo->EqStage = 0;
	nc_drv_video_input_eq_stage_set(pVdInfo);

	/****************************************************************
	 * Coaxial Initialize
	 ****************************************************************/
	pVdInfo->Coax_ver = COAX_COAX_NORMAL;
	nc_drv_coax_initialize_set(pVdInfo);

#if AUDIO_BLOCK == 1
	/****************************************************************
	 * AOC Initialize
	 ****************************************************************/
	if(nc_drv_audio_mode_get() == NC_AD_AOC && fmt_std == FMT_AHD)
		nc_drv_audio_video_format_set(pVdInfo);
#endif

	/****************************************************************
	 * Turn on the HPLL again, when finished video settings
	 ****************************************************************/
	if( fmt_std == FMT_SD )
	{
		internal_nc_drv_video_sd_format_set(dev, chn, fmt);
	}
	else
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, 0x05 + chn);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xB8, 0x39);
	}

//	NVP6168_OutPort_Set(dev, chn, fmt);
}

void nc_drv_video_input_manual_set(void * pParam)
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 info_chn = pVdInfo->Chn;
	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;

	NC_FORMAT_STANDARD_E Fmt_std   = pVdInfo->FormatStandard;
	NC_FORMAT_RESOLUTION_E Fmt_res = pVdInfo->FormatResolution;
	NC_FORMAT_FPS_E	Fmt_fps		   = pVdInfo->FormatFps;
	NC_CABLE_E cable 			   = pVdInfo->VideoCable;
	NC_U8 eq_stage 				   = 0;	// pVdInfo->EqStage;

	NC_VIVO_CH_FORMATDEF_E fmt = nc_drv_table_video_format_get(Fmt_std, Fmt_res, Fmt_fps);

	NC_VIDEO_FMT_INIT_TABLE_S *stTableVideo;

	if( cable == CABLE_3C2V )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else if( cable == CABLE_UTP )
	{
		stTableVideo = (NC_VIDEO_FMT_INIT_TABLE_S*)nc_drv_table_video_init_vlaue_get(fmt);
	}
	else
	{
		//printk("\n");
		return;
	}

	//printk("[%s]info_chn(%d), Dev(%d), Chn(%d) Video Fmt(%d::%s)\n", __func__, info_chn, dev, chn, fmt, stTableVideo->name);

	/********************************************************************************
	 * Common Information Set
	 ********************************************************************************/
	nc_drv_common_info_video_format_manual_set(info_chn, fmt);
	nc_drv_common_info_eq_stage_set(info_chn, eq_stage);
	nc_drv_common_info_cable_set(info_chn, cable);

	/****************************************************************
	 * For 3M RT or higher format, BT1120 setting is required.
	 * Post automatically separates Y and C.
	 ****************************************************************/
	if( fmt == AHD_3M_25P || fmt == AHD_3M_30P || fmt == AHD_4M_25P || fmt == AHD_4M_30P || fmt == AHD_8M_12_5P || fmt == AHD_8M_15P ||\
		fmt == CVI_4M_25P || fmt == CVI_4M_30P || fmt == CVI_8M_12_5P || fmt == CVI_8M_15P ||\
		fmt == TVI_4M_25P || fmt == TVI_4M_30P || fmt == TVI_8M_12_5P || fmt == TVI_8M_15P )
	{
		NC_U8 mask = 0;


		// 1. Post YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x69);
		mask = mask|(0x01<<4);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x69, mask);

		// 2. TM Mux_YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xed);
		mask = mask|(0x01<<chn);   // set 1
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xed, mask);
	}
	else
	{
		NC_U8 mask = 0;

		// 1. Post YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x69);
		mask = mask&0xEF; // Set 0[4] 0xEF 1110 1111
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x69, mask);

		// 2. TM Mux_YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xed);
		mask = mask&~(0x01<<chn);	// set 0[3:0]
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xed, mask);
	}

	/********************************************************************************
	 * Video Input Set
	 ********************************************************************************/
	nc_drv_video_input_set(pVdInfo);
}

void nc_drv_video_output_port_set( void *pParam )
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = 0;

	NC_VO_WORK_MODE_E mux		= pVdInfo->VO_MuxMode;
//	NC_VO_INTF_MODE_E interface = pVdInfo->VO_Interface;

	NC_DECODER_CHIP_MODEL_E chip_id = nc_drv_common_info_chip_id_get(dev);
	/**************************************************************
	 * Video Output_Mux Setting
	 **************************************************************/
	if(chip_id == NC_DECODER_6168)
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc, 0x58);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcd, 0x58);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xce, 0x58);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcf, 0x58);

		if( mux == NC_VO_WORK_MODE_1MUX )
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x10);

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x00); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x00); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0xFF); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]
#if 1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x00); // Port A - chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x11); // Port B - chn2
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x11);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x22); // Port C - chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x22);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x33); // Port D - chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x33);
#else
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x33|0x88); // Port A - chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x33|0x88);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x22|0x88); // Port B - chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x22|0x88);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x11|0x88); // Port C - chn2
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x11|0x88);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x00|0x88); // Port D - chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x00|0x88);
#endif
			//NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + 0, 0x58);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + 1, 0x58);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + 2, 0x58);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + 3, 0x58);
		}
		else if( mux == NC_VO_WORK_MODE_2MUX )
		{
			/***************************************************
			 * Vi Channel 0, 1
			 ***************************************************/
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x10);

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x22); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x22); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0xFF);
	//		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0x66); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x10); // Port A - chn2, chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x32); // Port B - chn2, chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x10); // Port C - chn4, chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x32); // Port D - chn4, chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x00);
		}
		else if( mux == NC_VO_WORK_MODE_4MUX )
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x32);

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x88); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x88); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0xFF);
	//		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0x11); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x10); // Port A - chn1, chn2, chn3, chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x32);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x10); // Port B - chn1, chn2, chn3, chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x32);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x10); // Port C - chn1, chn2, chn3, chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x32);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x10); // Port D - chn1, chn2, chn3, chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x32);
		}
		else
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x00); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x00); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0xFF); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x00); // Port A - chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x11); // Port B - chn2
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x11);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x22); // Port C - chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x22);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x33); // Port D - chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x33);
		}
	}
	else if(chip_id == NC_DECODER_6168C)
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcd, 0x58);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xce, 0x58);

		if( mux == NC_VO_WORK_MODE_1MUX )
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x10);

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x00); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x00); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0xFF); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x00); // Port A - chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x11); // Port B - chn2
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x11);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x22); // Port C - chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x22);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x33); // Port D - chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x33);
		}
		else if( mux == NC_VO_WORK_MODE_2MUX )
		{
			/***************************************************
			 * Vi Channel 0, 1
			 ***************************************************/
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x10);

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x22); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x22); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0x66); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x10); // Port B - chn2, chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x32); // Port C - chn4, chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x00);
		}
		else if( mux == NC_VO_WORK_MODE_4MUX )
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x32);

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x88); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x88); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0x22); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x10); // Port B - chn1, chn2, chn3, chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x32);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x10); // Port c - chn1, chn2, chn3, chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x32);
		}
		else
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8, 0x00); // [7:4] B Port, [3:0] A Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC9, 0x00); // [7:4] D Port, [3:0] C Port
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, 0xFF); // [7:4] Clk_EN  [3:0] VD Out EN [4,3,2,1], [4,3,2,1]

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0, 0x00); // Port A - chn1
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC2, 0x11); // Port B - chn2
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC3, 0x11);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC4, 0x22); // Port C - chn3
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC5, 0x22);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC6, 0x33); // Port D - chn4
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC7, 0x33);
		}
	}

	//printk("[%s::%d]Device (%d), ID(%x), MuxMode %d\n", __func__, __LINE__, dev, chip_id, mux);
	for(chn = dev*4; chn<(dev*4)+4; chn++)
		nc_drv_common_info_vo_mode_set(chn, mux);
}

void nc_drv_video_output_port_manual_set( void *pParam )
{
	NC_S32 ret = 0;
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;

	NC_VO_WORK_MODE_E mux		= pVdInfo->VO_MuxMode;
	NC_VO_INTF_MODE_E interface = pVdInfo->VO_Interface;

	NC_U8 mux_mask = 0;
	NC_U8 set_val  = 0;

	NC_U8 ChnSeq_21 = pVdInfo->VO_ChnSeq[1]<<4 | pVdInfo->VO_ChnSeq[0];
	NC_U8 ChnSeq_43 = pVdInfo->VO_ChnSeq[3]<<4 | pVdInfo->VO_ChnSeq[2];


	//printk("[%s]Chn(%d), Intf(%d), Mux(%d), ChnSeq(%02X, %02X)\n", __func__, chn, interface, mux, ChnSeq_21, ChnSeq_43);


	// common - device&channel number check
	if( (ret = nc_drv_common_total_chn_count_get(pVdInfo->Chn)) != 0 )
	{
		//printk("[%s]nc_drv_common_total_chn_count_get error!!TotalChn>>>%d\n", __func__, ret);
		return;
	}

	/**************************************************************************************************
	 * Interface Mode Setting
	 * 1. Post YC Merge
	 * 	  - 5x69[4] Post YC Merge
	 *   	0 : Y, C_Merge      : BT656
	 *   	1 : Y, C_Seperation : BT1120
	 * 2. TM Mux_YC Merge
	 * 	  - 1xED[3:0] -> [1 : 2 : 3 : 4]
	 *   	0 : Y+C, Y+C	: BT656
	 *   	1 : Y, C_Merge : BT1120
	 **************************************************************************************************/
	if( interface == NC_VO_MODE_BT656 )
	{
		NC_U8 mask = 0;

		// 1. Post YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x69);
		mask = mask&0xEF; // Set 0[4] 0xEF 1110 1111
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x69, mask);

		// 2. TM Mux_YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xed);
		mask = mask&~(0x01<<chn);	// set 0[3:0]
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xed, mask);
	}
	else if( interface == NC_VO_MODE_BT1120_STANDARD )
	{
		NC_U8 mask = 0;

		// 1. Post YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x69);
		mask = mask|(0x01<<4);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x69, mask);

		// 2. TM Mux_YC Merge
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xed);
		mask = mask|(0x01<<chn);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xed, mask);
	}

	/**************************************************************************************************
	 * Mux_Mode Setting
	 * 1. VPORT_CH_OUT_SEL
	 *    - 1xC8 [7:4], [3:0] -> [2 : 1]
	 *      1xC9 [7:4], [3:0] -> [4 : 3]
	 * 2. VCLK, VDO EN
	 *    - 1xCA [7:4] - VCLK[4, 3, 2, 1]
	 *   		 [3:0] -  VDO[4, 3, 2, 1]
	 **************************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
	mux_mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xc8 + (chn/2));

	if( mux == NC_VO_WORK_MODE_1MUX )
	{
		set_val = 0x0;

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x32);
	}
	else if( mux == NC_VO_WORK_MODE_2MUX )
	{
		set_val = 0x2;

		if(ChnSeq_21 == 0x32)
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x10);
		}
		else if(ChnSeq_21 == 0x21)
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x11);
		}
		else if(ChnSeq_21 == 0x10)
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x32);
		}
	}
	else if( mux == NC_VO_WORK_MODE_4MUX )
	{
		set_val = 0x8;

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x32);
	}
	else
		//printk("[%s::%d]%d::Error!!\n", __func__, __LINE__, mux);

	if( chn%2 )
	{
		// Channel 1, 3
		mux_mask = mux_mask&0x0F;
		mux_mask = mux_mask|(set_val<<4);
	}
	else
	{
		// Channel 0, 2
		mux_mask = mux_mask&0xF0;
		mux_mask = mux_mask|(set_val);
	}

	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC8 + (chn/2), mux_mask);

	mux_mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xCA);
	mux_mask = mux_mask|(0x11<<chn);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xCA, mux_mask);

	/**************************************************************************************************
	 * Video Output Port Sequence Mode
	 * Example Port A
	 * 		1xC0 : VPORT1_SEQ2[7:4] VPORT1_SEQ1[3:0]
	 * 		1xC0 : VPORT1_SEQ4[7:4] VPORT1_SEQ3[3:0]
	 *
	 * Normal Display of Channel 1 ~ 4 ( 0x00 ~ 0x04 )
	 * Only Y Display of Channel 1 ~ 4 ( 0x04 ~ 0x07 )
	 * H_CIF Display of Channel  1 ~ 4 ( 0x08 ~ 0x0B )
	 * Only C Display of Channel 1 ~ 4 ( 0x0C ~ 0x0F )
	 **************************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0 + (chn*2), ChnSeq_21);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1 + (chn*2), ChnSeq_43);

	nc_drv_common_info_vo_mode_set(pVdInfo->Chn, mux);
}

void nc_drv_video_output_color_pattern_set( void *pParam )
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;

	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;
	NC_U8 Set = pVdInfo->Value;

	NC_U8 bgdcol   = 0;
	NC_U8 chn_mask = 0;

	if( chn%2 )
		chn_mask = 0x0F; // chn_1_3
	else
		chn_mask = 0xF0; // chn_0_2

	if(Set)
	{
		// Color Pattern ON
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
#if 0 
		bgdcol = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x78 + (chn/2));
		bgdcol = bgdcol & chn_mask;
		if( chn%2 )
			bgdcol = bgdcol | (0x0A<<4);
		else
			bgdcol = bgdcol | 0x0A;
#else
		bgdcol = 0x88;
#endif

		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x78 + (chn/2), bgdcol);

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2c, 0x08);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x6a, 0x80);
	}
	else
	{
		// Color Pattern OFF
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
#if 0
		bgdcol = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x78 + (chn/2));
		bgdcol = bgdcol & chn_mask;
		if( chn%2 )
			bgdcol = bgdcol | (0x08<<4);
		else
			bgdcol = bgdcol | 0x08;
#else
		bgdcol = 0x88;
#endif
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x78 + (chn/2), bgdcol);

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2c, 0x00);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x6a, 0x00);
	}
}

void nc_drv_video_auto_manual_mode_set(void *pParam)
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;
	NC_U8 dev = pVdInfo->Chn/4;
	NC_VIDEO_SET_MODE_E OpMode = pVdInfo->OP_Mode;

	NC_U8 Chn = 0;

	if(OpMode == NC_VIDEO_SET_MODE_AUTO)
	{
		for(Chn=0; Chn <16; Chn++)
		{
			nc_drv_common_info_video_format_set(Chn, 0xee);
			nc_drv_common_info_coax_fw_status_set(Chn, 0);
		}

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_13);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x30, 0x7f);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x70, 0xf0);

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x00, 0x18);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x01, 0x18);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x02, 0x18);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x03, 0x18);

		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x00, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x01, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x02, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x03, 0x10);
	}
	else if(OpMode == NC_VIDEO_SET_MODE_MANUAL)
	{
		for(Chn=0; Chn <16; Chn++)
		{
			nc_drv_common_info_coax_fw_status_set(Chn, 1);
		}

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_13);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x30, 0x00);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x70, 0x00);
	}
	else
	{
		//printk("[%s::%d]Unknown Video Set Mode!!! %d\n", __FILE__, __LINE__, OpMode);
	}
}

void nc_drv_video_color_tune_get(void *pParam)
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;
	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;

	NC_VIDEO_COLOR_TUNE_E sel = pVdInfo->VD_Color_Tune_Sel;
	NC_U8 val = 0;

	if( sel == NC_VIDEO_BRIGHTNESS )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		val = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x78 + chn);
	}
	else if( sel == NC_VIDEO_CONTRAST )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		val = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x10 + chn);
	}
	else if( sel == NC_VIDEO_HUE )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		val = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x40 + chn);
	}
	else if( sel == NC_VIDEO_SATURATION )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		val = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x3c + chn);
	}
	else if( sel == NC_VIDEO_H_DELAY )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		val = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x58 + chn);
	}
	else
	{
		//printk("[%s]Unknown Color Tuning Selection!!\n", __func__ );
	}

	 pVdInfo->Value = val;
}

void nc_drv_video_color_tune_set(void *pParam)
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;
	NC_U8 dev = pVdInfo->Chn/4;
	NC_U8 chn = pVdInfo->Chn%4;

	NC_VIDEO_COLOR_TUNE_E sel = pVdInfo->VD_Color_Tune_Sel;
	NC_U8 val = pVdInfo->Value;

	if( sel == NC_VIDEO_BRIGHTNESS )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x0c + chn, val);
	}
	else if( sel == NC_VIDEO_CONTRAST )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x10 + chn, val);
	}
	else if( sel == NC_VIDEO_HUE )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x40 + chn, val);
	}
	else if( sel == NC_VIDEO_SATURATION )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3c + chn, val);
	}
	else if( sel == NC_VIDEO_H_DELAY )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x58 + chn, val);
	}
	else
	{
		//printk("[%s]Unknown Color Tuning Selection!!\n", __func__ );
	}
}

/***************************************************************************************
 * Internal Function
 *
 ***************************************************************************************/
void internal_nc_drv_video_input_format_set(NC_U8 dev, NC_U8 chn, NC_VO_WORK_MODE_E mux_mode, NC_VIDEO_FMT_INIT_TABLE_S *stTableVideo)
{
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x81 + chn, stTableVideo->nc_table_fmt_std_mode_set_0x81_sd_ahd);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x85 + chn, stTableVideo->nc_table_fmt_std_mode_set_0x85_special);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x89 + chn, stTableVideo->nc_table_fmt_std_timing_0x89_h_delay_b);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x8e + chn, stTableVideo->nc_table_fmt_std_timing_0x8e_h_delay_c);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xa0 + chn, stTableVideo->nc_table_fmt_std_timing_0xa0_y_delay);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x64 + chn, stTableVideo->nc_table_fmt_std_timing_0x64_v_delay);
	// Bank0 Added
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x08 + chn, stTableVideo->nc_table_fmt_std_0x08_video_format);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x0c + chn, stTableVideo->nc_table_fmt_std_color_0x0c_brightness);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x14 + chn, stTableVideo->nc_table_fmt_std_0x14_h_v_sharpness);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x23 + (chn*0x04), stTableVideo->nc_table_fmt_std_0x23_novideo_det);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x34 + chn, stTableVideo->nc_table_fmt_std_0x34_linemem_md);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3c + chn, stTableVideo->nc_table_fmt_std_0x3c_saturation);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x5c + chn, stTableVideo->nc_table_fmt_std_0x5c_v_delay);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x30 + chn, stTableVideo->nc_table_fmt_std_0x30_y_delay);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xa4 + chn, stTableVideo->nc_table_fmt_std_0xa4_y_c_delay2);

	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x84 + chn, stTableVideo->nc_table_fmt_std_clock_1x84_vadc);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x8c + chn, stTableVideo->nc_table_fmt_std_clock_1x8c_post_pre);
	// Add
#if 0
	if(mux_mode == NC_VO_WORK_MODE_1MUX)
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + chn, stTableVideo->nc_table_fmt_std_1xcc_vport_oclk_sel);
#endif
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x24, stTableVideo->nc_table_fmt_std_color_5x24_burst_dec_a);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x5f, stTableVideo->nc_table_fmt_std_color_5x5f_burst_dec_b);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xd1, stTableVideo->nc_table_fmt_std_color_5xd1_burst_dec_c);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x90, stTableVideo->nc_table_fmt_std_timing_5x90_comb_mode);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xb9, stTableVideo->nc_table_fmt_std_timing_5xb9_h_pll_op_a);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x57, stTableVideo->nc_table_fmt_std_timing_5x57_mem_path);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x25, stTableVideo->nc_table_fmt_std_timing_5x25_fsc_lock_speed);
	// Bank5 Add
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x00, stTableVideo->nc_table_fmt_std_5x00_a_cmp);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x1b, stTableVideo->nc_table_fmt_std_5x1b_comb_th_a);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x1f, stTableVideo->nc_table_fmt_std_5x1f_y_c_gain);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x22, stTableVideo->nc_table_fmt_std_5x22_u_v_offset2);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x26, stTableVideo->nc_table_fmt_std_5x26_fsc_lock_sense);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x50, stTableVideo->nc_table_fmt_std_5x50_none_);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x5b, stTableVideo->nc_table_fmt_std_5x5b_vafe);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x64, stTableVideo->nc_table_fmt_std_5x64_mem_rdp_01);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xa9, stTableVideo->nc_table_fmt_std_5xa9_adv_stp_delay1);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xc8, stTableVideo->nc_table_fmt_std_5xc8_y_sort_sel);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x6e, stTableVideo->nc_table_fmt_std_5x6e_v_delay_ex_on);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x6f, stTableVideo->nc_table_fmt_std_5x6f_v_delay_val);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7b, stTableVideo->nc_table_fmt_std_5x7b_v_rst_point);

	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_9);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x96 + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x96_h_scaler1);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x98 + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x98_h_scaler3);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x99 + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x99_h_scaler4);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x9a + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x9a_h_scaler5);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x9b + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x9b_h_scaler6);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x9c + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x9c_h_scaler7);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x9d + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x9d_h_scaler8);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x9e + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x9e_h_scaler9);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x97 + (chn * 0x20), stTableVideo->nc_table_fmt_std_timing_9x97_h_scaler2);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x40 + chn, stTableVideo->nc_table_fmt_std_timing_9x40_pn_auto);
	// Bank9 Add
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x50 + (chn*0x04), stTableVideo->nc_table_fmt_std_9x50_fsc_ext_val_1_7_0);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x51 + (chn*0x04), stTableVideo->nc_table_fmt_std_9x51_fsc_ext_val_1_15_8);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x52 + (chn*0x04), stTableVideo->nc_table_fmt_std_9x52_fsc_ext_val_1_23_16);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x53 + (chn*0x04), stTableVideo->nc_table_fmt_std_9x53_fsc_ext_val_1_31_24);

	NC_DEVICE_DRIVER_BANK_SET(dev,  (chn < 2 ? BANK_A : BANK_B));
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x25 + (chn%2 * 0x80), stTableVideo->nc_table_fmt_std_color_ax25_y_filter_b);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x27 + (chn%2 * 0x80), stTableVideo->nc_table_fmt_std_color_ax27_y_filter_b_sel);

}

NC_U32 internal_nc_drv_video_distance_hsync_distortion_get( NC_U8 dev, NC_U8 chn, NC_U32 *Sam1Frame, NC_U32 *Sam8Frame )
{
	NC_U32 hsync_distortion_sum = 0;
	NC_U32 hsync_distortion_falling = 0;
	NC_U32 hsync_distortion_rising = 0;
	NC_U8 hsync_7_0 = 0;
	NC_U8 hsync_15_8 = 0;
	NC_U8 hsync_23_16 = 0;
	NC_U8 hsync_31_24 = 0;

	/***************************************************************************************
	 * hsync_falling & rising distortion read - 1 frame
	 ***************************************************************************************/
	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_13);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2B, chn);

	// 1. falling distortion read
	hsync_7_0   = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD0);
	hsync_15_8  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD1);
	hsync_23_16 = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD2);
	hsync_31_24 = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD3);
	hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

	// 2. rising distortion read
	hsync_7_0   = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD4);
	hsync_15_8  = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD5);
	hsync_23_16 = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD6);
	hsync_31_24 = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0xD7);
	hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

	// 3. falling - rising
	hsync_distortion_sum = hsync_distortion_falling - hsync_distortion_rising;

	return hsync_distortion_sum;
}

void internal_nc_drv_video_sd_format_set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt)
{
	NC_U8 chn_mask = 0;

	NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
	if( fmt == SD_H960_2EX_Btype_NT || fmt == SD_H960_2EX_NT || fmt == SD_H960_NT )
	{
		chn_mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x54);
		// chn_bit 0 -> 1
		chn_mask = chn_mask|(0x01<<(4+chn));
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x54, chn_mask);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x08 + chn, 0xa0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x5c + chn, 0xd0);
	}
	else if( fmt == SD_H960_2EX_Btype_PAL || fmt == SD_H960_2EX_PAL || fmt == SD_H960_PAL )
	{
		chn_mask = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x54);
		if(chn_mask & 0x01<<(4+chn))
		{
			// if chn_bit 1 -> 0
			chn_mask = chn_mask ^ (0x01<<(4+chn));
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x54, chn_mask);
		}
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x08 + chn, 0xdd);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x5c + chn, 0xbf);
	}

	if( fmt == SD_H960_NT  || fmt == SD_H960_EX_NT  || fmt == SD_H960_2EX_NT ||\
		fmt == SD_H960_PAL || fmt == SD_H960_EX_PAL || fmt == SD_H960_2EX_PAL )
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, 0x05 + chn);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xB8, 0xb9);
	}
	else
	{
		NC_DEVICE_DRIVER_BANK_SET(dev, 0x05 + chn);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xB8, 0x39);
	}
}

void internal_nc_drv_video_output_hide_set( NC_U8 Dev, NC_U8 Chn, NC_U8 Val )
{
	NC_U8 dev = Dev;
	NC_U8 chn = Chn;
	NC_U8 Set = Val;

	NC_U8 output_mode = 0;
	NC_U8 chn_mask    = 0;

	// 0x01 : Y(001~254), Cb_(001~254), Cr_(001~254)
	// 0x0F : Background color output  <<<<<  BGDCOL 0x78, 0x79

	if( chn%2 )
		chn_mask = 0x0F; // chn_1_3
	else
		chn_mask = 0xF0; // chn_0_2

	NC_DEVICE_DRIVER_BANK_SET(dev, 0x00);

	if( chn == 0 || chn == 1 )
	{
		output_mode = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7A);
		output_mode = output_mode & chn_mask;
		output_mode = output_mode | Set;
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7A, output_mode);
	}
	else if( chn == 2 || chn == 3 )
	{
		output_mode = gpio_i2c_read(g_nc_drv_i2c_addr[dev], 0x7B);
		output_mode = output_mode & chn_mask;
		output_mode = output_mode | Set;
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7B, output_mode);
	}
	else
	{
		//printk("[%s]Error!!\n", __func__);
		return;
	}
}

#if 0
// kukuri added
void nc_drv_video_output_port_seq_set( void *pParam )
{
	nc_decoder_s *pVdInfo = (nc_decoder_s*)pParam;
	NC_U8 ii;
	NC_VIVO_CH_FORMATDEF_E fmt;//   = nc_drv_common_info_video_fmt_def_get(info_chn);
	NC_U8 seq;

	NC_DEVICE_DRIVER_BANK_SET(0, BANK_1);
	for(ii = 0; ii < 4; ii++)
	{
		fmt = nc_drv_common_info_video_fmt_def_get(pVdInfo->Chn);

		if(fmt == AHD_720P_30P_EX_Btype|| fmt == AHD_720P_25P_EX_Btype)
		{
			seq = ((pVdInfo->VO_ChnSeq[ii]+8)<<4) | (pVdInfo->VO_ChnSeq[ii]+8);
		}
		else
		{
			seq = pVdInfo->VO_ChnSeq[ii]<<4 | pVdInfo->VO_ChnSeq[ii];
		}
		gpio_i2c_write(g_nc_drv_i2c_addr[0], 0xC0 + (ii*2), seq);
		gpio_i2c_write(g_nc_drv_i2c_addr[0], 0xC1 + (ii*2), seq);
	}
}
#endif
