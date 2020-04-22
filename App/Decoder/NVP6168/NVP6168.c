/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "NVP6168.h"
#include "delay.h"

#include "raptor4_common.h"
#include "raptor4_fmt.h"

const NC_CH_E NVP_Ch[4] =
{
	NC_CH4,
	NC_CH3,
	NC_CH2,
	NC_CH1
};

static eVPORT_MAP_t VO_PortMap = VPORT_MAP0;
static NC_U8 VO_PortMapChanged = 0;

NC_CH_E VO_Port[VPORT_MAP_MAX][4] = 
{
	{NC_PORT_D, NC_PORT_C, NC_PORT_B, NC_PORT_A},	 // full ch1, 2 split , 4 split(except quad), PIP with ch2
	{NC_PORT_D, NC_PORT_B, NC_PORT_C, NC_PORT_A},	 // PIP with ch3, quad
	{NC_PORT_B, NC_PORT_C, NC_PORT_D, NC_PORT_A}	// PIP with ch4
};


void NVP6168_ManualDetectionSet(void);
void NVP6168_OutPort_Set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt);

void NVP6168_Init(void)
{
	nc_drv_decoder_initialize();
	DECODER_Video_Output_Set(0, NC_VO_WORK_MODE_1MUX);
	NVP6168_ManualDetectionSet();
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
		if((DECODER_Video_Input_Format_Detection_Get(ch, &VFC, &VideoFormat)) ||(VO_PortMapChanged == 1))
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
			else if(VO_PortMapChanged == 1)
			{
				NVP6168_OutPort_Set(0, ch, VideoFormat);
			}
		}
	}
	VO_PortMapChanged = 0;
}

void NVP6168_VO_Port_Set(eVPORT_MAP_t vo_map)
{
	if(VO_PortMap != vo_map)
	{
		VO_PortMap = vo_map;
		VO_PortMapChanged = 1;
	}
}

void NVP6168_OutPort_Set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt)
{
	NC_U8 seq;
	NC_U8 vo_clk;
	NC_PORT_E port;
	

	port = VO_Port[VO_PortMap][chn];

	seq = ((chn&0x0F) << 4) | (chn&0x0F);
	
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
	else
	{
		vo_clk = 0x58;
	}

       NC_DEVICE_DRIVER_BANK_SET(dev, 0x01);
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC0+(port*2), seq); 
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xC1+(port*2), seq); 
	
	gpio_i2c_write(g_nc_drv_i2c_addr[dev], 0xcc + port, vo_clk);

}

NC_VIVO_CH_FORMATDEF_E NVP6168_Current_VideoFormat_Get(NC_U8 ch)
{
	return nc_drv_common_info_video_fmt_def_get(NVP_Ch[ch]);
}

void NVP6168_ManualDetectionSet(void)
{
	NC_U8 ch;

	for(ch = 0; ch < TOTAL_CHANNEL_CNT; ch++)
	{
		DECODER_Video_Input_Manual_Set(ch, CABLE_3C2V, FMT_AHD, FMT_1080P, FMT_30P);
		DECODER_SDK_Video_Auto_Manual_Mode_Set(ch, NC_VIDEO_SET_MODE_AUTO);
	}
}

