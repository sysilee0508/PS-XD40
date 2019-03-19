/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: video_output.c
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
#include "video_output.h"
#include "common.h"

extern unsigned int raptor3_i2c_addr[4];

void video_output_colorbar_set( unsigned char devnum )
{
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x00);
	
#ifdef __DEBUG_COLORBAR
	// colorbar on
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x78, 0xab);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x79, 0xcd);
#else
	// colorbar off
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x78, 0x88);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x79, 0x88);
#endif
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x68, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x69, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6a, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6b, 0x00);

#ifdef __DEBUG_COLORBAR
	//  Colorbar on
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x08);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x06);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x08);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x07);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x08);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x08);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x08);
#else
	// Colorbar off
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x06);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x07);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x08);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x2c, 0x00);
#endif
#ifdef __DEBUG_COLORBAR
	// moving cursor
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x05);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6a, 0xb0);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x06);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6a, 0xb0);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x07);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6a, 0xb0);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0xff, 0x08);
	gpio_i2c_write(raptor3_i2c_addr[devnum], 0x6a, 0xb0);
#endif
}

void video_output_channel_sequence_set(video_output_sequence *vout_seq)
{
	gpio_i2c_write(raptor3_i2c_addr[vout_seq->devnum], 0xff, 0x01);

	#ifdef __DEBUG_MSG
	printk("[DRV] vout_seq->port = %x\n", vout_seq->port);
	printk("[DRV] vout_seq->ch01 = %x\n", vout_seq->ch01);
	printk("[DRV] vout_seq->ch23 = %x\n", vout_seq->ch23);
	#endif

	gpio_i2c_write(raptor3_i2c_addr[vout_seq->devnum], 0xc0 + (vout_seq->port * 2), vout_seq->ch01);
	gpio_i2c_write(raptor3_i2c_addr[vout_seq->devnum], 0xc1 + (vout_seq->port * 2), vout_seq->ch23);
}

void video_output_channel_sequence_get(video_output_sequence_reg *vout_seq_reg)
{
	gpio_i2c_write(raptor3_i2c_addr[vout_seq_reg->devnum], 0xff, 0x01);

	vout_seq_reg->b1c0 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc0);
	vout_seq_reg->b1c1 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc1);
	vout_seq_reg->b1c2 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc2);
	vout_seq_reg->b1c3 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc3);
	vout_seq_reg->b1c4 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc4);
	vout_seq_reg->b1c5 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc5);
	vout_seq_reg->b1c6 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc6);
	vout_seq_reg->b1c7 = gpio_i2c_read(raptor3_i2c_addr[vout_seq_reg->devnum], 0xc7);
}

void video_output_port_mux_mode_set(video_output_port_mux_mode *vout_port_mux_mode)
{
	unsigned char val;

	#ifdef __DEBUG_MSG
	printk("[DRV] vout_port_mux_mode->port = %x\n", vout_port_mux_mode->port);
	printk("[DRV] vout_port_mux_mode->mode = %x\n", vout_port_mux_mode->mode);
	printk("[DRV] vout_port_mux_mode->devnum = %x\n", vout_port_mux_mode->devnum);
	#endif

	gpio_i2c_write(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xff, 0x01);
	switch(vout_port_mux_mode->port)
	{
		case PORTA:
					val = gpio_i2c_read(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc8) & 0xf0;
					val |= (vout_port_mux_mode->mode & 0xf);
					gpio_i2c_write(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc8, val);
					break;
		case PORTB:
					val = gpio_i2c_read(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc8) & 0x0f;
					val |= ((vout_port_mux_mode->mode & 0xf) << 4);

					gpio_i2c_write(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc8, val);
					break;
		case PORTC:
					val = gpio_i2c_read(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc9) & 0xf0;
					val |= (vout_port_mux_mode->mode & 0xf);

					gpio_i2c_write(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc9, val);
					break;
		case PORTD:
					val = gpio_i2c_read(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc9) & 0x0f;
					val |= ((vout_port_mux_mode->mode & 0xf) << 4);

					gpio_i2c_write(raptor3_i2c_addr[vout_port_mux_mode->devnum], 0xc9, val);
					break;
	}
}

void video_output_port_format_mode_set( video_output_port_fmt_s *pformat )
{
	unsigned char tmp_data;

	gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0xff, 1);
	gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0xed, pformat->merge & 0xff);

	if((pformat->merge&0x1) || !(pformat->merge & 0x1))
	{
		gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0xff, 5);
		tmp_data = gpio_i2c_read(raptor3_i2c_addr[pformat->devnum], 0x69);

		if(pformat->merge & 0x1)
		{
			tmp_data |= 0x10;
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
	}

	if(((pformat->merge >> 1) &0x1) || !((pformat->merge >> 1) & 0x1))
	{
		gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0xff, 6);
		tmp_data = gpio_i2c_read(raptor3_i2c_addr[pformat->devnum], 0x69);

		if((pformat->merge >> 1) & 0x1)
		{
			tmp_data |= 0x10;
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
	}

	if(((pformat->merge >> 2) &0x1) || !((pformat->merge >> 2) & 0x1))
	{
		gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0xff, 7);
		tmp_data = gpio_i2c_read(raptor3_i2c_addr[pformat->devnum], 0x69);

		if((pformat->merge >> 2) & 0x1)
		{
			tmp_data |= 0x10;
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
	}

	if(((pformat->merge >> 3) &0x1) || !((pformat->merge >> 3) & 0x1))
	{
		gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0xff, 8);
		tmp_data = gpio_i2c_read(raptor3_i2c_addr[pformat->devnum], 0x69);

		if((pformat->merge >> 3) & 0x1)
		{
			tmp_data |= 0x10;
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
		else
		{
			tmp_data &= ~(0x10);
			gpio_i2c_write(raptor3_i2c_addr[pformat->devnum], 0x69, tmp_data);
		}
	}
}

void video_out_port_enable(video_output_port_enable_s *portenable )
{
	gpio_i2c_write(raptor3_i2c_addr[portenable->devnum], 0xff, 0x01);
	gpio_i2c_write(raptor3_i2c_addr[portenable->devnum], 0xca, portenable->enable & 0xff);
}

void video_output_port_mux_chid_set(video_output_port_mux_chid *vout_chid)
{
	gpio_i2c_write(raptor3_i2c_addr[vout_chid->devnum], 0xFF, 0x00);
	gpio_i2c_write(raptor3_i2c_addr[vout_chid->devnum], 0x55, 0x10);
	gpio_i2c_write(raptor3_i2c_addr[vout_chid->devnum], 0x56, 0x10);
}

/********************************************************************************
* End of file
********************************************************************************/
