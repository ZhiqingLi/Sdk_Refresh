#include "include.h"

#if FUNC_AUX_EN

bool is_linein_enter_enable(void)
{
    if ((!xcfg_cb.func_aux_en)) {
        return false;
    }

    if (xcfg_cb.linein_2_pwrdown_en) {
        return false;
    }

#if LINEIN_DETECT_EN
    if (xcfg_cb.mode_2_aux_en) {
        return true;
    }
    if ((xcfg_cb.linein_det_iosel) && (!dev_is_online(DEV_LINEIN))) {
        return false;
    }
#endif // LINEIN_DETECT_EN

    return true;
}

#endif // FUNC_AUX_EN


#if LINEIN_DETECT_EN
///通过配置工具来选择LINEIN检测GPIO

static gpio_t linein_gpio;

void linein_detect_init(void)
{
    gpio_t *g = &linein_gpio;
    u8 io_num = xcfg_cb.linein_det_iosel;
    bsp_gpio_cfg_init(&linein_gpio, xcfg_cb.linein_det_iosel);

    if (!io_num) {
        return;
    } else if (io_num == IO_MUX_MICL) {
        //复用MICL检测
        GPIOFDE |= BIT(2);
        GPIOFPU |= BIT(2);
        GPIOFDIR |= BIT(2);
    } else if (io_num == IO_MUX_SDCLK) {
        SD_MUX_DETECT_INIT();
    } else if (io_num <= IO_PF5) {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.linein)
bool linein_is_online(void)
{
    gpio_t *g = &linein_gpio;
    u8 io_num = xcfg_cb.linein_det_iosel;

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
    } else if (io_num == IO_MUX_MICL) {
        return linein_micl_is_online();
    }else if (io_num == IO_MUX_SDDAT) {
        return SD_DAT_MUX_IS_ONLINE();
    } else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.linein)
bool is_detect_linein_busy(void)
{
    u8 io_num = xcfg_cb.linein_det_iosel;

    //无LINEIN检测
    if (!io_num) {
        return true;
    }

    //复用SDCMD或SDCLK检测
    if ((io_num == IO_MUX_SDCMD) || (io_num == IO_MUX_SDCLK) || (io_num == IO_MUX_SDDAT)) {
        return SD_MUX_IS_BUSY();
    }

    return false;
}

AT(.com_text.linein)
bool is_linein_det_mux_micl(void)
{
    if (xcfg_cb.linein_det_iosel == IO_MUX_MICL) {
        return true;
    } else {
        return false;
    }
}
#endif
