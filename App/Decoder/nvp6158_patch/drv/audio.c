/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: Raptor3 Device Driver
*  Description	: audio.c
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
#include "audio.h"

extern unsigned int raptor3_i2c_addr[4];

void audio_default_set(audio_set *audio_default_val)
{
	/* Audio Default Set ... Device 0 - later you have to consider multi device */
	audio_default_val->devnum  = 0x00;

	audio_default_val->clk_sel = 0x00;
	audio_default_val->bitrate = 0x80;
	audio_default_val->aigain1 = 0x09;
	audio_default_val->aigain2 = 0x09;
	audio_default_val->aigain3 = 0x09;
	audio_default_val->aigain4 = 0x09;
	audio_default_val->migain1 = 0x09;
	audio_default_val->aigain5 = 0x09;
	audio_default_val->aigain6 = 0x09;
	audio_default_val->aigain7 = 0x09;
	audio_default_val->aigain8 = 0x09;
	audio_default_val->aogain  = 0x08;
	audio_default_val->mix_outsel = 0x00;
	audio_default_val->mux_setting = 0x09;
	audio_default_val->delay_setting = 0x07;
	audio_default_val->live_mute = 0x00;
	audio_default_val->filter_en = 0x00;
	audio_default_val->dc_level = 0xf0;
	audio_default_val->sys_clk = 0x40;

	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0xFF, 0x01);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x94, audio_default_val->clk_sel);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x07, audio_default_val->bitrate);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x01, audio_default_val->aigain1);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x02, audio_default_val->aigain2);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x03, audio_default_val->aigain3);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x04, audio_default_val->aigain4);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x05, audio_default_val->migain1);

	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x22, audio_default_val->aogain);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x23, audio_default_val->mix_outsel);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x31, audio_default_val->mux_setting);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x47, audio_default_val->delay_setting);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x49, audio_default_val->live_mute);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x44, audio_default_val->filter_en);

	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x32, audio_default_val->dc_level);
	gpio_i2c_write(raptor3_i2c_addr[audio_default_val->devnum], 0x94, audio_default_val->sys_clk);
}
