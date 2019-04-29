/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*
 * Copyright (c) 1996, 1997, 1999, 2001, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center and Matt Thomas of 3am Software Foundry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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

#ifndef _JOS_MBUF_H_
#define _JOS_MBUF_H_

#include <include/jos.h>

/*
 * Macros for type conversion
 * mtod(m,t) -  convert mbuf pointer to data pointer of correct type
 */
#define mtod(m, t)      ((t)(void *)((m)->m_data))

/* flags copied when copying m_pkthdr */
#define M_COPYFLAGS     (M_PKTHDR|M_EOR|M_LINK0|M_PROTO1)

/* length to m_copym() to copy all */
#define	M_COPYALL       ((uint32_t)(-1))

/* mbuf types */
#define MT_FREE         0       /* should be on free list */
#define MT_DATA         1       /* dynamic (data) allocation */
#define MT_HEADER       2       /* packet header */
#define MT_SONAME       3       /* socket name */
#define MT_SOOPTS       4       /* socket options */
#define MT_FTABLE       5       /* fragment reassembly header */
#define MT_CONTROL      6       /* extra-data protocol message */
#define MT_OOBDATA      7       /* out-of-band data  */

typedef uint16_t mtype_t; /* keep the size of m_hdr 4 bytes aligned */

/* header at beginning of each mbuf, typical size 24 bytes: */
struct m_hdr {
    struct      mbuf *mh_next;          /* next buffer in chain */
    struct      mbuf *mh_nextpkt;       /* next chain in queue/record */
    int32_t     mh_buf_len;
    int32_t     mh_len;                 /* amount of data in this mbuf */
    phys_addr   mh_paddr;               /* physical address of mbuf */
    uint8_t     *mh_data;               /* beginning of valid data */
    uint16_t    mh_flags;               /* flags; see below */
    mtype_t     mh_type;                /* type of data in this mbuf */
    int32_t     mh_pkt_len;             /* valid if M_PKTHDR set */
   void       *mh_pkt_priv;           /* valid if M_PKTHDR set */
#ifdef CONFIG_JOS_MBUF_DMA_POOL
    jdma_handle mh_dma;                 /* DMA handle of mbuf allocation */
#endif
};

#define M_PADDR_INVALID POOL_PADDR_INVALID

typedef struct {
    uint8_t *ext_buf;
   void(*ext_free)(void *);
   void *args;
} m_buf_ext_t;

#define MLEN_MIN        4               /* min size of the data buffer */
struct mbuf {
    struct m_hdr        m_hdr;  /* 36 bytes. */
    union {
        m_buf_ext_t         m_buf_ext;             /* valid if M_EXT set */
        uint8_t             m_buf[MLEN_MIN];
    } mh_dat;    /* 12 bytes. */
};

#define MSIZE           256             /* default size of mbuf struct */
#define MCLSHIFT        11
#define MCLBYTES        (1 << MCLSHIFT) /* 'mbuf' cluster size */

/* Size of mbuf struct without the data buffer */
#define MSIZE_NODATA    J_OFFSET_OF(struct mbuf, m_buf)
#define MSIZE_EXT       sizeof(struct mbuf)

/* default size of mbuf data buffer */
#define MLEN (MSIZE - MSIZE_NODATA)
#define MHLEN MLEN      /* TODO: change this if using 'union' for fields that
                           are valid only when M_PKTHDR is set */

/* shortcuts */
#define m_next          m_hdr.mh_next
#define m_nextpkt       m_hdr.mh_nextpkt
#define m_buf_len       m_hdr.mh_buf_len
#define m_len           m_hdr.mh_len
#define m_paddr         m_hdr.mh_paddr
#define m_data          m_hdr.mh_data
#define m_flags         m_hdr.mh_flags
#define m_type          m_hdr.mh_type
#define m_pkt_len       m_hdr.mh_pkt_len
#define m_pkt_priv      m_hdr.mh_pkt_priv
#define m_dma           m_hdr.mh_dma
#define m_buf           mh_dat.m_buf
#define m_buf_ext       mh_dat.m_buf_ext

/*
 * Get current mbuf storage according to M_EXT flag
 */
#define M_BUF(m)       ((m)->m_flags & M_EXT ? \
    (m)->m_buf_ext.ext_buf : &(m)->m_buf[0])

/*
 * Set the m_data pointer of a newly-allocated mbuf (m_get/m_get_flags) to place
 * an object of the specified size at the end of the mbuf, uint32_t aligned.
 */
#define M_ALIGN(m, len)                                                 \
do {                                                                    \
    (m)->m_data += (m->m_buf_len - (len)) & (~(sizeof(uint32_t) - 1));  \
} while (0)

/*
 * As above, for mbufs allocated with m_gethdr/m_get_flags(type, M_PKTHDR, len)
 * or initialized by M_COPY_PKTHDR.
 */
#define MH_ALIGN M_ALIGN /* TODO: change this if using 'union' for fields that
                            are valid only when M_PKTHDR is set */

/*
 * Compute the amount of space available
 * before the current start of data in an mbuf
 */
#define M_LEADINGSPACE(m)       ((m)->m_data - M_BUF(m))

/*
 * Compute the amount of space available
 * after the end of data in an mbuf
 */
#define M_TRAILINGSPACE(m) \
    ((m)->m_data + (m)->m_len > (M_BUF(m) + (m)->m_buf_len - 1) ? 0 : \
        M_BUF(m) + (m)->m_buf_len - ((m)->m_data + (m)->m_len))

/*
 * Allow drivers and/or protocols to use the rcvif member of
 * PKTHDR mbufs to store private context information.
 */
#define	M_GETCTX(m, t)		((t)(m)->m_pkt_priv)
#define	M_SETCTX(m, c)		((void)((m)->m_pkt_priv = (void *)(c)))

/*
 * Simple mbuf queueing system
 *
 * this is basically a STAILQ adapted to mbuf use (ie using
 * m_nextpkt instead of field.sqe_next).
 *
 * m_next is ignored, so queueing chains of mbufs is possible
 */
#define MBUFQ_HEAD(name)                                        \
    struct name {                                               \
        struct mbuf *mq_first;                                  \
        struct mbuf **mq_last;                                  \
    }

#define MBUFQ_INIT(q)           do {                            \
    (q)->mq_first = NULL;                                       \
    (q)->mq_last = &(q)->mq_first;                              \
} while (0)

#define MBUFQ_ENQUEUE(q, m)     do {                            \
    (m)->m_nextpkt = NULL;                                      \
    *(q)->mq_last = (m);                                        \
    (q)->mq_last = &(m)->m_nextpkt;                             \
} while (0)

#define MBUFQ_PREPEND(q, m)     do {                            \
    if(((m)->m_nextpkt = (q)->mq_first) == NULL)               \
        (q)->mq_last = &(m)->m_nextpkt;                         \
    (q)->mq_first = (m);                                        \
} while (0)

#define MBUFQ_DEQUEUE(q, m)     do {                            \
    if(((m) = (q)->mq_first) != NULL)                          \
    {                                                           \
        if(((q)->mq_first = (m)->m_nextpkt) == NULL)           \
            (q)->mq_last = &(q)->mq_first;                      \
        else                                                    \
            (m)->m_nextpkt = NULL;                              \
    }                                                           \
} while (0)

#define MBUFQ_DRAIN(q)          do {                            \
    struct mbuf *__m0;                                          \
    while ((__m0 = (q)->mq_first) != NULL)                      \
    {                                                           \
        (q)->mq_first = __m0->m_nextpkt;                        \
        m_freem(__m0);                                          \
    }                                                           \
    (q)->mq_last = &(q)->mq_first;                              \
} while (0)

#define MBUFQ_ELEMNT_NUM(q,num)   do {                          \
	struct mbuf * __m0;											\
	__m0 = (q)->mq_first;										\
	while(__m0!=NULL)											\
	{														    \
		(*num)++; 											    \
		__m0 = (__m0)->m_nextpkt;							    \
	}														    \
}while(0)	

#define MBUFQ_ABANDON_EARLIEST(q)   do {                        \
	struct mbuf * __m0;									        \
	__m0 = (q)->mq_first;	                                    \
	(q)->mq_first = __m0->m_nextpkt;	                        \
    m_freem(__m0);                                              \
}while(0)	

#define MBUFQ_FIRST(q)          ((q)->mq_first)
#define MBUFQ_NEXT(m)           ((m)->m_nextpkt)
#define MBUFQ_LAST(q)           (*(q)->mq_last)
#define MBUFQ_EMPTY(q)          ((q)->mq_first == NULL)

#ifdef J_DEBUG
struct mbstat_cpu {
    uint32_t    m_mtypes[256];  /* type specific mbuf allocations */
};
#endif

struct mbuf *m_get_flags_ext(mtype_t type, uint16_t flags, int32_t len,
   void *ext_buf, void(*ext_free)(void *), void *args);
struct mbuf *m_free(struct mbuf *m);
void m_freem(struct mbuf *m);

/* Allocate a variable size mbuf with flags */
#define m_get_flags(type, flags, len) \
    m_get_flags_ext(type, flags, len, NULL, NULL, NULL)

/* Allocate a fixed size mbuf */
#define m_get(type) \
    m_get_flags(type, 0, MLEN)

/* Allocate a fixed size mbuf and initiate as a packet header */
#define m_gethdr(type) \
    m_get_flags(type, M_PKTHDR, MLEN)

/* Allocate a fixed size mbuf and zero out the data buffer */
#define m_getclr(type) \
    m_get_flags(type, M_ZERO, MLEN)

void m_reset(struct mbuf *m);

void m_adj(struct mbuf *m, int32_t len);
void m_cat(struct mbuf *m,struct mbuf *n);
struct mbuf *m_cat_pkt(struct mbuf *m, struct mbuf *n);
struct mbuf *m_prepend(struct mbuf *m, int32_t len);
struct mbuf *m_split(struct mbuf *m0, int32_t len0);

struct mbuf *m_copym(const struct mbuf *m, int32_t off0, int32_t len);
struct mbuf *m_copypacket(const struct mbuf *m);
result_t m_copyback(struct mbuf *m0, int32_t off, int32_t len,
    const void *vp);
void m_copydata(const struct mbuf *m, int32_t off, int32_t len, void *vp);
result_t m_trim(struct mbuf *m, int32_t len, void *p);
struct mbuf *m_release_pkt(struct mbuf *m, uint32_t *len);

/*
 * Return the number of bytes in the mbuf chain, m.
 */
static __INLINE__ int32_t m_length(const struct mbuf *m)
{
    const struct mbuf *m0;
    int32_t totlen;

    if((m->m_flags & M_PKTHDR) != 0)
        return m->m_pkt_len;

    for (m0 = m, totlen = 0; m0 != NULL; m0 = m0->m_next)
        totlen += m0->m_len;
    return totlen;
}

#define M_DBG_DUMP(type, level, m, hdr_only) \
    do {} while (0)

#endif /* !_JOS_MBUF_H_ */

