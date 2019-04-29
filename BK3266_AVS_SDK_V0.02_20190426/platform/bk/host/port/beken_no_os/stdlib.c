/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#include "includes.h"
#include <string.h>
#include <stdlib.h>

int32 os_memcmp(const void *s1, const void *s2, uint32 n)
{
    return memcmp(s1, s2, (unsigned int)n);
}

void *os_memcpy(void *out, const void *in, size_t n)
{
    return memcpy(out, in, n);
}

void *os_memset(void *b, int c, size_t len)
{
    return (void *)memset(b, c, len);
}

uint32 os_strlen(const char *str)
{
    return strlen(str);
}

int32 os_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

int32 os_strncmp(const char *s1, const char *s2, const uint32 n)
{
    return strncmp(s1, s2, (unsigned int)n);
}

int32 os_snprintf(char *buf, uint32 size, const char *fmt, ...)
{
    va_list args;
    int32 rc;

    va_start(args, fmt);
    rc = vsnprintf(buf, size, fmt, args);
    va_end(args);

    /* if want to print more than the limitation */
    if (rc > size)
        rc = (int32)size - rc;

    return rc;
}

int32 os_vsnprintf(char *buf, uint32 size, const char *fmt, va_list ap)
{
    return vsnprintf(buf, size, fmt, ap);
}

