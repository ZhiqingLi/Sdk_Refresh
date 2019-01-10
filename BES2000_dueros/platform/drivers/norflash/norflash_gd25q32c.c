#include "plat_types.h"
#include "norflash_drv.h"
#include "hal_norflaship.h"
#include "norflash_gd25q32c.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "task_schedule.h"

static uint32_t gd25q32c_read_status_s8_s15(void)
{
    uint8_t val;
    norflash_read_reg(GD25Q32C_CMD_READ_STATUS_S8_S15, &val, 1);
    return val;
}

static uint32_t gd25q32c_read_status_s0_s7(void)
{
    uint8_t val;
    norflash_read_reg(GD25Q32C_CMD_READ_STATUS_S0_S7, &val, 1);
    return val;
}

static void gd25q32c_status_WEL_1_wait(void)
{
    uint32_t status = 0;
    do {
        status = gd25q32c_read_status_s0_s7();
    } while (!(status&GD25Q32C_WEL_BIT_MASK) && TASK_SCHEDULE);
}

static void gd25q32c_status_WIP_1_wait(void)
{
    uint32_t status = 0;
    do {
        status = gd25q32c_read_status_s0_s7();
    } while ((status&GD25Q32C_WIP_BIT_MASK) && TASK_SCHEDULE);
}

static void gd25q32c_write_status_s8_s15(uint8_t status)
{
    norflash_write_reg(GD25Q32C_CMD_WRITE_STATUS_S8_S15, &status, 1);
}

static void POSSIBLY_UNUSED gd25q32c_write_status_s0_s7(uint8_t status)
{
    norflash_write_reg(GD25Q32C_CMD_WRITE_STATUS_S0_S7, &status, 1);
}

static void gd25q32c_status_QE_set(int enable)
{
    uint8_t status_s8_s15 = 0;

    status_s8_s15 = gd25q32c_read_status_s8_s15();

    if (enable)
        status_s8_s15 |= GD25Q32C_QE_BIT_MASK;
    else
        status_s8_s15 &= ~(GD25Q32C_QE_BIT_MASK);

    gd25q32c_write_status_s8_s15(status_s8_s15);
}

const struct NORFLASH_CFG_T gd25q32c_cfg = {
    .id = { 0xC8, 0x40, 0x16, },
    .hpm_cap = true,
    .erase_in_std_mode = false,
    .crm_en_bits = (1 << 5) | (0 << 4),
    .crm_dis_bits = 0,
    .page_size = GD25Q32C_PAGE_SIZE,
    .sector_size = GD25Q32C_SECTOR_SIZE,
    .block_size = GD25Q32C_BLOCK_SIZE,
    .total_size = GD25Q32C_TOTAL_SIZE,
    .max_speed = 0,
    .norflash_status_WEL_1_wait = gd25q32c_status_WEL_1_wait,
    .norflash_status_WIP_1_wait = gd25q32c_status_WIP_1_wait,
    .norflash_status_QE_set = gd25q32c_status_QE_set,
};

