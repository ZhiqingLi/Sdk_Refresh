#ifndef _UCOM_COMM_H
#define _UCOM_COMM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include <string.h>

#ifndef USE_STATIC_MEMORY
#include <stdlib.h>
#endif

#ifdef TARGET_BEST1000
#include "hal_trace.h"
#endif

#ifdef USE_MED_MALLOC
#include "med_memory.h"
#endif

#define MED_TRACE TRACE
#define MED_FUNC_ENTRY_TRACE() //FUNC_ENTRY_TRACE()

#define UCOM_MemSet(pvDes, ucData, uwCount) \
	memset((pvDes), (int)(ucData), (uwCount))

#ifndef USE_STATIC_MEMORY
#ifdef USE_MED_MALLOC
#define UCOM_MemAlloc(uwSize) \
	med_malloc(uwSize)

#define UCOM_MemFree(pMem) \
	med_free((void*)(pMem))
#else
#define UCOM_MemAlloc(uwSize) \
	malloc(uwSize)

#define UCOM_MemFree(pMem) \
	free((void*)(pMem))
#endif
#endif


#define UCOM_MemCpy(pvDes, pvSrc, uwCount) \
	memcpy((pvDes), (const void *)(pvSrc), (uwCount))

#if defined(_MSC_VER)
#define UCOM_ALIGN(bytes)	__declspec(align(bytes))

#define UCOM_INLINE			__inline
#elif defined(__GNUC__)
#define UCOM_ALIGN(bytes)	__attribute__((aligned(bytes)))

#define UCOM_INLINE			static inline
#elif defined(__CC_ARM)
#define UCOM_ALIGN(bytes)	__align(bytes)

#define UCOM_INLINE			__inline
#elif defined(__ICCARM__)
#define UCOM_ALIGN(bytes)	//__attribute__((aligned(bytes)))

#define UCOM_INLINE         _Pragma("inline=forced")
#endif

#ifndef TARGET_BEST1000
#include <stdio.h>
#define __func__    __FUNCTION__
#define TRACE(str, ...)     printf(str, ##__VA_ARGS__)
#endif

#define UCOM_ASSERT(cond)   { if (!(cond)) { TRACE("%s line: %d\n", __func__, __LINE__); while(1); } }
#define UCOM_NOTREACHED()   UCOM_ASSERT(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _UCOM_COMM_H */
