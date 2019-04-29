/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_MEM_H_
#define _PORT_MEM_H_

#include "uw_types.h"

#define M_ZERO       0x0001              /* BZero the allocation */
#define M_CACHABLE   0x0002              /* Request cachable buffer */
#define M_PAGE_ALIGN 0x0004              /* Must not cross page boundaries */
#define M_KERNEL     0x0008              /* Request kernel DMA buffer */
#define M_SHARED     0x0010              /* Request shared kernel/user DMA */
                                         /* buffer */

/* mbuf flags */
#define M_PKTHDR        0x0100          /* start of record */
#define M_EOR           0x0200          /* end of record */
#define M_EXT           0x0400          /* mbuf use external buffer*/

#define M_PROTO1        0x1000          /* protocol specific flag */
#define M_LINK0         0x2000          /* link layer specific flag */

void *os_malloc(uint32_t size, uint16_t flags);
void os_free(void *addr);

result_t os_dma_alloc(uint32_t size, void **vaddr, void **paddr, uint16_t flags,
    void **cookie);
void os_dma_free(void *cookie);

void os_cache_flush(void *vaddr, uint32_t size);
void os_cache_invalidate(void *vaddr, uint32_t size);
#endif

