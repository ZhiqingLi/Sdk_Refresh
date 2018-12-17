/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 设置图形EQ接口，可以通过UI进行反馈；最终转为设置DAE接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "music_common.h"
#include "common_func.h"

void com_set_eq_view(view_update_e mode);
void com_create_eq_view(void)
{
    create_view_param_t create_view_param;

#if (SUPPORT_LED_DRIVER == 1)
    if (sys_comval->support_led_display == 1)
    {
        create_view_param.type = VIEW_TYPE_MSG;
        create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
        create_view_param.overtime = 3000;
        create_view_param.ke_maplist = NULL;
        create_view_param.se_maplist = NULL;
        create_view_param.view_cb = com_set_eq_view;
        com_view_add(COM_VIEW_ID_SET_EQ, &create_view_param);
    }
#endif
}

static void __com_set_eq_redraw(void)
{
    led_display(LCD_COL, 0xff, 0); //清除:号
    led_display(LCD_P1, 0xff, 0); //清除.号
    led_display(NUMBER1, 0, 0);
    led_display(NUMBER2, NUM_E, 1);
    led_display(NUMBER3, NUM_Q, 1);
    led_display(NUMBER4, NUM_0 + sys_comval->eq_type, 1);
}

void com_set_eq_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            __com_set_eq_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __com_set_eq_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __com_set_eq_redraw();
        }
        break;

        default:
        break;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    设置EQ。
 * \param[in]    mode 设置EQ模式，0表示指定EQ，1表示向下切EQ，2表示向上切EQ
 * \param[in]    eq_type 指定EQ，只有mode为0时有效
 * \param[in]    eq_para 用户EQ参数，只有设置用户EQ有效
 *                       NULL表示使用系统当前的用户EQ参数，非NULL表示更新用户EQ参数
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 设置EQ成功
 * \retval           FALSE 设置EQ失败
 * \note
 *******************************************************************************/
bool com_set_sound_eq(uint8 mode, com_eqtype_e eq_type, uint8 *eq_para)
{
    daemode_param_t tmp_daemode_param;
    bool bret = TRUE;

    if (sys_comval->eq_enable == FALSE)
    {
        return FALSE;
    }

    switch (mode)
    {
        case SETEQ_TYPE:
        sys_comval->eq_type = eq_type;
        break;

        case SETEQ_NEXT:
        if (sys_comval->eq_type == EQ_USR_DEF)
        {
            sys_comval->eq_type = EQ_NOR;
        }
        else
        {
            sys_comval->eq_type++;
        }
        break;

        case SETEQ_PREV:
        if (sys_comval->eq_type == EQ_NOR)
        {
            sys_comval->eq_type = EQ_USR_DEF;
        }
        else
        {
            sys_comval->eq_type--;
        }
        break;

        default:
        break;
    }

    if (sys_comval->eq_type != EQ_USR_DEF)
    {
        libc_memset(tmp_daemode_param.eq_para, 0x00, MAX_GEQ_SEG);
    }
    else
    {
        if (eq_para != NULL)
        {
            libc_memcpy(tmp_daemode_param.eq_para, eq_para, MAX_GEQ_SEG);
        }
    }

    tmp_daemode_param.dae_mode = DAE_MODE_EQ;
    tmp_daemode_param.param.cur_eqtype = sys_comval->eq_type;

    bret = com_set_dae_config(&tmp_daemode_param);

    //更新DAE参数后，需要重新设置音量
    com_reset_sound_volume(0);

    return bret;
}

void com_eq_next(void)
{
    com_set_sound_eq(SETEQ_NEXT, 0, NULL);

    com_create_eq_view();
}

void com_eq_prev(void)
{
    com_set_sound_eq(SETEQ_PREV, 0, NULL);

    com_create_eq_view();
}

void com_eq_set(void)
{
    com_set_sound_eq(SETEQ_NEXT, 0, NULL);

    com_create_eq_view();
}

