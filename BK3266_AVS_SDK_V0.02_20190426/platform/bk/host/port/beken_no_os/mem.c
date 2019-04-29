/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#include "includes.h"

#ifndef CONFIG_MEMPOOL
/* Allocate memory region */
void *os_malloc(uint32_t size, uint16_t flags)
{
    return NULL;
}

/* Free memory region */
void os_free(void *addr)
{
}
#endif

#ifndef CONFIG_MEMPOOL_DMABLE
/* Allocate DMA-able memory region */
result_t os_dma_alloc(uint32_t size, void **vaddr, void **paddr, uint16_t flags,
    void **cookie)
{
    return UWE_NOTSUP;
}

/* Free DMA-able memory region */
void os_dma_free(void *cookie)
{
}
#endif

/* Flush the cache to the RAM */
void os_cache_flush(void *vaddr, uint32_t size)
{
}

/* Mark the cache as invalidate */
void os_cache_invalidate(void *vaddr, uint32_t size)
{
}

