/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟起闹处理，决定是否响应起闹；闹钟起闹结束后修改闹钟时间等。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_time_alarm.h"
#include "vm_def.h"

//计算星期
static uint8 get_weekofday(uint16 y, uint8 m, uint8 d)
{
    int8 wk; //星期
    uint8 ic; //年份的前两位
    uint8 ir; //年份的后两位

    if (m < 3)
    {
        m += 12;
        y--;
    }
    ic = (uint8) (y / 100);
    ir = (uint8) (y % 100);
    //Taylor's formula
    wk = (int8) ((ic / 4) + ir + (ir / 4) + (26 * (m + 1)) / 10 - 2 * ic + d - 1); // 避免负数
    while (wk >= 7) //矫正结果
    {
        wk -= 7;
    }
    while (wk < 0)
    {
        wk += 7;
    }
    return wk;
}

/*检查闹钟是否合法——匹配星期*/
static bool check_alarm_day(uint8 alarm_day, date_t *alarm_date)
{
    uint8 weekofday = get_weekofday(alarm_date->year, alarm_date->month, alarm_date->day);

    if (alarm_day != 0)
    { //循环
        if ((alarm_day & (1 << weekofday)) != 0)
        { //匹配
            return TRUE;
        }
        else
        { //不匹配
            return FALSE;
        }
    }
    else
    { //只闹一次闹钟，无需匹配星期
        return TRUE;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    TOOLS 应用闹钟播放处理
 * \param[in]    alarm_ring_mode ALARM_RING_ENTER, ALARM_RING_CLOSE, ALARM_RING_SNOOZE
 * \param[out]   none
 * \return       app_result_e 返回 RESULT_APP_QUIT 或 RESULT_NULL 等
 * \ingroup
 * \note
 *******************************************************************************/
app_result_e tm_alarm_alarm_ring_deal(alarm_ring_mode_e alarm_ring_mode)
{
    alarm_record_t cur_alarm_record;
    app_result_e result = RESULT_NULL;
    date_t alarm_date;

    //获取当前闹钟记录
    if (time_alarm_record_list_operate(RECORD_LIST_READ_CUR, &cur_alarm_record, 0) < 0)
    {
        //没有当前有效闹钟记录
        return RESULT_NULL;
    }

    //读取日期和时间
    sys_get_date(&alarm_date);
    if (alarm_date.year > 2099)
    {
        alarm_date.year = 2000;
        sys_set_date(&alarm_date);
    }

    //刚进入，检测闹钟是否合法等
    if (alarm_ring_mode == ALARM_RING_ENTER)
    {
        PRINT_INFO("alarm enter");

        if ((cur_alarm_record.snooze == 0) //只有初次起闹才需要匹配星期
        && (check_alarm_day(cur_alarm_record.alarm.alarm_day, &alarm_date) == FALSE))
        { //星期不匹配，更新闹钟时间后立即退出
          //更新下次闹钟时间，如果是只闹一次闹钟，那么将其关闭
            alarm_ring_mode = ALARM_RING_CLOSE;
            result = RESULT_NULL;
        }
    }
    else if (alarm_ring_mode == ALARM_RING_ENTER_CLOSE_AUTO)
    {
        PRINT_INFO("alarm enter, but close auto!");
        alarm_ring_mode = ALARM_RING_CLOSE;
        result = RESULT_NULL;
    }
    else
    {
        PRINT_INFO("alarm stop");
    }

    //关闭或者延时闹钟，重设闹钟时间
    if ((alarm_ring_mode == ALARM_RING_CLOSE) || (alarm_ring_mode == ALARM_RING_SNOOZE))
    {
        if (alarm_ring_mode == ALARM_RING_SNOOZE)
        {
            if ((sys_comval->snooze_max != 0)
                    && (cur_alarm_record.snooze >= sys_comval->snooze_max))
            {
                alarm_ring_mode = ALARM_RING_CLOSE;
            }
        }

        if (alarm_ring_mode == ALARM_RING_CLOSE)
        {
            time_alarm_record_list_operate(RECORD_LIST_UPDATE_ALARM, (void *) UPDATE_ALARM_CLOSE, 0);
        }
        else
        {
            time_alarm_record_list_operate(RECORD_LIST_UPDATE_ALARM, (void *) UPDATE_ALARM_SNOOZE, 0);
        }

        if (g_ap_switch_var.rtcalarm_poweron == TRUE)
        {
            g_ap_switch_var.rtcalarm_poweron = FALSE;
            g_ap_switch_var.rtcalarm_poweroff = TRUE;
            result = RESULT_POWER_OFF;
        }
    }
    else
    {
        result = RESULT_ENTER_ALARM;
    }

    PRINT_INFO_INT("alarm go to:", result);

    return result;
}
