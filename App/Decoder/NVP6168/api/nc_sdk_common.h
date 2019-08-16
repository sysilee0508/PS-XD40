/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: ACP-F/W information
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#ifndef _RAPTOR4_SDK_COMMON_
#define _RAPTOR4_SDK_COMMON_

#include "../raptor4_fmt.h"

int NC_APP_VD_USB_Mount( void );
int NC_APP_VD_USB_Umount( void );

int DECODER_SDK_App_Drv_Update_Check(void);

struct timeval* DECODER_stopwatch_start( void );
double DECODER_stopwatch_stop( struct timeval *st_time, char *str );

int DECODER_SDK_Update_File_Check( void );


void DECODER_SDK_USB_Format_Set(void);
int  DECODER_SDK_USB_Mount(void);
int  DECODER_SDK_USB_Umount( void );
int  DECODER_SDK_Audio_File_Save_Status(void);
void DECODER_SDK_USB_Umount_Wait(void);
int  DECODER_SDK_App_Drv_Update_Check( void);
void DECODER_SDK_NoVideo_Status_Logging( double tOnVideo );
void DECODER_SDK_OnVideo_Distance_Logging( double tNoVideo, int Cam_Seq, char *Str );
void DECODER_SDK_OnVideo_Status_Logging( double tNoVideo, int Cam_Seq, char *Str );
void DECODER_SDK_Distance_Test_Logging( int Chn, NC_U32 SamVal, NC_U32 EqStage,char *pName, NC_U8 Directory);
void DECODER_SDK_AutoDetect_Test_Logging( int Chn, NC_U8 VFC, char *pName);
void DECODER_SDK_AutoDetect_Test_Logging( int Chn, NC_U8 VFC, char *pName);

#endif
