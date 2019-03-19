/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: equalizer(distance)
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
#include <stddef.h>
#include <pthread.h>

#include "video_fmt_info.h"
#include "video_output.h"
#include "video_auto_detect.h"
#include "clock.h"

/**************************************************************************************
 * @desc
 * 	Function to read cable distance for EQ setting according to cable distance.(manual)
 *
 * @param_in		(unsigned char)Ch						Video Channel
 *
 * @return   	(CABLE_DISTANCE) 0				Short ( < 2M )
 * @return   	1								100M
 * @return   	2								200M
 * @return   	3								300M
 * @return   	4								400M
 * @return   	5								500M
 ***************************************************************************************/
#define MAX_EQ_COUNT 3
#define CHANNEL_COUNT 4
CABLE_DISTANCE NC_APP_VD_MANUAL_CABLE_DISTANCE_Get(unsigned char Ch, NC_VIVO_CH_FORMATDEF FmtDef )
{
	unsigned int Waiting_AGC_Stable_cnt = 0;
	unsigned char oChannel = 0;
	CABLE_DISTANCE Distance;
	unsigned char oMaxTimeCnt = 20;

	NC_VD_AUTO_HSYNC_STR Hsync_Accumulation;
	NC_VD_AUTO_SAM_STR SAM;
	NC_VD_AUTO_AGC_STR AGC;
	NC_VD_AUTO_CABLE_DIST_STR Cable_Distance;
	NC_VD_MANUAL_CABLE_DIST_STR sManualDistance;
	static int a=0,b=0,c=0,d=0;
	static unsigned int sync_ref[4][MAX_EQ_COUNT]={0,},sync_max[4]={0,},sync_max_aver,sync_min[4]={0,},sync_min_aver,sync_ref_dec[4][MAX_EQ_COUNT]={0,};

	unsigned char oDevAddr = 0x00;
	unsigned int AGC_Stable_Check = 0;

	oChannel = Ch % 4;
	oDevAddr = Ch / 4;

	SAM.Ch = oChannel;
	SAM.dev_addr = oDevAddr;
	Hsync_Accumulation.Ch = oChannel;
	Hsync_Accumulation.dev_addr = oDevAddr;
	AGC.Ch = oChannel;
	AGC.devnum = oDevAddr;
	Cable_Distance.Ch = oChannel;
	Cable_Distance.devnum = oDevAddr;

	while(1)
	{
		usleep(300*1000);

		NC_VD_AUTO_SAM_Get(oChannel, &SAM);
		NC_VD_AUTO_HSYNC_Get(oChannel, &Hsync_Accumulation);
		NC_VD_AUTO_AGC_Get(oChannel, &AGC);
		NC_VD_AUTO_ACC_GAIN_Get(Ch, ACC_GAIN_NORMAL);

		AGC_Stable_Check = NC_APP_VD_AGC_STABLE_Check(&Hsync_Accumulation, &AGC, &SAM);

		if(AGC_Stable_Check || ( Waiting_AGC_Stable_cnt >= oMaxTimeCnt ) )
		{
			/* temp  by edward */
			usleep(500*1000);
			NC_VD_AUTO_HSYNC_Get(oChannel, &Hsync_Accumulation);
			NC_VD_AUTO_AGC_Get(oChannel, &AGC);
			NC_VD_AUTO_SAM_Get(oChannel, &SAM);

			fprintf(stdout,"[APP][%d CH] Hsync 1 : %08x\n", oChannel, Hsync_Accumulation.Hsync_Accum_Val1);
			fprintf(stdout,"[APP][%d CH] Hsync 2 : %08x\n", oChannel, Hsync_Accumulation.Hsync_Accum_Val2);
			fprintf(stdout,"[APP][%d CH] Hsync Result : %08x\n", oChannel, Hsync_Accumulation.Hsync_Accum_Result);

			fprintf(stdout,"[APP][%d CH] Waiting for AGC Stable >>> %d\n", oChannel, Waiting_AGC_Stable_cnt + 1);

			if(Waiting_AGC_Stable_cnt >= oMaxTimeCnt)
			{
				fprintf(stdout,"[APP][%d CH] AGC Stable Fail\n", oChannel);
			}
			else
			{
				fprintf(stdout,"[APP][%d CH] AGC Stable Success.\n", oChannel);
				if(Hsync_Accumulation.Hsync_Accum_Result)
				{
					if((oChannel == 0) && (a < MAX_EQ_COUNT))
						sync_ref[oChannel][a++]=Hsync_Accumulation.Hsync_Accum_Result;
					else if((oChannel == 1) && (b < MAX_EQ_COUNT))
						sync_ref[oChannel][b++]=Hsync_Accumulation.Hsync_Accum_Result;
					else if((oChannel == 2) && (c < MAX_EQ_COUNT))
						sync_ref[oChannel][c++]=Hsync_Accumulation.Hsync_Accum_Result;
					else if((oChannel == 3) && (d < MAX_EQ_COUNT))
						sync_ref[oChannel][d++]=Hsync_Accumulation.Hsync_Accum_Result;

					if(((a == MAX_EQ_COUNT) && (b == MAX_EQ_COUNT) && (c == MAX_EQ_COUNT) && (d == MAX_EQ_COUNT)))
					{
						for(b=0;b<CHANNEL_COUNT;b++)
						{
							sync_min[b]=0xffffffff;
							sync_max[b]=0x00000000;
							for(a=0;a < MAX_EQ_COUNT;a++)
							{
								sync_min[b]=sync_min[b]>sync_ref[b][a]?sync_ref[b][a]:sync_min[b];
								sync_max[b]=sync_max[b]<sync_ref[b][a]?sync_ref[b][a]:sync_max[b];
							}
							printf("sync_min[%d]=%08x\n",b,sync_min[b]);
							printf("sync_max[%d]=%08x\n",b,sync_max[b]);
							sync_min_aver += sync_min[b];
							sync_max_aver += sync_max[b];
						}
						sync_min_aver = sync_min_aver/4;
						sync_max_aver = sync_max_aver/4;
						printf("sync_min_aver=%08x\n",sync_min_aver);
						printf("sync_max_aver=%08x\n",sync_max_aver);
						a=0,b=0,c=0,d=0,sync_min_aver=0,sync_max_aver=0;
					}
					else
					{
						printf("channel = %d\na = %d\n",oChannel,a);
						printf("b = %d\n",b);
						printf("c = %d\n",c);
					}		printf("d = %d\n",d);
				}
			}
			Waiting_AGC_Stable_cnt = 0;
			break;
		}

		Waiting_AGC_Stable_cnt++;
	}

	/* convert vfc to formatDefine for APP and save videoloss information */
	sManualDistance.Ch = oChannel;
	sManualDistance.FmtDef = FmtDef;
	sManualDistance.dev_addr = oDevAddr;
	sManualDistance.cabletype = 0; 		// Now, we use coaxial cable(0:coax, 1:utp, 2:reserved1, 3:reserved2

	Distance = NC_VD_MANUAL_CABLE_DISTANCE_Get(oChannel, &sManualDistance);

	return Distance;
}

