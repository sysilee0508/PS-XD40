/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
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
#ifndef _RAPTOR4_API_DRV_H_
#define _RAPTOR4_API_DRV_H_

#include "../raptor4_ioctl.h"
#include "../raptor4_fmt.h"

/******************************************************************************************
 * Driver Open & Close
*******************************************************************************************/
int NC_API_DRV_Driver_Init(void);
int NC_API_DRV_Driver_DeInit(void);

/******************************************************************************************
 * Decoder Chip Information Get
*******************************************************************************************/
void NC_API_DRV_InformationData_Get(nc_decoder_s *psVdtDevInfo);

/******************************************************************************************
 * Video
*******************************************************************************************/
/* Input ----*/
void NC_API_DRV_Video_Input_Set_Info_Get(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Input_VFC_Get(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Input_EQ_Stage_Get(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Input_Set(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Input_EQ_Stage_Set(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Input_Manual_Set(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Auto_Manual_Mode_Set(nc_decoder_s *pstVD_Video);

/* Output ----*/
void NC_API_DRV_Video_Output_Set(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Output_Manual_Set(nc_decoder_s *pstVD_Video);
void NC_API_DRV_Video_Output_Pattern_Set(nc_decoder_s *pstVD_Video);

/* AOC ----*/
void NC_API_DRV_Video_AOC_Format_Set(nc_decoder_s *pstVD_Video);

/******************************************************************************************
 * Function - Coaxial
*******************************************************************************************/
void NC_API_DRV_Coax_Initialize_Set(nc_decoder_s *pstCoax);
void NC_API_DRV_Coax_UP_Stream_Command_Set(nc_decoder_s *pstCoax);
void NC_API_DRV_Coax_Down_Stream_Data_Get(nc_decoder_s *pstCoax);

/* Coaxial FW Update ----*/
void NC_API_DRV_Coax_FW_ACP_Header_Get( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_Ready_CMD_Set( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_Ready_ACK_Get( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_Start_CMD_Set( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_Start_ACK_Get( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_OnePacketData_Set( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_OnePacketData_ACK_Get( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_End_CMD_Set( nc_decoder_s *pstFileInfo );
void NC_API_DRV_Coax_FW_End_CMD_Get( nc_decoder_s *pstFileInfo );


/******************************************************************************************
 * Function - Motion
*******************************************************************************************/
void NC_API_DRV_Motion_DetectionInfo_Get(nc_decoder_s *pstMotion);
void NC_API_DRV_Motion_PSEN_Set(nc_decoder_s *pstMotionInfo);
void NC_API_DRV_Motion_OnOff_Set(nc_decoder_s *pstMotion);
void NC_API_DRV_Motion_AllBlock_OnOff_Set(nc_decoder_s *pstMotion);
void NC_API_DRV_Motion_EachBlock_OnOff_Set(nc_decoder_s *pstMotion);
void NC_API_DRV_Motion_EachBlock_OnOff_Get(nc_decoder_s *pstMotion);
void NC_API_DRV_Motion_TSEN_Set(nc_decoder_s *pstMotion);
void NC_API_DRV_Motion_PSEN_Set(nc_decoder_s *pstMotion);

/******************************************************************************************
 * Function - Audio
*******************************************************************************************/
void NC_API_DRV_Audio_Init_Set(nc_decoder_s *pstMotion);

/******************************************************************************************
 * ETC - Register Dump ioctl
*******************************************************************************************/
void NC_API_DRV_Bank_Data_Dump(nc_decoder_s *BankReadval);
void NC_API_DRV_RegisterData_Set(nc_decoder_s *pstCoaxInfo);
void NC_API_DRV_RegisterData_Get(nc_decoder_s *pstCoaxInfo);



#endif
