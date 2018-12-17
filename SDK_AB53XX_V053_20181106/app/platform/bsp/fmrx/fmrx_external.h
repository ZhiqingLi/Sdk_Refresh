#ifndef _FMRX_EXTERNAL_H
#define _FMRX_EXTERNAL_H

#include "qn8035.h"

void fmrx_rec_init_ext(void);
void fmrx_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

#endif // _FMRX_EXTERNAL_H
