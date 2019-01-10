#ifndef RBPCMBUF_HEADER
#define RBPCMBUF_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _RB_PCMBUF_AUD_STATE_T {
    RB_PCMBUF_AUD_STATE_STOP = 0,
    RB_PCMBUF_AUD_STATE_START,
} RB_PCMBUF_AUD_STATE_T;

void rb_pcmbuf_init(void);
void rb_pcmbuf_stop(void);
void *rb_pcmbuf_request_buffer(int *size);
void rb_pcmbuf_write(unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* RBPCMBUF_HEADER */
