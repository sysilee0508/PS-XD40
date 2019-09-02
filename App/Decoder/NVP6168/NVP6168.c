/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "NVP6168.h"
#include "delay.h"

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

static NC_CH_E VO_Ch[VPORT_MAP_MAX][4] = 
{
	{NC_CH4, NC_CH3, NC_CH2, NC_CH1},	 // full ch1, 2 split , 4 split(except quad), PIP with ch2
	{NC_CH4, NC_CH2, NC_CH3, NC_CH1},	 // PIP with ch3, quad
	{NC_CH2, NC_CH4, NC_CH3, NC_CH1}	// PIP with ch4
};

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
		if(DECODER_Video_Input_Format_Detection_Get(ch, &VFC, &VideoFormat) )
		{
			if(( VideoFormat != NC_VIVO_CH_FORMATDEF_UNKNOWN ) ||(VO_PortMapChanged == 1))
			{
				/***********************************************************************
				 * Decoder Setting : Format Set -> Distance Check -> EQ Set
				 ***********************************************************************/
				DECODER_Video_Input_Auto_Sequence_Set(ch);
				EqStage = DECODER_Video_Input_EQ_Stage_Get(ch, &SamVal);
				DECODER_Video_Input_EQ_Stage_Set(ch, EqStage);
				VO_PortMapChanged = 0;
			}
		}
		//Delay_ms(500);
	}
}

void NVP6168_VO_Port_Set(eVPORT_MAP_t vo_port)
{
	nc_decoder_s video_info;
	NC_PORT_E port;


	if(VO_PortMap != vo_port)
	{
		for(port = NC_PORT_A; port < 4; port++)
		{
			video_info.VO_ChnSeq[port] = VO_Ch[vo_port][port];
		}
		nc_drv_video_output_port_seq_set(&video_info);

		VO_PortMap = vo_port;
		VO_PortMapChanged = 1;
	}
}

NC_VIVO_CH_FORMATDEF_E NVP6168_Current_VideoFormat_Get(NC_U8 ch)
{
	return nc_drv_common_info_video_fmt_def_get(NVP_Ch[ch]);
}

