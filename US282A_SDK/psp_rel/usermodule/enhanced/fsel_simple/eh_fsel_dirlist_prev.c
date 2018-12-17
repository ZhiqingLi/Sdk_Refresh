//依赖目录列表的文件操作
//执行时会占用PLIST_INFO_ADDR开始的0x1000空间(因为目录列表读到这个内存里)
#include "eh_fsel_entry.h"
#include "dir_list.h"

//获取当前目录的上一个文件
//strfile 后缀名
bool fsel_list_prev(uint8* strfile)
{
    uint16 temp_num;

    //文件总数为0
    if (eh_file_total == 0)
    {
        return FALSE;
    }
    //如果是shuffle
    if (TRUE == eh_shuffle_sw)
    {
        //shuffle
        temp_num = fsel_shuffle_prev_file();
        if (temp_num == 0)
        {
            return FALSE;
        }
        return fsel_list_get_byno_all(strfile, temp_num, temp_num);
    }

    if (eh_file_no == 1)
    {
        fsel_error = FSEL_ERR_OUTOF;
        return FALSE;
    }

    if (eh_file_no == 0)
    {
        //表示刚进入该目录
        eh_file_no = eh_file_total;

        //进入该目录,查找该目录最后一个文件
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, PREV_FLAG, plist_info, fsel_init_val.file_type_bit);

    }
    else
    {
        eh_file_no--;
        //从当前位置查找前一个文件
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, PREV_FLAG | 0x01, plist_info,
                fsel_init_val.file_type_bit);
    }
    if (eh_cur_ext_name != 0)
    {
        if (strfile != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strfile = eh_cur_ext_name;
        }
        return TRUE;
    }

    fsel_error = FSEL_ERR_FAIL;
    return FALSE;
}

//获取上一个文件,可以切换目录
//strfile 后缀名
bool fsel_list_prev_layer(uint8 *strfile)
{
    bool jump_dir_ret = TRUE;

    list_dir_info_t* cur_dir_ptr = plist_info;

    //取得当前目录文件总数
    if (eh_file_total == 0)
    {
        eh_file_total = fsel_total_file_curdir();
    }

    if (FALSE == fsel_list_prev(strfile))
    {
        if (eh_dir_no == 1)
        {
            return FALSE;
        }

        eh_dir_no--;

        //查找上一个文件夹
        plist_info--;

        jump_dir_ret = vfs_jump_to_direntry(eh_vfs_mount, PREV_FLAG | 0x01, plist_info, cur_dir_ptr);

        if (jump_dir_ret == TRUE)
        {
            //获得该子文件夹的文件总数
            eh_file_total = fsel_total_file_curdir();
            eh_file_no = 0;
            //检查当前目录是否有合适的文件
            if ((eh_file_total > 0) && (fsel_list_prev(strfile) != FALSE))
            {
                fsel_error = 0;
                return TRUE;
            }
            else
            {
                fsel_error = FSEL_ERR_EXCEPTION;
                return FALSE;
            }
        }
    }
    return jump_dir_ret;
}

//根据重复模式，获取上一个文件
//strfile 后缀名
bool fsel_list_get_prev(uint8 *strfile)
{
    bool ret_val;

    switch (fsel_init_val.fsel_mode)
    {
        case FSEL_MODE_LOOPONE:
        if (eh_file_total == 0)
        {
            ret_val = FALSE;
        }
        else if (eh_file_no == 0)
        {
            //取第一个文件
            ret_val = fsel_list_get_byno_all(strfile, 1, 1);
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

        //shuffle打开
        case FSEL_MODE_RANOM:
        //全部重复
        case FSEL_MODE_LOOPALL:
        if ((eh_file_no == 0) && (eh_dir_no == 0))
        {
            goto get_init_prev;
        }

        if (FALSE != fsel_list_prev_layer(strfile))
        {
            ret_val = TRUE;
        }
        else //已是最前的一个，切换到磁盘目录最后一个文件
        {
            get_init_prev:

            if (TRUE == eh_shuffle_sw)
            {
                fsel_init_shuffle(eh_file_total_all);
                //保证shuffle的方向正确
                eh_file_no_all = eh_file_total_all;
            }

            //定位到最后一个文件夹目录项位置
            fsel_last_list_entry();

            ret_val = fsel_list_prev_layer(strfile);
        }
        break;

        case FSEL_MODE_LOOPONEDIR:

        //当前目录下的前一个文件
        if (FALSE != fsel_list_prev(strfile))
        {
            ret_val = TRUE;
        }
        else
        {
            eh_file_no = 0;

            ret_val = fsel_list_prev(strfile);
        }

        default:

        ret_val = FALSE;
        break;

    }
    //文件序号
    eh_file_no_all = (eh_file_no + plist_info->begin_index - 1);
    return ret_val;
}

