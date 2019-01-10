#ifndef __BES_HAL_SDMMC_H__
#define __BES_HAL_SDMMC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bttypes.h"
#include "ff.h"

int32_t bes_hal_sdmmc_init(void);
int32_t bes_hal_sdmmc_deinit(void);
int32_t bes_hal_sdmmc_open_file(FIL* fp, uint8_t* path, uint8_t readFlag);
int32_t bes_hal_sdmmc_close_file(FIL* fp);
int32_t bes_hal_sdmmc_write_file(FIL* fp, const void* buf, uint32_t size, uint32_t* plen);
int32_t bes_hal_sdmmc_read_file(FIL* fp, void* buf, uint32_t size, uint32_t* len);

#ifdef __cplusplus
}
#endif
#endif