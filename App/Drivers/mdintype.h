//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name			:  MDINTYPE.H
// Description			:  This file contains typedefine for the driver files	
// Ref. Docment			: 
// Revision History 	:

#ifndef		__MDINTYPE_H__	
#define		__MDINTYPE_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

typedef void				VOID, *PVOID;		// void
typedef char				CHAR, *PCHAR;		// signed 8bit
typedef short				SHORT, *PSHORT;		// signed 16bit
typedef long				LONG, *PLONG;		// signed 32bit

typedef unsigned char		BYTE, *PBYTE;		// unsigned 8bit
typedef unsigned short		WORD, *PWORD;		// unsigned 16bit
typedef unsigned long		DWORD, *PDWORD;		// unsigned 32bit
typedef	float				FLOAT, *PFLOAT;		// floating number

typedef volatile void		VVOID, *VPVOID;		// volatile void
typedef volatile char		VCHAR, *VPCHAR;		// volatile signed 8bit
typedef volatile short		VSHORT, *VPSHORT;	// volatile signed 16bit
typedef volatile long		VLONG, *VPLONG;		// volatile signed 32bit

typedef volatile BYTE		VBYTE, *VPBYTE;		// volatile unsigned 8bit
typedef volatile WORD		VWORD, *VPWORD;		// volatile unsigned 16bit
typedef volatile DWORD		VDWORD, *VPDWORD;	// volatile unsigned 32bit

typedef DWORD				MDIN_COLORRGB, *PMDIN_COLORRGB;	// RGB color
typedef DWORD				MDIN_COLORYBR, *PMDIN_COLORYBR;	// YCbCr color

#define	BOOL				BYTE				// boolean flag
#define	ROMDATA				const				// program(read-only) memory

#define	TRUE				1
#define	FALSE				0

#define	ON					1
#define	OFF					0

#define	HIGH				1
#define	LOW					0

#define	POSI				1
#define	NEGA				0

#define	NACK				1
#define	ACK					0

#define DMA_RD				1
#define DMA_WR				0

#ifndef NULL
#define	NULL				0
#endif

//#define CH1					0
//#define CH2					1	
//#define CH3					2
//#define CH4					3

#define 	SYSTEM_USE_MDIN380			// for use MDIN380

//#define		SYSTEM_USE_4D1_IN		// by hungry 2012.02.08

#ifdef		SYSTEM_USE_4D1_IN
/*
#define		SYSTEM_USE_4D1_IN_QUADOUT
//#define	SYSTEM_USE_4D1_IN_656OUT
#define		NTSC_4D1_IN
//#define	PAL_4D1_IN

//#define	IN_960H_MODE		// added on 29Feb2012
*/
#endif

#define 	IN_960H_MODE	// added by kukuri s
#ifdef		SYSTEM_USE_MDIN380
//#define 	SYSTEM_USE_BUS_HIF			// for use BUS-IF & I2C-IF
//#define 	SYSTEM_USE_PCI_HIF			// for use PCI-IF & I2C-IF
#define 	SYSTEM_USE_I2C_HIF			// for use I2C-IF only
#endif

#if			defined(SYSTEM_USE_MDIN325A)||defined(SYSTEM_USE_MDIN345A)||defined(SYSTEM_USE_MDIN380)
//#define 	SYSTEM_USE_MCLK202			// for use 202MHz in memory clock
#define 	SYSTEM_USE_MCLK189			// for use 189MHz in memory clock
#endif

#ifdef		SYSTEM_USE_BUS_HIF
#define 	CPU_ACCESS_BUS_NBYTE	0	// "1" for 16-bit bus, "0" for 8-bit bus
#define 	CPU_ACCESS_BUS_NOMUX	0	// "1" for seperated-bus, "0" for muxed-bus
#define 	CPU_MEM_XFER_USE_DMA	0	// "1" for use DMA, "0" for no use DMA
#endif

#define		SYSTEM_USE_DAC_OUT		1	// "1" for use DAC, "0" for no use DAC

#if			defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN345A)||defined(SYSTEM_USE_MDIN380)
#define		SYSTEM_USE_HTX_HDCP		0	// "1" for use hdcp, "0" for no use hdcp
#endif

#define 	CPU_ALIGN_BIG_ENDIAN	0	// "1" for big-endian, "0" for little-endian
#define		VGA_SOURCE_EXTENSION	1	// "1" for extension, "0" for representative

#define		RGB_GAIN_OFFSET_TUNE	0	// "1" for tune gain/offset, "0" for not tune
#define		SOURCE_CSC_STD_RANGE	1	// "1" for standard range, "0" for full range
#define		OUTPUT_CSC_STD_RANGE	0	// "1" for standard range, "0" for full range

#define		DIFF(a,b)			((a)>(b)? (a-b):(b-a))
#define		MBIT(a,b)			(((a)&(b))? ON : OFF)
#define		RBIT(a,b)			(((a)&(b))? OFF : ON)

#define		MIN(a,b)			((a)>(b)? (b):(a))
#define		MAX(a,b)			((a)>(b)? (a):(b))

#define		HI4BIT(a)			((BYTE)((a)>>4))
#define		LO4BIT(a)			((BYTE)((a)&0x0f))

#define		HIBYTE(a)			((BYTE)((a)>>8))
#define		LOBYTE(a)			((BYTE)((a)&0xff))

#define		HIWORD(a)			((WORD)((a)>>16))
#define		LOWORD(a)			((WORD)((a)&0xffff))

#define		CLIP12(a)			((WORD)((((a)>2047)? 2047:((a)<-2048)? -2048:(a))))
#define		CLIP09(a)			((WORD)((((a)>255)? 255:((a)<-256)? -256:(a))))

#define		MAKEBYTE(a,b)		((BYTE)(((BYTE)(a)<<4)|(b)))
#define		MAKEWORD(a,b)		((WORD)(((WORD)(a)<<8)|(b)))
#define		MAKEDWORD(a,b)		((DWORD)(((DWORD)(a)<<16)|(b)))

#define		BYTESWAP(a)			(MAKEWORD(LOBYTE(a), HIBYTE(a)))

#define		ADDR2ROW(a,b)		((WORD)((((a)/(b))*2)|(((a)%(b))/((b)/2))))
#define		ADDR2COL(a,b)		((WORD)((a)%((b)/2)))
#define		ROW2ADDR(a,b)		((DWORD)((a)/2*(b))+(((a)&1)*((b)/2)))

#define		ROUNDUP(a,b)		((WORD)(((a)/(b))+(((a)%(b))? 1:0)))
#define		ROUNDDN(a,b)		((WORD)(((a)/(b))))

#ifdef		SYSTEM_USE_ARM_LINUX
#define		stPACKED			__attribute__((packed))
#else
#define		stPACKED
#endif

#define                SYSTEM_USE_FIXED_MEMMAP                     1 // "1" fix the aux start row, "0" according to the calculated main end row

#if                       SYSTEM_USE_FIXED_MEMMAP == 1 // row addr : 0~8191

//#define             AUX_START_ADDR                          3810       // 1080i input

//#define             AUX_START_ADDR                          4572       //when 2HD mode (1080p input)

#define  AUX_START_ADDR                          4956       //when 2HD mode (1080i input)

#endif 

#define  SYSTEM_USE_AUDLY_MEMSIZE                   16          // memory size for hdmi audio delay (lip sync)


#define 	__MDIN3xx_DBGPRT__		0	// "1" for debug print, "0" for disable debug
#define 	__MDINHTX_DBGPRT__		0	// "1" for debug print, "0" for disable debug
#define 	__MDINSDI_DBGPRT__		0	// "1" for debug print, "0" for disable debug	 //by hungry 2012.03.15

typedef	enum {
	MDIN_NO_ERROR = 0,		// no error
	MDIN_I2C_ERROR,			// i2c error (i2c_busy, i2c_no_ack)
	MDIN_TIMEOUT_ERROR,		// timeout error
	MDIN_INVALID_FORMAT,	// invalid src/out video format
	MDIN_INVALID_PARAM,		// invalid parameters
	MDIN_INVALID_DEV_ID,	// invalid device-ID

	MDIN_DDC_ACK_ERROR,		// ddc error (ddc_busy, ddc_no_ack)
	MDIN_DDC_CRC_ERROR,		// crc error (edid_crc)
	MDIN_SHA_CMP_ERROR		// sha error (compare vi)

}	MDIN_ERROR_t;

typedef	enum {
	MDIN_HOST_ID	= 0xc0,		// host block ID
	MDIN_LOCAL_ID	= 0xc2,		// local block ID
	MDIN_HDMI_ID	= 0xc4,		// hdmi block ID
	MDIN_SDRAM_ID	= 0xc6		// sdram block ID

}	MDIN_BLOCK_ID_t;

typedef	enum {
	MIN1_HOST_ID	= 0xc0,		// host block ID
	MIN1_LOCAL_ID	= 0xc2,		// local block ID
	MIN1_HDMI_ID	= 0xc4,		// hdmi block ID
	MIN1_SDRAM_ID	= 0xc6,		// sdram block ID
	
	MIN2_HOST_ID	= 0xd0,		// host block ID
	MIN2_LOCAL_ID	= 0xd2,		// local block ID
	MIN2_HDMI_ID	= 0xd4,		// hdmi block ID
	MIN2_SDRAM_ID	= 0xd6,		// sdram block ID
	
	MOUT1_HOST_ID	= 0xe0,		// host block ID
	MOUT1_LOCAL_ID	= 0xe2,		// local block ID
	MOUT1_HDMI_ID	= 0xe4,		// hdmi block ID
	MOUT1_SDRAM_ID	= 0xe6,		// sdram block ID

	MOUT2_HOST_ID	= 0xf0,		// host block ID
	MOUT2_LOCAL_ID	= 0xf2,		// local block ID
	MOUT2_HDMI_ID	= 0xf4,		// hdmi block ID
	MOUT2_SDRAM_ID	= 0xf6,		// sdram block ID
	
}	MDINXRC_BLOCK_ID_t;


#if			defined(SYSTEM_USE_MDIN380)&&defined(SYSTEM_USE_BUS_HIF)
	#if		CPU_MEM_XFER_USE_DMA == 1
		#if		CPU_ACCESS_BUS_NBYTE == 1
#define		MDINHIF_MultiWrite(id,addr,pBuf,bytes)		MDINBUS_MultiWrite(id,addr,pBuf,bytes)
#define		MDINHIF_RegWrite(id,addr,data)				MDINBUS_RegWrite(id,addr,data)
#define		MDINHIF_MultiRead(id,addr,pBuf,bytes)		MDINBUS_MultiRead(id,addr,pBuf,bytes)
#define		MDINHIF_RegRead(id,addr,pBuf)				MDINBUS_RegRead(id,addr,pBuf)
#define		MDINHIF_RegField(id,addr,pos,cnt,data)		MDINBUS_RegField(id,addr,pos,cnt,data)
#define 	MDINHIF_DMAWrite(id,addr,pBuf,bytes)		MDINBUS_DMAWrite(id,addr,pBuf,bytes)
#define 	MDINHIF_DMARead(id,addr,pBuf,bytes)			MDINBUS_MultiRead(id,addr,pBuf,bytes)

		#else// CPU_ACCESS_BUS_NBYTE == 0
#define		MDINHIF_MultiWrite(id,addr,pBuf,bytes)		MDINBUS_MultiWrite(id,addr,pBuf,bytes)
#define		MDINHIF_RegWrite(id,addr,data)				MDINBUS_RegWrite(id,addr,data)
#define		MDINHIF_MultiRead(id,addr,pBuf,bytes)		MDINBUS_MultiRead(id,addr,pBuf,bytes)
#define		MDINHIF_RegRead(id,addr,pBuf)				MDINBUS_RegRead(id,addr,pBuf)
#define		MDINHIF_RegField(id,addr,pos,cnt,data)		MDINBUS_RegField(id,addr,pos,cnt,data)
#define 	MDINHIF_DMAWrite(id,addr,pBuf,bytes)		MDINBUS_DMAWrite(id,addr,pBuf,bytes)
#define 	MDINHIF_DMARead(id,addr,pBuf,bytes)			MDINBUS_DMARead(id,addr,pBuf,bytes)
		#endif	// CPU_ACCESS_BUS_NBYTE

	#else// CPU_MEM_XFER_USE_DMA == 0 

#define		MDINHIF_MultiWrite(id,addr,pBuf,bytes)		MDINBUS_MultiWrite(id,addr,pBuf,bytes)
#define		MDINHIF_RegWrite(id,addr,data)				MDINBUS_RegWrite(id,addr,data)
#define		MDINHIF_MultiRead(id,addr,pBuf,bytes)		MDINBUS_MultiRead(id,addr,pBuf,bytes)
#define		MDINHIF_RegRead(id,addr,pBuf)				MDINBUS_RegRead(id,addr,pBuf)
#define		MDINHIF_RegField(id,addr,pos,cnt,data)		MDINBUS_RegField(id,addr,pos,cnt,data)
#define 	MDINHIF_DMAWrite(id,addr,pBuf,bytes)		MDINBUS_MultiWrite(id,addr,pBuf,bytes)
#define 	MDINHIF_DMARead(id,addr,pBuf,bytes)			MDINBUS_MultiRead(id,addr,pBuf,bytes)
	#endif	// CPU_MEM_XFER_USE_DMA 

#else
#define		MDINHIF_MultiWrite(id,addr,pBuf,bytes)		MDINI2C_MultiWrite(id,addr,pBuf,bytes)
#define		MDINHIF_RegWrite(id,addr,data)				MDINI2C_RegWrite(id,addr,data)
#define		MDINHIF_MultiRead(id,addr,pBuf,bytes)		MDINI2C_MultiRead(id,addr,pBuf,bytes)
#define		MDINHIF_RegRead(id,addr,pBuf)				MDINI2C_RegRead(id,addr,pBuf)
#define		MDINHIF_RegField(id,addr,pos,cnt,data)		MDINI2C_RegField(id,addr,pos,cnt,data)
#define		MDINHIF_DMAWrite(id,addr,pBuf,bytes)		MDINI2C_MultiWrite(id,addr,pBuf,bytes)
#define		MDINHIF_DMARead(id,addr,pBuf,bytes)			MDINI2C_MultiRead(id,addr,pBuf,bytes)
#endif



#endif	/* __MDINTYPE_H__ */
