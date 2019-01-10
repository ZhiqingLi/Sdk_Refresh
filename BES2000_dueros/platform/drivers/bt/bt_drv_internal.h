#ifndef __BT_DRV_INTERNAL_H__
#define __BT_DRV_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

uint8_t btdrv_rf_init(void);
void btdrv_test_mode_rf_txpwr_init(void);
void btdrv_ins_patch_init(void);
void btdrv_data_patch_init(void);
void btdrv_patch_en(uint8_t en);
void btdrv_config_init(void);
void btdrv_testmode_config_init(void);
void btdrv_bt_spi_rawbuf_init(void);
void btdrv_bt_spi_xtal_init(void);
void btdrv_sync_config(void);

#ifdef BT_50_FUNCTION
void btdrv_config_init_ble5(void);
void btdrv_ins_patch_init_50(void);

#endif
#ifdef __cplusplus
}
#endif

#endif
