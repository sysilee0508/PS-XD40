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

#ifndef _RAPTOR4_API_H_
#define _RAPTOR4_API_H_

#include "../raptor4_fmt.h"

/*********************************************************************************************************************************************************************
 * Load decoder device driver
 *********************************************************************************************************************************************************************/
int   DECODER_LoadDriver(void);
int   DECODER_Chip_Infomation_Get( void );
int   DECODER_Chip_Amount_Get( void );
NC_U8 DECODER_Chip_ID_Get(int ChipID);

/*********************************************************************************************************************************************************************
 * Decoder - Video API
 *********************************************************************************************************************************************************************/
void DECODER_Video_SettingStatus_information_get( NC_U8 Chn, NC_CABLE_E *Cable, NC_FORMAT_STANDARD_E *Fmt_std, NC_FORMAT_RESOLUTION_E *Fmt_res, NC_FORMAT_FPS_E *Fmt_fps, NC_U8 *Distance );
NC_U8 DECODER_Video_Input_Format_Detection_Get(NC_U8 Chn, NC_U8 *Vfc, NC_VIVO_CH_FORMATDEF_E *VideoFormat);
NC_U8 DECODER_Video_Input_EQ_Stage_Get(NC_U8 Chn, NC_U32 *pSamVal);
void DECODER_Video_Input_Auto_Sequence_Set(NC_U8 Chn);
void DECODER_Video_Input_Manual_Set(NC_U8 Chn, NC_CABLE_E Cable, NC_FORMAT_STANDARD_E Fmt_std, NC_FORMAT_RESOLUTION_E Fmt_res, NC_FORMAT_FPS_E Fmt_fps);
void DECODER_Video_Input_EQ_Stage_Set(int Chn, NC_EQ_STAGE_E stage);
void DECODER_Video_Output_Set(NC_U8 Chip, NC_VO_WORK_MODE_E MuxMode);
void DECODER_Video_Output_Manual_Set( NC_U8 Chn, NC_VO_INTF_MODE_E Intf, NC_VO_WORK_MODE_E MuxMode, NC_VO_CLK_EDGE_E Clock, int *pChnSeq);
void DECODER_Video_Output_NoVideo_Pattern_Set( NC_U8 Chn, NC_U8 Sel );
void DECODER_SDK_Video_Format_String_Get(NC_U8 Chn, char *pStr);
void DECODER_SDK_Channel_Video_Format_String_Get(NC_U8 Chn, char *pStr);
void DECODER_SDK_Video_Auto_Manual_Mode_Set(NC_U8 Chn, NC_VIDEO_SET_MODE_E Mode);

NC_EQ_STAGE_E DECODER_SDK_Video_EQ_Stage_ReCheck_Get(NC_U8 Chn);

/*********************************************************************************************************************************************************************
 * Decoder - Coaxial API
 *********************************************************************************************************************************************************************/
void DECODER_Coax_Initialize_Set( int Chn, int Mode );
void DECODER_Coax_Command_Send_Set( NC_U8 Chn, NC_U8 SelectItem, NC_COAX_CMD_DEF_E cmd );
void DECODER_Coax_DownStream_Data_Get( int Chn, unsigned char *pData );

 int DECODER_Coax_FW_Upgrade_Init( int Ch );
void DECODER_Coax_FW_File_Get( int Ch, char *pFileName, int select );
void DECODER_Coax_FW_Upadate_Start( int Ch, int FileSel );
 int DECODER_Coax_FW_Update_Status_Get( void );
 int DECODER_Coax_FW_Progress_Status_Get( void );

 NC_S32 DECODER_NVP2482H_ISP_Check_GET( int Chn );
 NC_S32 DECODER_ISP_Mode_Change_Coax_Set( int Chn );
 int    DECODER_ISP_3DNR_Data_Get( int Chn );
 int    DECODER_ISP_Info_Chip_Name_Get(unsigned char ISP_Chip, char *pStr);
 int    DECODER_ISP_Info_Sensor_Maker_Device_Get(unsigned char Maker, unsigned char Device, char *pMakerStr, char *pDeviceStr);

/*********************************************************************************************************************************************************************
 * Decoder - Motion API
 *********************************************************************************************************************************************************************/
NC_U8 DECODER_Motion_Get( int Chn, int SelectItem, int Val );
  int DECODER_Motion_Set( int Chn, int SelectItem, int Val );

/*********************************************************************************************************************************************************************
* Decoder - Audio API
*********************************************************************************************************************************************************************/
  void DECODER_Audio_Mode_Init(int Chn, int AdMode, int SmpRate, int BitWidth);

/*********************************************************************************************************************************************************************
 * Decoder - Register Setting API
 *********************************************************************************************************************************************************************/
 void DECODER_Bank_Data_Dump(unsigned char slave_addr, unsigned char bank, unsigned char *pData);
 void DECODER_Register_Data_Set(NC_U8 Slave, NC_U8 Bank, NC_U8 Addr, NC_U8 SetVal);
NC_U8 DECODER_Register_Data_Get(NC_U8 Slave, NC_U8 Bank, NC_U8 Addr);



#endif
