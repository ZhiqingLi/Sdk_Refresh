#ifndef AE_COMMON_H
#define AE_COMMON_H

#include <stdio.h>

#ifndef VQE_SIMULATE
#include "hal_trace.h"
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

/*
 * You can modify this (for example with "#define LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

/*
 * This is the local tag used for the following simplified
 * logging macros.  You can change this preprocessor definition
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#ifdef VQE_SIMULATE
void __ae_log_print(const char *tag, const char *fmt, ...);
#else
#define __ae_log_print(tag, fmt, ...) hal_trace_printf_with_tag(tag, fmt, ##__VA_ARGS__)
#endif

#if LOG_NDEBUG
#define AE_TRACE(...) ((void)0)
#else
#define AE_TRACE(fmt, ...) __ae_log_print(LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#define AE_ASSERT(cond, fmt, ...) \
do { \
    if (!(cond)) { \
      __ae_log_print(LOG_TAG, fmt, ##__VA_ARGS__); \
        while(1); \
    } \
} while(0)

#endif
