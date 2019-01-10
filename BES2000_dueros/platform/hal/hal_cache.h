#ifndef CACHE_HAL_H
#define CACHE_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum HAL_CACHE_ID_T {
    HAL_CACHE_ID_I_CACHE = 0,
    HAL_CACHE_ID_D_CACHE,
    HAL_CACHE_ID_NUM,
};

#define HAL_CACHE_YES 1
#define HAL_CACHE_NO 0

/* hal api */
uint8_t hal_cache_enable(enum HAL_CACHE_ID_T id, uint32_t val);
uint8_t hal_cache_writebuffer_enable(enum HAL_CACHE_ID_T id, uint32_t val);
uint8_t hal_cache_writebuffer_flush(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* CACHE_HAL_H */
