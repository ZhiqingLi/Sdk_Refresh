#ifndef AGC_H
#define AGC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *AgcState;

AgcState *agc_state_init(int32_t fs, int32_t frame_size);

void agc_state_destory(AgcState *st);

void agc_set_config(AgcState *st, int16_t target_level, int16_t compression_gain, uint8_t limiter_enable);

void agc_process(AgcState *st, int16_t *buf);

#ifdef __cplusplus
}
#endif

#endif