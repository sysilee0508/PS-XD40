#ifndef _VIDEO_DECODER_H_
#define _VIDEO_DECODER_H_

#include <raptor3_fmt.h>
#include <raptor3_ioctl.h>
#include <raptor3_audio.h>

#define RAPTOR3_MAX_CH_PER_CHIP 4
#define RAPTOR3_MAX_PORT_PER_CHIP 4

#define RAPTOR3_MAX_CH ( RAPTOR3_MAX_CH_PER_CHIP * RAPTOR3_MAX_PORT_PER_CHIP )

#define MAX_CH_PER_DECODERCHIP ( RAPTOR3_MAX_CH )
#define MAX_PORT_PER_DECODERCHIP ( RAPTOR3_MAX_CH )


#define RAPTOR3_4PORT_R0_ID 0xA1
#define RAPTOR3_2PORT_R0_ID 0xA0
#define RAPTOR3_1PORT_R0_ID 0xA2
#define CHIP_ID_UNKNOWN     0xFF

#define MAX_DEBOUNCE_CNT      10

#define DEV0 0
#define DEV1 1
#define DEV2 2
#define DEV3 3


#define VI_1MULTIPLEX_MODE  0
#define VI_2MULTIPLEX_MODE  1

typedef enum _NC_VIDEO_ONOFF
{
    VIDEO_LOSS_ON = 0,
    VIDEO_LOSS_OFF = 1,

} NC_VIDEO_ONOFF;

typedef struct VDEC_DEV_S
{
	int i2c_dev_num;
	int i2c_dev_addr;
	int vid_port_total;
	int vid_port_start;
	
	int ch_per_chip;
	int port_per_chip;

	NC_VIVO_CH_FORMATDEF FmtDef[MAX_CH_PER_DECODERCHIP];
	int FormatChanged[MAX_CH_PER_DECODERCHIP];

	int Mux_Mode;

} VDEC_DEV_S;

typedef struct VDEC_DEV_INFORM_S
{
	unsigned char chip_id[4];		/* Rx Chip ID 			*/
	unsigned char chip_rev[4];		/* Rx Chip Revision 	*/
	unsigned char chip_addr[4];		/* Rx Chip I2C Address  */
	unsigned char Total_Port_Num;	/* Rx Chip Total Port number */
	unsigned char Total_Chip_Cnt;	/* Rx Chip Total count  */
}VDEC_DEV_INFORM_S;

typedef struct _RAPTOR3_INFORMATION_S
{
    unsigned char           oMux;  										 /* 0:1mux, 1:2mux 				 */
    NC_VIVO_CH_FORMATDEF    curvideofmt[ RAPTOR3_MAX_CH ];				 /* Current Detection Format 	 */
    NC_VIVO_CH_FORMATDEF    prevideofmt[ RAPTOR3_MAX_CH ];				 /* Previous Detection Format 	 */
    unsigned char           curvideoloss[ RAPTOR3_MAX_CH ];				 /* Current Video Status 		 */
    unsigned char           vfc[RAPTOR3_MAX_CH];						 /* Video Format Class			 */
    unsigned char           debounce[RAPTOR3_MAX_CH][MAX_DEBOUNCE_CNT];	 /* Format Debounce	Array		 */
    unsigned char           debounceidx[RAPTOR3_MAX_CH];				 /* Format Debounce Index Arrary */
    VDEC_DEV_INFORM_S       chipinform;									 /* Decoder Information 		 */

} RAPTOR3_INFORMATION_S;


#endif

