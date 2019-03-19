/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: raptor3_drv.c
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
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/list.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "common.h"
#include "video_output.h"
#include "video_input.h"
#include "video_auto_detect.h"
#include "clock.h"
#include "coax_protocol.h"
#include "motion.h"
#include "audio.h"
#include "video_eq.h"

#include <raptor3_ioctl.h>

struct semaphore raptor3_lock;

unsigned int raptor3_i2c_addr[4] = {0x60, 0x62, 0x64, 0x66};
static unsigned int raptor3_cnt = 0;

int chip_id[4];
int rev_id[4];

decoder_get_information_str decoder_inform;

#define RAPTOR3_4PORT_R0_ID 0xA1
#define RAPTOR3_2PORT_R0_ID 0xA0
#define RAPTOR3_1PORT_R0_ID 0xA2
#define AFE_NVP6134E_R0_ID 	0x80

#define RAPTOR3_4PORT_REV_ID 0x00
#define RAPTOR3_2PORT_REV_ID 0x00
#define RAPTOR3_1PORT_REV_ID 0x00

/*******************************************************************************
*	Description		: Get Device ID
*	Argurments		: dec(slave address)
*	Return value	: Device ID
*	Modify			:
*	warning			:
*******************************************************************************/
int check_id(unsigned int dec)
{
	int ret;
	gpio_i2c_write(dec, 0xFF, 0x00);
	ret = gpio_i2c_read(dec, 0xf4);
	return ret;
}

/*******************************************************************************
*	Description		: Get rev ID
*	Argurments		: dec(slave address)
*	Return value	: rev ID
*	Modify			:
*	warning			:
*******************************************************************************/
int check_rev(unsigned int dec)
{
	int ret;
	gpio_i2c_write(dec, 0xFF, 0x00);
	ret = gpio_i2c_read(dec, 0xf5);
	return ret;
}


static struct i2c_board_info hi_info =
{
    I2C_BOARD_INFO("raptor", 0x60),
};

struct i2c_client* raptor3_client;



int raptor3_open(struct inode * inode, struct file * file)
{
	printk("[DRV] Raprot3 Driver Open\n");
	return 0;
} 

int raptor3_close(struct inode * inode, struct file * file)
{
	return 0;
}

long raptor3_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int cpy2usr_ret;
	unsigned int __user *argp = (unsigned int __user *)arg;	

	video_output_port_enable_s vout_port_enable;
	video_output_sequence vout_seq;
	video_output_sequence_reg vout_seq_reg;
	video_output_port_mux_mode vout_port_mux_mode;
	video_output_port_mux_chid vout_chid;
	video_input_auto_detect vin_auto_det;
	video_input_vfc vin_vfc;
	video_input_novid vin_novid;
	video_input_novid_set vin_novid_set;
	video_input_cable_dist vin_cable_dist;
	video_input_sam_val vin_sam_val;
	video_input_hsync_accum vin_hsync_acuum;
	video_input_agc_val vin_agc_val;
	video_input_fsc_val vin_fsc_val;
	video_input_aeq_set vin_aeq_set;
	video_input_deq_set vin_deq_set;
	video_equalizer_info_s vin_eq_set;
	video_input_acc_gain_val vin_acc_gain;
	video_output_data_out_mode vo_data_out_mode;
	video_output_port_fmt_s vout_port_format;
	video_input_manual_mode vin_manual_det;
	decoder_dev_ch_info_s dev_ch_info;
	clock_video_output clk_vout_auto;
	motion_mode motion_set;
	audio_set audio_default_val;
	clock clk_adc;
	clock clk_dec;
	NC_VD_COAX_STR           coax_val;
	NC_VD_COAX_BANK_DUMP_STR coax_bank_dump;
	FIRMWARE_UP_FILE_INFO    coax_fw_val;
	NC_VD_COAX_TEST_STR      coax_test_val;

	down(&raptor3_lock);

	switch (cmd)
	{
		/*-------------------------- Clock -------------------------*/
		case IOC_VDEC_ADC_CLOCK_SET:
			if(copy_from_user(&clk_adc, argp, sizeof(clock)))
				printk("IOC_VDEC_ADC_CLOCK_SET error\n");
			adc_clock_set(&clk_adc);
			break;
		case IOC_VDEC_PRE_CLOCK_SET:
			if(copy_from_user(&clk_dec, argp, sizeof(clock)))
				printk("IOC_VDEC_DEC_CLOCK_SET error\n");
			pre_clock_set(&clk_dec);
			break;
		case IOC_VDEC_OUTPUT_AUTO_VCLK_SET : // 170208 Temp TEST Auto Vclk Set
			if(copy_from_user(&clk_vout_auto, argp, sizeof(clock_video_output)))
				printk("IOC_VDEC_VIDEO_OUTPUT_CLOCK_SET error\n");
			video_out_auto_vclk_set(&clk_vout_auto);
			break;
		case IOC_VDEC_CLOCK_SET :
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_CLOCK_SET error\n");
			clock_set(&dev_ch_info);
			break;
		/*----------------------- Video Input ----------------------*/
		case IOC_VDEC_INPUT_FORMAT_SET:
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_FORMAT_SET error\n");
			video_input_format_set(&dev_ch_info);
			break;
		case IOC_VDEC_INPUT_CHROMA_SET:
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_CHROMA_SET error\n");
			video_input_chroma_set(&dev_ch_info);
			break;
		case IOC_VDEC_INPUT_H_TIMING_SET:
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_H_TIMING_SET error\n");
			video_input_h_timing_set(&dev_ch_info);
			break;
		case IOC_VDEC_INPUT_H_SCALER_MODE_SET:
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_H_SCALER_MODE_SET error\n");
			video_input_h_scaler_mode_set(&dev_ch_info);
			break;
		case IOC_VDEC_INPUT_HPLL_SET:		
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_HPLL_SET error\n");
			video_input_hpll_set(&dev_ch_info);
			break;
		case IOC_VDEC_INPUT_COLOR_SET :
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_COLOR_SET error\n");
			video_input_color_set(&dev_ch_info);
			break;
		case IOC_VDEC_INPUT_AFE_INIT :
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_INPUT_AFE_INIT error\n");
			video_input_vafe_init(&dev_ch_info);
			break;
		case IOC_VDEC_AUTO_CABLE_DIST_GET:	
			if(copy_from_user(&vin_cable_dist, argp, sizeof(video_input_cable_dist)))
				printk("IOC_VDEC_INPUT_CABLE_DIST_GET error\n");
			video_input_cable_dist_read(&vin_cable_dist);
			cpy2usr_ret = copy_to_user(argp, &vin_cable_dist, sizeof(video_input_cable_dist));
			break;
		case IOC_VDEC_MANUAL_CABLE_DIST_GET:
			if(copy_from_user(&vin_cable_dist, argp, sizeof(video_input_cable_dist)))
				printk("IOC_VDEC_INPUT_CABLE_DIST_GET error\n");
			video_input_cable_manualdist_read(&vin_cable_dist);
			cpy2usr_ret = copy_to_user(argp, &vin_cable_dist, sizeof(video_input_cable_dist));
			break;
		case IOC_VDEC_MANUAL_EQ_DIST_SET:
			if(copy_from_user(&vin_eq_set, argp, sizeof(video_equalizer_info_s)))
				printk("IOC_VDEC_MANUAL_EQ_DIST_SET error\n");
			video_input_eq_val_set(&vin_eq_set);
			break;
		case IOC_VDEC_AUTO_SAM_VAL_GET:	
			if(copy_from_user(&vin_sam_val, argp, sizeof(video_input_sam_val)))
				printk("IOC_VDEC_INPUT_SAM_VAL_GET error\n");
			video_input_sam_val_read(&vin_sam_val);
			cpy2usr_ret = copy_to_user(argp, &vin_sam_val, sizeof(video_input_sam_val));
			break;
		case IOC_VDEC_AUTO_HSYNC_ACCUM_GET:
			if(copy_from_user(&vin_hsync_acuum, argp, sizeof(video_input_hsync_accum)))
				printk("IOC_VDEC_INPUT_HSYNC_ACCUM_GET error\n");
			video_input_hsync_accum_read(&vin_hsync_acuum);
			cpy2usr_ret = copy_to_user(argp, &vin_hsync_acuum, sizeof(video_input_hsync_accum));
			break;
		case IOC_VDEC_AUTO_AGC_VAL_GET:	
			if(copy_from_user(&vin_agc_val, argp, sizeof(video_input_agc_val)))
				printk("IOC_VDEC_INPUT_AGC_VAL_GET error\n");
			video_input_agc_val_read(&vin_agc_val);
			cpy2usr_ret = copy_to_user(argp, &vin_agc_val, sizeof(video_input_agc_val));
			break;
		case IOC_VDEC_AUTO_NOVID_SET :		
			if(copy_from_user(&vin_novid_set, argp, sizeof(video_input_novid_set)))
				printk("IOC_VDEC_INPUT_NOVID_SET error\n");
			video_input_no_video_set(&vin_novid_set);
			break;
		case IOC_VDEC_AUTO_NOVID_GET:		
			if(copy_from_user(&vin_novid, argp, sizeof(video_input_novid)))
				printk("IOC_VDEC_INPUT_NOVID_GET error\n");
			video_input_novid_read(&vin_novid);
			cpy2usr_ret = copy_to_user(argp, &vin_novid, sizeof(video_input_novid));
			break;
		case IOC_VDEC_AUTO_VFC_GET:
			if(copy_from_user(&vin_vfc, argp, sizeof(video_input_vfc)))
				printk("IOC_VDEC_INPUT_VFC_GET error\n");
			video_input_vfc_read(&vin_vfc);
			cpy2usr_ret = copy_to_user(argp, &vin_vfc, sizeof(video_input_vfc));
			break;
		case IOC_VDEC_AUTO_DETECT_MODE_SET:
			if(copy_from_user(&vin_auto_det, argp, sizeof(video_input_auto_detect)))
				printk("IOC_VDEC_INPUT_AUTO_DETECT_SET error\n");
			video_input_auto_detect_set(&vin_auto_det);
			break;
		case IOC_VDEC_AUTO_FSC_GET:					// 0x50	170214 FSC Value Read
			if(copy_from_user(&vin_fsc_val, argp, sizeof(video_input_fsc_val)))
				printk("IOC_VDEC_INPUT_FSC_GET error\n");
			video_input_fsc_val_read(&vin_fsc_val);
			cpy2usr_ret = copy_to_user(argp, &vin_fsc_val, sizeof(video_input_fsc_val));
			break;
		case IOC_VDEC_AUTO_AEQ_SET:					// 0x51
			if(copy_from_user(&vin_aeq_set, argp, sizeof(video_input_aeq_set)))
				printk("IOC_VDEC_INPUT_AEQ_SET error\n");
			video_input_aeq_val_set(&vin_aeq_set);
			break;
		case IOC_VDEC_AUTO_DEQ_SET:					// 0x52
			if(copy_from_user(&vin_deq_set, argp, sizeof(video_input_deq_set)))
				printk("IOC_VDEC_INPUT_DEQ_SET error\n");
			video_input_deq_val_set(&vin_deq_set);
			break;
		case IOC_VDEC_AUTO_ACC_GAIN_GET:				// 0x53		170215 ACC_GAIN Value Read
			if(copy_from_user(&vin_acc_gain, argp, sizeof(video_input_acc_gain_val)))
				printk("IOC_VDEC_INPUT_DEQ_SET error\n");
			video_input_acc_gain_val_read(&vin_acc_gain);
			cpy2usr_ret = copy_to_user(argp, &vin_acc_gain, sizeof(video_input_acc_gain_val));
			break;
		case IOC_VDEC_AUTO_DATA_OUT_MODE_SET :
			if(copy_from_user(&vo_data_out_mode, argp, sizeof(video_output_data_out_mode)))
				printk("IOC_VDEC_AUTO_DATA_OUT_MODE_SET error\n");
			video_output_data_out_mode_set(&vo_data_out_mode);
			break;
		case IOC_VDEC_MANUAL_MODE_SET :
			if(copy_from_user(&vin_manual_det, argp, sizeof(video_input_manual_mode)))
				printk("IOC_VDEC_INPUT_MANUAL_MODE_SET error\n");
			video_input_manual_mode_set(&vin_manual_det);
			break;
		case IOC_VDEC_AUTO_ONVIDEO_SET :
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk(" IOC_VDEC_AUTO_ONVIDEO_SET error\n");
				video_input_onvideo_set(&dev_ch_info);
			break;
		case IOC_VDEC_AUTO_ONVIDEO_CHECK :
			if(copy_from_user(&vin_vfc, argp, sizeof(video_input_vfc)))
				printk(" IOC_VDEC_AUTO_ONVIDEO_CHECK error\n");
			video_input_onvideo_check_data(&vin_vfc);
			cpy2usr_ret = copy_to_user(argp, &vin_vfc,sizeof(video_input_vfc));
			break;
		case IOC_VDEC_HAFC_GAIN12_CTRL :
			if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk(" IOC_VDEC_HAFC_GAIN12_CTRL error\n");
			video_input_hafc_gain_ctrl(&dev_ch_info);
			break;
		/*----------------------- Video Output ----------------------*/
		case IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_SET:
			if(copy_from_user(&vout_seq, argp, sizeof(video_output_sequence)))
				printk("IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_SET error\n");
			video_output_channel_sequence_set(&vout_seq);
			break;
		case IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_GET:
			if(copy_from_user(&vout_seq_reg, argp, sizeof(video_output_sequence_reg)))
				printk("IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_GET error\n");
			video_output_channel_sequence_get(&vout_seq_reg);
			cpy2usr_ret = copy_to_user(argp, &vout_seq_reg, sizeof(video_output_sequence_reg));
			break;
		case IOC_VDEC_OUTPUT_PORT_MUX_MODE_SET:
			if(copy_from_user(&vout_port_mux_mode, argp, sizeof(video_output_port_mux_mode)))
				printk("IOC_VDEC_OUTPUT_PORT_MUX_MODE_SET error\n");
			video_output_port_mux_mode_set(&vout_port_mux_mode);
			break;
		case IOC_VDEC_OUTPUT_PORT_ENABLE_SET:
			if(copy_from_user(&vout_port_enable, argp, sizeof(video_output_port_enable_s)))
				printk("IOC_VDEC_OUTPUT_PORT_ENABLE_SET error\n");
			video_out_port_enable(&vout_port_enable);
			break;
		case IOC_VDEC_OUTPUT_PORT_MUX_CHID_SET :
			if(copy_from_user(&vout_chid, argp, sizeof(video_output_port_mux_chid)))
				printk("IOC_VDEC_OUTPUT_PORT_MUX_CHID_SET error\n");
			video_output_port_mux_chid_set(&vout_chid);
			break;
		case IOC_VDEC_OUTPUT_PORT_FORMAT_SET:
			if(copy_from_user(&vout_port_format, argp, sizeof(video_output_port_fmt_s)))
				printk("IOC_VDEC_OUTPUT_PORT_FORMAT_SET error\n");
			video_output_port_format_mode_set(&vout_port_format);
			break;
		/*----------------------- Coaxial Protocol ----------------------*/
		case IOC_VDEC_COAX_TX_INIT:   //SK_CHANGE 170703
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk("IOC_VDEC_COAX_TX_INIT error\n");
			coax_tx_init(&coax_val);
				break;
		case IOC_VDEC_COAX_TX_16BIT_INIT:   //SK_CHANGE 170703
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk("IOC_VDEC_COAX_TX_INIT error\n");
			coax_tx_16bit_init(&coax_val);
				break;
		case IOC_VDEC_COAX_TX_CMD_SEND: //SK_CHANGE 170703
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk(" IOC_VDEC_COAX_TX_CMD_SEND error\n");
			coax_tx_cmd_send(&coax_val);
				break;
		case IOC_VDEC_COAX_TX_16BIT_CMD_SEND: //SK_CHANGE 170703
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk(" IOC_VDEC_COAX_TX_CMD_SEND error\n");
			coax_tx_16bit_cmd_send(&coax_val);
				break;
			case IOC_VDEC_COAX_TX_CVI_NEW_CMD_SEND: //SK_CHANGE 170703
				if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
						printk(" IOC_VDEC_COAX_TX_CMD_SEND error\n");
				coax_tx_cvi_new_cmd_send(&coax_val);
					break;
		case IOC_VDEC_COAX_RX_INIT:
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk(" IOC_VDEC_COAX_RX_INIT error\n");
			coax_rx_init(&coax_val);
			break;
		case IOC_VDEC_COAX_RX_DATA_READ:
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
			printk(" IOC_VDEC_COAX_RX_DATA_READ error\n");
			coax_rx_data_get(&coax_val);
			cpy2usr_ret = copy_to_user(argp, &coax_val, sizeof(NC_VD_COAX_STR));
			break;
		case IOC_VDEC_COAX_RX_BUF_CLEAR:
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk(" IOC_VDEC_COAX_RX_BUF_CLEAR error\n");
			coax_rx_buffer_clear(&coax_val);
			break;
		case IOC_VDEC_COAX_RX_DEINIT:
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
				printk("IOC_VDEC_COAX_RX_DEINIT error\n");
			coax_rx_deinit(&coax_val);
			break;
		case IOC_VDEC_COAX_BANK_DUMP_GET:
			if(copy_from_user(&coax_bank_dump, argp, sizeof(NC_VD_COAX_BANK_DUMP_STR)))
			printk("IOC_VDEC_COAX_BANK_DUMP_GET error\n");
			coax_test_Bank_dump_get(&coax_bank_dump);
			cpy2usr_ret = copy_to_user(argp, &coax_bank_dump, sizeof(NC_VD_COAX_BANK_DUMP_STR));
			break;
		/*=============== Coaxial Protocol A-CP Option ===============*/
		case IOC_VDEC_COAX_RT_NRT_MODE_CHANGE_SET:
			if(copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
			printk(" IOC_VDEC_COAX_SHOT_SET error\n");
			coax_option_rt_nrt_mode_change_set(&coax_val);
			cpy2usr_ret = copy_to_user(argp, &coax_val, sizeof(NC_VD_COAX_STR));
			break;
		/*=========== Coaxial Protocol Firmware Update ==============*/
		case IOC_VDEC_COAX_FW_ACP_HEADER_GET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_ACP_HEADER_GET error\n");
			coax_fw_ready_header_check_from_isp_recv(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_READY_CMD_SET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_READY_CMD_SET error\n");
			coax_fw_ready_cmd_to_isp_send(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_READY_ACK_GET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_READY_ISP_STATUS_GET error\n");
			coax_fw_ready_cmd_ack_from_isp_recv(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_START_CMD_SET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n");
			coax_fw_start_cmd_to_isp_send(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_START_ACK_GET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n");
			coax_fw_start_cmd_ack_from_isp_recv(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_SEND_DATA_SET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n");
			coax_fw_one_packet_data_to_isp_send(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_SEND_ACK_GET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n");
			coax_fw_one_packet_data_ack_from_isp_recv(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_END_CMD_SET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n");
			coax_fw_end_cmd_to_isp_send(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		case IOC_VDEC_COAX_FW_END_ACK_GET:
			if(copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
							printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n");
			coax_fw_end_cmd_ack_from_isp_recv(&coax_fw_val);
			cpy2usr_ret = copy_to_user(argp, &coax_fw_val, sizeof(FIRMWARE_UP_FILE_INFO));
			break;
		/*=========== Coaxial Protocol Firmware Update END ==============*/
		case IOC_VDEC_COAX_TEST_TX_INIT_DATA_READ:
			if(copy_from_user(&coax_test_val, argp, sizeof(NC_VD_COAX_TEST_STR)))
			printk("IOC_VDEC_COAX_INIT_SET error\n");
			coax_test_tx_init_read(&coax_test_val);
			cpy2usr_ret = copy_to_user(argp, &coax_test_val, sizeof(NC_VD_COAX_TEST_STR));
			break;
		case IOC_VDEC_COAX_TEST_DATA_SET:
			if(copy_from_user(&coax_test_val, argp, sizeof(NC_VD_COAX_TEST_STR)))
				printk("IOC_VDEC_COAX_TEST_DATA_SET error\n");
			coax_test_data_set(&coax_test_val);
			break;
		case IOC_VDEC_COAX_TEST_DATA_READ:
			if(copy_from_user(&coax_test_val, argp, sizeof(NC_VD_COAX_TEST_STR)))
				printk("IOC_VDEC_COAX_TEST_DATA_SET error\n");
			coax_test_data_get(&coax_test_val);
			cpy2usr_ret = copy_to_user(argp, &coax_test_val, sizeof(NC_VD_COAX_TEST_STR));
			break;
		/*----------------------- MOTION ----------------------*/
		case IOC_VDEC_MOTION_DETECTION_GET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_SET error\n");
			motion_detection_get(&motion_set);
			cpy2usr_ret = copy_to_user(argp, &motion_set, sizeof(motion_mode));
		break;
		case IOC_VDEC_MOTION_SET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_SET error\n");
			motion_onoff_set(&motion_set);
			break;
		case IOC_VDEC_MOTION_PIXEL_SET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_Pixel_SET error\n");
			motion_pixel_onoff_set(&motion_set);
		break;
		case IOC_VDEC_MOTION_PIXEL_GET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_Pixel_SET error\n");
			motion_pixel_onoff_get(&motion_set);
			cpy2usr_ret = copy_to_user(argp, &motion_set, sizeof(motion_mode));
			break;
		case IOC_VDEC_MOTION_ALL_PIXEL_SET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_Pixel_SET error\n");
			motion_pixel_all_onoff_set(&motion_set);
		break;
		case IOC_VDEC_MOTION_TSEN_SET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_TSEN_SET error\n");
			motion_tsen_set(&motion_set);
		break;
		case IOC_VDEC_MOTION_PSEN_SET :
			if(copy_from_user(&motion_set, argp, sizeof(motion_set)))
				printk("IOC_VDEC_MOTION_PSEN_SET error\n");
			motion_psen_set(&motion_set);
		break;
		/*----------------------- AUDIO -----------------*/
		case IOC_VDEC_AUDIO_DEFAULT_SET :
			if(copy_from_user(&audio_default_val, argp, sizeof(audio_set)))
				printk("IOC_VDEC_AUDIO_DEFAULT_SET error\n");
			audio_default_set(&audio_default_val);
		break;
		/*----------------------- TEMP FUNCTION ----------------------*/
		case IOC_VDEC_GET_CHIP_ID :
			if(copy_to_user(argp, &decoder_inform, sizeof(decoder_get_information_str)))
				printk("IOC_VDEC_GET_CHIP_ID error\n");
		break;
		case IOC_VDEC_CH_SW_RESET :
		if(copy_from_user(&dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
			printk("IOC_VDEC_CH_SW_RESET error\n");
			video_input_auto_ch_sw_rst(&dev_ch_info);
		break;
		case IOC_VDEC_AFE_RESET :
		if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
			printk("IOC_VDEC_AFE_RESET error\n");
			video_input_vafe_reset(&dev_ch_info);
		break;

		case IOC_VDEC_AFE_CONTROL_PD :
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_AFE_CONTROL PowerDown error\n");
			video_input_vafe_control(&dev_ch_info, 0);
		break;

		case IOC_VDEC_AFE_CONTROL_PU :
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_AFE_CONTROL PowerUp error\n");
			video_input_vafe_control(&dev_ch_info, 1);
		break;

		case IOC_VDEC_MANUAL_AGC_STABLE_ENABLE:
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_MANUAL_AGC_STABLE_ENABLE error\n");
			video_input_manual_agc_stable_endi(&dev_ch_info, 1);
		break;
		case IOC_VDEC_MANUAL_AGC_STABLE_DISABLE:
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_MANUAL_AGC_STABLE_DISABLE error\n");
			video_input_manual_agc_stable_endi(&dev_ch_info, 0);
		break;
		case IOC_VDEC_CONTRAST_OFF :
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_CONTRAST_OFF error\n");
			video_input_contrast_off(&dev_ch_info);
			break;
		case IOC_VDEC_NEW_FORMAT_SET :
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_NEW_FORMAT_SET error\n");
			video_input_new_format_set(&dev_ch_info);
			break;
		case IOC_VDEC_AHD_TVI_DISTINGUISH :
			if(copy_from_user( &dev_ch_info, argp, sizeof(decoder_dev_ch_info_s)))
				printk("IOC_VDEC_AHD_TVI_DISTINGUISH error\n");
			video_input_ahd_tvi_distinguish(&dev_ch_info);
			cpy2usr_ret = copy_to_user(argp, &dev_ch_info, sizeof(decoder_dev_ch_info_s));
			break;
	}

	up(&raptor3_lock);

	return 0;
}

static int i2c_client_init(void)
{
    struct i2c_adapter* i2c_adap;


#ifdef SUPPORT_3520D_6158C
	printk("HI_CHIP_HI3520D\n");
    i2c_adap = i2c_get_adapter(0);
#else
	printk("HI_CHIP_HI3531A\n");
    i2c_adap = i2c_get_adapter(1);
#endif
    raptor3_client = i2c_new_device(i2c_adap, &hi_info);
    i2c_put_adapter(i2c_adap);

    return 0;
}

static void i2c_client_exit(void)
{
    i2c_unregister_device(raptor3_client);
}

static struct file_operations raptor3_fops = {
	.owner      = THIS_MODULE,
    .unlocked_ioctl	= raptor3_ioctl,
	.open       = raptor3_open,
	.release    = raptor3_close
};

static struct miscdevice raptor3_dev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "raptor3",
	.fops  		= &raptor3_fops,
};

/*******************************************************************************
*	Description		: It is called when "insmod nvp61XX_ex.ko" command run
*	Argurments		: void
*	Return value	: -1(could not register nvp61XX device), 0(success)
*	Modify			:
*	warning			:
*******************************************************************************/
static int __init raptor3_module_init(void)
{
	unsigned char ch;
	unsigned char port;
	int ret = 0;
	int chip;
	int i;
	unsigned char val_9x44;

	ret = misc_register(&raptor3_dev);
   	if (ret)
	{
		printk("ERROR: could not register raptor3_dev devices:%#x \n",ret);
        return -1;
	}
	
   	printk("RAPTOR3(B/C) EXT Driver %s COMPILE TIME[%s %s]\n", DRIVER_VER, __DATE__,__TIME__);
	
#ifdef __FOR_DEMO_171024
	printk("RAPTOR3 Special Driver : For MOBIS DEMO\n");
#elif __FOR_HIK_DEMO_180208
	printk("RAPTOR3 Special Driver : For HIKVISION DEMO\n");
#elif __FOR_IDIS_TVI_2M
	printk("RAPTOR3 Special Driver : For IDIS DEMO\n");
#else
	printk("RAPTOR3 Normal Driver\n");

#endif

    i2c_client_init();

	for(chip=0;chip<4;chip++)
	{
		chip_id[chip] = check_id(raptor3_i2c_addr[chip]);
		rev_id[chip]  = check_rev(raptor3_i2c_addr[chip]);
		if( (chip_id[chip] != RAPTOR3_4PORT_R0_ID )  	&&
			(chip_id[chip] != RAPTOR3_2PORT_R0_ID) 		&&
			(chip_id[chip] != RAPTOR3_1PORT_R0_ID)		&&
			(chip_id[chip] != AFE_NVP6134E_R0_ID)
		)
		{
			printk("Device ID Error... %x, Chip Count:[%d]\n", chip_id[chip], chip);
			raptor3_i2c_addr[chip] = 0xFF;
			chip_id[chip] = 0xFF;
		}
		else
		{
			printk("Device (0x%x) ID OK... %x , Chip Count:[%d]\n", raptor3_i2c_addr[chip], chip_id[chip], chip);
			printk("Device (0x%x) REV %x\n", raptor3_i2c_addr[chip], rev_id[chip]);
			raptor3_i2c_addr[raptor3_cnt] = raptor3_i2c_addr[chip];

			if(raptor3_cnt<chip)
			{
				raptor3_i2c_addr[chip] = 0xFF;
			}

			chip_id[raptor3_cnt] = chip_id[chip];
			rev_id[raptor3_cnt]  = rev_id[chip];

			raptor3_cnt++;
		}

		if((chip == 3) && (raptor3_cnt < chip))
		{
			for(i = raptor3_cnt; i < 4; i++)
			{
				chip_id[i] = 0xff;
				rev_id[i]  = 0xff;
			}
		}
	}

	printk("Chip Count = %d\n", raptor3_cnt);
	printk("Address [0x%x][0x%x][0x%x][0x%x]\n",raptor3_i2c_addr[0],raptor3_i2c_addr[1],raptor3_i2c_addr[2],raptor3_i2c_addr[3]);
	printk("Chip Id [0x%x][0x%x][0x%x][0x%x]\n",chip_id[0],chip_id[1],chip_id[2],chip_id[3]);
	printk("Rev Id [0x%x][0x%x][0x%x][0x%x]\n",rev_id[0],rev_id[1],rev_id[2],rev_id[3]);

	for( i = 0; i < 4; i++ )
	{
		decoder_inform.chip_id[i] = chip_id[i];
		decoder_inform.chip_rev[i] = rev_id[i];
		decoder_inform.chip_addr[i] = raptor3_i2c_addr[i];
	}
	decoder_inform.Total_Chip_Cnt = raptor3_cnt;

    /* initialize semaphore */
	sema_init(&raptor3_lock, 1);

    down(&raptor3_lock);

    for( i = 0; i < raptor3_cnt; i++ )
    {
    	/* set black screen */
    	video_output_colorbar_set(i);

    	/* set initialization value  */
    	for(ch = 0; ch < 4; ch++)
    	{
			gpio_i2c_write(raptor3_i2c_addr[i], 0xFF, 0x05+ch);
			gpio_i2c_write(raptor3_i2c_addr[i], 0xD5, 0x80);
			gpio_i2c_write(raptor3_i2c_addr[i], 0x00, 0xD0); //clamp speed
			gpio_i2c_write(raptor3_i2c_addr[i], 0x76, 0x00);
    	}

    	/* set NOVIDEO */
    	for(ch = 0; ch < 4; ch++)
    	{
    		gpio_i2c_write(raptor3_i2c_addr[i], 0xFF, 0x09);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x80 + (ch * 0x20), 0x00);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x81 + (ch * 0x20), 0x00);

    		gpio_i2c_write(raptor3_i2c_addr[i], 0xFF, 0x05 + ch);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x2C, 0x00);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0xB8, 0xB8);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0xB9, 0xB2);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x25, 0xDC);


    		gpio_i2c_write(raptor3_i2c_addr[i], 0xFF, 0x09);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x50 + (ch*4) , 0x30);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x51 + (ch*4) , 0x6F);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x52 + (ch*4) , 0x67);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x53 + (ch*4) , 0x48);

    		val_9x44 = gpio_i2c_read(raptor3_i2c_addr[i], 0x44);
    		val_9x44 &= ~(1 << ch);
    		val_9x44 |= (1 << ch);
    		gpio_i2c_write(raptor3_i2c_addr[i], 0x44 , val_9x44);
    	}

    	if(chip_id[i] == RAPTOR3_4PORT_R0_ID)
    	{
			gpio_i2c_write(raptor3_i2c_addr[i], 0xff, 0x01);
			/* set Port setting */
			for(port = 0; port < 4; port++)
			{
				/* port channel sequence set */
				gpio_i2c_write(raptor3_i2c_addr[i], 0xc0 + (port * 2), (port | (port << 4)));
				gpio_i2c_write(raptor3_i2c_addr[i], 0xc1 + (port * 2), (port | (port << 4)));
			}

			/* port 1mux set */
			gpio_i2c_write(raptor3_i2c_addr[i], 0xc8, 0x00);
			gpio_i2c_write(raptor3_i2c_addr[i], 0xc9, 0x00);

			/* all port enable */
			gpio_i2c_write(raptor3_i2c_addr[i], 0xca, 0xff);

			/* mux chid set */
			gpio_i2c_write(raptor3_i2c_addr[i], 0xff, 0x00);
			gpio_i2c_write(raptor3_i2c_addr[i], 0x55, 0x10);
			gpio_i2c_write(raptor3_i2c_addr[i], 0x56, 0x32);
    	}
    	else if (chip_id[i] == RAPTOR3_2PORT_R0_ID)
    	{
			gpio_i2c_write(raptor3_i2c_addr[i], 0xff, 0x01);
			/* set Port setting */
			for(port = 1; port < 3; port++)
			{
				/* port channel sequence set */
#ifdef	SUPPORT_3520D_6158C
				if(port == 1)
				{
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc0 + ((port+1) * 2), 0x10);
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc1 + ((port+1) * 2), 0x10);
				}
				else if(port == 2)
				{
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc0 + ((port-1) * 2), 0x32);
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc1 + ((port-1) * 2), 0x32);
				}

#else
				if(port == 1)
				{
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc0 + (port * 2), 0x10);
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc1 + (port * 2), 0x10);
				}
				else if(port == 2)
				{
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc0 + (port * 2), 0x32);
					gpio_i2c_write(raptor3_i2c_addr[i], 0xc1 + (port * 2), 0x32);
				}
#endif
			}
			/* port 1mux set */
			gpio_i2c_write(raptor3_i2c_addr[i], 0xc8, 0x20);
			gpio_i2c_write(raptor3_i2c_addr[i], 0xc9, 0x02);

			/* all port enable */
			gpio_i2c_write(raptor3_i2c_addr[i], 0xca, 0x66);

			/* mux chid set */
			gpio_i2c_write(raptor3_i2c_addr[i], 0xff, 0x00);
			gpio_i2c_write(raptor3_i2c_addr[i], 0x55, 0x10);
			gpio_i2c_write(raptor3_i2c_addr[i], 0x56, 0x10);
    	}

    	 // for image enhancement 3M RT upper format when cable distance 300M over
    	for(ch = 0; ch < 4; ch++)
    	{
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0xff, 0x05 + ch );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x50, 0xc6 );

    		 gpio_i2c_write(raptor3_i2c_addr[i], 0xff, 0x0a + (ch / 2));

    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x00 + ( 0x80 * (ch % 2)), 0x80 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x01 + ( 0x80 * (ch % 2)), 0x02 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x02 + ( 0x80 * (ch % 2)), 0x04 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x03 + ( 0x80 * (ch % 2)), 0x80 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x04 + ( 0x80 * (ch % 2)), 0x06 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x05 + ( 0x80 * (ch % 2)), 0x07 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x06 + ( 0x80 * (ch % 2)), 0x80 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x07 + ( 0x80 * (ch % 2)), 0x07 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x08 + ( 0x80 * (ch % 2)), 0x03 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x09 + ( 0x80 * (ch % 2)), 0x08 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x0a + ( 0x80 * (ch % 2)), 0x04 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x0b + ( 0x80 * (ch % 2)), 0x10 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x0c + ( 0x80 * (ch % 2)), 0x08 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x0d + ( 0x80 * (ch % 2)), 0x1f );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x0e + ( 0x80 * (ch % 2)), 0x2e );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x0f + ( 0x80 * (ch % 2)), 0x08 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x10 + ( 0x80 * (ch % 2)), 0x38 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x11 + ( 0x80 * (ch % 2)), 0x35 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x12 + ( 0x80 * (ch % 2)), 0x00 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x13 + ( 0x80 * (ch % 2)), 0x20 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x14 + ( 0x80 * (ch % 2)), 0x0d );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x15 + ( 0x80 * (ch % 2)), 0x80 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x16 + ( 0x80 * (ch % 2)), 0x54 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x17 + ( 0x80 * (ch % 2)), 0xb1 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x18 + ( 0x80 * (ch % 2)), 0x91 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x19 + ( 0x80 * (ch % 2)), 0x1c );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x1a + ( 0x80 * (ch % 2)), 0x87 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x1b + ( 0x80 * (ch % 2)), 0x92 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x1c + ( 0x80 * (ch % 2)), 0xe2 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x1d + ( 0x80 * (ch % 2)), 0x20 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x1e + ( 0x80 * (ch % 2)), 0xd0 );
    		 gpio_i2c_write(raptor3_i2c_addr[i], 0x1f + ( 0x80 * (ch % 2)), 0xcc );
         }
    }




  	up(&raptor3_lock);

	return 0;
}

/*******************************************************************************
*	Description		: It is called when "rmmod nvp61XX_ex.ko" command run
*	Argurments		: void
*	Return value	: void
*	Modify			:
*	warning			:
*******************************************************************************/
static void __exit raptor3_module_exit(void)
{
	misc_deregister(&raptor3_dev);
    i2c_client_exit();

    printk("RAPTOR3 DEVICE DRIVER UNLOAD SUCCESS");
}

module_init(raptor3_module_init);
module_exit(raptor3_module_exit);

MODULE_LICENSE("GPL");

/*******************************************************************************
*	End of file
*******************************************************************************/
