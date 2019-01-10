#ifndef NORFLASH_HAL_H
#define NORFLASH_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "norflash_drv.h"

#define HAL_NORFLASH_UNIQUE_ID_LEN          16  // add begin by cai.zhong 20190105 for unique id

enum HAL_NORFLASH_ID_T {
    HAL_NORFLASH_ID_0 = 0,
    HAL_NORFLASH_ID_NUM,
};

enum HAL_NORFLASH_SPEED {
    HAL_NORFLASH_SPEED_13M = 13000000,
    HAL_NORFLASH_SPEED_26M = 26000000,
    HAL_NORFLASH_SPEED_52M = 52000000,
    HAL_NORFLASH_SPEED_78M = 78000000,
    HAL_NORFLASH_SPEED_104M = 104000000,
    HAL_NORFLASH_SPEED_130M = 130000000,
    HAL_NORFLASH_SPEED_156M = 156000000,
    HAL_NORFLASH_SPEED_182M = 182000000,
    HAL_NORFLASH_SPEED_208M = 208000000,
    HAL_NORFLASH_SPEED_234M = 234000000,

    HAL_NORFLASH_SPEED_NUM = 10,
};

struct HAL_NORFLASH_CONFIG_T {
    uint32_t source_clk;
    uint32_t speed;
    enum DRV_NORFLASH_OP_MODE mode;

    /* internal use : can be config if need to (useful for rom) */
    uint8_t override_config:1;
    uint8_t neg_phase:1;
    uint8_t pos_neg:1;
    uint8_t cmdquad:1;
    uint8_t samdly:3;
    uint8_t div; /* least 2 */
    uint8_t dualmode:1;
    uint8_t holdpin:1;
    uint8_t wprpin:1;
    uint8_t quadmode:1;

    uint8_t spiruen:3;
    uint8_t spirden:3;

    uint8_t dualiocmd;
    uint8_t rdcmd;
    uint8_t frdcmd;
    uint8_t qrdcmd; /* quad io cmd */
};

/* hal api */
uint8_t hal_norflash_open(enum HAL_NORFLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg);
uint8_t hal_norflash_get_size(enum HAL_NORFLASH_ID_T id, uint32_t *total_size, uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);
uint8_t hal_norflash_get_boundary(enum HAL_NORFLASH_ID_T id, uint32_t address, uint32_t* block_boundary, uint32_t* sector_boundary);
uint8_t hal_norflash_get_id(enum HAL_NORFLASH_ID_T id, uint8_t *value, uint32_t len);
uint8_t hal_norflash_get_unique_id(enum HAL_NORFLASH_ID_T id, uint8_t *value, uint32_t len);  // add begin by cai.zhong 20190105 for unique id
uint8_t hal_norflash_erase(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint32_t len);
uint8_t hal_norflash_erase_chip(enum HAL_NORFLASH_ID_T id);
uint8_t hal_norflash_write(enum HAL_NORFLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len);
uint8_t hal_norflash_read(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
uint8_t hal_norflash_close(enum HAL_NORFLASH_ID_T id);
void    hal_norflash_sleep(enum HAL_NORFLASH_ID_T id);
void    hal_norflash_wakeup(enum HAL_NORFLASH_ID_T id);
uint32_t hal_norflash_get_flash_total_size(void);

#ifdef __cplusplus
}
#endif

#endif /* NORFLASH_HAL_H */
