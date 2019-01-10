#ifndef __APP_AUDTEST_H__
#define __APP_AUDTEST_H__

#include "audioflinger.h"
void da_output_sin1k(bool  on);
void adc_looptester(bool on,  uint8_t mode, enum AUD_SAMPRATE_T sample_rate);

#endif
