#ifndef __SPEECH_MEMORY_H__
#define __SPEECH_MEMORY_H__

#include "med_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

void *bes_med_malloc(size_t size);
void *bes_med_realloc(void *rmem, size_t newsize);
void *bes_med_calloc(size_t count, size_t size);
void bes_med_free(void *rmem);

//#define speech_heap_init(a, b)          med_system_heap_init(a, b)
#define speech_malloc(a)                bes_med_malloc(a)
#define speech_realloc(a, b)            bes_med_realloc(a, b)
#define speech_calloc(a, b)             bes_med_calloc(a, b)
#define speech_free(a)                  bes_med_free(a)
//#define speech_memory_info(a, b, c)     med_memory_info(a, b, c)

#ifdef __cplusplus
}
#endif

#endif