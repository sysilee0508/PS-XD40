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
#ifndef _RAPTOR3_RAPTOR3_DRV_H_
#define _RAPTOR3_RAPTOR3_DRV_H_

#define PORTA		0x00
#define PORTB		0x01
#define PORTC		0x02
#define PORTD		0x03
#define PORTAB		0x04
#define PORTCD		0x05

#define SEQUENCE_CH01	0x00
#define SEQUENCE_CH23  0x01

#define VO_WORK_MODE_1MUX	0X00
#define VO_WORK_MODE_2MUX	0X02
#define VO_WORK_MODE_4MUX	0X08

typedef struct _NC_VD_VO_SEQ_STR{
	unsigned char Port;					// PORT_A, PORTB, PORTC, PORTD
	unsigned char ChNum01;                // [7:4] Sequence2, [3:0] Sequence1
	unsigned char ChNum23;                // [7:4] Sequence4, [3:0] Sequence3
	unsigned char devnum;
}NC_VD_VO_SEQ_STR;

typedef struct _NC_VD_VO_SEQ_REG_STR{
	unsigned char Port;					// PORT_A, PORTB, PORTC, PORTD
	unsigned char b1c0;
	unsigned char b1c1;
	unsigned char b1c2;
	unsigned char b1c3;
	unsigned char b1c4;
	unsigned char b1c5;
	unsigned char b1c6;
	unsigned char devnum;
}NC_VD_VO_SEQ_REG_STR;

typedef struct _NC_VD_VO_PORT_MUX_MODE_STR{
	unsigned char Port;					//PORTAB PORTCD
	unsigned char Mode;               	//[7:4] PORTB or D [3:0] PORTA or C
	unsigned char devnum;
}NC_VD_VO_PORT_MUX_MODE_STR;

typedef struct _NC_VD_VO_PORT_MUX_CHID_STR{
	unsigned char Port;				// PORTA, PORTB 0x55 / PORTC, PORTD 0x56
	unsigned char ChId;               	//[7:4] PORTB or D [3:0] PORTA or C
	unsigned char devnum;
}NC_VD_VO_PORT_MUX_CHID_STR;

typedef struct _NC_VD_VI_FMT_STR{
	unsigned char Ch;
	unsigned char Format;			//B0       0x08/9/A/B
	unsigned char SpecialMode;		//B0       0x81/2/3/4
	unsigned char SdAhdMode;		//B0       0x85/6/7/8
}NC_VD_VI_FMT_STR;

typedef struct _NC_VD_VI_CHROMA_STR{
	unsigned char Ch;
	unsigned char PalCmOff;		//B0       	0x21/5/9/D
	unsigned char SPoint	;		//B5/6/7/8	0x28	
	unsigned char FscLockMode;	//B5/5/6/7	0x25
	unsigned char Hue;			//B0      	0x41/2/3/4
	unsigned char CombMode;		//B5/6/7/8	0x90
	unsigned char BurstDelay;	//B5/6/7/8 	0xD1
}NC_VD_VI_CHROMA_STR;

typedef struct _NC_VD_VI_H_TIMING_STR{
	unsigned char Ch;
	unsigned char devnum;
	unsigned char SyncRs;			//B5/6/7/8 0x47
	unsigned char HDelayLsb;		//B0       0x58/9/A/B
	unsigned char HMaskOn;			//B0       0x89/A/B/C
	unsigned char HMaskSel;		//B0       0x8E/8F/90/91
	unsigned char MemRdp;			//B5/6/7/8 0x64
}NC_VD_VI_H_TIMING_STR;

typedef struct _NC_VD_VI_H_SCALER_STR{
	unsigned char Ch;
	unsigned char HDownScaler;		//B9		0x96/B6/D6/F6
	unsigned char HScalerMode;		//B9		0x97/B7/D7/F7
	unsigned char RefBaseLsb;		//B9        0x98/B8/D8/F8
	unsigned char RefBaseMsb;		//B9        0x99/B9/D9/F9
	unsigned char LineMemMode;		//B0		0x34/5/6/7
}NC_VD_VI_H_SCALER_STR;

typedef struct _NC_VD_VI_CHID_STR{
	unsigned char Ch01;				//B0	    0x55
	unsigned char Ch23;				//B0        0x56
}NC_VD_VI_CHID_STR;

// Auto Detection
typedef struct _NC_VD_VI_VAFE_STR{
	unsigned char PowerDown;   		//B0  0x01/2/3/4 	  [0]
	unsigned char GainSel;			//B0  0x01/2/3/4	  [4]

	unsigned char ClpSPD;  			//B5/6/7/8 0x00       [5:4]

	unsigned char Ctrl_Reg;  		//B5/6/7/8 0x01       [6]
	unsigned char Ctrl_Ibs;  		//B5/6/7/8 0x01       [5:4]
	unsigned char AdcSPD;			//B5/6/7/8 0x01       [2]
	unsigned char ClpLevel;			//B5/6/7/8 0x01       [1:0]

	unsigned char EQBand;   		//B5/6/7/8 0x58       [6:4]
	unsigned char LPFFrontBand; 	//B5/6/7/8 0x58       [2:0]

	unsigned char ClpMode;			//B5/6/7/8 0x59       [7]
	unsigned char F_LPFBypass;		//B5/6/7/8 0x59       [4]
	unsigned char ClpROff;			//B5/6/7/8 0x59       [3]
	unsigned char B_LPFBypass;		//B5/6/7/8 0x59       [0]

	unsigned char Duty;		   		//B5/6/7/8 0x5B    	  [6:4]
	unsigned char REFVol;   		//B5/6/7/8 0x5B       [1:0]

	unsigned char LPFBackBand;  	//B5/6/7/8 0x5C       [6:4]
	unsigned char ClkSel;  			//B5/6/7/8 0x5C       [3]
	unsigned char EQGainSel;		//B5/6/7/8 0x5C       [2:0]

}NC_VD_VI_VAFE_STR;

// HPLL Setup // 170203 HPLL
typedef struct _NC_VD_VI_HPLL_STR{
 unsigned char Ch;
 unsigned char MaskOn;			//B5/6/7/8 0x50
/* 170222 HPLL MASK ON
 unsigned char BYPTh_E;			//B5/6/7/8 0xBB
 unsigned char BYPTh_S;			//B5/6/7/8 0xB7
*/
 unsigned char OpMd;				//B5/6/7/8 0xB8

}NC_VD_VI_HPLL_STR;

// 170223 HZOOM SET
typedef struct _NC_VD_VI_HZOOM_STR{
 unsigned char Ch;
 unsigned char Hzoom;				// B0 0x93/0x94/0x95/0x96
 unsigned char ZoomDto;				// B0 0x98/0x99/0x9A/0x9B
}NC_VD_VI_HZOOM_STR;

// 170223  SET
typedef struct _NC_VD_VI_AGC_STR{
 unsigned char Ch;
 unsigned char Agc;					// B5/B6/B7/B8 0x05
}NC_VD_VI_AGC_STR;

// 170223 HZOOM SET
typedef struct _NC_VD_VI_COMB_TH_STR{
 unsigned char Ch;
 unsigned char Comb_Th_A;			// B5/B6/B7/B8 0x1B
}NC_VD_VI_COMB_TH_STR;

// 170223 HZOOM SET
typedef struct _NC_VD_VI_YLPF_STR{
 unsigned char Ch;
 unsigned char Y_lpf;				// B0 0x18/0x19/0x1A/0x1B
}NC_VD_VI_YLPF_STR;

// 170314 SYNC_EN SET
typedef struct _NC_VD_VI_SYNC_EN_STR{
 unsigned char Ch;
 unsigned char SyncEnLsb;				// B5/B6/B7/B8 0x84
 unsigned char SyncEnMsb;				// B5/B6/B7/B8 0x86
}NC_VD_VI_SYNC_EN_STR;

// 170314 VBLK_END SET
typedef struct _NC_VD_VI_VBLK_END_EXT_STR{
 unsigned char Ch;
 unsigned char VblkEndLsb;				// B5/B6/B7/B8 0x6E
 unsigned char VblkEndMsb;				// B5/B6/B7/B8 0x6F
}NC_VD_VI_VBLK_END_EXT_STR;

typedef struct _NC_VD_VI_FB_YDELAY_STR{
	unsigned char Ch;
	unsigned char devnum;
	unsigned char DF_YDelay;			//B0 0xA0/0xA1/0xA2/0xA3 [3:0]
	unsigned char DB_YDelay;			//B0 0xA4/0xA5/0xA6/0xA7 [3:0]
}NC_VD_VI_FB_YDELAY_STR;

typedef struct _NC_VD_VI_FB_CDELAY_STR{
	unsigned char Ch;
	unsigned char DF_CDelay;			//B0 0xA0/0xA1/0xA2/0xA3 [7:4]
	unsigned char DB_CDelay;			//B0 0xA4/0xA5/0xA6/0xA7 [7:4]
}NC_VD_VI_FB_CDELAY_STR;

typedef struct _NC_VD_DEV_CH_INFO_STR{
	unsigned char Ch;
	unsigned char Dev_Num;
	unsigned char Fmt_Def;
}NC_VD_DEV_CH_INFO_STR;

typedef struct _NC_VD_DEV_PORT_ENABLE_STR{
	unsigned char Ch;
	unsigned char Dev_Num;
	unsigned char Fmt_Def;
	unsigned char enable;
}NC_VD_DEV_PORT_ENABLE_STR;


#endif /* _RAPTOR3_RAPTOR3_DRV_H_ */
