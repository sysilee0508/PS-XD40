/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "NVP6168.h"
#include "delay.h"

#include "raptor4_common.h"
#include "raptor4_fmt.h"

void NVP6168_Init(void)
{
	NC_U8 ch;
	
	nc_drv_decoder_initialize();

	DECODER_Video_Output_Set(0, NC_VO_WORK_MODE_1MUX);
}

void NVP6168_AutoDetection_Proc(void)
{
	NC_U8 ch;
	nc_decoder_s video_status;
	NC_VIVO_CH_FORMATDEF_E VideoFormat = NC_VIVO_CH_FORMATDEF_UNKNOWN;
	NC_U8 VFC 	  = 0;
	NC_U8 EqStage = 0;
	NC_U32 SamVal = 0;

	for(ch = 0; ch < TOTAL_CHANNEL_CNT; ch++)
	{
		if(DECODER_Video_Input_Format_Detection_Get(ch, &VFC, &VideoFormat))
		{
			if( VideoFormat != NC_VIVO_CH_FORMATDEF_UNKNOWN ) 
			{
				/***********************************************************************
				 * Decoder Setting : Format Set -> Distance Check -> EQ Set
				 ***********************************************************************/
				DECODER_Video_Input_Auto_Sequence_Set(ch);
				EqStage = DECODER_Video_Input_EQ_Stage_Get(ch, &SamVal);
				DECODER_Video_Input_EQ_Stage_Set(ch, EqStage);
			}
			else if( VideoFormat == NC_VIVO_CH_FORMATDEF_UNKNOWN )
			{
				DECODER_Video_Output_NoVideo_Pattern_Set(ch, 0);
			}
		}
		//Delay_ms(500);
	}
}

void NVP6168_OutPort_Set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt)
{
	NC_U8 seq;
	NC_U8 vo_clk;
	
	seq = ((chn&0x0F) << 4) | (chn&0x0F);
/*	
	if(fmt == SD_H960_2EX_Btype_NT || \
		fmt == SD_H960_2EX_Btype_PAL || \
		fmt == AHD_720P_30P_EX_Btype || \
		fmt == AHD_720P_25P_EX_Btype || \
		fmt == TVI_HD_30P_EX || \
		fmt == TVI_HD_25P_EX || \
		fmt == TVI_HD_B_30P_EX || \
		fmt == TVI_HD_B_25P_EX || \
		fmt == CVI_HD_30P_EX || \
		fmt == CVI_HD_25P_EX
		)
	{
		seq |= 0x88;
		vo_clk = 0x00;
	}
	else */
	{
		vo_clk = 0x58;
	}

       NC_DEVICE_DRIVER_BANK_SET(dev, 0x01);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0+(chn*2), seq); 
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1+(chn*2), seq); 
	
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + chn, vo_clk);

}

NC_VIVO_CH_FORMATDEF_E NVP6168_Current_VideoFormat_Get(NC_U8 ch)
{
	return nc_drv_common_info_video_fmt_def_get(ch);
}

