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

/* 1. Header file include --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if 0
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#endif
#include "nc_sdk_api_drv.h"
#include "nc_sdk_define.h"
#include "nc_sdk_common.h"

/* 2. Define  --------------------------------------------------------------*/
/* 3. Define variable  -----------------------------------------------------*/
/* 4. External variable & function  ----------------------------------------*/
/* 5. Function prototype  --------------------------------------------------*/
char DEVICE_NAME_USB[128] = { 0, };

/****************************************************************************************************************
 * System Upgrade
 * 1. mount   -   NC_API_SDK_USB_Mount
 * 2. unmount  -
 * 3. update file copy - NC_API_SDK_Update_File_Check
****************************************************************************************************************/
int disk_detection( char *dev_name )
{
	int ret = 0;

	int	loop_cnt = 0;
	int	read_cnt = 0;
	unsigned char line_buf[256] = {0, };
	FILE *	fp = NULL;
	char	devNameTmp[256];
	int		dev_len = strlen( dev_name );

	fp = fopen( "/proc/partitions", "r" );
	if( fp )
	{
		while( fgets((void*)&line_buf, sizeof(line_buf), fp) != NULL )
		{
			read_cnt = strlen( line_buf );
			if( read_cnt > 0 )
			{
				memset( devNameTmp, 0, 256);
				sscanf( line_buf, "%*d %*d %*d %s", devNameTmp);

				if( strncmp(devNameTmp, dev_name, dev_len) == 0 )
				{
					//fprintf( stdout, "[%s:%d] >>>>> Disk Detection OK : %s \n", __FILE__, __LINE__, dev_name );
					ret = 1;
					break;
				}
			}
			else
			{
				break;
			}
		}

		fclose( fp );
	}
	else
	{
		fprintf( stdout, "[%s:%d]:%s\n", __FILE__, __LINE__, dev_name );
	}

	return ret;
}


int NC_APP_VD_USB_Format( void )
{
	int ret = -1;
	char cmd[128] = {0, };

	sprintf(cmd, "mkfs.fat -F 32 -I %s", DEVICE_NAME_USB);
	printf("USB Format Device Name :: %s\n", DEVICE_NAME_USB);
	system(cmd);

	return 0;
}

int NC_APP_VD_USB_Mount( void )
{
	int ret = -1;
   char path[128] = {0, };

	sprintf( path, "%s", MOUNT_USB_NAME );
	if( access( path, F_OK) != 0 )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] There is no %s dir, and it will make %s\n", __func__, __LINE__,  MOUNT_USB_NAME, MOUNT_USB_NAME );
		if(mkdir(path, 0755))
   		{
	        //perror("mkdir error");
	    	fprintf(stdout, "===== mkdir error: %s=====\n", MOUNT_USB_NAME);
	    	return -1;
   		}
	}

	umount("/mnt/usb");

	ret = mount("/dev/sda1", "/mnt/usb", "vfat", 0, NULL);
	if(ret != 0)
	{
		printf("sda1 No! %d\n", ret);
		printf("ERROR:%d=%s\n",errno,strerror(errno));
		ret = mount("/dev/sda4", "/mnt/usb", "vfat", 0, NULL);
		if(ret != 0)
		{
			printf("sda4 No! %d\n", ret);
			printf("ERROR:%d=%s\n",errno,strerror(errno));
			ret = mount("/dev/sdb1", "/mnt/usb", "vfat", 0, NULL);
			if(ret != 0)
			{
				printf("sdb1 No! %d\n", ret);
				printf("ERROR:%d=%s\n",errno,strerror(errno));
				ret = mount("/dev/sda", "/mnt/usb", "vfat", 0, NULL);
				if(ret != 0)
				{
					printf("sda No! %d\n", ret);
					printf("ERROR:%d=%s\n",errno,strerror(errno));
					ret = mount("/dev/sdb", "/mnt/usb", "vfat", 0, NULL);
					if(ret != 0)
					{
						printf("sdb No! %d\n", ret);
						printf("ERROR:%d=%s\n",errno,strerror(errno));
						return -1;
					}
					else
						sprintf(DEVICE_NAME_USB, "/dev/sdb");
				}
				else
					sprintf(DEVICE_NAME_USB, "/dev/sda");
			}
			else
				sprintf(DEVICE_NAME_USB, "/dev/sdb1");
		}
		else
			sprintf(DEVICE_NAME_USB, "/dev/sda4");
	}
	else
		sprintf(DEVICE_NAME_USB, "/dev/sda1");

	return 0;
}

int NC_APP_VD_USB_Umount( void )
{
	int ret = 0;

	ret = umount("/mnt/usb");

	return ret;
}

int DECODER_SDK_Update_File_Check( void )
{
	int flag = 0;
	DIR *dir;
	struct  dirent *entry = NULL;

	const char	*fileName;
	const char raptor3_app_file[32] = "raptor3_dvr";
	const char raptor3_dvr_file[32] = "raptor3.ko";
	const char raptor4_app_file[32] = "raptor4_dvr";
	const char raptor4_dvr_file[32] = "raptor4.ko";
	char path[128] = {0, };
//	int FW_File_Cnt = 0;

	sprintf( path, "%s", "/mnt/usb" );
	dir = opendir(path);
	if(dir == NULL)
	{
		fprintf( stdout, "[%s:%d] >> Error Open Dir:%s\n",__FILE__, __LINE__, path );
		perror(" :");
		return -1;
	}
	else
	{
		/* add files in the directory */
		while( (entry=readdir(dir)) )
		{
			fileName = (char*)entry->d_name;
			if( entry->d_name[0] =='.' )
			{
				continue;
			}
			else
			{
				int	 len = strlen( fileName );
				//				fprintf( stdout, "[%s:%d]len(%d)::%s \n",__FILE__, __LINE__, len, fileName );
				if( strncmp(raptor3_app_file, fileName, len) == 0 )
				{
					fprintf( stdout, "[%s:%d] file Exist >>>> %s \n",__FILE__, __LINE__, fileName );
					system("mv /mnt/usb/raptor3_dvr /mnt/app/dvr");
					flag = 1;
				}
				if( strncmp(raptor3_dvr_file, fileName, len) == 0 )
				{
					fprintf( stdout, "[%s:%d] file Exist >>>> %s \n",__FILE__, __LINE__, fileName );
					system("mv /mnt/usb/raptor3.ko /mnt/app/ko/extdrv/nc_decoder.ko");
					flag = 1;
				}
				if( strncmp(raptor4_app_file, fileName, len) == 0 )
				{
					fprintf( stdout, "[%s:%d] file Exist >>>> %s \n",__FILE__, __LINE__, fileName );
					system("mv /mnt/usb/raptor4_dvr /mnt/app/dvr");
					flag = 1;
				}
				if( strncmp(raptor4_dvr_file, fileName, len) == 0 )
				{
					fprintf( stdout, "[%s:%d] file Exist >>>> %s \n",__FILE__, __LINE__, fileName );
					system("mv /mnt/usb/raptor4.ko /mnt/app/ko/extdrv/nc_decoder.ko");
					flag = 1;
				}
			}
		}

		sleep(1);
		if(flag)
		{
			fprintf( stdout, ">>>>> APP[%s:%d]System Restart!!\n", __func__, __LINE__ );
			system("reboot");
		}
		closedir( dir );
	}

	return 0;
}


/****************************************************************************************************************
 * StopWatch Function
 *
****************************************************************************************************************/
struct timeval* DECODER_stopwatch_start( void )
{
	struct timeval	*st_time = malloc(sizeof(struct timeval));
	gettimeofday( st_time, NULL );

	return st_time;
}

double DECODER_stopwatch_stop( struct timeval *st_time, char *str )
{
	struct timeval	ed_time;
	double			nDelay;

	if(!st_time) return 0.0;

	gettimeofday( &ed_time, NULL );

	nDelay = (ed_time.tv_sec * 1000000 + ed_time.tv_usec )					\
				 - (st_time->tv_sec * 1000000 + st_time->tv_usec );

	if( str != NULL )
	{
		fprintf(stdout, ">>%s time : %f\n", str, nDelay/1000000);
	}
	else
	{
//		fprintf(stdout, ">>stopwatch - time : %f\n", nDelay/1000000);
	}

	free(st_time);

	return nDelay;
}

/****************************************************************************************************************
 * Test
 *
****************************************************************************************************************/
void DECODER_SDK_USB_Format_Set(void)
{
	int ret = -1;

	ret = NC_APP_VD_USB_Mount();
	if(ret == 0)
	{
		ret = NC_APP_VD_USB_Umount();
		if(ret == 0)
		{
			NC_APP_VD_USB_Format();
			printf("[%s::%d]USB Format Complete!!\n", __FILE__, __LINE__);
		}
		else
		{
			while(1)
			{
				if(NC_APP_VD_USB_Umount() < 0)
				{
					sleep(1);
					continue;
				}
				else
				{
					NC_APP_VD_USB_Format();
					printf("[%s::%d]USB Format Complete!!\n", __FILE__, __LINE__);
				}
			}
		}

	}
	else
		printf("[%s::%d]USB Mount Fail!!!\n", __FILE__, __LINE__);

}

int DECODER_SDK_USB_Mount(void)
{
	int ret = -1;

	ret = NC_APP_VD_USB_Mount();

	return ret;
}

int DECODER_SDK_USB_Umount( void )
{
	int ret = 0;
	ret = NC_APP_VD_USB_Umount();

	return ret;
}

static int s_AudioSaveFlag = 0;

int DECODER_SDK_Audio_File_Save_Status(void)
{
	return s_AudioSaveFlag;
}

void *t_USB_Umount_thread( void *arg )
{

	fprintf( stdout, "Audio File Save Start");
	fflush(stdout);
	while(1)
	{
		int ret;

		if(NC_APP_VD_USB_Umount() < 0)
		{
			sleep(1);
			s_AudioSaveFlag = 0;
			fprintf( stdout, ".");
			fflush(stdout);
			continue;
		}
		else
		{
			s_AudioSaveFlag = 1;
			fprintf( stdout, "Save Complete!!\n");
			break;
		}

	}

	return 0;
}


void DECODER_SDK_USB_Umount_Wait(void)
{
	static pthread_t s_pthr_Umount;
	s_AudioSaveFlag = 0;

	pthread_create( &s_pthr_Umount, NULL, t_USB_Umount_thread, NULL );

}

int DECODER_SDK_App_Drv_Update_Check( void)
{
	int ret = -1;

	sleep(3);
	ret = DECODER_SDK_USB_Mount();
	if( ret != 0 )
	{
		DECODER_SDK_USB_Umount();
		fprintf( stdout, ">>>>> APP[%s:%d] USB Mount Error!!\n", __func__, __LINE__ );
		return -1;
	}
	else
	{
		fprintf( stdout, ">>>>> APP[%s:%d]Update File Check in USB!!\n", __func__, __LINE__ );
		ret = DECODER_SDK_Update_File_Check();

		if( ret != 0 )
		{
			DECODER_SDK_USB_Umount();
			fprintf( stdout, ">>>>> APP[%s:%d]USB Mount Error!!\n", __func__, __LINE__ );
			return -1;
		}
		else
		{
			DECODER_SDK_USB_Umount();
			fprintf( stdout, ">>>>> APP[%s:%d]USB Mount Success!!\n", __func__, __LINE__ );
			return 0;
		}
	}

	return ret;
}

void DECODER_SDK_NoVideo_Status_Logging( double tOnVideo )
{
	FILE *fp;

	char filename[512] = { 0, };
//	char error_filename[512] = { 0, };

	sprintf(filename, "/mnt/usb/test_log.txt");

	fp  = fopen(filename, "a+");

	fprintf(fp,"[OFF]On Video time(%f)\n", tOnVideo);
	fprintf(fp, "\n\n");

	fclose(fp);

}

void DECODER_SDK_OnVideo_Distance_Logging( double tNoVideo, int Cam_Seq, char *Str )
{
	static int TestCnt = 0;
	int TestSeq = 0;

	FILE *fp_sam;

	char filename_sam[512] = {0, };

	NC_U32 hsync_distortion_falling;
	NC_U32 hsync_distortion_rising;
	NC_U8 hsync_7_0   = 0;
	NC_U8 hsync_15_8  = 0;
	NC_U8 hsync_23_16 = 0;
	NC_U8 hsync_31_24 = 0;

	unsigned char mask = 0;
	unsigned char bitSAM = 0x01;
	unsigned char bitDMU = 0x02;

	sprintf(filename_sam, "/mnt/usb/%s_log.txt", Str);
	fp_sam   = fopen(filename_sam, "a+");

	mask = DECODER_Register_Data_Get( 0x60, 0x13, 0xBC );

	TestCnt++;
	printf("TestCnt::%d\n", TestCnt);
	fprintf(fp_sam, "================= Start::%d =================\n", TestCnt);
	while(1)
	{
		if(mask & bitSAM)
		{
			DECODER_Register_Data_Set(0x60, 0x13, 0x2b, 0x00);

			fprintf(fp_sam, "%x, %x/%x, ", DECODER_Register_Data_Get( 0x60, 0x13, 0x90 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0x91 ), DECODER_Register_Data_Get( 0x60, 0x13, 0x92 ) );
			fprintf(fp_sam, "%x, %x/%x, ", DECODER_Register_Data_Get( 0x60, 0x13, 0xc4 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc8 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xc9 ) );

			hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd0 );
			hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd1 );
			hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd2 );
			hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd3 );
			hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

			hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd4 );
			hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd5 );
			hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd6 );
			hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd7 );
			hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);
			fprintf(fp_sam, "%x, %x, %x, ",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);

			hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd8 );
			hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd9 );
			hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xda );
			hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xdb );
			hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

			hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xdc );
			hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xdd );
			hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xde );
			hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xdf );
			hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);
			fprintf(fp_sam, "%x, %x, %x\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);
			break;
		}
		else
		{
//			printf("CH1_SAM_DONE Check Error:: %02X, RetryCnt_%d\n", mask, TestSeq);
			printf("CH1_SAM_DONE Check Error:: %02X, RetryCnt_%d\n", DECODER_Register_Data_Get(0x60, 0x00, 0xe0), TestSeq);
			fprintf(fp_sam, "CH1_SAM_DONE Check Error:: %02X, RetryCnt_%d\n", mask, TestSeq );

			if(TestSeq >= 10)
			{
				break;
			}

			usleep(40 * 1000);
		}

		TestSeq++;
	}
	fprintf(fp_sam, "================= End::%d =================\n", TestCnt);
	fprintf(fp_sam, "\n");
	fclose(fp_sam);
}

void DECODER_SDK_OnVideo_Status_Logging( double tNoVideo, int Cam_Seq, char *Str )
{
	FILE *fp;
	FILE *fp_error;
	FILE *fp_sam;

	static int TestSeq = 0;

	char filename[512] = { 0, };
	char error_filename[512] = { 0, };
	char filename_sam[512] = {0, };

	unsigned char mask = 0;

	unsigned char bitSAM = 0x01;
	unsigned char bitDMU = 0x02;

	sprintf(filename, "/mnt/usb/test_log.txt");
	sprintf(error_filename, "/mnt/usb/test_error_log.txt");
	sprintf(filename_sam, "/mnt/usb/%s_log.txt", Str);

	fp 		 = fopen(filename, "a+");
	fp_error = fopen(error_filename, "a+");
	fp_sam   = fopen(filename_sam, "a+");

	fprintf(fp,"[ON]No Video time(%f) IspSeq(%d)\n", tNoVideo, Cam_Seq);
	fprintf(fp, "1. Format[0x%02X], [%s]\n", DECODER_Register_Data_Get( 0x60, 0x05, 0xF0 ),  Str );
	fprintf(fp, "2. VFC Status[0x%02X], Score[0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x05, 0xF1 ),  DECODER_Register_Data_Get( 0x60, 0x05, 0xF6 ) );
	fprintf(fp, "3. H/V cnt : H[0x%02X/0x%02X], V[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x05, 0xF3 ), DECODER_Register_Data_Get( 0x60, 0x05, 0xF2 ), \
															  	  	 DECODER_Register_Data_Get( 0x60, 0x05, 0xF5 ), DECODER_Register_Data_Get( 0x60, 0x05, 0xF4 ) \
															  	  	 );
	fprintf(fp, "4. VBI : AHD[0x%02X/0x%02X], CVI[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xA5 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xA4 ), \
																	 DECODER_Register_Data_Get( 0x60, 0x13, 0xA7 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xA6 ) \
														  	  	 	 );
	fprintf(fp, "5. LAST : [0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xA9 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xA8 ) );
	fprintf(fp, "6. SYNC : MAX[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xAB ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xAA ) );
	fprintf(fp, "7. VFC_ACC : MAX[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xc6 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc7 ) );

	usleep(1000 * 1000); // 7 Frame (Sleep)

	mask = DECODER_Register_Data_Get( 0x60, 0x13, 0xBC );

	NC_U32 hsync_distortion_falling;
	NC_U32 hsync_distortion_rising;
	NC_U8 hsync_7_0   = 0;
	NC_U8 hsync_15_8  = 0;
	NC_U8 hsync_23_16 = 0;
	NC_U8 hsync_31_24 = 0;

	printf("\n");
	printf("========= Test Count :: %d =========\n", TestSeq);
	if( (mask & bitSAM) && (mask & bitDMU) )
	{
//		NC_U32 hsync_distortion_sum;

		fprintf(fp, "========= Test Count :: %d =========\n", TestSeq );
//		fprintf(fp_sam, "Test Count :: %d, ", TestSeq );

		printf("DMU : AVG[0x%02X], TOTAL[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0x90 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0x91 ), DECODER_Register_Data_Get( 0x60, 0x13, 0x92 ) );
		printf("SAM : SAM_AVG[0x%02X], SAM_DIV[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xc4 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc8 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xc9 ));

		fprintf(fp, "8. DMU : AVG[0x%02X], TOTAL[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0x90 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0x91 ), DECODER_Register_Data_Get( 0x60, 0x13, 0x92 ) );
		fprintf(fp, "9. SAM : SAM_AVG[0x%02X], SAM_DIV[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xc4 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc8 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xc9 ) );
		fprintf(fp_sam, "%x, %x/%x, ", DECODER_Register_Data_Get( 0x60, 0x13, 0x90 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0x91 ), DECODER_Register_Data_Get( 0x60, 0x13, 0x92 ) );
		fprintf(fp_sam, "%x, %x/%x, ", DECODER_Register_Data_Get( 0x60, 0x13, 0xc4 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc8 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xc9 ) );


		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd0 );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd1 );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd2 );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd3 );
		hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd4 );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd5 );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd6 );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd7 );
		hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

		fprintf(fp, "1frame[%x], 1frame_F[%x], 1frame_R[%x]\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);
		fprintf(fp_sam, "%x, %x, %x, ",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd8 );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd9 );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xda );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xdb );
		hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xdc );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xdd );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xde );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xdf );
		hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);
		fprintf(fp, "8frame[%x], 8frame_F[%x], 8frame_R[%x]\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);
		fprintf(fp_sam, "%x, %x, %x\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);

	}
	else if( (mask & bitSAM) && !(mask & bitDMU) )
	{
		fprintf(fp, "========= Test Count :: %d =========\n", TestSeq );
		printf("!!!!!!!!!!!!!!! DMU Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq);
		printf("9. SAM : SAM_AVG[0x%02X], SAM_DIV[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xc4 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc8 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xc9 ));

		fprintf(fp, "!!!!!!!!!!!!!!! DMU Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq );
		fprintf(fp, "9. SAM : SAM_AVG[0x%02X], SAM_DIV[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0xc4 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0xc8 ), DECODER_Register_Data_Get( 0x60, 0x13, 0xc9 ) );

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd0 );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd1 );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd2 );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd3 );
		hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd4 );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd5 );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd6 );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xd7 );
		hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

		printf("1frame[%x], 1frame_F[%x], 1frame_R[%x]\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);
		fprintf(fp, "1frame[%x], 1frame_F[%x], 1frame_R[%x]\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xd8 );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xd9 );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xda );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xdb );
		hsync_distortion_falling = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);

		hsync_7_0   = DECODER_Register_Data_Get( 0x60, 0x13, 0xdc );
		hsync_15_8  = DECODER_Register_Data_Get( 0x60, 0x13, 0xdd );
		hsync_23_16 = DECODER_Register_Data_Get( 0x60, 0x13, 0xde );
		hsync_31_24 = DECODER_Register_Data_Get( 0x60, 0x13, 0xdf );
		hsync_distortion_rising = ((hsync_31_24 << 24) | (hsync_23_16 << 16) | (hsync_15_8 << 8) | hsync_7_0);
		printf("8frame[%x], 8frame_F[%x], 8frame_R[%x]\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);
		fprintf(fp, "8frame[%x], 8frame_F[%x], 8frame_R[%x]\n",(hsync_distortion_falling - hsync_distortion_rising), hsync_distortion_falling, hsync_distortion_rising);

	}
	else if( !(mask & bitSAM) && (mask & bitDMU) )
	{
		fprintf(fp, "========= Test Count :: %d =========\n", TestSeq );
		fprintf(fp, "!!!!!!!!!!!!!!! SAM Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq );
		printf("!!!!!!!!!!!!!!! SAM Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq);

		fprintf(fp, "8. DMU : AVG[0x%02X], TOTAL[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0x90 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0x91 ), DECODER_Register_Data_Get( 0x60, 0x13, 0x92 ) );
		printf("8. DMU : AVG[0x%02X], TOTAL[0x%02X/0x%02X]\n", DECODER_Register_Data_Get( 0x60, 0x13, 0x90 ),  DECODER_Register_Data_Get( 0x60, 0x13, 0x91 ), DECODER_Register_Data_Get( 0x60, 0x13, 0x92 ));
	}
	else
	{
		printf("!!!!!!!!!!!!!!! SAM Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq);
		printf("!!!!!!!!!!!!!!! DMU Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq);
		fprintf(fp, "!!!!!!!!!!!!!!! SAM, DMU Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq );
		fprintf(fp_sam, "!!!!!!!!!!!!!!! SAM, DMU Check Error :: %d !!!!!!!!!!!!!!\n", TestSeq );
	}


	if(Cam_Seq == 0)
		fprintf(fp_error, "\n");

	fprintf(fp_error, "%d ", Cam_Seq);

	TestSeq++;

	printf("[%s::%d]OnVide_Test_Log_Save!!\n", __func__, __LINE__);
	printf("\n");

	fclose(fp);
	fclose(fp_error);
	fclose(fp_sam);
}

void DECODER_SDK_Distance_Test_Logging( int Chn, NC_U32 SamVal, NC_U32 EqStage,char *pName, NC_U8 Directory)
{
	FILE *fp;
	char filename[512] = { 0, };

	if( Directory == 0 )
		sprintf(filename, "/mnt/usb/Chn%d_%s_EQ_log.txt", Chn, pName);
	else if( Directory == 1 )
		sprintf(filename, "/mnt/nfs/Chn%d_%s_EQ_log.txt", Chn, pName);
	else
		sprintf(filename, "/mnt/usb/Chn%d_%s_EQ_log.txt", Chn, pName);

	int Cam_Seq = 0;

	if(DECODER_SDK_USB_Mount() == 0)
	{
		fp = fopen(filename, "a+");

		fprintf(fp, "%x, %d\n", SamVal, EqStage);

		fclose(fp);

		DECODER_SDK_USB_Umount();
		printf("Raptor4_SDK_Distance_Test_Logging!!\n");
	}
	else
	{
		printf("Test Logging Error!! Can not find USB Disk!!\n");
	}

}

void DECODER_SDK_AutoDetect_Test_Logging( int Chn, NC_U8 VFC, char *pName)
{
	FILE *fp;
	char filename[512] = { 0, };
	sprintf(filename, "/mnt/usb/Chn%d_AutoDectec_log.txt", Chn);

	int Cam_Seq = 0;

	if(DECODER_SDK_USB_Mount() == 0)
	{
		printf("[%s::%d]VideoFmt::%s\n", __func__, __LINE__, pName);

		fp = fopen(filename, "a+");

		DECODER_OnlyTest_VFC_To_CameraSeq_Get( VFC, &Cam_Seq );

		if(Cam_Seq == 0)
			fprintf(fp, "\n");

		fprintf(fp, "%d ", Cam_Seq);

		fclose(fp);

		DECODER_SDK_USB_Umount();
	}
	else
	{
		printf("Test Logging Error!! Can not find USB Disk!!\n");
	}

}
