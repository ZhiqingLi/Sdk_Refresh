#ifndef __BSP_AUDIO_H__
#define __BSP_AUDIO_H__

#define AUDIO_PATH_AUX             0
#define AUDIO_PATH_FM              1
#define AUDIO_PATH_SPEAKER         2
#define AUDIO_PATH_BTMIC           3
#define AUDIO_PATH_USBMIC          4
#define AUDIO_PATH_KARAOK          5

void audio_path_init(u8 path_idx);
void audio_path_exit(u8 path_idx);
void audio_path_start(u8 path_idx);
u8 get_aux_channel_cfg(void);
void aux2mic_path_init(u8 enable);  //AUX2MIC Ö±Í¨ÅäÖÃ
#endif //__BSP_AUDIO_H__

