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
void enable_onoff_key_reset_function(int enable)
{
    if(enable)
        act_writel(act_readl(ONOFF_KEY) | (1 << ONOFF_KEY_ONOFF_RST_EN), ONOFF_KEY);
    else
        act_writel(act_readl(ONOFF_KEY) & (~(1 << ONOFF_KEY_ONOFF_RST_EN)), ONOFF_KEY);
}

app_result_e config_36410_charging_mode_loop(void)
{
    app_result_e result = RESULT_NULL;
    msg_apps_t msg;

    enable_onoff_key_reset_function(0);         //disable reset function
    libc_print("enter charging mode...", 0, 0);
    //进入空闲模式
    set_cur_func_id(APP_FUNC_IDLE_MODE);
    while(1) {
        result = com_view_loop();

        if(IS_EXTERN_CAHREGE_IN() == 0) {
            result = RESULT_POWER_OFF;
            break;
        }

        if((act_readl(ONOFF_KEY) & (1 << ONOFF_KEY_ONOFF_PRESS_0)) == 0) {
            //restart system
            enable_onoff_key_reset_function(1);         //enable reset function
            libc_print("charging mode reset to normal", 0, 0);
            ENABLE_WATCH_DOG(0x00);
            while(1);
            break;
        }
        //挂起10ms，多任务调度
        sys_os_time_dly(1);
        CLEAR_WATCH_DOG;
    }
    enable_onoff_key_reset_function(1);         //enable reset function

    return result;
}

