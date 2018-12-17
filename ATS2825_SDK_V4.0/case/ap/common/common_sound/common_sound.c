/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：音频输出管理接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "music_common.h"
#include "common_func.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    启动或关闭音频输出。
 * \param[in]    state 声音输出状态，见 soundout_state_e 定义
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 启动或关闭音频输出成功。
 * \retval           FALSE 启动或关闭音频输出失败。
 * \ingroup      common_func
 * \note
 * \li  关闭音频输出时，如果按键音使能，则进行按键音音量设置。
 *******************************************************************************/
bool com_set_sound_out(soundout_state_e state)
{
    //设置音频输出状态
    change_soundout_state(state);

    return TRUE;
}

/*! \endcond */

