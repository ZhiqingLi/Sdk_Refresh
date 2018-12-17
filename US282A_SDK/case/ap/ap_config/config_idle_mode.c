/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：外部充电方案的空闲模式，空闲模式即除了插入USB上电进入读卡器模式，以及开关
 *       为ON状态时的正常工作模式外的模式，在该模式中除了检测到开关拨到ON状态切换到
 *       正常工作模式之外，不响应任何其他事件。
 * 作者：cailizhen
 ********************************************************************************/

#include  "ap_config.h"

/******************************************************************************/
/*!
 * \par  Description:
 *  外部充电方案的空闲模式的处理
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
app_result_e config_idle_mode_loop(void)
{
    app_result_e result = RESULT_NULL;
    cable_state_e cur_cable_state, last_cable_state;

    //进入空闲模式
    set_cur_func_id(APP_FUNC_IDLE_MODE);

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    if ((g_discrete_led_a23_available == TRUE) && (g_app_info_state.charge_state == BAT_CHARGING))
    {
        discrete_led_set(LED_ID_POWER, LED_ALWAYS_OFF, NULL);
        discrete_led_set(LED_ID_POWER, LED_ALWAYS_ON, NULL);
    }
#endif

    last_cable_state = get_cable_state();

    while (1)
    {
        com_view_loop();

        //检测到硬开关拨到ON状态，进入工作状态
        if (IS_EXTERN_BATTERY_IN() == 1)
        {
            result = RESULT_NULL;
            break;
        }

        //检测到USB线插入，进入读卡器模式
        cur_cable_state = get_cable_state();
        if ((last_cable_state == CABLE_STATE_CABLE_NULL) && (cur_cable_state == CABLE_STATE_CABLE_IN))
        {
            result = RESULT_USB_TRANS;
            break;
        }
        last_cable_state = cur_cable_state;

        //挂起10ms，多任务调度
        sys_os_time_dly(1);
    }

    return result;
}

