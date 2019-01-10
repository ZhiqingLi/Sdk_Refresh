#include "include.h"

static gpio_t mute_gpio;

void loudspeaker_mute_init(void)
{
    if (!xcfg_cb.spk_mute_en) {
        xcfg_cb.loudspeaker_unmute_delay = 0;
        return;
    }
    u8 io_num = xcfg_cb.spk_mute_io_sel;
    gpio_t *p = &mute_gpio;
    bsp_gpio_cfg_init(p, io_num);
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] &= ~BIT(p->num);
    if (xcfg_cb.high_mute) {
        p->sfr[GPIOxSET] = BIT(p->num);
    } else {
        p->sfr[GPIOxCLR] = BIT(p->num);
    }
}

AT(.com_text.mute)
void loudspeaker_mute(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }
    if (xcfg_cb.high_mute) {
        p->sfr[GPIOxSET] = BIT(p->num);
    } else {
        p->sfr[GPIOxCLR] = BIT(p->num);
    }
}

AT(.com_text.mute)
void loudspeaker_unmute(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }
    if (xcfg_cb.high_mute) {
        p->sfr[GPIOxCLR] = BIT(p->num);
    } else {
        p->sfr[GPIOxSET] = BIT(p->num);
    }
}

AT(.com_text.mute)
void loudspeaker_disable(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDIR] |= BIT(p->num);
}

