/********************************************************************************
 *
 *  Copyright (C) 2018 	NEXTCHIP Inc. All rights reserved.
 *  Module		: raptor4_drv.c
 *  Description	:
 *  Author		:
 *  Date        :
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
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "raptor4_common.h"
#include "raptor4_video.h"
#include "raptor4_function.h"

/* ----------------------------------------------------------------------------------
 * 2. Define ------------------------------------------------------------------------
 * --------------------------------------------------------------------------------*/
#define DRIVER_VERSION_MAJOR	1
#define DRIVER_VERSION_MINOR1	0
#define DRIVER_VERSION_MINOR2	5
#define RELEASE_YEAR			19
#define RELEASE_MONTH			4
#define RELEASE_DAY				9

#define I2C_0       (0)
#define I2C_1       (1)
#define I2C_2       (2)
#define I2C_3       (3)

/* ----------------------------------------------------------------------------------
 * 3. Define variable ---------------------------------------------------------------
 * --------------------------------------------------------------------------------*/
static unsigned int s_nc_drv_chip_cnt = 0;
static unsigned int s_nc_drv_channel_cnt = 0;

NC_U8 g_nc_drv_chip_id[4]   = { 0xFF, 0xFF, 0xFF, 0xFF };
NC_U8 g_nc_drv_chip_rev[4]  = { 0xFF, 0xFF, 0xFF, 0xFF };
NC_U8 g_nc_drv_i2c_addr[4]  = { 0xFF, 0xFF, 0xFF, 0xFF };

struct semaphore  nc_drv_lock;
struct i2c_client *nc_i2c_client;

static struct i2c_board_info hi_info =
{
	I2C_BOARD_INFO("raptor4", 0x60),
};

/* ----------------------------------------------------------------------------------
 * 4. External variable & function --------------------------------------------------
 * --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
 * 5. Function prototype ------------------------------------------------------------
 * --------------------------------------------------------------------------------*/
void nc_drv_version_information_print(void);
int  nc_drv_chip_infomation_get( void );
void nc_drv_decoder_initialize(void);

int  nc_decoder_open(struct inode * inode, struct file * file);
int  nc_decoder_close(struct inode * inode, struct file * file);
long nc_decoder_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

void __I2CWriteByte8(unsigned char chip_addr, unsigned char reg_addr, unsigned char value);
unsigned char __I2CReadByte8(unsigned char chip_addr, unsigned char reg_addr);

void __I2CWriteByte8(unsigned char chip_addr, unsigned char reg_addr, unsigned char value)
{
	int ret;
	unsigned char buf[2];
	struct i2c_client* client = nc_i2c_client;

	nc_i2c_client->addr = chip_addr;

	buf[0] = reg_addr;
	buf[1] = value;

	ret = i2c_master_send(client, buf, 2);
	udelay(300);
	//return ret;
}

unsigned char __I2CReadByte8(unsigned char chip_addr, unsigned char reg_addr)
{
	int ret_data = 0xFF;
	int ret;
	struct i2c_client* client = nc_i2c_client;
	unsigned char buf[2];

	nc_i2c_client->addr = chip_addr;

	buf[0] = reg_addr;
	ret = i2c_master_recv(client, buf, 1);
	if (ret >= 0)
	{
		ret_data = buf[0];
	}
	return ret_data;
}

/*******************************************************************************
 * Print Driver Version
 *******************************************************************************/
void nc_drv_version_information_print(void)
{
	char szAppVersion[32];

	memset( &szAppVersion[0], 0, sizeof(szAppVersion));
	sprintf(szAppVersion, "%d.%d.%d/%02d%02d%02d",
			DRIVER_VERSION_MAJOR, DRIVER_VERSION_MINOR1, DRIVER_VERSION_MINOR2, RELEASE_YEAR, RELEASE_MONTH, RELEASE_DAY);

	printk("****************  RAPTOR4 Device Driver Version Information  ****************\n");
	printk("    Driver. VERSION       = %s\n", szAppVersion);
	printk("    COMPILED              = %s %s\n", __TIME__, __DATE__);
	printk("*****************************************************************************\n");

}

int nc_drv_chip_infomation_get( void )
{
	int ret = -1;
	int chip;
	NC_U8 I2C_ACK = 0;
	NC_U8 NC_Decoder_i2c_addr[4] = { 0x60, 0x62, 0x64, 0x66 };

	/**********************************************************************************
	 * I2C Communication(Receive) Check
	 * 0 == 0x60, 1 == 0x62, 2 == 0x64, 3 == 0x66
	 * CheckVal == Success 0x00, Fail 0xFF
	 **********************************************************************************/
	for(chip=0; chip<4; chip++)
	{
		gpio_i2c_write(NC_Decoder_i2c_addr[chip], 0xFF, 0x00);
		I2C_ACK = gpio_i2c_read(NC_Decoder_i2c_addr[chip], 0xFF);
		if( I2C_ACK == 0 )
		{
			g_nc_drv_i2c_addr[s_nc_drv_chip_cnt] = NC_Decoder_i2c_addr[chip];
			s_nc_drv_chip_cnt++;
			printk("<<<<<<<<<<< [ Success::0x%02X ] Decoder I2C Communication!!!! >>>>>>>>>>>\n", NC_Decoder_i2c_addr[chip]);
		}
		else
			printk(">>>>>>>>>>> [ Fail::0x%02X ] Decoder I2C Communication!!!! <<<<<<<<<<<\n", NC_Decoder_i2c_addr[chip]);
	}

	if( s_nc_drv_chip_cnt <= 0 )
	{
		printk("Decoder Driver I2C(0x60, 0x62, 0x64, 0x66) Communication Error!!!!\n");
		return ret;
	}
	else
	{
		s_nc_drv_channel_cnt = s_nc_drv_chip_cnt * 4;  // Each Chip 4 Channel Input
	}

	/**********************************************************************************
	 * 1. Decoder ID Check
	 * 2. Decoder Revision Check
	 **********************************************************************************/
	for(chip=0; chip<s_nc_drv_chip_cnt; chip++)
	{
		/* Decoder Device ID Check */
		gpio_i2c_write(g_nc_drv_i2c_addr[chip], 0xFF, BANK_0);
		g_nc_drv_chip_id[chip] = gpio_i2c_read(g_nc_drv_i2c_addr[chip], 0xF4);

		/* Decoder Device Revision Check */
		gpio_i2c_write(g_nc_drv_i2c_addr[chip], 0xFF, BANK_0);
		g_nc_drv_chip_rev[chip] = gpio_i2c_read(g_nc_drv_i2c_addr[chip], 0xF5);
	}

	ret = 0;
	printk("********************** Decoder Chip Information *********************\n");
	printk("Decoder Chip Count = %d\n", s_nc_drv_chip_cnt);
	printk("SlaveAddress    [0x%02X], [0x%02X], [0x%02X], [0x%02X]\n",g_nc_drv_i2c_addr[0],g_nc_drv_i2c_addr[1],g_nc_drv_i2c_addr[2],g_nc_drv_i2c_addr[3]);
	printk("DecoderID       [0x%02X], [0x%02X], [0x%02X], [0x%02X]\n",g_nc_drv_chip_id[0],g_nc_drv_chip_id[1],g_nc_drv_chip_id[2],g_nc_drv_chip_id[3]);
	printk("DecoderRevision [0x%02X], [0x%02X], [0x%02X], [0x%02X]\n",g_nc_drv_chip_rev[0],g_nc_drv_chip_rev[1],g_nc_drv_chip_rev[2],g_nc_drv_chip_rev[3]);
	printk("**********************************************************************\n");

	return ret;
}

#if 1
void nc_drv_chip_infomation_to_app( nc_decoder_s *psVdtDevInfo )
{
	int ii = 0;

	psVdtDevInfo->Total_Chip_Cnt = s_nc_drv_chip_cnt;

	for(ii=0; ii<4; ii++)
	{
		psVdtDevInfo->chip_id[ii]   = g_nc_drv_chip_id[ii];
		psVdtDevInfo->chip_rev[ii]  = g_nc_drv_chip_rev[ii];
		psVdtDevInfo->chip_addr[ii] = g_nc_drv_i2c_addr[ii];
	}
}
#endif

void nc_drv_decoder_init_test_view_set(NC_U8 dev, NC_U8 addr, NC_U8 val)
{
	gpio_i2c_write(dev, addr, val);

	if(addr == 0xff)
		printk("BANK >>>>>>>>> 0x%02X\n", val);
	else
		printk("0x%02X : 0x%02X\n", addr, val);
}


void nc_drv_decoder_init_set(void)
{
	int dev = 0;
	int chn = 0;

	for(dev=0; dev<s_nc_drv_chip_cnt; dev++)
	{
		printk("[%s]SlaveAddr::0x%02X Initialize!!\n", __func__, g_nc_drv_i2c_addr[dev]);

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x97, 0x00);	// CH_RST ON
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x97, 0x0f);	// CH_RST OFF
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7a, 0x0f);	// Clock Auto ON
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xca, 0xff);	// VCLK_EN, VDO_EN

		for(chn=0; chn<4; chn++)
		{
			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_5 + chn);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x00, 0xd0); // Raptor3 Set Value

			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x05, 0x04);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x08, 0x55);	// Slice Level
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x47, 0xEE);	// CONTROL_MODES
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x59, 0x00);	// Raptor3 Set Value
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x76, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x77, 0x80);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x78, 0x00);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x79, 0x11);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xB8, 0xB8);	// H_PLL_BYPASS
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7B, 0x11);	// v_rst_on
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xb9, 0x72);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xB8, 0xB8); // No Video Set

			NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x00+chn, 0x10);			// decoder rst
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x22+(chn*0x04), 0x0b);	// Raptor3 Set Value
			gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x23+(chn*0x04), 0x41);
		}

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_13);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x12, 0x04);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2E, 0x10);	// VFC Check Count
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x30, 0x7f);	// Auto 0x7F, Manual 0x00 - NOVIDEO_VFC_INIT_OP[4], NOVIDEO_VFC_INIT_EN[3:0]
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3a, 0xff);	// Auto 0xFF, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3b, 0xff);	// Auto 0xFF, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3c, 0xff);	// Auto 0xFF, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3d, 0xff);	// Auto 0xFF, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3e, 0xff);	// Auto 0xFF, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x3f, 0x0f);	// Auto 0xFF, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x70, 0xf0);	// Auto 0x30\f0, Manual 0x00
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x72, 0x05);	// 0x10, 0x08, 0x05
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x7A, 0xf0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x07, 0x47);

		/* SAM Range */
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x74, 0x00);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x76, 0x00);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x78, 0x00);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x75, 0xff);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x77, 0xff);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x79, 0xff);

		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x01, 0x0c);	// Novideo_keep_en
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x2f, 0xc8);	// NOVIDEO_KEEP_TIME ( Register * 10ms)....Default 0x32

		// EQ Stage Get
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x73, 0x23); // 0x23 3frame :: 0x91

		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_9);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x96, 0x03);

		/********************************************************
		 * Audio Default Setting
		 ********************************************************/
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x05, 0x09);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x07, 0x88);	// Master Mode, 16Khz, 16bit_Width
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x22, 0x08);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x44, 0x00);

		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x00, 0x02);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x06, 0x1B);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x31, 0x0A);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x32, 0x00);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x46, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x47, 0x01);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x48, 0xD0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x49, 0x88);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x58, 0x02);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x59, 0x00);

		/********************************************************
		 * VI Channel ID
		 ********************************************************/
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_0);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x55, 0x10);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0x56, 0x10);


		/********************************************************
		 * MPP_TST_SEL
		 * Default : BT656 + ACP
		 ********************************************************/
#if 1
		// MPP Coaxial mode select Ch1~4
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xA8, 0x08);  // MPP_TST_SEL1
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xA9, 0x09);  // MPP_TST_SEL2
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xAA, 0x0A);  // MPP_TST_SEL3
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xAB, 0x0B);  // MPP_TST_SEL4
#else
		// MPP BT601 mode select Ch1~4
		NC_DEVICE_DRIVER_BANK_SET(dev, BANK_1);
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xA8, 0x10);  // MPP_TST_SEL1
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xA9, 0x20);  // MPP_TST_SEL2
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xAA, 0x30);  // MPP_TST_SEL3
		gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xAB, 0x40);  // MPP_TST_SEL4
#endif
	}

}

void nc_drv_decoder_initialize(void)
{
	/**********************************************************************
	 * 1. I2C Communication Check
	 * 2. Decoder Chip ID Get
	 * 3. Decoder Chip Revision Get
	 **********************************************************************/
	nc_drv_chip_infomation_get();

	/**********************************************************************
	 * Device Driver Source Code Version
	 **********************************************************************/
	nc_drv_version_information_print();

	/**********************************************************************
	 * Raptor4 Device Driver Information Initialize
	 **********************************************************************/
	nc_drv_common_info_chip_data_init_set(s_nc_drv_chip_cnt, g_nc_drv_chip_id, g_nc_drv_chip_rev, g_nc_drv_i2c_addr);

	/**********************************************************************
	 * Raptor4 Device Initialize
	 **********************************************************************/
	nc_drv_decoder_init_set();
}

/*******************************************************************************************************
 * Initialize Function
 *
 *******************************************************************************************************/
int nc_decoder_open(struct inode * inode, struct file * file)
{
	printk("[DRV] Raptor4 Driver Open\n");

	return 0;
} 

int nc_decoder_close(struct inode * inode, struct file * file)
{
	printk("[DRV] Raptor4 Driver Close\n");
	return 0;
}

long nc_decoder_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	NC_S32 err = 0;
//	NC_S32 cpy2usr_ret;
	NC_U32 __user *argp = (unsigned int __user *)arg;

	nc_decoder_s stNC_DRV;

	down(&nc_drv_lock);

	if( (err = copy_from_user(&stNC_DRV, argp, sizeof(nc_decoder_s))) < 0 )
	{
		printk("copy_from_user error!!\n");
		up(&nc_drv_lock);
		return err;
	}

	switch (cmd)
	{
		/*===============================================================================================
		* Decoder Chip Information Get
		*===============================================================================================*/
		case IOC_VDEC_CHIP_INFO_GET:		nc_drv_chip_infomation_to_app(&stNC_DRV);		break;

		/*===============================================================================================
		* VIDEO
		*===============================================================================================*/
		case IOC_VDEC_VIDEO_INPUT_INFO_GET:			nc_drv_video_setting_info_get(&stNC_DRV);			break;
		case IOC_VDEC_VIDEO_INPUT_VFC_GET:			nc_drv_video_input_vfc_status_get(&stNC_DRV);		break;
		case IOC_VDEC_VIDEO_INPUT_SET:				nc_drv_video_input_set(&stNC_DRV);					break;
		case IOC_VDEC_VIDEO_INPUT_EQ_STAGE_GET:		nc_drv_video_input_eq_stage_get(&stNC_DRV);			break;
		case IOC_VDEC_VIDEO_INPUT_EQ_STAGE_SET:		nc_drv_video_input_eq_stage_set(&stNC_DRV);			break;
		case IOC_VDEC_VIDEO_OUTPUT_SET:				nc_drv_video_output_port_set(&stNC_DRV);			break;
		case IOC_VDEC_VIDEO_OUTPUT_MANUAL_SET:		nc_drv_video_output_port_manual_set(&stNC_DRV);		break;
		case IOC_VDEC_VIDEO_OUTPUT_PATTERN_SET:		nc_drv_video_output_color_pattern_set(&stNC_DRV);	break;
		case IOC_VDEC_VIDEO_AUTO_MANUAL_MODE_SET:	nc_drv_video_auto_manual_mode_set(&stNC_DRV);		break;
		case IOC_VDEC_VIDEO_INPUT_MANUAL_SET:		nc_drv_video_input_manual_set(&stNC_DRV);			break;
		case IOC_VDEC_VIDEO_COLOR_TUNE_GET:			nc_drv_video_color_tune_get(&stNC_DRV);				break;
		case IOC_VDEC_VIDEO_COLOR_TUNE_SET:			nc_drv_video_color_tune_set(&stNC_DRV);				break;

		/*===============================================================================================
		* Coaxial Protocol
		*===============================================================================================*/
		case IOC_VDEC_COAX_INIT_SET:			nc_drv_coax_initialize_set(&stNC_DRV);									break;
		case IOC_VDEC_COAX_INIT_MANUAL_SET:		nc_drv_coax_initialize_manual_set(&stNC_DRV);							break;
		case IOC_VDEC_COAX_UP_COMMAMD_SET:		nc_drv_coax_up_stream_command_set(&stNC_DRV);							break;
		case IOC_VDEC_COAX_DOWN_DATA_GET:		nc_drv_coax_down_stream_data_get(&stNC_DRV);							break;
		case IOC_VDEC_COAX_FW_ACP_HEADER_GET:	nc_drv_coax_fwseq_1_ready_header_check_from_isp_recv(&stNC_DRV);		break;
		case IOC_VDEC_COAX_FW_READY_CMD_SET:	nc_drv_coax_fwseq_2_1_ready_cmd_to_isp_send(&stNC_DRV);					break;
		case IOC_VDEC_COAX_FW_READY_ACK_GET:	nc_drv_coax_fwseq_2_2_ready_cmd_ack_from_isp_recv(&stNC_DRV);			break;
		case IOC_VDEC_COAX_FW_START_CMD_SET:	nc_drv_coax_fwseq_3_1_start_cmd_to_isp_send(&stNC_DRV);					break;
		case IOC_VDEC_COAX_FW_START_ACK_GET:	nc_drv_coax_fwseq_3_2_start_cmd_ack_from_isp_recv(&stNC_DRV);			break;
		case IOC_VDEC_COAX_FW_SEND_DATA_SET:	nc_drv_coax_fwseq_4_1_one_packet_data_to_isp_send(&stNC_DRV);			break;
		case IOC_VDEC_COAX_FW_SEND_ACK_GET:		nc_drv_coax_fwseq_4_2_one_packet_data_ack_from_isp_recv(&stNC_DRV);		break;
		case IOC_VDEC_COAX_FW_END_CMD_SET:		nc_drv_coax_fwseq_5_1_end_cmd_to_isp_send(&stNC_DRV);					break;
		case IOC_VDEC_COAX_FW_END_ACK_GET:		nc_drv_coax_fwseq_5_2_end_cmd_ack_from_isp_recv(&stNC_DRV);				break;
		case IOC_VDEC_COAX_TEST_ISP_DATA_SET:	nc_drv_coax_test_isp_data_set(&stNC_DRV);								break;

		/*===============================================================================================
		* AUDIO
		*===============================================================================================*/
		case IOC_VDEC_AUDIO_INIT_SET:				nc_drv_audio_init_set(&stNC_DRV);			break;
		case IOC_VDEC_AUDIO_VIDEO_AOC_FORMAT_SET:	nc_drv_audio_video_format_set(&stNC_DRV);	break;

		/*===============================================================================================
		* MOTION
		*===============================================================================================*/
		case IOC_VDEC_MOTION_ON_OFF_SET:		nc_drv_motion_onoff_set(&stNC_DRV);					break;
		case IOC_VDEC_MOTION_BLOCK_SET:			nc_drv_motion_each_block_onoff_set(&stNC_DRV);		break;
		case IOC_VDEC_MOTION_BLOCK_GET:			nc_drv_motion_each_block_onoff_get(&stNC_DRV);		break;
		case IOC_VDEC_MOTION_TSEN_SET:			nc_drv_motion_motion_tsen_set(&stNC_DRV);			break;
		case IOC_VDEC_MOTION_PSEN_SET:			nc_drv_motion_motion_psen_set(&stNC_DRV);			break;
		case IOC_VDEC_MOTION_ALL_BLOCK_SET:		nc_drv_motion_all_block_onoff_set(&stNC_DRV);		break;
		case IOC_VDEC_MOTION_DETECTION_GET:		nc_drv_motion_detection_info_get(&stNC_DRV);		break;

		/*===============================================================================================
		* ETC
		*===============================================================================================*/
		case IOC_VDEC_REG_BANK_DUMP_GET:		nc_drv_common_bank_data_get(&stNC_DRV);			break;
		case IOC_VDEC_REG_DATA_SET:				nc_drv_common_register_data_set(&stNC_DRV);		break;
		case IOC_VDEC_REG_DATA_GET:				nc_drv_common_register_data_get(&stNC_DRV);		break;
		default:	printk("[%s::%d]Decoder Device Driver Unknown ioctl!!!0x%x\n", __FILE__, __LINE__, cmd);

	}

	if( (err = copy_to_user(argp, &stNC_DRV, sizeof(nc_decoder_s))) < 0 )
	{
		printk("%d::copy_to_user error!!\n", cmd);
		up(&nc_drv_lock);
		return err;
	}

	up(&nc_drv_lock);

	return 0;
}

static int i2c_client_init(void)
{
	struct i2c_adapter* i2c_adap;

	printk("[DRV] I2C Client Init \n");
	i2c_adap = i2c_get_adapter(I2C_1);
	nc_i2c_client = i2c_new_device(i2c_adap, &hi_info);
	i2c_put_adapter(i2c_adap);

	return 0;
}

static void i2c_client_exit(void)
{
	i2c_unregister_device(nc_i2c_client);
}

static struct file_operations raptor4_fops = {
	.owner      = THIS_MODULE,
	.unlocked_ioctl	= nc_decoder_ioctl,
	.open           = nc_decoder_open,
//	.probe =
	.release        = nc_decoder_close
};

static struct miscdevice raptor4_dev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "raptor4",
	.fops  		= &raptor4_fops,
};

/*******************************************************************************
 *	Description		: It is called when "insmod raptor4.ko" command run
 *	Argurments		: void
 *	Return value	: -1(could not register jaguar1 device), 0(success)
 *	Modify			:
 *	warning			:
 *******************************************************************************/
static int __init raptor4_module_init(void)
{
	int ret = 0;

	ret = misc_register(&raptor4_dev);

	if (ret)
	{
		printk("ERROR: could not register raptor4_dev devices:%#x \n",ret);
		return -1;
	}

	printk("================================= raptor4_module_init_start =================================================\n");
	i2c_client_init();

	/* initialize semaphore */
	sema_init(&nc_drv_lock, 1);
	down(&nc_drv_lock);
	/* Decoder Initialize */
	nc_drv_decoder_initialize();
	up(&nc_drv_lock);
	printk("================================= raptor4_module_init_end ===================================================\n");

	return 0;
}

/*******************************************************************************
 *	Description		: It is called when "rmmod nvp61XX_ex.ko" command run
 *	Argurments		: void
 *	Return value	: void
 *	Modify			:
 *	warning			:
 *******************************************************************************/
static void __exit raptor4_module_exit(void)
{
	misc_deregister(&raptor4_dev);
	i2c_client_exit();

	printk("RAPTOR4 DEVICE DRIVER UNLOAD SUCCESS\n");
}

module_init(raptor4_module_init);
module_exit(raptor4_module_exit);

MODULE_LICENSE("GPL");

/*******************************************************************************
 *	End of file
 *******************************************************************************/
