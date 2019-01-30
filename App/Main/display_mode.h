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
extern void Set_DisplayoutputMode_table(void);
extern void DisplayMode_FullScreen(eChannel_t ch);
extern void DisplayMode_SplitScreen(eSplitMode_t splitMode);
extern void Set_SystemDisplayMode(eDisplayMode_t mode);
extern eDisplayMode_t Get_SystemDisplayMode(void);
extern void Set_SystemSplitMode(eSplitMode_t mode);
extern eSplitMode_t Get_SystemSplitMode(void);
extern void Set_SystemDisplayChannel(eChannel_t channel);
extern eChannel_t Get_SystemDisplayChannel(void);
#endif
