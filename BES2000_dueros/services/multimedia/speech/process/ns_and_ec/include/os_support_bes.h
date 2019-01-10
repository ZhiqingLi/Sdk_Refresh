#ifndef OS_SUPPORT_BES_H
#define OS_SUPPORT_BES_H

#if defined(__GNUC__) && defined(__arm__)
#include "hal_trace.h"
#include "med_memory.h"

void *bes_med_malloc(size_t size);
void *bes_med_realloc(void *rmem, size_t newsize);
void *bes_med_calloc(size_t count, size_t size);
void bes_med_free(void *rmem);


#define OVERRIDE_SPEEX_ALLOC 1
static inline void *speex_alloc (int size)
{
    return bes_med_calloc(size,1);
}

#define OVERRIDE_SPEEX_ALLOC_SCRATCH 1
static inline void *speex_alloc_scratch (int size)
{
    return bes_med_calloc(size, 1);
}

#define OVERRIDE_SPEEX_REALLOC 1
static inline void *speex_realloc (void *ptr, int size)
{
    return bes_med_realloc(ptr, size);
}

#define OVERRIDE_SPEEX_FREE 1
static inline void speex_free (void *ptr)
{
    bes_med_free(ptr);
}

#define OVERRIDE_SPEEX_FREE_SCRATCH 1
static inline void speex_free_scratch (void *ptr)
{
    bes_med_free(ptr);
}

#define OVERRIDE_SPEEX_FATAL 1
static inline void _speex_fatal(const char *str, const char *file, int line)
{
    (void)str;
    (void)file;
    (void)line;
    TRACE("Fatal error: %s\n", str);
   //exit(1);
}

#define OVERRIDE_SPEEX_WARNING 1
static inline void speex_warning(const char *str)
{
    (void)str;
    TRACE("warning: %s\n", str);
}

#define OVERRIDE_SPEEX_WARNING_INT 1
static inline void speex_warning_int(const char *str, int val)
{
    (void)str;
    (void)val;
    TRACE("warning: %s %d\n", str, val);
}

#define OVERRIDE_SPEEX_NOTIFY 1
static inline void speex_notify(const char *str)
{
    (void)str;
    TRACE("notice: %s\n", str);
}

#define OVERRIDE_SPEEX_PUTC 1
static inline void _speex_putc(int ch, void *file)
{
    (void)ch;
    (void)file;
    //FILE *f = (FILE *)file;
    //printf("%c", ch);
}

#endif // defined(__GNUC__) && defined(__arm__)
#endif // OS_SUPPORT_BES_H