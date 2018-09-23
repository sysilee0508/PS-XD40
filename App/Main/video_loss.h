#ifndef __VIDEO_LOSS_H__
#define __VIDEO_LOSS_H__

#define VIDEO_LOSS_CHANNEL_NONE				0x00
#define VIDEO_LOSS_CHANNEL_ALL				0x0F
#define VIDEO_LOSS_CHANNEL(x)				0x01 << x


extern void Loss_Buzzer(void);
extern void Video_Loss_Check(void);




#endif
