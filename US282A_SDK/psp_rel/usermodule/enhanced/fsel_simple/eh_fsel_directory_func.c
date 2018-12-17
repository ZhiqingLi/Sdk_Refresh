//不依赖目录列表的文件的操作
#include "eh_fsel_entry.h"
#include "dir_list.h"

//统计当前目录下文件的总数
//dir_info：文件位置信息(8字节)
//check_flag: 如果为1，表示：顺便更新eh_file_no
uint16 fsel_get_cur_dir_total_files(uint8* dir_info, uint8 check_flag)
{
    uint8 temp_offset_buf[8];
    uint8 dir_type = DIR_HEAD;
    uint16 total = 0;

    //保存当前路径
    vfs_file_dir_offset(eh_vfs_mount, &pdir_layer_buffer, &pfile_offset_buffer, 0);

    do
    {
        if (0 != vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit))
        {
            total++;
            if(total > SUPPORT_MUSIC_FILE)//超过文件支持数
            {
                break;
            }
            if (check_flag == 1)
            {
                //比较8字节offset
                vfs_file_dir_offset(eh_vfs_mount, NULL, &temp_offset_buf, 0);
                if (libc_memcmp(dir_info, &temp_offset_buf, 8) == 0)
                {
                    eh_file_no = total;
                    check_flag = 0;
                }
            }
        }
        else
        {
            break;
        }
        dir_type = DIR_NEXT;
    } while (1);

    //恢复当前路径
    vfs_file_dir_offset(eh_vfs_mount, &pdir_layer_buffer, &pfile_offset_buffer, 1);

    if (check_flag == 1)
    {
        eh_file_no = 0;
    }

    return total;
}

//获取当前目录下的下个文件
bool fsel_get_cur_dir_next_file(void)
{
    uint8 dir_type;
    if (eh_file_no == 0)
    {
        //序号为0则从头开始搜索
        dir_type = DIR_HEAD;
    }
    else
    {
        //序号不为0则dir下一个
        dir_type = DIR_NEXT;
    }
    //dir文件
    if (vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit) != 0)
    {
        //找到文件则序号增加
        eh_file_no++;
        return TRUE;
    }
    return FALSE;
}

//在磁盘范围内找下一个文件
bool fsel_get_disk_dir_next_file(void)
{
    //在当前目录向下找文件
    if (FALSE == fsel_get_cur_dir_next_file())
    {
        CurrentDiragain:
        if (eh_dir_layer >= (MAX_DIR_LAYER - 1)) //如果当前是8级目录就不找子文件夹
        {
            goto ParentDiragainNext;
        }
#ifdef FILTER_RECORD_DIR
        //寻找该目录的第一个子文件夹
        if ((vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, EXT_NAME_ALL_DIR) != 0) && (FALSE == filtrate_dir(DIR_NEXT)))
#else
        if (vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, EXT_NAME_ALL_DIR) != 0)
#endif
        {
            //进入子文件夹，获得该子文件夹的文件总数
            if (FALSE != vfs_cd(eh_vfs_mount, CD_SUB, NULL))
            {
                //目录层级增加
                eh_dir_layer++;
                //当前目录下的文件序号归零
                eh_file_no = 0;
                // 检查当前目录是否有合适的文件
                if (FALSE != fsel_get_cur_dir_next_file())
                {
                    return TRUE;
                }
                else //检查子目录
                {
                    goto CurrentDiragain;
                }
            }
            else //出错了
            {
                return FALSE;
            }
        }
        else //如果没有子文件夹，则向上跳一层，寻找与当前目录平级的下一个目录（平级寻找）
        {
            ParentDiragainNext:

            //如果当前是根目录则返回
            if (eh_dir_layer == 0)
            {
                return FALSE;
            }
            //跳到父目录
            if (FALSE == vfs_cd(eh_vfs_mount, CD_BACK, NULL))
            {
                //异常出错
                return FALSE;
            }
            //目录层级减
            eh_dir_layer--;
#ifdef FILTER_RECORD_DIR
            if ((vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, EXT_NAME_ALL_DIR) != 0) && (FALSE == filtrate_dir(DIR_NEXT)))
#else
            if (vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, EXT_NAME_ALL_DIR) != 0)
#endif 
            {
                //进入目录,查找该目录下的文件
                if (FALSE != vfs_cd(eh_vfs_mount, CD_SUB, NULL))
                {
                    //目录层级增加
                    eh_dir_layer++;
                    //文件序号为0
                    eh_file_no = 0;

                    if (FALSE != fsel_get_cur_dir_next_file())
                    {
                        return TRUE;
                    }
                    else
                    {
                        // 检查子目录.
                        goto CurrentDiragain;
                    }
                }
                else //出错了
                {
                    return FALSE;
                }
            }
            else
            {
                goto ParentDiragainNext;
            }
        }
    }
    return TRUE;
}

//磁盘中一共有多少文件
uint16 fsel_get_all_dirs_total_files(void)
{
    uint8 temp_dir_layer;
    //bool save_shuffle_flag;
    uint16 total;
    //uint32 ext_name;

    uint16 save_cur_no, save_cur_total;
    uint16 save_file_no_all;

    if (eh_vfs_mount == -1)
    {
        return 0;
    }
    //保存当前目录的文件序号和总数
    save_cur_no = eh_file_no;
    save_cur_total = eh_file_total;
    temp_dir_layer = eh_dir_layer;

    save_file_no_all = eh_file_no_all;

    //保存当前路径
    vfs_file_dir_offset(eh_vfs_mount, &pdir_layer_total, &pfile_offset_total, 0);

    fsel_init_sysdata();

    total = 0;
    do
    {
        //在磁盘范围内找下一个文件
        if (fsel_get_disk_dir_next_file() != FALSE)
        {
            total++;
            if (total >= SUPPORT_MUSIC_FILE)
            {
                break;
            }
        }
        else
        {
            break;
        }
    } while (1);

    //恢复当前路径
    vfs_file_dir_offset(eh_vfs_mount, &pdir_layer_total, &pfile_offset_total, 1);

    //eh_shuffle_sw = save_shuffle_flag;

    if ((TRUE == eh_shuffle_sw) && (total > 0))
    {
        fsel_init_shuffle(total);
    }

    //恢复当前目录的序号和总数
    eh_file_no = save_cur_no;
    eh_file_total = save_cur_total;
    eh_dir_layer = temp_dir_layer;

    eh_file_no_all = save_file_no_all;

    eh_file_total_all = total;

    return total;

}

//根据序号获取文件(用于数字点歌)
//strfile 后缀名
//num 磁盘中的序号
bool fsel_dir_get_byno_all(uint8 *strfile, uint16 num)
{
    uint8 dir_type;

    //save shuffle flag;
    uint8 save_shuffle_flag;

    bool ret_val = TRUE;

    uint16 dir_num, i;

    //如果从未获取过总文件数,则扫描磁盘
    if (eh_file_total_all == 0)
    {
        eh_file_total_all = fsel_get_all_dirs_total_files();
    }

    //无效序号则直接返回失败
    if ((num > eh_file_total_all) || (num == 0))
    {
        fsel_error = FSEL_ERR_OUTOF;
        return FALSE;
    }
    save_shuffle_flag = eh_shuffle_sw;
    eh_shuffle_sw = FALSE;
    if (eh_file_no_all < num)
    {
        //大于当前文件号，向后Dir
        dir_num = num - eh_file_no_all;
        i = 0;
        while (i < dir_num)
        {
            //向后dir文件
            if (fsel_dir_next_layer((uint8 *) &eh_cur_ext_name) != FALSE)
            {
                i++;
            }
            else
            {
                ret_val = FALSE;
                break;
            }
        }
    }
    else if (eh_file_no_all > num)
    {
        //小于当前文件号，向前Dir
        dir_num = eh_file_no_all - num;

        i = 0;

        while (i < dir_num)
        {
            //向前dir文件
            if (fsel_dir_prev_layer((uint8*) &eh_cur_ext_name) != FALSE)
            {
                i++;
            }
            else
            {
                ret_val = FALSE;
                break;
            }
        }
    }
    else
    {
        //和当前的文件号相等
        if (eh_file_no_all == 1)
        {
            //第一个文件
            dir_type = DIR_HEAD;
            eh_file_no = 0;

            eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit);
            if (0 == eh_cur_ext_name)
            {
                ret_val = FALSE;
            }
        }
    }
    if (ret_val == TRUE)
    {
        eh_file_no_all = num;
        if (strfile != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strfile = eh_cur_ext_name;
        }
    }
    eh_shuffle_sw = save_shuffle_flag;
    return ret_val;
}

//根据序号在当前目录下获取文件
//strfile 后缀名
//num 文件在磁盘中的序号
//注意：会改变eh_file_no的值;
//          取文件时调用前不能改变前一首的eh_file_no，
//          因为该接口要根据eh_file_no走不同的流程
bool fsel_dir_get_byno(uint8 *strfile, uint16 num)
{
    uint8 dir_type;

    uint16 dir_num;

    if ((num > eh_file_total) || (num == 0))
    {
        fsel_error = FSEL_ERR_OUTOF;
        return FALSE;
    }

    if (eh_file_no == 0)
    {
        //从头开始dir
        dir_type = DIR_HEAD;
    }
    else
    {
        //取下一个
        dir_type = DIR_NEXT;
    }
    if (eh_file_no < num)
    {
        //大于当前文件号，向后Dir
        dir_num = num - eh_file_no;

        eh_file_no = 0;
        while (eh_file_no < dir_num)
        {
            //向后dir文件
            eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit);
            //找到文件
            if (0 != eh_cur_ext_name)
            {
                eh_file_no++;
            }
            else
            {
                return FALSE;
            }
            dir_type = DIR_NEXT;
        }
    }
    else if (eh_file_no > num)
    {
        //小于当前文件号，向前Dir
        dir_type = DIR_PREV;

        dir_num = eh_file_no - num;

        eh_file_no = 0;

        while (eh_file_no < dir_num)
        {
            //向前dir文件
            eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit);

            if (0 != eh_cur_ext_name)
            {
                eh_file_no++;
            }
            else
            {
                return FALSE;
            }
        }
    }
    else
    {
        //和当前的文件号相等
        if (eh_file_no == 1)
        {
            //第一个文件
            dir_type = DIR_HEAD;
            eh_file_no = 0;

            eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit);
            if (0 == eh_cur_ext_name)
            {
                return FALSE;
            }
        }
    }

    eh_file_no = num;
    eh_file_no_all = eh_file_no;
    if (strfile != NULL)
    {
        vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
        *(uint32*) strfile = eh_cur_ext_name;
    }
    return TRUE;
}



//定位到文件
//location 存放路径信息指针
bool fsel_dir_set_location(file_location_t *location)
{
    bool ret_val;

    //如果输入的location为无效值
    if ((location == NULL) || (*(uint32*) &location->file_info.file_extend_info.file_ext == 0))
    {
        //当前目录下的文件总数
        eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);
        //文件序号
        eh_file_no = 0;
        return FALSE;
    }
    //定位文件
    ret_val = vfs_file_dir_offset(eh_vfs_mount, &location->dir_layer_info, \
    &location->file_info.file_extend_info.cluster_no, 1);

    if (ret_val != FALSE)
    {
        vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
        if (*(uint32*) &location->file_info.file_extend_info.file_ext != eh_cur_ext_name)
        {
            ret_val = FALSE;
        }
        else
        {
            //eh_file_no 在total_file_cur里赋值
            eh_file_total = fsel_get_cur_dir_total_files((uint8*) &location->file_info.file_extend_info.cluster_no, 1);
            eh_file_no_all = location->file_num;
            //eh_file_total_all = location->file_total;

        }
    }

    if ((ret_val == FALSE) || (eh_file_no == 0))
    {
        //目录空或目录信息有误，回到根目录
        fsel_init_sysdata();

        eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);

        return FALSE;
    }

    eh_dir_layer = location->dir_layer;

    //赋值更新eh_cur_ext_name
    libc_memcpy(&eh_cur_ext_name, location->file_info.file_extend_info.file_ext, 4);

    return TRUE;
}

