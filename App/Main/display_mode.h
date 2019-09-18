#ifndef __DISPLAY_MODE_H__
#define __DISPLAY_MODE_H__

//=============================================================================
//  MACRO
//=============================================================================
#define IS_FULL_MODE(mode)						(mode <= DISPLAY_MODE_FULL_CH4)?TRUE:FALSE
#define IS_PIP_MODE(mode)						((mode >= DISPLAY_MODE_PIP_A2)&&(mode <= DISPLAY_MODE_PIP_D4))?TRUE:FALSE

//=============================================================================
//  define
//=============================================================================
#define DISPLAY_WIDTH_1920X1080				1920
#define DISPLAY_HEIGHT_1920x1080				1080
#define DISPLAY_WIDTH_1280x720				1280
#define DISPLAY_HEIGHT_1280x720				720
#define DISPLAY_WIDTH_960						960
#define DISPLAY_HEIGHT_480						480
#define DISPLAY_HEIGHT_576						576

#define DISPLAY_WIDTH 							DISPLAY_WIDTH_1920X1080
#define DISPLAY_HEIGHT							DISPLAY_HEIGHT_1920x1080

#define DISPLAY_HALF_WIDTH 					(DISPLAY_WIDTH/2)	//960
#define DISPLAY_HALF_HEIGHT					(DISPLAY_HEIGHT/2)	//540
#define DISPLAY_QUAD_WIDTH					(DISPLAY_WIDTH/4)	//480
#define DISPLAY_QUAD_HEIGHT					(DISPLAY_HEIGHT/4)	//270

#define PIP_WINDOW_WIDTH						DISPLAY_WIDTH_1280x720 / 3	
#define PIP_WINDOW_HEIGHT						DISPLAY_HEIGHT_1280x720 / 3
#define PIP_POSITION_MARGIN					60

#define PIP_WINDOW_WIDTH_1080				DISPLAY_WIDTH_1920X1080/ 3	
#define PIP_WINDOW_HEIGHT_1080				DISPLAY_HEIGHT_1920x1080 / 3
#define PIP_POSITION_MARGIN_1080				90

//=============================================================================
//  enum
//=============================================================================
//typedef enum
//{
//	VIDEO_RESOLUTION_1080P,
//	VIDEO_RESOLUTION_720P,
//	VIDEO_RESOLUTION_MAX
//} eVideoResolution_t;

#if 0
typedef enum
{
	MDIN_ID_A = 0,
	MDIN_ID_B,
	MDIN_ID_C,
	MDIN_ID_D,
	MDIN_ID_MAX
} MDIN_CHIP_ID_t;
#endif
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
	DISPLAY_MODE_FULL_CH1 = 0,		// full screen - channel 1
	DISPLAY_MODE_FULL_CH2,
	DISPLAY_MODE_FULL_CH3,
	DISPLAY_MODE_FULL_CH4,
	// 2-Split	(8)
	DISPLAY_MODE_2SPLIT_HSCALE_A,	// ch 1,2
	DISPLAY_MODE_2SPLIT_HCROP_A,		// ch 1,2
	DISPLAY_MODE_2SPLIT_VSCALE_A,	// ch 1,2
	DISPLAY_MODE_2SPLIT_VCROP_A,		// ch 1,2
	DISPLAY_MODE_2SPLIT_HSCALE_B,	
	DISPLAY_MODE_2SPLIT_HCROP_B,
	DISPLAY_MODE_2SPLIT_VSCALE_B,
	DISPLAY_MODE_2SPLIT_VCROP_B,
	// 3-Splits 	(8)
	DISPLAY_MODE_3SPLIT_R2SCALE,
	DISPLAY_MODE_3SPLIT_R2CROP,
	DISPLAY_MODE_3SPLIT_L2SCALE,
	DISPLAY_MODE_3SPLIT_L2CROP,
	DISPLAY_MODE_3SPLIT_D2SCALE,
	DISPLAY_MODE_3SPLIT_D2CROP,
	DISPLAY_MODE_3SPLIT_U2SCALE,
	DISPLAY_MODE_3SPLIT_U2CROP,
	// 4-Split	(11)
	DISPLAY_MODE_4SPLIT_QUAD,
	DISPLAY_MODE_4SPLIT_R3SCALE,
	DISPLAY_MODE_4SPLIT_R3CROP,
	DISPLAY_MODE_4SPLIT_L3SCALE,
	DISPLAY_MODE_4SPLIT_L3CROP,
	DISPLAY_MODE_4SPLIT_D3SCALE,
	DISPLAY_MODE_4SPLIT_D3CROP,
	DISPLAY_MODE_4SPLIT_U3SCALE,
	DISPLAY_MODE_4SPLIT_U3CROP,
	DISPLAY_MODE_4SPLIT_H,
	DISPLAY_MODE_4SPLIT_X,
	// PIP	(12)
	DISPLAY_MODE_PIP_A2,	//main ch1, sub ch2, position A(right-top)
	DISPLAY_MODE_PIP_A3,	//main ch1, sub ch3, position A(right-top)
	DISPLAY_MODE_PIP_A4,	//main ch1, sub ch4, position A(right-top)
	DISPLAY_MODE_PIP_B2,	//main ch1, sub ch2, position B(right-bottom)
	DISPLAY_MODE_PIP_B3,	//main ch1, sub ch3, position B(right-bottom)
	DISPLAY_MODE_PIP_B4,	//main ch1, sub ch4, position B(right-bottom)
	DISPLAY_MODE_PIP_C2,
	DISPLAY_MODE_PIP_C3,
	DISPLAY_MODE_PIP_C4,
	DISPLAY_MODE_PIP_D2,
	DISPLAY_MODE_PIP_D3,
	DISPLAY_MODE_PIP_D4,
	DISPLAY_MODE_MAX	//35
} eDisplayMode_t;

typedef enum
{
	CROPPING_NONE,
	CROPPING_H,
	CROPPING_V
} eCroppingDirection_t;
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
extern void DisplayScreen(eDisplayMode_t mode);
extern void UpdateDisplayMode(void);
extern eDisplayMode_t GetCurrentDisplayMode(void);
//extern eDisplayMode_t GetSystemSplitMode(void);
extern BYTE GetInputVideoFormat(eChannel_t channel);
extern eChannel_t ConvertDisplayMode2Channel(eDisplayMode_t displayMode);
extern eChannel_t FindMainChannel(eDisplayMode_t displayMode, MDIN_CHIP_ID_t mdin);
extern eChannel_t FindMainChannel(eDisplayMode_t displayMode, MDIN_CHIP_ID_t mdin);
extern BOOL IsCroppingMode(eDisplayMode_t mode);
extern eCroppingDirection_t GetCroppingDirection(eDisplayMode_t mode);
#endif
