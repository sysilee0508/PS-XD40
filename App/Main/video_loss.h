#ifndef __VIDEO_LOSS_H__
#define __VIDEO_LOSS_H__

#define VIDEO_LOSS_CHANNEL_NONE				0x00
#define VIDEO_LOSS_CHANNEL_ALL				0x0F

extern void ScanVideoLossChannels(void);

extern void SetVideoLossEvent(BOOL event);
extern BOOL GetVideoLossEvent(void);
extern BOOL IsVideoLossChannel(eChannel_t channel);
extern u8 GetVideoLossChannels(void);
extern void InitVideoLossCheck(void);

extern u8 GetVideoLossBuzzerCount(void);
extern void ClearVideoLossBuzzerCount(void);
extern void DecreaseVideoLossBuzzerCount(void);
#endif
