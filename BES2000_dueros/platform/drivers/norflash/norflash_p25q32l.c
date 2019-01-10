#include "plat_types.h"
#include "norflash_drv.h"
#include "hal_norflaship.h"
#include "norflash_p25q32l.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "task_schedule.h"

static void POSSIBLY_UNUSED p25q32l_reset(void)
{
    norflaship_clear_rxfifo();
    norflaship_cmd_addr(P25Q32L_CMD_RESET_EN, 0);
    norflaship_busy_wait();

    norflaship_clear_rxfifo();
    norflaship_cmd_addr(P25Q32L_CMD_RESET, 0);
    norflaship_busy_wait();

    hal_sys_timer_delay(MS_TO_TICKS(1));
}

static uint32_t p25q32l_read_status_s8_s15(void)
{
    uint8_t val;
    norflash_read_reg(P25Q32L_CMD_READ_STATUS_S8_S15, &val, 1);
    return val;
}

static uint32_t p25q32l_read_status_s0_s7(void)
{
    uint8_t val;
    norflash_read_reg(P25Q32L_CMD_READ_STATUS_S0_S7, &val, 1);
    return val;
}

static void p25q32l_status_WEL_1_wait(void)
{
    uint32_t status = 0;
    do {
        status = p25q32l_read_status_s0_s7();
    } while (!(status&P25Q32L_WEL_BIT_MASK) && TASK_SCHEDULE);
}

static void p25q32l_status_WIP_1_wait(void)
{
    uint32_t status = 0;
    do {
        status = p25q32l_read_status_s0_s7();
    } while ((status&P25Q32L_WIP_BIT_MASK) && TASK_SCHEDULE);
}

static void p25q32l_write_status_s8_s15(uint8_t status)
{
    norflash_write_reg(P25Q32L_CMD_WRITE_STATUS_S8_S15, &status, 1);
}

static void p25q32l_status_QE_set(int enable)
{
    uint8_t status_s8_s15 = 0;

    status_s8_s15 = p25q32l_read_status_s8_s15();

    if (enable)
        status_s8_s15 |= P25Q32L_QE_BIT_MASK;
    else
        status_s8_s15 &= ~(P25Q32L_QE_BIT_MASK);

    p25q32l_write_status_s8_s15(status_s8_s15);

    status_s8_s15 = p25q32l_read_status_s8_s15();
}

const struct NORFLASH_CFG_T p25q32l_cfg = {
    .id = { 0x85, 0x60, 0x16, },
    .hpm_cap = false,
    .erase_in_std_mode = false,
    .crm_en_bits = (1 << 5) | (0 << 4),
    .crm_dis_bits = 0,
    .page_size = P25Q32L_PAGE_SIZE,
    .sector_size = P25Q32L_SECTOR_SIZE,
    .block_size = P25Q32L_BLOCK_SIZE,
    .total_size = P25Q32L_TOTAL_SIZE,
    .max_speed = 90000000,
    .norflash_status_WEL_1_wait = p25q32l_status_WEL_1_wait,
    .norflash_status_WIP_1_wait = p25q32l_status_WIP_1_wait,
    .norflash_status_QE_set = p25q32l_status_QE_set,
};

