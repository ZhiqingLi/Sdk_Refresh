#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_timer.h"
#include "hal_location.h"
#include "hal_psc.h"
#include "reg_psc_best2300.h"

#define PSC_WRITE_ENABLE                    0xCAFE0000

static struct AONPSC_T * const psc = (struct AONPSC_T *)AON_PMU_BASE;

void BOOT_TEXT_FLASH_LOC hal_psc_init(void)
{
    // Setup wakeup mask
    psc->REG_080 = 0xFFFFFFFF;
    psc->REG_084 = 0xFFFFFFFF;

    // Setup power up timer4
    psc->REG_00C = SET_BITFIELD(psc->REG_00C, PSC_AON_MCU_POWERUP_TIMER4, 0x3F);
}

void SRAM_TEXT_LOC hal_psc_mcu_auto_power_down(void)
{
    psc->REG_018 = PSC_WRITE_ENABLE | 0;
    psc->REG_000 = PSC_WRITE_ENABLE | PSC_AON_MCU_PG_AUTO_EN;
    psc->REG_010 = PSC_WRITE_ENABLE | PSC_AON_MCU_POWERDN_START;
}

void SRAM_TEXT_LOC hal_psc_mcu_auto_power_up(void)
{
    psc->REG_014 = PSC_WRITE_ENABLE | PSC_AON_MCU_POWERUP_START;
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_enable(void)
{
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR |
        PSC_AON_CODEC_CLK_STOP_DR;
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_disable(void)
{
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR | PSC_AON_CODEC_PSW_EN_REG |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_enable(void)
{
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR;

#ifdef JTAG_BT
    psc->REG_064 |= PSC_AON_CODEC_RESERVED(1 << 3);
    psc->REG_064 &= ~PSC_AON_CODEC_RESERVED(1 << 2);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_disable(void)
{
#ifdef JTAG_BT
    psc->REG_064 &= ~PSC_AON_CODEC_RESERVED(1 << 3);
    psc->REG_064 |= PSC_AON_CODEC_RESERVED(1 << 2);
#endif

    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR | PSC_AON_BT_PSW_EN_REG |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
}

