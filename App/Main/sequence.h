#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__


typedef enum
{
	AUTO_SEQ_NONE,
	AUTO_SEQ_NORMAL,
	AUTO_SEQ_ALARM,
//	AUTO_SEQ_MOTION,
	AUTO_SEQ_MAX
} eAutoSeqType_t;


extern void UpdateAutoSeqDisplayTime(void);
extern void InitializeAutoSeq(eAutoSeqType_t type);
extern void UpdateAutoSeqCount(void);
extern void DisplayAutoSeqChannel(void);
extern BOOL GetAutoSeqOn(void);
extern void ChangeAutoSeqOn(BOOL set);
extern eAutoSeqType_t GetCurrentAutoSeq(void);

#endif
