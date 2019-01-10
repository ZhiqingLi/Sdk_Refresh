#ifndef IIR_RESAMPLE_H
#define IIR_RESAMPLE_H

#include <stdint.h>

enum IIR_RESAMPLE_MODE
{
    IIR_RESAMPLE_MODE_1TO2 = 0,
    IIR_RESAMPLE_MODE_1TO3,
    IIR_RESAMPLE_MODE_1TO6,
    IIR_RESAMPLE_MODE_2TO1,
    IIR_RESAMPLE_MODE_3TO1,
    IIR_RESAMPLE_MODE_6TO1
};

struct IirResampleState_;

typedef struct IirResampleState_ IirResampleState;

#ifdef __cplusplus
extern "C" {
#endif

IirResampleState *iir_resample_init(enum IIR_RESAMPLE_MODE mode);

void iir_resample_destroy(IirResampleState *st);

int iir_resample_needed_out_count(IirResampleState *st, int in_size);

// frame_size is input buffer frame_size
void iir_resample_process(IirResampleState *st, int16_t *in, int16_t *out, int in_size);

#ifdef __cplusplus
}
#endif

#endif