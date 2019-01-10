#ifndef __EXPORT_FN_ROM_CUST_H__
#define __EXPORT_FN_ROM_CUST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdarg.h"

typedef void (*__RESV000)(void);
typedef unsigned int (*__RESV001)(unsigned int v1, const unsigned char *p, unsigned int v2);
typedef int (*__RESV002)(const unsigned char *p1, const unsigned char *p2, const unsigned char *p3, unsigned int v);
typedef const unsigned char* (*__SHA256_HASH)(const void* data, int len, unsigned char* digest);
typedef void (*__SHA256_INIT)(void* ctx);
typedef void (*__SHA256_UPDATE)(void* ctx, const void* data, int len);
typedef const unsigned char* (*__SHA256_FINAL)(void* ctx);

struct EXPORT_FN_ROM_CUST_T {
    __RESV000 r000;
    __RESV001 r001;
    __RESV002 r002;
    __SHA256_HASH SHA256_hash;
    __SHA256_INIT SHA256_init;
    __SHA256_UPDATE SHA256_update;
    __SHA256_FINAL SHA256_final;
};

extern struct EXPORT_FN_ROM_CUST_T __export_fn_rom_cust;

#ifdef __cplusplus
}
#endif

#endif

