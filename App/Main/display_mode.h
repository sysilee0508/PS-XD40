#ifndef __DISPLAY_MODE_H__
#define __DISPLAY_MODE_H__

//=============================================================================
//  MACRO
//=============================================================================

//=============================================================================
//  define
//=============================================================================
//#define

//=============================================================================
//  enum
//=============================================================================
typedef enum
{
	VIDEO_RESOLUTION_1080P,
	VIDEO_RESOLUTION_720P,
	VIDEO_RESOLUTION_MAX
} eVideoResolution_t;

typedef enum
{
	INPUT_VIDEO_1080P30,
	INPUT_VIDEO_1080P25,
	INPUT_VIDEO_720P30,
	INPUT_VIDEO_720P25,
	INPUT_VIDEO_CVBS_NTSC,
	INPUT_VIDEO_CVBS_PAL,
	INPUT_VIDEO_MAX
} eInputVideoMode_t;

typedef enum
{
//	DISPLAY_MODE_FULL,			// full screen mode for any channel
	DISPLAY_MODE_QUAD_A,		// 4 split screen
	DISPLAY_MODE_QUAD_B,
	DISPLAY_MODE_QUAD_B_CROP,
	DISPLAY_MODE_QUAD_C,
	DISPLAY_MODE_QUAD_C_CROP,
	DISPLAY_MODE_QUAD_D,
	DISPLAY_MODE_QUAD_D_CROP,
	DISPLAY_MODE_QUAD_E,
	DISPLAY_MODE_QUAD_E_CROP,
	DISPLAY_MODE_3SPLIT_A,		// 3 split
	DISPLAY_MODE_3SPLIT_A_CROP,
	DISPLAY_MODE_3SPLIT_B,
	DISPLAY_MODE_3SPLIT_B_CROP,
	DISPLAY_MODE_3SPLIT_C,
	DISPLAY_MODE_3SPLIT_C_CROP,
	DISPLAY_MODE_3SPLIT_D,
	DISPLAY_MODE_3SPLIT_D_CROP,
	DISPLAY_MODE_2SPLIT,			// 2 split
	DISPLAY_MODE_2SPLIT_CROP,
	DISPLAY_MODE_MAX
} eSplitMode_t;

typedef enum
{
	DISPLAY_MODE_FULL,
	DISPLAY_MODE_SPLIT
} eDisplayMode_t;


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
extern void Set_DisplayoutputMode_table(void);
extern void DisplayMode_FullScreen(eChannel_t ch);
extern void DisplayMode_SplitScreen(eSplitMode_t splitMode);
extern void Set_SystemDisplayMode(eDisplayMode_t mode);
extern eDisplayMode_t Get_SystemDisplayMode(void);
extern void Set_SystemSplitMode(eSplitMode_t mode);
extern eSplitMode_t Get_SystemSplitMode(void);
extern void Set_SystemDisplayChannel(eChannel_t channel);
extern eChannel_t Get_SystemDisplayChannel(void);
extern eInputVideoMode_t Get_InputVideoMode(eChannel_t channel);
#endif
