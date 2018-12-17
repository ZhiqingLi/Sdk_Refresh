/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：RCP 对话框提示功能实现。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

const app_result_e button_result[][3] =
{
    { RESULT_DIALOG_OK, RESULT_DIALOG_CANCEL, RESULT_NULL },
    { RESULT_DIALOG_ABORT, RESULT_DIALOG_RETRY, RESULT_DIALOG_IGNORE },
    { RESULT_DIALOG_YES, RESULT_DIALOG_NO, RESULT_DIALOG_CANCEL },
    { RESULT_DIALOG_YES, RESULT_DIALOG_NO, RESULT_NULL },
    { RESULT_DIALOG_RETRY, RESULT_DIALOG_CANCEL, RESULT_NULL },
    /*以下是自定义按钮列表*/
    { RESULT_DIALOG_USB_SOUND, RESULT_DIALOG_ONLY_CHARGE, RESULT_NULL },
    { RESULT_NULL, RESULT_NULL, RESULT_NULL },
};

void dialog_overtime(void)
{
    g_dialog_overtime_flag = TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    与APK配合实现对话框询问和信息提示。
 * \param[in]    type 对话框类型，见 dialog_type_e 定义
 * \param[in]    button_type 按键组合类型，包含几种内建按键组合和自定义按键组合，见 button_type_e 定义
 * \param[in]    desc_id 描述字符串ID，与APK约定要显示什么标题和描述信息
 * \param[in]    active_default 默认按键序号
 * \param[in]    check_cb 检测对话框是否合法，如果返回FALSE则返回 RESULT_DIALOG_CANCEL_AUTO
 * \param[out]   none 
 * \return       app_result_e  正常返回 RESULT_DIALOG_XXX 等
 * \ingroup      common_func
 * \note   
 * \li  按任意按键取消返回，返回值为默认按键。
 *******************************************************************************/
app_result_e com_rcp_dialog(uint8 type, uint8 button_type, uint8 desc_id, uint8 active_default,
        dialog_check_callback check_cb)
{
    input_gui_msg_t ui_msg;
    app_result_e result = RESULT_NULL;
    int8 dialog_timer_id;
    uint8 last_dialog_state;

    g_dialog_type = type;
    g_dialog_button_type = button_type;
    g_dialog_desc_id = desc_id;
    g_dialog_active_default = active_default;
    last_dialog_state = g_dialog_state = DIALOG_STATE_LAUNCH;

    g_dialog_overtime_flag = FALSE;
    dialog_timer_id = set_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8), 2000, dialog_overtime);

    while (1)
    {
        if (g_dialog_overtime_flag == TRUE)
        {
            g_dialog_type = DIALOG_TYPE_CANCEL;
            result = button_result[button_type][active_default];
            break;
        }

        if ((last_dialog_state == DIALOG_STATE_LAUNCH) && (g_dialog_state == DIALOG_STATE_CHOOSING))
        {
            if (g_dialog_overtime == (uint8) (-1))
            {
                result = button_result[button_type][active_default];
                break;
            }
            else
            {
                uint16 dialog_select_overtime;

                kill_app_timer(dialog_timer_id);

                dialog_select_overtime = (uint16) g_dialog_overtime * 1000 + 2000;
                dialog_timer_id = set_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8),
                        dialog_select_overtime, dialog_overtime);
            }
        }
        else if ((last_dialog_state == DIALOG_STATE_CHOOSING) && (g_dialog_state == DIALOG_STATE_CONFIRM))
        {
            result = button_result[button_type][g_dialog_choosed];
            break;
        }
        else
        {
            //for QAC
        }

        last_dialog_state = g_dialog_state;

        //如果对话框非法，自动取消对话框
        if ((check_cb != NULL) && (check_cb() == FALSE))
        {
            g_dialog_type = DIALOG_TYPE_CANCEL;
            result = RESULT_DIALOG_CANCEL_AUTO;
            break;
        }

        //获取gui消息
        if (get_gui_msg(&ui_msg) == TRUE) //有gui消息
        { //按任意按键取消返回
            g_dialog_type = DIALOG_TYPE_CANCEL;
            libc_memcpy(&g_buffer_gui_msg, &ui_msg, sizeof(input_gui_msg_t)); //备份缓冲消息
            result = button_result[button_type][active_default];
            break;
        }
        else
        {
            result = com_view_loop();
            if (result > RESULT_COMMON_RESERVE)
            {
                g_dialog_type = DIALOG_TYPE_CANCEL;
                break;
            }
        }

        //挂起10ms，多任务调度
        sys_os_time_dly(1);
    }

    kill_app_timer(dialog_timer_id);
    g_dialog_state = DIALOG_STATE_NONE;
    return result;
}

//检测USB线插入对话框是否合法
bool com_dialog_check_usbdialog(void)
{
    return (get_cable_state() == CABLE_STATE_CABLE_IN);
}
