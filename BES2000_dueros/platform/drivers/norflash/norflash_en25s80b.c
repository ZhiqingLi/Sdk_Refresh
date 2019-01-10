#include "plat_types.h"
#include "norflash_drv.h"
#include "hal_norflaship.h"
#include "norflash_en25s80b.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "task_schedule.h"

static uint32_t POSSIBLY_UNUSED en25s80b_read_status_s8_s15(void)
{
    uint8_t val;
    norflash_read_reg(EN25S80B_CMD_READ_STATUS_S8_S15, &val, 1);
    return val;
}

static uint32_t en25s80b_read_status_s0_s7(void)
{
    uint8_t val;
    norflash_read_reg(EN25S80B_CMD_READ_STATUS_S0_S7, &val, 1);
    return val;
}

static void en25s80b_status_WEL_1_wait(void)
{
    uint32_t status = 0;
    do {
        status = en25s80b_read_status_s0_s7();
    } while (!(status&EN25S80B_WEL_BIT_MASK) && TASK_SCHEDULE);
}

static void en25s80b_status_WIP_1_wait(void)
{
    uint32_t status = 0;
    do {
        status = en25s80b_read_status_s0_s7();
    } while ((status&EN25S80B_WIP_BIT_MASK) && TASK_SCHEDULE);
}

static void POSSIBLY_UNUSED en25s80b_en25s80b_reset(void)
{
    // ip quad mode
    norflaship_quad_mode(1);
    norflaship_busy_wait();

    hal_sys_timer_delay(20);

    // quad reset enable
    norflaship_clear_txfifo();
    norflaship_cmd_addr(EN25S80B_CMD_QUAD_RESET_ENABLE, 0);
    norflaship_busy_wait();

    hal_sys_timer_delay(20);

    // quad reset
    norflaship_clear_txfifo();
    norflaship_cmd_addr(EN25S80B_CMD_QUAD_RESET, 0);
    norflaship_busy_wait();

    hal_sys_timer_delay(20);

    // ip spi mode
    norflaship_quad_mode(0);
    norflaship_hold_pin(0);
    norflaship_wpr_pin(0);
    norflaship_busy_wait();

    hal_sys_timer_delay(20);

    // reset enable
    norflaship_clear_txfifo();
    norflaship_cmd_addr(EN25S80B_CMD_SPI_RESET_ENABLE, 0);
    norflaship_busy_wait();

    hal_sys_timer_delay(20);

    // reset
    norflaship_clear_txfifo();
    norflaship_cmd_addr(EN25S80B_CMD_SPI_RESET, 0);
    norflaship_busy_wait();

    hal_sys_timer_delay(20);
}

static void en25s80b_enter_OTP(void)
{
    norflaship_clear_txfifo();
    norflaship_cmd_addr(EN25S80B_CMD_ENTER_OTP, 0);
    en25s80b_status_WIP_1_wait();
}

static void en25s80b_exit_OTP(void)
{
    norflaship_clear_txfifo();
    norflaship_cmd_addr(EN25S80B_CMD_EXIT_OTP, 0);
    en25s80b_status_WIP_1_wait();
}

static void en25s80b_write_status(uint8_t status)
{
    norflash_write_reg(EN25S80B_CMD_WRITE_STATUS, &status, 1);
}

static void en25s80b_status_QE_set(int enable)
{
    uint8_t status;

    en25s80b_enter_OTP();
    status = en25s80b_read_status_s0_s7();
    if (enable) {
        status |= (EN25S80B_WHDIS_BIT_MASK);
    } else {
        status &= ~(EN25S80B_WHDIS_BIT_MASK);
    }
    en25s80b_write_status(status);
    en25s80b_exit_OTP();
}

const struct NORFLASH_CFG_T en25s80b_cfg = {
    .id = { 0x1C, 0x38, 0x14, },
    .hpm_cap = false,
    .erase_in_std_mode = true,
    .crm_en_bits = 0xA5,
    .crm_dis_bits = 0xAA,
    .page_size = EN25S80B_PAGE_SIZE,
    .sector_size = EN25S80B_SECTOR_SIZE,
    .block_size = EN25S80B_BLOCK_SIZE,
    .total_size = EN25S80B_TOTAL_SIZE,
    .max_speed = 0,
    .norflash_status_WEL_1_wait = en25s80b_status_WEL_1_wait,
    .norflash_status_WIP_1_wait = en25s80b_status_WIP_1_wait,
    .norflash_status_QE_set = en25s80b_status_QE_set,
};

