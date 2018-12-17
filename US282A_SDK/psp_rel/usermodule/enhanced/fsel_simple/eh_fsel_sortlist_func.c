#include "eh_fsel_entry.h"
#include "dir_list.h"

//大于当前文件号，向后查找
static bool _down(uint16 num)
{
    list_dir_info_t* p_entry = plist_info;
    //跳转的个数
    uint16 gap_num;

    while (1)
    {
        if (eh_dir_no == plist_head->dir_total)
        {
            //最后一个
            break;
        }
        if ((num >= plist_info->begin_index) && (num < (plist_info + 1)->begin_index))
        {
            break;
        }

        eh_dir_no++;
        plist_info++;
    }

    if (plist_info != p_entry)
    {
        //在别的目录下
        //定位到目录(向后跳)
        if(vfs_jump_to_direntry(eh_vfs_mount, 0, plist_info, p_entry) == FALSE)
        {
            return FALSE;
        }
        //定位到文件
        gap_num = num - (plist_info->begin_index - 1);
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, gap_num, plist_info,
            fsel_init_val.file_type_bit);
        eh_file_no = gap_num;
        eh_file_total = fsel_total_file_curdir();
        return TRUE;
    }

    //在同一个目录下
    gap_num = num - eh_disk_file_no;
    eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount,
        gap_num, plist_info, fsel_init_val.file_type_bit);
    eh_file_no = num - (plist_info->begin_index - 1);
    return TRUE;
}

//小于当前文件号，向前查找
static bool _up(uint16 num)
{
    list_dir_info_t* p_entry = plist_info;
    //跳转的个数
    uint16 gap_num;

    //确定目录
    while (1)
    {
        if ((eh_dir_no == plist_head->dir_total) && (num >= plist_info->begin_index))
        {
            //最后一个
            break;
        }
        else if ((num >= plist_info->begin_index) && (num < (plist_info + 1)->begin_index))
        {
            break;
        }
        else if (eh_dir_no == 1)
        {
            //第一个
            break;
        }
        else
        {
            ;//QAC
        }

        eh_dir_no--;
        plist_info--;
    }
    if (plist_info != p_entry)
    {
        //在别的目录下
        //定位到目录(向前跳)
        if(vfs_jump_to_direntry(eh_vfs_mount, PREV_FLAG, plist_info, p_entry) == FALSE)
        {
            return FALSE;
        }

        //定位到文件
        gap_num = num - (plist_info->begin_index - 1);
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, gap_num, plist_info,
            fsel_init_val.file_type_bit);
        eh_file_no = gap_num;
        eh_file_total = fsel_total_file_curdir();

        return TRUE;
    }
    //在同一个目录下
    gap_num = eh_disk_file_no - num;
    eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, PREV_FLAG | gap_num, plist_info,
        fsel_init_val.file_type_bit);
    eh_file_no -= gap_num;
    return TRUE;
}

//根据磁盘序号定位文件
//strfile 后缀名
//num 磁盘序号
//show_num 该文件的节目序号
bool fsel_list_get_byno_all(uint8 *strfile, uint16 num, uint16 show_num)
{
    bool ret_vals = TRUE;

    //如果从未获取过总文件数,则扫描磁盘
    if (eh_file_total_all == 0)
    {
        eh_file_total_all = fsel_total_file_alldir();
    }

    //无效序号则直接返回失败
    if ((num > eh_file_total_all) || (num == 0))
    {
        fsel_error = FSEL_ERR_OUTOF;
        return FALSE;
    }

    if (eh_disk_file_no < num)
    {
        ret_vals = _down(num);
    }
    else if (eh_disk_file_no > num)
    {
        ret_vals = _up(num);
    }
    else
    {
        /*和当前的文件号相等*/
        eh_file_no = (uint16)(eh_disk_file_no - plist_info->begin_index + 1);

        if (eh_disk_file_no == 1)
        {
            /*第一个文件*/
            //定位到文件
            eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, 0, plist_info, fsel_init_val.file_type_bit);
            eh_file_no = 1;
        }
    }
    //正确(need modify)
    if (eh_cur_ext_name == 0)
    {
        ret_vals = FALSE;
    }
    if ((ret_vals == TRUE) && (eh_cur_ext_name != 0))
    {
        eh_file_no_all = show_num;
        //update real num for next show
        eh_disk_file_no = num;

        if (strfile != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strfile = eh_cur_ext_name;
        }
    }
    return ret_vals;
}



//根据循环模式、节目表，获取下一个文件
//strfile 后缀名
bool fsel_sortlist_get_next(uint8 *strfile)
{
    //磁盘中的位置
    uint16 real_num, temp_num, dir_num;
    uint16 loopi = 0;
    bool ret_val = FALSE;

    switch (fsel_init_val.fsel_mode)
    {
        //重复当前一个文件
        case FSEL_MODE_LOOPONE:
        if (eh_file_no_all == 0)
        {
            //默认取第一首
            //eh_file_no_all = 1;
            real_num = fsel_sortlsit_vram_read(eh_file_no_all);
            ret_val = fsel_list_get_byno_all(strfile, real_num, 1);
        }
        else
        {
            //之前已取,直接返回ext
            if (strfile != NULL)
            {
                vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
                *(uint32*) strfile = eh_cur_ext_name;
            }
            ret_val = TRUE;
        }
        break;
        //当前目录重复播放(sort 没有目录概念,该循环方式无效)
        case FSEL_MODE_LOOPONEDIR:
        //目录序号
        dir_num = eh_file_no;
        //向下切换
        dir_num++;
        //check
        temp_num = eh_file_no_all;

        temp_num++;

        if (dir_num <= eh_file_total)
        {
            real_num = fsel_sortlsit_vram_read(temp_num);
        }
        else
        {
            //该目录第一首
            temp_num -= eh_file_total;
            //eh_file_no = 1;
            //获取文件
            real_num = fsel_sortlsit_vram_read(temp_num);
        }

        ret_val = fsel_list_get_byno_all(strfile, real_num, temp_num);
        break;

        //shuffle打开
        case FSEL_MODE_RANOM:
        //所有文件顺序读取，重复
        case FSEL_MODE_LOOPALL:
        temp_num = eh_file_no_all;
        while ((ret_val == FALSE) && (loopi < eh_file_total_all))
        {
            if (TRUE == eh_shuffle_sw)
            {
                temp_num = fsel_shuffle_next_file();
            }
            else
            {
                temp_num++;
            }

            if ((temp_num > eh_file_total_all) || (temp_num == 0))
            {
                temp_num = 1;
                //real_num = fsel_sortlsit_vram_read(temp_num);
                if (TRUE == eh_shuffle_sw)
                {
                    //保证shuffle的方向正确
                    fsel_init_shuffle(eh_file_total_all);
                    //shuffle first one
                    temp_num = fsel_shuffle_next_file();
                }
            }

            real_num = fsel_sortlsit_vram_read(temp_num);

            ret_val = fsel_list_get_byno_all(strfile, real_num, temp_num);

            loopi++;
            if (ret_val == FALSE)
            {
                //set to first entry
                fsel_first_list_entry();

                eh_file_no_all = 0;

                eh_disk_file_no = eh_file_no_all;

                temp_num = eh_file_no_all;
            }
        }

        break;

        default:
        ret_val = FALSE;
        break;
    }

    return ret_val;
}

//获取上一个文件
//strfile 后缀名
bool fsel_sortlist_get_prev(uint8 *strfile)
{
    bool ret_val = FALSE;

    uint16 loopi = 0;

    //磁盘中的位置
    uint16 real_num, temp_num, dir_num;

    switch (fsel_init_val.fsel_mode)
    {
        case FSEL_MODE_LOOPONE:
        if (eh_file_no_all == 0)
        {
            //取第一个文件
            //eh_file_no_all = 1;
            real_num = fsel_sortlsit_vram_read(0);
            ret_val = fsel_list_get_byno_all(strfile, real_num, 1);
        }
        else
        {
            if (strfile != NULL)
            {
                vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
                *(uint32*) strfile = eh_cur_ext_name;
            }
            ret_val = TRUE;
        }
        break;
        //当前目录重复播放(sort 没有目录概念,该循环方式无效)
        case FSEL_MODE_LOOPONEDIR:
        //read vram
        dir_num = eh_file_no;

        temp_num = eh_file_no_all;

        if (dir_num > 0)
        {
            dir_num--;
            //向下切换
            temp_num--;
        }

        if (dir_num > 0)
        {
            real_num = fsel_sortlsit_vram_read(temp_num);
        }
        else
        {
            //eh_file_no = eh_file_total;

            temp_num += eh_file_total;

            real_num = fsel_sortlsit_vram_read(temp_num);
        }
        ret_val = fsel_list_get_byno_all(strfile, real_num, temp_num);
        break;

        //shuffle打开
        case FSEL_MODE_RANOM:
        //全部重复
        case FSEL_MODE_LOOPALL:
        temp_num = eh_file_no_all;

        while ((ret_val == FALSE) && (loopi < eh_file_total_all))
        {
            if (TRUE == eh_shuffle_sw)
            {
                temp_num = fsel_shuffle_next_file();
            }
            else
            {
                //不是第一首歌,继续向前切
                if (temp_num > 0)
                {
                    temp_num--;
                }
            }

            if ((temp_num > eh_file_total_all) || (temp_num == 0))
            {
                //保证shuffle的方向正确
                temp_num = eh_file_total_all;

                if (TRUE == eh_shuffle_sw)
                {
                    fsel_init_shuffle(eh_file_total_all);
                    //shuffle first one
                    temp_num = fsel_shuffle_prev_file();
                }
            }

            real_num = fsel_sortlsit_vram_read(temp_num);

            ret_val = fsel_list_get_byno_all(strfile, real_num, temp_num);

            loopi++;

            if (ret_val == FALSE)
            {
                //定位到最后一个文件夹目录项位置
                fsel_last_list_entry();

                eh_file_no_all = 1;

                eh_disk_file_no = 1;

                temp_num = eh_file_no_all;
            }
        }
        break;

        default:

        ret_val = FALSE;
        break;

    }
    //文件序号
    return ret_val;
}

//查找下一个目录
bool __section__(".text.bank2") fsel_sortlist_next_dir(uint8 *strdir)
{
    //list have no dir concept so we switch to next file
    fsel_sortlsit_vram_read(eh_file_no_all);
    return fsel_list_next_dir(strdir);
}

//查找上一个目录
//strfile 后缀名
bool __section__(".text.bank2") fsel_sortlist_prev_dir(uint8 *strdir)
{
    fsel_sortlsit_vram_read(eh_file_no_all);
    return fsel_list_prev_dir(strdir);
}

//统计磁盘文件的总数
uint16 __section__(".text.bank2") fsel_total_file_sortlist(void)
{
    fsel_sortlsit_vram_read(0);
    return fsel_total_file_alldir();
}

