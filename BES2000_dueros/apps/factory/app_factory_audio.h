#ifndef __APP_FACTORY_BT_H__
#define __APP_FACTORY__BTH__

#include "app_utils.h"

int app_factorymode_audioloop(bool on, enum APP_SYSFREQ_FREQ_T freq);

int app_factorymode_mic_cancellation_run(void * mic_st, signed short *inbuf, int sample);

void *app_factorymode_mic_cancellation_init(void* (* alloc_ext)(int));

#endif
