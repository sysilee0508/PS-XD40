#ifndef __NVP6168_H__
#define __NVP6168_H__

#include "raptor4_fmt.h"

typedef enum  
{
	VPORT_MAP0,
	VPORT_MAP1,
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
NC_VIVO_CH_FORMATDEF_E NVP6168_Current_VideoFormat_Get(NC_U8 ch);
eVPORT_MAP_t NVP6168_Get_VportMap(void);
#endif
