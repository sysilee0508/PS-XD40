#ifndef __SEQ__
#define __SEQ__


extern u8 bAuto_Seq_Flag;
extern u8 vAuto_Seq_Cnt;	
extern u8 vAuto_Seq_Index; 

extern u8 Motion_Call_Cnt[16];
extern u8 bMotion_Call_FULL_Flag;
extern u8 bMotion_Call_SPLIT_Flag;
 

extern void Auto_Seq_Init(void);
extern void Auto_Seq_Cnt(void);
extern void Auto_Sequence(void);


#endif  


