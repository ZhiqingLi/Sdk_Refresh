#include "include.h"

#if CHARGE_EN
typedef struct {
    u8 chag_sta   : 3;          //充电状态
    u8 dcin_reset : 1;          //DC插入复位
    u8 const_curr : 4,          //恒流充电电流
       trick_curr : 4;          //涓流充电电流

    u8 stop_curr : 3,           //充电截止电流
       stop_volt : 2;           //充电截止电压
    u8 trick_curr_en    : 1;    //是否使能涓流充电
    u8 trick_stop_volt  : 1;    //涓流充电截止电压
    u8 charge_lx_detect : 2;
    u8 charge_lx_filter_time    : 1;
    u8 charge_lx_wake_sel       : 1;
    u8 charge_lx_dc_sel         : 1;
    volatile u16 stop_time;
} charge_cfg_t;

static charge_cfg_t charge_cfg;
void charge_stop(u8 mode);
void charge_init_p(charge_cfg_t *p);

AT(.com_text.charge)
void charge_disp_sta(u8 sta)
{
    if (sta == 1) {
        //充电开启
        sys_cb.charge_sta = 1;
        sys_cb.charge_bled_flag = 0;
        charge_led_on();
    } else {
        //充电关闭
        sys_cb.charge_sta = 0;
        sys_cb.charge_bled_flag = 0;
        charge_led_off();
        if (sta == 2) {
            sys_cb.charge_sta = 2;
            //充电满亮蓝灯
            if (BLED_CHARGE_FULL_EN) {
                sys_cb.charge_bled_flag = 1;
                charge_bled_on();
                sys_cb.charge_full_bled_on_cnt = xcfg_cb.charge_full_bled_on_time;
            }
        }
    }
}

AT(.com_text.charge)
u16 charge_get_vusb(void)
{
    return 1908 * (uint)adc_cb.vusb_val / adc_cb.bg;
}

void charge_off(void)
{
    if (charge_cfg.chag_sta >= 3) {
        charge_stop(0);
    }
}

void charge_set_stop_time(u16 stop_time)
{
    charge_cfg.stop_time = stop_time;
}

void charge_init(void)
{
    charge_cfg_t *p = &charge_cfg;
    memset(p, 0, sizeof(charge_cfg_t));

    p->trick_curr_en = CHARGE_TRICK_EN;
    p->const_curr = CHARGE_CONSTANT_CURR & 0x0f;
    p->trick_curr = CHARGE_TRICKLE_CURR & 0x0f;
    p->stop_curr = CHARGE_STOP_CURR & 0x07;
    p->stop_volt = CHARGE_STOP_VOLT & 0x03;
    p->dcin_reset = CHARGE_DC_RESET & 0x01;
    p->trick_stop_volt = CHARGE_TRICK_STOP_VOLT;
    p->stop_time = 18000;   //默认30分钟

    p->charge_lx_detect = 2;
    p->charge_lx_filter_time = 0;
    p->charge_lx_wake_sel = 1;
    p->charge_lx_dc_sel = 1;

    charge_init_p(p);
}
#endif // CHARGE_EN
