#include "plat_types.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "aud_section.h"
#include "audioflinger.h"
#include "anc_process.h"
#include "hal_key.h"
#include "app_key.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_timer_raw.h"
#include "hwtimer_list.h"
#include "app_thread.h"
#include "tgt_hardware.h"
#include "pmu.h"
#include "hal_bootmode.h"
#include "hal_codec.h"

#include "app_status_ind.h"
#ifdef __SIMPLE_INTERNAL_PLAYER_SUPPORT__
#include "simple_internal_player.h"
#endif
#include "hal_aud.h"

#define ANC_MODE_SWITCH_WITHOUT_FADE

extern uint8_t app_poweroff_flag;
uint32_t app_anc_get_anc_status(void);

void app_anc_disable(void);
void app_anc_close_anc(void);
osStatus osDelay(uint32_t millisec);
void app_anc_timer_set(uint32_t request, uint32_t delay);
extern bool app_mode_is_usbaudio(void); //96k 384k
extern bool app_mode_is_i2s(void);      //96k 384k
void app_anc_init_timer(void);
extern void i2s_player_send_stop(void);
extern void fb_anti_howl_start(void);
extern void fb_anti_howl_stop(void);
extern void hal_codec_reconfig_pll_freq(enum AUD_SAMPRATE_T dac_rate, enum AUD_SAMPRATE_T adc_rate);
extern void anc_status_sync(void);
extern uint8_t is_sco_mode(void);
extern void anc_status_sync_init(void);
void app_anc_switch_set_edge(uint8_t down_edge);

typedef void (*ANC_KEY_CALLBACK)(uint8_t status);
static ANC_KEY_CALLBACK app_switch_callback;

extern int app_shutdown(void);
extern int app_reset(void);

extern void analog_aud_codec_speaker_enable(bool en);

#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
extern bool anc_single_mode_is_on(void);
#endif
enum
{
    ANC_STATUS_OFF = 0,
    ANC_STATUS_ON,
    ANC_STATUS_WAITING_ON,
    ANC_STATUS_WAITING_OFF,
    ANC_STATUS_INIT_ON,
    ANC_STATUS_NONE
};

enum
{
    ANC_EVENT_INIT = 0,
    ANC_EVENT_OPEN,
    ANC_EVENT_CLOSE,
    ANC_EVENT_FADE_IN,
    ANC_EVENT_FADE_OUT,
    ANC_EVENT_CHANGE_SAMPLERATE,
    ANC_EVENT_CHANGE_STATUS,
    ANC_EVENT_HOWL_PROCESS,
    ANC_EVENT_SYNC_STATUS,
    ANC_EVENT_SYNC_INIT,
    ANC_EVENT_PWR_KEY_MONITOR,
    ANC_EVENT_PWR_KEY_MONITOR_REBOOT,
    ANC_EVENT_SWITCH_KEY_DEBONCE,
    SIMPLE_PLAYER_CLOSE_CODEC_EVT,
    SIMPLE_PLAYER_DELAY_STOP_EVT,
    ANC_EVENT_NONE
};

static uint32_t anc_work_status = ANC_STATUS_OFF;
static uint32_t anc_timer_request = ANC_EVENT_NONE;

static enum AUD_SAMPRATE_T anc_sample_rate[AUD_STREAM_NUM];

static HWTIMER_ID anc_timerid = NULL;
#define anc_gain_adjust_gap (60)

#define anc_init_switch_off_time (MS_TO_TICKS(1000 * 60 * 2))
#define anc_auto_power_off_time (MS_TO_TICKS(1000 * 60 * 60))

#define anc_switch_on_time (MS_TO_TICKS(600))

#define anc_close_delay_time (MS_TO_TICKS(1000 * 20))

#define anc_pwr_key_monitor_time (MS_TO_TICKS(1500))

#define anc_switch_key_debonce_time (MS_TO_TICKS(40))

enum
{
    APP_ANC_IDLE = 0,
    APP_ANC_FADE_IN,
    APP_ANC_FADE_OUT,
};

uint32_t app_anc_fade_status = APP_ANC_IDLE;

static bool app_init_done = false;
#ifdef ANC_SWITCH_PIN
static bool app_anc_shutdown = false;
#endif
bool anc_set_dac_pa_delay = false;
static enum ANC_INDEX anc_coef_idx = 0;

extern void analog_aud_enable_dac_pa(uint8_t dac);

void app_anc_set_init_done(void)
{
    app_init_done = true;
}

void app_anc_switch_turnled(bool on);

#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__

static ANC_KEY_CALLBACK app_pwr_key_monitor_callback;

static void app_pwr_key_monitor_handler(enum HAL_GPIO_PIN_T pin)
{
    uint8_t gpio_val = hal_gpio_pin_get_val(pin);

    TRACE(" %s :%d ", __func__, gpio_val);
    if (app_pwr_key_monitor_callback)
        app_pwr_key_monitor_callback(gpio_val);
}

bool app_pwr_key_monitor_get_val(void)
{
    return (bool)hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)PWR_KEY_MONITOR_PIN);
}

void app_pwr_key_monitor_set_int_edge(uint8_t down_edge)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;
    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_polarity = (down_edge == 1) ? HAL_GPIO_IRQ_POLARITY_LOW_FALLING : HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    gpiocfg.irq_handler = app_pwr_key_monitor_handler;
    TRACE(" %s :%d ", __func__, down_edge);
    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)PWR_KEY_MONITOR_PIN, &gpiocfg);
}

void app_pwr_key_monitor_init(ANC_KEY_CALLBACK callback)
{
    app_pwr_key_monitor_callback = callback;
}
#endif

#ifdef ANC_LED_PIN
void app_anc_switch_turnled(bool on)
{
    TRACE(" %s on %d ", __func__, on);

    if (cfg_anc_led.pin != HAL_IOMUX_PIN_NUM)
    {
        if (on)
        {
            TRACE("on\n");
            hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_anc_led.pin);
        }
        else
        {
            TRACE("off\n");
            hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_anc_led.pin);
        }
    }
}
#endif

#ifdef __ANC_STICK_SWITCH_USE_GPIO__

static void app_anc_switch_int_handler(enum HAL_GPIO_PIN_T pin)
{
    // uint8_t gpio_val;
    uint8_t gpio_val = hal_gpio_pin_get_val(pin);
    TRACE(" %s ,pin %d,status %d", __func__, pin, gpio_val);
#ifdef ANC_LED_PIN
    if (gpio_val)
        app_anc_switch_turnled(true);
    else
        app_anc_switch_turnled(false);
#endif
    app_anc_switch_set_edge(gpio_val);

    app_anc_timer_set(ANC_EVENT_SWITCH_KEY_DEBONCE, anc_switch_key_debonce_time);
    //if (app_switch_callback)
    //    app_switch_callback(gpio_val);
    // app_anc_key_handler();
}

void app_anc_switch_init(ANC_KEY_CALLBACK callback)
{
    app_switch_callback = callback;
}
#endif

#ifdef ANC_SWITCH_PIN
bool app_anc_set_reboot(void)
{
    return (app_anc_shutdown && hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)ANC_SWITCH_PIN));
}

bool app_anc_switch_get_val(void)
{
    return (bool)hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)ANC_SWITCH_PIN);
}

void app_anc_switch_set_edge(uint8_t down_edge)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;
    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_polarity = (down_edge == 1) ? HAL_GPIO_IRQ_POLARITY_LOW_FALLING : HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    gpiocfg.irq_handler = app_anc_switch_int_handler;
    TRACE(" %s :%d ", __func__, down_edge);
    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)ANC_SWITCH_PIN, &gpiocfg);
}
#endif

int __anc_usb_app_fadein(enum ANC_TYPE_T type)
{
    int32_t gain0_curr, gain1_curr;
    int32_t gain0_tg, gain1_tg;

    anc_get_gain(&gain0_curr, &gain1_curr, type);
    anc_get_cfg_gain(&gain0_tg, &gain1_tg, type);

    if (gain0_tg > 0)
    {
        if (gain0_curr < gain0_tg)
        {
            gain0_curr++;
        }
    }
    else
    {
        if (gain0_curr > gain0_tg)
        {
            gain0_curr--;
        }
    }

    if (gain1_tg > 0)
    {
        if (gain1_curr < gain1_tg)
        {
            gain1_curr++;
        }
    }
    else
    {
        if (gain1_curr > gain1_tg)
        {
            gain1_curr--;
        }
    }

    anc_set_gain(gain0_curr, gain1_curr, type);

    if ((gain0_curr == gain0_tg) && (gain1_curr == gain1_tg))
    {
        TRACE("[%s] cur gain: %d %d", __func__, gain0_curr, gain1_curr);
        return 0;
    }

    return 1;
}

int __anc_usb_app_fadeout(enum ANC_TYPE_T type)
{
    int32_t gain0_curr, gain1_curr;

    anc_get_gain(&gain0_curr, &gain1_curr, type);

    if (gain0_curr > 0)
    {
        gain0_curr--;
    }
    else if (gain0_curr < 0)
    {
        gain0_curr++;
    }

    if (gain1_curr > 0)
    {
        gain1_curr--;
    }
    else if (gain1_curr < 0)
    {
        gain1_curr++;
    }

    //  gain0_curr = gain1_curr = 0 ;

    anc_set_gain(gain0_curr, gain1_curr, type);

    if ((gain0_curr == 0) && (gain1_curr == 0))
    {
        TRACE("[%s] gain: %d, %d", __func__, gain0_curr, gain1_curr);
        return 0;
    }

    return 1;
}

void anc_gain_fade_handle(void)
{
    TRACE(" %s %d ", __func__, app_anc_fade_status);
    if (app_anc_fade_status == APP_ANC_FADE_OUT)
    {
#ifdef ANC_FF_ENABLED
        while (__anc_usb_app_fadeout(ANC_FEEDFORWARD))
            ;
#endif
#ifdef ANC_FB_ENABLED
        while (__anc_usb_app_fadeout(ANC_FEEDBACK))
            ;
#endif

        app_anc_disable();
    }

    if (app_anc_fade_status == APP_ANC_FADE_IN)
    {
#ifdef ANC_LED_PIN
        app_anc_switch_turnled(true);
#endif

#ifdef ANC_FF_ENABLED
        while (__anc_usb_app_fadein(ANC_FEEDFORWARD))
            ;
#endif
#ifdef ANC_FB_ENABLED
        while (__anc_usb_app_fadein(ANC_FEEDBACK))
            ;
#endif

        if (anc_set_dac_pa_delay)
        {
            anc_set_dac_pa_delay = false;
//          osDelay(600);
//          analog_aud_enable_dac_pa(48);
        }
    }
    app_anc_fade_status = APP_ANC_IDLE;
}
void app_anc_gain_fadein(void)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s ", __func__);
    if (app_poweroff_flag)
        return;

    app_anc_fade_status = APP_ANC_FADE_IN;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_FADE_IN;
    app_mailbox_put(&msg);
}

void app_anc_post_anc_codec_close(void)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s ", __func__);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = SIMPLE_PLAYER_CLOSE_CODEC_EVT;
    app_mailbox_put(&msg);
}

void app_anc_gain_fadeout(void)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s ", __func__);
    if (app_poweroff_flag)
        return;

    app_anc_fade_status = APP_ANC_FADE_OUT;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_FADE_OUT;
    app_mailbox_put(&msg);
}

void app_anc_status_post(uint8_t status)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s status", __func__, status);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_CHANGE_STATUS;
    msg.msg_body.message_Param0 = status;
    app_mailbox_put(&msg);
}

void app_anc_status_sync(uint8_t status)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s status", __func__, status);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_SYNC_STATUS;
    msg.msg_body.message_Param0 = status;
    app_mailbox_put(&msg);
}

void app_anc_status_sync_init(void)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s ", __func__);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_SYNC_INIT;
    app_mailbox_put(&msg);
}

void app_anc_do_init(void)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s DO INIT", __func__);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_INIT;
    app_mailbox_put(&msg);
}

void app_anc_send_howl_evt(uint32_t howl)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s %d", __func__, howl);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_HOWL_PROCESS;
    msg.msg_body.message_Param0 = howl;
    app_mailbox_put(&msg);
}

void app_anc_send_pwr_key_monitor_evt(uint8_t level)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s %d", __func__, level);
    if (app_poweroff_flag)
        return;

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = ANC_EVENT_PWR_KEY_MONITOR;
    msg.msg_body.message_Param0 = level;
    app_mailbox_put(&msg);
}

extern void simple_player_delay_stop(void);

void app_anc_post_simplayer_stop_evt(void)
{
    APP_MESSAGE_BLOCK msg;
    TRACE(" %s d", __func__);

    msg.mod_id = APP_MODUAL_ANC;
    msg.msg_body.message_id = SIMPLE_PLAYER_DELAY_STOP_EVT;
    msg.msg_body.message_Param0 = 0;
    app_mailbox_put(&msg);
}

bool anc_enabled(void)
{
    return (anc_work_status == ANC_STATUS_ON);
}
//extern void anc_active_codec();

void app_anc_resample(uint32_t res_ratio, uint32_t *in, uint32_t *out, uint32_t samples)
{
    uint32_t flag = int_lock();
    for (int i = samples; i > 0; i--)
    {
        for (int j = 0; j < res_ratio; j++)
        {
            *(out + (i - 1) * res_ratio + j) = *(in + i - 1);
        }
    }
    int_unlock(flag);
}

void app_anc_enable(void)
{
    TRACE("enter %s %d\n", __FUNCTION__, __LINE__);
//anc_active_codec();
#ifndef __SIMPLE_INTERNAL_PLAYER_SUPPORT__
    analog_aud_codec_speaker_enable(true);
#endif
#ifdef ANC_FF_ENABLED
    anc_open(ANC_FEEDFORWARD);
    anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx, ANC_FEEDFORWARD, ANC_GAIN_DELAY);
#endif

#ifdef ANC_FB_ENABLED
    anc_open(ANC_FEEDBACK);
    anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx, ANC_FEEDBACK, ANC_GAIN_DELAY);
#endif

    TRACE("exit %s %d\n", __FUNCTION__, __LINE__);
}

void app_anc_disable(void)
{
    TRACE("enter %s %d\n", __FUNCTION__, __LINE__);
#ifdef ANC_FF_ENABLED
    anc_close(ANC_FEEDFORWARD);
#endif
#ifdef ANC_FB_ENABLED
    anc_close(ANC_FEEDBACK);
#endif

    TRACE("exit %s %d\n", __FUNCTION__, __LINE__);
}

static void anc_sample_rate_change(enum AUD_STREAM_T stream, enum AUD_SAMPRATE_T rate, enum AUD_SAMPRATE_T *new_play, enum AUD_SAMPRATE_T *new_cap)
{
    enum AUD_SAMPRATE_T play_rate, cap_rate;

    if (anc_sample_rate[stream] != rate) {
#ifdef CHIP_BEST1000
        if (stream == AUD_STREAM_PLAYBACK) {
            play_rate = rate;
            cap_rate = rate * (anc_sample_rate[AUD_STREAM_CAPTURE] / anc_sample_rate[AUD_STREAM_PLAYBACK]);
        } else {
            play_rate = rate / (anc_sample_rate[AUD_STREAM_CAPTURE] / anc_sample_rate[AUD_STREAM_PLAYBACK]);
            cap_rate = rate;
        }
#else
        play_rate = rate;
        cap_rate = rate;
#ifdef ANC_FF_ENABLED
        anc_select_coef(play_rate,anc_coef_idx, ANC_FEEDFORWARD, ANC_GAIN_NO_DELAY);
#endif

#ifdef ANC_FB_ENABLED
        anc_select_coef(play_rate,anc_coef_idx, ANC_FEEDBACK, ANC_GAIN_NO_DELAY);
#endif

#endif //CHIP_BEST1000

        TRACE("%s: Update anc sample rate from %u/%u to %u/%u", __func__,
            anc_sample_rate[AUD_STREAM_PLAYBACK], anc_sample_rate[AUD_STREAM_CAPTURE], play_rate, cap_rate);

        if (new_play) {
            *new_play= play_rate;
        }
        if (new_cap) {
            *new_cap = cap_rate;
        }

        anc_sample_rate[AUD_STREAM_PLAYBACK] = play_rate;
        anc_sample_rate[AUD_STREAM_CAPTURE] = cap_rate;
    }
}

void app_anc_open_anc(void)
{
    enum AUD_SAMPRATE_T playback_rate;
    enum AUD_SAMPRATE_T capture_rate;
    AF_ANC_HANDLER handler;

    TRACE("enter %s %d\n", __FUNCTION__, __LINE__);

    handler = anc_sample_rate_change;

#ifdef __SIMPLE_INTERNAL_PLAYER_SUPPORT__
    app_start_player_by_anc();
#endif

    if (anc_sample_rate[AUD_STREAM_PLAYBACK] == AUD_SAMPRATE_NULL) {
#ifdef CHIP_BEST1000
        anc_sample_rate[AUD_STREAM_PLAYBACK] = AUD_SAMPRATE_96000;
        anc_sample_rate[AUD_STREAM_CAPTURE] = AUD_SAMPRATE_384000;
#else // !CHIP_BEST1000
        anc_sample_rate[AUD_STREAM_PLAYBACK] = AUD_SAMPRATE_48000;
        anc_sample_rate[AUD_STREAM_CAPTURE] = AUD_SAMPRATE_48000;
#endif
        anc_sample_rate[AUD_STREAM_PLAYBACK] = hal_codec_anc_convert_rate(anc_sample_rate[AUD_STREAM_PLAYBACK]);
        anc_sample_rate[AUD_STREAM_CAPTURE] = hal_codec_anc_convert_rate(anc_sample_rate[AUD_STREAM_CAPTURE]);
    }

    playback_rate = anc_sample_rate[AUD_STREAM_PLAYBACK];
    capture_rate = anc_sample_rate[AUD_STREAM_CAPTURE];

    pmu_anc_config(1);
    
#ifdef ANC_FF_ENABLED
    af_anc_open(ANC_FEEDFORWARD, playback_rate, capture_rate, handler);
    anc_open(ANC_FEEDFORWARD);
#endif

#ifdef ANC_FB_ENABLED
    af_anc_open(ANC_FEEDBACK, playback_rate, capture_rate, handler);
    anc_open(ANC_FEEDBACK);
#endif

    TRACE("exit %s %d\n", __FUNCTION__, __LINE__);
}

void app_anc_close_anc(void)
{
    TRACE("enter %s %d\n", __FUNCTION__, __LINE__);
    anc_set_dac_pa_delay = false;
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    if (anc_single_mode_is_on())
    {
        analog_aud_codec_speaker_enable(false);
    }
#endif

#ifdef ANC_FF_ENABLED
    anc_close(ANC_FEEDFORWARD);
    af_anc_close(ANC_FEEDFORWARD);
#endif

#ifdef ANC_FB_ENABLED
    anc_close(ANC_FEEDBACK);
    af_anc_close(ANC_FEEDBACK);
#endif

    pmu_anc_config(0);
#ifdef __SIMPLE_INTERNAL_PLAYER_SUPPORT__
    app_stop_player_by_anc();
#endif
    TRACE("exit %s %d\n", __FUNCTION__, __LINE__);
}

void app_anc_bitrate_reopen(void)
{
    TRACE(" %s status %d", __func__, anc_work_status);
    if (anc_work_status == ANC_STATUS_INIT_ON)
    {
//        hal_codec_reconfig_pll_freq(playback_rate, capture_rate);
    }
    else if (anc_work_status == ANC_STATUS_ON)
    {
    //    hal_codec_reconfig_pll_freq(playback_rate, capture_rate);
    }
    else if (anc_work_status == ANC_STATUS_WAITING_ON)
    {
    }
    else
    {
        TRACE("no deal situation.");
    }
}

void app_anc_status_change(void)
{
    TRACE(" %s status: %d ", __func__, anc_work_status);
    switch (anc_work_status)
    {
    case ANC_STATUS_OFF:
        anc_work_status = ANC_STATUS_WAITING_ON;
        app_anc_timer_set(ANC_EVENT_OPEN, anc_switch_on_time);
        app_anc_open_anc();
        app_anc_enable();
        break;
    case ANC_STATUS_ON:

#if (ANC_COEF_LIST_NUM > 1)      

        if (++anc_coef_idx < ANC_COEF_LIST_NUM)
        {
            TRACE(" %s set coef idx: %d ", __func__, anc_coef_idx);

#ifdef ANC_MODE_SWITCH_WITHOUT_FADE

#ifdef ANC_FF_ENABLED
           anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx,ANC_FEEDFORWARD,ANC_GAIN_NO_DELAY);
#endif
#ifdef ANC_FB_ENABLED
           anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx,ANC_FEEDBACK,ANC_GAIN_NO_DELAY);
#endif

#else //ANC_MODE_SWITCH_WITHOUT_FADE

#ifdef ANC_FF_ENABLED
            while (__anc_usb_app_fadeout(ANC_FEEDFORWARD));
            anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx,ANC_FEEDFORWARD,ANC_GAIN_DELAY);
            
#endif
#ifdef ANC_FB_ENABLED
            while (__anc_usb_app_fadeout(ANC_FEEDBACK));
            anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx,ANC_FEEDBACK,ANC_GAIN_DELAY);
#endif

#ifdef ANC_FF_ENABLED
            while (__anc_usb_app_fadein(ANC_FEEDFORWARD));
#endif

#ifdef ANC_FB_ENABLED
            while (__anc_usb_app_fadein(ANC_FEEDBACK));
#endif

#endif //ANC_MODE_SWITCH_WITHOUT_FADE
        }
        else
        {
            anc_coef_idx = 0;
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_close_delay_time);
            app_anc_gain_fadeout();
            anc_work_status = ANC_STATUS_WAITING_OFF;
        }
#else
        app_anc_timer_set(ANC_EVENT_CLOSE, anc_close_delay_time);
        app_anc_gain_fadeout();
        anc_work_status = ANC_STATUS_WAITING_OFF;
#endif
        break;
    case ANC_STATUS_INIT_ON:
        app_anc_enable();
        app_anc_gain_fadein();
        anc_work_status = ANC_STATUS_ON;
        app_anc_timer_set(ANC_EVENT_CLOSE, anc_auto_power_off_time);
        break;
    default:
        break;
    };
}

void app_anc_change_to_status(uint32_t status)
{
    TRACE(" %s status: %d ", __func__, anc_work_status);
    switch (status)
    {
    case ANC_STATUS_OFF:
        switch (anc_work_status)
        {
        case ANC_STATUS_OFF:
            break;
        case ANC_STATUS_ON:
        case ANC_STATUS_WAITING_ON:
        case ANC_STATUS_WAITING_OFF:
        case ANC_STATUS_INIT_ON:
        default:
            app_anc_timer_set(ANC_EVENT_CLOSE, 100);
            app_anc_gain_fadeout();
            anc_work_status = ANC_STATUS_WAITING_OFF;
            break;
        }
        break;
    case ANC_STATUS_ON:
        switch (anc_work_status)
        {
        case ANC_STATUS_ON:
            break;
        case ANC_STATUS_WAITING_ON:
            break;
        case ANC_STATUS_OFF:
            anc_work_status = ANC_STATUS_WAITING_ON;
            app_anc_timer_set(ANC_EVENT_OPEN, anc_switch_on_time);
            app_anc_open_anc();
            app_anc_enable();
            break;
        case ANC_STATUS_WAITING_OFF:
            app_anc_timer_set(ANC_EVENT_OPEN, 100);
            app_anc_gain_fadein();
            anc_work_status = ANC_STATUS_WAITING_ON;
            break;
        case ANC_STATUS_INIT_ON:
            app_anc_enable();
            app_anc_gain_fadein();
            anc_work_status = ANC_STATUS_ON;
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_auto_power_off_time);
            break;
        default:
            break;
        }
        break;
    case ANC_STATUS_INIT_ON:
        switch (anc_work_status)
        {
        case ANC_STATUS_OFF:
            app_anc_open_anc();
            anc_work_status = ANC_STATUS_INIT_ON;
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_init_switch_off_time);
            break;
        case ANC_STATUS_ON:
            app_anc_gain_fadeout();
            anc_work_status = ANC_STATUS_INIT_ON;
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_init_switch_off_time);
            app_anc_disable();
            break;
        case ANC_STATUS_WAITING_ON:
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_init_switch_off_time);
            anc_work_status = ANC_STATUS_INIT_ON;
            app_anc_disable();
            break;
        case ANC_STATUS_WAITING_OFF:
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_init_switch_off_time);
            anc_work_status = ANC_STATUS_INIT_ON;
            break;
        case ANC_STATUS_INIT_ON:
            break;
        default:
            break;
        }
        break;
    case ANC_STATUS_WAITING_OFF:
        switch (anc_work_status)
        {
        case ANC_STATUS_OFF:
            break;
        case ANC_STATUS_ON:
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_close_delay_time);
            app_anc_gain_fadeout();
            anc_work_status = ANC_STATUS_WAITING_OFF;
            break;
        case ANC_STATUS_WAITING_ON:
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_close_delay_time);
            app_anc_gain_fadeout();
            anc_work_status = ANC_STATUS_WAITING_OFF;
            break;
        case ANC_STATUS_WAITING_OFF:
            break;
        case ANC_STATUS_INIT_ON:
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_close_delay_time);
            app_anc_gain_fadeout();
            anc_work_status = ANC_STATUS_WAITING_OFF;
            break;
        default:
            break;
        }

        break;
    case ANC_STATUS_WAITING_ON:
        switch (anc_work_status)
        {
        case ANC_STATUS_OFF:
            anc_work_status = ANC_STATUS_WAITING_ON;
            app_anc_timer_set(ANC_EVENT_OPEN, anc_switch_on_time);
            app_anc_open_anc();
            app_anc_enable();
            break;
        case ANC_STATUS_ON:
            break;
        case ANC_STATUS_WAITING_ON:
            break;
        case ANC_STATUS_WAITING_OFF:
            app_anc_timer_set(ANC_EVENT_OPEN, anc_switch_on_time);
            app_anc_gain_fadein();
            break;
        case ANC_STATUS_INIT_ON:
            app_anc_enable();
            app_anc_gain_fadein();
            anc_work_status = ANC_STATUS_ON;
            app_anc_timer_set(ANC_EVENT_CLOSE, anc_auto_power_off_time);
            break;
        default:
            break;
        }

        break;
    default:
        break;
    };
}

static int app_anc_handle_process(APP_MESSAGE_BODY *msg_body)
{
    uint32_t evt = msg_body->message_id;
    uint32_t arg0 = msg_body->message_Param0;

    TRACE(" %s evt: %d, arg0: %d , anc status :%d", __func__, evt, arg0, anc_work_status);

    switch (evt)
    {
    case ANC_EVENT_INIT:
        //init anc_timer
        app_anc_init_timer();
        //init anc&open codec
#ifdef __AUDIO_SECTION_SUPPT__
        anc_load_cfg();
#endif 

#ifdef __ANC_INIT_ON__
        app_anc_open_anc();
        anc_work_status = ANC_STATUS_INIT_ON;
        app_anc_timer_set(ANC_EVENT_CLOSE, anc_init_switch_off_time);
#else
        anc_work_status = ANC_STATUS_OFF;
#endif
#ifdef __ANC_STICK_SWITCH_USE_GPIO__
        if (app_anc_switch_get_val() == ANC_SWITCH_ON_LEVEL)
        {
            app_anc_switch_set_edge(ANC_SWITCH_ON_LEVEL);
            app_anc_status_post(ANC_SWITCH_ON_LEVEL);
        }
        else
        {
            app_anc_switch_set_edge(!(bool)ANC_SWITCH_ON_LEVEL);
        }
#endif
        //anc_coef_idx = 0;
        break;
    case ANC_EVENT_FADE_IN:
    case ANC_EVENT_FADE_OUT:
        anc_gain_fade_handle();
        if (evt == ANC_EVENT_FADE_IN)
            anc_work_status = ANC_STATUS_ON;
        if (evt == ANC_EVENT_FADE_OUT)
        {
            anc_work_status = ANC_STATUS_INIT_ON;
        }
        break;
    case ANC_EVENT_CHANGE_SAMPLERATE:
        app_anc_bitrate_reopen();
        break;
    case ANC_EVENT_CHANGE_STATUS:

#ifdef __ANC_STICK_SWITCH_USE_GPIO__
        osDelay(10);
        if (app_anc_switch_get_val() != (bool)arg0)
        { //debonce
            TRACE("io level not equeue exit");
            break;
        }

#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
        if (anc_single_mode_is_on())
        {
            if (arg0 != ANC_SWITCH_ON_LEVEL)
            {
                TRACE(" Anc ON but exit %d ", app_init_done);
                if (app_init_done)
                {
                    app_anc_shutdown = true;
                    app_shutdown();
                }
                else
                {
                    app_anc_timer_set(ANC_EVENT_SWITCH_KEY_DEBONCE, anc_switch_key_debonce_time);
                }
                return 0;
            }
        }
#endif
        if (arg0 != ANC_SWITCH_ON_LEVEL)
            app_anc_switch_set_edge((!(bool)ANC_SWITCH_ON_LEVEL));
        else
            app_anc_switch_set_edge(ANC_SWITCH_ON_LEVEL);

        if (((arg0 == ANC_SWITCH_ON_LEVEL) && (anc_work_status == ANC_STATUS_ON)) ||
            ((arg0 != ANC_SWITCH_ON_LEVEL) && (anc_work_status == ANC_STATUS_OFF)))
        {
            //status same, no handle
            TRACE(" Anc NOT ON, exit");
            return 0;
        }
#endif
        app_anc_status_change();
        break;
    case ANC_EVENT_SYNC_STATUS:
#ifdef __TWS__
        TRACE(" %s status %d ---> %d ", __func__, anc_work_status, arg0);
        app_anc_change_to_status(arg0);
#endif
        break;
    case ANC_EVENT_SYNC_INIT:
#ifdef __TWS__
        anc_status_sync_init();
#endif
        break;
    case ANC_EVENT_HOWL_PROCESS:
        //disable anc,set max gain,enable anc
        {
            //uint32_t howl = arg0;
            if (anc_work_status == ANC_STATUS_ON)
            {
                //__anc_usb_app_howl_set_gain(howl);
            }
        }
        break;
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    case ANC_EVENT_PWR_KEY_MONITOR:
        osDelay(10);

        if (app_pwr_key_monitor_get_val() != (bool)arg0) //debonce
            return 0;

        if ((bool)arg0)
        {
            app_anc_timer_set(ANC_EVENT_PWR_KEY_MONITOR_REBOOT, anc_pwr_key_monitor_time);
            app_pwr_key_monitor_set_int_edge(1);
        }
        else
        {
            hwtimer_stop(anc_timerid);
            app_pwr_key_monitor_set_int_edge(0);
        }

        break;
#endif
    case SIMPLE_PLAYER_DELAY_STOP_EVT:
#ifdef __SIMPLE_INTERNAL_PLAYER_SUPPORT__
        simple_player_delay_stop();
#endif
        break;
    case SIMPLE_PLAYER_CLOSE_CODEC_EVT:
        app_anc_close_anc();
        anc_work_status = ANC_STATUS_OFF;

        break;
    default:
        break;
    }
    return 0;
}

static void anc_gain_fade_timer_handler(void *p)
{
    TRACE(" %s , request %d ", __func__, anc_timer_request);
    switch (anc_timer_request)
    {
    case ANC_EVENT_OPEN:
        app_anc_gain_fadein();
        break;
    case ANC_EVENT_CLOSE:
        //real close
        //app_anc_close_anc();
        //anc_work_status = ANC_STATUS_OFF;
        app_anc_post_anc_codec_close();

        break;
    case ANC_EVENT_PWR_KEY_MONITOR_REBOOT:
        TRACE(" reboot !");
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT_ANC_ON);
        hal_sw_bootmode_set(HAL_SW_BOOTMODE_REBOOT_BT_ON);
        app_reset();
        break;
    case ANC_EVENT_SWITCH_KEY_DEBONCE:
        TRACE(" ANC SWITCH KEY ");
#ifdef ANC_LED_PIN
        if (app_switch_callback)
            app_switch_callback(hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)ANC_SWITCH_PIN));
#endif
        break;
    default:
        break;
    }
    anc_timer_request = ANC_EVENT_NONE;
    hwtimer_stop(anc_timerid);
}

void app_anc_init_timer(void)
{
    if (anc_timerid == NULL)
        anc_timerid = hwtimer_alloc((HWTIMER_CALLBACK_T)anc_gain_fade_timer_handler, &anc_timer_request);
}

void app_anc_timer_set(uint32_t request, uint32_t delay)
{
    TRACE(" %s request %d , delay %ds ", __func__, request, (TICKS_TO_MS(delay) / 1000));
    if (anc_timerid == NULL)
        return;
    anc_timer_request = request;
    hwtimer_stop(anc_timerid);
    hwtimer_start(anc_timerid, delay);
}

void app_anc_key(APP_KEY_STATUS *status, void *param)
{
    TRACE(" %s anc_work_st: %d , timer %d", __func__, anc_work_status, anc_timer_request);
    app_anc_status_change();
#ifdef __TWS__
    anc_status_sync();
#endif
}

void app_anc_ios_init(void)
{
#ifdef __ANC_STICK_SWITCH_USE_GPIO__
    if (cfg_anc_switch.pin != HAL_IOMUX_PIN_NUM)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_anc_switch, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_anc_switch.pin, HAL_GPIO_DIR_IN, 0);
    }
#endif
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    if (cfg_pwr_key_monitor.pin != HAL_IOMUX_PIN_NUM)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_pwr_key_monitor, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_pwr_key_monitor.pin, HAL_GPIO_DIR_IN, 0);
    }
    if (cfg_anc_led.pin != HAL_IOMUX_PIN_NUM)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_anc_led, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_anc_led.pin, HAL_GPIO_DIR_OUT, 0);
    }

#endif
#ifdef ANC_LED_PIN
    uint8_t gpio_val = hal_gpio_pin_get_val(ANC_SWITCH_PIN);
    if (gpio_val)
    {
        app_anc_switch_turnled(true);
        TRACE(" %s turn on led", __func__);
    }
    else
    {
        app_anc_switch_turnled(false);
        TRACE(" %s turn off led", __func__);
    }
#endif
}

int app_anc_open_module(void)
{
    TRACE("%s ", __func__);
    //set app module
    app_set_threadhandle(APP_MODUAL_ANC, app_anc_handle_process);
    app_anc_do_init();
#ifdef __ANC_STICK_SWITCH_USE_GPIO__
    app_anc_switch_init(app_anc_status_post);
    app_anc_switch_set_edge(app_anc_switch_get_val());
#endif
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    if (anc_single_mode_is_on())
    {
        // app_pwr_key_monitor_init(app_anc_send_pwr_key_monitor_evt);
        //  app_pwr_key_monitor_set_int_edge(app_pwr_key_monitor_get_val());
    }
#endif

    return 0;
}

int app_anc_close_module(void)
{
    TRACE(" %s ", __func__);

    anc_timer_request = ANC_EVENT_NONE;
    if (anc_timerid)
    {
        hwtimer_stop(anc_timerid);
        hwtimer_free(anc_timerid);
        anc_timerid = NULL;
    }
    if (app_anc_get_anc_status() != ANC_STATUS_OFF)
    {
        anc_work_status = ANC_STATUS_OFF;
        app_anc_disable();
        app_anc_close_anc();
    }
    app_set_threadhandle(APP_MODUAL_ANC, NULL);
    return 0;
}

enum AUD_SAMPRATE_T app_anc_get_play_rate(void)
{
    return anc_sample_rate[AUD_STREAM_PLAYBACK];
}

bool app_anc_work_status(void)
{
   // TRACE(" %s st %d", __func__, anc_work_status);
    return (anc_work_status == ANC_STATUS_ON || anc_work_status == ANC_STATUS_WAITING_ON);
}

uint32_t app_anc_get_anc_status(void)
{
    return anc_work_status;
}

void test_anc(void)
{
    app_anc_open_anc();
    app_anc_enable();
}

void test_anc_switch_coef(void)
{
    anc_coef_idx++;

    TRACE(" %s set coef idx: %d ", __func__, anc_coef_idx);

    if (ANC_STATUS_OFF == anc_work_status || ANC_STATUS_INIT_ON == anc_work_status)
    {
        app_anc_status_change();
        anc_coef_idx = 0;
        return;
    }
    if (anc_coef_idx < (ANC_COEF_LIST_NUM))
    {
#ifdef ANC_FF_ENABLED
        while (__anc_usb_app_fadeout(ANC_FEEDFORWARD))
            ;
#endif
#ifdef ANC_FB_ENABLED
        while (__anc_usb_app_fadeout(ANC_FEEDBACK))
            ;
#endif

        app_anc_disable();
        anc_select_coef(anc_sample_rate[AUD_STREAM_PLAYBACK],anc_coef_idx,ANC_FEEDFORWARD,ANC_GAIN_DELAY);
        app_anc_enable();
        
#ifdef ANC_FF_ENABLED
        while (__anc_usb_app_fadein(ANC_FEEDFORWARD))
            ;
#endif
#ifdef ANC_FB_ENABLED
        while (__anc_usb_app_fadein(ANC_FEEDBACK))
            ;
#endif
    }
    else
    {
        anc_coef_idx = 0;
        app_anc_timer_set(ANC_EVENT_CLOSE, anc_close_delay_time);
        app_anc_gain_fadeout();
        anc_work_status = ANC_STATUS_WAITING_OFF;
    }
}
