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
#include <linux/string.h>
#include <linux/delay.h>
#include "common.h"
#include "video_input.h"
#include "video_input_table.h"

extern unsigned int raptor3_i2c_addr[4];

void video_input_h_timing_set(decoder_dev_ch_info_s *pDevChInfo)
{
	unsigned char val_0x54;
	video_input_basic_timing_init_s h_timing = (video_input_basic_timing_init_s)decoder_basic_timing_fmtdef[ pDevChInfo->fmt_def ];

	if(pDevChInfo->ch>3)
	{
		printk("[DRV] %s CHID Error\n", __func__);
		return;
	}


	//B0
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xFF, 0x00);
	//H_DELAY_LSB B0 0x58/9/a/b
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x58 + pDevChInfo->ch, h_timing.h_delay_lsb);
	//H_MASK_OB B0 0x89/a/b/c
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x89 + pDevChInfo->ch, h_timing.h_mask_on);
	//H_MASK_SEL B0 0x8E/F/90/91
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x8e + pDevChInfo->ch, h_timing.h_mask_sel);

	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xA0 + pDevChInfo->ch, h_timing.v_blk_end_b);

	val_0x54 = gpio_i2c_read(raptor3_i2c_addr[pDevChInfo->devnum], 0x54);
	val_0x54 &= ~((0x1) << (pDevChInfo->ch + 4));
	val_0x54 |= ((h_timing.fld_inv & 0x1) << (pDevChInfo->ch + 4));

	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x54, val_0x54);

	//B5/6/7/8
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xFF, 0x05 + pDevChInfo->ch);
	//SYNC_RS B5/6/7/8 0x47
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x47, h_timing.sync_rs);
	//MEM_RDP B5/6/7/8 0x64
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x64, h_timing.mem_rdp);

	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xA9, h_timing.v_blk_end_b);
}


void video_input_hpll_set(decoder_dev_ch_info_s *pDevChInfo)
{
	video_input_basic_hpll_init_s hpll = (video_input_basic_hpll_init_s) decoder_basic_hpll_fmtdef[ pDevChInfo->fmt_def ];
	if(pDevChInfo->ch>3)
	{
		printk("[DRV] %s CHID Error\n", __func__);
		return;
	}


	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xFF, 0x05 + pDevChInfo->ch);

	//B5/6/7/8 0x50 DECI_BYPASS, HPLL_MASK_ON
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0x50, hpll.hpll_mask_on);
	// unsigned char op_md;   					//B5/6/7/8 0xB8
	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xB8, hpll.hafc_op_md);

	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xBB, hpll.hafc_byp_th_e);

	gpio_i2c_write(raptor3_i2c_addr[pDevChInfo->devnum], 0xB7, hpll.hafc_byp_th_s);
}

void video_input_vafe_init(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char val_5678x00;
	unsigned char val_5678x01;
	unsigned char val_5678x58;
	unsigned char val_5678x59;
	unsigned char val_5678x5B;
	unsigned char val_5678x5C;

	video_input_vafe_init_s afe = (video_input_vafe_init_s) decoder_afe_fmtdef [decoder_info->fmt_def];

	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xFF, 0x00);

	//B0 0x00/1/2/3 gain[4], powerdown[0]
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x00 + decoder_info->ch, ((afe.gain & 0x01) << 4) | (afe.powerdown & 0x01));

	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xFF, 0x01);
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x84 + decoder_info->ch, 0x00);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//B5/6/7/8
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xFF, 0x05 + decoder_info->ch);

	//B5/6/7/8 0x01 spd[2], lpf_back_band[1:0]
	val_5678x00 = gpio_i2c_read(raptor3_i2c_addr[decoder_info->devnum], 0x00);
	val_5678x00 |= (afe.spd << 4);

	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x00, val_5678x00);

	val_5678x01 = ((afe.ctrlreg << 6) | (afe.ctrlibs << 4) | (afe.adcspd << 2) | (afe.clplevel));
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x01, val_5678x01 );

	//B5/6/7/8 0x58 eq_band[7:4], lpf_front_band[1:0]
	val_5678x58 = ((afe.eq_band << 4) | (afe.lpf_front_band));
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x58, val_5678x58);

		//B5/6/7/8 0x5B ref_vol[1:0]
	val_5678x59 = ((afe.clpmode << 7) | (afe.f_lpf_bypass << 4) | (afe.clproff << 3) | (afe.b_lpf_bypass));
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x59, val_5678x59);

	val_5678x5B = ((afe.duty << 4) | (afe.ref_vol));
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x5B, val_5678x5B);

	val_5678x5C = ((afe.lpf_back_band << 4) | (afe.clk_sel << 3) | (afe.eq_gainsel));
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x5C, val_5678x5C);
}

void video_input_color_set(decoder_dev_ch_info_s *decoder_info)
{
	//unsigned char val_9x44; // This IF sentense
	if(decoder_info->fmt_def == AHD20_1080P_30P || decoder_info->fmt_def == AHD20_1080P_25P ||
		decoder_info->fmt_def == CVI_FHD_30P || decoder_info->fmt_def == CVI_FHD_25P ||
		decoder_info->fmt_def == TVI_FHD_30P || decoder_info->fmt_def == TVI_FHD_25P ||
	   decoder_info->fmt_def == AHD30_4M_30P 	|| decoder_info->fmt_def == AHD30_4M_25P 	|| decoder_info->fmt_def == AHD30_4M_15P ||
	   decoder_info->fmt_def == CVI_4M_30P 		|| decoder_info->fmt_def == CVI_4M_25P 		||
	   decoder_info->fmt_def == TVI_4M_30P 		|| decoder_info->fmt_def == TVI_4M_25P
	   || decoder_info->fmt_def == TVI_5M_20P
	)
	{
		video_input_color_init_s color_init = (video_input_color_init_s)decoder_color_fmtdef[decoder_info->fmt_def];

		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xff, 0x00 );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x0c + decoder_info->ch, color_init.brightnees);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x10 + decoder_info->ch, color_init.contrast );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x22 + (decoder_info->ch*4), 0x0B );				// FIxed Value
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x3c + decoder_info->ch, color_init.saturation_a );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x40 + decoder_info->ch, color_init.hue      );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x44 + decoder_info->ch, color_init.u_gain   );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x48 + decoder_info->ch, color_init.v_gain   );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x4c + decoder_info->ch, color_init.u_offset );
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x50 + decoder_info->ch, color_init.v_offset );

		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xff, 0x05 + decoder_info->ch);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x2b, color_init.saturation_b);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x20, color_init.black_level);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x24, color_init.burst_dec_a);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x5f, color_init.burst_dec_b);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xd1, color_init.burst_dec_c);
	}
	else
	{

	}

}

void video_input_hafc_gain_ctrl(decoder_dev_ch_info_s *decoder_info)
{
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xFF, 0x05 + decoder_info->ch);

	if(decoder_info->fmt_def == NC_VIVO_CH_FORMATDEF_UNKNOWN)
	{
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xB9, 0xB2);
	}
	else
	{
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xB9, 0x72);
	}
}


