/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：TTS 播放模块，创建解码子线程进行播放；支持FIFO方式延迟播放；该模块还包括
 *       设置过滤TTS等接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_tts.h"
#include "common_func.h"

tts_play_info_t __section__(".gl_tts_data") g_tts_play_info;

extern bool tts_play_init(uint16 tts_mode, void* tts_ptr);
extern void tts_play_exit(uint16 tts_mode);
extern void tts_play_loop(void);

uint8 tts_fifo_out(uint16 *tts_mode, uint8 *tts_sections);
uint8 tts_fifo_in(uint16 tts_mode, uint8 *tts_sections);
bool com_tts_state_play_init(uint16 tts_mode, void* tts_ptr);
tts_play_ret_e com_tts_state_play_loop(uint16 tts_mode);
void com_tts_state_play_exit(uint16 tts_mode);

/******************************************************************************/
/*!
 * \par  Description:
 * \uint8 need_tts_state_play(uint8 tts_mode, void *tts_info)
 *  //是否需要TTS播报，0表示不需要播报，其他值表示要播报的TTS资源数目
 * \param[in]    tts_mode ,播报类型
 * \param[in]    tts_info 播报值
 * \param[out]   none
 * \return       返回TRUE表示可以播报，返回FALSE表示不能播报
 * \ingroup      misc_func
 * \note
 * \li
 *******************************************************************************/
bool need_tts_state_play(uint16 tts_mode, void *tts_info, uint8 *tts_sec_ids)
{
    uint8 section_cnt = 0;

    g_tts_play_info.tts_str = NULL;

    if (sys_comval->tts_enable == FALSE)
    { //不支持状态语音播报功能
        return 0;
    }

    if (((g_tts_play_info.option & IGNORE_PLAY_TTS) != 0)
            && ((tts_mode & TTS_MODE_PB_OPTION_MASK) == TTS_MODE_PB_NOPLAY))
    { //场景不便进行TTS，屏蔽TTS；电话簿播报除外
        return 0;
    }

    if (get_mute_enable() == TRUE)
    { //静音状态下，直接返回
        return 0;
    }

    if (g_app_info_state.autotest_mode == TRUE)
    {
        return 0;
    }

#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    if (get_card_state() != CARD_STATE_CARD_NULL)
    {
        return 0;
    }
#endif

    if ((sys_comval->volume_current == 0)
            && ((sys_comval->kt_tts_min_volume == 0)
                    || ((tts_mode & TTS_MODE_MIN_VOLUME_MASK) == TTS_MODE_MIN_VOLUME_NO)))
    { //允许音量为0时没有按键音和TTS
        return 0;
    }

    if ((tts_mode & TTS_MODE_TYPE_MASK) == TTS_MODE_ONLYONE)
    {
        tts_sec_ids[0] = (uint8) (uint32) tts_info;
        section_cnt = 1;
    }
    else if ((tts_mode & TTS_MODE_TYPE_MASK) == TTS_MODE_SECLIST)
    {
        uint8 *p_tts_sec = (uint8 *) tts_info;
        uint8 i;

        section_cnt = 0;
        for (i = 0; i < (TTS_SECTIONS_MAX - 1); i++)
        {
            if (p_tts_sec[i] == 0xff)
            {
                break;
            }
            tts_sec_ids[i] = p_tts_sec[i];
            section_cnt++;
        }
    }
    else if ((tts_mode & TTS_MODE_TYPE_MASK) == TTS_MODE_STRING)
    {
        g_tts_play_info.tts_str = tts_info;
    }
    else
    {
        section_cnt = 1;
        return section_cnt;
    }

    tts_sec_ids[section_cnt] = 0xff;

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void com_tts_state_play(uint8 tts_id, void *tts_info)
 *    状态语音播报
 * \param[in]    tts_id 状态编号
 * \param[in]    value 状态值
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 * \li
 *******************************************************************************/
tts_play_ret_e com_tts_state_play_deal(void)
{
    tts_play_ret_e ret_val = TTS_PLAY_RET_NORMAL;

    if ((g_tts_play_info.status == TTS_STATUS_READY) || (g_tts_play_info.status == TTS_STATUS_PLAYING))
    {
        return ret_val;
    }

    if ((g_tts_play_info.status == TTS_STATUS_IDLE) && (g_tts_play_info.count > 0))
    {
        tts_fifo_out(&(g_tts_play_info.cur_mode), g_tts_play_info.section_ids);
        if (com_tts_state_play_init(g_tts_play_info.cur_mode, g_tts_play_info.tts_str) == FALSE)
        {
            return TTS_PLAY_RET_NORMAL;
        }
        if ((g_tts_play_info.cur_mode & TTS_MODE_BLOCK_OPTION_MASK) == TTS_MODE_BLOCK)
        {
            ret_val = com_tts_state_play_loop(g_tts_play_info.cur_mode);
        }
    }

    if (g_tts_play_info.status == TTS_STATUS_PLAYOVER)
    {
        com_tts_state_play_exit(g_tts_play_info.cur_mode);
    }

    return ret_val;
}

bool com_tts_state_play_init(uint16 tts_mode, void* tts_ptr)
{
    pthread_ext_param_t tts_thread;

    PRINT_INFO_INT("tts enter:", ((uint32)tts_mode << 16) | g_tts_play_info.section_ids[0]);
     //DAE模式切换时必须保证按键音已播完
    keytone_play_deal_wait();
  
    //TTS播报加载空的音效库
	sys_comval->dae_cfg.enable = 0;
	com_update_dae_config(&sys_comval->dae_cfg);
    __set_dae_config(&sys_comval->dae_cfg);
	 
    if (tts_play_init(tts_mode, tts_ptr) == FALSE)
    {
        PRINT_ERR("tts init fail");
        g_tts_play_info.status = TTS_STATUS_IDLE;
        return FALSE;
    }

    g_tts_play_info.status = TTS_STATUS_READY;

    //TTS线程和DSP调频
    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL7, FREQ_LEVEL7);
    adjust_freq_add_value(0, sys_comval->dae_cfg.run_freq);

    //初始化创建线程的参数
    tts_thread.pthread_param.start_rtn = (void *) tts_play_loop;
    tts_thread.pthread_param.arg = NULL;
    tts_thread.pthread_param.ptos = (void *) AP_BACK_HIGH_STK_POS;
    tts_thread.stk_size = AP_BACK_HIGH_STK_SIZE;

    if (libc_pthread_create(&tts_thread, AP_BACK_HIGH_PRIO, CREATE_NOT_MAIN_THREAD) < 0)
    {
    }

    return TRUE;
}

void com_tts_state_play_exit(uint16 tts_mode)
{
    //TTS线程和DSP调频
    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    adjust_freq_add_value(0, 0);

    tts_play_exit(tts_mode);

    PRINT_DBG("tts exit");
}

tts_play_ret_e com_tts_state_play_wait(void)
{
    tts_play_ret_e ret_val = TTS_PLAY_RET_NORMAL;

    if (g_tts_play_info.status != TTS_STATUS_IDLE)
    {
        ret_val = com_tts_state_play_loop(g_tts_play_info.cur_mode);

        com_tts_state_play_exit(g_tts_play_info.cur_mode);
    }

    return ret_val;
}

tts_play_ret_e com_tts_state_play_loop(uint16 tts_mode)
{
    tts_play_ret_e ret_val = TTS_PLAY_RET_NORMAL;
    app_result_e result;

    while ((g_tts_play_info.status == TTS_STATUS_READY) || (g_tts_play_info.status == TTS_STATUS_PLAYING))
    {
        //纯粹TTS，不需要理会所有事件
        if ((tts_mode & TTS_MODE_NORECV_ALL_EV_MASK) == TTS_MODE_NORECV_ALL_EV_YES)
        {
            sys_os_time_dly(1);
            continue;
        }

        if (g_app_timer_count > 0)
        {
            handle_timers(g_app_timer_vector, g_app_timer_count);
        }
        handle_timers(g_com_app_timer_vct, COM_APP_TIMER_MAX);

        if ((tts_mode & TTS_MODE_KEY_OPTION_MASK) != TTS_MODE_KEY_NORECV)
        {
            input_gui_msg_t ui_msg;

            if (get_gui_msg(&ui_msg) == TRUE)
            {
                if ((tts_mode & TTS_MODE_KEY_OPTION_MASK) == TTS_MODE_KEY_BREAK_REDEAL)
                {
                    libc_memcpy(&g_buffer_gui_msg, &ui_msg, sizeof(input_gui_msg_t)); //redeal
                    g_tts_play_info.option |= (uint8) FORCE_STOP_TTS;
                    ret_val = TTS_PLAY_RET_BY_KEY;
                    PRINT_INFO("tts force stop by key!");
                    break;
                }
                else if ((tts_mode & TTS_MODE_KEY_OPTION_MASK) == TTS_MODE_KEY_BREAK_FILTER)
                {
                    com_filter_key_hold();
                    g_tts_play_info.option |= (uint8) FORCE_STOP_TTS;
                    ret_val = TTS_PLAY_RET_BY_KEY;
                    PRINT_INFO("tts force stop by key!");
                    break;
                }
                else
                {
                    com_filter_key_hold();
                }
            }
        }

        if ((tts_mode & TTS_MODE_SYS_OPTION_MASK) != TTS_MODE_SYS_NORECV)
        {
            private_msg_t pri_msg;

            if (get_app_msg(&pri_msg) == TRUE)
            {
                result = com_view_loop_sys(&pri_msg, TRUE);
                if (result > RESULT_COMMON_RESERVE)
                {
                    libc_memcpy(&g_buffer_app_msg, &pri_msg, sizeof(private_msg_t)); //redeal
                    g_tts_play_info.option |= (uint8) FORCE_STOP_TTS;
                    ret_val = TTS_PLAY_RET_BY_SYS;
                    PRINT_INFO("tts force stop by sys!");
                    break;
                }
            }
        }

        if ((tts_mode & TTS_MODE_RCP_OPTION_MASK) != TTS_MODE_RCP_NORECV)
        {
            if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
            {
                result = com_btmanager_loop(TRUE);
                if (result > RESULT_COMMON_RESERVE)
                {
                    g_tts_play_info.option |= (uint8) FORCE_STOP_TTS;
                    ret_val = TTS_PLAY_RET_BY_BTEV;
                    PRINT_INFO("tts stop by btstack!");
                    break;
                }
            }
        }

        sys_os_time_dly(1);
    }

    //FORCE_STOP_TTS后等待TTS结束
    while ((g_tts_play_info.status == TTS_STATUS_READY) || (g_tts_play_info.status == TTS_STATUS_PLAYING))
    {
        sys_os_time_dly(1);
    }

    return ret_val;
}

tts_play_ret_e com_tts_state_play(uint16 tts_mode, void *tts_info)
{
    tts_play_ret_e ret_val = TTS_PLAY_RET_NORMAL;
    uint8 tmp_tts_sec_ids[TTS_SECTIONS_MAX];

    if (need_tts_state_play(tts_mode, tts_info, tmp_tts_sec_ids) == FALSE)
    {
        return ret_val;
    }

    if ((tts_mode & TTS_MODE_FIFO_OPTION_MASK) == TTS_MODE_USEFIFO)
    {
        tts_mode &= (uint16) (~TTS_MODE_FIFO_OPTION_MASK);
        tts_fifo_in(tts_mode, tmp_tts_sec_ids);
        PRINT_DBG_INT("tts fifo:", g_tts_play_info.count);
        return ret_val;
    }

    //等待非阻塞TTS播放完毕
    while (g_tts_play_info.status != TTS_STATUS_IDLE)
    {
        if (g_tts_play_info.status == TTS_STATUS_PLAYOVER)
        {
            com_tts_state_play_exit(g_tts_play_info.cur_mode);
        }
    }

    g_tts_play_info.cur_mode = tts_mode;
    libc_memcpy(g_tts_play_info.section_ids, tmp_tts_sec_ids, TTS_SECTIONS_MAX);

    if (com_tts_state_play_init(g_tts_play_info.cur_mode, g_tts_play_info.tts_str) == FALSE)
    {
        return TTS_PLAY_RET_NORMAL;
    }

    if ((tts_mode & TTS_MODE_BLOCK_OPTION_MASK) == TTS_MODE_NOBLOCK)
    {
        return ret_val;
    }

    ret_val = com_tts_state_play_loop(g_tts_play_info.cur_mode);

    com_tts_state_play_exit(g_tts_play_info.cur_mode);

    return ret_val;
}

uint8 tts_fifo_out(uint16 *tts_mode, uint8 *tts_sections)
{
    uint8 i;

    if ((g_tts_play_info.count == 0) || (g_tts_play_info.count > TTS_FIFO_DEPTH))
    {
        return -1;
    }

    *tts_mode = g_tts_play_info.tts_mode[0];
    libc_memcpy(tts_sections, g_tts_play_info.tts_fifo[0], TTS_SECTIONS_MAX);
    g_tts_play_info.count--;
    
    for (i = 0; i < g_tts_play_info.count; i++)
    {
        g_tts_play_info.tts_mode[i] = g_tts_play_info.tts_mode[i + 1];
        libc_memcpy(g_tts_play_info.tts_fifo[i], g_tts_play_info.tts_fifo[i + 1], TTS_SECTIONS_MAX);
    }

    return 0;
}

uint8 tts_fifo_in(uint16 tts_mode, uint8 *tts_sections)
{
    if (g_tts_play_info.count >= TTS_FIFO_DEPTH)
    {
        return -1;
    }

    g_tts_play_info.tts_mode[g_tts_play_info.count] = tts_mode;
    libc_memcpy(g_tts_play_info.tts_fifo[g_tts_play_info.count], tts_sections, TTS_SECTIONS_MAX);
    g_tts_play_info.count++;

    return 0;
}

void com_tts_state_play_control(tts_control_opr_e opr)
{
    switch (opr)
    {
        case TTS_CTRL_IGNORE:
        g_tts_play_info.option |= (uint8) IGNORE_PLAY_TTS;
        break;

        case TTS_CTRL_CANCEL_IGNORE:
        g_tts_play_info.option &= (uint8) (~IGNORE_PLAY_TTS);
        break;

        case TTS_CTRL_FORCE_STOP:
        g_tts_play_info.option |= (uint8) FORCE_STOP_TTS;
        break;

        case TTS_CTRL_CANCEL_FORCE_STOP:
        g_tts_play_info.option &= (uint8) (~FORCE_STOP_TTS);
        break;

        default:
        break;
    }
}
