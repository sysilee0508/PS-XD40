#ifndef __DISPLAY_MODE_H__
#define __DISPLAY_MODE_H__

//=============================================================================
//  MACRO
//=============================================================================

//=============================================================================
//  define
//=============================================================================

//=============================================================================
//  enum
//=============================================================================
typedef enum
{
	VIDEO_RESOLUTION_1080P,	// 1080P
	VIDEO_RESOLUTION_720P,		// 720P
	VIDEO_RESOLUTION_MAX
} eVideoResolution_t;

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

//=============================================================================
//  struct
//=============================================================================

//=============================================================================
//  typedef
//=============================================================================

//=============================================================================
//  Extern Grobal Variable
//=============================================================================

//=============================================================================
//  Function Prototype
//=============================================================================
extern void Display_FullScreen(eChannel_t ch);
extern void Display_QuadScreen(void);
#endif

