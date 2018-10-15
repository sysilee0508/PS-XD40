
/* Includes ------------------------------------------------------------------*/
#include "NVP6158.h"
#include "i2c.h"
#include "delay.h"
#include "video_fmt_input.h"
#include "video_fmt_output.h"


RAPTOR3_INFORMATION_S	s_raptor3_vfmts;

static char g_MergeEn;


NC_VIVO_CH_FORMATDEF arrVfcType[0x100] = {
	/*  0x00 */	AHD20_SD_H960_2EX_Btype_NT,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x10 */ AHD20_SD_H960_2EX_Btype_PAL,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x20 */	AHD20_720P_30P_EX_Btype, AHD20_720P_25P_EX_Btype, AHD20_720P_60P, AHD20_720P_50P,
				0,
	/*  0x25 */ TVI_HD_30P_EX, TVI_HD_25P_EX, TVI_HD_60P, TVI_HD_50P, TVI_HD_B_30P_EX, TVI_HD_B_25P_EX,
	/*  0x2B */	CVI_HD_30P_EX, CVI_HD_25P_EX, CVI_HD_60P, CVI_HD_50P,
				0,
	/*  0x30 */	AHD20_1080P_30P, AHD20_1080P_25P,
				0,
	/*	0x33 */ TVI_FHD_30P, TVI_FHD_25P,
	/*  0x35 */	CVI_FHD_30P, CVI_FHD_25P,
				0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x40 */	AHD30_3M_30P, AHD30_3M_25P, AHD30_3M_18P,
				0,
	/*  0x44 */ TVI_3M_18P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x50 */	AHD30_4M_30P, AHD30_4M_25P, AHD30_4M_15P,
				0,
	/*  0x54 */	CVI_4M_30P, CVI_4M_25P,
				0,
	/*  0x57 */ TVI_4M_30P, TVI_4M_25P, TVI_4M_15P,
				0, 0, 0, 0, 0, 0,
	/*  0x60 */	AHD30_8M_X_30P, AHD30_8M_X_25P, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x70 */	AHD30_5M_20P, AHD30_5M_12_5P, AHD30_5_3M_20P, TVI_5M_12_5P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x80 */	AHD30_8M_15P, AHD30_8M_7_5P, AHD30_8M_12_5P, CVI_8M_15P, CVI_8M_12_5P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0x90 */	AHD30_6M_18P, AHD30_6M_20P,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*  0xA0 */
};

unsigned char NVP6158_I2C_READ(unsigned char slaveaddr, unsigned char regaddr)
{

    return I2C_READ(slaveaddr, regaddr);
}

void NVP6158_I2C_WRITE(unsigned char slaveaddr, unsigned char regaddr, unsigned char write_data)
{
	I2C_WRITE(slaveaddr, regaddr, write_data);		
}

NC_VIVO_CH_FORMATDEF NC_VD_AUTO_VFCtoFMTDEF(unsigned char VFC)
{
	return arrVfcType[VFC];
}

void NC_VD_AUTO_AutoMode_Set(unsigned char ch, unsigned char DevNum)
{
	unsigned char oChannel = ch%4;
	unsigned char oDevNum = DevNum;
	video_input_auto_detect VinAutoDet;

	VinAutoDet.ch = oChannel;
	VinAutoDet.devnum = oDevNum;

	video_input_auto_detect_set(&VinAutoDet);
}

NC_VO_PORT_FMT_S *NC_HI_VO_Get_PortFormat( NC_VIVO_CH_FORMATDEF def )
{
	NC_VO_PORT_FMT_S *pRet = &VD_VO_PortFormatDefs[ def ];

	return  pRet;
}

void NC_VD_VO_Mode_Set_New( unsigned char ch, unsigned char devnum, unsigned char port, NC_VO_PORT_FMT_S *pPortFmt,
						  unsigned char mux_mode,
						  unsigned char output_interface,
						  unsigned char edge,
						  unsigned char SEQ0, unsigned char SEQ1, unsigned char SEQ2, unsigned char SEQ3)
{
	clock_video_output VOutClk;
	video_output_sequence VOutSeq;
	video_output_port_mux_chid VoutChID;
	video_output_port_mux_mode VOutPortMuxMode;
	video_output_port_enable_s sPortEnable;
	video_output_sequence_reg 	sVOutSeqreg;
	video_output_port_fmt_s sPortFmt;

	VOutSeq.devnum = devnum;
	VoutChID.devnum = devnum;
	sVOutSeqreg.devnum = devnum;

	/********************************
	 * Set sequence
	********************************/
	if(output_interface == VI_INPUT_MODE_BT656) // BT656 Port Setting
	{
		if(mux_mode == VI_2MULTIPLEX_MODE)
		{
			video_output_channel_sequence_get(&sVOutSeqreg);
			if(port == PORTB)
			{
				if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				{
					VOutSeq.port = port;
					VOutSeq.ch01 = (ch==0)?(0x08|(sVOutSeqreg.b1c2&0xf0)):(0x90|(sVOutSeqreg.b1c2&0x0f));
					VOutSeq.ch23 = (ch==0)?(0x08|(sVOutSeqreg.b1c2&0xf0)):(0x90|(sVOutSeqreg.b1c2&0x0f));
				}
				else
				{
					VOutSeq.port = port;
					VOutSeq.ch01 = (ch==0)?(0x00|(sVOutSeqreg.b1c2&0xf0)):(0x10|(sVOutSeqreg.b1c2&0x0f));
					VOutSeq.ch23 = (ch==0)?(0x00|(sVOutSeqreg.b1c2&0xf0)):(0x10|(sVOutSeqreg.b1c2&0x0f));
				}
			}
			else if(port == PORTC)
			{
				if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				{
					VOutSeq.port = port;
					VOutSeq.ch01 = (ch==2)?(0x0a|(sVOutSeqreg.b1c4&0xf0)):(0xb0|(sVOutSeqreg.b1c4&0x0f));
					VOutSeq.ch23 = (ch==2)?(0x0a|(sVOutSeqreg.b1c4&0xf0)):(0xb0|(sVOutSeqreg.b1c4&0x0f));
				}
				else
				{
					VOutSeq.port = port;
					VOutSeq.ch01 = (ch==2)?(0x02|(sVOutSeqreg.b1c4&0xf0)):(0x30|(sVOutSeqreg.b1c4&0x0f));
					VOutSeq.ch23 = (ch==2)?(0x02|(sVOutSeqreg.b1c4&0xf0)):(0x30|(sVOutSeqreg.b1c4&0x0f));
				}
			}
			video_output_channel_sequence_set(&VOutSeq);

			if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				g_MergeEn |= (0x1 << ch);
			else
				g_MergeEn &= (~(0x1 << ch));
		}
		else
		{
			VOutSeq.port = port;
			VOutSeq.ch01 = ((SEQ0 & 0x0f) | ((SEQ1 & 0xf)<<4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ch23 = ((SEQ2 & 0x0f) | ((SEQ3 & 0xf)<<4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			video_output_channel_sequence_set(&VOutSeq);

			if( __IsOver3MRTVideoFormat( pPortFmt->name ) == 0 )
				g_MergeEn |= (0x1 << port);
			else
				g_MergeEn &= (~(0x1 << port));

		}
	}
	else // BT1120 Port Setting
	{
		/*
		0~3  :  Normal Display of Channel 1~4
		4~7  :  Only Y Display of Channel 1~4
		8~B  :  H_CIF Display of Channel 1~4
		C~F  :  Only C Display of Channel 1~4
		*/
		if(port == PORTA)
		{
			//Y Only Set for BT1120
			VOutSeq.port = PORTB;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ch01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ch23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			video_output_channel_sequence_set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.port = PORTA;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ch01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ch23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			video_output_channel_sequence_set(&VOutSeq);
		}
		else if(port == PORTC)
		{
			VOutSeq.port = PORTD;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ch01 = (((SEQ0 & 0x0f) + 4) | (((SEQ1 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ch23 = (((SEQ2 & 0x0f) + 4) | (((SEQ3 & 0x0f) + 4) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			video_output_channel_sequence_set(&VOutSeq);

			//C Only Set for BT1120
			VOutSeq.port = PORTC;		// PORT_A, PORTB, PORTC, PORTD
			VOutSeq.ch01 = (((SEQ0 & 0x0f) + 0x0c) | (((SEQ1 & 0x0f) + 0x0c) << 4));   // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			VOutSeq.ch23 = (((SEQ2 & 0x0f) + 0x0c) | (((SEQ3 & 0x0f) + 0x0c) << 4));  // [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
			video_output_channel_sequence_set(&VOutSeq);
		}
		else
		{

		}
		g_MergeEn |= (0x1 << port);
	}

	/********************************
	 * Mux YC merge
	 ********************************/
	sPortFmt.merge = g_MergeEn;
	sPortFmt.devnum = devnum;
	video_output_port_format_mode_set(&sPortFmt);

	/* Output Port Data Ch Number Selection */
	VOutPortMuxMode.devnum = devnum;
	VOutPortMuxMode.port = port;
	switch(mux_mode)
	{
		case VI_1MULTIPLEX_MODE :  VOutPortMuxMode.mode = 0; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
									//	VOutClk.ClkSel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_2MULTIPLEX_MODE :	VOutPortMuxMode.mode = 0x02; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
									//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
		case VI_4MULTIPLEX_MODE :  VOutPortMuxMode.mode = 0x08; 	//1MUX:0x0, 2MUX:0x02, 4MUX:0x08, PORTA/C[3:0], PORTB/D[7:4]
									//	VOutClk.ClkSel = 0x70;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
										break;
	}
	video_output_port_mux_mode_set(&VOutPortMuxMode);
	video_output_port_mux_chid_set(&VoutChID);

	/********************************
	 * Video Output Clock and delay
	 ********************************/
	if(output_interface == VI_INPUT_MODE_BT656)
	{
		VOutClk.port = port;	// PORT_A, PORTB, PORTC, PORTD
		VOutClk.devnum = devnum;
    	VOutClk.clk_sel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
		video_out_auto_vclk_set(&VOutClk);
    }
	else// BT1120
	{
		switch(port)
		{
			case PORTA :
				VOutClk.port = PORTA;	// PORT_A, PORTB, PORTC, PORTD
				 VOutClk.devnum = devnum;
				 VOutClk.clk_sel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				 video_out_auto_vclk_set(&VOutClk);
				 VOutClk.port = PORTB;	// PORT_A, PORTB, PORTC, PORTD
				 VOutClk.devnum = devnum;
				 VOutClk.clk_sel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				 video_out_auto_vclk_set(&VOutClk);
				 break;
			case PORTC :
				VOutClk.port = PORTC;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.clk_sel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				video_out_auto_vclk_set(&VOutClk);
				VOutClk.port = PORTD;	// PORT_A, PORTB, PORTC, PORTD
				VOutClk.devnum = devnum;
				VOutClk.clk_sel = pPortFmt->vo_clk;	//[7:4] Port Clock SEL, [3:0] Port Clock Delay
				video_out_auto_vclk_set(&VOutClk);
				break;
		}
    }

	/********************************
	 * Output Port CLK Enable
	 ********************************/
	sPortEnable.devnum = devnum;
	if(mux_mode == VI_2MULTIPLEX_MODE)
	{
		sPortEnable.enable = 0x66;
	}
	else
	{
		sPortEnable.enable = 0xff;
	}
	video_out_port_enable(&sPortEnable);
}

NC_VI_CH_FMT_S *NC_HI_VI_Get_ChannelFormat( NC_VIVO_CH_FORMATDEF def )
{

	NC_VI_CH_FMT_S *pRet = &VD_VI_ChannelFormatDefs[ def ];

	return  pRet;
}

NC_VIVO_CH_FORMATDEF NC_HI_VI_FindFormatDef( NC_FORMAT_STANDARD format_standard,
		NC_FORMAT_RESOLUTION format_resolution, NC_FORMAT_FPS format_fps  )
{
	int ii;

	if(format_standard == FMT_AUTO)
		return NC_VIVO_CH_FORMATDEF_AUTO;

	for(ii=0;ii<NC_VIVO_CH_FORMATDEF_MAX;ii++)
	{
		NC_VI_CH_FMT_S *pFmt = &VD_VI_ChannelFormatDefs[ ii ];

		if( pFmt->format_standard == format_standard )
		if( pFmt->format_resolution == format_resolution )
		if( pFmt->format_fps == format_fps )
			return ii;
	}

	return NC_VIVO_CH_FORMATDEF_UNKNOWN;
}

void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss)
{
	unsigned char 		oLogicalChannel = 0;

	for(oLogicalChannel = 0; oLogicalChannel < 4; oLogicalChannel++ )
	{
		*pVideoLoss |= s_raptor3_vfmts.videolosscheck[oLogicalChannel];
	}
}

NC_VIVO_CH_FORMATDEF NVP6158_Current_Video_Format_Check(unsigned char oLogicalChannel)
{

		return s_raptor3_vfmts.curvideofmt[oLogicalChannel];
}

void NVP6158_VideoDetectionProc(void)
{

	int ret;
	unsigned char		oDevNum = 0;
	unsigned char 		oLogicalChannel = 0;
	unsigned char 		oCurVideofmt = 0x00;
	unsigned char 		oPreVideofmt = 0x00;
	NC_VIVO_CH_FORMATDEF oFmtDef;
	NC_VIVO_CH_FORMATDEF oFmtB5Def;
	decoder_dev_ch_info_s sDevChInfo;
	video_output_data_out_mode DataOutMode;

	/* process video format on/off */
	for( oLogicalChannel = 0; oLogicalChannel < 4; oLogicalChannel++ )
	{
		/*****************************************************************
		*
		* This is Auto mode
		*
		*****************************************************************/
		/* get video format */
		RAPTOR3_SAL_GetFormatEachCh( oLogicalChannel, &s_raptor3_vfmts );

		oDevNum       = oLogicalChannel/4;
		oCurVideofmt  = s_raptor3_vfmts.curvideofmt[oLogicalChannel];
		oPreVideofmt  = s_raptor3_vfmts.prevideofmt[oLogicalChannel];

		if( s_raptor3_vfmts.curvideoloss[oLogicalChannel] == VIDEO_LOSS_ON )
		{
			if( (oCurVideofmt != NC_VIVO_CH_FORMATDEF_UNKNOWN) && (oPreVideofmt == NC_VIVO_CH_FORMATDEF_UNKNOWN) )
			{
				oFmtDef = NC_VD_AUTO_VFCtoFMTDEF( s_raptor3_vfmts.vfc[oLogicalChannel] );
				sDevChInfo.fmt_def = oFmtDef;

				sDevChInfo.ch = oLogicalChannel;
				sDevChInfo.devnum = oDevNum;
				if(oFmtDef == AHD30_5M_20P || oFmtDef == AHD30_3M_30P )
				{
					/* Read Bank13 0xA4, 0xA5 ---> integral_status_ahd_vbi[7:0], integral_status_ahd_vbi[15:8]  */
					/* integral_vbi level is Bank13 0x14 														*/
					/* If integral_status_ahd_vbi < threshold ----> TVI_5M_20P
					 * else AHD30_5M_20P Setting
					 */
					video_input_ahd_tvi_distinguish(&sDevChInfo);
					oFmtDef = sDevChInfo.fmt_def;

					if( oFmtDef == TVI_5M_20P )
					{
						s_raptor3_vfmts.curvideofmt[oLogicalChannel] = TVI_5M_20P;
					}
				}

				/* set video format(DEC) */
				RAPTOR3_SAL_OnVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

				video_input_manual_agc_stable_endi(&sDevChInfo,0);

				/* show decoder */
				DataOutMode.ch = oLogicalChannel;
				DataOutMode.set_val = 0x1;
				video_output_data_out_mode_set( &DataOutMode /*1 is OUT_MODE_ON*/ );

				/* save onvideo to prevideofmt */
				s_raptor3_vfmts.prevideofmt[oLogicalChannel] = s_raptor3_vfmts.curvideofmt[oLogicalChannel];

			}
			else
			{
				/* check debouce video format(-1:changed, 0:not checked) when you changed video format(Dahua-NVP2450). */
				ret = RAPTOR3_SAL_AutoDebouceCheck( oLogicalChannel, &s_raptor3_vfmts );
				if( ret == -1 )
				{
					sDevChInfo.ch = oLogicalChannel;
					sDevChInfo.devnum = oDevNum;
					sDevChInfo.fmt_def = oFmtB5Def;

					/* hide decoder */
					video_input_contrast_off(&sDevChInfo);

					DataOutMode.ch = oLogicalChannel;
					DataOutMode.set_val = 0xF;
					video_output_data_out_mode_set( &DataOutMode /*0xF is OUT_MODE_OFF*/ );

					/* decoder afe power down */
					video_input_vafe_control(&sDevChInfo, 0);
					/* set no video- first(i:channel, raptor3_vfmts:information */
					RAPTOR3_SAL_NoVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );
					/* decoder afe power up */
					video_input_vafe_control(&sDevChInfo, 1);

					/* for forced agc stable */
					video_input_manual_agc_stable_endi(&sDevChInfo, 1);
					Delay_ms(500);

					/* save onvideo to prevideofmt */
					s_raptor3_vfmts.prevideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

				}
			}
		}
		else
		{
			/* no video */
			//if( oPreVideofmt !=  NC_VIVO_CH_FORMATDEF_UNKNOWN )
			{
				/* hide decoder */
				DataOutMode.ch = oLogicalChannel;
				DataOutMode.set_val = 0xF;
				video_output_data_out_mode_set( &DataOutMode /*OUT_MODE_OFF*/);

				/* set no video- first(i:channel, raptor3_vfmts:information */
				RAPTOR3_SAL_NoVIdeoSetFormat( oLogicalChannel, &s_raptor3_vfmts );

				/* save novideo to prevideofmt */
				s_raptor3_vfmts.prevideofmt[oLogicalChannel] = NC_VIVO_CH_FORMATDEF_UNKNOWN;

			}
		}
	}
	//Delay_ms(200);
}

void NVP6158_init(void)
{
	int chip_id;
	unsigned char ch;
	unsigned char port;
	unsigned char val_9x44;

	chip_id = check_id(NVP6158_ADDR);
	if(chip_id != RAPTOR3_4PORT_R0_ID )
	{
		chip_id= 0xFF;
	}

	/* set black screen */
	video_output_colorbar_set();

	/* set initialization value  */
	for(ch = 0; ch < 4; ch++)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05+ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xD5, 0x80);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x00, 0xD0); //clamp speed
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x76, 0x00);
	}

	/* set NOVIDEO */
	for(ch = 0; ch < 4; ch++)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x09);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x80 + (ch * 0x20), 0x00);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x81 + (ch * 0x20), 0x00);

		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x05 + ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x2C, 0x00);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB8, 0xB8);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xB9, 0xB2);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x25, 0xDC);


		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xFF, 0x09);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x50 + (ch*4) , 0x30);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x51 + (ch*4) , 0x6F);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x52 + (ch*4) , 0x67);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x53 + (ch*4) , 0x48);

		val_9x44 = NVP6158_I2C_READ(NVP6158_ADDR, 0x44);
		val_9x44 &= ~(1 << ch);
		val_9x44 |= (1 << ch);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x44 , val_9x44);
	}

	if(chip_id == RAPTOR3_4PORT_R0_ID)
	{
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);
		/* set Port setting */
		for(port = 0; port < 4; port++)
		{
			/* port channel sequence set */
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc0 + (port * 2), (port | (port << 4)));
			NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc1 + (port * 2), (port | (port << 4)));
		}

		/* port 1mux set */
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc8, 0x00);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xc9, 0x00);

		/* all port enable */
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xca, 0xff);

		/* mux chid set */
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x00);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x55, 0x10);
		NVP6158_I2C_WRITE(NVP6158_ADDR, 0x56, 0x32);
	}

/* set auto mode, but if you want to change mode from auto to manual mode, check this!(2017-07-30) */
	for( ch = 0; ch < 4; ch++ )
	{
		NC_VD_AUTO_AutoMode_Set(ch, ch/4);
		Delay_ms(100);
	}
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0xff, 0x01);
	//NVP6158_I2C_WRITE(NVP6158_ADDR, 0xcb, 0x01);
	
	s_raptor3_vfmts.oMux = VI_1MULTIPLEX_MODE;
}


