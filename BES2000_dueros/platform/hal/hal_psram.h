#ifndef PSRAM_HAL_H
#define PSRAM_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_location.h"
#include "plat_types.h"
#include "reg_psramip.h"

enum HAL_PSRAM_ID_T {
    HAL_PSRAM_ID_0 = 0,
    HAL_PSRAM_ID_NUM,
};

enum HAL_PSRAM_SPEED {
    HAL_PSRAM_SPEED_13M = 13000000,
    HAL_PSRAM_SPEED_26M = 26000000,
    HAL_PSRAM_SPEED_50M = 50000000,
    HAL_PSRAM_SPEED_52M = 52000000,
    HAL_PSRAM_SPEED_78M = 78000000,
    HAL_PSRAM_SPEED_104M = 104000000,
    HAL_PSRAM_SPEED_130M = 130000000,
    HAL_PSRAM_SPEED_156M = 156000000,
    HAL_PSRAM_SPEED_182M = 182000000,
    HAL_PSRAM_SPEED_208M = 208000000,
    HAL_PSRAM_SPEED_234M = 234000000,

    HAL_PSRAM_SPEED_NUM = 10,
};

struct HAL_PSRAM_CONFIG_T {
    uint32_t source_clk;
    uint32_t speed;
    uint32_t dqs_rd_sel;
    uint32_t dqs_wr_sel;

    /* internal use : can be config if need to (useful for rom) */
    uint8_t override_config:1;
    uint8_t div;
};

#define PSRAM_DEVICE_ID_LEN_MAX 8
struct HAL_Psram_Context {
    uint8_t device_id[PSRAM_DEVICE_ID_LEN_MAX];
    uint32_t total_size;
    uint32_t read_size;
    uint32_t write_size;
};

/* hal api */
#if 0
uint8_t hal_psram_open(enum HAL_PSRAM_ID_T id, struct HAL_PSRAM_CONFIG_T *cfg);
uint8_t hal_psram_close(enum HAL_PSRAM_ID_T id);
void hal_psram_reg_dump(enum HAL_PSRAM_ID_T id);
#endif
SRAM_TEXT_LOC void hal_psram_suspend(enum HAL_PSRAM_ID_T id);
SRAM_TEXT_LOC void hal_psram_resume(enum HAL_PSRAM_ID_T id);
SRAM_TEXT_LOC void hal_psram_sleep(void);
SRAM_TEXT_LOC void hal_psram_wakeup(void);
SRAM_TEXT_LOC void hal_psram_init(void);
SRAM_TEXT_LOC void hal_psram_wakeup_init(void);
SRAM_TEXT_LOC void hal_psram_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif /* PSRAM_HAL_H */
