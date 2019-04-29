/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _STR_UTILS_H_
#define _STR_UTILS_H_

uint32_t j_snscanf(char *buf, char **end_ptr, char *format, ...);
uint32_t j_vsnscanf(char *buf, char **end_ptr, char *format, va_list args);

#endif

