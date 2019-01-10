#include "plat_types.h"
#include "norflash_drv.h"
#include "hal_norflaship.h"
#include "hal_norflash.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "norflash_gd25q32c.h"

#define NORFLASH_UNIQUE_ID_LEN              18 // add by cai.zhong 20190105 for unique id

extern const struct NORFLASH_CFG_T en25s80b_cfg;
extern const struct NORFLASH_CFG_T gd25lq32c_cfg;
extern const struct NORFLASH_CFG_T gd25lq16c_cfg;
extern const struct NORFLASH_CFG_T gd25q32c_cfg;
extern const struct NORFLASH_CFG_T gd25q80c_cfg;
extern const struct NORFLASH_CFG_T p25q80h_cfg;
extern const struct NORFLASH_CFG_T p25q32l_cfg;

static const struct NORFLASH_CFG_T * const flash_list[] = {
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_GD25Q32C__)
    &gd25q32c_cfg,
#endif
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_GD25Q80C__)
    &gd25q80c_cfg,
#endif
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_GD25LQ32C__) || defined(__NORFLASH_GD25LQ32D__)
    &gd25lq32c_cfg,
#endif
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_GD25LQ6C__)
    &gd25lq16c_cfg,
#endif
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_EN25S80B__)
    &en25s80b_cfg,
#endif
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_P25Q80H__)
    &p25q80h_cfg,
#endif
#if defined(__NORFLASH_ALL__) || defined(__NORFLASH_P25Q32L__)
    &p25q32l_cfg,
#endif
};

// Sample Delay will be larger if:
// 1) flash speed is higher (major impact)
// 2) vcore voltage is lower (secondary major impact)
// 3) flash voltage is lower (minor impact)

#if (CHIP_FLASH_CTRL_VER == 1)
#define SAM_EDGE_FALLING                (1 << 4)
#define SAM_NEG_PHASE                   (1 << 5)
#define SAMDLY_MASK                     (0xF << 0)

// Sample delays: 0, 0.5, 1, 1.5, 2, 3, 4, 5, 6, 7
static const uint8_t samdly_list[] = { /*0,*/ SAM_EDGE_FALLING | 1, 1, SAM_NEG_PHASE | SAM_EDGE_FALLING | 2, 2, 3, 4, /*5, 6, 7,*/ };
#else
static const uint8_t samdly_list[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, };
#endif

static uint8_t flash_idx;
static uint32_t norflash_op_mode = 0;

static uint8_t norflash_set_hpm(uint8_t on)
{
    if (!flash_list[flash_idx]->hpm_cap) {
        return 0;
    }

    if (on) {
        norflaship_cmd_addr(GD25Q32C_CMD_HIGH_PERFORMANCE, 0);
    } else {
        norflaship_cmd_addr(GD25Q32C_CMD_RELEASE_FROM_DP, 0);
    }
    norflaship_busy_wait();

    return 0;
}

static uint8_t norflash_set_continuous_read(uint8_t on)
{
    uint8_t cmd;

    if (on) {
        norflaship_continuous_read_on();

        norflaship_continuous_read_mode_bit(flash_list[flash_idx]->crm_en_bits);
    }
    else {
        norflaship_continuous_read_off();

        norflaship_continuous_read_mode_bit(flash_list[flash_idx]->crm_dis_bits);

        /* Disable Continuous Read Mode */
        if (norflash_op_mode & DRV_NORFLASH_OP_MODE_QUAD) {
            cmd = GD25Q32C_CMD_FAST_QUAD_READ;
        } else if (norflash_op_mode & DRV_NORFLASH_OP_MODE_DUAL) {
            cmd = GD25Q32C_CMD_FAST_DUAL_READ;
        } else {
            cmd = GD25Q32C_CMD_STANDARD_READ;
        }
        norflaship_cmd_addr(cmd, 0xFFFFFC);
    }
    norflaship_busy_wait();

    return 0;
}

static uint8_t norflash_set_quadmode(uint8_t on)
{
    if (on) {
        norflaship_cmd_addr(GD25Q32C_CMD_WRITE_ENABLE, 0);
        flash_list[flash_idx]->norflash_status_WIP_1_wait();
        flash_list[flash_idx]->norflash_status_WEL_1_wait();
        flash_list[flash_idx]->norflash_status_QE_set(1);
        flash_list[flash_idx]->norflash_status_WIP_1_wait();

        norflaship_quad_mode(1);
    }
    else {
        norflaship_cmd_addr(GD25Q32C_CMD_WRITE_ENABLE, 0);
        flash_list[flash_idx]->norflash_status_WIP_1_wait();
        flash_list[flash_idx]->norflash_status_WEL_1_wait();
        flash_list[flash_idx]->norflash_status_QE_set(0);
        flash_list[flash_idx]->norflash_status_WIP_1_wait();

        norflaship_quad_mode(0);
    }
    norflaship_busy_wait();

    return 0;
}

uint8_t norflash_get_size(uint32_t *total_size, uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size)
{
    if (total_size) {
        *total_size  = flash_list[flash_idx]->total_size;
    }
    if (block_size) {
        *block_size  = flash_list[flash_idx]->block_size;
    }
    if (sector_size) {
        *sector_size = flash_list[flash_idx]->sector_size;
    }
    if (page_size) {
        *page_size   = flash_list[flash_idx]->page_size;
    }

    return 0;
}

uint8_t norflash_set_mode(uint32_t mode, uint32_t op)
{
    norflash_op_mode = mode;

    if (mode & DRV_NORFLASH_OP_MODE_HIGH_PERFORMANCE) {
        norflash_set_hpm(1);
    }

    if (op & DRV_NORFLASH_OP_MODE_CONTINUOUS_READ) {
        if (norflash_op_mode & (DRV_NORFLASH_OP_MODE_QUAD | DRV_NORFLASH_OP_MODE_DUAL)) {
            if ((mode & DRV_NORFLASH_OP_MODE_CONTINUOUS_READ) == 0) {
                norflash_set_continuous_read(0);
            }
        }
    }

    /* Dual Mode */
    if (op & DRV_NORFLASH_OP_MODE_DUAL) {
        if (mode & DRV_NORFLASH_OP_MODE_DUAL) {
            //norflash_set_dualmode(1);
        } else {
            //norflash_set_dualmode(0);
        }
    }

    /* Quad Mode */
    if (op & DRV_NORFLASH_OP_MODE_QUAD) {
        if (mode & DRV_NORFLASH_OP_MODE_QUAD) {
            norflash_set_quadmode(1);
        } else {
            norflash_set_quadmode(0);
        }
    }

    if (op & DRV_NORFLASH_OP_MODE_CONTINUOUS_READ) {
        if (norflash_op_mode & (DRV_NORFLASH_OP_MODE_QUAD | DRV_NORFLASH_OP_MODE_DUAL)) {
            if (mode & DRV_NORFLASH_OP_MODE_CONTINUOUS_READ) {
                norflash_set_continuous_read(1);
            }
        }
    }

    if ((mode & DRV_NORFLASH_OP_MODE_HIGH_PERFORMANCE) == 0) {
        norflash_set_hpm(0);
    }

    norflaship_cmd_done();

    return 0;
}

uint8_t norflash_pre_operation(void)
{
    if (norflash_op_mode & DRV_NORFLASH_OP_MODE_CONTINUOUS_READ) {
        norflash_set_continuous_read(0);
    }

    return 0;
}

uint8_t norflash_post_operation(void)
{
    if (norflash_op_mode & DRV_NORFLASH_OP_MODE_CONTINUOUS_READ) {
        norflash_set_continuous_read(1);
    }

    norflaship_cmd_done();

    return 0;
}

uint8_t norflash_read_reg(uint8_t cmd, uint8_t *val, uint32_t len)
{
    int i;

    norflaship_clear_fifos();
#ifdef TRY_EMBEDDED_CMD
    if ((cmd == GD25Q32C_CMD_READ_STATUS_S0_S7 || cmd == GD25Q32C_CMD_READ_STATUS_S8_S15) && len == 1) {
        norflaship_cmd_addr(cmd, 0);
    } else {
        if (cmd == GD25Q32C_CMD_ID) {
            norflaship_blksize(len);
            norflaship_cmd_addr(cmd, 0);
        } else {
            norflaship_ext_rx_cmd(cmd, 0, len);
        }
    }
#else
    norflaship_ext_rx_cmd(cmd, 0, len);
#endif
    norflaship_rxfifo_count_wait(len);
    for (i = 0; i < len; i++) {
        val[i] = norflaship_read_rxfifo();
    }

    return 0;
}

// add begin by cai.zhong 20190105 for unique id
uint8_t norflash_read_reg_ex(uint8_t cmd, uint8_t *param, uint32_t param_len, uint8_t *val, uint32_t len)
{
    int i;

    norflaship_clear_fifos();
    if (param && param_len > 0) {
        norflaship_write_txfifo(param, param_len);
    } else {
        param_len = 0;
    }
    norflaship_ext_rx_cmd(cmd, param_len, len);
    for (i = 0; i < len; i++) {
        norflaship_rxfifo_empty_wait();
        val[i] = norflaship_read_rxfifo();
    }

    return 0;
}
// add end by cai.zhong 20190105 for unique id

uint8_t norflash_write_reg(uint8_t cmd, const uint8_t *val, uint32_t len)
{
    norflaship_clear_txfifo();
    norflaship_write_txfifo(val, len);
#ifdef TRY_EMBEDDED_CMD
    if (cmd == GD25Q32C_CMD_WRITE_STATUS_S0_S7) {
        norflaship_cmd_addr(GD25Q32C_CMD_WRITE_STATUS_S0_S7, 0);
    } else {
        norflaship_ext_tx_cmd(cmd, len);
    }
#else
    norflaship_ext_tx_cmd(cmd, len);
#endif
    flash_list[flash_idx]->norflash_status_WIP_1_wait();

    return 0;
}

uint8_t norflash_get_id(uint8_t *value, uint32_t len)
{
    norflash_pre_operation();

    if (len > 3) {
        len = 3;
    }
    norflash_read_reg(GD25Q32C_CMD_ID, value, len);

    norflash_post_operation();

    return 0;
}

// add begin by cai.zhong 20190105 for unique id
uint8_t norflash_get_unique_id(uint8_t *value, uint32_t len)
{
    uint32_t param;

    norflash_pre_operation();

    if (len > NORFLASH_UNIQUE_ID_LEN) {
        len = NORFLASH_UNIQUE_ID_LEN;
    }
    param = 0;
    norflash_read_reg_ex(GD25Q32C_CMD_UNIQUE_ID, (uint8_t *)&param, sizeof(param), value, len);

    norflash_post_operation();

    return 0;
}
// add end by cai.zhong 20190105 for unique id

void norflash_set_sample_delay(uint32_t index)
{
#if (CHIP_FLASH_CTRL_VER == 1)
    norflaship_pos_neg(samdly_list[index] & SAM_EDGE_FALLING);
    norflaship_neg_phase(samdly_list[index] & SAM_NEG_PHASE);
    norflaship_samdly(samdly_list[index] & SAMDLY_MASK);
#else
    norflaship_samdly(samdly_list[index]);
#endif
}

uint8_t norflash_sample_delay_calib(uint32_t *samp_cnt, uint32_t *samp_idx)
{
    uint8_t id[3];
    const uint8_t *cmp_id;
    int i;
    uint32_t matched_cnt = 0;
    uint32_t matched_idx = 0;

    for (i = 0; i < ARRAY_SIZE(samdly_list); i++) {
        norflash_set_sample_delay(i);

        norflash_get_id(id, 3);

        cmp_id = flash_list[flash_idx]->id;
        if (id[0] == cmp_id[0] && id[1] == cmp_id[1] && id[2] == cmp_id[2]) {
            if (matched_cnt == 0) {
                matched_idx = i;
            }
            matched_cnt++;
        } else if (matched_cnt) {
            break;
        }
    }

    if (samp_cnt) {
        *samp_cnt = matched_cnt;
    }
    if (samp_idx) {
        *samp_idx = matched_idx;
    }

    if (matched_cnt) {
        // Assuming vcore voltage will be decreased later, so select a medium-larger sample delay.
        // This behaviour can be overridden by calling norflash_set_sample_delay() again.
        matched_idx += matched_cnt / 2;
        norflash_set_sample_delay(matched_idx);

        return true;
    }

    return false;
}

uint8_t norflash_match_chip(const struct HAL_NORFLASH_CONFIG_T *cfg)
{
    bool div_changed = false;
    uint32_t old_div;

    old_div = norflaship_get_div();

    for (flash_idx = 0; flash_idx < ARRAY_SIZE(flash_list); flash_idx++) {
        if (flash_list[flash_idx]->max_speed && flash_list[flash_idx]->max_speed < cfg->speed) {
            div_changed = true;
            norflaship_div((cfg->source_clk + flash_list[flash_idx]->max_speed - 1) / flash_list[flash_idx]->max_speed);
        }

        if (norflash_sample_delay_calib(NULL, NULL)) {
            return 1;
        }

        if (div_changed) {
            div_changed = false;
            norflaship_div(old_div);
        }
    }

    flash_idx = 0;

    return 0;
}

uint8_t norflash_erase(uint32_t start_address, enum DRV_NORFLASH_ERASE_TYPE type)
{
    if (flash_list[flash_idx]->erase_in_std_mode
            && (norflash_op_mode & DRV_NORFLASH_OP_MODE_QUAD)) {
        norflash_set_quadmode(0);
    }

    norflaship_cmd_addr(GD25Q32C_CMD_WRITE_ENABLE, start_address);
    // Need 1us. Or flash_list[flash_idx]->norflash_status_WEL_1_wait(), which needs 6us.
    switch(type) {
        case DRV_NORFLASH_ERASE_SECTOR:
            norflaship_cmd_addr(GD25Q32C_CMD_SECTOR_ERASE, start_address);
            break;
        case DRV_NORFLASH_ERASE_BLOCK:
            norflaship_cmd_addr(GD25Q32C_CMD_BLOCK_ERASE, start_address);
            break;
        case DRV_NORFLASH_ERASE_CHIP:
            norflaship_cmd_addr(GD25Q32C_CMD_CHIP_ERASE, start_address);
            break;
    }
    flash_list[flash_idx]->norflash_status_WIP_1_wait();

    if (flash_list[flash_idx]->erase_in_std_mode
            && (norflash_op_mode & DRV_NORFLASH_OP_MODE_QUAD)) {
        norflash_set_quadmode(1);
    }

    norflaship_cmd_done();

    return 0;
}

uint8_t norflash_write(uint32_t start_address, const uint8_t *buffer, uint32_t len)
{
    uint32_t POSSIBLY_UNUSED remains;

    norflaship_clear_txfifo();
#if (CHIP_FLASH_CTRL_VER > 1)
    norflaship_blksize(len);
#endif
    remains = norflaship_write_txfifo(buffer, len);
    norflaship_cmd_addr(GD25Q32C_CMD_WRITE_ENABLE, start_address);
    if (norflash_op_mode & DRV_NORFLASH_OP_MODE_QUAD) {
        norflaship_cmd_addr(GD25Q32C_CMD_QUAD_PAGE_PROGRAM, start_address);
    }
    /* not support Dual Write */
    //else if (norflash_op_mode & DRV_NORFLASH_OP_MODE_DUAL) {
    //}
    else {
        norflaship_cmd_addr(GD25Q32C_CMD_PAGE_PROGRAM, start_address);
    }
#if (CHIP_FLASH_CTRL_VER > 1)
    while (remains > 0) {
        buffer += len - remains;
        len = remains;
        remains = norflaship_write_txfifo(buffer, len);
    }
#endif
    norflaship_busy_wait();

    flash_list[flash_idx]->norflash_status_WIP_1_wait();

    norflaship_cmd_done();

    return 0;
}

uint8_t norflash_read(uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    uint32_t index = 0;

    norflaship_clear_rxfifo();
    norflaship_busy_wait();
    /* read NORFLASHIP_ONCE_READ_SIZE (always is page size) , but fifo is NORFLASHIP_RXFIFO_SIZE_MAX (8 bytes) now */
    /* so we need to read when fifo is not empty, finally check total count */
    /* BUG : when read speed < spi in speed, lost read bytes,
     * NORFLASHIP_ONCE_READ_SIZE is not ok always (only when clock is slow, not easy to handle), NORFLASHIP_RXFIFO_SIZE_MAX is OK always
     * */
    norflaship_blksize(len);
    if (norflash_op_mode & DRV_NORFLASH_OP_MODE_QUAD) {
        /* Quad , only fast */
        norflaship_cmd_addr(GD25Q32C_CMD_FAST_QUAD_READ, start_address);
    }
    else if (norflash_op_mode & DRV_NORFLASH_OP_MODE_DUAL) {
        /* Dual, only fast */
        norflaship_cmd_addr(GD25Q32C_CMD_FAST_DUAL_READ, start_address);
    }
    else {
        /* fast */
        if (norflash_op_mode & DRV_NORFLASH_OP_MODE_FAST) {
            norflaship_cmd_addr(GD25Q32C_CMD_STANDARD_FAST_READ, start_address);
        }
        /* normal */
        else {
            norflaship_cmd_addr(GD25Q32C_CMD_STANDARD_READ, start_address);
        }
    }
    while(1) {
        norflaship_rxfifo_empty_wait();
        buffer[index] = norflaship_read_rxfifo();
        ++index;

        if(index >= len) {
            break;
        }
    }

    norflaship_cmd_done();

    return 0;
}

void norflash_sleep(void)
{
    norflash_pre_operation();
    if (norflash_op_mode & DRV_NORFLASH_OP_MODE_HIGH_PERFORMANCE) {
        norflash_set_hpm(0);
    }
    norflaship_cmd_addr(GD25Q32C_CMD_DEEP_POWER_DOWN, 0);
}

void norflash_wakeup(void)
{
    norflaship_cmd_addr(GD25Q32C_CMD_RELEASE_FROM_DP, 0);
    // Wait 20us for flash to finish
    hal_sys_timer_delay(2);
    if (norflash_op_mode & DRV_NORFLASH_OP_MODE_HIGH_PERFORMANCE) {
        norflash_set_hpm(1);
    }
    norflash_post_operation();
}

