#include "app_mengine.h"

//bank变量，临时用一下
static mengine_config_t s_cfg _BANK_DATA_ATTR_;

void mengine_vram_write(uint8 save_disk)
{
    //写vram
    sys_vm_write(g_eg_cfg_p,
            (save_disk == DISK_H) ? VM_CARD_BKINFO : VM_UHOST_BKINFO,
            sizeof(*g_eg_cfg_p));
}

//始化文件选择器，定位到上次退出的文件
bool switch_disk_file_init(void)
{
    //文件选择器初始化参数
    fsel_param_t switch_init_param;
    //后缀名buffer
    uint8 exname[4];
    //文件选择类型
    //fsel_type_e file_source;
    //路径信息指针
    file_location_t* loact_ptr = &g_eg_cfg_p->location.dirlocation;
    //初始化文件选择器结果
    bool init_result = FALSE;

    //介质存在标志
    int8 uhost_exist = -1;
    int8 card_exist = -1;

    g_file_sys_id = -1;
    //卡存在否
    //禁止访问列表
    g_list_access_allow = FALSE;

    if (g_card_support == 1)
    {
        //卡存在否
        card_exist = (int8) sys_detect_disk(DRV_GROUP_STG_CARD);
    }
    if (g_uhost_support == 1)
    {
        //u盘存在否
        uhost_exist = (int8) sys_detect_disk(DRV_GROUP_STG_UHOST);
    }

    //卡和u盘都不存在
    if ((card_exist == -1) && (uhost_exist == -1))
    {
        goto switch_init_over;
    }
    if ((loact_ptr->disk == DISK_U) && (uhost_exist == -1))
    {
        //拔卡出错,且u盘存在,则切换到u盘播放
        goto switch_init_over;
    }
    if ((loact_ptr->disk == DISK_H) && (card_exist == -1))
    {
        //拔卡出错,且u盘存在,则切换到卡播放
        goto switch_init_over;
    }

    //文件选择器初始化接口的参数设置

    //盘符信息
    switch_init_param.disk = loact_ptr->disk;

    switch_init_param.fsel_type = g_fsel_type;

    //循环模式
    switch_init_param.fsel_mode = FSEL_MODE_LOOPALL;

    //bitmap 设置
    switch_init_param.file_type_bit = MUSIC_BITMAP;

    //文件选择器初始化
    g_file_sys_id = fsel_init(&switch_init_param, 0);
    //初始化成功
    if (g_file_sys_id != -1)
    {
        //设置上次退出的文件
        init_result = fsel_set_location(loact_ptr);
        if (init_result == TRUE)
        {
            //比较后缀名
            vfs_get_name(g_file_sys_id, exname, 0);
            if (libc_strncmp(exname, loact_ptr->file_info.file_extend_info.file_ext, sizeof(exname)) != 0)
            {
                init_result = FALSE;
            }
        }

        //设置失败
        if (init_result == FALSE)
        {
            //清除断点信息
            libc_memset(&g_eg_cfg_p->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

            //获取当前模式下的第一个可播放文件
            init_result = fsel_get_nextfile(loact_ptr->file_info.file_extend_info.file_ext);
            //找到可播放文件
            if (init_result == TRUE)
            {
                //获取当前文件的location信息
                init_result = fsel_get_location(loact_ptr);
            }
        }
#ifdef MENGINE_ID3_SUPPORT
        if(init_result == TRUE)
        {
            mengine_getid3_done(&g_eg_cfg_p->location);
        }
#endif
#ifdef DIR_NAME_SUPPORT
        if(init_result == TRUE)
        {
            fsel_get_dir_name(g_dir_name_buf, sizeof(g_dir_name_buf) / 2 - 2, &(g_eg_cfg_p->location.dirlocation));
        }
#endif
    }

    switch_init_over:
    if (init_result == FALSE)
    {
        //没有可播放文件
        //loact_ptr->file_total = 0;

        //设置状态为出错无文件
        g_eg_status_p->err_status = EG_ERR_NO_FILE;
    }
    return init_result;
}

//切换磁盘
app_result_e mengine_switch_disk(void* msg_ptr)
{
    //存放目标磁盘盘符的数据指针
    private_msg_t* data_ptr = (private_msg_t *) msg_ptr;
    uint8 dest_disk;
    bool ret_val;
    uint8 temp_buf[4];

    //升频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL10, FREQ_NULL);

    temp_buf[0] = 'W';
    temp_buf[1] = 'A';
    temp_buf[2] = 'V';
    temp_buf[3] = 0;

    g_eg_status_p->err_status = EG_ERR_NONE;

    //停止当前歌曲播放
    stop(STOP_NORMAL);
    if (g_file_sys_id != -1)
    {
        vfs_get_name(g_file_sys_id, g_eg_cfg_p->name_buf,
                sizeof(g_eg_cfg_p->name_buf) / 2 - 2);
#ifdef CHECK_BK_DIR
        fsel_get_dir_name(g_eg_cfg_p->dir_name, sizeof(g_eg_cfg_p->dir_name) / 2 - 2,
                &g_eg_cfg_p->location.dirlocation);
#endif

        //关闭文件选择器
        fsel_exit();
    }

    //禁止访问列表
    //g_list_access_allow = FALSE;

    //获取目标磁盘盘符
    dest_disk = *(uint8*) (data_ptr->msg.content.addr);
    //保存断点
    mengine_vram_write(g_eg_cfg_p->location.dirlocation.disk);
    sys_vm_read(&s_cfg,
            (dest_disk == DISK_H) ? VM_CARD_BKINFO : VM_UHOST_BKINFO,
            sizeof(s_cfg));
    //vram断点值
    libc_memcpy(&g_eg_cfg_p->location, &s_cfg.location, sizeof(mengine_bkinfo_t));

    g_fsel_type = FSEL_TYPE_COMMONDIR;

    g_eg_cfg_p->location.dirlocation.disk = dest_disk;

    ret_val = switch_disk_file_init();

    if (ret_val == TRUE)
    {
        if (g_eg_cfg_p->bk_infor.bp_time_offset != 0)
        {
            //修改插拔磁盘时wav的文件位置信息出错导致断点无法播放的问题
            if (libc_strncmp(g_eg_cfg_p->location.dirlocation.file_info.file_extend_info.file_ext, temp_buf, 4) == 0)
            {
                //清除文件位置信息
                g_eg_cfg_p->bk_infor.bp_file_offset = 0;
            }
            ret_val = play(PLAY_RESUME); //播放(play中会先进行setfile)
        }
        else
        {
            ret_val = play(PLAY_NORMAL); //播放(play中会先进行setfile)
        }
        //播放出错
        if (ret_val == FALSE)
        {
            error_handle();
        }
    }
    //返回
    mengine_reply_msg(msg_ptr, ret_val);

    //降频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    return RESULT_IGNORE;
}

//访问列表开关打开
app_result_e mengine_list_access_allow(void* msg_ptr)
{
    g_list_access_allow = TRUE;
    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);
    return RESULT_IGNORE;
}

//设置循环模式
app_result_e mengine_set_playmode(void* msg_ptr)
{
    //设置循环模式的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    uint8 cur_mode;

    //获取当前循环模式
    cur_mode = *(uint8*) (data_ptr->msg.content.addr);

    if (cur_mode == FSEL_MODE_RANOM)
    {
        g_shuffle_flag = SHUFFLE;
        g_repeat_mode = FSEL_MODE_LOOPALL;
    }
    else
    {
        g_shuffle_flag = SHUFFLE_NONE;
        g_repeat_mode = cur_mode;
    }

    //暂停或则停止时设置循环模式
    if ((g_eg_status_p->play_status == StopSta) || (g_eg_status_p->play_status == PauseSta))
    {
        fsel_set_mode(cur_mode); //设置循环模式
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//设置播放文件路径信息
app_result_e mengine_set_filepath(void* msg_ptr)
{
    bool ret_vals;
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    //路径信息指针
    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;
    //盘符
    uint8 disk_old = g_eg_cfg_p->location.dirlocation.disk;
    uint8 disk_new = locat_info->file_path.dirlocation.disk;

    fsel_type_e cur_type = locat_info->file_source;
    ret_vals = stop(STOP_NORMAL); //停止播放

    if (cur_type == FSEL_TYPE_SDFILE)
    {
        libc_memcpy(g_file_name, locat_info->file_path.dirlocation.file_info.file_extend_info.file_ext, 12);
        ret_vals = TRUE;
        g_mengine_enter_mode = ENTER_ALARM;
    }
    else
    {
        //录音文件播放时不会走这个分支
        //(刚才是磁盘播放，此时是换盘)
        if ((disk_new != disk_old) && (g_mengine_enter_mode == ENTER_MUSIC_START))
        {
            mengine_vram_write(disk_old);
        }
        //alarm 闹钟设置了磁盘播放
        if (g_mengine_enter_mode == ENTER_ALARM)
        {
            libc_memcpy(&g_eg_cfg_p->location, &locat_info->file_path, sizeof(file_location_t));
        }
        else
        {
            sys_vm_read(&s_cfg,
                    (disk_new == DISK_H) ? VM_CARD_BKINFO : VM_UHOST_BKINFO,
                    sizeof(s_cfg));

            //磁盘文件选择
            //会转为music播放,音乐播放时不会转为alarm
            g_mengine_enter_mode = ENTER_MUSIC_START;
            //{
            //vram断点值
            libc_memcpy(&g_eg_cfg_p->location, &s_cfg.location, sizeof(mengine_bkinfo_t));
        }

        g_fsel_type = cur_type; //更改引擎保存的文件选择类型
        g_eg_cfg_p->location.dirlocation.disk = disk_new;
        //关闭文件选择器
        fsel_exit();

        //重新初始化文件选择器
        ret_vals = switch_disk_file_init();
        if (ret_vals == TRUE)
        {
            g_eg_playinfo_p->cur_file_switch |= 0x01;
        }
    }
    g_fsel_type = cur_type; //更改引擎保存的文件选择类型
    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//获取播放介质
app_result_e mengine_get_playtype(void* msg_ptr)
{
    //设置播放类型的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    uint8 cur_type;

    if (g_mengine_enter_mode == ENTER_RECOD_PLAY)
    {
        cur_type = PLAY_RECORD_FILE;
    }
    else
    {
        //普通播放和sd播放
        cur_type = (0 << 7); //播放类型放在 bit[7]
        if (g_eg_cfg_p->location.dirlocation.disk == DISK_H)
        {
            //cur_type |= (2 << 5);//SDCARD介质，bit[5]
            cur_type = PLAY_DISK_CARD;
        }
        else
        {
            //cur_type |= (1 << 5);//UHOST介质，bit[5]
            cur_type = PLAY_DISK_UHOST;
        }
    }

    //获取当前播放类型
    *(uint8*) (data_ptr->reply->content) = cur_type;

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

app_result_e mengine_clear_fileswitch(void* msg_ptr)
{
    g_eg_playinfo_p->cur_file_switch &= (uint8) (0xfe);

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//获取文件信息，包括文件总时间，波特率，采样率等
app_result_e mengine_get_fileinfo(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy当前文件信息到消息指针指向的数据区
    libc_memcpy((data_ptr->reply->content), &g_eg_file_info, sizeof(mmm_mp_file_info_t));

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//更新文件序号
app_result_e mengine_set_sortnumber(void *msg_ptr)
{
    //消息指针
    private_msg_t *data_ptr = (private_msg_t *) msg_ptr;

    //param
    uint16 set_num;

    set_num = *(uint16 *) (data_ptr->msg.content.addr);

    //change cur number
    g_eg_cfg_p->location.dirlocation.file_num = set_num;

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//获取当前文件序号
app_result_e mengine_get_disknumber(void *msg_ptr)
{
    //消息指针
    private_msg_t *data_ptr = (private_msg_t *) msg_ptr;

    *(uint16 *) (data_ptr->reply->content) = fsel_get_fileno();

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
