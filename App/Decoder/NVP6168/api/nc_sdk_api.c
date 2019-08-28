/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: Decoder api
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if 0
#include <sys/types.h>
#include <sys/sysinfo.h>
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
#endif
/* 1. Header file include --------------------------------------------------*/
#include "nc_sdk_define.h"
#include "nc_sdk_api.h"
#include "nc_sdk_common.h"
#include "nc_sdk_coax_firmup.h"
#include "nc_sdk_api_drv.h"

#include "NVP6168.h"
/* 2. Define  --------------------------------------------------------------*/


/* 3. Define variable  -----------------------------------------------------*/
static nc_decoder_s sVdtDevInfo;
char FrimUP_Status[1024];
static char CoaxRx[30][128];

/* 4. External variable & function  -----------------------------------*/

/* 5. Function prototype  --------------------------------------------------*/
int DECODER_Chip_Infomation_Get( void );

/****************************************************************************************************************
 * Load Decoder Device Driver Function
 *
 ****************************************************************************************************************/
 #if PLATFORM_LINUX == 1
int DECODER_LoadDriver( void )
{
	NC_API_DRV_Driver_Init();

	DECODER_Chip_Infomation_Get();

	return 0;
}
#endif

int DECODER_Chip_Infomation_Get( void )
{
	memset(&sVdtDevInfo, 0, sizeof(nc_decoder_s));
	//NC_API_DRV_InformationData_Get( &sVdtDevInfo );
	nc_drv_chip_infomation_to_app(&sVdtDevInfo);
	
	return 0;
}

int DECODER_Chip_Amount_Get( void )
{
	//return sVdtDevInfo.Total_Chip_Cnt;
	return TOTAL_CHIP_CNT;
}

NC_U8 DECODER_Chip_ID_Get(int ChipID)
{
	//return sVdtDevInfo.chip_id[ChipID];
	return sVdtDevInfo.chip_id[0];
}

/****************************************************************************************************************
 * Decoder API
 * Video Auto Detection Function
 *
 ****************************************************************************************************************/
void DECODER_Video_SettingStatus_information_get( NC_U8 Chn, NC_CABLE_E *Cable, NC_FORMAT_STANDARD_E *Fmt_std, NC_FORMAT_RESOLUTION_E *Fmt_res, NC_FORMAT_FPS_E *Fmt_fps, NC_U8 *Distance )
{
	nc_decoder_s stVD_Video;

	stVD_Video.Chn = Chn;

	//NC_API_DRV_Video_Input_Set_Info_Get(&stVD_Video);
	nc_drv_video_setting_info_get(&stVD_Video);

	*Cable    = stVD_Video.VideoCable;
	*Fmt_std  = stVD_Video.FormatStandard;
	*Fmt_res  = stVD_Video.FormatResolution;
	*Fmt_fps  = stVD_Video.FormatFps;
	*Distance = stVD_Video.EqStage;
}

NC_U8 DECODER_Video_Input_Format_Detection_Get(NC_U8 Chn, NC_U8 *Vfc, NC_VIVO_CH_FORMATDEF_E *VideoFormat)
{
	nc_decoder_s stVD_Video;
	stVD_Video.Chn = Chn;

	//NC_API_DRV_Video_Input_VFC_Get(&stVD_Video);
	nc_drv_video_input_vfc_status_get(&stVD_Video);

	*Vfc 		 = stVD_Video.Vfc;
	*VideoFormat = stVD_Video.VideoFormat;

	return stVD_Video.FmtChange;
}

NC_U8 DECODER_Video_Input_EQ_Stage_Get(NC_U8 Chn, NC_U32 *pSamVal)
{
	nc_decoder_s pstVD_Video;

	pstVD_Video.Chn = Chn;
	//NC_API_DRV_Video_Input_EQ_Stage_Get(&pstVD_Video);
	nc_drv_video_input_eq_stage_get(&pstVD_Video);

	*pSamVal = pstVD_Video.SamVal;

	return pstVD_Video.EqStage;
}

void DECODER_Video_Input_Auto_Sequence_Set(NC_U8 Chn)
{
	nc_decoder_s pstVD_Video;

	pstVD_Video.Chn = Chn;
	pstVD_Video.VideoSetMode = NC_VIDEO_SET_MODE_AUTO;
	//NC_API_DRV_Video_Input_Set(&pstVD_Video);
	nc_drv_video_input_set(&pstVD_Video);
}

void DECODER_Video_Input_Manual_Set(NC_U8 Chn, NC_CABLE_E Cable, NC_FORMAT_STANDARD_E Fmt_std, NC_FORMAT_RESOLUTION_E Fmt_res, NC_FORMAT_FPS_E Fmt_fps)
{
	nc_decoder_s pstVD_Video;

	pstVD_Video.Chn 			 = Chn;
	pstVD_Video.VideoCable  	 = Cable;
	pstVD_Video.FormatStandard   = Fmt_std;
	pstVD_Video.FormatResolution = Fmt_res;
	pstVD_Video.FormatFps		 = Fmt_fps;
	pstVD_Video.VideoSetMode     = NC_VIDEO_SET_MODE_MANUAL;

	//NC_API_DRV_Video_Input_Manual_Set(&pstVD_Video);
	nc_drv_video_input_manual_set(&pstVD_Video);
}

void DECODER_Video_Input_EQ_Stage_Set(int Chn, NC_EQ_STAGE_E stage)
{
	nc_decoder_s stDecoder;
	stDecoder.Chn 			 = Chn;
	stDecoder.EqStage		 = stage;
	//NC_API_DRV_Video_Input_EQ_Stage_Set(&stDecoder);
	nc_drv_video_input_eq_stage_set(&stDecoder);
}

void DECODER_Video_Output_Set(NC_U8 Chip, NC_VO_WORK_MODE_E MuxMode)
{
	nc_decoder_s stVD_Video;

	stVD_Video.Chn 			= Chip;
	stVD_Video.VO_MuxMode   = MuxMode;

	stVD_Video.PortNo = 0;

	//NC_API_DRV_Video_Output_Set(&stVD_Video);
	nc_drv_video_output_port_set(&stVD_Video);
}

void DECODER_Video_Output_Manual_Set( NC_U8 Chn, NC_VO_INTF_MODE_E Intf, NC_VO_WORK_MODE_E MuxMode, NC_VO_CLK_EDGE_E Clock, int *pChnSeq)
{
	int ii = 0;
	nc_decoder_s stVD_Video;

	stVD_Video.Chn 			= Chn;
	stVD_Video.VO_Interface = Intf;
	stVD_Video.VO_MuxMode   = MuxMode;
	stVD_Video.VO_Edge 		= Clock;

	for(ii=0; ii<4; ii++)
	{
		stVD_Video.VO_ChnSeq[ii] = pChnSeq[ii];
	}

	//NC_API_DRV_Video_Output_Manual_Set(&stVD_Video);
	nc_drv_video_output_port_manual_set(&stVD_Video);
}

void DECODER_Video_Output_NoVideo_Pattern_Set( NC_U8 Chn, NC_U8 Sel )
{
	nc_decoder_s stVD_Video;
	stVD_Video.Chn   = Chn;
	stVD_Video.Value = Sel;
	//NC_API_DRV_Video_Output_Pattern_Set(&stVD_Video);
	nc_drv_video_output_color_pattern_set(&stVD_Video);
}

NC_EQ_STAGE_E DECODER_SDK_Video_EQ_Stage_ReCheck_Get(NC_U8 Chn)
{
	int Ret = 0;

	nc_decoder_s stVD_Video;
	memset(&stVD_Video, 0, sizeof(stVD_Video));
	stVD_Video.Chn = Chn;
	//NC_API_DRV_Video_Input_Set_Info_Get(&stVD_Video);
	nc_drv_video_setting_info_get(&stVD_Video);

	return stVD_Video.EqStage;
}

#if 0
void DECODER_SDK_Video_Format_String_Get(NC_U8 Chn, char *pStr)
{
	nc_decoder_s pstVD_Video;

	pstVD_Video.Chn = Chn;
	NC_API_DRV_Video_Input_Set_Info_Get(&pstVD_Video);

	if(pstVD_Video.FmtStr == NULL)
	{
		sprintf(pStr, "Chn%d: Unknown Channel", Chn+1);
		printf("[%s::%d]%s::Unknown Channel%d\n", __FILE__, __LINE__, __func__, Chn );
		return;
	}

	sprintf(pStr, "Chn%d: %s", Chn+1, pstVD_Video.FmtStr);

}
void DECODER_SDK_Channel_Video_Format_String_Get(NC_U8 Chn, char *pStr)
{
	nc_decoder_s pstVD_Video;

	pstVD_Video.Chn = Chn;
	NC_API_DRV_Video_Input_Set_Info_Get(&pstVD_Video);

	if(pstVD_Video.FmtStr == NULL)
	{
		sprintf(pStr, "Chn%d: Unknown Channel", Chn+1);
		printf("[%s::%d]%s::Unknown Channel%d\n", __FILE__, __LINE__, __func__, Chn );
		return;
	}

	sprintf(pStr, "%s", pstVD_Video.FmtStr);

}
#endif


void DECODER_SDK_Video_Auto_Manual_Mode_Set(NC_U8 Chn, NC_VIDEO_SET_MODE_E Mode)
{
	nc_decoder_s stVD_Video;
	stVD_Video.Chn	   = Chn;
	stVD_Video.OP_Mode = Mode;

	//NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);
	nc_drv_video_auto_manual_mode_set(&stVD_Video);
}


#if COAXIAL_PROTOCOL == 1
/****************************************************************************************************************
 * Decoder API
 * Coaxial Protocol
 *
 ****************************************************************************************************************/
void DECODER_Coax_Initialize_Set( int Chn, int Mode )
{
	nc_decoder_s stCoaxInfo;

	stCoaxInfo.Chn = Chn;

	switch(Mode)
	{
		case    VD_COAX_NORMAL_MODE :
			stCoaxInfo.Coax_ver = COAX_COAX_NORMAL;
			NC_API_DRV_Coax_Initialize_Set( &stCoaxInfo );
			break;
		case    VD_COAX_ACP_16BIT_MODE :
			stCoaxInfo.Coax_ver = COAX_AHD_16BIT;
			NC_API_DRV_Coax_Initialize_Set( &stCoaxInfo );
			break;
		case    VD_COAX_CCP_NEW_MODE :
			stCoaxInfo.Coax_ver = COAX_CVI_NEW;
			NC_API_DRV_Coax_Initialize_Set( &stCoaxInfo );
			break;
		default :
			printf("[%s]Unknown Coax CMD Select!!\n", __func__);
			break;
	}
}

void DECODER_Coax_Command_Send_Set( NC_U8 Chn, NC_U8 SelectItem, NC_COAX_CMD_DEF_E cmd )
{
	nc_decoder_s stCoaxInfo;

	stCoaxInfo.Chn = Chn;
	stCoaxInfo.Cmd = cmd;

	switch(SelectItem)
	{
		case    VD_COAX_NORMAL_MODE :
			stCoaxInfo.Coax_ver = COAX_COAX_NORMAL;
			NC_API_DRV_Coax_UP_Stream_Command_Set( &stCoaxInfo );
			break;
		case    VD_COAX_ACP_16BIT_MODE :
			stCoaxInfo.Coax_ver = COAX_AHD_16BIT;
			NC_API_DRV_Coax_UP_Stream_Command_Set( &stCoaxInfo );
			break;
		case    VD_COAX_CCP_NEW_MODE :
			stCoaxInfo.Coax_ver = COAX_CVI_NEW;
			NC_API_DRV_Coax_UP_Stream_Command_Set( &stCoaxInfo );
			break;
		default :
			printf("[%s]Unknown Coax CMD Select!!\n", __func__);
			break;
	}
}

void DECODER_Coax_DownStream_Data_Get( int Chn, unsigned char *pData )
{
	int ii = 0;

	nc_decoder_s RxData;
	RxData.Chn = Chn;

	memset(CoaxRx, 0, sizeof(CoaxRx));

	NC_API_DRV_Coax_Down_Stream_Data_Get(&RxData);


	if( (RxData.FormatStandard == FMT_TVI) || (RxData.FormatStandard == FMT_CVI) )
	{
		for(ii=0; ii<5; ii++)
		{
			pData[ii] 	 = RxData.rx_data1[ii];
			pData[ii+5]  = RxData.rx_data2[ii];
			pData[ii+10] = RxData.rx_data3[ii];
			pData[ii+15] = RxData.rx_data4[ii];
			pData[ii+20] = RxData.rx_data5[ii];
			pData[ii+25] = RxData.rx_data6[ii];

			sprintf(CoaxRx[ii],    "%X", RxData.rx_data1[ii]);
			sprintf(CoaxRx[ii+5],  "%X", RxData.rx_data2[ii]);
			sprintf(CoaxRx[ii+10], "%X", RxData.rx_data3[ii]);
			sprintf(CoaxRx[ii+15], "%X", RxData.rx_data4[ii]);
			sprintf(CoaxRx[ii+20], "%X", RxData.rx_data5[ii]);
			sprintf(CoaxRx[ii+25], "%X", RxData.rx_data6[ii]);
		}
	}
	else
	{
		for(ii=0; ii<8; ii++)
		{
			pData[ii] = RxData.rx_pelco_data[ii];
			sprintf(CoaxRx[ii], "%X", RxData.rx_pelco_data[ii]);
		}
	}
}


/*============================================================================================================================
 * Coaxial Protocol F/W Upgrade
 *
 ============================================================================================================================*/
int DECODER_Coax_FW_Upgrade_Init( int Ch )
{
	int FileCnt = 0;

	FileCnt = NC_APP_VD_COAX_FirmUP_File_Count_Get("/mnt/usb");

	printf("[%s::%d]Coax_FirmUP update file count :: %d\n", __FILE__, __LINE__, FileCnt);

	return FileCnt;
}

void DECODER_Coax_FW_File_Get( int Ch, char *pFileName, int select )
{
	NC_APP_VD_COAX_FirmUP_File_Select_Get( pFileName, select );
}


void DECODER_Coax_FW_Upadate_Start( int Ch, int FileSel )
{
	printf("[%s::%d]Coax_FirmUP Upgrade Start\n", __func__, __LINE__);

	NC_APP_VD_COAX_FirmUP_CoaxFWUpdate_Start(Ch, FileSel);

}

int DECODER_Coax_FW_Update_Status_Get( void )
{
	int progress_flag = 0;
	progress_flag = NC_APP_VD_COAX_FirmUP_Status_Get(FrimUP_Status);

	return progress_flag;
}

int DECODER_Coax_FW_Progress_Status_Get( void )
{
	return NC_APP_VD_COAX_FirmUP_ProgressBar_Status_Get();
}


void DECODER_Coax_Test_ISP_FW_Data_Set(NC_U8 SignalOn, NC_COAX_CMD_DEF_E cmd)
{
	nc_decoder_s data;
	data.Param = SignalOn;
	data.Cmd   = cmd;

	NC_API_DRV_Coax_Test_ISP_Data_Set(&data);
}



NC_S32 DECODER_NVP2482H_ISP_Check_GET( int Chn )
{
	NC_S32 ret 		= -1;
	NC_U8  ISP_Chip = 0;

	nc_decoder_s stCoaxInfo;

	stCoaxInfo.Chn = Chn;
	NC_API_DRV_Coax_Down_Stream_Data_Get(&stCoaxInfo);

	if( stCoaxInfo.rx_pelco_data[0] == 0x55 )
	{
		/*********************************************
		 * Chip Info[7:3], EQ[2], B/W[1], _Rev[0]
		 *********************************************/
		ISP_Chip = stCoaxInfo.rx_pelco_data[1]>>3;

		/*********************************************
		 * SOC 3DNR Support - NVP2482H
		 *********************************************/
		if(ISP_Chip == 0x17)
		{
			ret = 0;
//			printf("[%s]NVP2482H::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
		}
		else
		{
#if 0
			if(ISP_Chip == 0x10)		printf("[%s]NVP2470H::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else if(ISP_Chip == 0x11)	printf("[%s]NVP2433H::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else if(ISP_Chip == 0x12)	printf("[%s]NVP2443I::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else if(ISP_Chip == 0x13)	printf("[%s]NVP2475H::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else if(ISP_Chip == 0x14)	printf("[%s]NVP2475H_UNI::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else if(ISP_Chip == 0x15)	printf("[%s]NVP2477H::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else if(ISP_Chip == 0x16)	printf("[%s]NVP2481H::(0x%02X, %d)\n", __func__, ISP_Chip, ISP_Chip);
			else printf("Unknown ISP Information::(%d, 0x%02X)\n", ISP_Chip, ISP_Chip);
#endif
			ret = -1;
		}
	}

	return ret;
}

NC_S32 DECODER_ISP_Mode_Change_Coax_Set( int Chn )
{
	NC_S32 ret = 0;

	nc_decoder_s stCoaxInfo;

	stCoaxInfo.Chn 		= Chn;
	stCoaxInfo.Cmd 		= COAX_CMD_SPECIAL_3DNR_REQUEST;
	stCoaxInfo.Coax_ver = COAX_COAX_NORMAL;
	NC_API_DRV_Coax_UP_Stream_Command_Set(&stCoaxInfo);
	NC_API_DRV_Coax_Down_Stream_Data_Get(&stCoaxInfo);

	if( stCoaxInfo.rx_pelco_data[7] == 0x10 )
	{
		ret = 1;
	}

	return ret;
}

int DECODER_ISP_3DNR_Data_Get( int Chn )
{
	NC_S32 ret = 0;
	nc_decoder_s stCoaxInfo;

	stCoaxInfo.Chn = Chn;

	NC_API_DRV_Coax_Down_Stream_Data_Get(&stCoaxInfo);
	ret = stCoaxInfo.rx_pelco_data[3];

	return ret;
}


/*************************************************************************************
 * ISP Information
 *************************************************************************************/
int DECODER_ISP_Info_Chip_Name_Get(unsigned char ISP_Chip, char *pStr)
{
	int ret = 0;
	unsigned char ISP = 0;

	ISP = ISP_Chip>>3;

	if(ISP == 0x10)			strcpy(pStr, "NVP2470H");
	else if(ISP == 0x01)	strcpy(pStr, "NVP2450H");
	else if(ISP == 0x02)	strcpy(pStr, "NVP2440H");
	else if(ISP == 0x03)	strcpy(pStr, "NVP2630");
	else if(ISP == 0x04)	strcpy(pStr, "NVP2630");
	else if(ISP == 0x05)	strcpy(pStr, "NVP2480H");
	else if(ISP == 0x11)	strcpy(pStr, "NVP2433H");
	else if(ISP == 0x12)	strcpy(pStr, "NVP2443I");
	else if(ISP == 0x13)	strcpy(pStr, "NVP2475H");
	else if(ISP == 0x14)	strcpy(pStr, "NVP2475H_UNI");
	else if(ISP == 0x15)	strcpy(pStr, "NVP2477H");
	else if(ISP == 0x16)	strcpy(pStr, "NVP2481H");
	else if(ISP == 0x17)	strcpy(pStr, "NVP2482H");
	else
	{
		strcpy(pStr, "Unknown ISP");
		printf("Unknown ISP Information::(%d, 0x%02X)\n", ISP, ISP);
		ret = -1;
	}

	return ret;
}

static char SONY_DevName[20][128] = {
	"IMX136", "IMX222",	"IMX238", "IMX225", "IMX224", "IMX290", "IMX291", "IMX104",	"IMX174","IMX236", "IMX322", "IMX323", "IMX185", "IMX124", "IMX326", "IMX123", "IMX335", "IMX390", "IMX334",
};

static char OMNIVISION_DevName[20][128] = {
	"OV9712", "OX2710",	"OV9750", "OV9732",	"OV10635", "OV10640", "OV2718", "OV2775", "OV0823", "OV4689", "OV5653", "OV5658", "OV5680", "OV2735", "OS05A", "OS05A10", "OS08A10", "OV12895",
};

static char APTINA_DevName[20][128] = {
	"AR0130", "AR0132", "AR0140", "AR0141", "AR0330", "AR0230", "AR0331", "AR0237", "AR0335", "AR0521", "AR082x",
};

static char SOI_DevName[20][128] = {
	"JXH22", "JXH42", "JXF02", "H61", "JXF22", "H65", "K03", "K02", "H62",
};

static char PIXELPLUS_DevName[20][128] = {
	"PO3100", "PS4100",	"PS1210",
};

static char PIXART_DevName[20][128] = {
	"PAS5130", "PS5220", "PAS5120", "PAS5230", "PAS5250", "PAS5510",
};

static char GALAXYCORE_DevName[20][128] = {
	"GC1004", "GC1014", "GC1024", "GC1064",	"GC1323", "GC2023",	"GC4603", "GC5024",	"GC5604",
};

static char BYD_DevName[20][128] = {
	"BF3116", "BF3016",
};

static char SMARTSENS_DevName[20][128] = {
	"SC1035", "SC1045",	"SC2035", "SC1135", "SC1145", "SC2045", "SC2135", "SC3035", "SC5035", "SC0820", "SC5300", "SC2235",	"SC1235", "SC1245", "SC4236", "SC5235", "SC2232",
};

static char BRIGATES_DevName[20][128] = {
	"BG0703", "BG0803",	"BG0805", "BG0806",
};

static char SILICONFILE_DevName[20][128] = {
	"HI131",
};

static char SAMSUNG_LSI_DevName[20][128] = {
	"S5K3H5XA(M3M85)",
};

static char PANASONIC_DevName[20][128] = {
	"MN34229", "MN34210", "MN34041", "MN34227",
};

static char CLAREPIXEL_DevName[20][128] = {
	"CP5112",
};

static char IMAGEDISIGN_DevName[20][128] = {
	"MIS1002", "MIS2001", "MIS3001",
};

static char NOVATECH_DevName[20][128] = {
	"MT99143", "MT99231", "MT99546",
};

static char SUPERPIX_DevName[20][128] = {
	"SP1409", "SP140A",
};

int DECODER_ISP_Info_Sensor_Maker_Device_Get(unsigned char Maker, unsigned char Device, char *pMakerStr, char *pDeviceStr)
{
	int ret = 0;

	if(Maker == 0x01)
	{
		strcpy(pMakerStr, "SONY");
		if( 0x00 <= Device && Device <= 0x12 )
			strcpy(pDeviceStr, SONY_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x02)
	{
		strcpy(pMakerStr, "OMNIVISION");
		if( 0x00 <= Device && Device <= 0x11 )
			strcpy(pDeviceStr, OMNIVISION_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x03)
	{
		strcpy(pMakerStr, "APTINA");
		if( 0x00 <= Device && Device <= 0x0A )
			strcpy(pDeviceStr, APTINA_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x04)
	{
		strcpy(pMakerStr, "SOI");
		if( 0x00 <= Device && Device <= 0x08 )
			strcpy(pDeviceStr, SOI_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x05)
	{
		strcpy(pMakerStr, "PIXELPLUS");
		if( 0x00 <= Device && Device <= 0x02 )
			strcpy(pDeviceStr, PIXELPLUS_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x06)
	{
		strcpy(pMakerStr, "PIXART");
		if( 0x00 <= Device && Device <= 0x05 )
			strcpy(pDeviceStr, PIXART_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x07)
	{
		strcpy(pMakerStr, "GALAXYCORE");
		if( 0x00 <= Device && Device <= 0x08 )
			strcpy(pDeviceStr, GALAXYCORE_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x08)
	{
		strcpy(pMakerStr, "BYD");
		if( 0x00 <= Device && Device <= 0x01 )
			strcpy(pDeviceStr, BYD_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x09)
	{
		strcpy(pMakerStr, "SMARTSENS");
		if( 0x00 <= Device && Device <= 0x10 )
			strcpy(pDeviceStr, SMARTSENS_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x0a)
	{
		strcpy(pMakerStr, "BRIGATES");
		if( 0x00 <= Device && Device <= 0x03 )
			strcpy(pDeviceStr, BRIGATES_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x0b)
	{
		strcpy(pMakerStr, "SILICONFILE");
		if( 0x00 == Device )
			strcpy(pDeviceStr, SILICONFILE_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x0c)
	{
		strcpy(pMakerStr, "SAMSUNG LSI");
		if( 0x00 == Device )
			strcpy(pDeviceStr, SAMSUNG_LSI_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x0d)
	{
		strcpy(pMakerStr, "PANASONIC");
		if( 0x00 <= Device && Device <= 0x03 )
			strcpy(pDeviceStr, PANASONIC_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x0e)
	{
		strcpy(pMakerStr, "CLAREPIXEL");
		if( 0x00 == Device )
			strcpy(pDeviceStr, CLAREPIXEL_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x0f)
	{
		strcpy(pMakerStr, "IMAGEDISIGN");
		if( 0x00 <= Device && Device <= 0x02 )
			strcpy(pDeviceStr, IMAGEDISIGN_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x10)
	{
		strcpy(pMakerStr, "NOVATECH");
		if( 0x00 <= Device && Device <= 0x02 )
			strcpy(pDeviceStr, NOVATECH_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else if(Maker==0x11)
	{
		strcpy(pMakerStr, "SUPERPIX");
		if( 0x00 <= Device && Device <= 0x01 )
			strcpy(pDeviceStr, SUPERPIX_DevName[Device]);
		else
			strcpy(pDeviceStr, "Unknown");
	}
	else
	{
		strcpy(pMakerStr, "Unknown Maker");
		strcpy(pDeviceStr, "Unknown Device");
		printf("Unknown Sensor Maker::(%d, 0x%02X)\n", Maker, Maker);
		ret = -1;
	}

	return ret;
}

#endif //COAXIAL_PROTOCOL


/*============================================================================================================================
 * Motion
 *
 ============================================================================================================================*/
int DECODER_Motion_Set( int Chn, int SelectItem, int Val )
{
	int Ret = -1;

	nc_decoder_s pstMotion;

	pstMotion.Chn    = Chn;
	pstMotion.Select = Val;

	switch(SelectItem)
	{
		case    VD_MOTION_SET_ON_OFF :
			//printf("VD_MOTION_SET_ON_OFF\n");
			//NC_API_DRV_Motion_OnOff_Set(&pstMotion);
			nc_drv_motion_onoff_set(&pstMotion);	
			Ret = 0;
			break;
		case    VD_MOTION_SET_ALL_BLOCK_ON_OFF :
			//printf("VD_MOTION_SET_ALL_BLOCK_ON_OFF\n");
			//NC_API_DRV_Motion_AllBlock_OnOff_Set(&pstMotion);
			nc_drv_motion_all_block_onoff_set(&pstMotion);
			break;
		case    VD_MOTION_SET_EACH_BLOCK_ON_OFF :
			//printf("VD_MOTION_SET_EACH_BLOCK_ON_OFF\n");
			//NC_API_DRV_Motion_EachBlock_OnOff_Set(&pstMotion);
			nc_drv_motion_each_block_onoff_set(&pstMotion);
			Ret = 0;
			break;
		case    VD_MOTION_SET_TSEN :
			//printf("VD_MOTION_SET_TSEN\n");
			//NC_API_DRV_Motion_TSEN_Set(&pstMotion);
			nc_drv_motion_motion_tsen_set(&pstMotion);
			Ret = 0;
			break;
		case    VD_MOTION_SET_PSEN :
			//printf("VD_MOTION_SET_PSEN\n");
			//NC_API_DRV_Motion_PSEN_Set(&pstMotion);
			nc_drv_motion_motion_psen_set(&pstMotion);
			Ret = 0;
			break;
		default :
			//printf("[%s]UnKown Motion Select!!\n", __func__);
			Ret = -1;
			break;
	}

	return Ret;
}

NC_U8 DECODER_Motion_Get( int Chn, int SelectItem, int Val )
{
	NC_U8 ReadValue = 0;

	nc_decoder_s stMotion;
	stMotion.Chn = Chn;
	stMotion.Select = Val;

	switch(SelectItem)
	{
		case    VD_MOTION_GET_DETECT_INFO :
			//NC_API_DRV_Motion_DetectionInfo_Get(&stMotion);
			nc_drv_motion_detection_info_get(&stMotion);
			ReadValue = stMotion.Param;
			break;
		case    VD_MOTION_GET_BLOCK_ON_OFF_INFO :
			//NC_API_DRV_Motion_EachBlock_OnOff_Get(&stMotion);
			nc_drv_motion_each_block_onoff_get(&stMotion);	
			ReadValue = stMotion.Param;
			break;
		default :
			//printf("[%s]UnKown Motion Select!!\n", __func__);
			break;
	}

	return ReadValue;
}

#if 0
/*============================================================================================================================
 * AUDIO
 *
 ============================================================================================================================*/
void DECODER_Audio_Mode_Init(int Chn, int AdMode, int SmpRate, int BitWidth)
{
	nc_decoder_s stAudio;

	stAudio.Chn = 0;
	stAudio.audio_mode 		  = AdMode;
	stAudio.audio_sample_rate = SmpRate;
	stAudio.audio_bit_width   = BitWidth;
	NC_API_DRV_Audio_Init_Set(&stAudio);
}
#endif

#if 0
/*============================================================================================================================
 * ETC
 *
 ============================================================================================================================*/
void DECODER_Bank_Data_Dump(unsigned char slave_addr, unsigned char bank, unsigned char *pData)
{
	nc_decoder_s stRegDumpInfo;

	stRegDumpInfo.Dev  = slave_addr;
	stRegDumpInfo.Bank = bank;

	//NC_API_DRV_Bank_Data_Dump(&stRegDumpInfo);
	nc_drv_common_bank_data_get(&stRegDumpInfo);
	
	memcpy(pData, stRegDumpInfo.DumpData, sizeof(stRegDumpInfo.DumpData) );
}

void DECODER_Register_Data_Set(NC_U8 Slave, NC_U8 Bank, NC_U8 Addr, NC_U8 SetVal)
{
	nc_decoder_s stRegDumpInfo;
	stRegDumpInfo.Dev   = Slave;
	stRegDumpInfo.Bank  = Bank;
	stRegDumpInfo.Addr  = Addr;
	stRegDumpInfo.Param = SetVal;
	//NC_API_DRV_RegisterData_Set(&stRegDumpInfo);
	nc_drv_common_register_data_set(&stRegDumpInfo);
}

NC_U8 DECODER_Register_Data_Get(NC_U8 Slave, NC_U8 Bank, NC_U8 Addr)
{
	nc_decoder_s stRegDumpInfo;
	stRegDumpInfo.Dev   = Slave;
	stRegDumpInfo.Bank  = Bank;
	stRegDumpInfo.Addr  = Addr;
	//NC_API_DRV_RegisterData_Get(&stRegDumpInfo);
	nc_drv_common_register_data_get(&stRegDumpInfo);
	
	return stRegDumpInfo.Param;
}
#endif
