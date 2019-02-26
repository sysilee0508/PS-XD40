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
	DISPLAY_MODE_FULL_CH1,		// full screen - channel 1
	DISPLAY_MODE_FULL_CH2,
	DISPLAY_MODE_SPLIT_A,
	DISPLAY_MODE_SPLIT_B,
	DISPLAY_MODE_SPLIT_C,
	DISPLAY_MODE_SPLIT_D,
	DISPLAY_MODE_SPLIT_E,
	DISPLAY_MODE_PIP_A,
	DISPLAY_MODE_PIP_B,
	DISPLAY_MODE_PIP_C,
	DISPLAY_MODE_PIP_D,
	DISPLAY_MODE_MAX
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
extern void DisplayScreen(eDisplayMode_t mode);
extern void UpdateDisplayMode(void);
extern eDisplayMode_t GetCurrentDisplayMode(void);
extern BYTE GetInputVideoFormat(eChannel_t channel);
#endif
