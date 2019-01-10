#include "include.h"

#if (GUI_SELECT == GUI_LEDSEG_7P7S)

AT(.com_text.ledseg)
void ledseg_7p7s_set(uint8_t seg_bits, uint8_t com_pin)
{
    u8 gpio_out = GPIOA & 0x80;
    u8 gpio_dir = GPIOADIR | 0x7f;
    if (seg_bits) {
        gpio_dir &= ~seg_bits;                  //需要显示的SEG设置输出，其他输入
        gpio_dir &= ~BIT(com_pin);              //对应COM设置输出高电平
        gpio_out |= BIT(com_pin);
    }
    GPIOA = gpio_out;
    GPIOADIR = gpio_dir;
}

AT(.com_text.ledseg)
void ledseg_7p7s_clr(void)
{
    GPIOADE |= 0x7f;
    GPIOADIR |= 0x7f;                       //全部输入，则不显示
    GPIOADRV |= 0x7f;                       //32mA驱动电流
}

AT(.text.ledseg)
void ledseg_7p7s_off(void)
{
    ledseg_init();
    GPIOADE |= 0x7f;
    GPIOADIR |= 0x7f;
    GPIOADRV &= ~0x7f;
}

AT(.com_text.ledseg)
void ledseg_7p7s_scan_hook(u8 *buf)
{
}

AT(.com_text.ledseg)
bool ledseg_7p7s_reuse_hook(void)
{
    return false;
}

AT(.text.ledseg)
void ledseg_7p7s_update_dispbuf(void)
{
    u8 dis_buf[7];
    memset(dis_buf, 0, 7);

    if (ledseg_buf[0] & SEG_A)  dis_buf[0] |= PIN1;
    if (ledseg_buf[0] & SEG_B)  dis_buf[0] |= PIN2;
    if (ledseg_buf[0] & SEG_C)  dis_buf[3] |= PIN0;
    if (ledseg_buf[0] & SEG_D)  dis_buf[4] |= PIN0;
    if (ledseg_buf[0] & SEG_E)  dis_buf[0] |= PIN3;
    if (ledseg_buf[0] & SEG_F)  dis_buf[1] |= PIN0;
    if (ledseg_buf[0] & SEG_G)  dis_buf[2] |= PIN0;

    if (ledseg_buf[1] & SEG_A)  dis_buf[1] |= PIN2;
    if (ledseg_buf[1] & SEG_B)  dis_buf[1] |= PIN3;
    if (ledseg_buf[1] & SEG_C)  dis_buf[4] |= PIN1;
    if (ledseg_buf[1] & SEG_D)  dis_buf[1] |= PIN5;
    if (ledseg_buf[1] & SEG_E)  dis_buf[1] |= PIN4;
    if (ledseg_buf[1] & SEG_F)  dis_buf[2] |= PIN1;
    if (ledseg_buf[1] & SEG_G)  dis_buf[3] |= PIN1;

    if (ledseg_buf[2] & SEG_A)  dis_buf[4] |= PIN3;
    if (ledseg_buf[2] & SEG_B)  dis_buf[2] |= PIN4;
    if (ledseg_buf[2] & SEG_C)  dis_buf[3] |= PIN4;
    if (ledseg_buf[2] & SEG_D)  dis_buf[5] |= PIN0;
    if (ledseg_buf[2] & SEG_E)  dis_buf[5] |= PIN2;
    if (ledseg_buf[2] & SEG_F)  dis_buf[3] |= PIN2;
    if (ledseg_buf[2] & SEG_G)  dis_buf[4] |= PIN2;

    if (ledseg_buf[3] & SEG_A)  dis_buf[6] |= PIN5;
    if (ledseg_buf[3] & SEG_B)  dis_buf[5] |= PIN6;
    if (ledseg_buf[3] & SEG_C)  dis_buf[4] |= PIN5;
    if (ledseg_buf[3] & SEG_D)  dis_buf[5] |= PIN3;
    if (ledseg_buf[3] & SEG_E)  dis_buf[3] |= PIN5;
    if (ledseg_buf[3] & SEG_F)  dis_buf[5] |= PIN4;
    if (ledseg_buf[3] & SEG_G)  dis_buf[4] |= PIN6;

    if (ledseg_buf[4] & ICON_PLAY)   dis_buf[0] |= PIN5;
    if (ledseg_buf[4] & ICON_PAUSE)  dis_buf[2] |= PIN5;
    if (ledseg_buf[4] & ICON_USB)    dis_buf[5] |= PIN1;
    if (ledseg_buf[4] & ICON_SD)     dis_buf[0] |= PIN4;
    if (ledseg_buf[4] & ICON_DDOT)   dis_buf[2] |= PIN3;
    if (ledseg_buf[4] & ICON_FM)     dis_buf[6] |= PIN2;
    if (ledseg_buf[4] & ICON_MP3)    dis_buf[2] |= PIN6;

    ledseg_7p7s_update_dispbuf_do(dis_buf);
}

#endif  // #if (GUI_SELECT == GUI_LEDSEG_7P7S)
