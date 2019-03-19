/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: video_auto_detect.c
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
#include "video_eq.h"
#include "video_eq_table.h"

extern unsigned int raptor3_i2c_addr[4];

unsigned char __video_cable_manualdistance( unsigned char cabletype, video_input_hsync_accum *pvin_hsync_accum, video_input_acc_gain_val *pvin_acc_val, video_equalizer_distance_table_s *pdistance_value )
{
	int i = 0;
	unsigned char distance = 0; /* default : short(0) */

	/* for coaxial */
	if( cabletype == 0 )
	{
		for( i = 0; i < 9; i++ )
		{
			if( (pvin_hsync_accum->hsync_accum_result > pdistance_value->hsync_stage.hsync_stage[i]) )
			{
				distance = i;
				break;
			}

		}
		if( i == 9 )
		{
			distance = 5;
		}
	}

	if( pvin_hsync_accum->hsync_accum_result == 0 )
	{
		distance = 0; 
	}

	printk("DRV[%s:%d] CH:%d, distance:%d\n", __func__, __LINE__, pvin_hsync_accum->ch, distance );

	return distance;
}

void __eq_base_set_value( video_equalizer_info_s *pvin_eq_set, video_equalizer_base_s *pbase )
{
	unsigned char devnum = pvin_eq_set->devnum;
	unsigned char ch = pvin_eq_set->Ch;
	unsigned char dist = pvin_eq_set->distance;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x01, pbase->eq_bypass[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x58, pbase->eq_band_sel[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5C, pbase->eq_gain_sel[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, (ch < 2 ? 0x0a : 0x0b) );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3d + (ch%2 * 0x80), pbase->deq_a_on[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + (ch%2 * 0x80), pbase->deq_a_sel[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x09 );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x80 + (ch * 0x20), pbase->deq_b_sel[dist] );

#ifdef __DEBUG_MSG 
	printk("ch[%d]: BASE, dist:%d, eq_bypass[%02x]\n", ch, dist, pbase->eq_bypass[dist] );
	printk("ch[%d]: BASE, dist:%d, eq_band_sel[%02x]\n", ch, dist, pbase->eq_band_sel[dist] );
	printk("ch[%d]: BASE, dist:%d, eq_gain_sel[%02x]\n", ch, dist, pbase->eq_gain_sel[dist] );
	printk("ch[%d]: BASE, dist:%d, deq_a_on[%02x]\n", ch, dist, pbase->deq_a_on[dist] );
	printk("ch[%d]: BASE, dist:%d, deq_a_sel[%02x]\n", ch, dist, pbase->deq_a_sel[dist] );
	printk("ch[%d]: BASE, dist:%d, deq_b_sel[%02x]\n", ch, dist, pbase->deq_b_sel[dist] );
#endif
}

void __eq_coeff_set_value( video_equalizer_info_s *pvin_eq_set, video_equalizer_coeff_s *pcoeff )
{
	unsigned char devnum = pvin_eq_set->devnum;
	unsigned char ch = pvin_eq_set->Ch;
	unsigned char dist = pvin_eq_set->distance;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, (ch < 2 ? 0x0a : 0x0b) );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x30 + (ch%2 * 0x80), pcoeff->deqA_01[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x31 + (ch%2 * 0x80), pcoeff->deqA_02[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x32 + (ch%2 * 0x80), pcoeff->deqA_03[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x33 + (ch%2 * 0x80), pcoeff->deqA_04[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x34 + (ch%2 * 0x80), pcoeff->deqA_05[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x35 + (ch%2 * 0x80), pcoeff->deqA_06[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x36 + (ch%2 * 0x80), pcoeff->deqA_07[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x37 + (ch%2 * 0x80), pcoeff->deqA_08[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x38 + (ch%2 * 0x80), pcoeff->deqA_09[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x39 + (ch%2 * 0x80), pcoeff->deqA_10[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3a + (ch%2 * 0x80), pcoeff->deqA_11[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3b + (ch%2 * 0x80), pcoeff->deqA_12[dist] );

#ifdef __DEBUG_MSG 
	printk("ch[%d]: COEFF, dist:%d, deqA_01[%02x]\n", ch, dist, pcoeff->deqA_01[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_02[%02x]\n", ch, dist, pcoeff->deqA_02[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_03[%02x]\n", ch, dist, pcoeff->deqA_03[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_04[%02x]\n", ch, dist, pcoeff->deqA_04[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_05[%02x]\n", ch, dist, pcoeff->deqA_05[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_06[%02x]\n", ch, dist, pcoeff->deqA_06[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_07[%02x]\n", ch, dist, pcoeff->deqA_07[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_08[%02x]\n", ch, dist, pcoeff->deqA_08[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_09[%02x]\n", ch, dist, pcoeff->deqA_09[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_10[%02x]\n", ch, dist, pcoeff->deqA_10[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_11[%02x]\n", ch, dist, pcoeff->deqA_11[dist] );
	printk("ch[%d]: COEFF, dist:%d, deqA_12[%02x]\n", ch, dist, pcoeff->deqA_12[dist] );
#endif
}

void __eq_color_set_value( video_equalizer_info_s *pvin_eq_set, video_equalizer_color_s *pcolor )
{
	unsigned char devnum = pvin_eq_set->devnum;
	unsigned char ch = pvin_eq_set->Ch;
	unsigned char dist = pvin_eq_set->distance;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x10 + ch, pcolor->contrast[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x18 + ch, pcolor->h_peaking[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x21 + ch*4, pcolor->c_filter[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x40 + ch, pcolor->hue[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x44 + ch, pcolor->u_gain[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x48 + ch, pcolor->v_gain[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x4C + ch, pcolor->u_offset[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50 + ch, pcolor->v_offset[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x20, pcolor->black_level[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x27, pcolor->acc_ref[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x28, pcolor->cti_delay[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2b, pcolor->sub_saturation[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x24, pcolor->burst_dec_a[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5f, pcolor->burst_dec_b[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xd1, pcolor->burst_dec_c[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xd5, pcolor->c_option[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, (ch < 2 ? 0x0a : 0x0b) );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x25 + (ch%2 * 0x80), pcolor->y_filter_b[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x27 + (ch%2 * 0x80), pcolor->y_filter_b_sel[dist] );

	if( pvin_eq_set->FmtDef == TVI_HD_B_30P_EX || pvin_eq_set->FmtDef == TVI_HD_B_25P_EX )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 : case 4 :
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf0 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			case 6 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
			case 7 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf0 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			case 8 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xe0 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			case 9 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == TVI_FHD_30P || pvin_eq_set->FmtDef == TVI_FHD_25P ||
		pvin_eq_set->FmtDef == TVI_HD_30P_EX || pvin_eq_set->FmtDef == TVI_HD_25P_EX
		)
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf8 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			case 1 : case 2 :
			case 3 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf4 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			case 4 :
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf8 );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
					 break;
			case 6 :
			case 7 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == TVI_5M_20P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 : 
			case 4 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf0 );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );

				 	 gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1f, 0x8f );
					 break;
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );

					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1f, 0x8f );
					 break;
		}
	}

	if( pvin_eq_set->FmtDef == TVI_5M_12_5P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 : case 4:
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf8 );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
					 break;
		}
	}

	if( pvin_eq_set->FmtDef == TVI_4M_25P || pvin_eq_set->FmtDef == TVI_4M_30P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 :  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					  break;
			case 1 :  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf8 );
					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					  break;
			case 2 :  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf0 );
					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
		 			  gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x84 );
					  break;
			case 3 :
			case 4 :  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf0 );
					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
		 			  gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
 					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1f, 0x8f );
					  break;
			case 5 :  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
		 			  gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
 					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x22, 0x43 );
 					  gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1f, 0x8f );
					  break;

			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == TVI_8M_15P || pvin_eq_set->FmtDef == TVI_8M_12_5P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
	}

	if( pvin_eq_set->FmtDef == CVI_HD_25P_EX || pvin_eq_set->FmtDef == CVI_HD_30P_EX )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 :	case 4 : case 5 : case 6 : case 7 :
			case 8 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf0 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == CVI_FHD_25P || pvin_eq_set->FmtDef == CVI_FHD_30P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 :	case 4 : 
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0xf4 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == CVI_4M_25P || pvin_eq_set->FmtDef == CVI_4M_30P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 :	case 4 : 
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == AHD20_720P_25P_EX_Btype || pvin_eq_set->FmtDef == AHD20_720P_30P_EX_Btype )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 :	case 4 : case 5 : case 6 : case 7 : case 8 : case 9 :
			case 10 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == AHD20_1080P_25P || pvin_eq_set->FmtDef == AHD20_1080P_30P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 :	case 4 : 
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 break;
			default : break;
		}
	}

	if( pvin_eq_set->FmtDef == AHD30_4M_25P || pvin_eq_set->FmtDef == AHD30_4M_30P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
		switch(dist)
		{
			case 0 : case 1 : case 2 : case 3 :	case 4 : 
			case 5 : gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ch, 0x00 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0x3c + ch, 0x80 );
					 gpio_i2c_write(raptor3_i2c_addr[devnum], 0xa0 + ch, 0x00 );
					 break;
			default : break;
		}
	}

#ifdef __DEBUG_MSG
	printk("ch[%d]: COLOR, dist:%d, contrast[%02x]\n", ch, dist, pcolor->contrast[dist] );
	printk("ch[%d]: COLOR, dist:%d, h_peaking[%02x]\n", ch, dist, pcolor->h_peaking[dist] );
	printk("ch[%d]: COLOR, dist:%d, c_filter[%02x]\n", ch, dist, pcolor->c_filter[dist] );

	printk("ch[%d]: COLOR, dist:%d, hue[%02x]\n", ch, dist, pcolor->hue[dist] );
	printk("ch[%d]: COLOR, dist:%d, u_gain[%02x]\n", ch, dist, pcolor->u_gain[dist] );
	printk("ch[%d]: COLOR, dist:%d, v_gain[%02x]\n", ch, dist, pcolor->v_gain[dist] );
	printk("ch[%d]: COLOR, dist:%d, u_offset[%02x]\n", ch, dist, pcolor->u_offset[dist] );
	printk("ch[%d]: COLOR, dist:%d, v_offset[%02x]\n", ch, dist, pcolor->v_offset[dist] );

	printk("ch[%d]: COLOR, dist:%d, black_level[%02x]\n", ch, dist, pcolor->black_level[dist] );
	printk("ch[%d]: COLOR, dist:%d, cti_delay[%02x]\n", ch, dist, pcolor->cti_delay[dist] );
	printk("ch[%d]: COLOR, dist:%d, sub_saturation[%02x]\n", ch, dist, pcolor->sub_saturation[dist] );

	printk("ch[%d]: COLOR, dist:%d, burst_dec_a[%02x]\n", ch, dist, pcolor->burst_dec_a[dist] );
	printk("ch[%d]: COLOR, dist:%d, burst_dec_b[%02x]\n", ch, dist, pcolor->burst_dec_b[dist] );
	printk("ch[%d]: COLOR, dist:%d, burst_dec_c[%02x]\n", ch, dist, pcolor->burst_dec_c[dist] );

	printk("ch[%d]: COLOR, dist:%d, c_option[%02x]\n", ch, dist, pcolor->c_option[dist] );
#endif
}

void __eq_timing_a_set_value( video_equalizer_info_s *pvin_eq_set, video_equalizer_timing_a_s *ptiming_a )
{
	unsigned char devnum = pvin_eq_set->devnum;
	unsigned char ch = pvin_eq_set->Ch;
	unsigned char dist = pvin_eq_set->distance;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x00 );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x58 + ch, ptiming_a->h_delay_a[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x89 + ch, ptiming_a->h_delay_b[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x8e + ch, ptiming_a->h_delay_c[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xa0 + ch, ptiming_a->y_delay[dist] );

#ifdef __DEBUG_MSG
	printk("ch[%d]: TIMING_A, dist:%d, h_delay_a[%02x]\n", ch, dist, ptiming_a->h_delay_a[dist] );
	printk("ch[%d]: TIMING_A, dist:%d, h_delay_b[%02x]\n", ch, dist, ptiming_a->h_delay_b[dist] );
	printk("ch[%d]: TIMING_A, dist:%d, h_delay_c[%02x]\n", ch, dist, ptiming_a->h_delay_c[dist] );
	printk("ch[%d]: TIMING_A, dist:%d, y_delay[%02x]\n", ch, dist, ptiming_a->y_delay[dist] );
#endif
}

void __eq_clk_set_value( video_equalizer_info_s *pvin_eq_set, video_equalizer_clk_s *pclk )
{
	unsigned char devnum = pvin_eq_set->devnum;
	unsigned char ch = pvin_eq_set->Ch;
	unsigned char dist = pvin_eq_set->distance;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x01 );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x84 + ch, pclk->clk_adc[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x8C + ch, pclk->clk_dec[dist] );
}
void __eq_timing_b_set_value( video_equalizer_info_s *pvin_eq_set, video_equalizer_timing_b_s *ptiming_b )
{
	unsigned char devnum = pvin_eq_set->devnum;
	unsigned char ch = pvin_eq_set->Ch;
	unsigned char dist = pvin_eq_set->distance;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x09 );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x96 + (ch * 0x20), ptiming_b->h_scaler1[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x97 + (ch * 0x20), ptiming_b->h_scaler2[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x98 + (ch * 0x20), ptiming_b->h_scaler3[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x99 + (ch * 0x20), ptiming_b->h_scaler4[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x9A + (ch * 0x20), ptiming_b->h_scaler5[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x9B + (ch * 0x20), ptiming_b->h_scaler6[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x9C + (ch * 0x20), ptiming_b->h_scaler7[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x9D + (ch * 0x20), ptiming_b->h_scaler8[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x9E + (ch * 0x20), ptiming_b->h_scaler9[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x40 + ch , 		 ptiming_b->pn_auto[dist] );

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x90, ptiming_b->comb_mode[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xb9, ptiming_b->h_pll_op_a[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x57, ptiming_b->mem_path[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x25, ptiming_b->fsc_lock_speed[dist] );


	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00 );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x81 + ch, ptiming_b->format_set1[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x85 + ch, ptiming_b->format_set2[dist] );
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x64 + ch, ptiming_b->v_delay[dist] );

#ifdef __DEBUG_MSG 
	printk("ch[%d]: TIMING_B, dist:%d, h_scaler1[%02x]\n", ch, dist, ptiming_b->h_scaler1[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, h_scaler2[%02x]\n", ch, dist, ptiming_b->h_scaler2[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, h_scaler3[%02x]\n", ch, dist, ptiming_b->h_scaler3[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, h_scaler4[%02x]\n", ch, dist, ptiming_b->h_scaler4[dist] );

	printk("ch[%d]: TIMING_B, dist:%d, pn_auto[%02x]\n", ch, dist, ptiming_b->pn_auto[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, comb_mode[%02x]\n", ch, dist, ptiming_b->comb_mode[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, h_pll_op_a[%02x]\n", ch, dist, ptiming_b->h_pll_op_a[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, mem_path[%02x]\n", ch, dist, ptiming_b->mem_path[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, format_set1[%02x]\n", ch, dist, ptiming_b->format_set1[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, format_set2[%02x]\n", ch, dist, ptiming_b->format_set2[dist] );
	printk("ch[%d]: TIMING_B, dist:%d, v_delay[%02x]\n", ch, dist, ptiming_b->v_delay[dist] );
#endif

}

unsigned int __get_acc_gain(unsigned char ch, unsigned char devnum)
{
	unsigned int acc_gain_status;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x05+ch%4);
	acc_gain_status = gpio_i2c_read(raptor3_i2c_addr[devnum],0xE2);
	acc_gain_status <<= 8;
	acc_gain_status |= gpio_i2c_read(raptor3_i2c_addr[devnum],0xE3);

	return acc_gain_status;
}

unsigned int __get_yplus_slope(unsigned char ch, unsigned char devnum)
{
	unsigned int y_plus_slp_status;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x05+ch%4);
	y_plus_slp_status = gpio_i2c_read(raptor3_i2c_addr[devnum],0xE8)&0x07;
	y_plus_slp_status <<= 8;
	y_plus_slp_status |= gpio_i2c_read(raptor3_i2c_addr[devnum],0xE9);

	return y_plus_slp_status;
}

unsigned int __get_yminus_slope(unsigned char ch, unsigned char devnum)
{
	unsigned int y_minus_slp_status;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x05+ch%4);
	y_minus_slp_status = gpio_i2c_read(raptor3_i2c_addr[devnum],0xEA)&0x07;
	y_minus_slp_status <<= 8;
	y_minus_slp_status |= gpio_i2c_read(raptor3_i2c_addr[devnum],0xEB);

	return y_minus_slp_status;
}

unsigned int __get_sync_width( unsigned char ch, unsigned char devnum )
{
	unsigned char	 reg_B0_E0 = 0;
	unsigned char	 agc_stable = 0;
	unsigned int	 sync_width = 0;
	unsigned int 	 check_timeout = 0;

	while(agc_stable == 0)
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x00);
		reg_B0_E0 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0xE0 )&0xF;
		agc_stable = reg_B0_E0 & (0x01 << (ch%4));

		if( check_timeout++ > 100 )
		{
			printk("DRV[%s:%d] CH:%d, TimeOut, AGC_stable[%x] check[%x] in get sync width\n", __func__, __LINE__, ch, reg_B0_E0, agc_stable );
			break;
		}
		msleep(1);
	}

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05+(ch%4));
	sync_width = gpio_i2c_read(raptor3_i2c_addr[devnum], 0xC4)&0x0F;
	sync_width <<=8;
	sync_width |= gpio_i2c_read(raptor3_i2c_addr[devnum], 0xC5);
	sync_width = sync_width & 0x0FFF;
	printk("DRV[%s:%d] CH:%d, sync_width:0x%x\n", __func__, __LINE__, ch, sync_width );

	return sync_width;
}

int video_input_cable_measure_way( unsigned char ch, unsigned char devnum )
{
	unsigned int  acc_gain;
	unsigned int  y_slope;
	unsigned char y_plus_slope;
	unsigned char y_minus_slope;
	unsigned int  sync_width;

	acc_gain = __get_acc_gain(ch, devnum);
	y_plus_slope = __get_yplus_slope(ch, devnum);
    y_minus_slope = __get_yminus_slope(ch, devnum);
    y_slope = y_plus_slope + y_minus_slope;
    sync_width = __get_sync_width(ch, devnum);

    printk("DRV[%s:%d] CH:%d, accgain=0x%x(%d), yslope=0x%x(%d), syncwidth=0x%x(%d)\n", \
    		__func__, __LINE__, ch, acc_gain, acc_gain, y_slope, y_slope, sync_width, sync_width );

    return 0;
}

void video_input_cable_manualdist_read(video_input_cable_dist *vin_cable_dist )
{
	video_input_acc_gain_val vin_acc;
	video_input_hsync_accum  vin_hsync_accum;

	/* cable type => 0:coaxial, 1:utp, 2:reserved1, 3:reserved2 */
	video_equalizer_distance_table_s distance_value = (video_equalizer_distance_table_s)equalizer_distance_fmtdef[vin_cable_dist->FmtDef];

	if( vin_cable_dist->FmtDef == CVI_4M_30P || vin_cable_dist->FmtDef == CVI_4M_25P 	||
		vin_cable_dist->FmtDef == CVI_8M_15P || vin_cable_dist->FmtDef == CVI_8M_12_5P 	||
		vin_cable_dist->FmtDef == CVI_FHD_30P				|| vin_cable_dist->FmtDef == CVI_FHD_25P			||
		vin_cable_dist->FmtDef == AHD30_4M_25P || vin_cable_dist->FmtDef == AHD30_4M_30P    ||
		vin_cable_dist->FmtDef == AHD30_5M_20P ||
		vin_cable_dist->FmtDef == TVI_4M_25P || vin_cable_dist->FmtDef == TVI_4M_30P ||
		vin_cable_dist->FmtDef == TVI_5M_12_5P || vin_cable_dist->FmtDef == TVI_3M_18P ||
		vin_cable_dist->FmtDef == AHD20_1080P_30P || vin_cable_dist->FmtDef == AHD20_1080P_25P ||
		vin_cable_dist->FmtDef == TVI_FHD_30P 			  	|| vin_cable_dist->FmtDef == TVI_FHD_25P 			||
		vin_cable_dist->FmtDef == AHD20_720P_30P_EX_Btype 	|| vin_cable_dist->FmtDef == AHD20_720P_25P_EX_Btype||
		vin_cable_dist->FmtDef == AHD20_720P_30P		 	|| vin_cable_dist->FmtDef == AHD20_720P_25P			||

		vin_cable_dist->FmtDef == CVI_HD_30P_EX           	|| vin_cable_dist->FmtDef == CVI_HD_25P_EX			||
		vin_cable_dist->FmtDef == CVI_HD_30P              	|| vin_cable_dist->FmtDef == CVI_HD_25P  			||

		vin_cable_dist->FmtDef == TVI_HD_30P_EX		   		|| vin_cable_dist->FmtDef == TVI_HD_25P_EX			||
		vin_cable_dist->FmtDef == TVI_HD_B_30P_EX		   	|| vin_cable_dist->FmtDef == TVI_HD_B_25P_EX		||

		vin_cable_dist->FmtDef == AHD30_5M_12_5P		   	|| vin_cable_dist->FmtDef == AHD30_4M_15P			||
		vin_cable_dist->FmtDef == AHD30_3M_25P		   		|| vin_cable_dist->FmtDef == AHD30_3M_30P			|| 	
		vin_cable_dist->FmtDef == TVI_5M_20P				|| vin_cable_dist->FmtDef == AHD30_5_3M_20P			||

		vin_cable_dist->FmtDef == AHD30_8M_12_5P			  || vin_cable_dist->FmtDef == AHD30_8M_15P  		||
		vin_cable_dist->FmtDef == AHD30_8M_7_5P				|| vin_cable_dist->FmtDef == TVI_8M_15P				||
		vin_cable_dist->FmtDef == TVI_8M_12_5P		   


		/*
		vin_cable_dist->FmtDef == TVI_HD_60P			   	|| vin_cable_dist->FmtDef == TVI_HD_50P			    ||
		vin_cable_dist->FmtDef == CVI_HD_60P              	|| vin_cable_dist->FmtDef == CVI_HD_50P   			||
		*/
		)
	{
		/* get hsync*/
		vin_hsync_accum.ch = vin_cable_dist->ch;
		vin_hsync_accum.devnum = vin_cable_dist->devnum;
		video_input_hsync_accum_read(&vin_hsync_accum );

		/* get acc */
		vin_acc.ch = vin_cable_dist->ch;
		vin_acc.devnum = vin_cable_dist->devnum;
		vin_acc.func_sel = 0;
		/* 1 is ACC_GAIN_DEBUG
		   0 is ACC_GAIN_NORMAL */
		video_input_acc_gain_val_read(&vin_acc);

		/* measure eq */
		video_input_cable_measure_way(vin_cable_dist->ch, vin_cable_dist->devnum);

		/* decision distance using hsync and distance table */
		vin_cable_dist->dist = __video_cable_manualdistance( vin_cable_dist->cabletype, &vin_hsync_accum, &vin_acc, &distance_value );


		printk("[DRV] CH:%d, hsync : %08x\n", vin_cable_dist->ch, vin_hsync_accum.hsync_accum_result);
		printk("[DRV] CH:%d, eq stage:%d\n", vin_cable_dist->ch, vin_cable_dist->dist);
	}
	else if( vin_cable_dist->FmtDef == AHD20_SD_H960_2EX_Btype_NT || vin_cable_dist->FmtDef == AHD20_SD_H960_2EX_Btype_PAL )
	{
		/* CVBS Resolution not need distance distinguish, because cvbs format has low color frequency */
		vin_cable_dist->dist = 0;
	}
	else
	{
		vin_cable_dist->dist = 0;

		printk("[DRV] CH:%d, This Format Not support Yet [%d] eq stage:%d\n", vin_cable_dist->ch, vin_cable_dist->FmtDef ,vin_cable_dist->dist);

	}
}

void video_input_eq_val_set(video_equalizer_info_s *pvin_eq_set)
{
	unsigned char val_13x30;
	unsigned char val_13x31;
	unsigned char val_0x54;
	unsigned char val_5678x69;
	unsigned char val_9x44;

	unsigned char ch = pvin_eq_set->Ch;
	unsigned char devnum = pvin_eq_set->devnum;

	if( pvin_eq_set->FmtDef == CVI_4M_25P  			   || pvin_eq_set->FmtDef == CVI_4M_30P  			||
	    pvin_eq_set->FmtDef == CVI_FHD_25P 			   || pvin_eq_set->FmtDef == CVI_FHD_30P 			||
	    pvin_eq_set->FmtDef == CVI_8M_12_5P			   || pvin_eq_set->FmtDef == CVI_8M_15P  			||
	    pvin_eq_set->FmtDef == TVI_4M_25P  			   || pvin_eq_set->FmtDef == TVI_4M_30P 			||
		pvin_eq_set->FmtDef == AHD30_3M_18P			   || pvin_eq_set->FmtDef == AHD30_3M_25P 			||
		pvin_eq_set->FmtDef == AHD30_3M_30P			   || pvin_eq_set->FmtDef == AHD30_4M_15P 		    ||
	    pvin_eq_set->FmtDef == AHD30_4M_25P			   || pvin_eq_set->FmtDef == AHD30_4M_30P 			||
		pvin_eq_set->FmtDef == AHD30_5M_12_5P		   || pvin_eq_set->FmtDef == AHD30_5M_20P			||
		pvin_eq_set->FmtDef == TVI_5M_12_5P			   || pvin_eq_set->FmtDef == TVI_3M_18P 			||
		pvin_eq_set->FmtDef == AHD20_1080P_30P         || pvin_eq_set->FmtDef == AHD20_1080P_25P 		||
		pvin_eq_set->FmtDef == TVI_FHD_30P             || pvin_eq_set->FmtDef == TVI_FHD_25P			||
		pvin_eq_set->FmtDef == AHD20_720P_30P_EX_Btype || pvin_eq_set->FmtDef == AHD20_720P_25P_EX_Btype||
	    pvin_eq_set->FmtDef == CVI_HD_30P_EX           || pvin_eq_set->FmtDef == CVI_HD_25P_EX			||
		pvin_eq_set->FmtDef == CVI_HD_30P              || pvin_eq_set->FmtDef == CVI_HD_25P  			||
		pvin_eq_set->FmtDef == CVI_HD_60P              || pvin_eq_set->FmtDef == CVI_HD_50P   			||
	    pvin_eq_set->FmtDef == TVI_HD_30P			   || pvin_eq_set->FmtDef == TVI_HD_25P			    ||
	    pvin_eq_set->FmtDef == TVI_HD_30P_EX		   || pvin_eq_set->FmtDef == TVI_HD_25P_EX			||
	    pvin_eq_set->FmtDef == TVI_HD_B_30P			   || pvin_eq_set->FmtDef == TVI_HD_B_25P 			||
	    pvin_eq_set->FmtDef == TVI_HD_B_30P_EX		   || pvin_eq_set->FmtDef == TVI_HD_B_25P_EX		||
		pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_NT || pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_PAL ||
		pvin_eq_set->FmtDef == AHD30_8M_12_5P		   || pvin_eq_set->FmtDef == AHD30_8M_15P  			||
	    pvin_eq_set->FmtDef == AHD30_8M_7_5P		   ||
		pvin_eq_set->FmtDef == TVI_5M_20P			   || pvin_eq_set->FmtDef == AHD30_5_3M_20P			||

		pvin_eq_set->FmtDef == TVI_8M_12_5P   		   || pvin_eq_set->FmtDef == TVI_8M_15P   		  	||

		pvin_eq_set->FmtDef == TVI_4M_15P   		   
        
		/* and all format */
		)
	{
		/* cable type => 0:coaxial, 1:utp, 2:reserved1, 3:reserved2 */
		video_equalizer_value_table_s eq_value = (video_equalizer_value_table_s)equalizer_value_fmtdef[pvin_eq_set->FmtDef];

                /* for verification by edward */

		if(pvin_eq_set->FmtDef == AHD20_720P_30P_EX_Btype || pvin_eq_set->FmtDef == AHD20_720P_25P_EX_Btype ||
		   pvin_eq_set->FmtDef == CVI_HD_30P_EX           || pvin_eq_set->FmtDef == CVI_HD_25P_EX			||
		   pvin_eq_set->FmtDef == CVI_HD_30P              || pvin_eq_set->FmtDef == CVI_HD_25P  			||
		   pvin_eq_set->FmtDef == TVI_HD_30P			  || pvin_eq_set->FmtDef == TVI_HD_25P			    ||
		   pvin_eq_set->FmtDef == TVI_HD_30P_EX			  || pvin_eq_set->FmtDef == TVI_HD_25P_EX			||
		   pvin_eq_set->FmtDef == TVI_HD_B_30P			  || pvin_eq_set->FmtDef == TVI_HD_B_25P 			||
		   pvin_eq_set->FmtDef == TVI_HD_B_30P_EX		  || pvin_eq_set->FmtDef == TVI_HD_B_25P_EX   	    

		  )
		{
			printk("[DRV] This Format Support Maximum EQ Stage 10\n");
			printk("[DRV] Now Select EQ Stage %d\n", pvin_eq_set->distance);
		}
		else
		{
			if(pvin_eq_set->distance > 5)
			{
				printk("[DRV] This Format Only Support Maximum EQ Stage 5\n");
				printk("[DRV] Now Select EQ Stage %d\n", pvin_eq_set->distance);
				pvin_eq_set->distance = 5;
			}
		}

		/* set eq value */
		__eq_base_set_value( pvin_eq_set, &eq_value.eq_base );
		__eq_coeff_set_value( pvin_eq_set, &eq_value.eq_coeff );
		__eq_color_set_value( pvin_eq_set, &eq_value.eq_color);
		__eq_timing_a_set_value( pvin_eq_set, &eq_value.eq_timing_a );
		__eq_clk_set_value( pvin_eq_set, &eq_value.eq_clk );
		__eq_timing_b_set_value( pvin_eq_set, &eq_value.eq_timing_b );

						if( pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_NT || pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_PAL )
				{
					gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00);
					gpio_i2c_write(raptor3_i2c_addr[devnum], 0x34 + ch, 0x01);		/* line_mem_mode Enable */

					if( pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_NT )
					{
						val_0x54 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x54);
						val_0x54 &= ~((0x1 << (ch+4)));
						val_0x54 |= ((0x1 << (ch+4)));
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x54, val_0x54);  /* Enable FLD_INV for CVBS NT format */

						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ch, 0xa0);
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5c + ch, 0xd0); /* Set V_Delay */
					}
					else if( pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_PAL )
					{
						val_0x54 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x54);
						val_0x54 &= ~((0x1 << (ch+4)));
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x54, val_0x54);  /* Disable FLD_INV for CVBS PAL format */

						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ch, 0xdd);
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5c + ch, 0xbf);  /* Set V_Delay */
					}

					gpio_i2c_write( raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
					gpio_i2c_write( raptor3_i2c_addr[devnum], 0x64, 0x00 );         /* disable mem_path */
//					gpio_i2c_write( raptor3_i2c_addr[devnum], 0xa9, 0x00 );
//					gpio_i2c_write( raptor3_i2c_addr[devnum], 0x47, 0x00 );		

					val_5678x69 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x69);
					val_5678x69 &= ~(0x1);

					gpio_i2c_write( raptor3_i2c_addr[devnum], 0x69, val_5678x69 );	/* Enable sd_freq_sel */
				}
				else
				{
					gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00);
					gpio_i2c_write(raptor3_i2c_addr[devnum], 0x34 + ch, 0x00);		/* line_mem_mode Disable */

					if(  pvin_eq_set->FmtDef == AHD20_SD_H960_NT )
					{
						val_0x54 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x54);
						val_0x54 &= ~((0x1 << (ch+4)));
						val_0x54 |= ((0x1 << (ch+4)));
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x54, val_0x54);  /* Enable FLD_INV for CVBS NT format */

						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ch, 0xa0);
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5c + ch, 0xd0); /* Set V_Delay */

						gpio_i2c_write( raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
						val_5678x69 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x69);
						val_5678x69 &= ~(0x1);
						gpio_i2c_write( raptor3_i2c_addr[devnum], 0x69, val_5678x69 );	/* Diable sd_freq_sel */
					}
					else
					{
						val_0x54 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x54);
						val_0x54 &= ~((0x1 << (ch+4)));
						gpio_i2c_write(raptor3_i2c_addr[devnum], 0x54, val_0x54);	/* Disable FLD_INV */

						if(  pvin_eq_set->FmtDef == AHD20_SD_H960_PAL )
						{
							gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ch, 0xdd);
							gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5c + ch, 0xbf);  /* Set V_Delay */

							gpio_i2c_write( raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
							val_5678x69 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x69);
							val_5678x69 &= ~(0x1);
							gpio_i2c_write( raptor3_i2c_addr[devnum], 0x69, val_5678x69 );	/* Disable sd_freq_sel */
							gpio_i2c_write( raptor3_i2c_addr[devnum], 0x1b, 0x20 );	/* Enable sd_freq_sel */

						}
						else
						{
							gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00);
							gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ch, 0x00);
							gpio_i2c_write(raptor3_i2c_addr[devnum], 0x5c + ch, 0x80);	/* Recovery V_Delay */
						}
					}

					gpio_i2c_write( raptor3_i2c_addr[devnum], 0xff, 0x05 + ch );
					gpio_i2c_write( raptor3_i2c_addr[devnum], 0x64, 0x00 );        /* Disable Mem_Path */
				}

		if( pvin_eq_set->FmtDef == CVI_4M_25P  			   || pvin_eq_set->FmtDef == CVI_4M_30P  			||
		    pvin_eq_set->FmtDef == CVI_FHD_25P 			   || pvin_eq_set->FmtDef == CVI_FHD_30P 			||
			pvin_eq_set->FmtDef == CVI_8M_12_5P			   || pvin_eq_set->FmtDef == CVI_8M_15P  			||
		    pvin_eq_set->FmtDef == TVI_4M_25P  			   || pvin_eq_set->FmtDef == TVI_4M_30P  			||
			pvin_eq_set->FmtDef == AHD30_3M_18P			   || pvin_eq_set->FmtDef == AHD30_3M_25P 			||
			pvin_eq_set->FmtDef == AHD30_3M_30P			   || pvin_eq_set->FmtDef == AHD30_4M_15P 		    ||
			pvin_eq_set->FmtDef == AHD30_4M_25P			   || pvin_eq_set->FmtDef == AHD30_4M_30P 			||
			pvin_eq_set->FmtDef == AHD30_5M_12_5P		   || pvin_eq_set->FmtDef == AHD30_5M_20P			||
			pvin_eq_set->FmtDef == TVI_5M_12_5P			   || pvin_eq_set->FmtDef == TVI_3M_18P 			||
			pvin_eq_set->FmtDef == AHD20_1080P_30P         || pvin_eq_set->FmtDef == AHD20_1080P_25P 		||
			pvin_eq_set->FmtDef == TVI_FHD_30P             || pvin_eq_set->FmtDef == TVI_FHD_25P			||
			pvin_eq_set->FmtDef == AHD20_720P_30P_EX_Btype || pvin_eq_set->FmtDef == AHD20_720P_25P_EX_Btype||
			pvin_eq_set->FmtDef == CVI_HD_30P_EX           || pvin_eq_set->FmtDef == CVI_HD_25P_EX			||
			pvin_eq_set->FmtDef == CVI_HD_30P              || pvin_eq_set->FmtDef == CVI_HD_25P  			||
			pvin_eq_set->FmtDef == CVI_HD_60P              || pvin_eq_set->FmtDef == CVI_HD_50P   			||
			pvin_eq_set->FmtDef == TVI_HD_30P			   || pvin_eq_set->FmtDef == TVI_HD_25P			    ||
			pvin_eq_set->FmtDef == TVI_HD_30P_EX		   || pvin_eq_set->FmtDef == TVI_HD_25P_EX			||
			pvin_eq_set->FmtDef == TVI_HD_B_30P			   || pvin_eq_set->FmtDef == TVI_HD_B_25P 			||
			pvin_eq_set->FmtDef == TVI_HD_B_30P_EX		   || pvin_eq_set->FmtDef == TVI_HD_B_25P_EX		||
			pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_NT || pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_PAL ||
		    pvin_eq_set->FmtDef == AHD30_8M_12_5P		   || pvin_eq_set->FmtDef == AHD30_8M_15P  			||
			pvin_eq_set->FmtDef == AHD30_8M_7_5P		   || pvin_eq_set->FmtDef == TVI_5M_20P				||
			pvin_eq_set->FmtDef == AHD30_5_3M_20P		   ||
			pvin_eq_set->FmtDef == TVI_8M_15P		   		|| pvin_eq_set->FmtDef == TVI_8M_12_5P		   	|| 

			pvin_eq_set->FmtDef == TVI_4M_15P               
   		   )
		{	/* Auto Mode Off */
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x13 );
			val_13x30 = gpio_i2c_read(raptor3_i2c_addr[pvin_eq_set->devnum], 0x30);
			val_13x30 &= ~(0x11 << pvin_eq_set->Ch);
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x30, val_13x30 );

			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x13 );
			val_13x31 = gpio_i2c_read(raptor3_i2c_addr[pvin_eq_set->devnum], 0x31);
			val_13x31 &= ~(0x11 << pvin_eq_set->Ch);
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x31, val_13x31 );

            gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x05 + ch);
            gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x59, 0x00 );
		}
		else
		{   /* Auto Mode ON */
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x13 );
			val_13x30 = gpio_i2c_read(raptor3_i2c_addr[pvin_eq_set->devnum], 0x30);
			val_13x30 |= (0x11 << pvin_eq_set->Ch);
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x30, val_13x30 );

			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x13 );
			val_13x31 = gpio_i2c_read(raptor3_i2c_addr[pvin_eq_set->devnum], 0x31);
			val_13x31 |= (0x11 << pvin_eq_set->Ch);
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x31, val_13x31 );
		}

		gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x00);
		gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x23 + (pvin_eq_set->Ch * 4), 0x41);
	}

	if( pvin_eq_set->FmtDef == TVI_5M_20P || pvin_eq_set->FmtDef == TVI_5M_12_5P ||
		pvin_eq_set->FmtDef == TVI_4M_30P || pvin_eq_set->FmtDef == TVI_4M_25P	 ||
		pvin_eq_set->FmtDef == TVI_8M_15P || pvin_eq_set->FmtDef == TVI_8M_12_5P	)

	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xB8, 0x39);	
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x26, 0x40);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1B, 0x08);	

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x09);
		val_9x44 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x44);
		val_9x44 &= ~(1 << ch);
		val_9x44 |= (1 << ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x44 , val_9x44);

		if( pvin_eq_set->FmtDef == TVI_5M_20P)
		{
			/* TVI 5M 20P PN Value Set */
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50 + ( ch * 4 ) , 0x36);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x51 + ( ch * 4 ) , 0x40);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x52 + ( ch * 4 ) , 0xa7);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x53 + ( ch * 4 ) , 0x74);

			gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x11);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x02 + ( ch * 0x20 ), 0xdb);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x03 + ( ch * 0x20 ), 0x0a);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x05 + ( ch * 0x20 ), 0x0e);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x06 + ( ch * 0x20 ), 0xa6);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ( ch * 0x20 ), 0x96);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0a + ( ch * 0x20 ), 0x07);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0b + ( ch * 0x20 ), 0x98);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ( ch * 0x20 ), 0x07);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0d + ( ch * 0x20 ), 0xbc);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x11 + ( ch * 0x20 ), 0xa0);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x13 + ( ch * 0x20 ), 0xfa);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x15 + ( ch * 0x20 ), 0x65);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + ( ch * 0x20 ), 0x0f);
		}
		else if( pvin_eq_set->FmtDef == TVI_5M_12_5P)
		{
			/* TVI 5M 12_5P PN Value Set */
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50 + ( ch * 4 ) , 0x8b);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x51 + ( ch * 4 ) , 0xae);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x52 + ( ch * 4 ) , 0xbb);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x53 + ( ch * 4 ) , 0x48);

			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0xff, 0x05 + ch);
			gpio_i2c_write(raptor3_i2c_addr[pvin_eq_set->devnum], 0x26, 0x20);
		}
		else if( pvin_eq_set->FmtDef == TVI_4M_30P || pvin_eq_set->FmtDef == TVI_4M_25P )
		{
			/* TVI 4M 30P PN Value Set */
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50 + ( ch * 4 ) , 0x9e);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x51 + ( ch * 4 ) , 0x48);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x52 + ( ch * 4 ) , 0x59);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x53 + ( ch * 4 ) , 0x74);
		}
		else if( pvin_eq_set->FmtDef == TVI_8M_15P || pvin_eq_set->FmtDef == TVI_8M_12_5P )
		{
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50 + ( ch * 4 ) , 0x73);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x51 + ( ch * 4 ) , 0x76);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x52 + ( ch * 4 ) , 0x58);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x53 + ( ch * 4 ) , 0x74);

			gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x11);

			if( pvin_eq_set->FmtDef == TVI_8M_12_5P )
			{
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x02 + ( ch * 0x20 ), 0x9b);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x03 + ( ch * 0x20 ), 0x0f);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x05 + ( ch * 0x20 ), 0x14);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x06 + ( ch * 0x20 ), 0xa0);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ( ch * 0x20 ), 0x80);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0a + ( ch * 0x20 ), 0x08);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0b + ( ch * 0x20 ), 0x70);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ( ch * 0x20 ), 0x08);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0d + ( ch * 0x20 ), 0xca);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x12 + ( ch * 0x20 ), 0x01);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x13 + ( ch * 0x20 ), 0xcc);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x15 + ( ch * 0x20 ), 0x3c);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + ( ch * 0x20 ), 0x0d);
			}
			else
			{
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x02 + ( ch * 0x20 ), 0x9b);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x03 + ( ch * 0x20 ), 0x0f);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x05 + ( ch * 0x20 ), 0x11);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x06 + ( ch * 0x20 ), 0x30);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ( ch * 0x20 ), 0x80);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0a + ( ch * 0x20 ), 0x08);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0b + ( ch * 0x20 ), 0x70);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ( ch * 0x20 ), 0x08);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0d + ( ch * 0x20 ), 0xca);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x12 + ( ch * 0x20 ), 0x01);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x13 + ( ch * 0x20 ), 0xcc);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x15 + ( ch * 0x20 ), 0x3c);
				gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + ( ch * 0x20 ), 0x0d);
			}

		}
	}
	else if( pvin_eq_set->FmtDef == TVI_4M_15P )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x11);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + ( ch * 0x20 ), 0x0f);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x01 + ( ch * 0x20 ), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x02 + ( ch * 0x20 ), 0xd0);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x03 + ( ch * 0x20 ), 0x0a);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x04 + ( ch * 0x20 ), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x05 + ( ch * 0x20 ), 0x97);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x06 + ( ch * 0x20 ), 0x70);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x07 + ( ch * 0x20 ), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x08 + ( ch * 0x20 ), 0x78);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0a + ( ch * 0x20 ), 0x05);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0b + ( ch * 0x20 ), 0xa0);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0c + ( ch * 0x20 ), 0x06);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x0d + ( ch * 0x20 ), 0x71);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x10 + ( ch * 0x20 ), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x11 + ( ch * 0x20 ), 0x50);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x12 + ( ch * 0x20 ), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x13 + ( ch * 0x20 ), 0x96);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x14 + ( ch * 0x20 ), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x15 + ( ch * 0x20 ), 0x30);

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6e, 0x10);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6f, 0x7e);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x47, 0xee);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50, 0xc6);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xb7, 0xfc);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xbb, 0x0f);
	}
	else if( pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_NT || pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_PAL )
	{
		unsigned char val_11x00;
		unsigned char val_11x01;

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xB8, 0xB8);	// SD Rees Only
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1B, 0x20);	

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x09);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x40 + ch, 0x00);

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x11);
		val_11x00 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x00 + (ch*0x20));
		val_11x00 |= 0x1;
		val_11x01 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x01 + (ch*0x20));
		val_11x01 |= 0x1;

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + (ch*0x20), val_11x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x01 + (ch*0x20), val_11x01);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x03 + (ch*0x20), 0x0f);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x04 + (ch*0x20), 0x00);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x05 + (ch*0x20), 0x12);

		if( pvin_eq_set->FmtDef == AHD20_SD_H960_2EX_Btype_NT )
		{
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x02 + (ch*0x20), 0x00);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x06 + (ch*0x20), 0x6f);
		}
		else
		{
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x02 + (ch*0x20), 0x28);
			gpio_i2c_write(raptor3_i2c_addr[devnum], 0x06 + (ch*0x20), 0x90);
		}
	}
	else if( pvin_eq_set->FmtDef == AHD20_SD_H960_NT || pvin_eq_set->FmtDef == AHD20_SD_H960_PAL )
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xB8, 0xB9);	// SD Rees Only
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1B, 0x20);	

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x09);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x40 + ch, 0x00);

	}
	else
	{
		unsigned char val_1x7a = 0x00;
		unsigned char val_11x00 = 0x00;

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xB8, 0x39);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x26, 0x40);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x1B, 0x08);	

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6e, 0x00);	
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6f, 0x00);	

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x09);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x40 + ch, 0x00);

		val_9x44 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x44);
		val_9x44 &= ~(1 << ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x44 , val_9x44);

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x01);
		val_1x7a = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x7a);
		val_1x7a |= (0x1 << ch);
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x7A, val_1x7a);

		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x11);
		val_11x00 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x00 + (ch*0x20));
		val_11x00 &= ~0x10;
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + (ch * 0x20) , val_11x00);
	}


}
