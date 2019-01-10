#ifndef __MED_MEMORY_H__
#define __MED_MEMORY_H__

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

/* #define RT_MEM_DEBUG */
#define RT_MEM_STATS

#ifdef __cplusplus
extern "C" {
#endif

#define MED_MULTI_INSTANCE 1

#define rt_size_t uint32_t
#define rt_uint8_t uint8_t
#define rt_uint16_t uint16_t
#define rt_uint32_t uint32_t

#if defined(MED_MULTI_INSTANCE)

struct med_heap_instance {
    /** pointer to the heap: for alignment, heap_ptr is now a pointer instead of an array */
    uint8_t *heap_ptr;
    /** the last entry, always unused! */
    struct heap_mem *heap_end;
    struct heap_mem *lfree;   /* pointer to the lowest free block */
    rt_size_t mem_size_aligned;
#ifdef RT_MEM_STATS
    rt_size_t used_mem, max_mem;
#endif
};

extern void med_system_heap_init(struct med_heap_instance *heap_inst, void *begin_addr, void *end_addr);
extern void *med_malloc(struct med_heap_instance *heap_inst, uint32_t size);
extern void *med_realloc(struct med_heap_instance *heap_inst, void *rmem, uint32_t newsize);
extern void *med_calloc(struct med_heap_instance *heap_inst, uint32_t count, uint32_t size);
extern void med_free(struct med_heap_instance *heap_inst, void *rmem);
#else
extern void med_system_heap_init(void *begin_addr, void *end_addr);
extern void *med_malloc(uint32_t size);
extern void *med_realloc(void *rmem, uint32_t newsize);
extern void *med_calloc(uint32_t count, uint32_t size);
extern void med_free(void *rmem);
#endif

#ifdef RT_MEM_STATS
#if defined(MED_MULTI_INSTANCE)
void med_memory_info(struct med_heap_instance *heap_inst, uint32_t *total,
                    uint32_t *used,
                    uint32_t *max_used);
#else
void med_memory_info(uint32_t *total,
                    uint32_t *used,
                    uint32_t *max_used);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /*__OPUS_MEMORY_H__ */
