/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_GENERIC_H_
#define _PORT_GENERIC_H_

int32 os_printf(const char *fmt, ...);
int32 os_null_printf(const char *fmt, ...);

int32 os_memcmp(const void *s1, const void *s2, uint32 n);
int32 os_snprintf(char *buf, uint32_t size, const char *fmt, ...);
int32 os_strcmp(const char *s1, const char *s2);
int32 os_strncmp(const char *s1, const char *s2, const uint32 n);
char *os_strcat(char *dst, const char *src);
char *os_strncat(char *dst, const char *src, j_size_t n);
void *os_memset(void *b, int32 c, size_t len);
void *os_memcpy(void *out, const void *in, size_t n);
uint32 os_strlen(const char *str);

void os_exit(void);

result_t os_port_init(void);
void os_port_uninit(void);

#endif

