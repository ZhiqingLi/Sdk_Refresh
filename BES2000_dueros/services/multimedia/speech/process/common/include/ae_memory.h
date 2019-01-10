#ifndef AE_MEMORY_H
#define AE_MEMORY_H

#include <string.h>

#ifdef USE_MED_MALLOC
#include "med_memory.h"

void *bes_med_malloc(size_t size);
void *bes_med_realloc(void *rmem, size_t newsize);
void *bes_med_calloc(size_t count, size_t size);
void bes_med_free(void *rmem);


#define AE_MALLOC(size) \
    bes_med_malloc(size)

#define AE_FREE(size) \
    bes_med_free(size)
#else
#include <stdlib.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#define AE_MALLOC(size) \
    malloc(size)

#define AE_FREE(size) \
    free(size)
#endif

#define AE_MEMCPY(dst, src, len) \
    memcpy(dst, src, len)

#define AE_MEMSET(p, v, len) \
    memset(p, v, len)

#endif
