/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */
/*
 * Copyright (c) 1982, 1986, 1991, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _UW_TYPES_H_
#define _UW_TYPES_H_

#include "types.h"

#include <config/config.h>
#include <include/uw_errno.h>

#ifndef CONFIG_BYTE_ORDER
# error "CONFIG_BYTE_ORDER for this configuration is not specified !"
#endif

#define CPU_LITTLE_ENDIAN          1234 /* LSB first: i386, vax */
#define CPU_BIG_ENDIAN             4321 /* MSB first: 68000, ibm, net */

#define UW_MAX_INT8                (0x7f)
#define UW_MAX_INT16               (0x7fff)
#define UW_MAX_INT32               (0x7fffffff)
#define UW_MAX_UINT8               (0xff)
#define UW_MAX_UINT16              (0xffff)
#define UW_MAX_UINT32              (0xffffffff)

typedef struct {
#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
    uint32_t    high;
    uint32_t    low;
#else
    uint32_t    low;
    uint32_t    high;
#endif
} j_uint64_t;

typedef struct {
#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
    j_uint64_t        high;
    j_uint64_t        low;
#else
    j_uint64_t        low;
    j_uint64_t        high;
#endif
} j_uint128_t;

/* Unaligned types */
typedef uint8_t uw_u8_t;
typedef uint8_t uw_u16_t[2];
typedef uint8_t uw_u32_t[4];
typedef uint8_t uw_u64_t[8];
typedef uint8_t uw_u128_t[16];

typedef uint32_t phys_addr;

typedef struct {
    void        *vaddr;
    phys_addr   paddr;
    uint32_t    size;
} sg_list_item_t;

/* General size type (in bytes) */
typedef uint32_t j_size_t;
#define J_SIZE_MAX 0xffffffff

typedef void *jdma_handle;

/* Transfer block descriptor */
typedef struct mem_desc_s {
    void           *vaddr;
    phys_addr      paddr;
    sg_list_item_t *sg_list;
    jdma_handle    dma_h;
} mem_desc_t;
typedef mem_desc_t *mem_desc_h;

/* TODO add dma_h argument and fix all calls of MEM_DESC_SET */
#define MEM_DESC_SET(desc_h, _vaddr, _paddr, _sg_list) \
    do { \
        (desc_h)->vaddr = (_vaddr); \
        (desc_h)->paddr = (_paddr); \
        (desc_h)->sg_list = (_sg_list); \
        (desc_h)->dma_h = NULL; \
    } while (0)

#define MEM_DESC_CLEAR(desc_h) \
    MEM_DESC_SET(desc_h, NULL, 0, NULL)

#define MEM_DESC_COPY(dest_h, src_h) \
    do { \
        KASSERT0((src_h)); \
        MEM_DESC_SET(dest_h, (src_h)->vaddr, (src_h)->paddr, \
            (src_h)->sg_list); \
        (dest_h)->dma_h = (src_h)->dma_h; \
    } while (0)

/* MAC Address */
#define UW_MAC_ADDR_LEN     6
typedef uint8_t uw_mac_addr_t[UW_MAC_ADDR_LEN];

#ifndef UW_PAGE_SHIFT
#define UW_PAGE_SHIFT   12           /* LOG2(UW_PAGE_SIZE) */
#endif
#ifndef UW_PAGE_SIZE
#define UW_PAGE_SIZE    (1UL << UW_PAGE_SHIFT) /* bytes/page */
#endif

#endif
