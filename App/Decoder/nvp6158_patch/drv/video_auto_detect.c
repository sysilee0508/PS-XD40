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
#include "video_auto_detect.h"

#define ACC_GAIN_NORMAL 0
#define ACC_GAIN_DEBUG  1

extern unsigned int raptor3_i2c_addr[4];

void _video_input_auto_detect_vafe_set(video_input_auto_detect *vin_auto_det)
{
	unsigned char val_1x7A;
	unsigned char val_5678x00;
	unsigned char val_5678x01;
	unsigned char val_5678x58;
	unsigned char val_5678x59;
	unsigned char val_5678x5B;
	unsigned char val_5678x5C;

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x01);
	val_1x7A = gpio_i2c_read(raptor3_i2c_addr[vin_auto_det->devnum], 0x7A);
	val_1x7A |= (1 << vin_auto_det->ch);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x7A, val_1x7A);

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x00);
	//B0 0x00/1/2/3 gain[4], powerdown[0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x00 + vin_auto_det->ch, ((vin_auto_det->vafe.gain & 0x01) << 4) |
												  (vin_auto_det->vafe.powerdown & 0x01));

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x01);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x84 + vin_auto_det->ch, 0x00);

	//B5/6/7/8
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x05 + vin_auto_det->ch);

	//B5/6/7/8 0x01 spd[2], lpf_back_band[1:0]
	val_5678x00 = gpio_i2c_read(raptor3_i2c_addr[vin_auto_det->devnum], 0x00);
	val_5678x00 &= ~(0xF << 4);
	val_5678x00 |= vin_auto_det->vafe.spd << 4;

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x00, val_5678x00);

	val_5678x01 = ((vin_auto_det->vafe.ctrlreg << 6) | (vin_auto_det->vafe.ctrlibs << 4) | (vin_auto_det->vafe.adcspd << 2) | (vin_auto_det->vafe.clplevel));
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x01, val_5678x01 );

	//B5/6/7/8 0x58 eq_band[7:4], lpf_front_band[1:0]
	val_5678x58 = ((vin_auto_det->vafe.eq_band << 4) | (vin_auto_det->vafe.lpf_front_band));
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x58, val_5678x58);

	//B5/6/7/8 0x5B ref_vol[1:0]
	val_5678x59 = ((vin_auto_det->vafe.clpmode << 7) | (vin_auto_det->vafe.f_lpf_bypass << 4) | (vin_auto_det->vafe.clproff << 3) | (vin_auto_det->vafe.b_lpf_bypass));
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x59, val_5678x59);

	val_5678x5B = ((vin_auto_det->vafe.duty << 4) | (vin_auto_det->vafe.ref_vol));
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x5B, val_5678x5B);

	val_5678x5C = ((vin_auto_det->vafe.lpf_back_band << 4) | (vin_auto_det->vafe.clk_sel << 3) | (vin_auto_det->vafe.eq_gainsel));
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x5C, val_5678x5C);
}

void video_input_auto_detect_set(video_input_auto_detect *vin_auto_det)
{
	unsigned char val_0x30;
	unsigned char val_0x31;
	unsigned char val_0x32;

	vin_auto_det->vafe.powerdown 		= 0x00;
	vin_auto_det->vafe.gain		 		= 0x01;
	vin_auto_det->vafe.spd		 		= 0x0d;
	vin_auto_det->vafe.ctrlreg 	 		= 0x01;
	vin_auto_det->vafe.ctrlibs	 		= 0x02;
	vin_auto_det->vafe.adcspd	 		= 0x00;
	vin_auto_det->vafe.clplevel  		= 0x02;
	vin_auto_det->vafe.eq_band	 		= 0x00;
	vin_auto_det->vafe.lpf_front_band 	= 0x07;
	vin_auto_det->vafe.clpmode   		= 0x00;
	vin_auto_det->vafe.f_lpf_bypass 	= 0x01;
	vin_auto_det->vafe.clproff 			= 0x00;
	vin_auto_det->vafe.b_lpf_bypass 	= 0x00;
	vin_auto_det->vafe.duty				= 0x04;
	vin_auto_det->vafe.ref_vol			= 0x03;
	vin_auto_det->vafe.lpf_back_band	= 0x07;
	vin_auto_det->vafe.clk_sel			= 0x01;
	vin_auto_det->vafe.eq_gainsel		= 0x07;

	vin_auto_det->d_cmp					= 0x3f;
	vin_auto_det->slice_level			= 0x5a;
	vin_auto_det->stable_mode_1			= 0x04;
	//vin_auto_det->stable_mode_2			= 0x04;
	vin_auto_det->stable_mode_2			= 0x00;
	vin_auto_det->novid_det				= 0x43;

	_video_input_auto_detect_vafe_set(vin_auto_det);

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x05 + vin_auto_det->ch);

	//B5/6/7/8 0x03 Digital Clamp
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x03, vin_auto_det->d_cmp);
	//B5/6/7/8 0x08 Slice Level
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x08, vin_auto_det->slice_level);
	//B5/6/7/8 0xB9 HAFC_LPF_SEL[7:6] GAIN1[5:4] GAIN2[3:0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xB9, 0x72);

	//B5/6/7/8 0xCA ADV_V_DELAY_AD[4] ADV_V_DELAY_ON[0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xCA, 0x10);

	//B13 0x30 AUTO_FMT_SET_EN_2[3:0], AUTO_FMT_SET_EN    [3:0]
	//B13 0x31 AUTO_FMT_SET_EN_4[3:0], AUTO_FMT_SET_EN_3  [3:0]
	//B13 0x32 [	  RESERVED  	], NOVIDEO_VFC_INIT_EN[3:0]

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x13);
	val_0x30 = gpio_i2c_read(raptor3_i2c_addr[vin_auto_det->devnum], 0x30);
	val_0x31 = gpio_i2c_read(raptor3_i2c_addr[vin_auto_det->devnum], 0x31);
	val_0x32 = gpio_i2c_read(raptor3_i2c_addr[vin_auto_det->devnum], 0x32);
	val_0x30 |= ((1 << (vin_auto_det->ch + 4)) | (1 << vin_auto_det->ch));
	val_0x31 |= ((1 << (vin_auto_det->ch + 4)) | (1 << vin_auto_det->ch));
	val_0x32 |= ((1 << vin_auto_det->ch) & 0xF);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x30, val_0x30);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x31, val_0x31);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x32, val_0x32);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x36, 0x0A);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x37, 0x82);

	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x81+vin_auto_det->ch, 0x0A);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x85+vin_auto_det->ch, 0x02);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//B13
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x13);
	//B13 0x00  Stable Mode set
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x00, vin_auto_det->stable_mode_1);
	//B13 0x01  Stable Mode Set
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x01, ((vin_auto_det->stable_mode_2) & 0x3));
	//B13 0x40 VFC_EQ_BAND_SEL[7:4] VFC_LPF_F_SEL[1:0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x40, 0x07);
	//B13 0x41 VFC_REF_VTG
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x41, 0x01);
	//B13 0x42 VFC_D_CMP_SET
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x42, 0x3F);
	//B13 0x43 VFC_SLICE_VALUE
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x43, 0x5A);
	//B13 0x44 VFC_SLICE_MD2
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x44, 0x30);
	//B13 0x45 VFC_CONTROL_MODES
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x45, 0xEE);
	//B13 0x46 VFC_GDF_FIX_COEFF
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x46, 0xC6);
	//B13 0x47 VFC_DFE_REF_SEL_OLD[4] VFC_DFE_REF_SEL_NEW[0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x47, 0x00);
	//B13 0x48 VFC_D_BLK_CNT_NEW[[7:4] VFC_HAFC_BYPASS_NEW[1] VFC_UPDN_SEL[0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x48, 0x80);
	//B13 0x49 VFC_OLD_WPD_ON[4] VFC_NEW_WPD_ON[0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x49, 0x00);
	//B13 0x4A VFC_D_CMP_FZ_OLD[4] VFC_D_CMP_FZ_NEW[1]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x4A, 0x11);
	//B13 0x4B VFC_AUTO_GNOS_MODE
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x4B, 0x7F);
	//B13 0x4C VFC_AUTO_SYNC_MODE
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x4C, 0x00);
	//B13 0x4D VFC_HAFC_BYPASS[7] ??? [6:0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x4D, 0xB9);
	//B13 0x4E VFC_VAFE_B_LPF_SEL[6:4] VFC_VAFE_CKSEL[3] VFC_VAFE_EQ_GAIN_SEL[2:0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x4E, 0x78);
	//B13 0x4F VFC_VAFE_CTRL_RES[7:6] VFC_VAFE_IBS_CTRL[5:4] VFC_VAFE_SPD[2] VFC_VAFE_CLP_LEVEL[1:0]
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x4F, 0x62);

	//B0  0x23/0x27/0x2B/0x2F No Video Detect
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x0);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x23 + ((vin_auto_det->ch) * 4), vin_auto_det->novid_det);

	/* clock set */
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x1);
	gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x84 + vin_auto_det->ch, 0x00);
    gpio_i2c_write(raptor3_i2c_addr[vin_auto_det->devnum], 0x8c + vin_auto_det->ch, 0x55);
}

void video_input_vfc_read(video_input_vfc *vin_vfc)
{
	gpio_i2c_write(raptor3_i2c_addr[vin_vfc->devnum], 0xFF, 0x13);
	vin_vfc->vfc = gpio_i2c_read(raptor3_i2c_addr[vin_vfc->devnum], 0xF0 + vin_vfc->ch);
}

void video_input_novid_read(video_input_novid *vin_novid)
{
	unsigned char val_0xA8;

	gpio_i2c_write(raptor3_i2c_addr[vin_novid->devnum], 0xFF, 0x00);
	val_0xA8 = gpio_i2c_read(raptor3_i2c_addr[vin_novid->devnum], 0xA8);

	vin_novid->novid = (((val_0xA8 >> vin_novid->ch) & 0x1)) ;
}


void video_input_no_video_set(video_input_novid_set *auto_novid)
{
	unsigned char val_13x30;
	unsigned char val_13x31;
	unsigned char val_13x32;

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xFF, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x21 + (auto_novid->ch * 4), 0x82);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x23 + (auto_novid->ch * 4), 0x43);

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xFF, 0x09);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x80 + (auto_novid->ch * 0x20), 0x00);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x81 + (auto_novid->ch * 0x20), 0x00);

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xFF, 0x05 + auto_novid->ch);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x2C, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x01, 0x62);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x6e, 0x00 );
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x6f, 0x00 );

	/* DECI_FILTER_OFF */
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x50, 0xc6);

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x58, 0x47);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x5C, 0x7f);

    /* Low-Poass Filter (LPF) Bypass Enable  Bank5/6/7/8 0x59 */
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x59, 0x10);

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xB8, 0xB8);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xB9, 0xB2);

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xFF, 0x13);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x43, 0x5a);

	val_13x30 = gpio_i2c_read(raptor3_i2c_addr[auto_novid->devnum], 0x30);
	val_13x31 = gpio_i2c_read(raptor3_i2c_addr[auto_novid->devnum], 0x31);
	val_13x32 = gpio_i2c_read(raptor3_i2c_addr[auto_novid->devnum], 0x32);
	val_13x30 |= ((1 << (auto_novid->ch + 4)) | (1 << auto_novid->ch));
	val_13x31 |= ((1 << (auto_novid->ch + 4)) | (1 << auto_novid->ch));
	val_13x32 |= ((1 << auto_novid->ch) & 0xF);

	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x30, val_13x30);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x31, val_13x31);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x32, val_13x32);

	/* disable Bank11 0x00, if before setting format TVI 5M 20P when onvideo */
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0xFF, 0x11);
	gpio_i2c_write(raptor3_i2c_addr[auto_novid->devnum], 0x00 + ( auto_novid->ch * 0x20 ), 0x00);
}

void video_input_cable_dist_read(video_input_cable_dist *vin_cable_dist)
{
	gpio_i2c_write(raptor3_i2c_addr[vin_cable_dist->devnum], 0xFF, 0x13);

	//B13 0xA0 [3:0] Cable Distance Value
	vin_cable_dist->dist = gpio_i2c_read(raptor3_i2c_addr[vin_cable_dist->devnum], 0xA0 + vin_cable_dist->ch ) & 0xF;
}



void video_input_sam_val_read(video_input_sam_val *vin_sam_val ) 
{
	unsigned char val1, val2;

	// Channel Change Sequence
	gpio_i2c_write(raptor3_i2c_addr[vin_sam_val->devnum], 0xFF, 0x13);
	gpio_i2c_write(raptor3_i2c_addr[vin_sam_val->devnum], 0x2B, vin_sam_val->ch);

	gpio_i2c_write(raptor3_i2c_addr[vin_sam_val->devnum], 0xFF, 0x13); 
	//B13 0xC9 [7:0] SAM Value
	val1 = gpio_i2c_read(raptor3_i2c_addr[vin_sam_val->devnum], 0xC9) ;
	//B13 0xC8 [9:8] SAM Value
	val2 = gpio_i2c_read(raptor3_i2c_addr[vin_sam_val->devnum], 0xC8) & 0x3;

	vin_sam_val->sam_val = ((val2 << 8) | val1);
}

void video_input_hsync_accum_read(video_input_hsync_accum *vin_hsync_accum )
{
	unsigned char val01, val02, val03, val04;
	unsigned char val11, val12, val13, val14;

	unsigned char h_lock;
	unsigned int val_1;
	unsigned int val_2;
	unsigned int val_result;

	gpio_i2c_write(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xFF, 0x00);
	h_lock = (gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xE2) >> vin_hsync_accum->ch) & 0x1;

	vin_hsync_accum->h_lock = h_lock;

	gpio_i2c_write(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xFF, 0x13);
	gpio_i2c_write(raptor3_i2c_addr[vin_hsync_accum->devnum], 0x2B, vin_hsync_accum->ch);

	gpio_i2c_write(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xFF, 0x13); 	/* + vin_sam_val->ch  */

	//B13 0xB4 [ 7:0] Hsync Accumulation Value
	val01 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD0);	// 170214 0xB4 -> 0xD0 Fix
	//B13 0xB5 [15:8] Hsync Accumulation Value
	val02 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD1);	// 170214 0xB5 -> 0xD1 Fix
	//B13 0xB6 [23:16] Hsync Accumulation Value
	val03 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD2);	// 170214 0xB6 -> 0xD2 Fix
	//B13 0xB7 [31:24] Hsync Accumulation Value
	val04 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD3);	// 170214 0xB7 -> 0xD3 Fix

	//B13 0xB4 [ 7:0] Hsync Accumulation Value
	val11 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD4);	// 170214 0xB8 -> 0xD4 Fix
	//B13 0xB5 [15:8] Hsync Accumulation Value
	val12 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD5);	// 170214 0xB9 -> 0xD5 Fix
	//B13 0xB6 [23:16] Hsync Accumulation Value
	val13 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD6);	// 170214 0xBA -> 0xD6 Fix
	//B13 0xB7 [31:24] Hsync Accumulation Value
	val14 = gpio_i2c_read(raptor3_i2c_addr[vin_hsync_accum->devnum], 0xD7);	// 170214 0xBB -> 0xD7 Fix

	val_1 = ((val04 << 24) | (val03 << 16) | (val02 << 8) | val01);
	val_2 = ((val14 << 24) | (val13 << 16) | (val12 << 8) | val11);

	val_result = val_1 - val_2;

	vin_hsync_accum->hsync_accum_val1 = val_1;
	vin_hsync_accum->hsync_accum_val2 = val_2;
	vin_hsync_accum->hsync_accum_result = val_result;
}

void video_input_agc_val_read(video_input_agc_val *vin_agc_val) 
{
	unsigned char agc_lock;

	gpio_i2c_write(raptor3_i2c_addr[vin_agc_val->devnum], 0xFF, 0x00);
	agc_lock = (gpio_i2c_read(raptor3_i2c_addr[vin_agc_val->devnum], 0xE0) >> vin_agc_val->ch) & 0x1;

	 vin_agc_val->agc_lock = agc_lock;

	gpio_i2c_write(raptor3_i2c_addr[vin_agc_val->devnum], 0xFF, 0x13);
	gpio_i2c_write(raptor3_i2c_addr[vin_agc_val->devnum], 0x2B, vin_agc_val->ch);

	gpio_i2c_write(raptor3_i2c_addr[vin_agc_val->devnum], 0xFF, 0x13); /* + vin_sam_val->ch ); */

	//B13 0xB8 [ 7:0] Hsync Accumulation Value
	vin_agc_val->agc_val = gpio_i2c_read(raptor3_i2c_addr[vin_agc_val->devnum], 0xC4); 
}

void video_input_acc_gain_val_read(video_input_acc_gain_val *vin_acc_gain)
{
	unsigned char val1, val2;

	if(vin_acc_gain->func_sel == ACC_GAIN_NORMAL) {

		gpio_i2c_write(raptor3_i2c_addr[vin_acc_gain->devnum], 0xFF, 0x05 + vin_acc_gain->ch);

		val1 = gpio_i2c_read(raptor3_i2c_addr[vin_acc_gain->devnum],0xE2) & 0x7; // B5 0xE2 acc gain [10:8]
		val2 = gpio_i2c_read(raptor3_i2c_addr[vin_acc_gain->devnum],0xE3); 		 // B5 0xE3 acc gain [7:0]
	}
	else if(vin_acc_gain->func_sel == ACC_GAIN_DEBUG) { 	// DEBUG
		gpio_i2c_write(raptor3_i2c_addr[vin_acc_gain->devnum], 0xFF, 0x00);
		val1 = 0;
		val2 = gpio_i2c_read(raptor3_i2c_addr[vin_acc_gain->devnum],0xD8 + vin_acc_gain->ch); // B13 0xC6 acc gain [9:8]
	}
	else
	{
		gpio_i2c_write(raptor3_i2c_addr[vin_acc_gain->devnum], 0xFF, 0x05);

		val1 = gpio_i2c_read(raptor3_i2c_addr[vin_acc_gain->devnum],0xE2) & 0x7; // B5 0xE2 acc gain [10:8]
		val2 = gpio_i2c_read(raptor3_i2c_addr[vin_acc_gain->devnum],0xE3); 		 // B5 0xE3 acc gain [7:0]
	}

	vin_acc_gain->acc_gain_val = val1 << 8 | val2;
}

void video_output_data_out_mode_set(video_output_data_out_mode *vo_data_out_mode)
{
	unsigned char temp_val = 0x0;

	//  Show/Hide mode is using register Bank 0 0x7A, 7B
	// 		   CH2	  CH1		    CH4    CH3
	//	0x7A [7 : 4][3 : 0]  0x7B [7 : 4][3 : 0]
	gpio_i2c_write(raptor3_i2c_addr[vo_data_out_mode->devnum], 0xFF, 0x00);

	switch(vo_data_out_mode -> ch)
	{
		case CH1 :
		case CH2 : temp_val = gpio_i2c_read(raptor3_i2c_addr[vo_data_out_mode->devnum], 0x7A);
					break;
		case CH3 :
		case CH4 : temp_val = gpio_i2c_read(raptor3_i2c_addr[vo_data_out_mode->devnum], 0x7B);
					break;
	}

	switch(vo_data_out_mode -> ch)
	{
		case CH1 :
		case CH3 :	temp_val = ((temp_val & 0xF0) | (vo_data_out_mode -> set_val & 0xF));
					break;
		case CH2 :
		case CH4 :  temp_val = ((temp_val & 0x0F) | ((vo_data_out_mode -> set_val & 0xF) << 4));
					break;
	}

	switch(vo_data_out_mode -> ch)
	{
		case CH1 :
		case CH2 : gpio_i2c_write(raptor3_i2c_addr[vo_data_out_mode->devnum], 0x7A, temp_val);
				   break;
		case CH3 :
		case CH4 : gpio_i2c_write(raptor3_i2c_addr[vo_data_out_mode->devnum], 0x7B, temp_val);
				   break;
	}
}


unsigned char __IsOver3MRTVideoFormat( decoder_dev_ch_info_s *decoder_info )
{
	unsigned char ret = 0; //

	if(
	   (decoder_info->fmt_def == AHD30_3M_30P) ||
	   (decoder_info->fmt_def == AHD30_3M_25P) ||
	   (decoder_info->fmt_def == AHD30_4M_30P) ||
	   (decoder_info->fmt_def == AHD30_4M_25P) ||
	   (decoder_info->fmt_def == AHD30_5M_20P) ||
	   (decoder_info->fmt_def == AHD30_5_3M_20P) ||
	   (decoder_info->fmt_def == AHD30_6M_18P) ||
	   (decoder_info->fmt_def == AHD30_6M_20P) ||
	   (decoder_info->fmt_def == AHD30_8M_12_5P) ||
	   (decoder_info->fmt_def == AHD30_8M_15P) ||
	   (decoder_info->fmt_def == TVI_4M_30P) ||
	   (decoder_info->fmt_def == TVI_4M_25P) ||
	   (decoder_info->fmt_def == TVI_5M_20P) ||
	   (decoder_info->fmt_def == CVI_4M_25P) ||
	   (decoder_info->fmt_def == CVI_4M_30P) ||
	   (decoder_info->fmt_def == CVI_8M_15P) ||
	   (decoder_info->fmt_def == CVI_8M_12_5P) ||
	   (decoder_info->fmt_def == TVI_8M_15P) ||
	   (decoder_info->fmt_def == TVI_8M_12_5P)

	   )
	{
		ret = 1;
	}
	return ret; // 0:Over 3M RT, 1:other formation
}

unsigned char s_only_onetime_run[32] = {0, };
void video_input_onvideo_set(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char format_3M_RT;
	unsigned char ch = decoder_info->ch % 4;
	unsigned char devnum = decoder_info->devnum;
	unsigned char val_9x44;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x09);
	val_9x44 = gpio_i2c_read(raptor3_i2c_addr[devnum], 0x44);
	val_9x44 &= ~(1 << ch);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x44, val_9x44);

	/* CVI HD 30P PN Value Set */
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50 + (ch*4) , 0x30);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x51 + (ch*4) , 0x6F);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x52 + (ch*4) , 0x67);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x53 + (ch*4) , 0x48);

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x11);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x00 + (ch*0x20), 0x00);

	/* after 09/12 */
	format_3M_RT = __IsOver3MRTVideoFormat(decoder_info);

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);

	if(format_3M_RT)
	{
		/* DECI_FILTER_ON */
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50, 0x76);
	}
	else
	{
		/* DECI_FILTER_OFF */
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0x50, 0xc6);
	}
}

void video_input_onvideo_check_data(video_input_vfc *vin_vfc)
{
	unsigned char val_5678xF0;
	gpio_i2c_write(raptor3_i2c_addr[vin_vfc->devnum], 0xFF, 0x05 + vin_vfc->ch);
	val_5678xF0 = gpio_i2c_read(raptor3_i2c_addr[vin_vfc->devnum], 0xF0);
	vin_vfc->vfc = val_5678xF0;
}

void video_input_manual_agc_stable_endi(decoder_dev_ch_info_s *decoder_info, int endi)
{
	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xFF, 0x05+decoder_info->ch);
	if( endi == 1 )
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x82, 0xff);
	else
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x82, 0x00);
}

void video_input_vafe_control(decoder_dev_ch_info_s *decoder_info, int cmd)
{
	unsigned char val_0x00;

	gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0xFF, 0x00);

	if(cmd == 0)
	{
		val_0x00 = gpio_i2c_read(raptor3_i2c_addr[decoder_info->devnum], 0x00 + decoder_info->ch);
		_SET_BIT(val_0x00, 0);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x00 + decoder_info->ch, val_0x00);

		printk("[DRV] [Ch:%d] AFE Power Down \n", decoder_info->ch);

		msleep(10);
	}
	else if(cmd == 1)
	{
		val_0x00 = gpio_i2c_read(raptor3_i2c_addr[decoder_info->devnum], 0x00 + decoder_info->ch);
		_CLE_BIT(val_0x00, 0);
		gpio_i2c_write(raptor3_i2c_addr[decoder_info->devnum], 0x00 + decoder_info->ch, val_0x00);

		printk("[DRV] [Ch:%d] AFE Power Up \n", decoder_info->ch);

		msleep(10);
	}
}

unsigned int __s_max_min_exclude_avg_func(unsigned int* input_arry, int cnt)
{
	unsigned int max, min, sum = 0, result = 0;
	unsigned int ii;

	max = input_arry[0];
	min = input_arry[0];

	for(ii = 0; ii < cnt; ii++)
	{
		max = max > input_arry[ii] ? max : input_arry[ii];
		min = min > input_arry[ii] ? input_arry[ii] : min;

		sum += input_arry[ii];
	}

	result = sum - (max + min);

	if(result == 0)
	{
		return 0;
	}
	else
	{
		result /= ( cnt - 2 );
	}

	return result;
}

unsigned int __s_distinguish_5M_ahd_tvi_func(unsigned int* input_arry, int cnt)
{
	unsigned int chk1, chk2;
	unsigned int max, max_idx = 0;
	unsigned int calc_array[10][10] = { {0, 0},  };
	unsigned int need_update = 0;
	unsigned int find_idx = 0;
	unsigned int ii, ij;
	unsigned int inner_idx = 0;

	chk1 = input_arry[0];

	for(ii = 0; ii < cnt; ii++)
	{
		chk2 = input_arry[ii];

		if( chk1 == chk2)
		{
			calc_array[0][inner_idx] += 1;
			calc_array[1][inner_idx] = chk1;
		}
		else if( chk1 != chk2 )
		{
			for(ij = 0; ij < ii; ij++)
			{
				if( calc_array[1][ij] == chk2 )
				{
					find_idx = ij;
					calc_array[0][find_idx] += 1;
					calc_array[1][find_idx] = chk2;
					need_update = 0;
					break;
				}
				need_update = 1;
			}

			if(need_update)
			{
				inner_idx += 1;
				calc_array[0][inner_idx] += 1;
				calc_array[1][inner_idx] = chk2;
			}
		}

		chk1 = chk2;
	}

	max = calc_array[0][0];

	for(ii = 0; ii < cnt; ii++)
	{
		if( max < calc_array[0][ii] )
		{
			max_idx = ii;
			max = calc_array[0][ii];
		}
	}

	return calc_array[1][max_idx];
}

void video_input_ahd_tvi_distinguish(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char ch = decoder_info->ch;
	unsigned char devnum = decoder_info->devnum;
	unsigned char fmtdef = decoder_info->fmt_def;
	unsigned char ii;
	unsigned int check_point;

	unsigned char check_time = 10;

	unsigned int B5xF5_F4[10];

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);

	for(ii = 0; ii < check_time; ii++)
	{
		msleep(10);
		B5xF5_F4[ii] = ( gpio_i2c_read(raptor3_i2c_addr[devnum], 0xF5) << 8 ) | gpio_i2c_read(raptor3_i2c_addr[devnum], 0xF4);
	}

	check_point = __s_distinguish_5M_ahd_tvi_func( B5xF5_F4, check_time );

	if( fmtdef == AHD30_5M_20P )
	{
		if( ( check_point & 0xfff ) == 0x7c2 )
			decoder_info->fmt_def = TVI_5M_20P;
	}
	else
	{
		decoder_info->fmt_def = fmtdef;
	}


}

unsigned int __s_distinguish_8M_cvi_tvi_func(unsigned int* input_arry, int cnt)
{
	unsigned int chk1, chk2;
	unsigned int max, max_idx = 0;
	unsigned int calc_array[10][10] = { {0, 0},  };
	unsigned int need_update = 0;
	unsigned int find_idx = 0;
	unsigned int ii, ij;
	unsigned int inner_idx = 0;

	chk1 = input_arry[0];

	for(ii = 0; ii < cnt; ii++)
	{
		chk2 = input_arry[ii];

		if( chk1 == chk2)
		{
			calc_array[0][inner_idx] += 1;
			calc_array[1][inner_idx] = chk1;
		}
		else if( chk1 != chk2 )
		{
			for(ij = 0; ij < ii; ij++)
			{
				if( calc_array[1][ij] == chk2 )
				{
					find_idx = ij;
					calc_array[0][find_idx] += 1;
					calc_array[1][find_idx] = chk2;
					need_update = 0;
					break;
				}
				need_update = 1;
			}

			if(need_update)
			{
				inner_idx += 1;
				calc_array[0][inner_idx] += 1;
				calc_array[1][inner_idx] = chk2;
			}
		}

		chk1 = chk2;
	}

	max = calc_array[0][0];

	for(ii = 0; ii < cnt; ii++)
	{
		if( max < calc_array[0][ii] )
		{
			max_idx = ii;
			max = calc_array[0][ii];
		}
	}

	return calc_array[1][max_idx];
}

void video_input_cvi_tvi_distinguish(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char ch = decoder_info->ch;
	unsigned char devnum = decoder_info->devnum;
	unsigned char fmtdef = decoder_info->fmt_def;
	unsigned char ii;
	unsigned int check_point;

	unsigned char check_time = 10;

	unsigned int B13xAB[10];
	unsigned int B5xF5_F4[10];

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x13);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2B, ch % 4 );

	for(ii = 0; ii < check_time; ii++)
	{
		msleep(10);
		B13xAB[ii] = gpio_i2c_read(raptor3_i2c_addr[devnum], 0xAB);
	}

	check_point = __s_distinguish_8M_cvi_tvi_func( B13xAB, check_time );

	if( fmtdef == CVI_8M_15P || fmtdef == CVI_8M_12_5P )
	{
		if( ( check_point & 0xff ) > 0x1A )
		{
			if( fmtdef == CVI_8M_12_5P )
				decoder_info->fmt_def = TVI_8M_12_5P;
			else
				decoder_info->fmt_def = TVI_8M_15P;
		}
	}
	else if(fmtdef == CVI_4M_25P)
	{
		gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF, 0x05 + ch);

		for(ii = 0; ii < check_time; ii++)
		{
			msleep(10);
			B5xF5_F4[ii] = ( gpio_i2c_read(raptor3_i2c_addr[devnum], 0xF5) << 8 ) | gpio_i2c_read(raptor3_i2c_addr[devnum], 0xF4);
		}

		check_point = __s_distinguish_5M_ahd_tvi_func( B5xF5_F4, check_time );

		if( ( check_point & 0xfff ) == 0x5E2)
			decoder_info->fmt_def = TVI_4M_25P;
	}
	else
	{
		decoder_info->fmt_def = fmtdef;
	}
}

/******************************************************************************
 *   Description     : Get Y plus slope
 *   Argurments      : ch : channel number
 *   Return value    : Y plus slope value
 *   Modify          :
 *   warning         :
 *******************************************************************************/
unsigned int __get_LE_YPlusSlope(unsigned char ch, unsigned char devnum)
{
	unsigned int y_plus_slp_status;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x05+ch);
	y_plus_slp_status = gpio_i2c_read(raptor3_i2c_addr[devnum],0xE8)&0x07;
	y_plus_slp_status <<= 8;
	y_plus_slp_status |= gpio_i2c_read(raptor3_i2c_addr[devnum],0xE9);

	return y_plus_slp_status;
}



/******************************************************************************
 *   Description     : Get Y minus slope
 *   Argurments      : ch : channel number
 *   Return value    : Y minus slope value
 *   Modify          :
 *   warning         :
 *******************************************************************************/
unsigned int __get_LE_YMinusSlope(unsigned char ch, unsigned char devnum)
{
	unsigned int y_minus_slp_status;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x05+ch);
	y_minus_slp_status = gpio_i2c_read(raptor3_i2c_addr[devnum],0xEA)&0x07;
	y_minus_slp_status <<= 8;
	y_minus_slp_status |= gpio_i2c_read(raptor3_i2c_addr[devnum],0xEB);

	return y_minus_slp_status;
}

/******************************************************************************
 *   Description     : Get EQ pattern color gain
 *   Argurments      : ch : channel number
 *   Return value    : EQ pattern color gain value
 *   Modify          :
 *   warning         :
 *******************************************************************************/
unsigned int __get_EQ_AccGain(unsigned char ch, unsigned char devnum)
{
	unsigned int acc_gain_status;

	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xFF,0x05+ch);
	acc_gain_status = gpio_i2c_read(raptor3_i2c_addr[devnum],0xE2) & 0x1f;
	acc_gain_status <<= 8;
	acc_gain_status |= gpio_i2c_read(raptor3_i2c_addr[devnum],0xE3);
	//printk("acc_gain_status    : 0x%x\n", acc_gain_status);
	return acc_gain_status;
}

unsigned int __s_distinguish_2M_ahd_cvi_func(unsigned int* input_arry, int cnt)
{
	unsigned int chk1, chk2;
	unsigned int max, max_idx = 0;
	unsigned int calc_array[10][10] = { {0, 0},  };
	unsigned int need_update = 0;
	unsigned int find_idx = 0;
	unsigned int ii, ij;
	unsigned int inner_idx = 0;

	chk1 = input_arry[0];

	for(ii = 0; ii < cnt; ii++)
	{
		chk2 = input_arry[ii];

		if( chk1 == chk2)
		{
			calc_array[0][inner_idx] += 1;
			calc_array[1][inner_idx] = chk1;
		}
		else if( chk1 != chk2 )
		{
			for(ij = 0; ij < ii; ij++)
			{
				if( calc_array[1][ij] == chk2 )
				{
					find_idx = ij;
					calc_array[0][find_idx] += 1;
					calc_array[1][find_idx] = chk2;
					need_update = 0;
					break;
				}
				need_update = 1;
			}

			if(need_update)
			{
				inner_idx += 1;
				calc_array[0][inner_idx] += 1;
				calc_array[1][inner_idx] = chk2;
			}
		}

		chk1 = chk2;
	}

	max = calc_array[0][0];

	for(ii = 0; ii < cnt; ii++)
	{
		if( max < calc_array[0][ii] )
		{
			max_idx = ii;
			max = calc_array[0][ii];
		}
	}


	for(ii = 0; ii < cnt; ii++)
	{
		printk("[DRV] [ idx %d ] [ num %d ] [ val %x ]\n", ii, calc_array[0][ii], calc_array[1][ii]);
	}

	printk("[DRV] [ max_idx : %d ]\n", max_idx);
	printk("[DRV] [ inner_idx : %d ]\n", inner_idx);

	return calc_array[1][max_idx];
}

static unsigned int __n_first_boot[16] = {0, };

void video_input_ahd_cvi_distinguish(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char ch = decoder_info->ch;
	unsigned char devnum = decoder_info->devnum;
	unsigned char fmtdef = decoder_info->fmt_def;
	unsigned char ii;

	unsigned char check_time = 10;

	unsigned int B5xE3_E2[10];
	unsigned int B5xE9_E8[10];
	unsigned int B5xEB_EA[10];

	unsigned int y_p_slp;
	unsigned int y_m_slp;
	unsigned int acc_gain;


	for( ii = 0; ii < check_time; ii++ )
	{
		B5xE9_E8[ii] = __get_LE_YPlusSlope( ch, devnum);
		B5xEB_EA[ii] = __get_LE_YMinusSlope( ch, devnum);
		B5xE3_E2[ii] = __get_EQ_AccGain( ch, devnum );
		msleep(16);
	}

	y_p_slp = __s_distinguish_2M_ahd_cvi_func( B5xE9_E8, check_time );
	y_m_slp = __s_distinguish_2M_ahd_cvi_func( B5xEB_EA, check_time );
	acc_gain = __s_distinguish_2M_ahd_cvi_func( B5xE3_E2, check_time );

	if( fmtdef == CVI_FHD_30P )
	{
		if( !__n_first_boot[ch])
		{
			{
				printk("[DRV] [Ch:%d] maybe now situation first boot environment..  acc_gain result is error. Forced Setting  CVI 2M\n", decoder_info->ch);
				decoder_info->fmt_def = CVI_FHD_30P;
				__n_first_boot[ch] = 1;
				return;
			}
		}

		if( acc_gain > 0x1f40 )
		{
			decoder_info->fmt_def = AHD20_1080P_30P;
			printk("[DRV] [Ch:%d] acc_gain result is AHD 2M\n", decoder_info->ch);
		}
		else
		{
			decoder_info->fmt_def = CVI_FHD_30P;
			printk("[DRV] [Ch:%d] acc_gain result is CVI 2M\n", decoder_info->ch);
		}
	}
	else
	{
		decoder_info->fmt_def = fmtdef;
	}
}


