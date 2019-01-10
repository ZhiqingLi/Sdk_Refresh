#ifndef __WEBRTC_COMM_H__
#define __WEBRTC_COMM_H__

#include "hal_trace.h"

#if defined(__GNUC__) && !(defined(ROM_BUILD) || defined(PROGRAMMER))
#define VDT_FAST_LOC                   __attribute__((section(".overlay_text0")))
#else
#define VDT_FAST_LOC
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif//__WEBRTC_COMM_H__

