#ifndef __DISPLAY_MODE_H__
#define __DISPLAY_MODE_H__

//=============================================================================
//  MACRO
//=============================================================================

//=============================================================================
//  define
//=============================================================================
#define DISPLAY_WIDTH_1920X1080				1920
#define DISPLAY_HEIGHT_1920x1080			1080
#define DISPLAY_WIDTH_1280x720				1280
#define DISPLAY_HEIGHT_1280x720				720
#define DISPLAY_WIDTH_960					960
#define DISPLAY_HEIGHT_480					480
#define DISPLAY_HEIGHT_576					576

#define DISPLAY_WIDTH 						DISPLAY_WIDTH_1920X1080
#define DISPLAY_HEIGHT						DISPLAY_HEIGHT_1920x1080

#define DISPLAY_HALF_WIDTH 					(DISPLAY_WIDTH/2)	//960
#define DISPLAY_HALF_HEIGHT					(DISPLAY_HEIGHT/2)	//540
#define DISPLAY_QUAD_WIDTH					(DISPLAY_WIDTH/4)	//480
#define DISPLAY_QUAD_HEIGHT					(DISPLAY_HEIGHT/4)	//270

#define PIP_WINDOW_WIDTH					DISPLAY_WIDTH/3
#define PIP_WINDOW_HEIGHT					DISPLAY_HEIGHT/3
#define PIP_POSITION_MARGIN					100

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
	// full
	DISPLAY_MODE_FULL_CH1,		// full screen - channel 1
	DISPLAY_MODE_FULL_CH2,
	DISPLAY_MODE_FULL_CH3,
	DISPLAY_MODE_FULL_CH4,
	// 2-Split
	DISPLAY_MODE_2SPLIT_HSCALE,
	DISPLAY_MODE_2SPLIT_HCROP,
	DISPLAY_MODE_2SPLIT_VSCALE,
	DISPLAY_MODE_2SPLIT_VCROP,
	// 4-Split
	DISPLAY_MODE_4SPLIT_QUAD,
	DISPLAY_MODE_4SPLIT_R3SCALE,
	DISPLAY_MODE_4SPLIT_R3CROP,
	DISPLAY_MODE_4SPLIT_L3SCALE,
	DISPLAY_MODE_4SPLIT_L3CROP,
	DISPLAY_MODE_4SPLIT_D3SCALE,
	DISPLAY_MODE_4SPLIT_D3CROP,
	DISPLAY_MODE_4SPLIT_U3SCALE,
	DISPLAY_MODE_4SPLIT_U3CROP,
	// PIP
	DISPLAY_MODE_PIP_A2,
	DISPLAY_MODE_PIP_A3,
	DISPLAY_MODE_PIP_A4,
	DISPLAY_MODE_PIP_B2,
	DISPLAY_MODE_PIP_B3,
	DISPLAY_MODE_PIP_B4,
	DISPLAY_MODE_PIP_C2,
	DISPLAY_MODE_PIP_C3,
	DISPLAY_MODE_PIP_C4,
	DISPLAY_MODE_PIP_D2,
	DISPLAY_MODE_PIP_D3,
	DISPLAY_MODE_PIP_D4,
	DISPLAY_MODE_MAX
} eDisplayMode_t;

#if 0
typedef enum
{
//	DISPLAY_MODE_FULL,			// full screen mode for any channel
	DISPLAY_MODE_QUAD_A,		// 4 split screen
	DISPLAY_MODE_QUAD_B,
	DISPLAY_MODE_QUAD_C,
	DISPLAY_MODE_QUAD_D,
	DISPLAY_MODE_QUAD_E,
	DISPLAY_MODE_3SPLIT_A,		// 3 split
	DISPLAY_MODE_3SPLIT_B,
	DISPLAY_MODE_3SPLIT_C,
	DISPLAY_MODE_3SPLIT_D,
	DISPLAY_MODE_2SPLIT,			// 2 split
	DISPLAY_MODE_MAX
} eSplitMode_t;

typedef enum
{
	DISPLAY_MODE_FULL,
	DISPLAY_MODE_SPLIT
} eDisplayMode_t;
#endif

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
//extern void DisplayMode_SplitScreen(eSplitMode_t splitMode);
//extern void Set_SystemDisplayMode(eDisplayMode_t mode);
//extern eDisplayMode_t Get_SystemDisplayMode(void);
//extern void Set_SystemSplitMode(eSplitMode_t mode);
//extern eSplitMode_t Get_SystemSplitMode(void);
//extern void Set_SystemDisplayChannel(eChannel_t channel);
//extern eChannel_t Get_SystemDisplayChannel(void);
extern eInputVideoMode_t Get_InputVideoMode(eChannel_t channel);
#endif
