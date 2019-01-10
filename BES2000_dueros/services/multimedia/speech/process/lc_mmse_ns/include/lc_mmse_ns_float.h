#ifndef __LC_MMSE_NS_H__
#define __LC_MMSE_NS_H__

#include "speex/speex_echo.h"

#define LC_MMSE_NS_ENABLE_RES_ECHO

#ifdef __cplusplus
extern "C" {
#endif

/*
low complexity and log MMSE-based noise suppress
*/
struct LC_MMSE_NS_FLOAT_State_;

typedef struct LC_MMSE_NS_FLOAT_State_ LC_MMSE_NS_FLOAT_State;

LC_MMSE_NS_FLOAT_State *lc_mmse_ns_float_init(int frame_size, int sampling_rate, float denoise_dB);

void lc_mmse_ns_float_set_echo_state(LC_MMSE_NS_FLOAT_State *st, SpeexEchoState *echo_state);

void lc_mmse_ns_float_set_ec2_state(LC_MMSE_NS_FLOAT_State *state, void *ec2_state);

void lc_mmse_ns_float_set_echo_suppress(LC_MMSE_NS_FLOAT_State *st, int echo_suppress);

void lc_mmse_ns_float_analysis(LC_MMSE_NS_FLOAT_State *state, const short *in);

int lc_mmse_ns_float_run(LC_MMSE_NS_FLOAT_State *st, short *in);

int lc_mmse_ns_float_deinit(LC_MMSE_NS_FLOAT_State *st);


#ifdef __cplusplus
}
#endif

#endif
