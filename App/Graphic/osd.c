//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	OSD.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include ".\main\common.h"
#include	"..\drivers\mdinfont.h"
#include	"..\drivers\mdinaero.h"

#if	defined(SYSTEM_USE_MDIN380)
#include	"..\drivers\mdinpalt.h"
//#include	"..\drivers\mdintrue.h"
#endif

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
GACMAP_FONT_INFO stFONT;
SPRITE_CTL_INFO stOSD[12];
LAYER_CTL_INFO stLayer[3];
CURSOR_CTL_INFO stCurCTL;
SBOX_CTL_INFO stSBOX[8];
OSD_4CHVIEW_INFO st4CH;


// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
/*static void CreateCursorInstance(void)
{
	MDIN_BITMAP_INFO stBMP;
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152;	 //API v0.31(2012.05.02)

	// load cursor bmp
	stBMP.pBuff		= (PBYTE)mdin3xx_aero_cnv_bmp;
	stBMP.size		= sizeof(mdin3xx_aero_cnv_bmp);
	stBMP.addr		= e_addr-32*32;		// X-32*32;
	MDINOSD_SetBitmapData(stBMP.addr, stBMP.size, stBMP.pBuff);

#if __MDIN3xx_DBGPRT__ == 1
	printf("[OSD] CUR-BMP addr = %d, size = %d\n", stBMP.addr, stBMP.size);
#endif

	memset(&stCurCTL, 0, sizeof(CURSOR_CTL_INFO));
	stCurCTL.ctrl			= CURSOR_32BIT_8COLOR;
	stCurCTL.alpha			= CURSOR_OPAQUE;
	stCurCTL.stBMP.w		= 32;
	stCurCTL.stBMP.h		= 32;
	stCurCTL.stBMP.addr		= stBMP.addr;
	stCurCTL.stPAL.pBuff	= (PBYTE)mdin3xx_aero_rgb_pal;
	stCurCTL.stPAL.size		= sizeof(mdin3xx_aero_rgb_pal);

	MDINCUR_SetCursorConfig(&stCurCTL);
	MDINCUR_SetCursorPosition(&stCurCTL, 1000, 640);
}
*/
/*
#if	defined(SYSTEM_USE_BUS_HIF)
//--------------------------------------------------------------------------------------------------------------------------
static void XferDataBMP(DWORD src, DWORD dst, DWORD bytes)
{
	WORD unit = 4096;	// fix for MDIN380

	while (bytes>0) {
		WORD len = MIN(unit-(dst%unit), bytes);

		//FMEMORY_MultiRead(src, (PBYTE)BMPBuffDMA, len);
		//MDINOSD_SetBitmapData(dst, len, (PBYTE)BMPBuffDMA);
		src += len; dst += len; bytes -= len;
	}
}
#endif
*/

//--------------------------------------------------------------------------------------------------------------------------
/*static void CreateSpriteInstance(void)
{
	MDIN_BITMAP_INFO stBMP;
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*32;	   //API v0.31(2012.05.02)

	memset(&stBMP, 0, sizeof(MDIN_BITMAP_INFO));	// clear stBMP

#if	defined(SYSTEM_USE_BUS_HIF)
	stBMP.pBuff		= (PBYTE)0;				// 0
	stBMP.size		= 1280*960*3;
	stBMP.addr		= e_addr-stBMP.size;	// X-1280*960*3;
	XferDataBMP((DWORD)stBMP.pBuff, stBMP.addr, stBMP.size);

#if __MDIN3xx_DBGPRT__ == 1
	printf("[OSD] 24BIT-TRUE addr = %d, size = %d\n", stBMP.addr, stBMP.size);
#endif
#endif

	memset(&stOSD[SPRITE_INDEX3], 0, sizeof(SPRITE_CTL_INFO));
	stOSD[SPRITE_INDEX3].ctrl		= SPRITE_aRGB0888_24BIT;
	stOSD[SPRITE_INDEX3].alpha		= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX3].stBMP.w	= 1280;
	stOSD[SPRITE_INDEX3].stBMP.h	= 960;
	stOSD[SPRITE_INDEX3].stBMP.addr	= stBMP.addr;	// 0x0000

	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX3], SPRITE_INDEX3);
	MDINOSD_SetSpritePosition(&stOSD[SPRITE_INDEX3], 100, 60);

	e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*32-1280*960*3;		//API v0.31(2012.05.02)

#if	defined(SYSTEM_USE_BUS_HIF)
	stBMP.pBuff		= (PBYTE)3686400;	// 0+1280*960*3;
	stBMP.size		= 480*528*2;
	stBMP.addr		= e_addr-stBMP.size;	// X-480*528*2;
	XferDataBMP((DWORD)stBMP.pBuff, stBMP.addr, stBMP.size);

#if __MDIN3xx_DBGPRT__ == 1
	printf("[OSD] 16BIT-TRUE addr = %d, size = %d\n", stBMP.addr, stBMP.size);
#endif
#endif

	memset(&stOSD[SPRITE_INDEX4], 0, sizeof(SPRITE_CTL_INFO));
	stOSD[SPRITE_INDEX4].ctrl		= SPRITE_aRGB0565_16BIT;
	stOSD[SPRITE_INDEX4].alpha		= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX4].stBMP.w	= 480;
	stOSD[SPRITE_INDEX4].stBMP.h	= 528;
	stOSD[SPRITE_INDEX4].stBMP.addr	= stBMP.addr;	//

	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX4], SPRITE_INDEX4);
	MDINOSD_SetSpritePosition(&stOSD[SPRITE_INDEX4], 1340, 110);

	e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*144-1280*960*3-480*528*2;		 //API v0.31(2012.05.02)

#if	defined(SYSTEM_USE_BUS_HIF)
	stBMP.pBuff		= (PBYTE)4193280;	// 0+1280*960*3+480*528*2;
	stBMP.size		= 1024*560/2;
	stBMP.addr		= e_addr-stBMP.size;	// X-1024*560/2;
	XferDataBMP((DWORD)stBMP.pBuff, stBMP.addr, stBMP.size);

	st4CH.w_m = 1024; st4CH.h_m = 560; st4CH.addr_m = stBMP.addr;

#if __MDIN3xx_DBGPRT__ == 1
	printf("[OSD] 4BIT-PALT addr1 = %d, size = %d\n", stBMP.addr, stBMP.size);
#endif
#endif

	memset(&stOSD[SPRITE_INDEXB], 0, sizeof(SPRITE_CTL_INFO));
	stOSD[SPRITE_INDEXB].ctrl		= SPRITE_4BITPALT_COLOR;
	stOSD[SPRITE_INDEXB].alpha		= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEXB].stBMP.w	= 1024;
	stOSD[SPRITE_INDEXB].stBMP.h	= 560;
	stOSD[SPRITE_INDEXB].stBMP.addr	= stBMP.addr;	//

	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEXB], SPRITE_INDEXB);
	MDINOSD_SetSpritePosition(&stOSD[SPRITE_INDEXB], 876, 512);

	e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*144-1280*960*3-480*528*2-1024*560/2;		//API v0.31(2012.05.02)

#if	defined(SYSTEM_USE_BUS_HIF)
	stBMP.pBuff		= (PBYTE)4480000;	// 0+1280*960*3+480*528*2+1024*560/2;
	stBMP.size		= 1024*20/2*10;
	stBMP.addr		= e_addr-stBMP.size;	// X-1024*20/2*10;
	XferDataBMP((DWORD)stBMP.pBuff, stBMP.addr, stBMP.size);

	st4CH.w_s = 1024; st4CH.h_s = 20; st4CH.addr_s = stBMP.addr;

#if __MDIN3xx_DBGPRT__ == 1
	printf("[OSD] 4BIT-PALT addr2 = %d, size = %d\n", stBMP.addr, stBMP.size);
#endif
#endif

//#if	defined(SYSTEM_USE_BUS_HIF)
//	stBMP.pBuff		= (PBYTE)mdin3xx_true_cnv_bmp;
//	stBMP.size		= sizeof(mdin3xx_true_cnv_bmp);
//	stBMP.addr		= e_addr-stBMP.size;	// X-144*128*3;
//	MDINOSD_SetBitmapData(stBMP.addr, stBMP.size, stBMP.pBuff);

//#if __MDIN3xx_DBGPRT__ == 1
//	printf("[OSD] 24BIT-TRUE addr = %d, size = %d\n", stBMP.addr, stBMP.size);
//#endif
//#endif

	memset(&stOSD[SPRITE_INDEX4], 0, sizeof(SPRITE_CTL_INFO));
	stOSD[SPRITE_INDEX4].ctrl		= SPRITE_aRGB0888_24BIT;
	stOSD[SPRITE_INDEX4].alpha		= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX4].stBMP.w	= 144;
	stOSD[SPRITE_INDEX4].stBMP.h	= 128;
	stOSD[SPRITE_INDEX4].stBMP.addr	= stBMP.addr;	//

	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX4], SPRITE_INDEX4);
	MDINOSD_SetSpritePosition(&stOSD[SPRITE_INDEX4], 1340, 110);

}*/

//--------------------------------------------------------------------------------------------------------------------------
//static void CreateCopyInstance(void)
//{
/*
	GACCOPY_BMP_INFO stBMP;	MDIN_AREA_BOX stSRC, stDST;
#if	defined(SYSTEM_USE_MDIN380)
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*144-1280*960*3;
#else
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*32-1280*960*3;
#endif

	memset(&stBMP, 0, sizeof(GACCOPY_BMP_INFO));	// clear stBMP
	stBMP.attb		= GACMAP_24BITFULL_COLOR;
	stBMP.w			= 144;
	stBMP.h			= 128;
	stBMP.addr		= e_addr-144*128*3;

	MDINGAC_SetCopyMode(&stBMP, &stOSD[SPRITE_INDEX3]);

	stSRC.x =   0; stSRC.y =   0; stSRC.w = 144; stSRC.h = 128;
	stDST.x =   0; stDST.y =   0; stDST.w = 144; stDST.h = 128;
	MDINGAC_CopyRectangle(&stSRC, &stDST);

	stSRC.x =   0; stSRC.y =   0; stSRC.w = 144; stSRC.h = 128;
	stDST.x = 480; stDST.y =   0; stDST.w = 144; stDST.h = 128;
	MDINGAC_CopyRectangle(&stSRC, &stDST);
*/
//}

//--------------------------------------------------------------------------------------------------------------------------
//static void CreateFillInstance(void)
//{
/*
	MDIN_BITMAP_INFO stBMP;	GACFILL_BOX_INFO stFILL;
#if	defined(SYSTEM_USE_MDIN380)
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*144-1280*960*3-480*528*2;
#else
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112-384*152-32*32-1280*960*3-480*528*2;
#endif
	DWORD s_addr = e_addr-1920*1080*2;

	memset(&stBMP, 0, sizeof(MDIN_BITMAP_INFO));	// clear stBMP

#if	defined(SYSTEM_USE_BUS_HIF)
	stBMP.pBuff		= (PBYTE)4193280;	// 0+1280*960*3+480*528*2;
	stBMP.size		= 1920*1080*2;
	stBMP.addr		= s_addr;			//
	XferDataBMP((DWORD)stBMP.pBuff, stBMP.addr, stBMP.size);

#if __MDIN3xx_DBGPRT__ == 1
	printf("[OSD] 16BIT-FILL addr = %d, size = %d\n", stBMP.addr, stBMP.size);
#endif
#endif

	// set sprite4 in dspBMP-OSD display => SPRITE_INDEX4
	memset(&stOSD[SPRITE_INDEX5], 0, sizeof(SPRITE_CTL_INFO));	// clear stOSD[SPRITE_INDEX4]
	stOSD[SPRITE_INDEX5].ctrl			= SPRITE_aRGB1555_16BIT;
	stOSD[SPRITE_INDEX5].alpha			= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX5].x				= 0;
	stOSD[SPRITE_INDEX5].y				= 0;
	stOSD[SPRITE_INDEX5].stBMP.w		= 1920;			// width of dstBMP
	stOSD[SPRITE_INDEX5].stBMP.h		= 1080;			// height of dstBMP
	stOSD[SPRITE_INDEX5].stBMP.addr		= s_addr;		// address of dstBMP 
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX5], SPRITE_INDEX5);

	memset(&stFILL, 0, sizeof(GACFILL_BOX_INFO));	// clear stBOX
	stFILL.attb		= GACMAP_16BITFULL_COLOR;
	MDINGAC_SetFillMode(&stFILL, &stOSD[SPRITE_INDEX5]);

	stBOX.x = 256; stBOX.y = 256; stBOX.w = 512; stBOX.h = 512;
	MDINGAC_FillRectangle(&stBOX, aRGB(1,0,255,0));

	// enable sprite5
	MDINOSD_EnableSprite(&stOSD[SPRITE_INDEX5], ON);	// dstBMP-OSD display ON
*/
//}

//--------------------------------------------------------------------------------------------------------------------------
/*static void CreateSBoxInstance(void)
{
	memset(&stSBOX[SBOX_INDEX0], 0, sizeof(SBOX_CTL_INFO));
	stSBOX[SBOX_INDEX0].ctrl		= SBOX_BORDER_OFF | SBOX_PLANE_OFF;
	stSBOX[SBOX_INDEX0].alpha		= SBOX_OPAQUE;
	stSBOX[SBOX_INDEX0].h_thk		= 1;
	stSBOX[SBOX_INDEX0].v_thk		= 1;

//	stSBOX[SBOX_INDEX0].stRECT.lx	= 0;		// SBOX area
//	stSBOX[SBOX_INDEX0].stRECT.ly	= 0;
//	stSBOX[SBOX_INDEX0].stRECT.rx	= 719;
//	stSBOX[SBOX_INDEX0].stRECT.ry	= 479;

	stSBOX[SBOX_INDEX0].b_color		= RGB(0,64,128);	// SBOX border color
//	stSBOX[SBOX_INDEX0].p_color		= RGB(0,255,0);		// SBOX plane color

	MDINOSD_SetSBoxConfig(&stSBOX[SBOX_INDEX0], SBOX_INDEX0);

//	memset(&stSBOX[SBOX_INDEX1], 0, sizeof(SBOX_CTL_INFO));
//	stSBOX[SBOX_INDEX1].ctrl		= SBOX_BORDER_OFF | SBOX_PLANE_OFF;
//	stSBOX[SBOX_INDEX1].alpha		= SBOX_OPAQUE;
//	stSBOX[SBOX_INDEX1].h_thk		= 1;
//	stSBOX[SBOX_INDEX1].v_thk		= 1;

//	stSBOX[SBOX_INDEX1].b_color		= RGB(0,64,128);	// SBOX border color
//	stSBOX[SBOX_INDEX1].p_color		= RGB(0,255,255);	// SBOX plane color

	MDINOSD_SetSBoxConfig(&stSBOX[SBOX_INDEX1], SBOX_INDEX1);

}*/

//--------------------------------------------------------------------------------------------------------------------------
/*static void OSD_SetSprites(void)
{
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112;	 //API v0.31(2012.05.02)

	memset(&stOSD[SPRITE_INDEX0], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX0]
	stOSD[SPRITE_INDEX0].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEX0].alpha			= SPRITE_OPAQUE;
	//stOSD[SPRITE_INDEX0].x				= 104;
	//stOSD[SPRITE_INDEX0].y				= 60;
	stOSD[SPRITE_INDEX0].x				= 120;
	stOSD[SPRITE_INDEX0].y				= 320;
	stOSD[SPRITE_INDEX0].stBMP.w		= 12*32;			// stFONT.w * 32-column
	stOSD[SPRITE_INDEX0].stBMP.h		= 16*16;			// stFONT.h * 16-line
	stOSD[SPRITE_INDEX0].stBMP.addr		= e_addr-384*128;	// X-384*128
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX0], SPRITE_INDEX0);


	memset(&stOSD[SPRITE_INDEX1], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX1]
	stOSD[SPRITE_INDEX1].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEX1].alpha			= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX1].x				= 500;
	stOSD[SPRITE_INDEX1].y				= 800;
	stOSD[SPRITE_INDEX1].stBMP.w		= 12*32;			// stFONT.w * 32-column
	stOSD[SPRITE_INDEX1].stBMP.h		= 16*1;				// stFONT.h * 2-line
	stOSD[SPRITE_INDEX1].stBMP.addr		= e_addr-384*136;	// X-384*8
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX1], SPRITE_INDEX1);


	memset(&stOSD[SPRITE_INDEX2], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX2]
	stOSD[SPRITE_INDEX2].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEX2].alpha			= SPRITE_OPAQUE;
	//stOSD[SPRITE_INDEX2].x				= 120;
	//stOSD[SPRITE_INDEX2].y				= 320;
	stOSD[SPRITE_INDEX2].x				= 12;
	stOSD[SPRITE_INDEX2].y				= 16;
	stOSD[SPRITE_INDEX2].stBMP.w		= 12*32;			// stFONT.w * 32-column
	stOSD[SPRITE_INDEX2].stBMP.h		= 16*1;				// stFONT.h * 1-line
	stOSD[SPRITE_INDEX2].stBMP.addr		= e_addr-384*144;	// X-384*8
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX2], SPRITE_INDEX2);

}
*/
void OSD_SetSprite_layer0(void)
{
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-192*96;	 

	memset(&stOSD[SPRITE_INDEX0], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX0]
	stOSD[SPRITE_INDEX0].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEX0].alpha			= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX0].x				= 0;
	stOSD[SPRITE_INDEX0].y				= 0;
	stOSD[SPRITE_INDEX0].stBMP.w		= 1920;				
	stOSD[SPRITE_INDEX0].stBMP.h		= 1080;			
	stOSD[SPRITE_INDEX0].stBMP.addr		= e_addr-1920*(1080/2);
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX0], SPRITE_INDEX0);

	//OSD_SetLayer0
	memset(&stLayer[LAYER_INDEX0], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX0]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX0], LAYER_INDEX0);
}

/*void OSD_SetSprite_layerA(WORD x, WORD y)
{
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-192*96-1920*(1080/2);	 

	memset(&stOSD[SPRITE_INDEXA], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX0]
	stOSD[SPRITE_INDEXA].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEXA].alpha			= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEXA].x				= x;
	stOSD[SPRITE_INDEXA].y				= y;
	stOSD[SPRITE_INDEXA].stBMP.w		= 720;				
	stOSD[SPRITE_INDEXA].stBMP.h		= 576;			
	stOSD[SPRITE_INDEXA].stBMP.addr		= e_addr-720*(576/2);
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEXA], SPRITE_INDEXA);

	//OSD_SetLayer2
	memset(&stLayer[LAYER_INDEX2], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX2]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX2], LAYER_INDEX2);
}*/
/*void OSD_SetSprite_layer1(WORD x, WORD y)
{
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112;	 //API v0.31(2012.05.02)

	memset(&stOSD[SPRITE_INDEX1], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX1]
	stOSD[SPRITE_INDEX1].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEX1].alpha			= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX1].x				= x;
	stOSD[SPRITE_INDEX1].y				= y;
	stOSD[SPRITE_INDEX1].stBMP.w		= 12*32;			// stFONT.w * 32-column
	stOSD[SPRITE_INDEX1].stBMP.h		= 16*1;				// stFONT.h * 2-line
	stOSD[SPRITE_INDEX1].stBMP.addr		= e_addr-384*136;	// X-384*8
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX1], SPRITE_INDEX1);

	//OSD_SetLayer1
	memset(&stLayer[LAYER_INDEX1], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX1]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX1], LAYER_INDEX1);
}*/
/*
void OSD_SetSprite_layer2(WORD x, WORD y)
{
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192-32*112;	 //API v0.31(2012.05.02)

	memset(&stOSD[SPRITE_INDEX2], 0, sizeof(SPRITE_CTL_INFO));		// clear stOSD[SPRITE_INDEX2]
	stOSD[SPRITE_INDEX2].ctrl			= 0;				// font palette addr = 0
	stOSD[SPRITE_INDEX2].alpha			= SPRITE_OPAQUE;
	stOSD[SPRITE_INDEX2].x				= x;
	stOSD[SPRITE_INDEX2].y				= y;
	stOSD[SPRITE_INDEX2].stBMP.w		= 12*32;			// stFONT.w * 32-column
	stOSD[SPRITE_INDEX2].stBMP.h		= 16*1;				// stFONT.h * 1-line
	stOSD[SPRITE_INDEX2].stBMP.addr		= e_addr-384*144;	// X-384*8
	MDINOSD_SetSpriteConfig(&stOSD[SPRITE_INDEX2], SPRITE_INDEX2);

	//OSD_SetLayer2
	memset(&stLayer[LAYER_INDEX2], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX1]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX2], LAYER_INDEX2);
}
*/
//--------------------------------------------------------------------------------------------------------------------------
/*static void OSD_SetLayers(void)
{
	memset(&stLayer[LAYER_INDEX0], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX0]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX0], LAYER_INDEX0);

	memset(&stLayer[LAYER_INDEX1], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX1]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX1], LAYER_INDEX1);

	memset(&stLayer[LAYER_INDEX2], 0, sizeof(LAYER_CTL_INFO));		// clear stLayer[LAYER_INDEX1]
	MDINOSD_SetLayerConfig(&stLayer[LAYER_INDEX2], LAYER_INDEX2);
}*/

//--------------------------------------------------------------------------------------------------------------------------
void OSD_SetFontMAP(void)
{
	MDIN_BITMAP_INFO stBMP;
	DWORD e_addr = (DWORD)MDIN3xx_GetSizeOfBank()*2*8192;		//API v0.31(2012.05.02)

	stBMP.pBuff		= (PBYTE)mdin3xx_font_cnv_bmp;
	stBMP.size		= sizeof(mdin3xx_font_cnv_bmp);
	stBMP.addr		= e_addr-stBMP.size;	// 192*96-font
	MDINOSD_SetBitmapData(stBMP.addr, stBMP.size, stBMP.pBuff);

	stFONT.mode		= MDIN_GAC_DRAW_XYMODE;
	stFONT.attb		= GACMAP_4BITPALT_COLOR | MDIN_GAC_16Vx32H_GRID | MDIN_GAC_HORI_AUTOINC;
	stFONT.w		= 12;
	stFONT.h		= 24;
	stFONT.addr		= stBMP.addr;
	MDINGAC_SetFontMode(&stFONT, &stOSD[SPRITE_INDEX0]);

}

//--------------------------------------------------------------------------------------------------------------------------
//static void OSD_LoadPalette(void)
//{
/*	PLAYER_CTL_INFO pCTL = &stLayer[LAYER_INDEX0];
	PLAYER_PAL_INFO pPAL = &stLayer[LAYER_INDEX0].stPAL;

	pPAL->addr		= 0;
	pPAL->size		= sizeof(mdin3xx_font_rgb_pal);
	pPAL->pBuff		= (PBYTE)mdin3xx_font_rgb_pal;
	MDINOSD_SetLayerPalette(pCTL, pPAL->addr, pPAL->size, pPAL->pBuff);
#if	defined(SYSTEM_USE_MDIN380)
	pCTL = &stLayer[LAYER_INDEX1];
	pPAL = &stLayer[LAYER_INDEX1].stPAL;

	pPAL->addr		= 0;
	pPAL->size		= sizeof(mdin3xx_palt_rgb_pal);
	pPAL->pBuff		= (PBYTE)mdin3xx_palt_rgb_pal;
	MDINOSD_SetLayerPalette(pCTL, pPAL->addr, pPAL->size, pPAL->pBuff);
#endif	//	defined(SYSTEM_USE_MDIN380)
	pCTL = &stLayer[LAYER_INDEX2];
	pPAL = &stLayer[LAYER_INDEX2].stPAL;

	pPAL->addr		= 0;
	pPAL->size		= sizeof(mdinaux_font_yuv_pal);
	pPAL->pBuff		= (PBYTE)mdinaux_font_yuv_pal;
	MDINOSD_SetLayerPalette(pCTL, pPAL->addr, pPAL->size, pPAL->pBuff);
*/
//}

//--------------------------------------------------------------------------------------------------------------------------
static void OSD_LoadMenuStatus(void)
{
	SetMenuDefaultStatus();
}

//--------------------------------------------------------------------------------------------------------------------------
/*void OSD_SetDemo(void)
{
	CreateCursorInstance();
	CreateSpriteInstance();
	CreateCopyInstance();
	CreateFillInstance();
	CreateSBoxInstance();
}
*/
//--------------------------------------------------------------------------------------------------------------------------
void CreateOSDInstance(void)
{
	OSD_SetFontMAP();				// set GAC in character mode
	OSD_SetSprite_layer0();
	OSD_LoadMenuStatus();
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_SetFontGAC(SPRITE_INDEX_t index)
{
	MDINGAC_SetFontMode(&stFONT, &stOSD[index]);
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_ModifyPalette_M(BOOL rgb)
{
	PLAYER_CTL_INFO pCTL = &stLayer[LAYER_INDEX0];
	PLAYER_PAL_INFO pPAL = &stLayer[LAYER_INDEX0].stPAL;

	if (rgb==OSD_RGB_PALETTE) pPAL->pBuff = (PBYTE)mdin3xx_font_rgb_pal;
	else					  pPAL->pBuff = (PBYTE)mdin3xx_font_yuv_pal;

	pPAL->addr		= 0;
	pPAL->size		= sizeof(mdin3xx_font_rgb_pal);
	MDINOSD_SetLayerPalette(pCTL, pPAL->addr, pPAL->size, pPAL->pBuff);
}

//--------------------------------------------------------------------------------------------------------------------------
/*void OSD_ModifyPalette_X(BOOL rgb)
{
	PLAYER_CTL_INFO pCTL = &stLayer[LAYER_INDEX2];
	PLAYER_PAL_INFO pPAL = &stLayer[LAYER_INDEX2].stPAL;

	if (rgb==OSD_RGB_PALETTE) pPAL->pBuff = (PBYTE)mdin3xx_font_rgb_pal;
	else					  pPAL->pBuff = (PBYTE)mdin3xx_font_yuv_pal;

	pPAL->addr		= 0;
	pPAL->size		= sizeof(mdin3xx_font_rgb_pal);
	MDINOSD_SetLayerPalette(pCTL, pPAL->addr, pPAL->size, pPAL->pBuff);
}*/



/*  FILE_END_HERE */
