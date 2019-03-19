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
#ifndef _RAPTOR3_RAPTOR3_API_H_
#define _RAPTOR3_RAPTOR3_API_H_

#include <video_decoder.h>

void RAPTOR3_Chip_Information_Init(unsigned char* chip_id, unsigned char* chip_rev, unsigned char* chip_addr, VDEC_DEV_INFORM_S* DevInfo);

int RAPTOR3_SAL_GetFormat( RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_GetFormatEachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_GetFormat_Bank5_EachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_OnVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_NoVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );


/////////////////////////////////////// Video Channel, Port Setting //////////////////////////////////////

void RAPTOR3_KH_8M_X_25P_Rx_Test_Set( int ch );
void RAPTOR3_KH_8M_X_30P_Rx_Test_Set( int ch );

/////////////////////////////////////// Motion ///////////////////////////////////////////////////////////

void RAPTOR3_Motion_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_Pixel_All_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch,  unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_Pixel_OnOff_set(VDEC_DEV_S *pVdevDev, unsigned char ch,  unsigned char sel, unsigned char fmtdef);
unsigned char RAPTOR3_Motion_Pixel_OnOff_get(VDEC_DEV_S *pVdevDev, unsigned char ch,  unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_TSEN_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef);
void RAPTOR3_Motion_PSEN_set(VDEC_DEV_S *pVdevDev, unsigned char ch, unsigned char sel, unsigned char fmtdef);

/////////////////////////////////////// Coaxial ///////////////////////////////////////////////////////////

int RAPTOR3_Coax_FW_Upgrade_Init(VDEC_DEV_S *pVdevDev, int vi_ch);
void RAPTOR3_Coax_FW_File_Get(VDEC_DEV_S *pVdevDev, int vi_ch, char *pFileName, int select);
void RAPTOR3_Coax_FW_Upadate_Start(VDEC_DEV_S *pVdevDev, int vi_ch, int select);

int RAPTOR3_Coax_Test_Addr_Val_Get(VDEC_DEV_S *pVdevDev,int slave_addr, int bank, int addr );

#endif
