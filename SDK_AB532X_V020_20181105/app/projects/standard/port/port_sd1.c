#include "include.h"

#if MUSIC_SDCARD1_EN
static gpio_t sd1det_gpio;

void sdcard1_detect_init(void)
{
    gpio_t *g = &sd1det_gpio;
    u8 io_num = xcfg_cb.sd1det_iosel;
    bsp_gpio_cfg_init(g, io_num);

    if (!io_num) {
        return;
    } else if (io_num == IO_MUX_SDCLK) {
        SD1_MUX_DETECT_INIT();
    } else if (io_num < IO_PF5) {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.sdio)
bool sdcard1_is_online(void)
{
    gpio_t *g = &sd1det_gpio;
    u8 io_num = xcfg_cb.sd1det_iosel;

    if (!io_num) {
        return false;
    } else if (io_num == IO_MUX_SDCLK) {
        return SD1_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_SDCMD) {
        return SD1_CMD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_PWRKEY) {
        return pwrkey_detect_flag;
    } else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.sdio)
bool is_det_sdcard1_busy(void)
{
    u8 io_num = xcfg_cb.sd1det_iosel;

    //无SD检测
    if (!io_num) {
        return true;
    }

    //复用SDCMD或SDCLK检测
    if ((io_num == IO_MUX_SDCMD) || (io_num == IO_MUX_SDCLK)) {
        return SD1_MUX_IS_BUSY();
    }
    return false;
}

AT(.text.sdcard)
void sd1_gpio_init(u8 type)
{
    if (type == 0) {
        SD1_MUX_IO_INIT();
        SD1_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
    } else if (type == 1) {
        SD1_CLK_DIR_OUT();
        SD1_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
        FUNCMCON0 = SD1_MAPPING;
    } else {
        SD1_CLK_DIR_IN();
        SD1_CMD_MUX_PU10K();                     //IDLE状态，SDCMD上拉10K检测外设
    }
}

#else
AT(.text.sdcard)
void sd1_gpio_init(u8 type)
{
}
#endif          // MUSIC_SDCARD1_EN
