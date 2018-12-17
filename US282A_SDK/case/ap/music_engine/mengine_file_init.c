#include "app_mengine.h"
#include "audio_device.h"

//初始化文件选择器,定位到设定文件or当前模式第一个可播放文件
bool mengine_file_init(void)
{
    fsel_param_t init_param; //文件选择器初始化参数
    //后缀名buffer
    uint8 exname[4];
    //路径信息指针
    plist_location_t *loaction_ptr;
    //文件选择类型
    //初始化文件选择器结果
    bool init_result = FALSE;
    //介质存在标志
    int8 uhost_exist = -1;
    int8 card_exist = -1;
    uint8 temp_buf[4];

    for (;;)
    {
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

        //断点信息就放在g_eg_cfg_p->location
        loaction_ptr = &(g_eg_cfg_p->location.plist_location);

        //保证盘符正确
        if (loaction_ptr->disk == DISK_C)
        {
            loaction_ptr->disk = DISK_U;
        }
        //卡和u盘都不存在
        if ((card_exist == -1) && (uhost_exist == -1))
        {
            break;
        }
        if (g_mengine_enter_mode == ENTER_RECOD_PLAY)
        {
            if (uhost_exist == -1)
            {
                //u盘不存在
                loaction_ptr->disk = DISK_H;
            }
            else if (card_exist == -1)
            {
                //卡不存在
                loaction_ptr->disk = DISK_U;
            }
            else
            {
                ; //do nothing
            }
        }
        if ((uhost_exist == -1) && (loaction_ptr->disk == DISK_U))
        {
            break;
        }

        if ((card_exist == -1) && (loaction_ptr->disk == DISK_H))
        {
            break;
        }

        //文件选择器初始化接口的参数设置

        //盘符信息
        init_param.disk = loaction_ptr->disk;

        //文件选择类型
        init_param.fsel_type = g_fsel_type;

        //循环模式
        init_param.fsel_mode = FSEL_MODE_LOOPALL;

        //bitmap 设置
        init_param.file_type_bit = MUSIC_BITMAP;

        //文件选择器初始化
        g_file_sys_id = fsel_init(&init_param, 0);
        if (g_file_sys_id == -1)
        {
            break;
        }

        //为了BUG：文件还能定位到，但实际已经格式化了
        if (vfs_dir(g_file_sys_id, DIR_NEXT, NULL, EXT_NAME_ALL) == 0)
        {
            _clear_vm_deal(loaction_ptr->disk);

            break;
        }

        if (g_mengine_enter_mode == ENTER_RECOD_PLAY)
        {
            uint8 rec_name_less[8 + 1];
            uint8 rec_name_more[16 + 1];

            libc_memset(rec_name_less, 0x00, sizeof(rec_name_less));
            libc_memset(rec_name_more, 0x00, sizeof(rec_name_more));
            com_load_share_data(COM_SHARE_DATA_REC_FOLDER, rec_name_less, sizeof(rec_name_less) - 1, 0);
            com_load_share_data(COM_SHARE_DATA_REC_FOLDER, rec_name_more, sizeof(rec_name_more) - 1, 1);

            //设置进入record目录,定位到第一个文件
            init_result = fsel_enter_dir((uint8 *) &rec_name_less, (uint8 *) &rec_name_more, DIR_TAIL);
            if (init_result == FALSE)
            {
                //未找到record 目录
                break;
            }
            init_result = fsel_get_location(loaction_ptr);
            //清除断点信息
            libc_memset(&g_eg_cfg_p->bk_infor, 0, sizeof(mmm_mp_bp_info_t));
        }

        //record if bk is ok for front apk update plist and if need scan
        g_eg_status_p->first_file_status = FIRST_PLAY_FROM_BK;

        for (;;)
        {
            if (init_result == TRUE)
            {
                break;
            }

            //设置上次退出的文件
            init_result = fsel_set_location(loaction_ptr);

            if (init_result == FALSE)
            {
                break;
            }

            //比较后缀名
            vfs_get_name(g_file_sys_id, exname, 0);

            if (libc_strncmp(exname, loaction_ptr->file_info.file_extend_info.file_ext, sizeof(exname)) != 0)
            {
                init_result = FALSE;
                break;
            }

            temp_buf[0] = 'W';
            temp_buf[1] = 'A';
            temp_buf[2] = 'V';
            temp_buf[3] = 0;
            //修改插拔磁盘时wav的文件位置信息出错导致断点无法播放的问题
            if (libc_strncmp(exname, temp_buf, sizeof(exname)) == 0)
            {
                //清除文件位置信息
                g_eg_cfg_p->bk_infor.bp_file_offset = 0;
            }
            vfs_get_name(g_file_sys_id, g_file_name, sizeof(g_file_name) / 2 - 2);

            if (libc_strncmp(g_file_name, g_eg_cfg_p->name_buf, sizeof(g_file_name)) != 0)
            {
                init_result = FALSE;
                break;
            }

#ifdef CHECK_BK_DIR
            {
                uint8 dir_name[sizeof(g_eg_cfg_p->dir_name)];

                fsel_get_dir_name(dir_name, sizeof(dir_name) / 2 - 2, &g_eg_cfg_p->location.dirlocation);
                if (libc_strncmp(dir_name, g_eg_cfg_p->dir_name, sizeof(dir_name)) != 0)
                {
                    init_result = FALSE;
                }
            }
#endif
            break;
        }

        //设置失败
        if (init_result == FALSE)
        {
            //清除断点信息
            libc_memset(&g_eg_cfg_p->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

            //获取当前模式下的第一个可播放文件
            init_result = fsel_get_nextfile(loaction_ptr->file_info.file_extend_info.file_ext);
            //找到可播放文件
            if (init_result == TRUE)
            {
                //获取当前文件的location信息
                init_result = fsel_get_location(loaction_ptr);
            }
            //first play file need form start，bk is error
            g_eg_status_p->first_file_status = FIRST_PLAY_FROM_START;
        }

        if (init_result == TRUE)
        {
#ifdef MENGINE_ID3_SUPPORT
            mengine_getid3_done(&g_eg_cfg_p->location);
#endif
#ifdef DIR_NAME_SUPPORT
            fsel_get_dir_name(g_dir_name_buf, sizeof(g_dir_name_buf) / 2 - 2, &(g_eg_cfg_p->location.dirlocation));
#endif
            vfs_get_name(g_file_sys_id, g_eg_cfg_p->name_buf, sizeof(g_eg_cfg_p->name_buf) / 2 - 2);
#ifdef CHECK_BK_DIR
            fsel_get_dir_name(g_eg_cfg_p->dir_name, sizeof(g_eg_cfg_p->dir_name) / 2 - 2,
                    &g_eg_cfg_p->location.dirlocation);
#endif
        }

        break;
    }
    if ((init_result == FALSE) || ((loaction_ptr->disk != g_open_disk) && (g_mengine_enter_mode != ENTER_RECOD_PLAY)))
    {
        //没有可播放文件
        //loaction_ptr->file_total = 0;

        init_result = FALSE;

        //设置状态为出错无文件
        g_eg_status_p->err_status = EG_ERR_NO_FILE;
    }

    return init_result;
}

