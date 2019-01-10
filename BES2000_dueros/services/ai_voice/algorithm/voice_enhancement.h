#ifndef AI_VOICE_ENHANCEMENT_H
#define AI_VOICE_ENHANCEMENT_H

#include <stdint.h>

#ifdef SPEECH_CAPTURE_TWO_CHANNEL
#define SV_ENHANCEMENT_FRAME_SIZE (16000 / 1000 * 10 * 2)
#else
#define SV_ENHANCEMENT_FRAME_SIZE (16000 / 1000 * 10)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void sv_enhancement_init(int sample_rate, int frame_size, int channels);

void sv_enhancement_destroy(void);

uint32_t sv_enhancement_process_capture(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
