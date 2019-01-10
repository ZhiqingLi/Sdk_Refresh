#ifndef CNG_H
#define CNG_H

#include <stdint.h>

#ifdef CNG_DEBUG
#include <stdio.h>
#endif

typedef struct
{
    int frame_size;
    float alpha;
    int kk;
    float step;
    float ramp;
    uint32_t seed;
    float cng_norm_thr;

    int16_t *dd;
    int16_t *DD;
    float *Sy;
    float *Sym;
    float *Sym1;
    float *Sym2;

    void *fft_lookup;

#ifdef CNG_DEBUG
    FILE *sym_file;
    FILE *sym2_file;
#endif
} CngState;

CngState *cng_init();

void cng_estimate_noise_floor(CngState *st, const int16_t *dk);

void cng_process(CngState *st, const float *gain, float *noise);

void cng_destroy(CngState *st);

#endif