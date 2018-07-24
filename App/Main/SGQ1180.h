
#ifndef _SGQ1180_H_
#define _SGQ1180_H_

#define	INTSTAT		0x2
#define INTMASK		0x3
#define DISPLAYMODE	0x4
#define DISPLAYCFG	0x5
#define SCRATCH		0x6
#define REC01MODE	0x7
#define REC23MODE	0x8
#define COLOR0		0x9
#define COLOR1		0xa
#define COLOR2		0xb
#define COLOR3		0xc
#define COLORPB		0xd
#define COLORREC01	0xe
#define COLORREC23	0xf
#define IFORMAT0	0x10
#define IFORMAT1	0x11
#define IFORMAT2	0x12
#define IFORMAT3	0x13
#define IFORMAT4	0x14
#define RECMODE		0x15
//#define REC01SW		0x15
//#define REC23SW		0x16
#define LOC4		0x17
#define LOC9		0x18
#define LOC16		0x19
#define IFORMATCS	0x1a
#define STATUSLED	0x1b
#define ZOOMPOS		0x1c
#define ZOOMH		0x1d
#define ZOOMV		0x1e

#define LOCSPECIAL	0x20

#define AudioSTAT00 0x22
#define AudioSTAT10 0x23
#define AudioCtrl0  0x24
#define AudioStat01 0x25
#define AudioStat11 0x26
#define AudioCtrl1  0x27
#define AudioSTAT02 0x28
#define AudioSTAT12 0x29
#define AudioCtrl2  0x2A
#define AudioSTAT03 0x2B
#define AudioSTAT13 0x2C
#define AudioCtrl3  0x2D

#define BT656Output 0x30

#define FPGA_ADDR0  0x14 * 2
#define FPGA_ADDR1  0x15 * 2
#define FPGA_ADDR2  0x16 * 2
#define FPGA_ADDR3  0x17 * 2
#define FPGA_ADDR4  0x10 * 2
#define FPGA_ADDR5  0x11 * 2
#define FPGA_ADDR6  0x12 * 2
#define FPGA_ADDR7  0x13 * 2

#define SDI_CH0    0x001
#define SDI_CH1    0x002
#define SDI_CH2    0x003
#define SDI_CH3    0x004
#define REC_CH0    0x010
#define REC_CH1    0x020
#define REC_CH2    0x030
#define REC_CH3    0x040
#define PB_CH      0x050
#define FPGA0      0x000
#define FPGA1      0x100
#define FPGA2      0x200
#define FPGA3      0x300

#define FPGA0_ADDR FPGA_ADDR0
#define FPGA1_ADDR FPGA_ADDR1
#define FPGA2_ADDR FPGA_ADDR2
#define FPGA3_ADDR FPGA_ADDR3

#define DELTA_FRACTION 15
#define DISP_WIDTH  1920
#define DISP_HEIGHT 1080
#define ZOOM_RATIO_DECODER(src,dst) (((src-1)/(dst-1))*(0x1<<DELTA_FRACTION))

#define SGQ_ENABLE  1
#define SGQ_DISABLE 0

#define SPECIAL_ENABLE  1
#define SPECIAL_DISABLE 0

#define LEFT_TOP 	0x0
#define RIGHT_TOP 	0x1
#define LEFT_BOT 	0x2
#define RIGHT_BOT 	0x3
#define CENTER		0x4

#define SGQ_LOCK    1
#define SGQ_UNLOCK  1

//output mode selection
#define SPLIT1  0
#define SPLIT4  1
#define SPLIT9  2
#define SPLIT16 3

//Video mode selection
#define Mode1080i60 0x2
#define Mode1080i50 0x3
#define Mode1080p30 0x4
#define Mode1080p25 0x5
#define Mode1080p24 0x6
#define Mode720p60  0x7
#define Mode720p50  0x9
#define Mode720p30  0xA
#define Mode720p25  0xB
#define Mode720p24  0xC

#define FORCECOLOR_DLY 200
#define STM32F10X_FIRM_METIS 

unsigned int I2C_READ(unsigned char fpga_addr, unsigned char regaddr);
void I2C_WRITE(unsigned char fpga_addr, unsigned char regaddr, unsigned int write_data);
void SGQ_CH_FULL(unsigned int ch);
void SGQ_SPLIT(unsigned char mode);
void SGQ_CH_POSITION(unsigned int ch, unsigned char en, unsigned char pos_num, unsigned char mode);
//void SGQ_RECSW_SEL(unsigned int ch, unsigned int sw_val);
//void SGQ_RECMODE_Encoding_SET(unsigned int ch, unsigned char en);
void SGQ_RECMODE_Enable_SET(unsigned int ch, unsigned char en);
//void SGQ_RECMODE_ChannelSWMax_SET(unsigned int ch, unsigned int val);
//void SGQ_RECMODE_VideoMode_SET(unsigned int ch, unsigned int val);
void SGQ_DISPLAY_VideoMode_SET(unsigned int ch, unsigned int val);
void SGQ_DISPLAY_Enable_SET(unsigned int ch, unsigned char en);
void SGQ_DISPLAY_Terminal_SET(unsigned int ch, unsigned char en);
void SGQ_DISPLAY_SlaveMode_SET(unsigned int ch, unsigned char en);
void SGQ_DISPLAY_CascadeMix_SET(unsigned int ch, unsigned char en);
void SGQ_DISPLAY_OSGOverlay_SET(unsigned int ch, unsigned char en);
void SGQ_DISPLAY_PBMode_SET(unsigned int ch, unsigned char en);
void SGQ_RECDISPLAY_INIT(unsigned int ch, unsigned int mode);
void SGQ_CH_Color(unsigned int ch, unsigned char force_en, unsigned char idle_count,
                  unsigned char y, unsigned char cb, unsigned char cr); 

unsigned char SGQ_READ_CH_LOCK(unsigned int ch);

void SGQ_1080i_16CH_INIT(void);
void SGQ_1080i_9CH_INIT(void);
void SGQ_1080i_4CH_INIT(void);

void SGQ_4CH_INIT(unsigned int mode);
void SGQ_9CH_INIT(unsigned int mode);
void SGQ_16CH_INIT(unsigned int mode);

void ForceColor(unsigned char mode);
void ForceColorDis(unsigned char mode);
//int ZoomDemo();
int ZoomDemo(unsigned char data);
void SGQ_RECMODE_BT656_BT1120_SET(char InFORM);

unsigned char SGQ_READ_CH_FLOCK(unsigned int ch); 


unsigned int demo4ch(unsigned char data);
unsigned int demo9ch(unsigned char data);
unsigned int demo16ch(unsigned char data);


void FullScreen(unsigned char data);

//void SGQ_SPECIALCH_DIS(unsigned int mode, unsigned char specialMode);
unsigned int demoSpecialSplit(unsigned char data, unsigned char specialMode);

#endif
