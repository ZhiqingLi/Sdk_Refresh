#ifndef __FMDEC_H__
#define __FMDEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ATAN2_HARDWARE 1
#define FM_NEWMODE 1
//#undef FM_NEWMODE

#define NUMOFSAMPLE 576





#define FM_FLOAT 1
#define FM_FIXED 2
#define FM_FIXED16BIT 3

#define FM_MATH  FM_FIXED16BIT

int FmDemodulate(short *Input, short *output, int NumSample);

#ifdef __cplusplus
}
#endif

#endif//__FMDEC_H__
