#ifndef CMSIS_FFTWRAP_H
#define CMSIS_FFTWRAP_H

void *cmsis_f32_fft_init(int size);

void cmsis_f32_fft_destroy(void *table);

void cmsis_f32_fft(void *table, float *in, float *out);

void cmsis_f32_ifft(void *table, float *in, float *out);

#endif