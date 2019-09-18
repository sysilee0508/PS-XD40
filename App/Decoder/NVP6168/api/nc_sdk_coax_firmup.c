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
#include <dirent.h>

#include "nc_sdk_api_drv.h"
#include "nc_sdk_define.h"
#include "nc_sdk_coax_firmup.h"

static pthread_t s_pthr_FirmUP_Start;

static int s_FW_File_Size = 0;
static int s_FW_Packet_Size = 0;
char g_FW_Status_String[1024] = {0, };

static int s_Progress_Start_flag = 0;
static int s_percentage = 0;


static nc_coax_firmup_s 	s_fileinfo;
FIRMWARE_FILE_DEC_INFO *Detect_FW_File;       // Memory Allocation

/**************************************************************************************
 * @desc
 * JAGUAR1's   Register the firmware update file size...Not used
 *
 * @param_in		(int)size			                            FirmUP file size
 *
 * @return   	void  		       								None
 *
 ***************************************************************************************/
void NC_APP_VD_COAX_FirmUP_FW_File_Size_Set(int size)
{
	s_FW_File_Size = size;
}

/**************************************************************************************
 * @desc
 * JAGUAR1's   Returns firmware update file size...Not used
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								FW File size
 *
 ***************************************************************************************/
int NC_APP_VD_COAX_FirmUP_FW_File_Size_Get(void)
{
	return s_FW_File_Size;
}

/**************************************************************************************
 * @desc
 * JAGUAR1's   Register the firmware update one packet size...Not used
 *
 * @param_in		(int)size			                            one packet size
 *
 * @return   	void  		       								None
 *
 ***************************************************************************************/
void NC_APP_VD_COAX_FirmUP_FW_Packet_Size_Set(int size)
{
	s_FW_Packet_Size = size;
}

/**************************************************************************************
 * @desc
 * JAGUAR1's   Returns firmware update one packet size...Not used
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								one packet size
 *
 ***************************************************************************************/
int NC_APP_VD_COAX_FirmUP_FW_Packet_Size_Get(int size)
{
	return s_FW_Packet_Size;
}

/**************************************************************************************
 * @desc
 * 	JAGUAR1's   Returns firmware update file size...Not used
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								FW File size
 *
 ***************************************************************************************/
void __NC_APP_VD_COAX_FirmUP_UI_Status_Set(char *pString)
{
	memset( g_FW_Status_String, 0, sizeof(g_FW_Status_String));
	sprintf( g_FW_Status_String, "%s",pString );
}

/**************************************************************************************
 * @desc
 * 	JAGUAR1's   Returns firmware update status
 * 	            return 0 : Ready or start checking
 * 	            return 1 : Data Sending..Progress Bar Update in GUI
 * 	            return 2 : FirmUP End..Switch the GUI screen.
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								FW File size
 *
 ***************************************************************************************/
int NC_APP_VD_COAX_FirmUP_Status_Get(char *pString)
{
	memcpy(pString, g_FW_Status_String, sizeof(g_FW_Status_String));
	return s_Progress_Start_flag;
}

/**************************************************************************************
 * @desc
 * 	JAGUAR1's   Register the firmware update data transmission rate 0 ~ 100%
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								FW File size
 *
 ***************************************************************************************/
void NC_APP_VD_COAX_FirmUP_ProgressBar_Status_Set(int percentage)
{
	s_percentage = percentage;
}

/**************************************************************************************
 * @desc
 * 	JAGUAR1's   Returns firmware update data transmission rate
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								data transmission rate 0 ~ 100%
 *
 ***************************************************************************************/
int NC_APP_VD_COAX_FirmUP_ProgressBar_Status_Get(void)
{
	return s_percentage;
}



/**************************************************************************************
 * @desc
 * 	JAGUAR1's   USB Mount
 *
 * @param_in		void			                                None
 *
 * @return   	int  		       								Success or Error
 *
 ***************************************************************************************/
int __NC_API_COAX_FirmUP_USB_Mount( void )
{
	char path[128] = {0, };
	char cmd[64] = {0, };

	/* If there is /mnt/usb directory, make /mnt/usb */
	sprintf( path, "%s", MOUNT_USB_NAME );
	if( access( path, F_OK) != 0 )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] There is no %s dir, and it will make %s\n", __func__, __LINE__,  MOUNT_USB_NAME, MOUNT_USB_NAME );
		if(mkdir(path, 0755))
		{
			//perror("mkdir error");
			fprintf(stdout, "===== mkdir error: %s=====\n", MOUNT_USB_NAME);
			return COAX_FW_FAILURE;
		}
	}

	/* mount USB directory */
	sprintf(cmd,"/bin/mount /dev/sda1 /mnt/usb >& /dev/null");
	if(0!=system(cmd))
	{
		sprintf(cmd,"/bin/mount /dev/sda4 /mnt/usb >& /dev/null");
		if(0!=system(cmd))
		{
			sprintf(cmd,"/bin/mount /dev/sdb1 /mnt/usb >& /dev/null");
			//system(cmd);
			if(0!=system(cmd))
			{
				sprintf(cmd,"/bin/mount /dev/sda /mnt/usb >& /dev/null");
				system(cmd);
			}
		}
	}

	return COAX_FW_SUCCESS;
}

/**************************************************************************************
 * @desc
 * 	JAGUAR1's   USB Unmount
 *
 * @param_in		void			                                None
 *
 * @return   	void  		       								None
 *
 ***************************************************************************************/
void __NC_APP_VD_COAX_FirmUP_USB_Umount( void )
{
	char cmd[128] = {0, };

	sprintf( cmd,"/bin/umount /mnt/usb" );
	system( cmd );
}

/*******************************************************************************
 *  Description    : Memory allocation by the number of firmware update files
 *  Argurments     : char *path - Firmware update file path
 *                   int file_cnt - Number of firmware update files
 *  Return value   : 0 Success
 *                   -1 Fail
 *  Modify         :
 *  warning        :
 *******************************************************************************/

/**************************************************************************************
 * @desc
 * 	JAGUAR1's   Memory allocation by the number of firmware update files
 *
 * @param_in		void			                                None
 *
 * @return   	void  		       								None
 *
 ***************************************************************************************/
int __NC_APP_VD_COAX_FirmUP_File_Memory_Alloc( char *path, int file_cnt )
{
	int ii = 0;
	DIR *dir;
	struct  dirent *entry = NULL;
	char  *fileName;
//	char cmd[64]     = {0, };
//	unsigned char fileType[8] = "nuf";
	char fileType[8] = "nuf";

	Detect_FW_File = (FIRMWARE_FILE_DEC_INFO*) malloc(sizeof(FIRMWARE_FILE_DEC_INFO)*file_cnt);
	memset(Detect_FW_File, 0, file_cnt);

	/* open directory, and check file whether update file exist or not */
	dir = opendir(path);
	if(dir == NULL)
	{
		fprintf( stdout, "%s:%d >> Error Open Dir:%s\n",__FILE__, __LINE__, path );
		perror(" :");
		return COAX_FW_FAILURE;
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
//				char extn[4]={0,};
				char extn[4]={0,};
				int	 len = strlen( fileName );
				strncpy( extn, fileName+(len-3), 3 );
				if( strncmp(extn, fileType, 3) == 0 )
				{
					strncpy( Detect_FW_File[ii].firmup_fileName, fileName, len);
					Detect_FW_File[ii].strlen = len;
					ii++;
				}
			}
		}
		closedir( dir );
	}
	return COAX_FW_SUCCESS;
}

/*******************************************************************************
 *  Description    : Memory allocation Free
 *  Argurments     : void
 *  Return value   : void
 *  Modify         :
 *  warning        :
 *******************************************************************************/
void __NC_APP_VD_COAX_FirmUP_File_Memory_Alloc_Free( void )
{
	free(Detect_FW_File);
}

/*******************************************************************************
 *  Description    : F/W Update File Count
 *  Argurments     : void
 *  Return value   : void
 *  Modify         :
 *  warning        :
 *******************************************************************************/
int NC_APP_VD_COAX_FirmUP_File_Count_Get( char *Path )
{
	DIR *dir;
	struct  dirent *entry = NULL;

	char	*fileName;
//	unsigned char cmd[64] = {0, };
	char fileType[8] = "nuf";
	char path[128] = {0, };
	int FW_File_Cnt = 0;

#if 1
	if( __NC_API_COAX_FirmUP_USB_Mount() != COAX_FW_SUCCESS )
	{
		return COAX_FW_FAILURE;
	}
#endif

	sprintf( path, "%s", Path );

	dir = opendir(path);
	if(dir == NULL)
	{
		fprintf( stdout, "%s:%d >> Error Open Dir:%s\n",__FILE__, __LINE__, path );
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
				char extn[4]={0,};
				int	 len = strlen( fileName );
				strncpy( extn, fileName+(len-3), 3 );
				if( strncmp(extn, fileType, 3) == 0 )
				{
					FW_File_Cnt++;
				}
			}
		}

		closedir( dir );
	}

	if( FW_File_Cnt != 0 )
		__NC_APP_VD_COAX_FirmUP_File_Memory_Alloc( path, FW_File_Cnt );

	return FW_File_Cnt;

}

/*******************************************************************************
 *  Description    : F/W Update File Select
 *  Argurments     : void
 *  Return value   : void
 *  Modify         :
 *  warning        :
 *******************************************************************************/
void NC_APP_VD_COAX_FirmUP_File_Select_Get( char *pfileName, int fileNo )
{
	int len = Detect_FW_File[fileNo].strlen;
	strncpy(pfileName, Detect_FW_File[fileNo].firmup_fileName, len);
}

int __NC_APP_VD_COAX_FirmUP_File_Size_Get( void *ctx, char *dir_name)
{
	struct stat fstat;
	char cmd[256] = {0, };
	char *pfirmware_file = dir_name;

	/* check file size */
	sprintf( cmd, "%s/%s", "/mnt/usb", pfirmware_file );
	lstat( cmd, &fstat);
	if( fstat.st_size == 0 )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] There is file:%s, firmware update size is 0\n", __func__, __LINE__, cmd );
		return ACP_FIRMUP_FWFILE_IS_ZERO;
	}
	return 0;
}

/*******************************************************************************
 *  description    : get firmware file information
 *  argurments     : pFile(File Pointor), firmware_name(Camera firmware name),
 * 					p_param(file management)
 *  return value   : 0(success), -1(failed)
 *  modify         :
 *  warning        :
 *******************************************************************************/
int __NC_APP_VD_COAX_FirmUP_File_information_Get( FILE *pFile, char *firmware_name, void *p_param )
{
//	int i = 0;
	int ret = 0;
	struct stat fstat;
//	unsigned int filesum = 0;
//	unsigned int readsize = 0;
//	unsigned char firmwarebuf[512*1024] = {0, };
	nc_coax_firmup_s *pstFileInfo = (nc_coax_firmup_s*)p_param;


	/* check firmware file open */
	if( pFile == NULL )
	{
		fprintf( stdout, ">>>>> APP[%s:%d] Failed, Firmware file open\n", __func__, __LINE__ );
		return -1;
	}

	/*
	 *  get file status
	 *   - filename("FW_EAGLET2P5_STD_VER_01_00_00_SFLASH_UPDATE_20160624A.nc")
	 *   - filePullname("FW_EAGLET2P5_STD_VER_01_00_00_SFLASH_UPDATE_20160624A.nc"
	 *   - currentpacketnum : currnet packet number
	 *   - filepacketnum : number of file packet(128bytes)
	 *   - onepacketbuf : buffer buffer of one packet
	 */
	memset( pstFileInfo, 0x00, sizeof(nc_coax_firmup_s));
	sprintf( pstFileInfo->filename, "%s", firmware_name );
	//	sprintf( pstFileInfo->filePullname, "/%s/%s", MOUNT_USB_NAME, firmware_name );
	sprintf( pstFileInfo->filePullname, "/%s/%s", MOUNT_USB_NAME, firmware_name );
	lstat( pstFileInfo->filePullname, &fstat);
	pstFileInfo->filesize = fstat.st_size;
	pstFileInfo->currentpacketnum = 0;
	pstFileInfo->filepacketnum = (pstFileInfo->filesize / ONE_PACKET_MAX_SIZE );
	pstFileInfo->filepacketnum += (pstFileInfo->filesize % ONE_PACKET_MAX_SIZE ) ? 1 : 0;

	NC_APP_VD_COAX_FirmUP_FW_File_Size_Set( pstFileInfo->filesize );
	NC_APP_VD_COAX_FirmUP_FW_Packet_Size_Set( pstFileInfo->filepacketnum );

	/* move point to start of file */
	fseek( pFile, 0, SEEK_SET );

#define TEST_DEBUG_PRINT
#ifdef TEST_DEBUG_PRINT // Debug
	printf( "=======================================================\n");
	printf( "    ACP[ISP FIRMWARE UPDATE, FILE INFORMATION\n");
	printf( "=======================================================\n");
	printf( "1.file name       : %s\n",   pstFileInfo->filename );
	printf( "2.file pull name  : %s\n",   pstFileInfo->filePullname );
	printf( "3.file size       : %d\n",   pstFileInfo->filesize);
	printf( "5.file packet num : %d\n",   pstFileInfo->filepacketnum );
	printf( "=======================================================\n");
#endif

	return ret;
}

void __NC_APP_VD_COAX_FirmUP_Fail(FILE *pFile)
{
	s_fileinfo.result = COAX_FW_FAILURE;

	if( pFile != NULL )
		fclose(pFile);

	__NC_APP_VD_COAX_FirmUP_File_Memory_Alloc_Free();
	__NC_APP_VD_COAX_FirmUP_USB_Umount();
}


void __NC_APP_VD_COAX_FirmUP_Logical_Flag_Init( void )
{
	s_Progress_Start_flag = 0;
	s_percentage = 0;
}


int __NC_APP_VD_COAX_FirmUP_ACP_Header_Check(void *p_param)
{
	int ret = COAX_FW_FAILURE;
	int timeout_cnt = 0;
	char FW_Status[1024] = {0,};

	nc_coax_firmup_s *pstFileInfo = (nc_coax_firmup_s*)p_param;
	unsigned int ch = pstFileInfo->channel;

	nc_decoder_s stFWInfo;
	stFWInfo.Chn = pstFileInfo->channel;

	// ACP Header Check 0x55
	while( timeout_cnt++ < COAX_FW_TIMEOUT )
	{
		NC_API_DRV_Coax_FW_ACP_Header_Get( &stFWInfo );
		if(stFWInfo.result == COAX_FW_SUCCESS)
		{
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[ACP Check]Header Check Success!!!");
			ret = COAX_FW_SUCCESS;
			break;
		}
		else
		{
			sprintf(FW_Status, "ACP Header Check Fail!!..Recv Retry: %d", timeout_cnt);
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set(FW_Status);
			fprintf( stdout, ">>>>> APP[%s:%d]Ch:%d FW_Ready_Check_Retry::%d\n", __func__, __LINE__, ch, timeout_cnt);
			ret = COAX_FW_FAILURE;
			if( timeout_cnt == COAX_FW_TIMEOUT )
			{
				__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[ACP Check]Header Check Fail - ACK Recv Time out!!");
				ret = COAX_FW_FAILURE;
				break;
			}
		}
		sleep(1);
	}

	return ret;
}

int __NC_APP_VD_COAX_FirmUP_Ready_Check(void *p_param)
{
	int ret = COAX_FW_FAILURE;
	int timeout_cnt = 0;
	char FW_Status[1024] = {0,};

	nc_coax_firmup_s *pstFileInfo = (nc_coax_firmup_s*)p_param;
	unsigned int ch = pstFileInfo->channel;

	nc_decoder_s stFWInfo;
	stFWInfo.Chn            = pstFileInfo->channel;

	NC_API_DRV_Coax_FW_Ready_CMD_Set( &stFWInfo );
	if( stFWInfo.result == 0 )
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[Ready_Send]ACP FIRMUP Ready Command!");

		sleep(3);

		nc_decoder_s stVD_Video;
		stVD_Video.Chn	   = 0;
		stVD_Video.OP_Mode = NC_VIDEO_SET_MODE_MANUAL;
		NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);

		nc_decoder_s pstVD_Video;
		pstVD_Video.Chn 			 = stFWInfo.Chn ;
		pstVD_Video.VideoCable  	 = CABLE_3C2V;
		pstVD_Video.FormatStandard   = FMT_AHD;
		pstVD_Video.FormatResolution = FMT_1080P;
		pstVD_Video.FormatFps		 = FMT_25P;
		pstVD_Video.VideoSetMode     = NC_VIDEO_SET_MODE_MANUAL;
		NC_API_DRV_Video_Input_Manual_Set(&pstVD_Video);

		while( timeout_cnt++ < COAX_FW_TIMEOUT  )
		{
			/* 2) ACK Receive From ISP...FW Ready OK */
			NC_API_DRV_Coax_FW_Ready_ACK_Get( &stFWInfo );

			if(stFWInfo.result == 0)
			{
				__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[Ready_ACK]ACP FIRMUP Ready ACK Recv OK!!");
				fprintf( stdout, ">>>>> APP[%s:%d]Ch:%d FW_Ready_Check_OK!!\n", __func__, __LINE__, ch );
				ret = COAX_FW_SUCCESS;
				break;
			}
			else
			{
				sprintf(FW_Status, "[Ready_ACK]ACP FIRMUP Ready ACK Recv Retry: %d", timeout_cnt);
				__NC_APP_VD_COAX_FirmUP_UI_Status_Set(FW_Status);
				fprintf( stdout, ">>>>> APP[%s:%d]Ch:%d FW_Ready_Check_Retry::%d\n", __func__, __LINE__, ch, timeout_cnt);
				if( timeout_cnt == COAX_FW_TIMEOUT )
				{
					__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[Ready_ACK]ACP FIRMUP Ready ACK Recv Fail - ACK Recv Time out!!");
					ret = COAX_FW_FAILURE;
					break;
				}
				sleep(1);
			}
		}
	}
	else
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[FAIL]ACP FIRMUP Ready - NOT SUPPORT!!");
		ret = COAX_FW_FAILURE;
	}
	sleep(1);

	return ret;
}

int __NC_APP_VD_COAX_FirmUP_Start_Check(void *p_param)
{
	int ret = COAX_FW_FAILURE;
	int timeout_cnt = 0;
	char FW_Status[1024] = {0,};

	nc_coax_firmup_s *pstFileInfo = (nc_coax_firmup_s*)p_param;
	unsigned int ch = pstFileInfo->channel;


	nc_decoder_s stFWInfo;
	stFWInfo.Chn            = pstFileInfo->channel;


	NC_API_DRV_Coax_FW_Start_CMD_Set( &stFWInfo );
	__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[Start_Send]FW Start Command Send!");

	sleep(2);


	while(timeout_cnt++ < COAX_FW_TIMEOUT)
	{
		NC_API_DRV_Coax_FW_Start_ACK_Get( &stFWInfo );
		if( stFWInfo.result == 0 )
		{
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[Start_ACK]ACP FIRMUP Start ACK Recv OK!!");
			fprintf( stdout, ">>>>> APP[%s:%d](CH:%d)FW_Start_Check::OK\n", __func__, __LINE__, ch );
			ret = COAX_FW_SUCCESS;
			break;
		}
		else
		{
			sprintf(FW_Status, "[Start_ACK]ACP FIRMUP Start ACK Recv Retry: %d", timeout_cnt);
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set(FW_Status);
			fprintf( stdout, ">>>>> APP[%s:%d](CH:%d)FW_Start_Check::Retry:%d\n", __func__, __LINE__, ch, timeout_cnt );
			if( timeout_cnt == COAX_FW_TIMEOUT )
			{
				__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[Start_ACK]ACP FIRMUP Start ACK Recv FAIL - ACK Recv Time out!!");
				ret = COAX_FW_FAILURE;
				break;
			}
			sleep(1);
		}
	}

	return ret;
}

int __NC_VD_COAX_Firmware_Update_Data_Send( FILE *pFile, void *p_param )
{
	int ret = 0;
	int ch = 0;
	int ii = 0;
	int retrycnt = 0;
	unsigned int  itotalsize = 0;
	unsigned int  onepacketsize = 0;
	unsigned int  fileoffset = 0;
	unsigned int  readsize = 0;
	unsigned int  currentPacketNum = 0;
	char FW_Update[1024] = {0, };
	unsigned char onepacketbuf[256] = {0, };
	int timeoutcnt = 100;

	float current_num;
	float total_num;
	int percentage = 0;

	nc_coax_firmup_s *pstFileInfo = (nc_coax_firmup_s*)p_param;

	nc_decoder_s stFWInfo;
	stFWInfo.Chn            = pstFileInfo->channel;
	stFWInfo.filepacketnum  = pstFileInfo->filepacketnum;

	ch = pstFileInfo->channel;
	s_Progress_Start_flag = 1;

	/* move point to start of file */
	fseek( pFile, 0, SEEK_SET );

	/* set total size of file */
	itotalsize = pstFileInfo->filesize;

	fprintf( stdout, ">>>>> APP[%s:%d] CH:%d, Camera UPDATE loading.... wait please!\n", __func__, __LINE__, ch );

	/* get firmware data( 128bytes ) */
	while( itotalsize != 0)
	{
		retrycnt = 0;

		/* decision one packet size */
		onepacketsize = ( itotalsize > ONE_PACKET_MAX_SIZE ) ? ONE_PACKET_MAX_SIZE : itotalsize;

		/* read one packet */
		readsize = fread( onepacketbuf, 1, onepacketsize, pFile );
		if( 0 > onepacketsize )
		{
			printf(">>>>> APP[%s:%d] failed: read data : %d\n", __func__, __LINE__, readsize );
			ret = COAX_FW_FAILURE;
			break;
		}

		memcpy( stFWInfo.onepacketbuf, onepacketbuf, readsize );
		stFWInfo.currentpacketnum  = currentPacketNum++;
		stFWInfo.currentFileOffset = fileoffset;
		stFWInfo.readsize 		   = readsize;

		NC_API_DRV_Coax_FW_OnePacketData_Set( &stFWInfo );

		for( ii=0; ii<timeoutcnt; ii++ )
		{
			NC_API_DRV_Coax_FW_OnePacketData_ACK_Get( &stFWInfo );

			if( stFWInfo.result == 0 )
			{
				sprintf(FW_Update, "Firmware Update[%d/%d]offset[0x%06X/%d] : Retry[%d] ",
						stFWInfo.currentpacketnum+1,
						stFWInfo.filepacketnum,
						stFWInfo.currentFileOffset,
						stFWInfo.currentFileOffset,
						retrycnt);
				__NC_APP_VD_COAX_FirmUP_UI_Status_Set(FW_Update);
				break;
			}
			else
			{
				usleep(30*1000);
				retrycnt++;
			}
		}

		current_num = stFWInfo.currentpacketnum;
		total_num   = stFWInfo.filepacketnum;
		percentage  = (current_num/total_num) * 100;
		NC_APP_VD_COAX_FirmUP_ProgressBar_Status_Set( percentage );

		if( retrycnt >= timeoutcnt )
		{

			/* debug message */
			for( ii = 0; ii < 139; ii++ )
			{
				if( (ii!=0) && ((ii%16)==0) )
					printf("\n");
				printf("%02x ", pstFileInfo->onepacketbuf[ii] );
			}
			printf("\n");
			printf(">>>>> DRV[%s:%d] CH:%d, write timeout:%d, offset of file:0x%06X\n", __func__, __LINE__, ch, timeoutcnt, stFWInfo.currentFileOffset );
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[FAIL]ACP FIRMUP Data Send - ACK Recv Time out!!");
			ret = COAX_FW_FAILURE;
			return ret;
		}

		/* adjust offset and size to read */
		itotalsize -= readsize;
		fileoffset += readsize;
	}

	NC_APP_VD_COAX_FirmUP_ProgressBar_Status_Set(100);

	ret = COAX_FW_SUCCESS;
	pstFileInfo->result = ret;

	return ret;
}

int __NC_APP_VD_COAX_FirmUP_End_Check(void *p_param)
{
	int ret = COAX_FW_FAILURE;
	int timeout_cnt = 0;
	char FW_Status[1024] = {0,};

	nc_coax_firmup_s *pstFileInfo = (nc_coax_firmup_s*)p_param;
	unsigned int ch = pstFileInfo->channel;

	nc_decoder_s stFWInfo;
	stFWInfo.Chn            = pstFileInfo->channel;

	NC_API_DRV_Coax_FW_End_CMD_Set( &stFWInfo );
	__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[End_Send]FW End Command Send!");

	sleep(2);
	while(timeout_cnt++ < COAX_FW_TIMEOUT)
	{
		NC_API_DRV_Coax_FW_End_CMD_Get( &stFWInfo );
		if( stFWInfo.result == 0 )
		{
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[End_ACK]ACP FIRMUP End ACK Recv OK!!");
			fprintf( stdout, ">>>>> APP[%s:%d] stage2(CH:%d), OK  : Start F/W [F/W table]\n", __func__, __LINE__, ch );
			ret = COAX_FW_SUCCESS;
			break;
		}
		else
		{
			sprintf(FW_Status, "[End_ACK]ACP FIRMUP End ACK Recv Retry Retry: %d", timeout_cnt);
			__NC_APP_VD_COAX_FirmUP_UI_Status_Set(FW_Status);
			fprintf( stdout, ">>>>> APP[%s:%d] stage2(CH:%d), Failed  : Start F/W [F/W table]...Retry:%d\n", __func__, __LINE__, ch, timeout_cnt );
			if( timeout_cnt == COAX_FW_TIMEOUT )
			{
				__NC_APP_VD_COAX_FirmUP_UI_Status_Set("[End_ACK]ACP FIRMUP End ACK Recv FAIL - ACK Recv Time out!!");
				ret = COAX_FW_FAILURE;
				sleep(1);
				break;
			}
			sleep(1);
		}
	}

	return ret;
}

void *t_NC_APP_VD_COAX_FirmUP_CoaxFWUpdate_Start( void *arg )
{
	char cmd[1024] = {0, };
	char firmware_name[1024] = {0, };

	FILE *pFile = NULL;

	nc_decoder_s stVD_Video;

	FIRMWARE_START_INFO *pParam = (FIRMWARE_START_INFO*)arg;
	int channel = pParam->ch;
	int fileNum = pParam->fileNo;

	/* initialize F/W manager */
	memset( &s_fileinfo, 0x00, sizeof(nc_coax_firmup_s));

	/* initialize Logical Flag*/
	__NC_APP_VD_COAX_FirmUP_Logical_Flag_Init();

	/***********************************************************************************************************************
	 * 1. Update File Open
	 ***********************************************************************************************************************/
	NC_APP_VD_COAX_FirmUP_File_Select_Get( firmware_name, fileNum );
	sprintf( cmd, "/%s/%s", MOUNT_USB_NAME, firmware_name );

	pFile = fopen( cmd, "rb");
	if(pFile == NULL)
	{
		fprintf( stdout, ">>>>> APP[%s:%d] failed open : %s\n", __func__, __LINE__, cmd );
		// must make
		__NC_APP_VD_COAX_FirmUP_USB_Umount();
		return 0;
	}
	__NC_APP_VD_COAX_FirmUP_UI_Status_Set("open firmware update file");

	/***********************************************************************************************************************
	 * 2. FW Manger get of Update File information
	 ***********************************************************************************************************************/
	__NC_APP_VD_COAX_FirmUP_File_information_Get( pFile, firmware_name, (void*)&s_fileinfo );
	__NC_APP_VD_COAX_FirmUP_UI_Status_Set("firmware file information Get");

	s_fileinfo.channel = channel;	               // physical

	/***********************************************************************************************************************
	 * 3. [FirmUP] Step: "ACP Check"  -  Decoder <- ISP Synchronization
	 *    1) ACP Header Check From ISP....ACP 1st Line Vale(Header)..0x55
	 ***********************************************************************************************************************/
	if( __NC_APP_VD_COAX_FirmUP_ACP_Header_Check(&s_fileinfo) != COAX_FW_SUCCESS )
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("ACP Header Check Error!\n");
		fprintf( stdout, ">>>>> APP[%s:%d] __NC_APP_VD_COAX_FirmUP_Not Supported Format!(CH:%d), FAIL!!\n", __func__, __LINE__, channel );

		s_Progress_Start_flag = 2;
		__NC_APP_VD_COAX_FirmUP_Fail(pFile);
		return 0;
	}

	/***********************************************************************************************************************
	 * 4. [FirmUP] Step: "FW Ready"  -  Decoder <-> ISP Synchronization
	 *    1) Command send "FW Update Ready" to ISP....Mode Change AFHD@25P
	 *    2) Ready ACK Receive From ISP
	 ***********************************************************************************************************************/
	if( __NC_APP_VD_COAX_FirmUP_Ready_Check(&s_fileinfo) != COAX_FW_SUCCESS )
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("ACP Ready Check Error!");
		fprintf( stdout, ">>>>> APP[%s:%d] __NC_APP_VD_COAX_FirmUP_Ready_Check(CH:%d), FAIL!!\n", __func__, __LINE__, channel );
		s_Progress_Start_flag = 2;
		__NC_APP_VD_COAX_FirmUP_Fail(pFile);

		stVD_Video.Chn	   = 0;
		stVD_Video.OP_Mode = NC_VIDEO_SET_MODE_AUTO;
		NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);

		return 0;
	}

	/***********************************************************************************************************************
	 * 5. [FirmUP] Step: "FW Start"  -  Decoder <-> ISP Synchronization
	 *    1) Command send "FW Update Start" to ISP....Mode Change Black Pattern
	 *    2) Start ACK Receive From ISP
	 ***********************************************************************************************************************/
	if( __NC_APP_VD_COAX_FirmUP_Start_Check(&s_fileinfo) != COAX_FW_SUCCESS )
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("ACP Start Check Error!");
		fprintf( stdout, ">>>>> APP[%s:%d] __NC_APP_VD_COAX_FirmUP_Start_Check(CH:%d), FAIL!!\n", __func__, __LINE__, channel );
		s_Progress_Start_flag = 2;
		__NC_APP_VD_COAX_FirmUP_Fail(pFile);

		stVD_Video.Chn	   = 0;
		stVD_Video.OP_Mode = NC_VIDEO_SET_MODE_AUTO;
		NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);

		return 0;
	}

	/***********************************************************************************************************************
	 * 6. [FirmUP] Step: "FW Data Send"  -  Decoder <-> ISP Synchronization
	 *    1) Data send to ISP(Send in 139 bytes size)....The update status is displayed on the screen.
	 *    2) Receive From ISP
	 ***********************************************************************************************************************/
	if( __NC_VD_COAX_Firmware_Update_Data_Send( pFile, &s_fileinfo ) != COAX_FW_SUCCESS )
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("ACP FirmUP Data Send Error!!");
		fprintf( stdout, ">>>>> APP[%s:%d] FirmUP_Data_Send_Fail!!(CH:%d)\n", __func__, __LINE__, channel );

		stVD_Video.Chn	   = 0;
		stVD_Video.OP_Mode = NC_VIDEO_SET_MODE_AUTO;
		NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);

	}

	/***********************************************************************************************************************
	 * 7. [FirmUP] Step: "FW End"  -  Decoder <-> ISP Synchronization
	 *    1) Command Send "FW Update End" to ISP....Mode Change Black Pattern
	 *    2) End ACK Receive From ISP
	 ***********************************************************************************************************************/
	if( __NC_APP_VD_COAX_FirmUP_End_Check( &s_fileinfo ) != COAX_FW_SUCCESS )
	{
		__NC_APP_VD_COAX_FirmUP_UI_Status_Set("ACP End Check Error!!");
		fprintf( stdout, ">>>>> APP[%s:%d] FirmUP_END_Check_Fail!!(CH:%d)\n", __func__, __LINE__, channel );
		s_Progress_Start_flag = 2;
		__NC_APP_VD_COAX_FirmUP_Fail(pFile);

		stVD_Video.Chn	   = 0;
		stVD_Video.OP_Mode = NC_VIDEO_SET_MODE_AUTO;
		NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);

		return 0;
	}

	/***********************************************************************************************************************
	 * 8. File Close
	 ***********************************************************************************************************************/
	if( pFile != NULL )
		fclose(pFile);

	/***********************************************************************************************************************
	 * 9. Unmount USB
	 ***********************************************************************************************************************/
	// __NC_APP_VD_COAX_FirmUP_USB_Umount();
	__NC_APP_VD_COAX_FirmUP_UI_Status_Set("Firmware Upgrade Complete");
	s_Progress_Start_flag = 2;

	stVD_Video.Chn	   = 0;
	stVD_Video.OP_Mode = NC_VIDEO_SET_MODE_AUTO;
	NC_API_DRV_Video_Auto_Manual_Mode_Set(&stVD_Video);

	return 0;

}

static FIRMWARE_START_INFO *g_Param;
void NC_APP_VD_COAX_FirmUP_CoaxFWUpdate_Start( int Ch, int fileNum )
{
	g_Param = malloc(sizeof(FIRMWARE_START_INFO));
	g_Param->ch              = Ch;
	g_Param->fileNo          = fileNum;

	pthread_create( &s_pthr_FirmUP_Start, NULL, t_NC_APP_VD_COAX_FirmUP_CoaxFWUpdate_Start, (void*)g_Param );
}

int NC_APP_VD_COAX_System_FirmUP_File_Check(void)
{
	int flag = 0;
	DIR *dir;
	struct  dirent *entry = NULL;

	const char	*fileName;
	const char app_file[32] = "dvr";
	const char dvr_file[32] = "raptor4.ko";
	char path[128] = {0, };
//	int FW_File_Cnt = 0;

	sprintf( path, "%s", "/mnt/usb" );
	dir = opendir(path);
	if(dir == NULL)
	{
		fprintf( stdout, "[%s:%d] >> Error Open Dir:%s\n",__FILE__, __LINE__, path );
		perror(" :");
		return COAX_FW_FAILURE;
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

				if( strncmp(app_file, fileName, len) == 0 )
				{
					fprintf( stdout, "[%s:%d] file Exist >>>> %s \n",__FILE__, __LINE__, fileName );
					system("cp /mnt/usb/dvr /mnt/app");
					system("rm /mnt/usb/dvr");
					flag = 1;
				}
				if( strncmp(dvr_file, fileName, len) == 0 )
				{
					fprintf( stdout, "[%s:%d] file Exist >>>> %s \n",__FILE__, __LINE__, fileName );
					system("cp /mnt/usb/raptor4.ko /mnt/app/ko/extdrv/");
					system("rm /mnt/usb/raptor4.ko");
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

	return COAX_FW_SUCCESS;
}

