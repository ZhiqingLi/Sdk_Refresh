#ifdef CHIP_HAS_PSRAM

#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_psram.h"
#include "hal_psramip.h"
#include "hal_trace.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "pmu.h"

#define PSRAM_HIGH_SPEED 1

#define HAL_PSRAM_YES 1
#define HAL_PSRAM_NO 0

#define HAL_PSRAM_CMD_REG_READ 0x40
#define HAL_PSRAM_CMD_REG_WRITE 0xc0
#define HAL_PSRAM_CMD_RAM_READ 0x00
#define HAL_PSRAM_CMD_RAM_WRITE 0x80

struct HAL_Psram_Context psram_ctx[HAL_PSRAM_ID_NUM];

//static const char * const invalid_drv = "psram drv invalid";

#define DIGITAL_REG(a) *(volatile uint32_t *)(a)

static uint32_t _psram_get_reg_base(enum HAL_PSRAM_ID_T id)
{
    switch(id) {
        case HAL_PSRAM_ID_0:
        default:
            return PSRAM_CTRL_BASE;
            break;
    }
}

static void _psram_exitsleep_onprocess_wait(enum HAL_PSRAM_ID_T id)
{
    uint32_t reg_base = 0;
    reg_base = _psram_get_reg_base(id);
    while (psramip_r_exit_sleep_onprocess(reg_base));

    while (!psramip_r_sleep_wakeup_state(reg_base));
}
static void _psram_busy_wait(enum HAL_PSRAM_ID_T id)
{
    uint32_t reg_base = 0;
    reg_base = _psram_get_reg_base(id);
    while (psramip_r_busy(reg_base));
}
#if 0
static void _psram_div(enum HAL_PSRAM_ID_T id)
{
    /* TODO */
}
/* hal api */
uint8_t hal_psram_open(enum HAL_PSRAM_ID_T id, struct HAL_PSRAM_CONFIG_T *cfg)
{
    uint32_t div = 0, reg_base = 0;
    //uint32_t psram_id = 0;
    reg_base = _psram_get_reg_base(id);

    /* over write config */
    if (cfg->override_config) {
        /* div */
        _psram_div(cfg->div);
    }
    else {
        div = cfg->source_clk/cfg->speed;
        _psram_div(div);
    }

    /* 0. dqs config */
    psramip_w_dqs_rd_sel(reg_base, cfg->dqs_rd_sel);
    psramip_w_dqs_wr_sel(reg_base, cfg->dqs_wr_sel);

    /* 1. high speed mode */
    if (cfg->speed >= HAL_PSRAM_SPEED_50M)
        psramip_w_high_speed_enable(reg_base, HAL_PSRAM_YES);
    else
        psramip_w_high_speed_enable(reg_base, HAL_PSRAM_NO);

    _psram_busy_wait(id);

    /* 2. wait calib done or FIXME timeout */
    psramip_w_enable_and_trigger_calib(reg_base);
    while (!psramip_r_calibst(reg_base));

    psramip_w_wrap_mode_enable(reg_base, HAL_PSRAM_YES);
    //psramip_w_wrap_mode_enable(reg_base, HAL_PSRAM_NO);

    //psramip_w_32bytewrap_mode(reg_base);
    psramip_w_1kwrap_mode(reg_base);
#if 0
    /* psram device register read 1 or 2 or 3 */
    psramip_w_acc_size(reg_base, 1);
    psramip_w_cmd_addr(reg_base, HAL_PSRAM_CMD_REG_READ, 2);
    _psram_busy_wait(id);
    psram_id = psramip_r_rx_fifo(reg_base);

    uart_printf("psram id 0x%x\n", psram_id);
#endif

    return 0;
}
uint8_t hal_psram_close(enum HAL_PSRAM_ID_T id)
{
    return 0;
}
void hal_psram_reg_dump(enum HAL_PSRAM_ID_T id)
{
    uint32_t reg_base = 0;
    uint32_t psram_id = 0;
    reg_base = _psram_get_reg_base(id);

    /* psram device register read 1 or 2 or 3 */
    psramip_w_acc_size(reg_base, 1);
    psramip_w_cmd_addr(reg_base, HAL_PSRAM_CMD_REG_READ, 2);
    _psram_busy_wait(id);
    psram_id = psramip_r_rx_fifo(reg_base);

    uart_printf("psram id 0x%x\n", psram_id);
}

#endif
SRAM_TEXT_LOC void hal_psram_suspend(enum HAL_PSRAM_ID_T id)
{
    uint32_t reg_base = 0;
    reg_base = _psram_get_reg_base(id);

    psramip_w_acc_size(reg_base, 1);
    psramip_w_tx_fifo(reg_base, 0xf0);
    psramip_w_cmd_addr(reg_base, HAL_PSRAM_CMD_REG_WRITE, 6);
    _psram_busy_wait(id);
}

SRAM_TEXT_LOC void hal_psram_resume(enum HAL_PSRAM_ID_T id)
{
    uint32_t reg_base = 0;
    reg_base = _psram_get_reg_base(id);

    psramip_w_exit_sleep(reg_base);

    _psram_exitsleep_onprocess_wait(id);
}
//uint32_t test_ctrl =0;


#ifdef CHIP_BEST2000
SRAM_TEXT_LOC void hal_psram_init(void)
{
#if PSRAM_HIGH_SPEED
    unsigned int mr = 0;
    enum HAL_PSRAM_ID_T id = HAL_PSRAM_ID_0;
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_208M);
    pmu_psram_freq_config(HAL_PSRAM_SPEED_208M);
#else
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);
    pmu_psram_freq_config(HAL_PSRAM_SPEED_26M);
#endif

    hal_cmu_clock_enable(HAL_CMU_MOD_O_PSRAM);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_PSRAM);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_PSRAM);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_PSRAM);
    DIGITAL_REG(0x4008004c) |= 0x1<<21;
 #if 0   //for debug
    DIGITAL_REG(0x40150020) = 0x01;
    DIGITAL_REG(0x400000ac) &= ~(0xc0000);

    DIGITAL_REG(0x400000ac) |= (0x80000);

    DIGITAL_REG(0x40087008) = 0xFFFFFFAF;
    DIGITAL_REG(0x4008701c) = 0x2;
#endif

#if PSRAM_HIGH_SPEED
    DIGITAL_REG(0x40150040) = 0x44712133;

    DIGITAL_REG(0x40150044) = 0x200040b1;
    DIGITAL_REG(0x40150044) |= (1<<1)|(1<<30);	

    while (((DIGITAL_REG(0x40150044)) & (0x1<<30)));
    hal_sys_timer_delay(MS_TO_TICKS(10));
    DIGITAL_REG(0x40150044) &= ~(1<<0);

    while (!((DIGITAL_REG(0x40150044)) & (0x1<<0)));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    DIGITAL_REG(0x40000090) = 0x0001ffff;
    while (!((DIGITAL_REG(0x40000090)) & (0x1<<20)));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    //lp delay
    DIGITAL_REG(0x4015001C)=0xffffffff;//c
    //ramp=1k select low speed
    DIGITAL_REG(0x4015002C)=0x0000000d;//c
    //tx/rx Phase
    DIGITAL_REG(0x40150024)=0x55000373;//223
   // DIGITAL_REG(0x40150024)=0x55000223;//223
    while (!((DIGITAL_REG(0x40150024)) & (0x1<<3)));
#if 1
    //reset psram chip
    psramip_write32(0xff, 0x40150000, 0x00);
    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));
    //clear fifo
    psramip_write32(0x03, 0x40150000, 0x18);

    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));
    hal_sys_timer_delay(MS_TO_TICKS(10));
   // test_ctrl = 1;
   // while (test_ctrl ==1);

    //read MR0
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_READ, 0);

    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));
   // {
      //  hal_sys_timer_delay(MS_TO_TICKS(100));
   // };
    //need read twice
    mr = psramip_r_rx_fifo(0x40150000);
    mr = psramip_r_rx_fifo(0x40150000);

    mr &= 0xff;
    mr &= ~(0x03 << 6); // MR[7:6] must set 0
    // driver strength 00:1/16 01:1/2 10:1/4 11:1/8 MR0[1:0]
    mr &= ~0x03;
    mr |= 0x01;
    //for >> 200M MR0[4:2]
    mr &= ~(0x07 << 2);
    mr |= (0x06 << 2);
    psramip_w_tx_fifo(0x40150000, mr);
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_WRITE, 0);
    _psram_busy_wait(id);

    //read MR0 again
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_READ, 0);

    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));	
    mr = psramip_r_rx_fifo(0x40150000);
    mr = psramip_r_rx_fifo(0x40150000);
    //  hal_uart_printf("count=0x%x mr0 = 0x%x \n", psramip_r_status(0x40150000), mr);	
    //read MR4
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_READ, 4);
    _psram_busy_wait(id);
    mr = psramip_r_rx_fifo(0x40150000);
    mr = psramip_r_rx_fifo(0x40150000);
    //  hal_uart_printf("mr4 = 0x%x\n", mr);

    _psram_busy_wait(id);
#endif
    hal_sys_timer_delay(MS_TO_TICKS(10));
#else
    // open psram phy power
    DIGITAL_REG(0x4008004c) |= (1 << 21);
    //init psram
    //lp delay
    DIGITAL_REG(0x4015001C)=0x3fff3cff;//c
    //ramp=1k select low speed
    DIGITAL_REG(0x4015002C)=0x0000000c;
    //tx/rx Phase
    DIGITAL_REG(0x40150024)=0x55000223;
#endif
}
SRAM_TEXT_LOC void hal_psram_wakeup_init(void)
{
#if PSRAM_HIGH_SPEED
     DIGITAL_REG(0x40150040) = 0x44712133;

    DIGITAL_REG(0x40150044) = 0x200040b1;
    DIGITAL_REG(0x40150044) |= (1<<1)|(1<<30);	

    TRACE("0x40150044 = 0x%x \n",DIGITAL_REG(0x40150044));
    while (((DIGITAL_REG(0x40150044)) & (0x1<<30)));
    hal_sys_timer_delay(MS_TO_TICKS(10));
    DIGITAL_REG(0x40150044) &= ~(1<<0);

    TRACE("0x40150044 = 0x%x \n", DIGITAL_REG(0x40150044));
    while (!((DIGITAL_REG(0x40150044)) & (0x1<<0)));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    DIGITAL_REG(0x40000090) = 0x0001ffff;
    TRACE("0x40000090 = 0x%x \n", DIGITAL_REG(0x40000090));
    while (!((DIGITAL_REG(0x40000090)) & (0x1<<20)));
    hal_sys_timer_delay(MS_TO_TICKS(10));
#endif	

	hal_psram_resume(HAL_PSRAM_ID_0);
}

SRAM_TEXT_LOC void hal_psram_sleep(void)
{
#if PSRAM_HIGH_SPEED
	 hal_psram_suspend(HAL_PSRAM_ID_0);
     /* dll power off */
	 DIGITAL_REG(0x40150044) &= ~(1 << 1);
     /* phy power off */
	 /* DIGITAL_REG(0x4008004c) &= ~(1 << 21) */
#endif
}

#else

SRAM_TEXT_LOC void hal_psram_init(void)
{
#if PSRAM_HIGH_SPEED
    unsigned int mr = 0;
    enum HAL_PSRAM_ID_T id = HAL_PSRAM_ID_0;
#endif
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_208M);

    hal_cmu_clock_enable(HAL_CMU_MOD_O_PSRAM);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_PSRAM);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_PSRAM);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_PSRAM);


#if PSRAM_HIGH_SPEED
    DIGITAL_REG(0x4001f038) = 0x77783533;

    DIGITAL_REG(0x4001f03c) = 0x3ffffff4;
    DIGITAL_REG(0x4001f03c) |= (1<<1)|(1<<30);	
    while (((DIGITAL_REG(0x4001f03c)) & (0x1<<30)));
    hal_sys_timer_delay(MS_TO_TICKS(10));
    DIGITAL_REG(0x4001f03c) &= ~(1<<0);
    while (!((DIGITAL_REG(0x4001f03c)) & (0x1<<0)));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    DIGITAL_REG(0x40000090) = 0x0001ffff;
    while (!((DIGITAL_REG(0x40000090)) & (0x1<<20)));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    //lp delay
    DIGITAL_REG(0x4015001C)=0xffffffff;//c
    //ramp=1k select low speed
    DIGITAL_REG(0x4015002C)=0x0000000d;//c
    //tx/rx Phase
    DIGITAL_REG(0x40150024)=0x55000563;//223
    while (!((DIGITAL_REG(0x40150024)) & (0x1<<3)));
#if 1
    //reset psram chip
    psramip_write32(0xff, 0x40150000, 0x00);
    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));
    //clear fifo
    psramip_write32(0x03, 0x40150000, 0x18);
    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    //read MR0
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_READ, 0);
    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));	
    //need read twice
    mr = psramip_r_rx_fifo(0x40150000);
    mr = psramip_r_rx_fifo(0x40150000);
    // hal_uart_printf("count=0x%x mr0 = 0x%x \n", psramip_r_status(0x40150000), mr);

    mr &= 0xff;
    mr &= ~(0x03 << 6); // MR[7:6] must set 0
    // driver strength 00:1/16 01:1/2 10:1/4 11:1/8 MR0[1:0]
    mr &= ~0x03;
    mr |= 0x01;
    //for >> 200M MR0[4:2]
    mr &= ~(0x07 << 2);
    mr |= (0x06 << 2);
    psramip_w_tx_fifo(0x40150000, mr);
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_WRITE, 0);
    _psram_busy_wait(id);

    //read MR0 again
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_READ, 0);
    while((psramip_r_status(0x40150000)& PSRAM_STAT_BUSY));	
    mr = psramip_r_rx_fifo(0x40150000);
    mr = psramip_r_rx_fifo(0x40150000);
    //  hal_uart_printf("count=0x%x mr0 = 0x%x \n", psramip_r_status(0x40150000), mr);	
    //read MR4
    psramip_w_cmd_addr(0x40150000, HAL_PSRAM_CMD_REG_READ, 4);
    _psram_busy_wait(id);
    mr = psramip_r_rx_fifo(0x40150000);
    mr = psramip_r_rx_fifo(0x40150000);
    //  hal_uart_printf("mr4 = 0x%x\n", mr);

    _psram_busy_wait(id);
#endif
    hal_sys_timer_delay(MS_TO_TICKS(10));
#else
    // open psram phy power
    DIGITAL_REG(0x4001f02c) |= (1 << 16);
    //init psram
    //lp delay
    DIGITAL_REG(0x4015001C)=0x3fff3cff;//c
    //ramp=1k select low speed
    DIGITAL_REG(0x4015002C)=0x0000000c;
    //tx/rx Phase
    DIGITAL_REG(0x40150024)=0x55000223;
#endif
}

SRAM_TEXT_LOC void hal_psram_wakeup_init(void)
{
#if PSRAM_HIGH_SPEED
    // dll
    DIGITAL_REG(0x4001f038) = 0x77783533;

    DIGITAL_REG(0x4001f03c) = 0x3ffffff4;
    DIGITAL_REG(0x4001f03c) |= (1<<1)|(1<<30);	
    while (((DIGITAL_REG(0x4001f03c)) & (0x1<<30)));
    hal_sys_timer_delay(MS_TO_TICKS(10));
    DIGITAL_REG(0x4001f03c) &= ~(1<<0);
    while (!((DIGITAL_REG(0x4001f03c)) & (0x1<<0)));
    hal_sys_timer_delay(MS_TO_TICKS(10));

    DIGITAL_REG(0x40000090) = 0x0001ffff;
    while (!((DIGITAL_REG(0x40000090)) & (0x1<<20)));
    hal_sys_timer_delay(MS_TO_TICKS(10));
#endif	

	hal_psram_resume(HAL_PSRAM_ID_0);
}

SRAM_TEXT_LOC void hal_psram_sleep(void)
{
#if PSRAM_HIGH_SPEED
	 hal_psram_suspend(HAL_PSRAM_ID_0);
     /* dll power off */
	 DIGITAL_REG(0x4001f03c) &= ~(1 << 1);
     /* phy power off */
	 /* DIGITAL_REG(0x4001f02c) &= ~(1 << 16) */
#endif
}

#endif

SRAM_TEXT_LOC void hal_psram_wakeup(void)
{
#if PSRAM_HIGH_SPEED
	hal_psram_wakeup_init();
#endif
}

#endif
