/********************************************************************************
 *
 *  Copyright (C) 2018	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: Driver Interface
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/

/* 1. Header file include --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>

#include <termios.h>

#include "nc_sdk_define.h"
#include "nc_sdk_api_drv.h"


#if PLATFORM_LINUX == 1

int g_fd_raptor4_drv=-1;
nc_decoder_s g_Decoder_Chip_info;

static int OpenDeviceVdec( void )
{
	int fd = -1;

	fd = open(DEV_PATH_VDEC, O_RDONLY);

	if( fd < 0 )
	{
		printf("[%s::%d]VDEC OPEN ERROR!! : %s\n", __FILE__, __LINE__, DEV_PATH_VDEC);
		return fd;
	}

	printf("[%s]Raptor4 Driver Load Success!!::%s\n", __func__, DEV_PATH_VDEC);

	return fd;
}

static int CloseDeviceVdec( int fd )
{
	printf("[%s::%d]%s\n", __FILE__, __LINE__, DEV_PATH_VDEC);

	if(fd>=0)
	{
		close( fd );
		fd = -1;
	}

	return 0;
}

int NC_API_DRV_Driver_Init(void)
{
	g_fd_raptor4_drv = OpenDeviceVdec();

	return 0;
}

int NC_API_DRV_Driver_DeInit(void)
{
	CloseDeviceVdec( g_fd_raptor4_drv );
	return 0;
}

/******************************************************************************************
 * 1. Driver Chip Information Get
*******************************************************************************************/
void NC_API_DRV_InformationData_Get(nc_decoder_s *psDevInfo)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_CHIP_INFO_GET, psDevInfo );
}


/******************************************************************************************
 * 2. Video
*******************************************************************************************/
void NC_API_DRV_Video_Input_Set_Info_Get(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_INFO_GET, pstVD_Video );
}

void NC_API_DRV_Video_Input_VFC_Get(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_VFC_GET, pstVD_Video );
}

void NC_API_DRV_Video_Input_EQ_Stage_Get(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_EQ_STAGE_GET, pstVD_Video );
}

void NC_API_DRV_Video_Input_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_SET, pstVD_Video );
}

void NC_API_DRV_Video_Input_EQ_Stage_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_EQ_STAGE_SET, pstVD_Video );
}

void NC_API_DRV_Video_Input_format_define_get(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_MANUAL_SET, pstVD_Video );
}

void NC_API_DRV_Video_Input_Manual_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_MANUAL_SET, pstVD_Video );
}


void NC_API_DRV_Video_Output_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_SET, pstVD_Video );
}

void NC_API_DRV_Video_Output_Manual_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_MANUAL_SET, pstVD_Video );
}

void NC_API_DRV_Video_Output_Pattern_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_PATTERN_SET, pstVD_Video );
}


void NC_API_DRV_Video_AOC_Format_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_PATTERN_SET, pstVD_Video );
}

void NC_API_DRV_Video_Auto_Manual_Mode_Set(nc_decoder_s *pstVD_Video)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_AUTO_MANUAL_MODE_SET, pstVD_Video );
}


/******************************************************************************************
 * 3. Coaxial
*******************************************************************************************/
void NC_API_DRV_Coax_Initialize_Set(nc_decoder_s *pstCoax)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_INIT_SET, pstCoax );
}

void NC_API_DRV_Coax_Initialize_Manual_Set(nc_decoder_s *pstCoax)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_INIT_MANUAL_SET, pstCoax );
}

void NC_API_DRV_Coax_UP_Stream_Command_Set(nc_decoder_s *pstCoax)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_UP_COMMAMD_SET, pstCoax );
}

void NC_API_DRV_Coax_Down_Stream_Data_Get(nc_decoder_s *pstCoax)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_DOWN_DATA_GET, pstCoax );
}


void NC_API_DRV_Coax_FW_ACP_Header_Get( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_ACP_HEADER_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Ready_CMD_Set( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_READY_CMD_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Ready_ACK_Get( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_READY_ACK_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Start_CMD_Set( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_START_CMD_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Start_ACK_Get( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_START_ACK_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_OnePacketData_Set( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_SEND_DATA_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_OnePacketData_ACK_Get( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_SEND_ACK_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_End_CMD_Set( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_END_CMD_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_End_CMD_Get( nc_decoder_s *pstFileInfo )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_END_ACK_GET, pstFileInfo );
}


void NC_API_DRV_Coax_Test_ISP_Data_Set( nc_decoder_s *pstDecoder )
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_TEST_ISP_DATA_SET, pstDecoder );
}

/******************************************************************************************
 * 4. AOC
*******************************************************************************************/
void NC_API_DRV_Audio_Init_Set(nc_decoder_s *pstAudio)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_AUDIO_INIT_SET, pstAudio );

}


/******************************************************************************************
 * 5. Motion
*******************************************************************************************/
void NC_API_DRV_Motion_DetectionInfo_Get(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_DETECTION_GET, pstMotion );

}

void NC_API_DRV_Motion_OnOff_Set(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_ON_OFF_SET, pstMotion );

}

void NC_API_DRV_Motion_AllBlock_OnOff_Set(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_ALL_BLOCK_SET, pstMotion );
}

void NC_API_DRV_Motion_EachBlock_OnOff_Set(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_BLOCK_SET, pstMotion );
}

void NC_API_DRV_Motion_EachBlock_OnOff_Get(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_BLOCK_GET, pstMotion );
}
void NC_API_DRV_Motion_TSEN_Set(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_TSEN_SET, pstMotion );
}

void NC_API_DRV_Motion_PSEN_Set(nc_decoder_s *pstMotion)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_PSEN_SET, pstMotion );
}

/******************************************************************************************
 * 6. ETC
*******************************************************************************************/
void NC_API_DRV_Bank_Data_Dump(nc_decoder_s *BankReadval)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_REG_BANK_DUMP_GET, BankReadval );
}

void NC_API_DRV_RegisterData_Set(nc_decoder_s *pstCoaxInfo)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_REG_DATA_SET, pstCoaxInfo );
}

void NC_API_DRV_RegisterData_Get(nc_decoder_s *pstCoaxInfo)
{
	if(g_fd_raptor4_drv<0)
	{
		printf("Driver Load Error\n");
		return;
	}
	ioctl( g_fd_raptor4_drv, IOC_VDEC_REG_DATA_GET, pstCoaxInfo );
}

#else

/******************************************************************************************
 * 1. Driver Chip Information Get
*******************************************************************************************/
void NC_API_DRV_InformationData_Get(nc_decoder_s *psDevInfo)
{
	//ioctl( g_fd_raptor4_drv, IOC_VDEC_CHIP_INFO_GET, psDevInfo );
	nc_drv_chip_infomation_to_app(psDevInfo);
}


/******************************************************************************************
 * 2. Video
*******************************************************************************************/
void NC_API_DRV_Video_Input_Set_Info_Get(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_INFO_GET, pstVD_Video );
}

void NC_API_DRV_Video_Input_VFC_Get(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_VFC_GET, pstVD_Video );
}

void NC_API_DRV_Video_Input_EQ_Stage_Get(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_EQ_STAGE_GET, pstVD_Video );
}

void NC_API_DRV_Video_Input_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_SET, pstVD_Video );
}

void NC_API_DRV_Video_Input_EQ_Stage_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_EQ_STAGE_SET, pstVD_Video );
}

void NC_API_DRV_Video_Input_format_define_get(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_MANUAL_SET, pstVD_Video );
}

void NC_API_DRV_Video_Input_Manual_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_INPUT_MANUAL_SET, pstVD_Video );
}


void NC_API_DRV_Video_Output_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_SET, pstVD_Video );
}

void NC_API_DRV_Video_Output_Manual_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_MANUAL_SET, pstVD_Video );
}

void NC_API_DRV_Video_Output_Pattern_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_PATTERN_SET, pstVD_Video );
}


void NC_API_DRV_Video_AOC_Format_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_OUTPUT_PATTERN_SET, pstVD_Video );
}

void NC_API_DRV_Video_Auto_Manual_Mode_Set(nc_decoder_s *pstVD_Video)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_VIDEO_AUTO_MANUAL_MODE_SET, pstVD_Video );
}


/******************************************************************************************
 * 3. Coaxial
*******************************************************************************************/
void NC_API_DRV_Coax_Initialize_Set(nc_decoder_s *pstCoax)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_INIT_SET, pstCoax );
}

void NC_API_DRV_Coax_Initialize_Manual_Set(nc_decoder_s *pstCoax)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_INIT_MANUAL_SET, pstCoax );
}

void NC_API_DRV_Coax_UP_Stream_Command_Set(nc_decoder_s *pstCoax)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_UP_COMMAMD_SET, pstCoax );
}

void NC_API_DRV_Coax_Down_Stream_Data_Get(nc_decoder_s *pstCoax)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_DOWN_DATA_GET, pstCoax );
}


void NC_API_DRV_Coax_FW_ACP_Header_Get( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_ACP_HEADER_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Ready_CMD_Set( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_READY_CMD_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Ready_ACK_Get( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_READY_ACK_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Start_CMD_Set( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_START_CMD_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_Start_ACK_Get( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_START_ACK_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_OnePacketData_Set( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_SEND_DATA_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_OnePacketData_ACK_Get( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_SEND_ACK_GET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_End_CMD_Set( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_END_CMD_SET, pstFileInfo );
}

void NC_API_DRV_Coax_FW_End_CMD_Get( nc_decoder_s *pstFileInfo )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_FW_END_ACK_GET, pstFileInfo );
}


void NC_API_DRV_Coax_Test_ISP_Data_Set( nc_decoder_s *pstDecoder )
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_COAX_TEST_ISP_DATA_SET, pstDecoder );
}

/******************************************************************************************
 * 4. AOC
*******************************************************************************************/
void NC_API_DRV_Audio_Init_Set(nc_decoder_s *pstAudio)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_AUDIO_INIT_SET, pstAudio );

}


/******************************************************************************************
 * 5. Motion
*******************************************************************************************/
void NC_API_DRV_Motion_DetectionInfo_Get(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_DETECTION_GET, pstMotion );

}

void NC_API_DRV_Motion_OnOff_Set(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_ON_OFF_SET, pstMotion );

}

void NC_API_DRV_Motion_AllBlock_OnOff_Set(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_ALL_BLOCK_SET, pstMotion );
}

void NC_API_DRV_Motion_EachBlock_OnOff_Set(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_BLOCK_SET, pstMotion );
}

void NC_API_DRV_Motion_EachBlock_OnOff_Get(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_BLOCK_GET, pstMotion );
}
void NC_API_DRV_Motion_TSEN_Set(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_TSEN_SET, pstMotion );
}

void NC_API_DRV_Motion_PSEN_Set(nc_decoder_s *pstMotion)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_MOTION_PSEN_SET, pstMotion );
}

/******************************************************************************************
 * 6. ETC
*******************************************************************************************/
void NC_API_DRV_Bank_Data_Dump(nc_decoder_s *BankReadval)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_REG_BANK_DUMP_GET, BankReadval );
}

void NC_API_DRV_RegisterData_Set(nc_decoder_s *pstCoaxInfo)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_REG_DATA_SET, pstCoaxInfo );
}

void NC_API_DRV_RegisterData_Get(nc_decoder_s *pstCoaxInfo)
{
	ioctl( g_fd_raptor4_drv, IOC_VDEC_REG_DATA_GET, pstCoaxInfo );
}

#endif
