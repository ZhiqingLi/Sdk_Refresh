/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 充电和电量管理模块，包括电量等级表配置，充电和电量监测HANDLE，需要
 *       挂载在2Hz软定时器运行。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

//充电与电量发生改变时需要处理
//返回TRUE表示处理完毕，本次检测处理已OK；返回FALSE表示继续后续计时处理
void __section__(".bank") com_battery_charge_change_deal(uint8 cur_charge_state, uint8 cur_bat_val)
{
    led_duty_t pw_link_duty;
    uint16 msg;

    if ((cur_charge_state != BAT_CHARGING) && (g_app_info_state.charge_state == BAT_CHARGING))
    {
        //关掉充电指示
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        if (g_discrete_led_a23_available == TRUE)
        {
            discrete_led_set(LED_ID_POWER, LED_ALWAYS_OFF, NULL);
        }
#endif
    }
    else if ((cur_charge_state == BAT_CHARGING) && (g_app_info_state.charge_state != BAT_CHARGING))
    {
        //开启充电指示
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        if (g_discrete_led_a23_available == TRUE)
        {
            discrete_led_set(LED_ID_POWER, LED_ALWAYS_ON, NULL);
        }
#endif
    }
    else
    {
        ; //nothing
    }

    if (cur_charge_state == BAT_NORMAL)
    {
        if (cur_bat_val <= 1)
        {
            if ((g_app_info_state.charge_state != BAT_NORMAL)
                    || (g_app_info_state.bat_value > cur_bat_val))
            {
                g_sys_counter.bat_low_cnt = BAT_LOW_WARNING_CYCLE - 4;//2秒后报低电

                //电源LED指示灯闪烁，500ms:500ms
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
                if (g_discrete_led_a23_available == TRUE)
                {
                    pw_link_duty.cycle_count = -1;
                    pw_link_duty.duty_on_time = 500;
                    pw_link_duty.duty_off_time = 500;
                    discrete_led_set(LED_ID_POWER, LED_TWINKLE, &pw_link_duty);
                }
#endif
            }
        }
    }
    else if (cur_charge_state == BAT_FULL)
    {
        if (g_app_info_state.charge_state != BAT_FULL)
        {
            g_sys_counter.charge_full_cnt = 0;
        }
    }
    else
    {
        ; //nothing for QAC
    }

    g_app_info_state.charge_state = cur_charge_state;
    g_app_info_state.bat_value = cur_bat_val;
}

void __section__(".bank") com_battery_charge_full_check(uint32 real_charge_state)
{
    uint16 msg;

    g_sys_counter.charge_full_cnt++;
    if (g_sys_counter.charge_full_cnt > 4)//2秒后，检测电压是否满
    {
        if (real_charge_state < (BATTERY_GRADE_MAX + 1))//BATTERY_GRADE_MAX = 4.20V
        {
            g_sys_counter.charge_full_cnt = 0;
            key_battery_charge_open(0, 0);//打开充电，用上一次参数
        }
    }
    if (g_sys_counter.charge_full_cnt >= BAT_FULL_WARNING_CYCLE)
    {
        g_sys_counter.charge_full_cnt = 0;

        //充电已满
        msg = MSG_FULL_CHARGE;
        sys_mq_send(MQ_ID_SYS, &msg);
    }    
}
/******************************************************************************/
/*!
 * \par  Description:
 *  处理跟电量有关功能，显显示充电标志，检测低压(<3.6V为低压)等
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup
 * \note
 *******************************************************************************/
void com_battery_charge_deal(void)
{
    uint16 msg;
    uint32 tmp_battery_value; //当前电池电量
    uint32 tmp_charge_state; //当前充电状态，分无充电，正在充电，充电已满，无电池
    uint32 real_charge_state;
    int8 volume_limit;
    uint8 app_id = 0;//g_app_info_vector[1].app_id;//linein不作特殊处理

#if (SUPPORT_OUTER_CHARGE == 1)
    if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if ((g_app_info_state.extern_bat_in == 0) && (IS_EXTERN_BATTERY_IN() == 1))
        {
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
            key_extern_tk_op(TK_OP_ON);
#endif
            g_app_info_state.extern_bat_in = 1;
        }
        else if ((g_app_info_state.extern_bat_in == 1) && (IS_EXTERN_BATTERY_IN() == 0))
        {
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
            key_extern_tk_op(TK_OP_OFF);
#endif
            g_app_info_state.extern_bat_in = 0;
        }

        if ((g_app_info_state.stub_tools_type == 0) && (sys_comval->stub_enable == FALSE)
                && (IS_EXTERN_BATTERY_IN() == 0))
        {
            if ((g_this_app_info->app_id != APP_ID_UDISK) && (g_this_app_info->app_id != APP_ID_CONFIG))
            {
                //进入空闲模式
                msg = MSG_IDLE_MODE;
                sys_mq_send(MQ_ID_SYS, &msg);
            }
        }
    }
#endif

    tmp_charge_state = key_battery_get_status(&real_charge_state, &volume_limit, &app_id);

    if (real_charge_state < BATTERY_GRADE_MAX)
    {
        tmp_battery_value = real_charge_state;
    }
    else
    {
        tmp_battery_value = BATTERY_GRADE_MAX;
    }
    //PRINT_DBG_INT("bat charge:", (tmp_charge_state << 8) | tmp_battery_value);

    volume_limit *= 10;
    
    if (volume_limit != sys_comval->volume_limit)
    {
        com_update_volume_limit(volume_limit);
    }       

    if ((tmp_charge_state == BAT_CHECKING) || (tmp_charge_state == BAT_NO_EXIST))
    {
        tmp_charge_state = BAT_NORMAL;
        tmp_battery_value = BATTERY_GRADE_MAX;
    }

    //需要过滤电池供电时电池电量不稳定的情况，只允许电池电量往下跌，不允许往上涨
    if ((g_app_info_state.charge_state == BAT_NORMAL) && (tmp_charge_state == BAT_NORMAL) && (tmp_battery_value
            > g_app_info_state.bat_value))
    {
        tmp_battery_value = g_app_info_state.bat_value;
    }
    
    //libc_print("bat:",g_app_info_state.bat_value,2);
    
    if (g_sys_counter.bat_change_cnt >= 10)
    {
        com_battery_charge_change_deal((uint8)tmp_charge_state, (uint8)tmp_battery_value);
        g_sys_counter.bat_change_cnt = 0;
        return;
    }
    
    if ((g_app_info_state.charge_state != tmp_charge_state) || (g_app_info_state.bat_value != tmp_battery_value))
    {
        g_sys_counter.bat_change_cnt ++;
    }
    else
    {
        g_sys_counter.bat_change_cnt = 0;
    }

    if ((g_app_info_state.charge_state == BAT_NORMAL) && (g_app_info_state.bat_value <= 1))
    {
        g_sys_counter.bat_low_cnt++;
        if (g_sys_counter.bat_low_cnt >= BAT_LOW_WARNING_CYCLE)
        {
            g_sys_counter.bat_low_cnt = 0;

            //检测到低电，广播低电消息
            msg = MSG_LOW_POWER;
            sys_mq_send(MQ_ID_SYS, &msg);
        }
    }
    else if (g_app_info_state.charge_state == BAT_FULL)
    {
        com_battery_charge_full_check(real_charge_state);
    }
    else
    {
        ; //nothing for QAC
    }
}
