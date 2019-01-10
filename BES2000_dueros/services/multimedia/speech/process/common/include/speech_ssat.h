#ifndef __SPEECH_SSAT_H__
#define __SPEECH_SSAT_H__


#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) && defined(__arm__)
#include "cmsis.h"
static inline short speech_ssat_int16(int in)
{
    short out;

    out = __SSAT(in,16);

    return out;
}
#else
static inline short speech_ssat_int16(int in)
{
    short out;

    if (in>32767)
    {
        in = 32767;
    }
    else if (in<-32768)
    {
        in = -32768;
    }
    out = (int)in;
    return out;
}
#endif

#ifdef __cplusplus
}
#endif

#endif