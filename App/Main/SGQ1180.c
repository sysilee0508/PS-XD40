#if 0


/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "SGQ1180.h"

/* value define --------------------------------------------------------------*/


unsigned int I2C_READ(unsigned char slaveaddr, unsigned char regaddr)
{
	unsigned int receive_data[4] = {0,0,0,0};

	I2C_Start();
	I2C_P2S(slaveaddr&0xFE); 	 AckDetect();
	I2C_P2S(regaddr); 			 AckDetect();		
	I2C_Start(); 
	I2C_P2S(slaveaddr|0x01); 	 AckDetect();
	receive_data[0] = I2C_S2P(); AckSend();  				
	receive_data[1] = I2C_S2P(); AckSend();  				
	receive_data[2] = I2C_S2P(); AckSend();  				
	receive_data[3] = I2C_S2P(); NotAck();
	I2C_Stop();												

    return (receive_data[0]<<24) | (receive_data[1]<<16) | (receive_data[2]<<8) | receive_data[3];
}


void I2C_WRITE(unsigned char slaveaddr, unsigned char regaddr, unsigned int write_data)
{
	I2C_Start();
	I2C_P2S(slaveaddr&0xFE); 						AckDetect();
	I2C_P2S(regaddr); 		 						AckDetect();		
	I2C_P2S((BYTE)((write_data >> 24)&0x000000ff));	AckDetect();  		
	I2C_P2S((BYTE)((write_data >> 16)&0x000000ff));	AckDetect();  		
	I2C_P2S((BYTE)((write_data >> 8)&0x000000ff));	AckDetect();  		
	I2C_P2S((BYTE)((write_data)&0x000000ff)); 		AckDetect();  	
	I2C_Stop();		
}



/**
  * @brief  SGQ-1180 EVB (METIS) software for check full display mode
  * @param  ch : channel number
  * @retval None
  */

char SGQ_FULL_CHECK(void) 
{
    unsigned int read_buffer;
    read_buffer = I2C_READ(FPGA1_ADDR, DISPLAYCFG);
    return ((read_buffer & 0xC)>>2); 
}

/**
  * @brief  SGQ-1180 EVB (METIS) software for  full display
  * @param  ch : channel number
  * @retval None
  */

void SGQ_CH_FULL(unsigned int ch) 
{

    unsigned int read_buffer;
    unsigned char faddr_set;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr_set = FPGA1_ADDR;  break;
		case 0x02 : faddr_set = FPGA2_ADDR;  break;
		case 0x03 : faddr_set = FPGA3_ADDR;  break;
		default   : faddr_set = FPGA0_ADDR;  break; 
	} 


    if((ch & 0xf) == PB_CH) 
	{
        read_buffer = I2C_READ(FPGA0_ADDR, DISPLAYCFG);
        read_buffer = (read_buffer & 0xffffff83); 
        I2C_WRITE(FPGA0_ADDR, DISPLAYCFG, read_buffer);

        SGQ_DISPLAY_OSGOverlay_SET(FPGA1, SGQ_DISABLE);
        SGQ_DISPLAY_PBMode_SET(FPGA1, SGQ_ENABLE);

    } 
	else if(faddr_set == FPGA0_ADDR) 
	{
        read_buffer = I2C_READ(faddr_set, DISPLAYCFG);
        read_buffer = (read_buffer & 0xffffff83) | 
                     ((((ch - 1) & 0x3) | 0x4) & 0xf) << 4; 
        I2C_WRITE(faddr_set, DISPLAYCFG, read_buffer);
        
        read_buffer = I2C_READ(FPGA1_ADDR, DISPLAYCFG);
        read_buffer = (read_buffer & 0xffffff83); 
        I2C_WRITE(FPGA1_ADDR, DISPLAYCFG, read_buffer);
    } 
	else 
	{
        read_buffer = I2C_READ(faddr_set, DISPLAYCFG);
        read_buffer = (read_buffer & 0xffffff83) | 
                     ((((ch - 1) & 0x3) | 0x4) & 0xf) << 4; 
        I2C_WRITE(faddr_set, DISPLAYCFG, read_buffer);
        
        read_buffer = I2C_READ(FPGA0_ADDR, DISPLAYCFG);
        read_buffer = (read_buffer & 0xffffff83); 
        I2C_WRITE(FPGA0_ADDR, DISPLAYCFG, read_buffer);
    }
}



/**
  * @brief  SGQ-1180 EVB (METIS) software for SPLIT display
  * @param  mode : SPLIT4/9/16
  * @retval None
  */

void SGQ_SPLIT(unsigned char mode) 
{

    unsigned int read_buffer;
    unsigned char smode;

	switch(mode  & 0xf) 
	{ 
		case SPLIT1  : 	smode = 0x0;  break;
		case SPLIT9  :  smode = 0x8;  break;
		case SPLIT16 :  smode = 0xC;  break;
		default      :  smode = 0x4;  break; 
	} 

    //METIS 8ch
    read_buffer = I2C_READ(FPGA0_ADDR, DISPLAYCFG);
    read_buffer = (read_buffer & 0xfffffff3) | (smode & 0xf) | (0x1 << 7);			
    I2C_WRITE(FPGA0_ADDR, DISPLAYCFG, read_buffer);

    read_buffer = I2C_READ(FPGA1_ADDR, DISPLAYCFG);
    read_buffer = (read_buffer & 0xfffffff3) | (smode & 0xf) | (0x1 << 7);			
    I2C_WRITE(FPGA1_ADDR, DISPLAYCFG, read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for position
  * @param  ch : channel number
  *         pos_num : position number
  *         en : enalbe /disable channel
  *         mode : 0 -> 4ch 1 -> 8ch 2 -> 16ch
  * @retval None
  */

void SGQ_CH_POSITION(unsigned int ch, unsigned char en, unsigned char pos_num, unsigned char mode) {

    unsigned int read_buffer, pos, en_pos_num;
    unsigned char faddr;
    unsigned char loc_ch;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    loc_ch = (ch - 1) * 8;

    pos = 0x0;
    pos = (0x1f << loc_ch) ^ (0xffffffff);

    en_pos_num = (((pos_num & 0xf) | ((en & 0x1) << 4)) << loc_ch) & 0xffffffff;


    switch(mode) 
	{
        case SPLIT4: // 4 channel
            read_buffer = I2C_READ(faddr, LOC4);
            read_buffer = (pos & read_buffer) | en_pos_num;
            I2C_WRITE(faddr, LOC4, read_buffer);
//printf("----------position addr [%x] 0x%x [0x%x] %x \n\r",faddr, pos,  en_pos_num, read_buffer);
            break;
        case SPLIT9: // 9 channel
            read_buffer = I2C_READ(faddr, LOC9);
            read_buffer = (pos & read_buffer) | en_pos_num;
            I2C_WRITE(faddr, LOC9, read_buffer);
            break;
        default:    // 16 channel
            read_buffer = I2C_READ(faddr, LOC16);
            read_buffer = (pos & read_buffer) | en_pos_num;
            I2C_WRITE(faddr, LOC16, read_buffer);
            break;
    }
}

void SGQ_CH_POSITION_SPECIAL_CLEAR()
{
	I2C_WRITE(FPGA0_ADDR, LOCSPECIAL, 0x0);
	I2C_WRITE(FPGA1_ADDR, LOCSPECIAL, 0x0);
	printf("LocSpecial %x \r\n",I2C_READ(FPGA0_ADDR, LOCSPECIAL));
	printf("LocSpecial %x \r\n",I2C_READ(FPGA1_ADDR, LOCSPECIAL));
}
/**
  * @brief  SGQ-1180 EVB (METIS) software for position
  * @param  ch : channel number
  *         pos_num : position number
  *         en : enalbe /disable channel
  *         specialEn : enable /disable special channel
  * @retval None
  */

void SGQ_CH_POSITION_SPECIAL(unsigned int ch, unsigned char specialEn, unsigned char en, unsigned char pos_num) 
{
    unsigned int read_buffer, pos, en_pos_num;
    unsigned char faddr;
    unsigned char loc_ch;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    loc_ch = (ch - 1) * 8;

    pos = 0x0;
    pos = (0x3f << loc_ch) ^ (0xffffffff);

    en_pos_num = (((pos_num & 0xf) | ((en & 0x1) << 4) | ((specialEn & 0x1)<<5)) << loc_ch) & 0xffffffff;

	read_buffer = I2C_READ(faddr, LOCSPECIAL);
	read_buffer = (pos & read_buffer) | en_pos_num;
	I2C_WRITE(faddr, LOCSPECIAL, read_buffer);
	//printf("specialEn %d : pos_num %02d :: pos%08x en_pos_num%08x read_buffer%08x \r\n",specialEn,pos_num,pos,en_pos_num,read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Record swith selection
  * @param  ch : channel number
  *         sw_val : switch value 
  * @retval None
  */
/*
void SGQ_RECSW_SEL(unsigned int ch, unsigned int sw_val)
{

    unsigned char faddr;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    if((ch & 0xff) == REC_CH01)
            I2C_WRITE(faddr, REC01SW, sw_val);
    else    I2C_WRITE(faddr, REC23SW, sw_val);

}
*/

/**
  * @brief  SGQ-1180 EVB (METIS) software for Record mode setting
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */
/*
void SGQ_RECMODE_Encoding_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    if((ch & 0xff) == REC_CH01) 
    {
        read_buffer = I2C_READ(faddr, REC01MODE);
        read_buffer = (read_buffer & 0xFFFFDFFF) | ((0x1 & en) << 13);
        I2C_WRITE(faddr, REC01MODE, read_buffer);
    } 
    else if((ch & 0xff) == REC_CH23) 
    {
        read_buffer = I2C_READ(faddr, REC23MODE);
        read_buffer = (read_buffer & 0xFFFFDFFF) | ((0x1 & en) << 13);
        I2C_WRITE(faddr, REC23MODE, read_buffer);
    }
}
*/

/**
  * @brief  SGQ-1180 EVB (METIS) software for Record mode setting
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_RECMODE_Enable_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 
//	printf("read start\r\n");
	read_buffer = I2C_READ(faddr, RECMODE);
//	printf("read end\r\n");
	switch(ch & 0xff) 
	{ 
		case REC_CH0 : 
			read_buffer = (en)?(read_buffer | 1) : (read_buffer & ~(1));
			break;
		case REC_CH1 : 
			read_buffer = (en)?(read_buffer | 2) : (read_buffer & ~(2));
			break;
		case REC_CH2 : 
			read_buffer = (en)?(read_buffer | 4) : (read_buffer & ~(4));
			break;
		case REC_CH3 : 
			read_buffer = (en)?(read_buffer | 8) : (read_buffer & ~(8));
			break;
		default : return;
	}

	I2C_WRITE(faddr, RECMODE, read_buffer);
}



/**
  * @brief  SGQ-1180 EVB (METIS) software for Record mode setting
  * @param  ch : channel number
  *         val : setting number
  * @retval None
  */
/*
void SGQ_RECMODE_ChannelSWMax_SET(unsigned int ch, unsigned int val)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    if((ch & 0xff) == REC_CH01) 
    {
        read_buffer = I2C_READ(faddr, REC01MODE);
        read_buffer = (read_buffer & 0xFFFFE0FF) | ((0x1f & val) << 8);
        I2C_WRITE(faddr, REC01MODE, read_buffer);
    } 
    else if((ch & 0xff) == REC_CH23) 
    {
        read_buffer = I2C_READ(faddr, REC23MODE);
        read_buffer = (read_buffer & 0xFFFFE0FF) | ((0x1f & val) << 8);
        I2C_WRITE(faddr, REC23MODE, read_buffer);
    }
}
*/

/**
  * @brief  SGQ-1180 EVB (METIS) software for Record mode setting
  * @param  ch : channel number
  *         val : setting value
  * @retval None
  */
/*
void SGQ_RECMODE_VideoMode_SET(unsigned int ch, unsigned int val)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    if((ch & 0xff) == REC_CH01) 
    {
        read_buffer = I2C_READ(faddr, REC01MODE);
        read_buffer = (read_buffer & 0xFFFFFFF0) | ((0xf & val) << 0);
        I2C_WRITE(faddr, REC01MODE, read_buffer);
    } 
    else if((ch & 0xff) == REC_CH23) 
    {
        read_buffer = I2C_READ(faddr, REC23MODE);
        read_buffer = (read_buffer & 0xFFFFFFF0) | ((0xf & val) << 0);
        I2C_WRITE(faddr, REC23MODE, read_buffer);
    }
}
*/

/**
  * @brief  SGQ-1180 EVB (METIS) software for DISPLAY mode setting
  * @param  ch : channel number
  *         val : setting value
  * @retval None
  */

void SGQ_DISPLAY_VideoMode_SET(unsigned int ch, unsigned int val)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYMODE);
    read_buffer = (read_buffer & 0xFFFFFFF0) | ((0xf & val) << 0);
    I2C_WRITE(faddr, DISPLAYMODE, read_buffer);
}



/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_Enable_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYMODE);
//printf("-------------FPGA0- read 0x%x [0x%x]\n\r",faddr,  read_buffer);
    read_buffer = (read_buffer & 0xFFFFFFEF) | ((0x1 & en) << 4);
//printf("-------------FPGA0- read 0x%x [0x%x]\n\r",faddr,  read_buffer);
    I2C_WRITE(faddr, DISPLAYMODE, read_buffer);

    read_buffer = I2C_READ(faddr, DISPLAYMODE);
//printf("----------display---FPGA0- read 0x%x [0x%x]\n\r",faddr,  read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  *         connect display output to the other cascae input port
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_Terminal_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYMODE);
    read_buffer = (read_buffer & 0xFFFFFFBF) | ((0x1 & en) << 6);
    I2C_WRITE(faddr, DISPLAYMODE, read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  *         cascae input port enable
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_SlaveMode_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYMODE);
    read_buffer = (read_buffer & 0xFFFFFFDF) | ((0x1 & en) << 5);
    I2C_WRITE(faddr, DISPLAYMODE, read_buffer);

}

/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  *         special locaction enable
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_SpecialLoc_SET(unsigned int ch, unsigned char en, unsigned char specialMode)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYCFG);
    read_buffer = (read_buffer & 0xFFE1FFFF) | ((0x1 & en) << 20) | ((0x7 & specialMode)<<17);
    I2C_WRITE(faddr, DISPLAYCFG, read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  *         cascae mix enable
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_CascadeMix_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYCFG);
    read_buffer = (read_buffer & 0xFFFFFF7F) | ((0x1 & en) << 7);
    I2C_WRITE(faddr, DISPLAYCFG, read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  *         cascae mix enable
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_OSGOverlay_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYCFG);
    read_buffer = (read_buffer & 0xFFFFFFFD) | ((0x1 & en) << 1);
//    read_buffer = (read_buffer & 0xFFFFFFFC) | ((0x1 & en) << 1);
//    read_buffer = (read_buffer & 0xFFFFFFFC); // thlee test
    I2C_WRITE(faddr, DISPLAYCFG, read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Display mode setting
  *         cascae mix enable
  * @param  ch : channel number
  *         en : ENABLE / DISABLE 
  * @retval None
  */

void SGQ_DISPLAY_PBMode_SET(unsigned int ch, unsigned char en)
{
    unsigned char faddr;
    unsigned int read_buffer;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = I2C_READ(faddr, DISPLAYCFG);
    read_buffer = (read_buffer & 0xFFFFFFFE) | ((0x1 & en) << 0);
    I2C_WRITE(faddr, DISPLAYCFG, read_buffer);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for 1080i REC/Display initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */

void SGQ_RECDISPLAY_INIT(unsigned int ch, unsigned int mode)
{
/*		
    SGQ_RECMODE_VideoMode_SET(ch | REC_CH01, mode);
    SGQ_RECMODE_VideoMode_SET(ch | REC_CH23, mode);
    SGQ_RECSW_SEL(ch | REC_CH01, 0xFFFF0000);
    SGQ_RECSW_SEL(ch | REC_CH23, 0xAAAAAAAA);
    SGQ_RECMODE_Encoding_SET(ch | REC_CH01, SGQ_ENABLE);
    SGQ_RECMODE_Encoding_SET(ch | REC_CH23, SGQ_ENABLE);
    SGQ_RECMODE_ChannelSWMax_SET(ch | REC_CH01, 0x1f);
    SGQ_RECMODE_ChannelSWMax_SET(ch | REC_CH23, 0x1f);
    SGQ_RECMODE_Enable_SET(ch | REC_CH01,  SGQ_ENABLE);
    SGQ_RECMODE_Enable_SET(ch | REC_CH23,  SGQ_ENABLE);
*/

    SGQ_RECMODE_Enable_SET(ch | REC_CH0,  SGQ_ENABLE);
    SGQ_RECMODE_Enable_SET(ch | REC_CH1,  SGQ_ENABLE);
    SGQ_RECMODE_Enable_SET(ch | REC_CH2,  SGQ_ENABLE);
    SGQ_RECMODE_Enable_SET(ch | REC_CH3,  SGQ_ENABLE);
    SGQ_DISPLAY_VideoMode_SET(ch, mode);
//    SGQ_DISPLAY_OSGOverlay_SET(ch, SGQ_ENABLE);
    SGQ_DISPLAY_Enable_SET(ch, SGQ_ENABLE);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for filling the color
  * @param  ch : channel number
  *         force_en : force color enable
  * @retval None
  */

void SGQ_CH_Color(unsigned int ch, unsigned char force_en,
                  unsigned char idle_count,
                  unsigned char y,
                  unsigned char cb,
                  unsigned char cr) 
{
    unsigned int read_buffer;
    unsigned char faddr;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

    read_buffer = ((force_en & 0x1)   << 28)  | 
                  ((idle_count & 0xf) << 24)  |
                  ((y  & 0xff) << 16)       | 
                  ((cb & 0xff) << 8 )       | 
                  ((cr & 0xff) << 0 )       ; 

	switch(ch & 0xff) 
	{ 
		case SDI_CH0: I2C_WRITE(faddr, COLOR0, read_buffer); break;
		case SDI_CH1: I2C_WRITE(faddr, COLOR1, read_buffer); break;
		case SDI_CH2: I2C_WRITE(faddr, COLOR2, read_buffer); break;
		case SDI_CH3: I2C_WRITE(faddr, COLOR3, read_buffer); break;
		case PB_CH   :I2C_WRITE(faddr, COLORPB	 , read_buffer); break;
			//case REC_CH01:I2C_WRITE(faddr, COLORREC01, read_buffer); break;
			//case REC_CH23:I2C_WRITE(faddr, COLORREC23, read_buffer); break;
    }
}

/**
  * @brief  SGQ-1160 EVB (PHOBOS) software for 1080i 16Ch split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_1080i_16CH_INIT(void)
{
    //FPGA0 temination mode
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_ENABLE);
    SGQ_RECDISPLAY_INIT(FPGA0, Mode1080i60);

    //FPGA1 slave mode
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_RECDISPLAY_INIT(FPGA1, Mode1080i60);

    //8ch position setting
    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT16);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT16);
    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT16);
    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT16);

    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x8, SPLIT16);
    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0x9, SPLIT16);
    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0xa, SPLIT16);
    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, 0xb, SPLIT16);

    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    //16ch split
    SGQ_SPLIT(SPLIT16);
}




/**
  * @brief  SGQ-1160 EVB (PHOBOS) software for 1080i 9Ch split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_1080i_9CH_INIT(void)
{
    //FPGA0 temination mode
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_ENABLE);
    SGQ_RECDISPLAY_INIT(FPGA0, Mode1080i60);

    //FPGA1 slave mode
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_RECDISPLAY_INIT(FPGA1, Mode1080i60);

    //8ch position setting
    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT9);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT9);
    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT9);
    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT9);

    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x5, SPLIT9);
    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0x6, SPLIT9);
    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0x7, SPLIT9);
    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, 0x8, SPLIT9);

    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    //8ch split
    SGQ_SPLIT(SPLIT9);
}

/**
  * @brief  SGQ-1160 EVB (PHOBOS) software for 1080i 4Ch split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_1080i_4CH_INIT(void)
{
    //FPGA1 slave mode
    SGQ_RECDISPLAY_INIT(FPGA1, Mode1080i60);
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA1, SGQ_DISABLE);

    //FPGA0 temination mode
    SGQ_RECDISPLAY_INIT(FPGA0, Mode1080i60);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_ENABLE);


    //4ch position setting
    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_DISABLE,0x4, SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_DISABLE,0x5, SPLIT4);

    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x2, SPLIT4);
    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_DISABLE,0x6, SPLIT4);
    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_DISABLE,0x7, SPLIT4);
    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT4);

    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    //4ch split
    SGQ_SPLIT(SPLIT4);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for read channel lock
  * @param  ch : channel number
  * @retval None
  */
unsigned char SGQ_READ_CH_LOCK(unsigned int ch) 
{
    unsigned int read_buffer;
    unsigned char faddr;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

	switch((ch) & 0xff) 
	{ 
        case SDI_CH0 : read_buffer = 0x1 & (I2C_READ(faddr, IFORMAT0) >> 25); break; 
        case SDI_CH1 : read_buffer = 0x1 & (I2C_READ(faddr, IFORMAT1) >> 25); break; 
        case SDI_CH2 : read_buffer = 0x1 & (I2C_READ(faddr, IFORMAT2) >> 25); break; 
        case SDI_CH3 : read_buffer = 0x1 & (I2C_READ(faddr, IFORMAT3) >> 25); break; 
        case PB_CH   : read_buffer = 0x1 & (I2C_READ(faddr, IFORMAT4) >> 25); break; 
    }

    return read_buffer;
}


unsigned char SGQ_READ_CH_FLOCK(unsigned int ch) 
{
    unsigned int read_buffer = 0;

	switch(ch) 
	{ 
        case 0 : read_buffer = 0x1 & (I2C_READ(FPGA0_ADDR, IFORMAT0) >> 27); break; 
        case 1 : read_buffer = 0x1 & (I2C_READ(FPGA0_ADDR, IFORMAT1) >> 27); break; 
        case 2 : read_buffer = 0x1 & (I2C_READ(FPGA0_ADDR, IFORMAT2) >> 27); break; 
        case 3 : read_buffer = 0x1 & (I2C_READ(FPGA0_ADDR, IFORMAT3) >> 27); break; 
        case 4 : read_buffer = 0x1 & (I2C_READ(FPGA1_ADDR, IFORMAT0) >> 27); break; 
        case 5 : read_buffer = 0x1 & (I2C_READ(FPGA1_ADDR, IFORMAT1) >> 27); break; 
        case 6 : read_buffer = 0x1 & (I2C_READ(FPGA1_ADDR, IFORMAT2) >> 27); break; 
        case 7 : read_buffer = 0x1 & (I2C_READ(FPGA1_ADDR, IFORMAT3) >> 27); break; 
    }

    return read_buffer;
}



/**
  * @brief  SGQ-1180 EVB (METIS) software for read channel format
  * @param  ch : channel number
  * @retval None
  */
unsigned int SGQ_READ_CH_WIDTH(unsigned int ch) 
{
    unsigned int read_buffer;
    unsigned char faddr;

	switch((ch >> 8) & 0xf) 
	{ 
		case 0x01 : faddr = FPGA1_ADDR;  break;
		case 0x02 : faddr = FPGA2_ADDR;  break;
		case 0x03 : faddr = FPGA3_ADDR;  break;
		default   : faddr = FPGA0_ADDR;  break; 
	} 

	switch((ch) & 0xff) 
	{ 
        case SDI_CH0 : read_buffer = 0x7ff & (I2C_READ(faddr, IFORMAT0) >> 12); break; 
        case SDI_CH1 : read_buffer = 0x7ff & (I2C_READ(faddr, IFORMAT1) >> 12); break; 
        case SDI_CH2 : read_buffer = 0x7ff & (I2C_READ(faddr, IFORMAT2) >> 12); break; 
        case SDI_CH3 : read_buffer = 0x7ff & (I2C_READ(faddr, IFORMAT3) >> 12); break; 
        case PB_CH   : read_buffer = 0x7ff & (I2C_READ(faddr, IFORMAT4) >> 12); break; 
    }

    return read_buffer;
}



/**
  * @brief  SGQ-1180 EVB (METIS) software for 16Ch split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_16CH_INIT(unsigned int mode)
{
    //FPGA0 master mode
    SGQ_RECDISPLAY_INIT(FPGA0, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA0, SGQ_DISABLE);

    //FPGA1 slave & temination mode
    SGQ_RECDISPLAY_INIT(FPGA1, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA1, SGQ_DISABLE);

	if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
	{
	    //8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0xc, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0xd, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0xe, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT16);

	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x7, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0xb, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0xf, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT16);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
	{
	    //8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x4, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x8, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0xc, SPLIT16);

	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0xd, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0xe, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0xf, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT16);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
	{
	    //8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT16);

	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x7, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0xb, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0xf, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT16);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
	{
	    //8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT16);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT16);

	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x4, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0x8, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0xc, SPLIT16);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT16);
	}

    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    //16ch split
//    SGQ_SPLIT(SPLIT16);
    ForceColor(SPLIT16);
    Delay_ms(10);
   	ForceColorDis(SPLIT16);
}




/**
  * @brief  SGQ-1180 EVB (METIS) software for 9Ch split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_9CH_INIT(unsigned int mode)
{
    //FPGA0 master mode
    SGQ_RECDISPLAY_INIT(FPGA0, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA0, SGQ_DISABLE);

    //FPGA1 slave & temination mode
    SGQ_RECDISPLAY_INIT(FPGA1, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA1, SGQ_DISABLE);

	if(sys_status.current_split_mode == SPLITMODE_SPLIT9_1)
	{
		//8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x4, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0x5, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0x6, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, 0x7, SPLIT9);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_6)
	{
		//8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x6, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x7, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x5, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x8, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT9);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_7)
	{
		//8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x3, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x6, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x7, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x8, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT9);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_8)
	{
		//8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x5, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x8, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT9);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_9)
	{
		//8ch position setting
	    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT9);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x6, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_DISABLE, 0x0, SPLIT9);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x0, SPLIT9);
	}


    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    //8ch split
//    SGQ_SPLIT(SPLIT9);
    ForceColor(SPLIT9);
    Delay_ms(10);
   	ForceColorDis(SPLIT9);
}

/**
  * @brief  SGQ-1180 EVB (METIS) software for 4Ch split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_4CH_INIT(unsigned int mode)
{
    //FPGA0 temination mode
    SGQ_RECDISPLAY_INIT(FPGA0, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA0, SGQ_DISABLE);
    
	//FPGA1 slave mode
    SGQ_RECDISPLAY_INIT(FPGA1, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA1, SGQ_DISABLE);
	
    //4ch position setting
	if(sys_status.current_split_mode == SPLITMODE_SPLIT4_1)
	{
		SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT4);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT4);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT4);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT4);

	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_DISABLE, 0x4, SPLIT4);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_DISABLE, 0x5, SPLIT4);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_DISABLE, 0x6, SPLIT4);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_DISABLE, 0x7, SPLIT4);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT4_2) 
	{
		SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_DISABLE, 0x4, SPLIT4);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_DISABLE, 0x5, SPLIT4);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_DISABLE, 0x6, SPLIT4);
	    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_DISABLE, 0x7, SPLIT4);

	    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, 0x0, SPLIT4);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, 0x1, SPLIT4);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, 0x2, SPLIT4);
	    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, 0x3, SPLIT4);
	}
		
    //channel color
	//												 y	   cb    cr		
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 

    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 

//	printf("FPGA config end\r\n");
    //4ch split
    ForceColor(SPLIT4);
    Delay_ms(10);
   	ForceColorDis(SPLIT4);
}
/*
void SGQ_4CH_INIT(unsigned int mode)
{
    //FPGA0 temination mode
    SGQ_RECDISPLAY_INIT(FPGA0, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    //SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_ENABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA0, SGQ_DISABLE);
    
    //4ch position setting
	SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, 0x7, SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, 0x6, SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, 0x5, SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, 0x4, SPLIT4);

    //channel color
	//												 y	   cb    cr		
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80, 0x80); 

//	printf("FPGA config end\r\n");
    //4ch split
    ForceColor(SPLIT4);
    Delay_ms(10);
   	ForceColorDis(SPLIT4);
}*/


/**
  * @brief  SGQ-1180 EVB (METIS) software for Special split mode initial
  *         cascae mix enable
  * @param  None
  * @retval None
  */
void SGQ_SPECIALCH_INIT(unsigned int mode)
{
    //FPGA0 master mode
    SGQ_RECDISPLAY_INIT(FPGA0, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA0, SGQ_DISABLE);

    //FPGA1 slave & temination mode
    SGQ_RECDISPLAY_INIT(FPGA1, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_PBMode_SET(FPGA1, SGQ_DISABLE);

    //special position setting

	if(sys_status.current_split_mode == SPLITMODE_SPLIT9_2)
    {
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0,SPECIAL_ENABLE  ,SGQ_ENABLE, 0x0);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0xc);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0xd);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xe);

	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0,SPECIAL_DISABLE ,SGQ_ENABLE, 0x3);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x7);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0xb);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xf);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_3)
    {
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0,SPECIAL_ENABLE  ,SGQ_ENABLE, 0x1);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x0);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0x4);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0x8);

	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0,SPECIAL_DISABLE ,SGQ_ENABLE, 0xc);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0xd);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0xe);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xf);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_4)
    {
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0,SPECIAL_ENABLE  ,SGQ_ENABLE, 0x4);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x0);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0x1);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0x2);

	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0,SPECIAL_DISABLE ,SGQ_ENABLE, 0x3);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x7);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0xb);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xf);
	}
	else if(sys_status.current_split_mode == SPLITMODE_SPLIT9_5)
    {
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0,SPECIAL_ENABLE  ,SGQ_ENABLE, 0x6);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x0);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0x1);
	    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0x2);

	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0,SPECIAL_DISABLE ,SGQ_ENABLE, 0x3);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x4);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0x8);
	    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xc);
	}
	
    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 

    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 

    //SGQ_SPLIT(SPLIT16);
}

/*
void SGQ_SPECIALCH_DIS(unsigned int mode, unsigned char specialMode)
{
    //special position setting
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0,SPECIAL_DISABLE ,SGQ_DISABLE, 0x0);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1,SPECIAL_DISABLE ,SGQ_DISABLE, 0xc);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2,SPECIAL_DISABLE ,SGQ_DISABLE, 0xd);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3,SPECIAL_DISABLE ,SGQ_DISABLE, 0xe);

    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0,SPECIAL_DISABLE ,SGQ_DISABLE, 0x3);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1,SPECIAL_DISABLE ,SGQ_DISABLE, 0x7);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2,SPECIAL_DISABLE ,SGQ_DISABLE, 0xb);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3,SPECIAL_DISABLE ,SGQ_DISABLE, 0xf);

	SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, specialMode);	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, specialMode);	
}*/

/*
void SGQ_SPECIALCH_INIT(unsigned int mode)
{
    //FPGA0 temination mode
    SGQ_RECDISPLAY_INIT(FPGA0, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA0, SGQ_DISABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA0, SGQ_ENABLE);

    //FPGA1 slave mode
    SGQ_RECDISPLAY_INIT(FPGA1, mode);
    SGQ_DISPLAY_SlaveMode_SET(FPGA1, SGQ_ENABLE);
    SGQ_DISPLAY_Terminal_SET(FPGA1, SGQ_DISABLE);

    //special position setting
//    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0,SPECIAL_ENABLE  ,SGQ_ENABLE, 0x0);
//    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0x3);
//    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0x7);
//    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xb);

//    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0,SPECIAL_DISABLE ,SGQ_ENABLE, 0xc);
//    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1,SPECIAL_DISABLE ,SGQ_ENABLE, 0xd);
//    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2,SPECIAL_DISABLE ,SGQ_ENABLE, 0xe);
//    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3,SPECIAL_DISABLE ,SGQ_ENABLE, 0xf);

    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 

    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10, 0x80 , 0x80); 

    //SGQ_SPLIT(SPLIT16);
}*/

/**
  * @brief  SGQ-1180 EVB (METIS) software for Zoom Enable
  * @param  None
  * @retval None
  */
void SGQ_LED_INIT(void)
{
    I2C_WRITE(FPGA0_ADDR, STATUSLED, 0x88888888);
    I2C_WRITE(FPGA1_ADDR, STATUSLED, 0x88888888);
}

/**
  * @brief  SGQ-1180 EVB (METIS) software for Zoom Enable
  * @param  None
  * @retval None
  */
void DispFrameStartCheck(char fpga_addr)
{
	unsigned int read_buffer;
	read_buffer = (read_buffer | 0x2000); // VsyncDisplay clear
    I2C_WRITE(fpga_addr, INTSTAT, read_buffer);
	
	while(!((I2C_READ(fpga_addr,INTSTAT) & 0x2000)>>13));
/*	
	while(1)
	{
		read_buffer = (I2C_READ(fpga_addr,INTSTAT) & 0x2000);
		if(read_buffer)
			break;
	}
*/	
}

/**
  * @brief  SGQ-1180 EVB (METIS) software for Zoom Enable
  * @param  None
  * @retval None
  */
void ZoomEnable(char fpga_addr)
{
	unsigned int read_buffer;

    read_buffer = I2C_READ(fpga_addr, DISPLAYCFG);
	read_buffer = (read_buffer | 0x00010000); // zoom enable
    I2C_WRITE(fpga_addr, DISPLAYCFG, read_buffer);
}

/**
  * @brief  SGQ-1180 EVB (METIS) software for Zoom Disable
  * @param  None
  * @retval None
  */
void ZoomDisable(char fpga_addr)
{
	unsigned int read_buffer;

    read_buffer = I2C_READ(fpga_addr, DISPLAYCFG);
	read_buffer = (read_buffer & 0xfffeffff); // zoom disable
    I2C_WRITE(fpga_addr, DISPLAYCFG, read_buffer);
}

/**
  * @brief  SGQ-1180 EVB (METIS) software for Zoom Configuration
  * @param  None
  * @retval None
  */
void ZoomConfig(char fpga_addr ,int zoomPosX, int zoomPosY, int zoomHRatio, int zoomVRatio, int zoomWidth, int zoomHeight)
{
	unsigned int read_buffer;

//	ZoomDisable(fpga_addr);
	DispFrameStartCheck(fpga_addr); //wait display start

    I2C_WRITE(fpga_addr, ZOOMPOS, ((zoomPosY<<16)   | (zoomPosX  )) );
    I2C_WRITE(fpga_addr, ZOOMH ,  ((zoomHRatio<<16) | (zoomWidth )) );
    I2C_WRITE(fpga_addr, ZOOMV ,  ((zoomVRatio<<16) | (zoomHeight)) );
	ZoomEnable(fpga_addr);

    printf("DEMO Exit 'Any Key press' : ZOOM DEMO  \n\r");

//	printf("zoomPosY %d ,zoomPosX %d ,zoomHRatio %d ,zoomVRatio %d ,zoomWidth %d ,zoomHeight %d \r\n",
//					zoomPosY,zoomPosX,zoomHRatio,zoomVRatio,zoomWidth,zoomHeight);
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Zoom In/Out
  * @param  None
  * @retval None
  */
extern int cmode;
int ZoomInOut(char fpga_addr, char src720p)
{
	int i,idx;
	char tmp;
	int zoomHRatio;
	int zoomVRatio;
	float dstWidth;
	float dstHeight;

	const unsigned int pos[]={100, 200, 300, 400};
	const unsigned int width[] ={1080, 880,  680,   480,  // src 1280
								 1720, 1520, 1320, 1120}; // src 1920

	const unsigned int height[]={ 570,	420,  270,  120,  // src 720
								  880,  680,  480,  280}; // src 1080

	if(cmode==2) {
		dstWidth  = 1279.0;
		dstHeight = 719.0;
	}
	else {
		dstWidth  = 1919.0;
		dstHeight = 1079.0;
	}
		
	// zoom in
	for(i=0;i<4;i++)
	{	
/*		//demo end
		if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)){
			tmp = (USART_ReceiveData(USART1) & 0x7F);	
			ZoomDisable(fpga_addr);
			return 1;
		}
*/		
		//StatusChecker();

		if(src720p || cmode==2) idx = i;
		else 		idx = i+4;
		zoomHRatio = ((float)((width[idx]-1 )/dstWidth))*32768;
		zoomVRatio = ((float)((height[idx]-1)/dstHeight))*32768;
		ZoomConfig(fpga_addr, pos[i], pos[i], zoomHRatio, zoomVRatio, width[idx],height[idx]);	
        Delay_ms(1200);
	}
	
	// zoom out
	for(i=3;i>0;i--)
	{	
/*		//demo end
		if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)){
			tmp = (USART_ReceiveData(USART1) & 0x7F);	
			ZoomDisable(fpga_addr);
			return 1;
		}	
*/
		if(src720p || cmode==2) idx = i-1;
		else 		idx = i+3;
		//StatusChecker();
		zoomHRatio = ((float)((width[idx]-1 )/dstWidth))*32768;
		zoomVRatio = ((float)((height[idx]-1)/dstHeight))*32768;
		ZoomConfig(fpga_addr, pos[i-1], pos[i-1], zoomHRatio, zoomVRatio, width[idx],height[idx]);	
        Delay_ms(1200);
	}

	ZoomDisable(fpga_addr);
	return 0;
}


/**
  * @brief  SGQ-1180 EVB (METIS) software for Record mode setting
  * @param  InFORM : 0-BT.1120 / 1-BT.656
  *         
  * @retval None
  */
void SGQ_RECMODE_BT656_BT1120_SET(char InFORM)
{

    unsigned int read_buffer;

    read_buffer = I2C_READ(FPGA1_ADDR, BT656Output);

    if(InFORM == 1) read_buffer = (read_buffer & 0xFFFFFF00) | 0xFF;
    else            read_buffer = (read_buffer & 0xFFFFFFF0);

    I2C_WRITE(FPGA1_ADDR, BT656Output, read_buffer);
}





const unsigned char position_data81[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
const unsigned char position_data8[]  = {0, 1, 2, 5, 8, 7, 6, 3};
const unsigned char position_data161[]= {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                                         0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
const unsigned char position_data16[] = {0x0, 0x1, 0x2, 0x3, 0x8, 0x9, 0xA, 0xB,
                                         0x4, 0x5, 0x6, 0x7, 0xC, 0xD, 0xE, 0xF,
                                         0x0, 0x5, 0xA, 0xF, 0x3, 0x6, 0x9, 0xC,
                                         0x4, 0x5, 0x6, 0x7, 0xC, 0xD, 0xE, 0xF};

unsigned char position_data_left_top[]  = {0x0, 0x3, 0x7, 0xb, 0xc, 0xd, 0xe, 0xf};
unsigned char position_data_right_top[] = {0x3, 0x0, 0x4, 0x8, 0xc, 0xd, 0xe, 0xf};
unsigned char position_data_left_bot[]  = {0xc, 0x0, 0x1, 0x2, 0x3, 0x7, 0xb, 0xf};
unsigned char position_data_right_bot[] = {0xf, 0x0, 0x1, 0x2, 0x3, 0x4, 0x8, 0xc};
unsigned char position_data_center[]    = {0x5, 0x0, 0x1, 0x2, 0x3, 0x4, 0x7, 0x8, 0xb, 0xc, 0xd, 0xe, 0xf};

/*int Delay(__IO uint32_t nTime)
{ 
  //TimingDelay = nTime;
	int i = nTime * 1000;
	unsigned char tmp;
	while(i != 0){
		if(!(USART_GetFlagStatus(USARTy, USART_FLAG_RXNE) == RESET)){
			tmp = (USART_ReceiveData(USARTy) & 0x7F);
			if(tmp == 0x1b){
				printf("DEMO END\n\r");
				return 1;
			}
		}
  		i--;
	}
	return 0;

  //while(TimingDelay != 0);
}*/


void ForceColor(unsigned char mode)
{
    //channel color
    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_ENABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 


	switch(mode  & 0xf) 
	{ 
		case SPLIT1 : 	SGQ_SPLIT(SPLIT1);	 break;
		case SPLIT9 :   SGQ_SPLIT(SPLIT9);   break;
		case SPLIT16 :  SGQ_SPLIT(SPLIT16);  break;
		default   :     SGQ_SPLIT(SPLIT4);   break; 
	} 
}




void ForceColorDis(unsigned char mode)
{
    //channel color
    //if((mode & 0xf) == SPLIT4) 
    //{
    //SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_ENABLE , 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_ENABLE , 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_ENABLE , 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_ENABLE , 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    //} else {

    SGQ_CH_Color(FPGA0 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA0 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH0, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH1, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH2, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 
    SGQ_CH_Color(FPGA1 | SDI_CH3, SGQ_DISABLE, 0xe, 0x10/*y*/, 0x80 /*Cb*/, 0x80 /*Cr*/); 

    //}

	switch(mode  & 0xf) 
	{ 
		case SPLIT1 : 	SGQ_SPLIT(SPLIT1);	 break;
		case SPLIT9 :   SGQ_SPLIT(SPLIT9);   break;
		case SPLIT16 :  SGQ_SPLIT(SPLIT16);  break;
		default   :     SGQ_SPLIT(SPLIT4);   break; 
	} 
}

static void ChangePositionSpecial(unsigned char data, unsigned char *tb, unsigned char specialMode)
{
/*    unsigned char position[8];
	unsigned char specialEn[8];
    unsigned char i,idx;
    for(i=0; i<8; i++) 
	{
        if(data > i) position[i] = (8 - data) + i;
        else         position[i] = i - data;
*//*			
        if((data + i) >7 ) position[i] = (data + i) - 8;
        else               position[i] =  data + i;
*/		
/*		if(data==i) specialEn[i] = 1;
		else specialEn[i] = 0;

//		printf("SpecialCh%d ,i%d, pos%d, position %d,specialEn%d \r\n",data,i,position[i],tb[position[i]],specialEn[i]);
    }
*/	SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, specialMode);	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, specialMode);	
	
    //16ch position setting
/*    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0, specialEn[0], SGQ_ENABLE, tb[position[0]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1, specialEn[1], SGQ_ENABLE, tb[position[1]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2, specialEn[2], SGQ_ENABLE, tb[position[2]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3, specialEn[3], SGQ_ENABLE, tb[position[3]], SPLIT16);

    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0, specialEn[4], SGQ_ENABLE, tb[position[4]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1, specialEn[5], SGQ_ENABLE, tb[position[5]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2, specialEn[6], SGQ_ENABLE, tb[position[6]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3, specialEn[7], SGQ_ENABLE, tb[position[7]], SPLIT16);
*/	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_ENABLE, specialMode);	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_ENABLE, specialMode);	

    //16ch split
    SGQ_SPLIT(SPLIT16);
}


static void ChangePositionSpecial0(unsigned char data,unsigned char specialPos, unsigned char *tb, unsigned char specialMode)
{
    unsigned char position[8];
	unsigned char specialEn[8];
    unsigned char i,idx;
    for(i=0; i<8; i++) 
	{
        if((data + i) >7 ) position[i] = (data + i) - 8;
        else               position[i] =  data + i;
		if(tb[position[0]]==specialPos) specialEn[i] = 1;
		else specialEn[i] = 0;
//		printf("i%d,specialPos%d, pos%d, position %d,specialEn%d \r\n",i,specialPos,position[i],tb[position[i]],specialEn[i]);
    }
	SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, specialMode);	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, specialMode);	
	
    //16ch position setting
/*    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH0, specialEn[0], SGQ_ENABLE, tb[position[0]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH1, specialEn[1], SGQ_ENABLE, tb[position[1]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH2, specialEn[2], SGQ_ENABLE, tb[position[2]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA0 | SDI_CH3, specialEn[3], SGQ_ENABLE, tb[position[3]], SPLIT16);

    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH0, specialEn[4], SGQ_ENABLE, tb[position[4]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH1, specialEn[5], SGQ_ENABLE, tb[position[5]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH2, specialEn[6], SGQ_ENABLE, tb[position[6]], SPLIT16);
    SGQ_CH_POSITION_SPECIAL(FPGA1 | SDI_CH3, specialEn[7], SGQ_ENABLE, tb[position[7]], SPLIT16);
*/	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_ENABLE, specialMode);	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_ENABLE, specialMode);	

    //16ch split
    SGQ_SPLIT(SPLIT16);
}


static void ChangePosigion16(unsigned char data, unsigned char *tb)
{
    unsigned char position[9];
    unsigned char i;

    for(i=0; i<8; i++) 
	{
        if((data + i) >16) position[i] = (data + i) - 16;
        else               position[i] =  data +i;
    }

    //16ch position setting
    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, tb[position[0]], SPLIT16);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, tb[position[1]], SPLIT16);
    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, tb[position[2]], SPLIT16);
    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, tb[position[3]], SPLIT16);

    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, tb[position[4]], SPLIT16);
    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, tb[position[5]], SPLIT16);
    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, tb[position[6]], SPLIT16);
    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, tb[position[7]], SPLIT16);

    //16ch split
    SGQ_SPLIT(SPLIT16);
}



static void ChangePosigion9(unsigned char data, unsigned char *tb)
{
    unsigned char position[9];
    unsigned char i;

    for(i=0; i<8; i++) 
	{
        if((data + i) >8) position[i] = (data + i) - 8;
        else              position[i] = data +i;
    }

    //8ch position setting
    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, tb[position[0]], SPLIT9);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, tb[position[1]], SPLIT9);
    SGQ_CH_POSITION(FPGA0 | SDI_CH2, SGQ_ENABLE, tb[position[2]], SPLIT9);
    SGQ_CH_POSITION(FPGA0 | SDI_CH3, SGQ_ENABLE, tb[position[3]], SPLIT9);

    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, tb[position[4]], SPLIT9);
    SGQ_CH_POSITION(FPGA1 | SDI_CH1, SGQ_ENABLE, tb[position[5]], SPLIT9);
    SGQ_CH_POSITION(FPGA1 | SDI_CH2, SGQ_ENABLE, tb[position[6]], SPLIT9);
    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, tb[position[7]], SPLIT9);

    //9ch split
    SGQ_SPLIT(SPLIT9);
}

static void ChangePosigion4(unsigned char data)
{
    unsigned char position[4];
    unsigned char i;

    for(i=0; i<4; i++) 
	{
        if((data + i) >4) position[i] = (data + i) - 4;
        else              position[i] = data +i;
    }

    //4ch position setting
    
    SGQ_CH_POSITION(FPGA0 | SDI_CH0, SGQ_ENABLE, position[0], SPLIT4);
    SGQ_CH_POSITION(FPGA0 | SDI_CH1, SGQ_ENABLE, position[1], SPLIT4);
    SGQ_CH_POSITION(FPGA1 | SDI_CH0, SGQ_ENABLE, position[2], SPLIT4);
    SGQ_CH_POSITION(FPGA1 | SDI_CH3, SGQ_ENABLE, position[3], SPLIT4);

    //9ch split
    SGQ_SPLIT(SPLIT4);
}


void FullScreen(unsigned char data)
{
    unsigned int ch_num;
    switch(data) 
	{
        case 0x0:    ch_num = (FPGA0 | SDI_CH0); break;
        case 0x1:    ch_num = (FPGA0 | SDI_CH1); break;
        case 0x2:    ch_num = (FPGA0 | SDI_CH2); break;
        case 0x3:    ch_num = (FPGA0 | SDI_CH3); break;
        case 0x4:    ch_num = (FPGA1 | SDI_CH0); break;
        case 0x5:    ch_num = (FPGA1 | SDI_CH1); break;
        case 0x6:    ch_num = (FPGA1 | SDI_CH2); break;
        case 0x7:    ch_num = (FPGA1 | SDI_CH3); break;
    }


    if(SGQ_READ_CH_LOCK(ch_num) == SGQ_LOCK) 
	{
	    ForceColor(SPLIT1);
        SGQ_CH_FULL(ch_num);
        //printf("Full Screen : Locking channel FPGA%d [0x%x] \n\r", (ch_num >> 8) & 0x1,  ch_num & 0xf);
	    Delay_ms(FORCECOLOR_DLY);
    	ForceColorDis(SPLIT1);
    } 
	else 
	{
        SGQ_CH_FULL(ch_num);
        //printf("NOT CONNECT FPGA%d SDI%d - Press any key !! \n\r", (ch_num >> 8) & 0x1, ch_num & 0xf);
    }
}


static void FullScreen9(unsigned char data)
{
    unsigned int ch_num;
    switch(data) 
	{
        case 0x0:    ch_num = (FPGA0 | SDI_CH0); break;
        case 0x1:    ch_num = (FPGA0 | SDI_CH1); break;
        case 0x2:    ch_num = (FPGA0 | SDI_CH2); break;
        case 0x3:    ch_num = (FPGA0 | SDI_CH3); break;
        case 0x4:    ch_num = (FPGA1 | SDI_CH0); break;
        case 0x5:    ch_num = (FPGA1 | SDI_CH1); break;
        case 0x6:    ch_num = (FPGA1 | SDI_CH2); break;
        case 0x7:    ch_num = (FPGA1 | SDI_CH3); break;
    }

    if(SGQ_READ_CH_LOCK(ch_num) == SGQ_LOCK) 
	{
	    ForceColor(SPLIT1);
        SGQ_CH_FULL(ch_num);
        printf("DEMO Exit 'Any Key press' : Locking channel FPGA%d [0x%x]\n\r", (ch_num >> 8) & 0x1,  ch_num & 0xf);
	    Delay_ms(FORCECOLOR_DLY);
    	ForceColorDis(SPLIT1);
    }
}


static void FullScreen4(unsigned char data)
{
    unsigned int ch_num;
    switch(data) 
	{
        case 0x0:    ch_num = (FPGA0 | SDI_CH0); break;
        case 0x1:    ch_num = (FPGA0 | SDI_CH1); break;
        case 0x2:    ch_num = (FPGA1 | SDI_CH0); break;
        case 0x3:    ch_num = (FPGA1 | SDI_CH3); break;
    }

    if(SGQ_READ_CH_LOCK(ch_num) == SGQ_LOCK) 
	{
 	    ForceColor(SPLIT1);
        SGQ_CH_FULL(ch_num);
        printf("DEMO Exit 'Any Key press' : Locking channel FPGA%d [0x%x]\n\r", (ch_num >> 8) & 0x1,  ch_num & 0xf);
		Delay_ms(FORCECOLOR_DLY);
		ForceColorDis(SPLIT1);
    }
}


unsigned int demo4ch(unsigned char data)
{
    unsigned char pos = 0, tmp = 0, full_num = 0, i;
    unsigned int tmp32, delay_time, exiten = 0;

    delay_time = 10000;

	printf("----------------------------------------------------------------------\n\r");
	printf(" 4 channel display demo [EXIT : AnyKey press !!!!] \n\r");
	printf("----------------------------------------------------------------------\n\r");

    SGQ_4CH_INIT(change_mode[cmode]);

    while(1) 
	{
        //demo end
        /*if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET))
		{
		    tmp = (USART_ReceiveData(USART1) & 0x7F);	

            if(data == 0xff) return 1;
            else             return 0;
        }*/

        //StatusChecker(); //peri status checking
		VideoProcessHandler();	// video process handler
		MDINDLY_mSec(1);		// delay for HDMI-Tx register !! //by hungry 2012.02.27
		VideoHTXCtrlHandler();	// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.

        //position changer
        if(pos<4) 
		{
            ForceColor(SPLIT4);
	        Delay_ms(5);
            printf("DEMO Exit 'Any Key press' : 4Ch Position display demo [%x] \n\r", pos);
            ChangePosigion4(pos++);
            ForceColorDis(SPLIT4);
            delay_time = 800;
	        Delay_ms(delay_time);
        }

        //full screen changer
        if((full_num < 4) & (pos == 4)) 
		{
            FullScreen4(full_num++); delay_time = 800;
	        Delay_ms(delay_time);
        }

        //clear position 
        if((full_num == 4) & (pos == 4)) 
		{
            full_num = 0; pos = 0;
            if(data == 0xff) return 0;
        }
    }
}

unsigned int demo9ch(unsigned char data)
{
    unsigned char pos = 0, tmp = 0, full_num = 0, i;
    unsigned int tmp32, delay_time, exiten = 0;

    delay_time = 10000;

	printf("----------------------------------------------------------------------\n\r");
	printf(" 8 channel display demo [EXIT : AnyKey press !!!!] \n\r");
	printf("----------------------------------------------------------------------\n\r");

    //SGQ_1080i_9CH_INIT();
    SGQ_9CH_INIT(change_mode[cmode]);

	while(1) 
	{
        //demo end
        /*if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET))
		{
		    tmp = (USART_ReceiveData(USART1) & 0x7F);	

            if(data == 0xff) return 1;
            else             return 0;
        }*/

        //StatusChecker(); //peri status checking
		VideoProcessHandler();	// video process handler
		MDINDLY_mSec(1);		// delay for HDMI-Tx register !! //by hungry 2012.02.27
		VideoHTXCtrlHandler();	// video HDMI-TX handler	//maybe error is occured when register read speed is very fast.

        //position changer
        if(pos<8) 
		{
            ForceColor(SPLIT9);
	        Delay_ms(5);
            printf("DEMO Exit 'Any Key press' : 8Ch Position display demo [%x] \n\r", pos);
            ChangePosigion9(pos++, (unsigned char *)position_data8);
            ForceColorDis(SPLIT9);
            delay_time = 800;
	        Delay_ms(delay_time);
        }

        //full screen changer
        if((full_num < 8) & (pos == 8)) 
		{
            FullScreen9(full_num++); delay_time = 800;
	        Delay_ms(delay_time);
        }

        //clear position 
        if((full_num == 8) & (pos == 8)) 
		{
            full_num = 0; pos = 0;
            if(data == 0xff) return 0;
        }
    }
}


unsigned int demo16ch(unsigned char data)
{
    unsigned char pos = 0, tmp = 0, full_num = 0, i;
    unsigned int tmp32, delay_time, exiten = 0;

    delay_time = 10000;

	printf("----------------------------------------------------------------------\n\r");
	printf(" 16 channel display demo [EXIT : AnyKey press !!!!] \n\r");
	printf("----------------------------------------------------------------------\n\r");

    //SGQ_1080i_16CH_INIT();
    //printf("%x---- %x \r\n", cmode, change_mode[cmode]);
    SGQ_16CH_INIT(change_mode[cmode]);

    while(1) 
	{
        //demo end
        /*if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET))
		{
		    tmp = (USART_ReceiveData(USART1) & 0x7F);	

            if(data == 0xff) return 1;
            else             return 0;
        }*/

        //StatusChecker(); //peri status checking

        //position changer
        if(pos<16) 
		{
            ForceColor(SPLIT16);
	        Delay_ms(5);
            printf("DEMO Exit 'Any Key press' : 16Ch Position display demo [%x] \n\r", pos);
            ChangePosigion16(pos++, (unsigned char *)position_data16);
            ForceColorDis(SPLIT16);
            delay_time = 1200;
	        Delay_ms(delay_time);
        }

        //full screen changer
        if((full_num < 8) & (pos == 16)) 
		{
            FullScreen9(full_num++); delay_time = 800;
	        Delay_ms(delay_time);
        }

        //clear position 
        if((full_num == 8) & (pos == 16)) 
		{
            full_num = 0; pos = 0;
            if(data == 0xff) return 0;
		}
    }
}

unsigned int demoSpecialSplit(unsigned char data, unsigned char specialMode)
{
    unsigned char pos = 0, tmp = 0, full_num = 0, i;
    unsigned int tmp32, delay_time, exiten = 0;
	unsigned char *position_data;
	unsigned char validChannel[8];
	unsigned char totalValidCh=0;
	unsigned int ch_num,idx=0,cnt=0;


    delay_time = 10000;

//	printf("----------------------------------------------------------------------\n\r");
//	printf(" 4:3 split display demo %d [EXIT : AnyKey press !!!!] \n\r",specialMode);
//	printf("----------------------------------------------------------------------\n\r");
	for(i=0;i<8;i++) 
	{
		switch(i) 
		{
			case 0x0:    ch_num = (FPGA0 | SDI_CH0); break;
			case 0x1:    ch_num = (FPGA0 | SDI_CH1); break;
			case 0x2:    ch_num = (FPGA0 | SDI_CH2); break;
			case 0x3:    ch_num = (FPGA0 | SDI_CH3); break;
			case 0x4:    ch_num = (FPGA1 | SDI_CH0); break;
			case 0x5:    ch_num = (FPGA1 | SDI_CH1); break;
			case 0x6:    ch_num = (FPGA1 | SDI_CH2); break;
			default :    ch_num = (FPGA1 | SDI_CH3); break;
		}

		if(SGQ_READ_CH_LOCK(ch_num) == SGQ_LOCK) 
		{
			validChannel[idx] = i;
			totalValidCh++;
			idx++;
			//printf("chNum %d , idx %d :: ",totalValidCh,idx);
	        //printf("DEMO Exit 'Any Key press' : Locking channel FPGA%d [0x%x] i%d\n\r", (ch_num >> 8) & 0x1,  ch_num & 0xf,i);
    	}
	}
	if(totalValidCh==0)
	{
        if(data == 0xff) return 1;
        else             return 0;
	}
	//printf("Input Total %d \r\n",totalValidCh);

	switch(specialMode)
	{
		case 0x0 : position_data = position_data_left_top;  break;
		case 0x1 : position_data = position_data_right_top; break;
		case 0x2 : position_data = position_data_left_bot;  break;
		case 0x3 : position_data = position_data_right_bot; break;
		default  : position_data = position_data_center;    break; 
	}

	SGQ_SPECIALCH_INIT(change_mode[cmode]);
    delay_time = 2500;
//    Delay_ms(delay_time);

 	while(1) 
	{
        //demo end
        //if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET))
		//{
		//    tmp = (USART_ReceiveData(USART1) & 0x7F);	

        //    if(data == 0xff) return 1;
        //    else             return 0;
        //}

        //StatusChecker(); //peri status checking

        //position changer
        if(cnt<totalValidCh) 
		{
            ForceColor(SPLIT16);
            //printf("DEMO Exit 'Any Key press' : 3/4 Position display demo \n\r");
            ChangePositionSpecial(validChannel[cnt], position_data,specialMode);
	        //Delay_ms(10);
	       //Delay_ms(1);
            ForceColorDis(SPLIT16);
	        //Delay_ms(delay_time);
	       //Delay_ms(1);
			cnt++;
        }
		else
			return 0;
    }
}


unsigned int demoSpecialSplit0(unsigned char data, unsigned char specialMode)
{
    unsigned char pos = 0, tmp = 0, full_num = 0, i;
    unsigned int tmp32, delay_time, exiten = 0;
	unsigned char *position_data;
	unsigned char specialPos;

    delay_time = 10000;

	printf("----------------------------------------------------------------------\n\r");
	printf(" special split display demo %d [EXIT : AnyKey press !!!!] \n\r",specialMode);
	printf("----------------------------------------------------------------------\n\r");
	
	switch(specialMode)
	{
		case 0x0 : {specialPos = 0;  position_data = position_data_left_top; break;}
		case 0x1 : {specialPos = 3;  position_data = position_data_right_top; break;}
		case 0x2 : {specialPos = 12; position_data = position_data_left_bot; break;}
		case 0x3 : {specialPos = 15; position_data = position_data_right_bot; break;}
		default  : {specialPos = 5 ; position_data = position_data_center; break;} 
	}

	SGQ_SPECIALCH_INIT(change_mode[cmode]);
    delay_time = 2500;
    Delay_ms(delay_time);

	while(1) 
	{
        //demo end
        /*if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET))
		{
		    tmp = (USART_ReceiveData(USART1) & 0x7F);	

            if(data == 0xff) return 1;
            else             return 0;
        }*/

        //StatusChecker(); //peri status checking

        //position changer
        if(pos<8) 
		{
            ForceColor(SPLIT16);
	        Delay_ms(5);
            printf("DEMO Exit 'Any Key press' : 3/4 Position display demo [%x] \n\r", pos);
            ChangePositionSpecial0(pos++, specialPos,position_data,specialMode);
	        Delay_ms(5);
            ForceColorDis(SPLIT16);
	        Delay_ms(delay_time);
        }
		else
			return 0;
    }
}

int demoSpecial(int data)
{
	int i;
	unsigned char retVal;
//	while(1)
//	{
/*			
        if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)){
		    tmp = (USART_ReceiveData(USART1) & 0x7F);	
            if(data == 0xff) return 1;
            else             return 0;
        }
*/		
		retVal=demoSpecialSplit(0xff,LEFT_TOP); 
		if(retVal)
		{
			SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, 0);	
			SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, 0);	
		   	return retVal;
		}

/*		retVal=demoSpecialSplit(0xff,RIGHT_TOP);
		if(retVal)
		{
			SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, 0);	
			SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, 0);	
		   	return retVal;
		}

		retVal=demoSpecialSplit(0xff,LEFT_BOT);	
		if(retVal)
		{
			SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, 0);	
			SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, 0);	
		   	return retVal;
		}

		retVal=demoSpecialSplit(0xff,RIGHT_BOT);
		if(retVal)
		{
			SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, 0);	
			SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, 0);	
		   	return retVal;
		}

		retVal=demoSpecialSplit(0xff,CENTER); 	
		if(retVal)
		{
			SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, 0);	
			SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, 0);	
		   	return retVal;
		}

		if(data) break;
*/
/*		
		if(demoSpecialSplit(0xff,LEFT_TOP))  return retVal;
		if(demoSpecialSplit(0xff,RIGHT_TOP)) return retVal;
		if(demoSpecialSplit(0xff,LEFT_BOT))	 return retVal;
		if(demoSpecialSplit(0xff,RIGHT_BOT)) return retVal;
		if(demoSpecialSplit(0xff,CENTER)) 	 return retVal;
*/		
//	}

	SGQ_DISPLAY_SpecialLoc_SET(FPGA0, SGQ_DISABLE, 0);	
	SGQ_DISPLAY_SpecialLoc_SET(FPGA1, SGQ_DISABLE, 0);	

	return 0;
}

int ZoomDemoMenu(unsigned char chNum)
{
	int i=0;
	unsigned char tmp = 0;
    unsigned int ch_num,valid_ch;
	unsigned int ch_width;
	unsigned char src720p;	
	unsigned char fpga_addr;
	
	printf("----------------------------------------------------------------------\n\r");
	printf(" Zoom DEMO [EXIT : AnyKey press !!!!] \n\r");
	printf("----------------------------------------------------------------------\n\r");
	if(cmode==1) 
	{
		printf("----- Not support interlace!!!! ----- \n\r");
		return 0;
	}

	i=0;

	if(SGQ_FULL_CHECK())
	{
		printf("----- Current display mode isn't FULL mode!!!! ----- \n\r");
		return 0;
	}

	switch(chNum) 
	{
		case 0x0:    ch_num = (FPGA0 | SDI_CH0); break;
		case 0x1:    ch_num = (FPGA0 | SDI_CH1); break;
		case 0x2:    ch_num = (FPGA0 | SDI_CH2); break;
		case 0x3:    ch_num = (FPGA0 | SDI_CH3); break;
		case 0x4:    ch_num = (FPGA1 | SDI_CH0); break;
		case 0x5:    ch_num = (FPGA1 | SDI_CH1); break;
		case 0x6:    ch_num = (FPGA1 | SDI_CH2); break;
		case 0x7:    ch_num = (FPGA1 | SDI_CH3); break;
	}

	switch((ch_num >> 8) & 0xf) 
	{ 
		case 0x01 : fpga_addr = FPGA1_ADDR;  break;
		case 0x02 : fpga_addr = FPGA2_ADDR;  break;
		case 0x03 : fpga_addr = FPGA3_ADDR;  break;
		default   : fpga_addr = FPGA0_ADDR;  break; 
	} 
		
	if(SGQ_READ_CH_LOCK(ch_num) == SGQ_LOCK) 
	{
		printf("Full Screen : Locking channel [%d] \n\r", chNum);
		ch_width = SGQ_READ_CH_WIDTH(ch_num);
		if(ch_width==1280)
			src720p = 1;
		else
			src720p = 0;
		ZoomInOut(fpga_addr,src720p);
	} 
	else 
	{
		printf("NOT CONNECT SDI chennel [%d] \n\r", chNum);
	}
}


int ZoomDemo(unsigned char data)
{
	int i=0;
	unsigned char tmp = 0;
    unsigned int ch_num,valid_ch;
	unsigned int ch_width;
	unsigned char src720p;	
	unsigned char fpga_addr;
	
	printf("----------------------------------------------------------------------\n\r");
	printf(" Zoom DEMO [EXIT : AnyKey press !!!!] \n\r");
	printf("----------------------------------------------------------------------\n\r");
	if(cmode==1) 
	{
		printf("----- Not support interlace!!!! ----- \n\r");
		return 0;
	}

	i=0;

    while(1) 
	{
		/*if(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET))
		{
			tmp = (USART_ReceiveData(USART1) & 0x7F);	
			SGQ_CH_FULL(valid_ch);
            if(data == 0xff) return 1;
            else             return 0;
		}*/	

        //StatusChecker(); //peri status checking

		if(i==8)
		{
		   	i=0;
			SGQ_CH_FULL(valid_ch);
			if(data==0xff) return 0;
		}

		switch(i) 
		{
			case 0x0:    ch_num = (FPGA0 | SDI_CH0); break;
			case 0x1:    ch_num = (FPGA0 | SDI_CH1); break;
			case 0x2:    ch_num = (FPGA0 | SDI_CH2); break;
			case 0x3:    ch_num = (FPGA0 | SDI_CH3); break;
			case 0x4:    ch_num = (FPGA1 | SDI_CH0); break;
			case 0x5:    ch_num = (FPGA1 | SDI_CH1); break;
			case 0x6:    ch_num = (FPGA1 | SDI_CH2); break;
			case 0x7:    ch_num = (FPGA1 | SDI_CH3); break;
		}

		switch((ch_num >> 8) & 0xf)
		{ 
			case 0x01 : fpga_addr = FPGA1_ADDR;  break;
			case 0x02 : fpga_addr = FPGA2_ADDR;  break;
			case 0x03 : fpga_addr = FPGA3_ADDR;  break;
			default   : fpga_addr = FPGA0_ADDR;  break; 
		} 
		
		if(SGQ_READ_CH_LOCK(ch_num) == SGQ_LOCK) 
		{
			valid_ch = ch_num;
	 	    ForceColor(SPLIT1);

			SGQ_CH_FULL(ch_num);

			Delay_ms(FORCECOLOR_DLY);
			ForceColorDis(SPLIT1);

			printf("Full Screen : Locking channel FPGA%d [0x%x] \n\r", (ch_num >> 8) & 0x1,  ch_num & 0xf);
			ch_width = SGQ_READ_CH_WIDTH(ch_num);
			if(ch_width==1280)
				src720p = 1;
			else
				src720p = 0;

			if(ZoomInOut(fpga_addr,src720p))
				break;
		}
		else
		{
			printf("NOT CONNECT FPGA%d SDI%d \n\r", (ch_num >> 8) & 0x1, ch_num & 0xf);
		}

        Delay_ms(2000);

        i++;
	}
}

void FullDemo(void)
{		
    while(1) 
	{
        if(demo4ch(0xff) == 1) break; 
        if(demo9ch(0xff) == 1) break; 
        if(demo16ch(0xff) == 1) break; 
		if(ZoomDemo(0xff) == 1) break;
		if(demoSpecial(1) == 1) break;
    }
}

void FullDemo8Ch(void)
{		
    while(1) 
	{
        if(demo4ch(0xff) == 1) break; 
        if(demo9ch(0xff) == 1) break; 
//        if(demo16ch(0xff) == 1) break; 
		if(ZoomDemo(0xff) == 1) break;
		if(demoSpecial(1) == 1) break;
    }
}




#endif
