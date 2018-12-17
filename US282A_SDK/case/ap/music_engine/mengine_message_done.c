#include "app_mengine.h"
#include <analog.h>

void reset_by_PW_off(void);


//引擎消息处理,处理引擎接收到的私有消息和系统消息
app_result_e mengine_message_done_bank(private_msg_t* cur_msg)
{
    g_mengine_result = RESULT_NULL;

    switch (cur_msg->msg.type)
    {
        case MSG_MENGINE_GET_PLAYMODE_SYNC: //获取循环模式+shuffle
        mengine_get_playmode(cur_msg);
        break;

        case MSG_MENGINE_GET_FILEPATH_SYNC: //获取文件location信息
        mengine_get_filepath(cur_msg);
        break;

        /*设置参数相关*/
        case MSG_SET_DAE_CFG_SYNC: //设置eq +eq curve data
        mengine_set_eq(cur_msg);
        break;

        case MSG_MENGINE_SET_PLAYMODE_SYNC: //设置循环模式+shuffle
        mengine_set_playmode(cur_msg);
        break;
#ifdef AB_SUPPORT
        /*AB复读相关*/
        case MSG_MENGINE_SET_AB_COUNT_SYNC: //设置AB复读次数
        mengine_set_ab_count(cur_msg);
        break;

        case MSG_MENGINE_SET_AB_GAP_SYNC: //设置AB复读间隔
        mengine_set_ab_gap(cur_msg);
        break;

        case MSG_MENGINE_START_AB_SYNC: //启动ab复读，等待设置a点
        mengine_start_ab(cur_msg);
        break;

        case MSG_MENGINE_SET_APOINT_SYNC: //设置a点
        mengine_set_apoint(cur_msg);
        break;

        case MSG_MENGINE_SET_BPOINT_SYNC: //设置b点，并开始复读
        mengine_set_bpoint(cur_msg);
        break;

        case MSG_MENGINE_CLEAR_AB_SYNC: //清除ab复读
        mengine_clear_ab(cur_msg);
        break;
#endif //#ifdef AB_SUPPORT
        /*播放音乐相关*/
        case MSG_MENGINE_PLAY_SYNC: //开始播放音乐
        mengine_play(cur_msg);
        break;

        case MSG_MENGINE_STOP_SYNC: //停止播放音乐
        mengine_stop(cur_msg);
        break;

        case MSG_MENGINE_PAUSE_SYNC: //暂停播放音乐
        mengine_pause(cur_msg);
        break;

        case MSG_MENGINE_RESUME_SYNC: //恢复播放音乐(从暂停到播放)
        mengine_resume(cur_msg);
        break;

        /*快进退相关*/
        case MSG_MENGINE_FFWD_SYNC:
        mengine_fast_forward(cur_msg); //快进
        break;

        case MSG_MENGINE_FBWD_SYNC:
        mengine_fast_backward(cur_msg); //快退
        break;

        case MSG_MENGINE_CANCEL_FFB_SYNC:
        mengine_cancel_ffb(cur_msg); //取消快进退
        break;

        case MSG_MENGINE_SET_FFBSTEP_SYNC:
        mengine_fastplay_step(cur_msg); //设置快进退步长
        break;

        //设置播放文件的路径location
        case MSG_MENGINE_SET_FILEPATH_SYNC:
        mengine_set_filepath(cur_msg);
        break;

        case MSG_MENGINE_PLAY_NEXT_SYNC: //播放下一曲
        mengine_play_next(cur_msg);
        break;

        case MSG_MENGINE_PLAY_PREV_SYNC: //播放上一曲
        mengine_play_prev(cur_msg);
        break;

        case MSG_MENGINE_SWITCH_DISK_SYNC: //切换磁盘
        mengine_switch_disk(cur_msg);
        break;

        case MSG_MENGINE_LIST_ACCESS_ALLOW_SYNC: //可访问列表
        mengine_list_access_allow(cur_msg);
        break;

        case MSG_MENGINE_DIGITAL_SONG_SYNC: //数字点歌
        mengine_play_digital_song(cur_msg);
        break;

        case MSG_MENGINE_FOLFER_NEXT_SYNC: //切换下一个目录
        mengine_play_folder_next(cur_msg);
        break;

        case MSG_MENGINE_FOLFER_PREV_SYNC: //切换上一个目录
        mengine_play_folder_prev(cur_msg);
        break;
        case MSG_MENGINE_GET_FILEINFO_SYNC: //获取文件格式、b/s、总时间等信息
        mengine_get_fileinfo(cur_msg);
        break;

        case MSG_MENGINE_CLEAR_FILESWITCH_SYNC:
        mengine_clear_fileswitch(cur_msg);
        break;

        case MSG_MENGINE_GET_PLAYTYPE_SYNC:
        mengine_get_playtype(cur_msg);
        break;

        case MSG_MENGINE_SET_SORTNUMBER_SYNC: //更新文件序号
        mengine_set_sortnumber(cur_msg);
        break;

        case MSG_MENGINE_GET_DISKNUMBER_SYNC: //获取当前文件序号
        mengine_get_disknumber(cur_msg);
        break;

        /*获取id3信息*/
        case MSG_MENGINE_GET_ID3INFO_SYNC:
        #ifdef MENGINE_ID3_SUPPORT
        mengine_getid3_info(cur_msg);
#endif
        break;

        case MSG_MENGINE_GET_DIRNAME_SYNC:
        #ifdef DIR_NAME_SUPPORT
        mengine_get_dirname(cur_msg);
#endif
        break;

        case MSG_ENGINE_STANDBY_SYNC:
        g_mengine_result = mengine_standby(cur_msg);
        break;

        case MSG_APP_QUIT: //退出app
        if ((cur_msg->msg.content.data[0] & 0x01) != 0)
        {
            PRINT_INFO("eg enter s3bt");
        }
        g_mengine_result = RESULT_APP_QUIT;
        break;

        case MSG_TTS_START_SYNC: //准备进入TTS播报
        case MSG_SWITCH_AOUT_START_SYNC: //准备切换音频输出
        g_mengine_result = mengine_tts_start(cur_msg);
        break;

        case MSG_TTS_STOP_SYNC: //TTS播报完成
        case MSG_SWITCH_AOUT_STOP_SYNC: //完成切换音频输出
        g_mengine_result = mengine_tts_stop(cur_msg);
        break;

        case MSG_MENGINE_SAVE_BKINFO_SYNC:
        mengine_vram_write(g_eg_cfg_p->location.dirlocation.disk);
        mengine_reply_msg(cur_msg, TRUE);
        g_mengine_result = RESULT_IGNORE;
        break;

        case MSG_MENGINE_SD_PW_RESET_SYNC://SD卡断电重上电
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        reset_by_PW_off();
        mengine_reply_msg(cur_msg, TRUE);
#else
        mengine_reply_msg(cur_msg, FALSE);
#endif
        break;

        case MSG_MENGINE_SET_FIO_SYNC: //设置SDD扩展区文件操作接口
        {
            bool msg_ret = FALSE;
            void *io = (void*)cur_msg->msg.content.addr;
            if(g_mp_handle != NULL)
            {
                mmm_mp_cmd(g_mp_handle, MMM_MP_SET_SDD_FILE_IO, io);
                msg_ret = TRUE;
            }
            mengine_reply_msg(cur_msg, msg_ret);
        }
        break;
        
        //清除断点错误标志
        case MSG_MENGINE_CLR_BKERR_SYNC:
        g_eg_status_p->first_file_status = FIRST_PLAY_FROM_BK;
        mengine_reply_msg(cur_msg, TRUE); 
        break;

        default:
        break;
    }

    return g_mengine_result;
}

//断电SD卡复位
void reset_by_PW_off(void)
{
    #if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    uint32 bk_init,bk_set;
    bk_init = Get_SD_PW_PIN_INOUT();
    bk_set = Get_SD_PW_PIN_ONOFF();
    PRINT_ERR("SD-PW-RESET...");
    SET_SD_PW_PIN_ON();
    SD_PW_PIN_SET_OUTPUT();
    sys_os_time_dly(1);
    SET_SD_PW_PIN_RESET();
    sys_os_time_dly(8);
    SET_SD_PW_PIN_ON();
    sys_os_time_dly(2);
    if(bk_init == 0)//是否要改回输入状态
    {
        SD_PW_PIN_SET_INPUT();
    }
    
    if(bk_set != 0)//是否要改回为复位状态
    {
        SET_SD_PW_PIN_RESET();
    }
    #endif
    
}

//开始播放
app_result_e mengine_play(void* msg_ptr)
{
    bool ret_vals;

    ret_vals = play(PLAY_NORMAL); //播放
    g_eg_playinfo_p->cur_file_switch |= 0x01;

    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//停止播放
app_result_e mengine_stop(void* msg_ptr)
{
    bool ret_vals;

    ret_vals = stop(STOP_NORMAL); //停止播放

    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//暂停播放
app_result_e mengine_pause(void* msg_ptr)
{
    bool ret_vals;

    ret_vals = stop(STOP_PAUSE); //暂停播放

    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//恢复播放(由暂停恢复)
app_result_e mengine_resume(void* msg_ptr)
{
    bool ret_vals;
    private_msg_t *p = (private_msg_t *)msg_ptr;

    //设置文件播放
    ret_vals = play(*((play_mode_e *)p->msg.content.addr)); //播放

    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//保存引擎错误号
void mengine_save_errno_no(void)
{
    eg_err_e err_status = EG_ERR_NONE;

    mmm_mp_status_t mmm_status;

    //读取错误号
    mmm_mp_cmd(g_mp_handle, MMM_MP_GET_ERRORNO, (unsigned int) &mmm_status.err_no);

    //转换错误号
    switch (mmm_status.err_no)
    {
        case MMM_MP_ERR_OPEN_FILE:
        err_status = EG_ERR_OPEN_FILE; //打开文件失败
        break;

        case MMM_MP_ERR_FILE_NOT_SUPPORT: //文件格式不支持
        err_status = EG_ERR_NOT_SUPPORT;
        PRINT_ERR("FILE NOT SUPPORT");
        break;

        case MMM_MP_ERR_FILE_DAMAGED: //文件损毁
        err_status = EG_ERR_NOT_SUPPORT;
        PRINT_ERR("FILE DESTORY");
        break;

        case MMM_MP_ERR_DECODER_ERROR: //解码出错
        err_status = EG_ERR_DECODER_ERROR;
        PRINT_ERR("DECODER ERROR");
        break;

        case MMM_MP_ERR_NO_LICENSE: //无license
        err_status = EG_ERR_NO_LICENSE;
        break;

        case MMM_MP_ERR_SECURE_CLOCK: //DRM时钟错误
        err_status = EG_ERR_SECURE_CLOCK;
        break;

        case MMM_MP_ERR_LICENSE_INFO: //播放次数已满或是播放时间已经到期
        err_status = EG_ERR_DRM_INFO;
        break;
        
        case MMM_MP_SAMPLE_RATE_NOT_SUPPORT: //采样率不支持
        err_status = EG_ERR_NOT_SUPPORT;
        PRINT_ERR("SAMPLE RATE NOT SUPPORT");
        break;   
        
        default:
        err_status = EG_ERR_DECODER_ERROR; //解码出错
        break;
    }
    //保存错误状态
    g_eg_status_p->err_status = err_status;

}

void error_handle2(void)
{
    //关闭解码中间件
    stop(STOP_NORMAL);

    if (g_file_sys_id != -1)
    {
        //获取文件总数(保证未扫描完成时获取到正确的文件总数)
        g_eg_cfg_p->location.dirlocation.file_total = fsel_get_total();
    }
}

//错误处理
void error_handle(void)
{
    //保存出错状态
    mengine_save_errno_no();
    error_handle2();
}

