#ifndef __APP_SDMMC_H__
#define __APP_SDMMC_H__

#ifdef __cplusplus
extern "C" {
#endif

enum APP_SDMMC_DUMP_T{
    APP_SDMMC_DUMP_OPEN = 0,
    APP_SDMMC_DUMP_READ,
    APP_SDMMC_DUMP_WRITE,
    APP_SDMMC_DUMP_CLOSE,
    APP_SDMMC_DUMP_NUM
};

int sd_open();

void  dump_data2sd(enum APP_SDMMC_DUMP_T opt, uint8_t *buf, uint32_t len);

#ifdef __cplusplus
	}
#endif

#endif//__FMDEC_H__
