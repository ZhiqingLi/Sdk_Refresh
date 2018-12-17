/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 系统软定时器管理，包括初始化和销毁接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    创建系统计时软定时器和 headbar 周期更新软定时器，由前台应用进入应用时调用。
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 * \li  除了 config.app，前台应用在进入时必须调用该接口，否则背光，屏保，省电关机，
 *      定时关机，状态栏更新，等等功能将无法使用。
 *******************************************************************************/
void sys_timer_init(void)
{
    sys_counter_timer_id = set_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8), 500, sys_counter_handle);

    peripheral_detect_timer_id = set_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8), PER_DETECT_PERIOD, \
            peripheral_detect_handle);

    //电池及其充电检测处理
    //com_battery_charge_deal();

    //USB,UHOST,CARD,AUX检测
    g_detect_card_force_flag = 1;
    peripheral_detect_handle();
}

/******************************************************************************/
/*!
 * \par  Description:
 *    删除系统计时软定时器和 headbar 周期更新软定时器，由前台应用退出应用时调用。
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 * \li  退出应用时，需要调用 sys_counter_exit，以删除 sys_counter_init 创建的软定时器。
 *******************************************************************************/
void sys_timer_exit(void)
{
    kill_app_timer(sys_counter_timer_id);

    kill_app_timer(peripheral_detect_timer_id);
}

/*! \endcond */
