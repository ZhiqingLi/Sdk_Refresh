#ifndef __SPEECH_UTILS_H__
#define __SPEECH_UTILS_H__


#ifdef __cplusplus
extern "C" {
#endif

// len = fs / 1000 * ms
#define SPEECH_FRAME_MS_TO_LEN(fs, ms)      ( (fs) / 1000 * (ms) )
#define SPEECH_FRAME_LEN_TO_MS(fs, len)     ( (len) / ( (fs) / 1000) )

#ifdef __cplusplus
}
#endif

#endif