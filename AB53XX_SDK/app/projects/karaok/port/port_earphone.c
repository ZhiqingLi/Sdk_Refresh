#include "include.h"

#if EARPHONE_DETECT_EN
///通过配置工具来选择EARPHONE检测GPIO

static gpio_t earphone_gpio;

void earphone_detect_init(void)
{
    gpio_t *g = &earphone_gpio;
    u8 io_num = xcfg_cb.earphone_det_iosel;
    bsp_gpio_cfg_init(g, io_num);

    if (!io_num) {
        return;
    } else if (io_num == IO_MUX_SDCLK) {
        SD_MUX_DETECT_INIT();
    } else if (io_num <= IO_PF5) {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.linein)
bool earphone_is_online(void)
{
    gpio_t *g = &earphone_gpio;
    u8 io_num = xcfg_cb.earphone_det_iosel;

    //无LINEIN检测
    if (!io_num) {
        return false;
    }

    if (io_num == IO_MUX_SDCLK) {
        return SD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_SDCMD) {
        return SD_CMD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_PWRKEY) {
        return pwrkey_detect_flag;
    } else if (io_num == IO_MUX_ADKEY) {
        return adkey_detect_flag;
    } else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.linein)
bool is_detect_earphone_busy(void)
{
    u8 io_num = xcfg_cb.earphone_det_iosel;

    //无LINEIN检测
    if (!io_num) {
        return true;
    }

    //复用SDCMD或SDCLK检测
    if ((io_num == IO_MUX_SDCMD) || (io_num == IO_MUX_SDCLK)) {
        return SD_MUX_IS_BUSY();
    }

    return false;
}

#endif
