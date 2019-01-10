#ifndef POLARSSL_MEMORY_BUFFER_ALLOC_H
#define POLARSSL_MEMORY_BUFFER_ALLOC_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "nvrec_config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <stdlib.h>

typedef struct _memory_header memory_header;
struct _memory_header
{
    size_t          magic1;
    size_t          size;
    size_t          alloc;
    memory_header   *prev;
    memory_header   *next;
    memory_header   *prev_free;
    memory_header   *next_free;
#if defined(POLARSSL_MEMORY_BACKTRACE)
    char            **trace;
    size_t          trace_count;
#endif
    size_t          magic2;
};


typedef struct
{
    unsigned char   *buf;
    size_t          len;
    memory_header   *first;
    memory_header   *first_free;
    size_t          current_alloc_size;
    int             verify;
#if defined(POLARSSL_MEMORY_DEBUG)
    size_t          malloc_count;
    size_t          free_count;
    size_t          total_used;
    size_t          maximum_used;
    size_t          header_count;
    size_t          maximum_header_count;
#endif
#if defined(POLARSSL_THREADING_C)
    threading_mutex_t   mutex;
#endif
}
buffer_alloc_ctx;

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in nvrec_config.h or define them on the compiler command line.
 * \{
 */

#if !defined(POLARSSL_MEMORY_ALIGN_MULTIPLE)
#define POLARSSL_MEMORY_ALIGN_MULTIPLE       4 /**< Align on multiples of this value */
#endif

/* \} name SECTION: Module settings */

#define MEMORY_VERIFY_NONE         0
#define MEMORY_VERIFY_ALLOC        (1 << 0)
#define MEMORY_VERIFY_FREE         (1 << 1)
#define MEMORY_VERIFY_ALWAYS       (MEMORY_VERIFY_ALLOC | MEMORY_VERIFY_FREE)

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief   Initialize use of stack-based memory allocator.
 *          The stack-based allocator does memory management inside the
 *          presented buffer and does not call malloc() and free().
 *          It sets the global pool_malloc() and polarssl_free() pointers
 *          to its own functions.
 *          (Provided pool_malloc() and polarssl_free() are thread-safe if
 *           POLARSSL_THREADING_C is defined)
 *
 * \note    This code is not optimized and provides a straight-forward
 *          implementation of a stack-based memory allocator.
 *
 * \param buf   buffer to use as heap
 * \param len   size of the buffer
 *
 * \return              0 if successful
 */
int memory_buffer_alloc_init( unsigned char *buf, size_t len );

/**
 * \brief   Free the mutex for thread-safety and clear remaining memory
 */
void memory_buffer_alloc_free( void );

/**
 * \brief   Determine when the allocator should automatically verify the state
 *          of the entire chain of headers / meta-data.
 *          (Default: MEMORY_VERIFY_NONE)
 *
 * \param verify    One of MEMORY_VERIFY_NONE, MEMORY_VERIFY_ALLOC,
 *                  MEMORY_VERIFY_FREE or MEMORY_VERIFY_ALWAYS
 */
void memory_buffer_set_verify( int verify );
extern void * (*pool_malloc)(size_t);
extern void (*pool_free)(void *);
#if defined(POLARSSL_MEMORY_DEBUG)
/**
 * \brief   Print out the status of the allocated memory (primarily for use
 *          after a program should have de-allocated all memory)
 *          Prints out a list of 'still allocated' blocks and their stack
 *          trace if POLARSSL_MEMORY_BACKTRACE is defined.
 */
void memory_buffer_alloc_status( void );
#endif /* POLARSSL_MEMORY_DEBUG */

/**
 * \brief   Verifies that all headers in the memory buffer are correct
 *          and contain sane values. Helps debug buffer-overflow errors.
 *
 *          Prints out first failure if POLARSSL_MEMORY_DEBUG is defined.
 *          Prints out full header information if POLARSSL_MEMORY_DEBUG_HEADERS
 *          is defined. (Includes stack trace information for each block if
 *          POLARSSL_MEMORY_BACKTRACE is defined as well).
 *
 * \returns             0 if verified, 1 otherwise
 */
int memory_buffer_alloc_verify( void );
buffer_alloc_ctx *memory_buffer_heap_getaddr(void);
void mem_pool_set_malloc_free(void);
#ifdef __cplusplus
}
#endif

#endif /* nvrec_mempool.h */
