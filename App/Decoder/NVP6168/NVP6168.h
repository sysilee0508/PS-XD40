#ifndef __NVP6168_H__
#define __NVP6168_H__

#include "raptor4_fmt.h"

typedef enum  
{
	VPORT_MAP0,		// 1, 2, 3, 4	// full ch1,  all 4 split, PIP with ch3
	VPORT_MAP1,		// 1, 3, 2, 4	// 2splits,
	VPORT_MAP2,		// 1, 4, 2, 3	// PIP with ch4
	VPORT_MAP3,		// 1, 2, 2, 3	// PIP with ch2
	VPORT_MAP_MAX
} eVPORT_MAP_t;

#define PLATFORM_LINUX					0
#define NVP6168_I2C_ADDR				0x60
#define NVP_MULTI_CHIP					0
#define COAXIAL_PROTOCOL				0
#define AUDIO_BLOCK						0

#define TOTAL_CHIP_CNT					1
#define TOTAL_CHANNEL_CNT				4 * TOTAL_CHIP_CNT


void NVP6168_Init(void);
void NVP6168_AutoDetection_Proc(void);
void NVP6168_VO_Port_Set(eVPORT_MAP_t vo_port);
//void NVP6168_OutPort_Set(NC_U8 dev, NC_U8 chn, NC_VIVO_CH_FORMATDEF_E fmt);
NC_VIVO_CH_FORMATDEF_E NVP6168_Current_VideoFormat_Get(NC_U8 ch);
#endif
