/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：分立LED灯管理模块，包括初始化，设置，以及闪烁定时器回调函数；分立LED灯
 *       支持呼吸灯模式。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

const discrete_led_config_t discrete_led_cfg[LED_ID_MAX] =
{
    { LED_BL_LINK_PORT, LED_BL_LINK_PIN },
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    { LED_MUSIC_PLAY_PORT, LED_MUSIC_PLAY_PIN },
    { LED_AUX_PLAY_PORT, LED_AUX_PLAY_PIN },
    { LED_FM_PLAY_PORT, LED_FM_PLAY_PIN },
    { LED_POWER_PORT, LED_POWER_PIN },
#endif
};

void discrete_led_twinkle(void);

//初始化分立LED灯
void discrete_led_init(void)
{
    uint8 n;

    g_discrete_led_a23_available = ((uint8) com_get_config_default(SETTING_UART_PRINT_ENABLE) == 0);

    for (n = 0; n < LED_ID_MAX; n++)
    {
        //将timer_id都设置为-1
        //因为，例如：两次调用discrete_led_set, 第2次应该把第一次用的定时器关掉。
        //但第一次定时器是否在用，得有个标记(-1表示没有在用，其他表示在用)
        g_discrete_led_info[n].timer_id = -1;

        g_discrete_led_info[n].led_port = discrete_led_cfg[n].port;
        g_discrete_led_info[n].led_pin = discrete_led_cfg[n].pin;
        g_discrete_led_info[n].lightness = 0; //暂时不用
        g_discrete_led_info[n].work_mode = LED_ALWAYS_OFF;
        g_discrete_led_info[n].on_off = FALSE;

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        if ((g_discrete_led_a23_available == FALSE) && (n == LED_ID_POWER))
        { //如果A23被UART TX占用，就不要初始化之
            continue;
        }
#endif
        key_discrete_led_init(discrete_led_cfg[n].port, discrete_led_cfg[n].pin);
    }
}

//设置分立LED灯，开/关/闪烁效果
void discrete_led_set(led_id_e id, led_mode_e mode, led_duty_t *duty_param)
{
    if (id >= LED_ID_MAX)
    {
        return;
    }

    if ((mode == LED_TWINKLE) && (duty_param == NULL))
    {
        mode = LED_ALWAYS_ON;
    }

    g_discrete_led_info[id].work_mode = mode;

    if (g_discrete_led_info[id].timer_id != -1)
    {
        kill_app_timer_safe(g_discrete_led_info[id].timer_id, discrete_led_twinkle);
        g_discrete_led_info[id].timer_id = -1;
    }


    if (mode == LED_ALWAYS_OFF)
    {
        if (g_discrete_led_info[id].on_off == TRUE)
        {
            //灭掉LED灯
            g_discrete_led_info[id].on_off = FALSE;
            key_discrete_led_off(g_discrete_led_info[id].led_port, g_discrete_led_info[id].led_pin);
        }
    }
    else
    {
        if (g_discrete_led_info[id].on_off == FALSE)
        {
            //点亮LED灯
            g_discrete_led_info[id].on_off = TRUE;
            key_discrete_led_on(g_discrete_led_info[id].led_port, g_discrete_led_info[id].led_pin);
        }

        if (mode == LED_TWINKLE)
        {
            twinkle_app_timer_param_t twinkle_param;

            twinkle_param.count = duty_param->cycle_count;
            twinkle_param.on_off = TRUE;
            twinkle_param.timeout_on = duty_param->duty_on_time;
            twinkle_param.timeout_off = duty_param->duty_off_time;
            twinkle_param.func_proc = discrete_led_twinkle;

            g_discrete_led_info[id].timer_id = set_twinkle_app_timer(
                    APP_TIMER_ATTRB_COM_CONTROL | (APP_TIMER_TAG_SYS << 8),
                    &twinkle_param);
        }
    }
}

//进低功耗灭掉所有LED灯
void discrete_led_low_power(void)
{
    uint8 n;

    g_discrete_led_a23_available = ((uint8) com_get_config_default(SETTING_UART_PRINT_ENABLE) == 0);

    for (n = 0; n < LED_ID_MAX; n++)
    {
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        if ((g_discrete_led_a23_available == FALSE) && (n == LED_ID_POWER))
        { //如果A23被UART TX占用，就不要初始化之
            continue;
        }
#endif

        if (g_discrete_led_info[n].on_off == TRUE)
        {
            key_discrete_led_off(g_discrete_led_info[n].led_port, g_discrete_led_info[n].led_pin);
        }
    }
}

//退出低功耗恢复LED灯
void discrete_led_restore(void)
{
    uint8 n;

    g_discrete_led_a23_available = ((uint8) com_get_config_default(SETTING_UART_PRINT_ENABLE) == 0);

    for (n = 0; n < LED_ID_MAX; n++)
    {
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        if ((g_discrete_led_a23_available == FALSE) && (n == LED_ID_POWER))
        { //如果A23被UART TX占用，就不要初始化之
            continue;
        }
#endif

        if (g_discrete_led_info[n].on_off == TRUE)
        {
            key_discrete_led_on(g_discrete_led_info[n].led_port, g_discrete_led_info[n].led_pin);
        }
    }
}

//分立led灯闪烁效果
void __section__(".text.led_twinkle") discrete_led_twinkle(void)
{
    uint8 i;

    for (i = 0; i < LED_ID_MAX; i++)
    {
        if ((g_discrete_led_info[i].timer_id & 0x3f) == g_app_timer_id) //COMMON软定时器ID只有低6bit可用
        {
            break;
        }
    }

    if (i == LED_ID_MAX)
    {
        //这种情况不应该发生
        return;
    }

    if (g_discrete_led_info[i].on_off == TRUE)
    {
        //灭掉LED灯
        g_discrete_led_info[i].on_off = FALSE;
        key_discrete_led_off(g_discrete_led_info[i].led_port, g_discrete_led_info[i].led_pin);
    }
    else
    {
        //亮LED灯
        g_discrete_led_info[i].on_off = TRUE;
        key_discrete_led_on(g_discrete_led_info[i].led_port, g_discrete_led_info[i].led_pin);
    }
}

