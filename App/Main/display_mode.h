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
extern void DisplayScreen(eDisplayMode_t mode);
extern void UpdateDisplayMode(void);
extern eDisplayMode_t GetCurrentDisplayMode(void);
extern BYTE GetInputVideoFormat(eChannel_t channel);
#endif
