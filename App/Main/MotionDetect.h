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

//=============================================================================
//  Static Variable Declaration
//=============================================================================

//=============================================================================
//  Array Declaration (data table)
//=============================================================================

//=============================================================================
//  Function Definition
//=============================================================================
void Read_MotionDetect_OnOff(void);
void Write_MotionDetect_OnOff(eChannel_t ch, BOOL enabled);
BOOL Get_MotionDetect_OnOff(eChannel_t ch);
void Set_MotionDetect_OnOff(eChannel_t ch, BOOL enabled);
void Read_MotionDetect_Sensitivity(eChannel_t ch);
void Write_MotionDetect_Sensitivity(eChannel_t ch, BYTE value);
BYTE Get_MotionDetect_Sensitivity(eChannel_t ch);
void Set_MotionDetect_Sensitivity(eChannel_t ch, BYTE value);
void MotionDetectCheck(void);

#endif
