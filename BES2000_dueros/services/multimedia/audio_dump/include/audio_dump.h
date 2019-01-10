#ifndef __AUDIO_DUMP_H__
#define __AUDIO_DUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AUDIO_DEBUG
#define AUDIO_DUMP
#endif

void audio_dump_init(int frame_len, int channel_num);
void audio_dump_clear_up(void);
void audio_dump_add_channel_data(int channel_id, short *pcm_buf, int pcm_len);
void audio_dump_run(void);

#ifdef __cplusplus
}
#endif

#endif