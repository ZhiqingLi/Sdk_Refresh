#include "mfcc.h"
#define DTW_DIS_ERR     0xFFFFFFFF
#define DTW_DIS_MAX     0xFFFFFFFF

#define DTW_INSIDE      0
#define DTW_OUTSIDE     1

int dtw(mfcc_vct_t *vct_in, mfcc_vct_t *vct_mdl);