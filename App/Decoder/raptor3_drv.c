#include <string.h>
#include "NVP6158.h"
#include "delay.h"
#include "video_input_table.h"
#include "video_eq_table.h"

/*******************************************************************************
*	Description		: Get Device ID
*	Argurments		: dec(slave address)
*	Return value	: Device ID
*	Modify			:
*	warning			:
*******************************************************************************/
int check_id(unsigned char dec)
{
	int ret = 0;
	NVP6158_I2C_WRITE(dec, 0xFF, 0x00);
	ret = NVP6158_I2C_READ(dec, 0xf4);
	return ret;
}
void video_output_colorbar_set( void )
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x00);
	// colorbar off
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x78, 0x88);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x79, 0x88);
	// colorbar on
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x78, 0xab);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x79, 0xcd);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x68, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x6a, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x6b, 0x00);

	//  Colorbar on
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x05);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x08);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x06);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x08);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x07);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x08);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x08);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x08);
	// Colorbar off
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x05);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x06);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x07);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x08);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2c, 0x00);

	// moving cursor
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x05);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x6a, 0xb0);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x06);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x6a, 0xb0);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x07);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x6a, 0xb0);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x08);
//	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x6a, 0xb0);
}
void _video_input_auto_detect_vafe_set(video_input_auto_detect *vin_auto_det)
{
	unsigned char val_1x7A;
	unsigned char val_5678x00;
	unsigned char val_5678x01;
	unsigned char val_5678x58;
	unsigned char val_5678x59;
	unsigned char val_5678x5B;
	unsigned char val_5678x5C;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x01);
	val_1x7A = NVP6158_I2C_READ(NVP6158_ADDR, 0x7A);
	val_1x7A |= (1 << vin_auto_det->ch);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x7A, val_1x7A);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	//B0 0x00/1/2/3 gain[4], powerdown[0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + vin_auto_det->ch, ((vin_auto_det->vafe.gain & 0x01) << 4) |
												  (vin_auto_det->vafe.powerdown & 0x01));

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x01);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x84 + vin_auto_det->ch, 0x00);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//B5/6/7/8
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + vin_auto_det->ch);

	//B5/6/7/8 0x01 spd[2], lpf_back_band[1:0]
	val_5678x00 = NVP6158_I2C_READ(NVP6158_ADDR, 0x00);
	val_5678x00 &= ~(0xF << 4);
	val_5678x00 |= vin_auto_det->vafe.spd << 4;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00, val_5678x00);

	val_5678x01 = ((vin_auto_det->vafe.ctrlreg << 6) | (vin_auto_det->vafe.ctrlibs << 4) | (vin_auto_det->vafe.adcspd << 2) | (vin_auto_det->vafe.clplevel));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x01, val_5678x01 );

	//B5/6/7/8 0x58 eq_band[7:4], lpf_front_band[1:0]
	val_5678x58 = ((vin_auto_det->vafe.eq_band << 4) | (vin_auto_det->vafe.lpf_front_band));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x58, val_5678x58);

	//B5/6/7/8 0x5B ref_vol[1:0]
	val_5678x59 = ((vin_auto_det->vafe.clpmode << 7) | (vin_auto_det->vafe.f_lpf_bypass << 4) | (vin_auto_det->vafe.clproff << 3) | (vin_auto_det->vafe.b_lpf_bypass));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x59, val_5678x59);

	val_5678x5B = ((vin_auto_det->vafe.duty << 4) | (vin_auto_det->vafe.ref_vol));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5B, val_5678x5B);

	val_5678x5C = ((vin_auto_det->vafe.lpf_back_band << 4) | (vin_auto_det->vafe.clk_sel << 3) | (vin_auto_det->vafe.eq_gainsel));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5C, val_5678x5C);
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

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + vin_auto_det->ch);

	//B5/6/7/8 0x03 Digital Clamp
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x03, vin_auto_det->d_cmp);
	//B5/6/7/8 0x08 Slice Level
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x08, vin_auto_det->slice_level);
	//B5/6/7/8 0xB9 HAFC_LPF_SEL[7:6] GAIN1[5:4] GAIN2[3:0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB9, 0x72);

	//B5/6/7/8 0xCA ADV_V_DELAY_AD[4] ADV_V_DELAY_ON[0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xCA, 0x10);

	//B13 0x30 AUTO_FMT_SET_EN_2[3:0], AUTO_FMT_SET_EN    [3:0]
	//B13 0x31 AUTO_FMT_SET_EN_4[3:0], AUTO_FMT_SET_EN_3  [3:0]
	//B13 0x32 [	  RESERVED  	], NOVIDEO_VFC_INIT_EN[3:0]

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x13);
	val_0x30 = NVP6158_I2C_READ(NVP6158_ADDR, 0x30);
	val_0x31 = NVP6158_I2C_READ(NVP6158_ADDR, 0x31);
	val_0x32 = NVP6158_I2C_READ(NVP6158_ADDR, 0x32);
	val_0x30 |= ((1 << (vin_auto_det->ch + 4)) | (1 << vin_auto_det->ch));
	val_0x31 |= ((1 << (vin_auto_det->ch + 4)) | (1 << vin_auto_det->ch));
	val_0x32 |= ((1 << vin_auto_det->ch) & 0xF);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x30, val_0x30);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x31, val_0x31);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x32, val_0x32);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x36, 0x0A);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x37, 0x82);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x81+vin_auto_det->ch, 0x0A);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x85+vin_auto_det->ch, 0x02);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//B13
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x13);
	//B13 0x00  Stable Mode set
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00, vin_auto_det->stable_mode_1);
	//B13 0x01  Stable Mode Set
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x01, ((vin_auto_det->stable_mode_2) & 0x3));
	//B13 0x40 VFC_EQ_BAND_SEL[7:4] VFC_LPF_F_SEL[1:0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x40, 0x07);
	//B13 0x41 VFC_REF_VTG
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x41, 0x01);
	//B13 0x42 VFC_D_CMP_SET
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x42, 0x3F);
	//B13 0x43 VFC_SLICE_VALUE
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x43, 0x5A);
	//B13 0x44 VFC_SLICE_MD2
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x44, 0x30);
	//B13 0x45 VFC_CONTROL_MODES
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x45, 0xEE);
	//B13 0x46 VFC_GDF_FIX_COEFF
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x46, 0xC6);
	//B13 0x47 VFC_DFE_REF_SEL_OLD[4] VFC_DFE_REF_SEL_NEW[0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x47, 0x00);
	//B13 0x48 VFC_D_BLK_CNT_NEW[[7:4] VFC_HAFC_BYPASS_NEW[1] VFC_UPDN_SEL[0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x48, 0x80);
	//B13 0x49 VFC_OLD_WPD_ON[4] VFC_NEW_WPD_ON[0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x49, 0x00);
	//B13 0x4A VFC_D_CMP_FZ_OLD[4] VFC_D_CMP_FZ_NEW[1]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4A, 0x11);
	//B13 0x4B VFC_AUTO_GNOS_MODE
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4B, 0x7F);
	//B13 0x4C VFC_AUTO_SYNC_MODE
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4C, 0x00);
	//B13 0x4D VFC_HAFC_BYPASS[7] ??? [6:0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4D, 0xB9);
	//B13 0x4E VFC_VAFE_B_LPF_SEL[6:4] VFC_VAFE_CKSEL[3] VFC_VAFE_EQ_GAIN_SEL[2:0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4E, 0x78);
	//B13 0x4F VFC_VAFE_CTRL_RES[7:6] VFC_VAFE_IBS_CTRL[5:4] VFC_VAFE_SPD[2] VFC_VAFE_CLP_LEVEL[1:0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4F, 0x62);

	//B0  0x23/0x27/0x2B/0x2F No Video Detect
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x0);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x23 + ((vin_auto_det->ch) * 4), vin_auto_det->novid_det);

	/* clock set */
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x1);
    //NVP6158_I2C_WRITE(NVP6158_ADDR, 0x84 + vin_auto_det->ch, 0x04);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x84 + vin_auto_det->ch, 0x00);
    NVP6158_I2C_WRITE(NVP6158_ADDR, 0x8c + vin_auto_det->ch, 0x55);

//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0xa + (vin_auto_det->ch / 2));
//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0x74 + ((vin_auto_det->ch) * 0x80), 0x02); // y_slope_input_sel
//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0x75 + ((vin_auto_det->ch % 2) * 0x80), 0x03); // frame starting point 3
//
//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0xFF, 0x05 + vin_auto_det->ch);
//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0xc0, 0x16);	// get slope data start v count
//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0xc1, 0x05);	// get slope data width v count
//															// Check video signal 6 line
//	NVP6158_I2C_WRITE(raptor3_i2c_addr[vin_auto_det->devnum], 0xc8, 0x04);	// get slope avg 1 frame
}

void video_input_vfc_read(video_input_vfc *vin_vfc)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x13);
	vin_vfc->vfc = NVP6158_I2C_READ(NVP6158_ADDR, 0xF0 + vin_vfc->ch);
}

void video_input_novid_read(video_input_novid *vin_novid)
{
	unsigned char val_0xA8;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	val_0xA8 = NVP6158_I2C_READ(NVP6158_ADDR, 0xA8);

	vin_novid->novid = (((val_0xA8 >> vin_novid->ch) & 0x1)) ;
}

unsigned char __IsOver3MRTVideoFormat_auto_detect( decoder_dev_ch_info_s *decoder_info )
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
	   (decoder_info->fmt_def == CVI_8M_12_5P)
	   )
	{
		ret = 1;
	}
	return ret; // 0:Over 3M RT, 1:other formation
}

unsigned char __IsOver3MRTVideoFormat( const char *poFmtName )
{
	unsigned char ret = 1; //

	if((strcmp(poFmtName, "AHD30_3M_30P") == 0) ||
	   (strcmp(poFmtName, "AHD30_3M_25P") == 0) ||
	   (strcmp(poFmtName, "AHD30_4M_30P") == 0) ||
	   (strcmp(poFmtName, "AHD30_4M_25P") == 0) ||
	   (strcmp(poFmtName, "AHD30_5M_20P") == 0) ||
	   (strcmp(poFmtName, "AHD30_5_3M_20P") == 0) ||
	   (strcmp(poFmtName, "AHD30_6M_18P") == 0) ||
	   (strcmp(poFmtName, "AHD30_6M_20P") == 0) ||
	   (strcmp(poFmtName, "AHD30_8M_12_5P") == 0) ||
	   (strcmp(poFmtName, "AHD30_8M_15P") == 0) ||
	   (strcmp(poFmtName, "TVI_4M_30P") == 0) ||
	   (strcmp(poFmtName, "TVI_4M_25P") == 0) ||
	   (strcmp(poFmtName, "TVI_5M_20P") == 0) ||
	   (strcmp(poFmtName, "CVI_4M_25P") == 0) ||
	   (strcmp(poFmtName, "CVI_4M_30P") == 0) ||
	   (strcmp(poFmtName, "CVI_8M_15P") == 0) ||
	   (strcmp(poFmtName, "CVI_8M_12_5P") == 0))
	{
		ret = 0;
	}
	return ret; // 0:Over 3M RT, 1:other formation
}
void video_input_vafe_reset(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char val_0x00;
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	val_0x00 = NVP6158_I2C_READ(NVP6158_ADDR, 0x00 + decoder_info->ch);
	_SET_BIT(val_0x00, 0);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + decoder_info->ch, val_0x00);
	Delay_ms(10);
	_CLE_BIT(val_0x00, 0);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + decoder_info->ch, val_0x00);
}

unsigned char s_only_onetime_run[32] = {0, };
void video_input_onvideo_set(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char format_3M_RT;

	/* after 09/12 */
	format_3M_RT = __IsOver3MRTVideoFormat_auto_detect(decoder_info);

	if(format_3M_RT)
	{
		/* DECI_FILTER_ON */
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + decoder_info->ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x50, 0x76);
	}
	else
	{
		/* DECI_FILTER_OFF */
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + decoder_info->ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x50, 0xc6);
	}


		if(	decoder_info->fmt_def == CVI_HD_30P 		||
			decoder_info->fmt_def == CVI_HD_30P_EX		||
			decoder_info->fmt_def == AHD20_720P_30P		||
			decoder_info->fmt_def == AHD20_720P_30P_EX	||
			decoder_info->fmt_def == AHD20_720P_30P_EX_Btype)
		{
			//meant to remove pre-connection issue. 07.31
			if( s_only_onetime_run[decoder_info->ch * decoder_info->devnum] == 0)
			{
			video_input_vafe_reset(decoder_info);
				s_only_onetime_run[decoder_info->ch * decoder_info->devnum] = 1;
			}
		}
		else
		{
			if( s_only_onetime_run[decoder_info->ch * decoder_info->devnum] == 0)
			{
				s_only_onetime_run[decoder_info->ch * decoder_info->devnum] = 1;
			}
		}

}

void video_input_h_timing_set(decoder_dev_ch_info_s *pDevChInfo)
{
	unsigned char val_0x54;
	video_input_basic_timing_init_s h_timing = decoder_basic_timing_fmtdef[ pDevChInfo->fmt_def ];

	if(pDevChInfo->ch>3)
	{
		return;
	}


	//B0
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	//H_DELAY_LSB B0 0x58/9/a/b
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x58 + pDevChInfo->ch, h_timing.h_delay_lsb);
	//H_MASK_OB B0 0x89/a/b/c
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x89 + pDevChInfo->ch, h_timing.h_mask_on);
	//H_MASK_SEL B0 0x8E/F/90/91
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x8e + pDevChInfo->ch, h_timing.h_mask_sel);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xA0 + pDevChInfo->ch, h_timing.v_blk_end_b);

	val_0x54 = NVP6158_I2C_READ(NVP6158_ADDR, 0x54);
	val_0x54 &= ~((0x1) << (pDevChInfo->ch + 4));
	val_0x54 |= ((h_timing.fld_inv & 0x1) << (pDevChInfo->ch + 4));

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x54, val_0x54);

	//B5/6/7/8
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + pDevChInfo->ch);
	//SYNC_RS B5/6/7/8 0x47
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x47, h_timing.sync_rs);
	//MEM_RDP B5/6/7/8 0x64
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x64, h_timing.mem_rdp);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xA9, h_timing.v_blk_end_b);
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
		video_input_color_init_s color_init = decoder_color_fmtdef[decoder_info->fmt_def];

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x00 );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x0c + decoder_info->ch, color_init.brightnees);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x10 + decoder_info->ch, color_init.contrast );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x22 + (decoder_info->ch*4), 0x0B );				// FIxed Value
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x3c + decoder_info->ch, color_init.saturation_a );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x40 + decoder_info->ch, color_init.hue      );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x44 + decoder_info->ch, color_init.u_gain   );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x48 + decoder_info->ch, color_init.v_gain   );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x4c + decoder_info->ch, color_init.u_offset );
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x50 + decoder_info->ch, color_init.v_offset );

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x05 + decoder_info->ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2b, color_init.saturation_b);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x20, color_init.black_level);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x24, color_init.burst_dec_a);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5f, color_init.burst_dec_b);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xd1, color_init.burst_dec_c);
	}
	else
	{
		//printk("NOT_YET_SUPPORT_COLOR_SET_FORMAT:[%d]",decoder_info->fmt_def);
	}


	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x05 + decoder_info->ch);

	if(decoder_info->fmt_def == TVI_5M_12_5P)
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x26,0x20);
	else
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x26,0x40);


	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + decoder_info->ch);

	if(decoder_info->fmt_def == AHD20_SD_H960_2EX_Btype_NT || decoder_info->fmt_def == AHD20_SD_H960_2EX_Btype_PAL)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB8, 0xB8);	// SD Rees Only
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x09);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x40 + decoder_info->ch, 0x60);
	}
	else
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB8, 0x39); // Other Format
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x09);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x40 + decoder_info->ch, 0x00);
	}
}

void video_input_vafe_init(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char val_5678x00;
	unsigned char val_5678x01;
	unsigned char val_5678x58;
	unsigned char val_5678x59;
	unsigned char val_5678x5B;
	unsigned char val_5678x5C;

	video_input_vafe_init_s afe = decoder_afe_fmtdef [decoder_info->fmt_def];

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);

	//B0 0x00/1/2/3 gain[4], powerdown[0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + decoder_info->ch, ((afe.gain & 0x01) << 4) | (afe.powerdown & 0x01));

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x01);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x84 + decoder_info->ch, 0x00);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//B5/6/7/8
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + decoder_info->ch);

	//B5/6/7/8 0x01 spd[2], lpf_back_band[1:0]
	val_5678x00 = NVP6158_I2C_READ(NVP6158_ADDR, 0x00);
	val_5678x00 |= (afe.spd << 4);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00, val_5678x00);

	val_5678x01 = ((afe.ctrlreg << 6) | (afe.ctrlibs << 4) | (afe.adcspd << 2) | (afe.clplevel));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x01, val_5678x01 );

	//B5/6/7/8 0x58 eq_band[7:4], lpf_front_band[1:0]
	val_5678x58 = ((afe.eq_band << 4) | (afe.lpf_front_band));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x58, val_5678x58);

		//B5/6/7/8 0x5B ref_vol[1:0]
	val_5678x59 = ((afe.clpmode << 7) | (afe.f_lpf_bypass << 4) | (afe.clproff << 3) | (afe.b_lpf_bypass));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x59, val_5678x59);

	val_5678x5B = ((afe.duty << 4) | (afe.ref_vol));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5B, val_5678x5B);

	val_5678x5C = ((afe.lpf_back_band << 4) | (afe.clk_sel << 3) | (afe.eq_gainsel));
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5C, val_5678x5C);
}

void video_input_vafe_control(decoder_dev_ch_info_s *decoder_info, int cmd)
{
	unsigned char val_0x00;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);

	if(cmd == 0)
	{
		val_0x00 = NVP6158_I2C_READ(NVP6158_ADDR, 0x00 + decoder_info->ch);
		_SET_BIT(val_0x00, 0);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + decoder_info->ch, val_0x00);

		Delay_ms(10);
	}
	else if(cmd == 1)
	{
		val_0x00 = NVP6158_I2C_READ(NVP6158_ADDR, 0x00 + decoder_info->ch);
		_CLE_BIT(val_0x00, 0);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + decoder_info->ch, val_0x00);

		Delay_ms(10);
	}
}

void video_input_new_format_set(decoder_dev_ch_info_s *decoder_info)
{
	unsigned char ch = decoder_info->ch % 4;
	//unsigned char fmtdef = decoder_info->fmt_def;
	unsigned char val_9x44;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF,0x09);
	val_9x44 = NVP6158_I2C_READ(NVP6158_ADDR, 0x44);
	val_9x44 &= ~(1 << ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x44, val_9x44);

		/* CVI HD 30P PN Value Set */
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x50 + ( ch * 4 ) , 0x30);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x51 + ( ch * 4 ) , 0x6F);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x52 + ( ch * 4 ) , 0x67);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x53 + ( ch * 4 ) , 0x48);

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x11);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + ( ch * 0x20 ), 0x00);
}

void video_input_onvideo_check_data(video_input_vfc *vin_vfc)
{
	unsigned char val_5678xF0;
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + vin_vfc->ch);
	val_5678xF0 = NVP6158_I2C_READ(NVP6158_ADDR, 0xF0);
	vin_vfc->vfc = val_5678xF0;
}

void video_input_no_video_set(video_input_novid_set *auto_novid)
{
	unsigned char val_13x30;
	unsigned char val_13x31;
	unsigned char val_13x32;


	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x21 + (auto_novid->ch * 4), 0x82);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x23 + (auto_novid->ch * 4), 0x41);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x23 + (auto_novid->ch * 4), 0x43);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x09);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x80 + (auto_novid->ch * 0x20), 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x81 + (auto_novid->ch * 0x20), 0x00);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + auto_novid->ch);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x01, 0x62);

	/* Before 08/28 */
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x58, 0x07);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5C, 0x78);
	/* After 08/28 */
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x58, 0x47);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x5C, 0x7f);

        /* Low-Poass Filter (LPF) Bypass Enable  Bank5/6/7/8 0x59 */
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x59, 0x10);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB8, 0xB8);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB9, 0xB2);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x13);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x43, 0x5a);

	val_13x30 = NVP6158_I2C_READ(NVP6158_ADDR, 0x30);
	val_13x31 = NVP6158_I2C_READ(NVP6158_ADDR, 0x31);
	val_13x32 = NVP6158_I2C_READ(NVP6158_ADDR, 0x32);
	val_13x30 |= ((1 << (auto_novid->ch + 4)) | (1 << auto_novid->ch));
	val_13x31 |= ((1 << (auto_novid->ch + 4)) | (1 << auto_novid->ch));
	val_13x32 |= ((1 << auto_novid->ch) & 0xF);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x30, val_13x30);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x31, val_13x31);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x32, val_13x32);

	/* disable Bank11 0x00, if before setting format TVI 5M 20P when onvideo */
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x11);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + ( auto_novid->ch * 0x20 ), 0x00);
}

void video_input_contrast_off(decoder_dev_ch_info_s *decoder_info)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x10 + decoder_info->ch, 0x00);
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
	unsigned char fmtdef = decoder_info->fmt_def;
	unsigned char ii;
	unsigned int check_point;

	unsigned char check_time = 10;
	unsigned int B5xF5_F4[10];

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + ch);

	for(ii = 0; ii < check_time; ii++)
	{
		Delay_ms(10);
		B5xF5_F4[ii] = ( NVP6158_I2C_READ(NVP6158_ADDR, 0xF5) << 8 ) | NVP6158_I2C_READ(NVP6158_ADDR, 0xF4);
	}

	check_point = __s_distinguish_5M_ahd_tvi_func( B5xF5_F4, check_time );

	if( fmtdef == AHD30_5M_20P )
	{
		if( ( check_point & 0xfff ) == 0x7c2 )
		{
			decoder_info->fmt_def = TVI_5M_20P;
		}
		else
		{
		}
	}
	else
	{
		decoder_info->fmt_def = fmtdef;
	}
}

void video_output_channel_sequence_get(video_output_sequence_reg *vout_seq_reg)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);

	vout_seq_reg->b1c0 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc0);
	vout_seq_reg->b1c1 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc1);
	vout_seq_reg->b1c2 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc2);
	vout_seq_reg->b1c3 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc3);
	vout_seq_reg->b1c4 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc4);
	vout_seq_reg->b1c5 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc5);
	vout_seq_reg->b1c6 = NVP6158_I2C_READ(NVP6158_ADDR, 0xc6);
}

void video_output_channel_sequence_set(video_output_sequence *vout_seq)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc0 + (vout_seq->port * 2), vout_seq->ch01);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc1 + (vout_seq->port * 2), vout_seq->ch23);
}

void video_output_port_format_mode_set( video_output_port_fmt_s *pformat )
{
	unsigned char tmp_data;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 1);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xed, pformat->merge & 0xff);

	if((pformat->merge&0x1) || !(pformat->merge & 0x1))
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 5);
		tmp_data = NVP6158_I2C_READ(NVP6158_ADDR, 0x69);

		if(pformat->merge & 0x1)
		{
			tmp_data |= 0x10;
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
	}

	if(((pformat->merge >> 1) &0x1) || !((pformat->merge >> 1) & 0x1))
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 6);
		tmp_data = NVP6158_I2C_READ(NVP6158_ADDR, 0x69);

		if((pformat->merge >> 1) & 0x1)
		{
			tmp_data |= 0x10;
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
	}

	if(((pformat->merge >> 2) &0x1) || !((pformat->merge >> 2) & 0x1))
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 7);
		tmp_data = NVP6158_I2C_READ(NVP6158_ADDR, 0x69);

		if((pformat->merge >> 2) & 0x1)
		{
			tmp_data |= 0x10;
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
	}

	if(((pformat->merge >> 3) &0x1) || !((pformat->merge >> 3) & 0x1))
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 8);
		tmp_data = NVP6158_I2C_READ(NVP6158_ADDR, 0x69);

		if((pformat->merge >> 3) & 0x1)
		{
			tmp_data |= 0x10;
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x69, tmp_data);
		}
	}
}

void video_output_port_mux_mode_set(video_output_port_mux_mode *vout_port_mux_mode)
{
	unsigned char val;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);
	switch(vout_port_mux_mode->port)
	{
		case PORTA:
					val = NVP6158_I2C_READ(NVP6158_ADDR, 0xc8) & 0xf0;
					val |= (vout_port_mux_mode->mode & 0xf);
					NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc8, val);
					break;
		case PORTB:
					val = NVP6158_I2C_READ(NVP6158_ADDR, 0xc8) & 0x0f;
					val |= ((vout_port_mux_mode->mode & 0xf) << 4);

					NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc8, val);
					break;
		case PORTC:
					val = NVP6158_I2C_READ(NVP6158_ADDR, 0xc9) & 0xf0;
					val |= (vout_port_mux_mode->mode & 0xf);

					NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc9, val);
					break;
		case PORTD:
					val = NVP6158_I2C_READ(NVP6158_ADDR, 0xc9) & 0x0f;
					val |= ((vout_port_mux_mode->mode & 0xf) << 4);

					NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc9, val);
					break;
	}
}

void video_output_port_mux_chid_set(video_output_port_mux_chid *vout_chid)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x55, 0x10);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0x56, 0x10);
}

void video_output_data_out_mode_set(video_output_data_out_mode *vo_data_out_mode)
{
	unsigned char temp_val = 0x0;

	//  Show/Hide mode is using register Bank 0 0x7A, 7B
	// 		   CH2	  CH1		    CH4    CH3
	//	0x7A [7 : 4][3 : 0]  0x7B [7 : 4][3 : 0]
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);

	switch(vo_data_out_mode->ch)
	{
		case CH1 :
		case CH2 : temp_val = NVP6158_I2C_READ(NVP6158_ADDR, 0x7A);
					break;
		case CH3 :
		case CH4 : temp_val = NVP6158_I2C_READ(NVP6158_ADDR, 0x7B);
					break;
	}

	switch(vo_data_out_mode->ch)
	{
		case CH1 :
		case CH3 :	temp_val = ((temp_val & 0xF0) | (vo_data_out_mode->set_val & 0xF));
					break;
		case CH2 :
		case CH4 :  temp_val = ((temp_val & 0x0F) | ((vo_data_out_mode->set_val & 0xF) << 4));
					break;
	}

	// printk("[%s:%s] : %s >>>> temp_val [ %x ]\n", __FILE__, __LINE__, __FUNCTION__,temp_val);
	switch(vo_data_out_mode->ch)
	{
		case CH1 :
		case CH2 : NVP6158_I2C_WRITE(NVP6158_ADDR, 0x7A, temp_val);
				   break;
		case CH3 :
		case CH4 : NVP6158_I2C_WRITE(NVP6158_ADDR, 0x7B, temp_val);
				   break;
	}
}

void video_out_auto_vclk_set(clock_video_output *clk_vout)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xCC + clk_vout->port, clk_vout->clk_sel);
}

void video_out_port_enable(video_output_port_enable_s *portenable )
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xca, portenable->enable & 0xff);
}


void video_input_manual_agc_stable_endi(decoder_dev_ch_info_s *decoder_info, int endi)
{
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05+decoder_info->ch);
	if( endi == 1 )
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x82, 0xff);
	}
	else
	{

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x82, 0x00);
	}
}

/**************************************************************************************
* @desc
* 	RAPTOR3's
*
* @param_in		(motion_mode *)p_param->channel                  FW Update channel
*
* @return   	void  		       								 None
*
* ioctl : IOC_VDEC_MOTION_SET
***************************************************************************************/
void motion_detection_get(motion_mode *motion_set)
{
	//BANK2_MOTION
	unsigned char ReadVal = 0;
	unsigned char ch_mask = 1;
	unsigned char ch = motion_set->ch;
	unsigned char ret = 0;

	ch_mask = ch_mask<<ch;

	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x00);
	ReadVal = NVP6158_I2C_READ(NVP6158_ADDR, 0xA9);

	ret = ReadVal&ch_mask;
	motion_set->set_val = ret;
}

extern unsigned char Get_MotionIndication(void);
void motion_onoff_set(motion_mode *motion_set)
{
	//BANK2_MOTION
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x02);

	if(motion_set->fmtdef == TVI_3M_18P || motion_set->fmtdef == TVI_5M_12_5P || motion_set->fmtdef == TVI_5M_20P)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + (0x07 * motion_set->ch), 0x0C);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x02 + (0x07 * motion_set->ch), 0x23);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x11);

		if(motion_set->fmtdef == TVI_3M_18P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0x78);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x40);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x72);
		}
		else if(motion_set->fmtdef == TVI_5M_12_5P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0xA2);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x51);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x9c);
		}
		else if(motion_set->fmtdef == TVI_5M_20P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0xA0);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x51);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x9a);
		}

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2B + (0x06 * motion_set->ch), 0x6);

	}
	else
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
	}

	if(motion_set->set_val<0 || motion_set->set_val>1)
	{
		return;
	}

	switch(motion_set->set_val)
	{
		case FUNC_OFF : NVP6158_I2C_WRITE(NVP6158_ADDR, (0x00 + (0x07 * motion_set->ch)), 0x0D);
						NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
			break;
		case FUNC_ON : 
			if(Get_MotionIndication() == FUNC_ON)
				NVP6158_I2C_WRITE(NVP6158_ADDR, (0x00 + (0x07 * motion_set->ch)), 0x0C);
			else
				NVP6158_I2C_WRITE(NVP6158_ADDR, (0x00 + (0x07 * motion_set->ch)), 0x00);
			break;
	}


}

void motion_pixel_all_onoff_set(motion_mode *motion_set)
{
	int ii=0;

	//BANK2_MOTION
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x02);

	if(motion_set->fmtdef == TVI_3M_18P || motion_set->fmtdef == TVI_5M_12_5P || motion_set->fmtdef == TVI_5M_20P)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + (0x07 * motion_set->ch), 0x0C);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x02 + (0x07 * motion_set->ch), 0x23);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x11);

		if(motion_set->fmtdef == TVI_3M_18P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0x78);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x40);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x72);
		}
		else if(motion_set->fmtdef == TVI_5M_12_5P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0xA2);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x51);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x9c);
		}
		else if(motion_set->fmtdef == TVI_5M_20P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0xA0);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x51);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x9a);
		}
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2B + (0x06 * motion_set->ch), 0x6);

	}
	else
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
	}

	for(ii=0; ii<24; ii++)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, (0x40 +(0x18 *motion_set->ch)) + ii, motion_set->set_val);
	}
}

void motion_pixel_onoff_set(motion_mode *motion_set)
{
	unsigned char val = 0x80;
	unsigned char ReadVal;
	unsigned char on;

	unsigned char ch      = motion_set->ch;
	unsigned char SetPix  = motion_set->set_val/8;
	unsigned char SetVal  = motion_set->set_val%8;

	val = val >> SetVal;

	//BANK2_MOTION
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x02);

	if(motion_set->fmtdef == TVI_3M_18P || motion_set->fmtdef == TVI_5M_12_5P || motion_set->fmtdef == TVI_5M_20P)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00 + (0x07 * motion_set->ch), 0x0C);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x02 + (0x07 * motion_set->ch), 0x23);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x11);

		if(motion_set->fmtdef == TVI_3M_18P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0x78);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x40);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x72);
		}
		else if(motion_set->fmtdef == TVI_5M_12_5P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0xA2);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x51);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x9c);
		}
		else if(motion_set->fmtdef == TVI_5M_20P)
		{
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x29 + (0x06 * motion_set->ch), 0xA0);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2A + (0x06 * motion_set->ch), 0x51);
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C + (0x06 * motion_set->ch), 0x9a);
		}

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2B + (0x06 * motion_set->ch), 0x6);

	}
	else
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
	}

	ReadVal = NVP6158_I2C_READ(NVP6158_ADDR, (0x40 +(0x18 *ch)) + SetPix);
	on = val&ReadVal;
	if(on)
	{
		val = ~val;
		val = val&ReadVal;
	}
	else
	{
		val = val|ReadVal;
	}
	NVP6158_I2C_WRITE(NVP6158_ADDR, (0x40 +(0x18 *ch)) + SetPix, val);
}

void motion_pixel_onoff_get(motion_mode *motion_set)
{
	unsigned char val = 0x80;
	unsigned char ReadVal;
	unsigned char on;

	unsigned char Ch      = motion_set->ch;
	unsigned char SetPix  = motion_set->set_val/8;
	unsigned char SetVal  = motion_set->set_val%8;

	val = val >> SetVal;

	//BANK2_MOTION
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x02);
	ReadVal = NVP6158_I2C_READ(NVP6158_ADDR, (0x40 +(0x18 *Ch)) + SetPix);

	on = val&ReadVal;

	if(on)
	{
		motion_set->set_val = 1;
	}
	else
	{
		motion_set->set_val = 0;
	}
}

void motion_tsen_set(motion_mode *motion_set)
{
	unsigned char ch = motion_set->ch;
	unsigned char SetVal = motion_set->set_val;

	//BANK2_MOTION
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x02);
	NVP6158_I2C_WRITE(NVP6158_ADDR, (0x01 +(0x07 * ch)), SetVal);
}

void motion_psen_set(motion_mode *motion_set)
{
	unsigned char msb_mask = 0xf0;
	unsigned char lsb_mask = 0x07;
	unsigned char ch = motion_set->ch;
	unsigned char SetVal = motion_set->set_val;
	unsigned char ReadVal;

	//BANK2_MOTION
	NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x02);
	ReadVal = NVP6158_I2C_READ(NVP6158_ADDR, (0x02 +(0x07 * ch)));

	msb_mask = msb_mask&ReadVal;
	SetVal = lsb_mask&SetVal;

	SetVal = SetVal|msb_mask;

	NVP6158_I2C_WRITE(NVP6158_ADDR, (0x02 +(0x07 * ch)), SetVal);
}

