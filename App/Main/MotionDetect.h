#ifndef __MOTION_DETECT_H__
#define __MOTION_DETECT_H__

//=============================================================================
//  Header Declaration
//=============================================================================

//=============================================================================
//  Define & MACRO
//=============================================================================
#define ROWS_OF_BLOCKS						12
#define COLUMMS_OF_BLOCKS					16

#define BLOCK_WIDTH							DISPLAY_WIDTH / COLUMMS_OF_BLOCKS //120
#define BLOCK_HEIGHT						DISPLAY_HEIGHT / ROWS_OF_BLOCKS	//90

//=============================================================================
//  Static Variable Declaration
//=============================================================================

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================
extern BOOL Get_MotionDetect_OnOff(eChannel_t ch);
extern BYTE Get_MotionDetect_Sensitivity(void);
extern void Set_MotionDetect_Sensitivity(BYTE value);
extern void Set_MotionDetect_ActivatedArea(eChannel_t channel);
extern void MotionDetectCheck(void);
extern BOOL Get_MotionDetectedStatus(eChannel_t channel);
extern BYTE GetMotionBuzzerCount(void);
extern void DecreaseMotionBuzzerCount(void);
extern BOOL Get_MotionIndication(void);
#endif
