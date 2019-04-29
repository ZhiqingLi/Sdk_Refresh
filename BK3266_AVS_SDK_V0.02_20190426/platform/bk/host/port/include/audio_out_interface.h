#ifndef _BT_SAMPLE_AUDIO_OUT_H_
#define _BT_SAMPLE_AUDIO_OUT_H_

#include <uw_types.h>
#include "sys_config.h"

typedef __attribute__((aligned(4))) void *audio_out_ctx_h;

result_t audio_out_open(int_t rate, int_t channels, int_t bits_per_sample,
    audio_out_ctx_h *audio_out_ctx);
void audio_out_close(audio_out_ctx_h *audio_out_ctx);

result_t audio_out_write(audio_out_ctx_h audio_out_ctx, uint8_t *buffer,
    j_size_t size);

#endif

