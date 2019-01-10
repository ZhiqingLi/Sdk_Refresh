#ifndef __SPEECH_WIN_H__
#define __SPEECH_WIN_H__


#ifdef __cplusplus
extern "C" {
#endif

/*
frame_size: 120, 128, 240, 256
*/
const short *get_conj_win_half(int win_size);

#ifdef __cplusplus
}
#endif

#endif
