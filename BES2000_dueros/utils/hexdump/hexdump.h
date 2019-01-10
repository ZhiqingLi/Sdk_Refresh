#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

void print_hex_dump(const char *prefix_str, int prefix_type,
                    unsigned int rowsize, unsigned int groupsize,
                    const void *buf, size_t len, bool ascii);

void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
                          const void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif

