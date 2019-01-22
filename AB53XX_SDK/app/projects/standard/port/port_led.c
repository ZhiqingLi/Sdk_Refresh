#include "include.h"

#if LED_DISP_EN
led_func_t bled_func AT(.buf.led);
led_func_t rled_func AT(.buf.led);
gpio_t bled_gpio AT(.buf.led);
gpio_t rled_gpio AT(.buf.led);

AT(.text.led_disp)
void led_power_up(void)
{
    //是否配置开机状态LED?
    if (xcfg_cb.led_pwron_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_poweron);
    } else {
        led_set_sta(0x00, 0xff, 1, 0);          //蓝灯常亮
    }
}

AT(.text.led_disp)
void led_power_down(void)
{
    //是否配置关机状态LED?
    if (xcfg_cb.led_pwroff_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_poweroff);
    } else {
        led_set_sta(0x00, 0x00, 0, 0);          //红灯，蓝灯灭
    }
}

AT(.text.led_disp)
void led_idle(void)
{
    led_set_sta(0x00, 0xff, 1, 0);               //蓝灯常亮
}

AT(.text.led_disp)
void led_bt_init(void)
{
    if (xcfg_cb.led_btinit_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_btinit);
    } else {
        led_set_sta(0x00, 0xaa, 10, 0);          //蓝灯500ms周期闪烁，默认跟w4pwrkey一样
    }
}

AT(.text.led_disp)
void led_bt_idle(void)
{
    //是否配置蓝牙配对状态LED?
    if (xcfg_cb.led_btpair_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_pairing);
    } else {
        led_set_sta(0x00, 0xaa, 10, 0);          //蓝灯500ms周期闪烁
    }
}

AT(.text.led_disp)
void led_bt_reconnect(void)
{
    if (xcfg_cb.led_reconnect_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_reconnect);
    } else {
        led_set_sta(0x00, 0xaa, 6, 0);          //蓝灯300ms周期闪烁
    }
}

AT(.text.led_disp)
void led_bt_connected(void)
{
    //是否配置蓝牙已连接状态LED?
    if (xcfg_cb.led_btconn_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_connected);
    } else {
        led_set_sta(0x00, 0x02, 2, 86);         //亮100ms, 灭5S
    }
}

AT(.text.led_disp)
void led_bt_play(void)
{
    //是否配置蓝牙音乐播放状态LED?
    if (xcfg_cb.led_btmusic_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_playing);
    } else {
        led_set_sta(0x00, 0x02, 2, 86);       //亮100ms, 灭5S
    }
}

AT(.text.led_disp)
void led_bt_ring(void)
{
    //是否配置蓝牙来电状态LED?
    if (xcfg_cb.led_btring_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_ring);
    } else {
        led_set_sta(0x00, 0x02, 2, 86);      //亮100ms, 灭5S
    }
}

AT(.text.led_disp)
void led_bt_call(void)
{
    //是否配置蓝牙来电状态LED?
    if (xcfg_cb.led_btcall_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_calling);
    } else {
        led_set_sta(0x00, 0x0a, 2, 86);       //亮100ms, 灭5S, 亮两次
    }
}

AT(.text.led_disp)
void led_music_busy(void)
{
     led_set_sta(0x00, 0xaa, 4, 0);            //蓝灯200ms周期快闪烁
}

AT(.text.led_disp)
void led_music_play(void)
{
    led_set_sta(0x00, 0xaa, 10, 0);           //蓝灯500ms周期闪烁
}

AT(.text.led_disp)
void led_aux_play(void)
{
     led_set_sta(0x00, 0xff, 20, 0);           //蓝灯长亮
}

AT(.text.led_disp)
void led_fm_play(void)
{
     led_set_sta(0x00, 0xaa, 20, 0);           //蓝灯1s周期闪烁
}

AT(.text.led_disp)
void led_fm_scan(void)
{
    led_set_sta(0x00, 0xaa, 4, 0);            //蓝灯200ms周期快闪烁
}

AT(.text.led_disp)
void led_record(void)
{
    led_set_sta(0x00, 0xaa, 20, 0);           //蓝灯1s周期闪烁
}

AT(.text.led_disp)
void led_bt_scan(void)
{
    led_bt_idle();
}

AT(.text.led_disp)
void led_bt_pb_sleep(void)
{
    led_set_sta(0x00, 0x02, 1, 92);           //每5秒亮50ms
}

AT(.sleep_text.led)
bool led_bt_sleep(void)
{
    static u8 cnt = 0;

    cnt++;
    if (cnt >= 10) {
        cnt = 0;
        LED_INIT();
        bled_set_on();
        return true;
    } else {
        bled_set_off();
        return false;
    }
}

AT(.com_text.bsp.sys)
bool is_led_scan_enable(void)
{
    return (LED_DISP_EN);
}

void led_func_init(void)
{
    bsp_gpio_cfg_init(&bled_gpio, xcfg_cb.bled_io_sel);
    bsp_gpio_cfg_init(&rled_gpio, xcfg_cb.rled_io_sel);

#if LED_PWR_EN
    if (xcfg_cb.bled_io_sel == xcfg_cb.rled_io_sel) {
        //1个IO推两个灯
        if (bled_gpio.sfr != NULL) {
            sys_cb.port2led_en = 1;
        }
        bled_func.port_init = led2_port_init;
        bled_func.set_on = bled2_set_on;
        bled_func.set_off = bled2_set_off;

        rled_func.port_init = led2_port_init;
        rled_func.set_on = rled2_set_on;
        rled_func.set_off = rled2_set_off;
    } else
#endif // LED_PWR_EN
    {
        sys_cb.port2led_en = 0;
        bled_func.port_init = led_cfg_port_init;
        bled_func.set_on = led_cfg_set_on;
        bled_func.set_off = led_cfg_set_off;

        #if USER_ADKEY_MUX_LED
        bled_func.port_init = led_mux_cfg_port_init;
        bled_func.set_on = led_mux_cfg_set_on;
        bled_func.set_off = led_mux_cfg_set_off;
        #endif // USER_ADKEY_MUX_LED

        rled_func.port_init = led_cfg_port_init;
        rled_func.set_on = led_cfg_set_on;
        rled_func.set_off = led_cfg_set_off;
    }
}

#endif
