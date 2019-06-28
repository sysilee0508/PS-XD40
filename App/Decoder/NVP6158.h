#ifndef __NVP6158_H__
#define __NVP6158_H__

#include "raptor3_fmt.h"
#include "video_fmt_info.h"

/* value define --------------------------------------------------------------*/
#define NVP6158_ADDR	0x60
#define RAPTOR3_4PORT_R0_ID 0xA1

#define VI_1MULTIPLEX_MODE  0
#define VI_2MULTIPLEX_MODE	1
#define VI_4MULTIPLEX_MODE  2

#define VI_INPUT_MODE_BT656  0        /* ITU-R BT.656 YUV4:2:2 */
#define VI_INPUT_MODE_BT601  1         /* ITU-R BT.601 YUV4:2:2 */
#define VI_INPUT_MODE_DIGITAL_CAMERA 2   /* digatal camera mode */
#define VI_INPUT_MODE_INTERLEAVED 3

#define VI_CLK_EDGE_SINGLE_UP  0         /* single-edge mode and in rising edge */
#define VI_CLK_EDGE_SINGLE_DOWN 1           /* single-edge mode and in falling edge */
#define VI_CLK_EDGE_DOUBLE 2                 /* Double edge mode */

#define _SET_BIT(data,bit) ((data)|=(1<<(bit)))
#define _CLE_BIT(data,bit) ((data)&=(~(1<<(bit))))

#define PORTA		0x00
#define PORTB		0x01
#define PORTC		0x02
#define PORTD		0x03
#define PORTAB		0x04
#define PORTCD		0x05

#define ACC_GAIN_NORMAL 0
#define ACC_GAIN_DEBUG  1

#define FUNC_ON		0x01
#define FUNC_OFF	0x00

#define MAX_DEBOUNCE_CNT      10

typedef enum _VI_WORK_MODE_E
{
    VI_WORK_MODE_1Multiplex = 0,    /* 1 Multiplex mode */
    VI_WORK_MODE_2Multiplex,        /* 2 Multiplex mode */
    VI_WORK_MODE_4Multiplex,        /* 4 Multiplex mode */
    

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;

typedef enum  
{
	VPORT_MAP0,		// 1, 2, 3, 4	// full ch1, 2 split 1&2 , all 4 split, PIP with ch2
	VPORT_MAP4,		// 1, 3, 2, 4	// PIP with ch3
	VPORT_MAP5,		// 1, 4, 2, 3	// PIP with ch4
	VPORT_MAP_MAX
} eVPORT_MAP_t;

// Auto Detection
typedef struct 
{
	unsigned char powerdown;		//B0       0x00/1/2/3 [0]
	unsigned char gain;			//B0       0x00/1/2/3 [4]
	unsigned char spd;				//B5/6/7/8 0x00       [5:4]

	unsigned char ctrlreg;			//B5/6/7/8 0x01       [6]
	unsigned char ctrlibs;			//B5/6/7/8 0x01       [5:4]
	unsigned char adcspd;			//B5/6/7/8 0x01       [2]
	unsigned char clplevel;		//B5/6/7/8 0x01       [1:0]


	unsigned char eq_band;			//B5/6/7/8 0x58       [6:4]
	unsigned char lpf_front_band;	//B5/6/7/8 0x58       [1:0]

	unsigned char clpmode;			//B5/6/7/8 0x59       [7]
	unsigned char f_lpf_bypass;	//B5/6/7/8 0x59       [4]
	unsigned char clproff;			//B5/6/7/8 0x59       [3]
	unsigned char b_lpf_bypass;	//B5/6/7/8 0x59       [0]

	unsigned char duty;		   	//B5/6/7/8 0x5B    	  [6:4]
	unsigned char ref_vol;			//B5/6/7/8 0x5B       [1:0]

	unsigned char lpf_back_band;	//B5/6/7/8 0x5C       [6:4]
	unsigned char clk_sel;			//B5/6/7/8 0x5C       [3]
	unsigned char eq_gainsel;		//B5/6/7/8 0x5C       [2:0]

}video_input_vafe;

typedef struct 
{
	unsigned char ch;
	unsigned char devnum;

	unsigned char d_cmp;				//B5/6/7/8 0x03
	unsigned char slice_level;		//B5/6/7/8 0x08
	unsigned char control_mode;		//B5/6/7/8 0x47
	unsigned char gdf_fix_coeff;		//B5/6/7/8 0x50
	unsigned char dfe_ref_sel;		//B5/6/7/8 0x76
	unsigned char wpd_77;				//B5/6/7/8 0x77
	unsigned char wpd_78;				//B5/6/7/8 0x78
	unsigned char wpd_79;				//B5/6/7/8 0x79
	unsigned char auto_gnos_mode;		//B5/6/7/8 0x82
	unsigned char auto_sync_mode;		//B5/6/7/8 0x83
	unsigned char hafc_bypass;			//B5/6/7/8 0xB8

	unsigned char novid_vfc_init;	//B13		0x31
	unsigned char stable_mode_1;	//B13		0x0
	unsigned char stable_mode_2;	//B13		0x1

	unsigned char novid_det;		//B0		0x23/0x27/0x2B/0x2F
	video_input_vafe vafe;
}video_input_auto_detect;

typedef struct _video_input_novid{
	unsigned char ch;
	unsigned char novid; 				//B0 0xa8 [3:0]  MSB 1Ch ~ LSB 4Ch
	unsigned char devnum;
}video_input_novid;

typedef struct _video_input_vfc{
	unsigned char ch;
	unsigned char vfc;				//B5/6/7/8 0xf0
	unsigned char devnum;
}video_input_vfc;

typedef struct _video_input_vafe_init_s{
	unsigned char powerdown;		//B0       0x00/1/2/3 [0]
	unsigned char gain;			//B0       0x00/1/2/3 [4]
	unsigned char spd;				//B5/6/7/8 0x00       [5:4]

	unsigned char ctrlreg;			//B5/6/7/8 0x01       [6]
	unsigned char ctrlibs;			//B5/6/7/8 0x01       [5:4]
	unsigned char adcspd;			//B5/6/7/8 0x01       [2]
	unsigned char clplevel;		//B5/6/7/8 0x01       [1:0]


	unsigned char eq_band;			//B5/6/7/8 0x58       [6:4]
	unsigned char lpf_front_band;	//B5/6/7/8 0x58       [1:0]

	unsigned char clpmode;			//B5/6/7/8 0x59       [7]
	unsigned char f_lpf_bypass;	//B5/6/7/8 0x59       [4]
	unsigned char clproff;			//B5/6/7/8 0x59       [3]
	unsigned char b_lpf_bypass;	//B5/6/7/8 0x59       [0]

	unsigned char duty;		   	//B5/6/7/8 0x5B    	  [6:4]
	unsigned char ref_vol;			//B5/6/7/8 0x5B       [1:0]

	unsigned char lpf_back_band;	//B5/6/7/8 0x5C       [6:4]
	unsigned char clk_sel;			//B5/6/7/8 0x5C       [3]
	unsigned char eq_gainsel;		//B5/6/7/8 0x5C       [2:0]

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;
}video_input_vafe_init_s;

typedef struct _video_input_color_init_s{
	unsigned char	brightnees;		//B0		0x0c/0x0d/0x0e/0x0f
	unsigned char	contrast;		//B0 		0x10/0x11/0x12/0x13
	unsigned char	hue;			//B0		0x40/0x41/0x42/0x43
	unsigned char	u_gain;         //B0		0x44/0x45/0x46/0x47
	unsigned char	v_gain;         //B0		0x48/0x49/0x4a/0x4b
	unsigned char	u_offset;       //B0		0x4c/0x4d/0x4e/0x4f
	unsigned char	v_offset;       //B0		0x50/0x51/0x52/0x53
	unsigned char	saturation_a;   //B0		0x3c/0x3d/0x3e/0x3f
	unsigned char	saturation_b;	//B5/6/7/8	0x2b
	unsigned char	black_level;  	 //B5/6/7/8	0x20
	unsigned char	burst_dec_a;	//B5/6/7/8	0x24
	unsigned char	burst_dec_b;    //B5/6/7/8	0x5f
	unsigned char	burst_dec_c;    //B5/6/7/8	0xd1
	unsigned char 	new_wpd_sel;	//B5/6/7/8 	0x75				[0]

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;

}video_input_color_init_s;

typedef struct _video_input_basic_vfmt_init_s{
	unsigned char	video_format;	//B0		0x08/0x09/0x0a/0x0b
	unsigned char	sd_ahd_mode;	//B0 		0x81/0x82/0x83/0x84
	unsigned char	spl_mode;		//B0		0x85/0x86/0x87/0x88
	unsigned char	sd_freq_sel;    //B5/6/7/8	0x69[0]

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;

}video_input_basic_vfmt_init_s;

typedef struct _video_input_basic_chroma_init_s{
	unsigned char	pal_cm_off;		//B0		0x21/0x25/0x29/0x2d
	unsigned char	s_point;		//B5/6/7/8 	0x28
	unsigned char	fsc_lock_mode;	//B5/6/7/8	0x25
	unsigned char	comb_mode;    	//B5/6/7/8	0x90

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;

}video_input_basic_chroma_init_s;


typedef struct _video_input_basic_timing_init_s{
	unsigned char	sync_rs;		//B5/6/7/8	0x47
	unsigned char	h_delay_lsb;	//B0 		0x58/0x59/0x5a/0x5b
	unsigned char	h_mask_on;		//B0		0x89/0x8a/0x8b/0x8c
	unsigned char	h_mask_sel;     //B0		0x8e/0x8f/0x90/0x91
	unsigned char 	mem_rdp;		//B5/6/7/8	0x64
	unsigned char	v_blk_end_b;	//B5/6/7/8	0xA9
	unsigned char	y_delay;		//B0		0xA0/0xA1/0xA2/0xA3
	unsigned char	fld_inv;		//B0		0x54[4]/[5]/[6]/[7]
	unsigned char	v_blk_end_a;	//B0		0x64/0x65/0x66/0x67

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;

}video_input_basic_timing_init_s;

typedef struct _video_input_basic_hscaler_mode_init_s{
	unsigned char	h_down_scaler;	//B9		0x96/0xb6/0xd6/0xf6
	unsigned char	h_scaler_mode;	//B9	 	0x97/0xb7/0xd7/0xf7
	unsigned char	ref_base_lsb;	//B9		0x98/0xb8/0xd8/0xf8
	unsigned char	ref_base_msb;  	//B9		0x99/0xb9/0xd9/0xf9
	unsigned char	line_mem_mode;	//B0		0x34/0x35/0x36/0x37

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;

}video_input_basic_hscaler_mode_init_s;

typedef struct _video_input_basic_hpll_init_s{
	unsigned char	hpll_mask_on;	//B5/6/7/8		0x50
	unsigned char	hafc_byp_th_e;	//B5/6/7/8	 	0xbb
	unsigned char	hafc_byp_th_s;	//B5/6/7/8		0xb7
	unsigned char	hafc_op_md;  	//B5/6/7/8		0xb8

	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char reserved6;

}video_input_basic_hpll_init_s;

typedef struct _video_input_sam_val{
	unsigned char ch;
	unsigned char devnum;
	/*
	unsigned char sam_val_1;		// B13 0xCD [7:0]
	unsigned char sam_val_2;		// B13 0xCC [9:8]
	*/
	unsigned int  sam_val;
}video_input_sam_val;

typedef struct _video_input_hsync_accum{
	unsigned char ch;
	unsigned char devnum;
	unsigned char h_lock;				// Bank 0 0xE2 [3:0] [Ch3:Ch0]
	unsigned int  hsync_accum_val1;		// Value 1  			// 170210 Add
	unsigned int  hsync_accum_val2;		// Value 2				// 170210 Add
	unsigned int  hsync_accum_result;	// Value 1 - Value 2	// 170210 Fix
}video_input_hsync_accum;

typedef struct _video_input_agc_val{
	unsigned char ch;
	unsigned char devnum;
	unsigned char agc_lock;
	unsigned char agc_val;			// B13 0xB8
}video_input_agc_val;

typedef struct _video_input_acc_gain_val{	// 170215 acc gain value read
	unsigned char ch;
	unsigned char devnum;
	unsigned int acc_gain_val;
	unsigned char func_sel;
}video_input_acc_gain_val;

typedef struct _video_input_cable_dist{	
	unsigned char ch;
	unsigned char devnum;
	unsigned char dist;				// B13 0xA0
	unsigned char FmtDef;
	unsigned char cabletype;			// 0:coax, 1:utp, 2:reserved1, 3:reserved2
}video_input_cable_dist;

typedef struct _video_input_novid_set{
	unsigned char ch;
	unsigned char devnum;
	unsigned char control_mode;
	unsigned char gdf_fix_coeff;
	unsigned char auto_gnos_mode;
	unsigned char auto_sync_mode;
	unsigned char hafc_bypass;
	unsigned char dfe_ref_sel;			//B5/6/7/8 0x76
	unsigned char wpd_77;				//B5/6/7/8 0x77
	unsigned char wpd_78;				//B5/6/7/8 0x78
	unsigned char wpd_79;				//B5/6/7/8 0x79
	unsigned char slice_mode;			//B5/6/7/8 0x0E
}video_input_novid_set;

typedef struct _video_output_sequence
{
	unsigned char port;       // PORT_A, PORTB, PORTC, PORTD
	unsigned char ch01;			// [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
	unsigned char ch23;			// [7:4] Sequence2 or 4, [3:0] Sequence1 or 3
	unsigned char devnum;
}video_output_sequence;

typedef struct _video_output_sequence_reg{
	unsigned char Port;					// PORT_A, PORTB, PORTC, PORTD
	unsigned char b1c0;
	unsigned char b1c1;
	unsigned char b1c2;
	unsigned char b1c3;
	unsigned char b1c4;
	unsigned char b1c5;
	unsigned char b1c6;
	unsigned char b1c7;
	unsigned char devnum;
}video_output_sequence_reg;

typedef struct
{
	const char *name;

	//For SoC
	int width;
	int height;

	unsigned char vo_clk;
	unsigned char merge;
	unsigned char mux_mode;
	unsigned char seq_ch01;
	unsigned char seq_ch23;
	unsigned char chid_vin01;
	unsigned char chid_vin23;
	unsigned char devnum;

} video_output_port_fmt_s;

typedef struct _video_output_port_mux_mode
{
	unsigned char port;		//PORTAB PORTCD
	unsigned char mode;		//[7:4] PORTB or D [3:0] PORTA or C
	unsigned char devnum;
}video_output_port_mux_mode;

typedef struct _video_output_port_mux_chid
{
	unsigned char port;
	unsigned char chid;
	unsigned char devnum;
}video_output_port_mux_chid;

typedef struct _video_output_data_out_mode{
	unsigned char ch;
	unsigned char devnum;
	unsigned char set_val;
}video_output_data_out_mode;

typedef struct _clock_video_output{
    unsigned char port;
    unsigned char devnum;
    unsigned char clk_sel;
}clock_video_output;

typedef struct _video_output_port_enable_s{
	unsigned char ch;
	unsigned char devnum;
	unsigned char Fmt_Def;
	unsigned char enable;
}video_output_port_enable_s;

typedef struct _video_equalizer_hsync_stage_s{
	unsigned int hsync_stage[9];
}video_equalizer_hsync_stage_s;

typedef struct _video_equalizer_agc_stage_s{
	unsigned int agc_stage[9];
}video_equalizer_agc_stage_s;

typedef struct _video_equalizer_distance_table_s{
	video_equalizer_hsync_stage_s hsync_stage;
	video_equalizer_agc_stage_s   agc_stage;
} video_equalizer_distance_table_s;

typedef struct _video_equalizer_base_s{
	unsigned char eq_bypass[11];			// B5x01
	unsigned char eq_band_sel[11];		// B5x58
	unsigned char eq_gain_sel[11];		// B5x5C

	unsigned char deq_a_on[11];			// BAx3d
	unsigned char deq_a_sel[11];			// BAx3C
	unsigned char deq_b_sel[11];			// B9x80

} video_equalizer_base_s;

typedef struct _video_equalizer_coeff_s{

	unsigned char deqA_01[11];	// BankA 0x30
	unsigned char deqA_02[11];	// BankA 0x31
	unsigned char deqA_03[11];   // BankA 0x32
	unsigned char deqA_04[11];   // BankA 0x33
	unsigned char deqA_05[11];   // BankA 0x34
	unsigned char deqA_06[11];   // BankA 0x35
	unsigned char deqA_07[11];   // BankA 0x36
	unsigned char deqA_08[11];   // BankA 0x37
	unsigned char deqA_09[11];   // BankA 0x38
	unsigned char deqA_10[11];   // BankA 0x39
	unsigned char deqA_11[11];   // BankA 0x3A
	unsigned char deqA_12[11];   // BankA 0x3B

} video_equalizer_coeff_s;

typedef struct _video_equalizer_color_s{
	unsigned char contrast[11];			// Bank0 0x10
	unsigned char h_peaking[11];			// Bank0 0x18
	unsigned char c_filter[11];			// Bank0 0x21
	unsigned char hue[11];				// Bank0 0x40
	unsigned char u_gain[11];			// Bank0 0x44
	unsigned char v_gain[11];			// Bank0 0x48
	unsigned char u_offset[11];			// Bank0 0x4c
	unsigned char v_offset[11];			// Bank0 0x50

	unsigned char black_level[11];		// Bank5 0x20
	unsigned char acc_ref[11];			// Bank5 0x27

	unsigned char cti_delay[11];			// Bank5 0x28
	unsigned char sub_saturation[11];    // Bank5 0x2B
	unsigned char burst_dec_a[11];       // Bank5 0x24
	unsigned char burst_dec_b[11];       // Bank5 0x5F
	unsigned char burst_dec_c[11];       // Bank5 0xD1
	unsigned char c_option[11];          // Bank5 0xD5

	unsigned char y_filter_b[11];		// BankA 0x25
	unsigned char y_filter_b_sel[11];	// BankA 0x27

} video_equalizer_color_s;

typedef struct _video_equalizer_timing_a_s{
	unsigned char h_delay_a[11];			// Bank0 0x58
	unsigned char h_delay_b[11];			// Bank0 0x89
	unsigned char h_delay_c[11];			// Bank0 0x8E
	unsigned char y_delay[11];			// Bank0 0xA0

} video_equalizer_timing_a_s;

typedef struct _video_equalizer_clk_s{
	unsigned char clk_adc[11];			// Bank1 0x84
	unsigned char clk_dec[11];			// Bank1 0x8C

} video_equalizer_clk_s;

typedef struct _video_equalizer_timing_b_s{
	unsigned char h_scaler1[11];		// B9x96 + ch*0x20
	unsigned char h_scaler2[11];		// B9x97 + ch*0x20
	unsigned char h_scaler3[11];		// B9x98 + ch*0x20
	unsigned char h_scaler4[11];		// B9x99 + ch*0x20
	unsigned char h_scaler5[11];		// B9x9a + ch*0x20
	unsigned char h_scaler6[11];		// B9x9b + ch*0x20
	unsigned char h_scaler7[11];		// B9x9c + ch*0x20
	unsigned char h_scaler8[11];		// B9x9d + ch*0x20
	unsigned char h_scaler9[11];		// B9x9e + ch*0x20

	unsigned char pn_auto[11];		// B9x40 + ch

	unsigned char comb_mode[11];		// B5x90
	unsigned char h_pll_op_a[11];	// B5xB9
	unsigned char mem_path[11];		// B5x57
	unsigned char fsc_lock_speed[11]; //B5x25

	unsigned char format_set1[11];	// B0x81
	unsigned char format_set2[11];	// B0x85

	unsigned char v_delay[11];		// B0x64
} video_equalizer_timing_b_s;

typedef struct _video_equalizer_value_table_s{
	video_equalizer_base_s		eq_base;
	video_equalizer_coeff_s 	eq_coeff;
	video_equalizer_color_s 	eq_color;

	video_equalizer_timing_a_s 	eq_timing_a;
	video_equalizer_clk_s		eq_clk;
	video_equalizer_timing_b_s	eq_timing_b;

} video_equalizer_value_table_s;

typedef struct _video_equalizer_info{
	unsigned char Ch;
	unsigned char devnum;
	unsigned char distance;
	unsigned char FmtDef;
} video_equalizer_info_s;

typedef struct _decoder_dev_ch_info_s
{
	unsigned char ch;
	unsigned char devnum;
	unsigned char fmt_def;
}decoder_dev_ch_info_s;

#if 0
typedef struct _motion_mode{
	unsigned char ch;
	unsigned char devnum;
	unsigned char set_val;

	unsigned char fmtdef;
}motion_mode;

typedef enum NC_CH
{
	CH1 = 0,
	CH2,
	CH3,
	CH4
} NC_CH;
#endif


///////////////////////////////// VIDEO_FORMAT_DETECT /////////////////////////////////
typedef struct _NC_VD_AUTO_VFC_STR{
 unsigned char Ch;
 unsigned char VFC;				//B5/6/7/8 0xf0
 unsigned char Dev_Num;		// Device Address
}NC_VD_AUTO_VFC_STR;

// NO Video Read 					// 170204 No Video Read 항목 추가
typedef struct _NC_VD_AUTO_NOVIDEO_STR{
 unsigned char Ch;
 unsigned char NoVid;				// B0 0xA8
 unsigned char Dev_Num;
}NC_VD_AUTO_NOVIDEO_STR;

typedef struct _NC_VD_AUTO_DATA_OUT_MODE_STR{	// 170329
	unsigned char Ch;
	unsigned char devnum;
	unsigned char SetVal;
}NC_VD_AUTO_DATA_OUT_MODE_STR;

typedef struct _NC_VD_AUTO_NOVIDEO_REG_STR{
	unsigned char Ch;
	unsigned char devnum;	
}NC_VD_AUTO_NOVIDEO_REG_STR;


#if 0
typedef enum NC_VIVO_CH_FORMATDEF
{
	NC_VIVO_CH_FORMATDEF_UNKNOWN = 0,
	NC_VIVO_CH_FORMATDEF_AUTO,

	AHD20_SD_H960_NT,
	AHD20_SD_H960_PAL,
	AHD20_SD_SH720_NT,
	AHD20_SD_SH720_PAL,
	AHD20_SD_H1280_NT,
	AHD20_SD_H1280_PAL,
	AHD20_SD_H1440_NT,
	AHD20_SD_H1440_PAL,
	AHD20_SD_H960_EX_NT,
	AHD20_SD_H960_EX_PAL,
	AHD20_SD_H960_2EX_NT,
	AHD20_SD_H960_2EX_PAL,
	AHD20_SD_H960_2EX_Btype_NT,
	AHD20_SD_H960_2EX_Btype_PAL,

	AHD20_1080P_60P, // For Test
	AHD20_1080P_50P, // For Test

	AHD20_1080P_30P,
	AHD20_1080P_25P,

	AHD20_720P_60P,
	AHD20_720P_50P,
	AHD20_720P_30P,
	AHD20_720P_25P,
	AHD20_720P_30P_EX,
	AHD20_720P_25P_EX,
	AHD20_720P_30P_EX_Btype,
	AHD20_720P_25P_EX_Btype,

	AHD30_4M_30P,
	AHD30_4M_25P,
	AHD30_4M_15P,
	AHD30_3M_30P,
	AHD30_3M_25P,
	AHD30_3M_18P,
	AHD30_5M_12_5P,
	AHD30_5M_20P,


	AHD30_5_3M_20P,
	AHD30_6M_18P,
	AHD30_6M_20P,
	AHD30_8M_X_30P,
	AHD30_8M_X_25P,
	AHD30_8M_7_5P,
	AHD30_8M_12_5P,
	AHD30_8M_15P,

	TVI_FHD_30P,
	TVI_FHD_25P,
	TVI_HD_60P,
	TVI_HD_50P,
	TVI_HD_30P,
	TVI_HD_25P,
	TVI_HD_30P_EX,
	TVI_HD_25P_EX,
	TVI_HD_B_30P,
	TVI_HD_B_25P,
	TVI_HD_B_30P_EX,
	TVI_HD_B_25P_EX,
	TVI_3M_18P,
	TVI_5M_12_5P,

	TVI_5M_20P, /*By Edward for testing when 170912 */

	TVI_4M_30P,
	TVI_4M_25P,
	TVI_4M_15P,  /*By Edward for testing when 170912 */

	CVI_FHD_30P,
	CVI_FHD_25P,
	CVI_HD_60P,
	CVI_HD_50P,
	CVI_HD_30P,
	CVI_HD_25P,
	CVI_HD_30P_EX,
	CVI_HD_25P_EX,
	CVI_4M_30P,
	CVI_4M_25P,
	CVI_8M_15P,
	CVI_8M_12_5P,

	NC_VIVO_CH_FORMATDEF_MAX,

} NC_VIVO_CH_FORMATDEF;

typedef enum NC_FORMAT_FPS
{
	FMT_FPS_UNKNOWN = 0,
	FMT_NT = 1,
	FMT_PAL,
	FMT_12_5P,
	FMT_7_5P,
	FMT_30P,
	FMT_25P,
	FMT_50P,
	FMT_60P,
	FMT_15P,
	FMT_18P,
	FMT_18_75P,
	FMT_20P,

	FMT_FPS_MAX,

} NC_FORMAT_FPS;

typedef enum NC_FORMAT_STANDARD
{
	FMT_STD_UNKNOWN = 0,
	FMT_SD,
	FMT_AHD20,
	FMT_AHD30,
	FMT_TVI,
	FMT_CVI,

	FMT_AUTO,		// FIXME

	FMT_STD_MAX,

} NC_FORMAT_STANDARD;


typedef enum NC_FORMAT_RESOLUTION
{
	FMT_RESOL_UNKNOWN = 0,
	FMT_SH720,
	FMT_H960,
	FMT_H1280,
	FMT_H1440,
	FMT_H960_EX,
	FMT_H960_2EX,
	FMT_H960_Btype_2EX,
	FMT_720P,
	FMT_720P_EX,
	FMT_720P_Btype,
	FMT_720P_Btype_EX,
	FMT_1080P,
	FMT_3M,
	FMT_4M,
	FMT_5M,
	FMT_5_3M,
	FMT_6M,
	FMT_8M_X,
	FMT_8M,

	FMT_H960_Btype_2EX_SP,
	FMT_720P_Btype_EX_SP,

	FMT_960P,

	FMT_RESOL_MAX,

} NC_FORMAT_RESOLUTION;

typedef struct _NC_VI_CH_FMT_S
{
	const char *name;

	// for SoC
	int width; // pixel
	int height; // pixel
	NC_FORMAT_FPS format_fps;
	NC_FORMAT_STANDARD format_standard;
	NC_FORMAT_RESOLUTION format_resolution;

	//For Decoder
	unsigned char sw_reset;
	unsigned char each_set;
	unsigned char no_video;
	unsigned char video_format;
	unsigned char sd_ahd_mode;
	unsigned char spl_mode;
	unsigned char pal_cm_off;
	unsigned char s_point;				
	unsigned char fsc_lock_mode;
	unsigned char hue;
	unsigned char comb_mode;
	unsigned char burst_delay;
	unsigned char sync_rs;
	unsigned char h_delay_lsb;
	unsigned char h_mask_on;
	unsigned char h_maks_sel;
	unsigned char mem_rdp;
	unsigned char h_down_scaler;
	unsigned char h_scaler_mode;
	unsigned char ref_base_lsb;
	unsigned char ref_base_msb;
	unsigned char line_mem_mode;
	unsigned char hpll_mask_on;
	unsigned char op_md;
	unsigned char hzoom_on;
	unsigned char zoom_dto;
	unsigned char agc;
	unsigned char comb_th_a;
	unsigned char y_lpf;
	unsigned char sync_en_lsb;
	unsigned char sync_en_msb;
	unsigned char vblk_end_lsb;
	unsigned char vblk_end_msb;

	unsigned char df_ydelay;
	unsigned char db_ydelay;
	unsigned char df_cdelay;
	unsigned char db_cdelay;

	unsigned char clk_adc;
	unsigned char clk_pre;
	unsigned char clk_post;
} NC_VI_CH_FMT_S;

typedef struct _NC_VO_PORT_FMT_S
{

	const char *name;

	//For SoC
	int width;
	int height;

	unsigned char vo_clk;
	unsigned char merge;
	unsigned char mux_mode;
	unsigned char seq_ch01;
	unsigned char seq_ch23;
	unsigned char chid_vin01;
	unsigned char chid_vin23;

	unsigned char devnum;

}NC_VO_PORT_FMT_S;
#endif

typedef enum _NC_VIDEO_ONOFF
{
	VIDEO_LOSS_ON = 0,
	VIDEO_LOSS_OFF = 1,

} NC_VIDEO_ONOFF;

typedef enum
{
	OUT_MODE_ON = 0x01,
	OUT_MODE_OFF = 0x0F
}DATA_OUT_MODE_NUM;


typedef struct _NC_VD_DEV_CH_INFO_STR{
	unsigned char Ch;
	unsigned char Dev_Num;
	unsigned char Fmt_Def;
}NC_VD_DEV_CH_INFO_STR;

typedef struct _NC_VD_CLK_VO_STR{
    unsigned char Port;				// PORT_A, PORTB, PORTC, PORTD
    unsigned char devnum;
    unsigned char ClkSel;			// [7:4] Port Clock SEL, [3:0] Port Clock Delay
}NC_VD_CLK_VO_STR;

typedef struct _NC_VD_VO_SEQ_STR{
	unsigned char Port;					// PORT_A, PORTB, PORTC, PORTD
	unsigned char ChNum01;                // [7:4] Sequence2, [3:0] Sequence1
	unsigned char ChNum23;                // [7:4] Sequence4, [3:0] Sequence3
	unsigned char devnum;
}NC_VD_VO_SEQ_STR;

typedef struct _NC_VD_VO_PORT_MUX_CHID_STR{
	unsigned char Port;				// PORTA, PORTB 0x55 / PORTC, PORTD 0x56
	unsigned char ChId;               	//[7:4] PORTB or D [3:0] PORTA or C
	unsigned char devnum;
}NC_VD_VO_PORT_MUX_CHID_STR;

typedef struct _NC_VD_VO_PORT_MUX_MODE_STR{
	unsigned char Port;					//PORTAB PORTCD
	unsigned char Mode;               	//[7:4] PORTB or D [3:0] PORTA or C
	unsigned char devnum;
}NC_VD_VO_PORT_MUX_MODE_STR;

typedef struct _NC_VD_DEV_PORT_ENABLE_STR{
	unsigned char Ch;
	unsigned char Dev_Num;
	unsigned char Fmt_Def;
	unsigned char enable;
}NC_VD_DEV_PORT_ENABLE_STR;

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

/*=============================================================
 * Coaxial UP/Down stream initialize structure
 ==============================================================*/
typedef struct NC_VD_COAX_Init_STR{
	 char *name;
	 unsigned char ch;
	 unsigned char rx_src;             //B5/6/7/8 0x7C
	 unsigned char rx_slice_lev;       //B5/6/7/8 0x7D
	 unsigned char tx_bank;
	 unsigned char tx_cmd_addr;
	 unsigned char tx_shot_addr;
	 unsigned char tx_baud[6];            //B3/4 0x00/80
	 unsigned char tx_pel_baud[6];        //B3/4 0x02/82
	 unsigned char tx_line_pos0[6];       //B3/4 0x03/83
	 unsigned char tx_line_pos1[6];       //B3/4 0x04/84
	 unsigned char tx_pel_line_pos0[6];   //B3/4 0x07/87
	 unsigned char tx_pel_line_pos1[6];   //B3/4 0x08/88
	 unsigned char tx_line_count;         //B3/4 0x05/85
	 unsigned char tx_line_count_max;     //B3/4 0x0A/8A
	 unsigned char tx_mode;               //B3/4 0x0B/8B
	 unsigned char tx_sync_pos0[6];       //B3/4 0x0D/8D
	 unsigned char tx_sync_pos1[6];       //B3/4 0x0E/8E
	 unsigned char tx_even;            //B3/4 0x2F/AF
	 unsigned char tx_zero_length;     //B3/4 0x0C/

	 unsigned char rx_comm_on;         //
	 unsigned char rx_area;            //
	 unsigned char rx_signal_enhance;  //
	 unsigned char rx_manual_duty;     //
	 unsigned char rx_head_matching;   //
	 unsigned char rx_data_rz;         //
	 unsigned char rx_sz;              //

	 int shot_delay;
	 int reset_delay;
}NC_VD_COAX_Init_STR;

/*=============================================================
 * Coaxial UP/Down Stream Initialize Structure[APP -> DRV]
 ==============================================================*/
typedef enum NC_COAX_CMD_DEF
{
	COAX_CMD_UNKNOWN = 0,
	COAX_CMD_IRIS_INC,
	COAX_CMD_IRIS_DEC,
	COAX_CMD_FOCUS_INC,
	COAX_CMD_FOCUS_DEC,
	COAX_CMD_ZOOM_INC,
	COAX_CMD_ZOOM_DEC,
	COAX_CMD_OSD_ON,
	COAX_CMD_PTZ_UP,
	COAX_CMD_PTZ_DOWN,
	COAX_CMD_PTZ_LEFT,
	COAX_CMD_PTZ_RIGHT,
	COAX_CMD_OSD_ENTER,
	COAX_CMD_SPECIAL_FW,
	COAX_CMD_SPECIAL_CAMEQ,
	COAX_CMD_SPECIAL_FPS,
	COAX_CMD_SPECIAL_MOTION,
	COAX_CMD_TVI_DOWNSTREAM_REQUEST,

	COAX_CMD_MAX,

} NC_COAX_CMD_DEF;

typedef struct NC_VD_COAX_STR{
	 char *name;
	 unsigned char ch;
	 unsigned char vd_dev;
	 unsigned char param;
	 NC_FORMAT_STANDARD format_standard;
	 NC_FORMAT_RESOLUTION format_resolution;
	 NC_FORMAT_FPS format_fps;
	 NC_VIVO_CH_FORMATDEF vivo_fmt;
	 NC_COAX_CMD_DEF cmd;

	 unsigned char rx_pelco_data[8];
	 unsigned char rx_data1[8];
	 unsigned char rx_data2[8];
	 unsigned char rx_data3[8];
	 unsigned char rx_data4[8];
	 unsigned char rx_data5[8];
	 unsigned char rx_data6[8];

}NC_VD_COAX_STR;


typedef enum DISTANCE
{
	SHORT_2M,
	LONG_100M,
	LONG_200M,
	LONG_300M,
	LONG_400M,
	LONG_500M,
}CABLE_DISTANCE;

typedef struct _NC_VD_EQ_STR{
	unsigned char Ch;
	unsigned char Dev_Num;
	unsigned char distance;
	unsigned char FmtDef;
} NC_VD_EQ_STR;

typedef struct _RAPTOR3_INFORMATION_S
{
	unsigned char			oMux;	/* 0:1mux,  */
	NC_VIVO_CH_FORMATDEF 	curvideofmt[ 4 ];
	NC_VIVO_CH_FORMATDEF 	prevideofmt[ 4 ];
	unsigned char 		 	curvideoloss[ 4 ];
	unsigned char			vfc[4];
	unsigned char			debounce[4][5];
	unsigned char			debounceidx[4];
	unsigned int 			videolosscheck[4]; /* 1:Loss 0:Video */
	unsigned char 			motiondetect;
	
	eVPORT_MAP_t 		vport_map;		// added by kukuri
	unsigned char 		vport_map_changed;
} RAPTOR3_INFORMATION_S;

void NVP6158_init(void);
void NVP6158_VideoDetectionProc(void);
unsigned char NVP6158_I2C_READ(unsigned char slaveaddr, unsigned char regaddr);
void NVP6158_I2C_WRITE(unsigned char slaveaddr, unsigned char regaddr, unsigned char write_data);
void NVP6158_Video_Loss_Check(unsigned int *pVideoLoss);
NC_VIVO_CH_FORMATDEF NVP6158_Current_Video_Format_Check(unsigned char oLogicalChannel);
void NC_VD_VO_Auto_Data_Mode_Set(unsigned char ch, unsigned char devnum, unsigned char SetVal);

int check_id(unsigned char dec);
void video_input_auto_detect_set(video_input_auto_detect *vin_auto_det);
void video_output_channel_sequence_get(video_output_sequence_reg *vout_seq_reg);
unsigned char __IsOver3MRTVideoFormat(const char *poFmtName);
void video_output_channel_sequence_set(video_output_sequence *vout_seq);
void video_output_port_format_mode_set( video_output_port_fmt_s *pformat );
void video_output_port_mux_mode_set(video_output_port_mux_mode *vout_port_mux_mode);
void video_output_port_mux_chid_set(video_output_port_mux_chid *vout_chid);
void video_out_auto_vclk_set(clock_video_output *clk_vout);
void video_out_port_enable(video_output_port_enable_s *portenable);
void video_input_eq_val_set(video_equalizer_info_s *pvin_eq_set);
void video_input_manual_agc_stable_endi(decoder_dev_ch_info_s *decoder_info, int endi);
void video_output_data_out_mode_set(video_output_data_out_mode *vo_data_out_mode);
void video_input_auto_ch_sw_rst(decoder_dev_ch_info_s *decoder_info);
void video_input_ahd_tvi_distinguish(decoder_dev_ch_info_s *decoder_info);
void video_input_contrast_off(decoder_dev_ch_info_s *decoder_info);
void video_input_vafe_control(decoder_dev_ch_info_s *decoder_info, int cmd);
void video_input_vfc_read(video_input_vfc *vin_vfc);
void video_output_colorbar_set( unsigned char on_off );
void video_input_novid_read(video_input_novid *vin_novid);
void video_input_onvideo_set(decoder_dev_ch_info_s *decoder_info);
void video_input_h_timing_set(decoder_dev_ch_info_s *pDevChInfo);
void video_input_color_set(decoder_dev_ch_info_s *decoder_info);
void video_input_vafe_init(decoder_dev_ch_info_s *decoder_info);
void video_input_new_format_set(decoder_dev_ch_info_s *decoder_info);
void video_input_onvideo_check_data(video_input_vfc *vin_vfc);
void video_input_no_video_set(video_input_novid_set *auto_novid);
//void motion_detection_get(motion_mode *motion_set);
//void motion_onoff_set(motion_mode *motion_set);
//void motion_pixel_all_onoff_set(motion_mode *motion_set);
//void motion_pixel_onoff_set(motion_mode *motion_set);
//void motion_tsen_set(motion_mode *motion_set);

int RAPTOR3_SAL_GetFormatEachCh( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_OnVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_AutoDebouceCheck( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );
int RAPTOR3_SAL_NoVIdeoSetFormat( unsigned char ch, RAPTOR3_INFORMATION_S *pInformation );

NC_VIVO_CH_FORMATDEF NC_VD_AUTO_VFCtoFMTDEF(unsigned char VFC);
NC_VO_PORT_FMT_S *NC_HI_VO_Get_PortFormat( NC_VIVO_CH_FORMATDEF def );
void NC_VD_VO_Mode_Set_New( unsigned char ch, unsigned char devnum, unsigned char port, NC_VO_PORT_FMT_S *pPortFmt,
                            unsigned char mux_mode,
                            unsigned char output_interface,
                            unsigned char edge,
                            unsigned char SEQ0, unsigned char SEQ1, unsigned char SEQ2, unsigned char SEQ3);

void NVP6158_Set_VportMap(eVPORT_MAP_t map);
eVPORT_MAP_t NVP6158_Get_VportMap(void);


#endif
