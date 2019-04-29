/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _OS_COMMON_H_
#define _OS_COMMON_H_

/*
 * Functions and prototypes common for the USBware stack and the porting layer
 */

#ifdef CONFIG_STDARG_H
#include CONFIG_STDARG_H
#else
#include <stdarg.h>
#endif

#include <include/uw_compiler.h>
#include <include/uw_types.h>
#include <include/uw_debug.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define JOS_MALLOC
uint32_t memory_usage_show(void);
#ifdef JMALLOC_STATISTICAL
void *jmalloc_cm(const char *call_func_name, uint32_t size, uint16_t flags);
void jfree_cm(const char *call_func_name, void *item);
#define jmalloc(size, flags) jmalloc_cm(__func__, size, flags)
#define jfree(addr)          jfree_cm(__func__, (void *)addr)
#else
void *jmalloc_s(uint32_t size, uint16_t flags);
void jfree_s(void *item);
#define jmalloc(size, flags) jmalloc_s(size, flags)
#define jfree(addr)          jfree_s((void *)addr)
#endif

#define LOCK_CHECK             do { } while (0)
#define LOCK_CHECK_D(file, line) do { } while (0)
#define jsafe_enter()
#define jsafe_leave()

#ifdef CONFIG_JOS_BUS
#define jcritical_enter       jos_critical_enter
#define jcritical_leave       jos_critical_leave
#define jcritical_check       jos_critical_check
void jos_critical_enter(void);
void jos_critical_leave(void);
bool_t jos_critical_check(void);
#else
/* Critical sections synchronize threads and ISR, therefore are not implemented
 * when there is no ISR (CONFIG_JOS_BUS is not defined) */
#define jcritical_enter()      do { } while (0)
#define jcritical_leave()      do { } while (0)
#define jcritical_check()      do { } while (0)
#endif

#ifdef CONFIG_JHOST_REMOTE_DEBUG
int32_t remote_debug_printf(const char *fmt, ...) CONFIG_DEBUG_ATTRIB_FIRST;
#endif

#ifndef CONFIG_STDARG_H
#ifdef CONFIG_COMMON_VSNPRINTF
int32_t j_vsnprintf(char *str, uint32_t n, const char *format,
    va_list ap);
#else
int32_t os_vsnprintf(char *str, uint32_t n, const char *format,
    va_list ap);
#define j_vsnprintf os_vsnprintf
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

