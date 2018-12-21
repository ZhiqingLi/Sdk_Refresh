#include "include.h"

#if MIC_DETECT_EN
///通过配置工具来选择MIC检测GPIO

static gpio_t mic_gpio;

void mic_detect_init(void)
{
    gpio_t *g = &mic_gpio;
    u8 io_num = xcfg_cb.mic_det_iosel;
    bsp_gpio_cfg_init(&mic_gpio, xcfg_cb.mic_det_iosel);

    if (!io_num) {
        return;
    } else if (io_num == IO_MUX_MICL) {
        //复用MICL检测
        GPIOFDE |= BIT(2);
        GPIOFPU |= BIT(2);
        GPIOFDIR |= BIT(2);
    } else if (io_num == IO_MUX_SDCLK) {
        SD_MUX_DETECT_INIT();
    } else if (io_num < IO_PF5) {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.mic)
bool mic_is_online(void)
{
    gpio_t *g = &mic_gpio;
    u8 io_num = xcfg_cb.mic_det_iosel;

    //无mic检测
    if (!io_num) {
        return false;
    }

    if (io_num == IO_MUX_SDCLK) {
        return SD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_SDCMD) {
        return SD_CMD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_PWRKEY) {
        return pwrkey_detect_flag;
    }else if (io_num == IO_MUX_SDDAT) {
        return SD_DAT_MUX_IS_ONLINE();
    } else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.mic)
bool is_detect_mic_busy(void)
{
    u8 io_num = xcfg_cb.mic_det_iosel;

    //无mic检测
    if (!io_num) {
        return true;
    }

    //复用SDCMD或SDCLK检测
    if ((io_num == IO_MUX_SDCMD) || (io_num == IO_MUX_SDCLK) || (io_num == IO_MUX_SDDAT)) {
        return SD_MUX_IS_BUSY();
    }

    return false;
}
#endif
