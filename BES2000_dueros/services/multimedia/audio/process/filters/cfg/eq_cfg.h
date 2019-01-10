#ifndef __FIR_PROCESS_H__
#define __FIR_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

void iir_lowshelf_coefs_generate(float gain, float fn, float Q, float *coefs);

void iir_highshelf_coefs_generate(float gain, float fn, float Q, float *coefs);

void iir_peak_notch_coefs_generate(float gain, float fn, float Q, float *coefs);

void iir_highpass_coefs_generate(float gain, float fn, float Q, float *coefs);

void iir_lowspass_coefs_generate(float gain, float fn, float Q, float *coefs);

float iir_convert_db_to_multiple(float db);

void iir_coefs_generate(IIR_TYPE_T type, float gain, float fn, float Q, float *coefs);

#ifdef __cplusplus
}
#endif

#endif
