#include "include.h"

volatile int adc_linein_flag;

#if FUNC_AUX_EN

bool is_linein_enter_enable(void)
{
    if ((!xcfg_cb.func_aux_en)) {
        return false;
    }

#if LINEIN_DETECT_EN
    if (!dev_is_online(DEV_LINEIN)) {
        return false;
    }
#endif // LINEIN_DETECT_EN

    return true;
}

#endif // FUNC_AUX_EN

#if LINEIN_DETECT_EN

gpio_t linein_gpio;

void linein_detect_init(void)
{
    gpio_t *g = &linein_gpio;
    bsp_gpio_cfg_init(&linein_gpio, xcfg_cb.linein_det_io);

    if (!xcfg_cb.linein_2_pwrdown_en) {
        return;
    }
    if (!xcfg_cb.linein_det_io) {
        //复用MICL检测
        GPIOFDE |= BIT(2);
        GPIOFPU |= BIT(2);
        GPIOFDIR |= BIT(2);
    } else {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.linein)
bool linein_is_online(void)
{
    gpio_t *g = &linein_gpio;

    if (!xcfg_cb.linein_2_pwrdown_en) {
        return false;
    }
    if (!xcfg_cb.linein_det_io) {
        return linein_micl_is_online();
    } else if (xcfg_cb.adc_detect_linein){
        return adc_linein_flag;
    } else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.linein)
bool is_detect_linein_busy(void)
{
    if (!xcfg_cb.linein_2_pwrdown_en) {
        return true;
    }
    return false;
}

AT(.com_text.linein)
bool is_sleep_dac_off_enable(void)
{
    if (!xcfg_cb.linein_2_pwrdown_en) {
        return true;
    }

    if (xcfg_cb.linein_det_io) {
        return true;
    }

    return false;
}

AT(.com_text.linein)
bool is_linein_det_mux_micl(void)
{
    if ((xcfg_cb.linein_2_pwrdown_en) && (!xcfg_cb.linein_det_io)) {
        return true;
    } else {
        return false;
    }
}
#endif
