#ifndef DRV_NORFLASH_H
#define DRV_NORFLASH_H

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DRV_NORFLASH_ERASE_TYPE {
    DRV_NORFLASH_ERASE_SECTOR = 0,
    DRV_NORFLASH_ERASE_BLOCK,
    DRV_NORFLASH_ERASE_CHIP,
};

enum DRV_NORFLASH_OP_MODE {
    /* (1) (2) (3) (4) can be used together, different flash-device may support different option(s) */

    /* (1) basic mode */
    DRV_NORFLASH_OP_MODE_STAND_SPI         = 0x1, /* standard spi mode */
    DRV_NORFLASH_OP_MODE_DUAL              = 0x2, /* dual mode */
    DRV_NORFLASH_OP_MODE_QUAD              = 0x4, /* quad mode */
    DRV_NORFLASH_OP_MODE_QPI               = 0x8, /* qpi mode */

    /* (2) read accelerate (no cmd bettween read operation) : may need Dual or Quad Mode */
    DRV_NORFLASH_OP_MODE_CONTINUOUS_READ   = 0x10,

    /* (3) read accelerate : fast clock */
    DRV_NORFLASH_OP_MODE_FAST              = 0x20,

    /* (4) output cmd and address accelerate (multi io pin) */
    DRV_NORFLASH_OP_MODE_OUTPUT            = 0x40,

    /* (5) high performance mode */
    DRV_NORFLASH_OP_MODE_HIGH_PERFORMANCE  = 0x80,

    DRV_NORFLASH_OP_MODE_RESERVED          = 0xFFFFFFFF,
};

struct HAL_NORFLASH_CONFIG_T;

uint8_t norflash_pre_operation(void);

uint8_t norflash_post_operation(void);

uint8_t norflash_read_reg(uint8_t cmd, uint8_t *val, uint32_t len);

uint8_t norflash_read_reg_ex(uint8_t cmd, uint8_t *param, uint32_t param_len, uint8_t *val, uint32_t len); // add by cai.zhong 20190105 for unique id

uint8_t norflash_write_reg(uint8_t cmd, const uint8_t *val, uint32_t len);

uint8_t norflash_set_mode(uint32_t mode, uint32_t op);

void norflash_set_sample_delay(uint32_t index);

uint8_t norflash_sample_delay_calib(uint32_t *samp_cnt, uint32_t *samp_idx);

uint8_t norflash_match_chip(const struct HAL_NORFLASH_CONFIG_T *cfg);

uint8_t norflash_get_size(uint32_t *total_size, uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);

uint8_t norflash_get_id(uint8_t *value, uint32_t len);

uint8_t norflash_get_unique_id(uint8_t *value, uint32_t len); // add by cai.zhong 20190105 for unique id

uint8_t norflash_erase(uint32_t start_address, enum DRV_NORFLASH_ERASE_TYPE type);

uint8_t norflash_write(uint32_t start_address, const uint8_t *buffer, uint32_t len);

uint8_t norflash_read(uint32_t start_address, uint8_t *buffer, uint32_t len);

void norflash_sleep(void);

void norflash_wakeup(void);

typedef void (* NORFLASH_STATUS_WEL_1_WAIT_CB)(void);
typedef void (* NORFLASH_STATUS_WIP_1_WAIT_CB)(void);
typedef void (* NORFLASH_STATUS_QE_SET_CB)(int enable);

struct NORFLASH_CFG_T {
    const uint8_t id[3];
    const bool hpm_cap;
    const bool erase_in_std_mode;
    const uint8_t crm_en_bits;
    const uint8_t crm_dis_bits;
    const uint16_t page_size;
    const uint16_t sector_size;
    const uint32_t block_size;
    const uint32_t total_size;
    const uint32_t max_speed;
    NORFLASH_STATUS_WEL_1_WAIT_CB norflash_status_WEL_1_wait;
    NORFLASH_STATUS_WIP_1_WAIT_CB norflash_status_WIP_1_wait;
    NORFLASH_STATUS_QE_SET_CB norflash_status_QE_set;
};

#ifdef __cplusplus
}
#endif

#endif /* DRV_NORFLASH_H */
