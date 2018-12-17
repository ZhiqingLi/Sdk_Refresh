//依赖目录列表的文件操作
//执行时会占用PLIST_INFO_ADDR开始的0x1000空间(因为目录列表读到这个内存里)
#include "eh_fsel_entry.h"
#include "dir_list.h"

//获取当前目录的下一个文件
//strfile 后缀名
//本函数不依赖于目录列表，也不依赖于PLIST_INFO_ADDR
bool fsel_list_next(uint8* strfile)
{
    uint16 temp_num;

    //当前目录文件个数为0
    if (eh_file_total == 0)
    {
        return FALSE;
    }

    //如果shuffle打开
    if (TRUE == eh_shuffle_sw)
    {
        temp_num = fsel_shuffle_next_file();
        if (temp_num == 0)
        {
            fsel_error = FSEL_ERR_OUTOF;
            return FALSE;
        }
        return fsel_list_get_byno_all(strfile, temp_num, temp_num);
    }

    //表示当前已是最后一个，结束
    if (eh_file_no == eh_file_total)
    {
        fsel_error = FSEL_ERR_OUTOF;
        return FALSE;
    }

    if (eh_file_no == 0)
    {
        //从头开始
        //调用文件系统接口向后dir
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, 0, plist_info, fsel_init_val.file_type_bit);
    }
    else
    {
        //下一个
        //调用文件系统接口向后dir
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, 1, plist_info, fsel_init_val.file_type_bit);
    }
    if (eh_cur_ext_name != 0)
    {
        //dir成功
        eh_file_no++;
        if (strfile != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strfile = eh_cur_ext_name;
        }
    }
    else
    {
        fsel_error = FSEL_ERR_FAIL;
        return FALSE;
    }
    return TRUE;
}

//获取下一个文件,可以切换目录
//strfile 后缀名
bool fsel_list_next_layer(uint8 *strfile)
{
    bool jump_ret = TRUE;
    list_dir_info_t* cur_entry_ptr = plist_info;
    //在当前目录下查找文件
    jump_ret = fsel_list_next(strfile);
    if ((jump_ret == FALSE) && (eh_shuffle_sw == TRUE))
    {
        return jump_ret;
    }
    if (jump_ret == FALSE)
    {
        if (eh_dir_no >= plist_head->dir_total)
        {
            return FALSE;
        }
        eh_dir_no++;
        //查找下一个文件夹
        plist_info++;

        //切换到下一个目录
        jump_ret = vfs_jump_to_direntry(eh_vfs_mount, 1, plist_info, cur_entry_ptr);
        if (jump_ret == TRUE)
        {
            //获得该子文件夹的文件总数
            eh_file_total = fsel_total_file_curdir();
            eh_file_no = 0;
            // 检查当前目录是否有合适的文件
            if ((eh_file_total > 0) && (fsel_list_next(strfile) != FALSE))
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

    return jump_ret;
}

//定位到第一个目录
bool fsel_first_list_entry(void)
{
    list_dir_info_t* cur_locat = plist_info;

    //定位到第一个目录位置
    eh_dir_no = 1;

    //指向第一个目录
    plist_info = (list_dir_info_t *) (plist_start_addr + sizeof(list_head_info_t));

    //第一个目录的文件总数
    eh_file_total = fsel_total_file_curdir();

    eh_file_no = 0;

    //跳转到第一个目录
    eh_cur_ext_name = vfs_jump_to_direntry(eh_vfs_mount, PREV_FLAG, plist_info, cur_locat);
    return TRUE;
}

//根据重复模式，获取下一个文件
//strfile 后缀名
bool fsel_list_get_next(uint8 *strfile)
{
    bool ret_val;

    switch (fsel_init_val.fsel_mode)
    {
        //重复当前一个文件
        case FSEL_MODE_LOOPONE:

        if (eh_file_no_all == 0)
        {
            //默认取第一首
            ret_val = fsel_list_get_byno_all(strfile, 1, 1);
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

        //shuffle打开
        case FSEL_MODE_RANOM:
        //所有文件顺序读取，重复
        case FSEL_MODE_LOOPALL:
        if (FALSE != fsel_list_next_layer(strfile))
        {
            ret_val = TRUE;
        }
        else
        {

            if (TRUE == eh_shuffle_sw)
            {
                fsel_init_shuffle(eh_file_total_all);
                //保证shuffle的方向正确
                eh_file_no_all = 0;
            }
            //定位到第一个目录项
            fsel_first_list_entry();
            ret_val = fsel_list_next_layer(strfile);
        }
        break;

        //当前目录重复播放
        case FSEL_MODE_LOOPONEDIR:

        if (FALSE != fsel_list_next(strfile))
        {
            ret_val = TRUE;
        }
        else
        {
            eh_file_no = 0;
            //获取当前目录第一首
            ret_val = fsel_list_next(strfile);
        }
        break;

        default:
        {
            ret_val = FALSE;
            break;
        }
    }
    //文件序号
    eh_file_no_all = (eh_file_no + plist_info->begin_index - 1);
    return ret_val;
}

