/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_MEM_H_
#define _JOS_MEM_H_

/* DMA Handling APIs */

/* Get Physical/Virtual address of a block */
void *jdma_phys_addr(jdma_handle p, uint32_t offset);
void *jdma_virt_addr(jdma_handle p, uint32_t offset);

/* Get full descriptor of dma block */
mem_desc_h jdma_buffer_get(jdma_handle p);

/* Allocate new DMA block */
result_t jdma_alloc(uint32_t size, uint16_t align, void **vaddr, 
                void **paddr, uint16_t flags, jdma_handle *handle);

/* Free DMA block */
void jdma_free(jdma_handle handle);

#endif

