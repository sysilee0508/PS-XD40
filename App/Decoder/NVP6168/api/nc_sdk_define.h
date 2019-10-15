/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: Decoder api header
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#ifndef _RAPTOR4_DEFINE_H_
#define _RAPTOR4_DEFINE_H_

#include "../raptor4_fmt.h"

//#define DEV_PATH_VDEC "/dev/raptor4"

#define VD_MOTION_GET_DETECT_INFO 		1
#define VD_MOTION_GET_BLOCK_ON_OFF_INFO 2

#define VD_MOTION_SET_ON_OFF			1
#define VD_MOTION_SET_ALL_BLOCK_ON_OFF  2
#define VD_MOTION_SET_EACH_BLOCK_ON_OFF 3
#define VD_MOTION_SET_TSEN 				4
#define VD_MOTION_SET_PSEN				5

#define VD_COAX_NORMAL_MODE 	1
#define VD_COAX_ACP_16BIT_MODE  2
#define VD_COAX_CCP_NEW_MODE  	3

#define	ONE_PACKET_MAX_SIZE 	139

//#define MOUNT_USB_NAME	"/mnt/usb"
//#define MOUNT_NFS_NAME	"/mnt/nfs"

#define COAX_FW_SUCCESS 0
#define COAX_FW_FAILURE -1

#define COAX_FW_TIMEOUT 60

#endif
