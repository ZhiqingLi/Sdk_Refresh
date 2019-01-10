// Microsoft version of 'inline'

#ifdef WIN32
#define inline
#endif

#ifndef FIXED_POINT
#define FIXED_POINT
#endif

#if defined(__GNUC__) && defined(__arm__)
#include "hal_location.h"
#else
#define FLASH_TEXT_LOC
#define FLASH_RODATA_LOC
#endif


// In Visual Studio, _M_IX86_FP=1 means /arch:SSE was used, likewise
// _M_IX86_FP=2 means /arch:SSE2 was used.
// Also, enable both _USE_SSE and _USE_SSE2 if we're compiling for x86-64
#if _M_IX86_FP >= 1 || defined(_M_X64)
//#define _USE_SSE
#endif

#if _M_IX86_FP >= 2 || defined(_M_X64)
//#define _USE_SSE2
#endif

// Visual Studio support alloca(), but it always align variables to 16-bit
// boundary, while SSE need 128-bit alignment. So we disable alloca() when
// SSE is enabled.
#ifndef _USE_SSE
#  define USE_ALLOCA
#endif

/* Default to floating point */
#ifndef FIXED_POINT
#  define FLOATING_POINT
#  define USE_SMALLFT
#else
#  define USE_BES_FFTWRAP
#  define USE_KISS_FFT
#ifndef CHIP_BEST1000
#  define USE_BES_HWFFT
#endif
// Current it is slower than kissfft
//#  define USE_CMSIS_FFT // should enable DSP_LIB in target.mk as well
#endif

/* We don't support visibility on Win32 */
#define EXPORT


//#define OVERRIDE_SPEEX_FATAL

#define DISABLE_WARNINGS

#define DISABLE_NOTIFICATIONS

// #define OVERRIDE_SPEEX_PUTC

#define ONLY_USE_SPEEX_ECHO_CANCEL

// #define USE_STATIC_MEMORY


#if (defined(ADAPTIVE_FREQ_ALG)  || defined(SPEECH_TX_2MIC_NS2) || defined(SPEECH_TX_NS2)) && defined(USB_AUDIO_APP)
#define USE_STATIC_MEMORY
#endif
// #define USE_EXTERN_MEMORY

//#define FLOAT_POINT_AGC

#ifdef USE_STATIC_MEMORY

#define SPEEX_C 1
#define SPEEX_K 1
#if defined(ADAPTIVE_FREQ_ALG) && defined(USB_AUDIO_APP)
#define SPEEX_FRAME_SIZE 135
#define SPEEX_FILTER_LENGTH 135
#elif (defined(SPEECH_TX_2MIC_NS2)|| defined(SPEECH_TX_NS2)) && defined(USB_AUDIO_APP)
#define SPEEX_FRAME_SIZE 180
#define SPEEX_FILTER_LENGTH 180
#else
#define SPEEX_FRAME_SIZE 120
#define SPEEX_FILTER_LENGTH 120
#endif

#define NB_BANDS 24

#endif

