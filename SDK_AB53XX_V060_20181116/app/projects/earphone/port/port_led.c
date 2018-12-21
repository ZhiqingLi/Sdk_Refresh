#include "include.h"

#if LED_DISP_EN
led_func_t bled_func AT(.buf.led);
led_func_t rled_func AT(.buf.led);
gpio_t bled_gpio AT(.buf.led);
gpio_t rled_gpio AT(.buf.led);

volatile u8 dac_pow_level;
volatile int fft_led_flag;

AT(.text.led_disp)
void led_power_up(void)
{
    //是否配置开机状态LED?
    if (xcfg_cb.led_pwron_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_poweron);
    } else {
        led_set_sta(0x02, 0x01, 10, 0xff);          //蓝灯先亮500ms, 红灯再亮500ms。 只亮一次
    }
}

AT(.text.led_disp)
void led_power_down(void)
{
#if LINEIN_2_PWRDOWN_EN
    if ((xcfg_cb.linein_2_pwrdown_en) && (!LINEIN_2_PWRDOWN_TONE_EN)) {
        if (dev_is_online(DEV_LINEIN)) {
            led_set_sta(0x00, 0x00, 1, 0);         //蓝灯灭， 红灯灭
            return;
        }
    }
#endif // LINEIN_2_PWRDOWN_EN

    //是否配置关机状态LED?
    if (xcfg_cb.led_pwroff_config_en) {
        led_cfg_set_sta((led_cfg_t *)&xcfg_cb.led_poweroff);
    } else {
        led_set_sta(0x40, 0x15, 4, 0xff);         //蓝灯闪3次，红灯闪1次。
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
        led_set_sta(0x60, 0x03, 3, 6);           //亮300ms, 灭450ms。 蓝红交替闪
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
void led_bt_scan(void)
{
    led_bt_idle();
}

AT(.text.led_disp)
void led_music_busy(void)
{
    led_set_sta(0x00, 0xaa, 4, 0);            //蓝灯200ms周期快闪烁
}

AT(.text.led_disp)
void led_music_play(void)
{
    led_set_sta(0x00, 0xaa, 20, 0);           //蓝灯1s周期闪烁
}

AT(.text.led_disp)
void led_aux_play(void)
{
    led_set_sta(0x00, 0xaa, 20, 0);           //蓝灯1s周期闪烁
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
void led_bt_pb_sleep(void)
{
    led_set_sta(0x00, 0x02, 1, 92);           //每5秒亮50ms
}

AT(.sleep_text.led_disp)
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

//蓝灯低电平推灯
AT(.com_text.led_disp)
bool led_cfg_is_on_0(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return false;
    }
    return (!(g->sfr[GPIOx] & BIT(g->num)));
}

AT(.com_text.led_disp)
void led_cfg_set_on_0(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return;
    }
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxDIR] &= ~BIT(g->num);
    g->sfr[GPIOxCLR] = BIT(g->num);
}

AT(.com_text.led_disp)
void led_cfg_set_off_0(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return;
    }
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxDIR] &= ~BIT(g->num);
    g->sfr[GPIOxSET] = BIT(g->num);
}

AT(.com_text.led_disp)
void led_cfg_port_init_0(gpio_t *g)
{
    led_cfg_set_off_0(g);
}


AT(.com_text.led_disp)
void bled_fft_dac_pow(void)
{
    u32 pow;
    static int cnt = 0, cnt5ms = 0;

    if ((BLED_FFT_EN) && (f_bt.disp_status == BT_STA_PLAYING)) {
        fft_led_flag = 1;
    } else {
        fft_led_flag = 0;
    }

    if (fft_led_flag)  {
        pow = ((u32)dac_pcm_pow_calc() * sys_cb.vol) / VOL_MAX;
        if (pow < 0x50) {
            dac_pow_level = 0;
        } else if (pow < 0x600) {
            dac_pow_level = 0x01;
        } else if (pow < 0x1000) {
            dac_pow_level = 0x03;
        } else if (pow < 0x1800) {
            dac_pow_level = 0x07;
        } else if (pow < 0x2800) {
            dac_pow_level = 0x0f;
        } else if (pow < 0x5000) {
            dac_pow_level = 0x1f;
        } else if (pow < 0x6000) {
            dac_pow_level = 0x3f;
        } else if (pow < 0x8000) {
            dac_pow_level = 0x7f;
        } else {
            dac_pow_level = 0xff;
        }

        cnt5ms++;
        if (cnt5ms >= 6) {
            cnt5ms = 0;
            bled_set_off();
            if (dac_pow_level & BIT(cnt)) {
                bled_set_on();
            }
            if (++cnt >= 8) {
                cnt = 0;
            }
        }
    }
}

AT(.com_text.bsp.sys)
bool is_led_scan_enable(void)
{
    return (LED_DISP_EN & (!fft_led_flag));
}

void led_func_init(void)
{
    bsp_gpio_cfg_init(&bled_gpio, xcfg_cb.bled_io_sel);
    bsp_gpio_cfg_init(&rled_gpio, xcfg_cb.rled_io_sel);
    fft_led_flag = 0;

    if (xcfg_cb.bled_io_sel != xcfg_cb.rled_io_sel) {
        sys_cb.port2led_en = 0;
        bled_func.port_init = led_cfg_port_init;
        bled_func.set_on = led_cfg_set_on;
        bled_func.set_off = led_cfg_set_off;

        if (BLED_LOW2ON_EN) {
            bled_func.port_init = led_cfg_port_init_0;
            bled_func.set_on = led_cfg_set_on_0;
            bled_func.set_off = led_cfg_set_off_0;
        }

        rled_func.port_init = led_cfg_port_init;
        rled_func.set_on = led_cfg_set_on;
        rled_func.set_off = led_cfg_set_off;
    } else {
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
    }
}
#endif
