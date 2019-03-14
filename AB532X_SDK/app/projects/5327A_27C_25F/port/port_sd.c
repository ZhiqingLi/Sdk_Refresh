#include "include.h"

#if MUSIC_SDCARD_EN
static gpio_t sddet_gpio;

void sdcard_detect_init(void)
{
    gpio_t *g = &sddet_gpio;
    u8 io_num = xcfg_cb.sddet_iosel;
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

AT(.com_text.sdio)
bool sdcard_is_online(void)
{
    gpio_t *g = &sddet_gpio;
    u8 io_num = xcfg_cb.sddet_iosel;

    if (!io_num) {
        return false;
    } else if (io_num == IO_MUX_SDCLK) {
        return SD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_SDCMD) {
        return SD_CMD_MUX_IS_ONLINE();
    } else if (io_num == IO_MUX_PWRKEY) {
        return pwrkey_detect_flag;
    } else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.sdio)
bool is_det_sdcard_busy(void)
{
    u8 io_num = xcfg_cb.sddet_iosel;

    //无SD检测
    if (!io_num) {
        return true;
    }

    //复用SDCMD或SDCLK检测
    if (io_num == IO_MUX_SDCLK) {
        return SD_MUX_IS_BUSY();
    }else if(io_num == IO_MUX_SDCMD){
        return SD_MUX_CMD_IS_BUSY();
    }
    return false;
}

AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
    if (type == 0) {
        SD_MUX_IO_INIT();
    #if SDCMD_MUX_DETECT_EN
        SD_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
    #endif
    } else if (type == 1) {
    #if USER_ADKEY_MUX_SDCLK || VBAT_DETECT_MUX_SDCLK
        adc_mux_sdclk_w4_convert();
    #endif
	#if SD_USB_MUX_IO_EN
		get_usb_chk_sta_convert();
	#endif
        SD_CLK_DIR_OUT();
    #if SDCMD_MUX_DETECT_EN
        SD_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
    #endif
        FUNCMCON0 = SD0_MAPPING;
    } else {
    #if ((USER_ADKEY_MUX_SDCLK  && (!ADKEY_PU10K_EN)) || VBAT_DETECT_MUX_SDCLK)
        if (xcfg_cb.user_adkey_mux_sdclk_en || VBAT_DETECT_MUX_SDCLK) {     //有外部上拉, 关掉内部上拉
            SD_CLK_IN_DIS_PU10K();
        } else
    #endif
        {
            if(xcfg_cb.sddet_iosel != IO_MUX_SDCMD){
                SD_CLK_DIR_IN();
            }
        }
    #if SDCMD_MUX_DETECT_EN
        SD_CMD_MUX_PU10K();                         //IDLE状态，SDCMD上拉10K检测外设
    #endif
    }
}

#else
AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
}
#endif          // MUSIC_SDCARD_EN
