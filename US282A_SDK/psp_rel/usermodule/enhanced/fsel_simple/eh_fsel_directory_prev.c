//不依赖目录列表的文件的操作
#include "eh_fsel_entry.h"

//获取当前目录中上一个文件
//strfile 后缀名
bool fsel_dir_prev(uint8* strfile)
{
    uint8 dir_type = DIR_PREV;
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
            fsel_init_shuffle(eh_file_total_all);
            return FALSE;
        }
        return fsel_dir_get_byno_all(strfile, temp_num);
    }

    //表示前一个已经目录的第一个
    if (eh_file_no == 1)
    {
        fsel_error = FSEL_ERR_OUTOF;
        return FALSE;
    }

    if (eh_file_no == 0) //表示刚进入该目录
    {
        eh_file_no = eh_file_total;
        //定位到最后一个目录项
        dir_type = DIR_TAIL;
    }
    else
    {
        eh_file_no--;
    }

    //dir文件
    eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit);
    if (eh_cur_ext_name != 0)
    {
        if ((eh_file_no_all == 0) || (eh_file_no_all == 1))
        {
            //如果从未获取过总文件数,则扫描磁盘
            if (eh_file_total_all == 0)
            {
                eh_file_total_all = fsel_get_all_dirs_total_files();
            }
            eh_file_no_all = eh_file_total_all;
        }
        else
        {
            //序号用于显示
            eh_file_no_all--;
        }
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

//定位到当前目录的最后一个文件夹
bool fsel_last_dir(void)
{
    //目录已是最大层级,返回失败
    if (eh_dir_layer >= (DIRLAYER_MAX - 1))
    {
        return FALSE;
    }
    // if no dir ,return error
#ifdef FILTER_RECORD_DIR
    if ((0 == vfs_dir(eh_vfs_mount, DIR_TAIL, NULL, EH_GET_EXT_DIR)) || (TRUE == filtrate_dir(DIR_PREV)))
#else
    if (0 == vfs_dir(eh_vfs_mount, DIR_TAIL, NULL, EH_GET_EXT_DIR))
#endif
    {
        return FALSE;
    }

    return TRUE;
}

//获取上一个文件,可以切换目录
//strfile 后缀名
bool fsel_dir_prev_layer(uint8 *strfile)
{
    //取得当前目录文件总数
    if (eh_file_total == 0)
    {
        eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);
    }

    if (FALSE == fsel_dir_prev(strfile))
    {
        //检查父目录
        ParentDiragainNext:
        //如果是根目录则返回
        if (eh_dir_layer == 0)
        {
            return FALSE;
        }
        //向上一级
        if (FALSE != vfs_cd(eh_vfs_mount, CD_BACK, NULL))
        {
            eh_dir_layer--;
#ifdef FILTER_RECORD_DIR
            //查找目录
            if ((0 != vfs_dir(eh_vfs_mount, DIR_PREV, NULL, EH_GET_EXT_DIR)) && (FALSE == filtrate_dir(DIR_PREV)))
#else 
            if (0 != vfs_dir(eh_vfs_mount, DIR_PREV, NULL, EH_GET_EXT_DIR))
#endif
            {
                //进入子目录
                vfs_cd(eh_vfs_mount, CD_SUB, NULL);
                //目录层级
                eh_dir_layer++;
                //定位到当前目录的最后一个文件夹
                while (FALSE != fsel_last_dir())
                {
                    //进入子目录
                    vfs_cd(eh_vfs_mount, CD_SUB, NULL);
                    eh_dir_layer++;
                    eh_file_total = 0;
                    eh_file_no = 0;
                }
                //获取当前目录下的文件总数
                eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);
                if (eh_file_total != 0)
                {
                    eh_file_no = 0;
                    //取得当前目录下的文件
                    return fsel_dir_prev(strfile);
                }
                else
                {
                    goto ParentDiragainNext;
                }
            }
            else
            {
                //当前目录下已找完文件夹，开始找文件
                eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);
                if (eh_file_total != 0)
                {
                    eh_file_no = 0;
                    return fsel_dir_prev(strfile);
                }
                goto ParentDiragainNext;
            }
        }
        else
        {
            //出错了
            fsel_init_sysdata();
            fsel_error = FSEL_ERR_OUTOF;
            return FALSE;
        }
    }
    return TRUE;
}

//根据重复模式获取上一个文件,可能会切换目录
//strfile 后缀名
bool fsel_dir_get_prev(uint8 *strfile)
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
            ret_val = fsel_dir_get_byno(strfile, 1);
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
        if ((eh_file_no == 0) && (eh_dir_layer == 0))
        {
            goto prev_init_get;
        }

        if (FALSE != fsel_dir_prev_layer(strfile))
        {
            ret_val = TRUE;
        }
        else //已是最前的一个，切换到磁盘目录最后一个文件
        {
            prev_init_get: fsel_init_sysdata();
            //定位到当前目录的最后一个文件夹目录项位置
            while (FALSE != fsel_last_dir())
            {
                vfs_cd(eh_vfs_mount, CD_SUB, NULL);
                eh_dir_layer++;
                eh_file_total = 0;
                eh_file_no = 0;
            }
            ret_val = fsel_dir_prev_layer(strfile);
        }
        break;

        case FSEL_MODE_LOOPONEDIR:
        //当前目录下的前一个文件
        if (FALSE != fsel_dir_prev(strfile))
        {
            ret_val = TRUE;
        }
        else
        {
            eh_file_no = 0;

            ret_val = fsel_dir_prev(strfile);
        }
        break;

        default:
        ret_val = FALSE;
        break;
    }

    return ret_val;
}

//定位到上一个目录的最后一首
//strdir 文件后缀名
bool fsel_dir_prev_dir(uint8 *strdir)
{
    //file no为该目录最后一首歌
    //前一个目录最后一个文件
    eh_file_no_all -= (eh_file_no - 1);

    //定位到当前目录第一首,切换歌曲
    eh_file_no = 1;

    if (fsel_dir_get_prev((uint8*) &eh_cur_ext_name) == TRUE)
    {
        if (eh_cur_ext_name != 0)
        {
            if (strdir != NULL)
            {
                vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
                *(uint32*) strdir = eh_cur_ext_name;
            }
            return TRUE;
        }
    }
    //如果没有找到则返回失败
    return FALSE;
}

//进入目录，定位到某个文件
//name_short 目录短名
//name_long 目录长名
//dir_direct 方向(第一个，还是最后一个)
bool fsel_enter_dir(uint8 *name_short, uint8* name_long, uint8 dir_direct)
{
    bool ret_dir = TRUE;

    //到根目录
    fsel_init_sysdata();

    //check long name dir
    //ret_dir = vfs_dir(eh_vfs_mount, DIR_HEAD, name_long, EXT_NAME_ALL_DIR);

    //check short name dir
    if (vfs_dir(eh_vfs_mount, DIR_HEAD, name_long, EXT_NAME_ALL_DIR) == FALSE)
    {
        //ret_dir = vfs_dir(eh_vfs_mount, DIR_HEAD, name_short, EXT_NAME_ALL_DIR);
        if (vfs_dir(eh_vfs_mount, DIR_HEAD, name_short, EXT_NAME_ALL_DIR) == FALSE)
        {
            ret_dir = FALSE;
        }
    }
    //enter directory
    if (ret_dir == TRUE)
    {
        //进入目录
        if (vfs_cd(eh_vfs_mount, CD_SUB, NULL) == FALSE)
        {
            ret_dir = FALSE;
        }
    }

    if (ret_dir == TRUE)
    {
        eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);

        eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_direct, NULL, fsel_init_val.file_type_bit);
        if (eh_cur_ext_name != 0)
        {
            if (dir_direct == DIR_TAIL)
            {
                //定位到最后一个文件
                eh_file_no = eh_file_total;
                eh_file_no_all = eh_file_total;
            }
            else
            {
                //定位到第一个文件
                eh_file_no = 1;
                eh_file_no_all = 1;
            }
            eh_file_total_all = eh_file_total;
        }
        else
        {
            ret_dir = FALSE;
        }
    }
    if (ret_dir == FALSE)
    {
        //回到根目录
        fsel_init_sysdata();
    }
    return ret_dir;
}

