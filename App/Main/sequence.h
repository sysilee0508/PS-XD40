#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__


//extern u8 autoSeqOn;

//extern u8 Motion_Call_Cnt[16];
//extern u8 bMotion_Call_FULL_Flag;
//extern u8 bMotion_Call_SPLIT_Flag;
//

extern void InitializeAutoSeq(void);
extern void UpdateAutoSeqCount(void);
extern void DisplayAutoSeqChannel(void);
//extern void Auto_Sequence(void);
extern BOOL GetAutoSeqOn(void);
extern void ChangeAutoSeqOn(BOOL set);

#endif
