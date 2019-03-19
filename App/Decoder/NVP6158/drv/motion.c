/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: motion.c
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

extern unsigned int NVP6158_ADDR[4];

#include "motion.h"

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

//	printk("motion_detection_get:: %x\n", motion_set->set_val);

}

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

		printk("[DRV_Motion_OnOff] ch(%d) fmtdef(%d)\n", motion_set->ch, motion_set->fmtdef);
	}
	else
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
	}

	if(motion_set->set_val<0 || motion_set->set_val>1)
	{
		printk("[DRV_Motion_OnOff]Error!! ch(%d) Setting Value Over:%x!! Only 0 or 1\n", motion_set->ch, motion_set->set_val);
		return;
	}

	switch(motion_set->set_val)
	{
		case FUNC_OFF : NVP6158_I2C_WRITE(NVP6158_ADDR, (0x00 + (0x07 * motion_set->ch)), 0x0D);
						NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
			break;
		case FUNC_ON : NVP6158_I2C_WRITE(NVP6158_ADDR, (0x00 + (0x07 * motion_set->ch)), 0x0C);
			break;
	}


}

void motion_pixel_all_onoff_set(motion_mode *motion_set)
{
	int ii=0;
	unsigned char addr = 0;

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

		printk("[DRV_Motion_OnOff] ch(%d) fmtdef(%d)\n", motion_set->ch, motion_set->fmtdef);
	}
	else
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x28 + (0x06 * motion_set->ch), 0x00);
	}

	for(ii=0; ii<24; ii++)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, (0x40 +(0x18 *motion_set->ch)) + ii, motion_set->set_val);
		addr = (0x40 +(0x18 *motion_set->ch)) + ii;
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

		printk("[DRV_Motion_OnOff] ch(%d) fmtdef(%d)\n", motion_set->ch, motion_set->fmtdef);
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
	printk("[DRV_Motion]ch(%d), TSEN Val(%x)\n", ch, SetVal);
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
	printk("[DRV_Motion]ch(%d), readVal(%x), SetVal(%x)\n", ch, ReadVal, SetVal);
}
