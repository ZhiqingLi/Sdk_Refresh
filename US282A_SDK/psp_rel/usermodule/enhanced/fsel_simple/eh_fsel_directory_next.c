//不依赖目录列表的文件的操作
#include "eh_fsel_entry.h"

const uint8 root_str[] =
{ 'R', 'O', 'O', 'T', 0 };

#ifdef FILTER_RECORD_DIR
const uint8 rec_dir_name[] = "RECORD  ";

//过滤根目录下的RECORD目录
//dir_type: 型如DIR_NEXT, DIR_HEAD
bool filtrate_dir(uint8 dir_type)
{
    uint8 dir_name_buffer[32];
    uint16 name_len;
    uint8 add_len, i, j;
    uint8 *buf;

    if (eh_dir_layer != 0)
    {
        return FALSE;
    }

    name_len = vfs_get_name(eh_vfs_mount, dir_name_buffer, sizeof(dir_name_buffer) / 2 - 2);

    if ((name_len == 11) && (dir_name_buffer[0] != 0xff))
    {
        //8+3
        buf = dir_name_buffer;
        name_len = 8;
        add_len = 1;
    }
    else
    {
        if ((name_len == 8) && (dir_name_buffer[0] == 0xff))
        {
            //unciode
            buf = dir_name_buffer + 2;
            name_len = 6;
            add_len = 2;
        }
        else
        {
            return FALSE;
        }
    }

    i = 0;
    for (j = 0; j < name_len; j++)
    {
        if (rec_dir_name[j] != buf[i])
        {
            return FALSE;
        }
        i = i + add_len;
    }

    //过滤成功，找上/下一个文件夹返回
    if (0 != vfs_dir(eh_vfs_mount, dir_type, NULL, EH_GET_EXT_DIR))
    {
        //找到，跑找到文件夹流程
        return FALSE;
    }
    else
    {
        //未找到，跑递归流程
        return TRUE;
    }

}
#endif

//获取当前目录的下一个文件
//strfile 后缀名
bool fsel_dir_next(uint8* strfile)
{
    uint8 dir_type;
    uint16 temp_num;

    if (eh_file_total == 0)
    {
        return FALSE;
    }
    //如果shuffle打开则
    if (TRUE == eh_shuffle_sw)
    {
        temp_num = fsel_shuffle_next_file();
        if (temp_num == 0)
        {
            fsel_init_shuffle(eh_file_total_all);
            fsel_error = FSEL_ERR_OUTOF;
            return FALSE;
        }
        return fsel_dir_get_byno_all(strfile, temp_num);
    }

    //表示当前已是最后一个，结束
    if (eh_file_no == eh_file_total)
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
        //dir下一个
        dir_type = DIR_NEXT;
    }
    //调用文件系统接口
    eh_cur_ext_name = vfs_dir(eh_vfs_mount, dir_type, NULL, fsel_init_val.file_type_bit);
    if (0 != eh_cur_ext_name)
    {
        eh_file_no++;
        //只有一个目录,解决录音文件播放的问题
        if (eh_file_total_all == eh_file_total)
        {
            eh_file_no_all = eh_file_no;
        }
        else
        {
            //磁盘文件序号
            eh_file_no_all++;
        }

        if (strfile != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strfile = eh_cur_ext_name;
        }
        return TRUE;
    }
    else
    {
        fsel_error = FSEL_ERR_FAIL;
    }
    return FALSE;
}

//获取下一个文件,可以切换目录
//strfile 后缀名
bool fsel_dir_next_layer(uint8 *strfile)
{
    //在当前目录向下找文件
    if (eh_file_total == 0)
    {
        eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);
        eh_file_no = 0;
    }

    if (fsel_dir_next(strfile) == FALSE)
    {
        CurrentDiragain:

        if (eh_dir_layer >= (DIRLAYER_MAX - 1))
        {
            goto ParentDiragainNext;
        }
#ifdef FILTER_RECORD_DIR
        //寻找该目录的第一个子文件夹
        if ((0 != vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, EH_GET_EXT_DIR)) && (FALSE == filtrate_dir(DIR_NEXT)))
#else
        if (0 != vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, EH_GET_EXT_DIR))
#endif
        {
            if (FALSE != vfs_cd(eh_vfs_mount, CD_SUB, NULL))
            {
                //进入子文件夹，获得该子文件夹的文件总数
                eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);
                eh_dir_layer++;

                eh_file_no = 0;
                // 检查当前目录是否有合适的文件
                if ((eh_file_total > 0) && (fsel_dir_next(strfile) != FALSE))
                {
                    fsel_error = 0;
                    return TRUE;
                }
                else
                {
                    //检查子目录
                    goto CurrentDiragain;
                }
            }
            else //出错了
            {
                fsel_init_sysdata();
                fsel_error = FSEL_ERR_OUTOF;
                return FALSE;
            }
        }
        else //如果没有子文件夹，则向上跳一层，寻找当前目录的下一个目录（平级寻找）
        {
            ParentDiragainNext:

            //当前是根目录
            if (0 == eh_dir_layer)
            {
                fsel_init_sysdata();
                fsel_error = FSEL_ERR_OUTOF;
                return FALSE;
            }

            if (vfs_cd(eh_vfs_mount, CD_BACK, NULL) != FALSE)
            {
                eh_dir_layer--;
#ifdef FILTER_RECORD_DIR
                if ((vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, EH_GET_EXT_DIR) != 0) && (FALSE == filtrate_dir(DIR_NEXT)))
#else
                if (vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, EH_GET_EXT_DIR) != 0)
#endif
                {
                    vfs_cd(eh_vfs_mount, CD_SUB, NULL);
                    eh_dir_layer++;
                    //切换目录
                    eh_file_total = fsel_get_cur_dir_total_files(NULL, 0);

                    eh_file_no = 0;

                    if ((eh_file_total > 0) && (FALSE != fsel_dir_next(strfile)))
                    {
                        fsel_error = 0;
                        return TRUE;
                    }
                    else
                    {
                        // 检查子目录.
                        goto CurrentDiragain;
                    }
                }
                else
                {
                    goto ParentDiragainNext;
                }
            }
            else
            { //异常出错
                fsel_init_sysdata();
                fsel_error = FSEL_ERR_EXCEPTION;
                return FALSE;
            }
        }
    }

    return TRUE;
}

//根据重复模式获取下一个文件,可能会切换目录
//strfile 后缀名
bool fsel_dir_get_next(uint8 *strfile)
{
    bool ret_val;

    switch (fsel_init_val.fsel_mode)
    {
        //重复当前一个文件
        case FSEL_MODE_LOOPONE:

        if (eh_file_no == 0)
        {
            //默认取第一首
            ret_val = fsel_dir_get_byno(strfile, 1);
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
        //如果从未获取过总文件数
        //所有文件顺序读取，重复
        case FSEL_MODE_LOOPALL:
        if (FALSE != fsel_dir_next_layer(strfile))
        {
            ret_val = TRUE;
        }
        else
        {
            fsel_init_sysdata();
            ret_val = fsel_dir_next_layer(strfile);
        }
        break;

        //当前目录重复播放
        case FSEL_MODE_LOOPONEDIR:

        if (FALSE != fsel_dir_next(strfile))
        {
            ret_val = TRUE;
        }
        else
        {
            eh_file_no = 0;
            //获取当前目录第一首
            ret_val = fsel_dir_next(strfile);
        }
        break;

        default:
        {
            ret_val = FALSE;
            break;
        }
    }
    return ret_val;
}

//定位到下一个目录的第一个文件
//strdir 文件名
bool fsel_dir_next_dir(uint8 *strdir)
{
    //uint8 dir_type = DIR_NEXT;

    //file no为该目录最后一首歌
    //下一个目录第一个文件
    eh_file_no_all -= eh_file_no;
    eh_file_no_all += eh_file_total;

    //定位到当前目录最后一首,切换歌曲
    eh_file_no = eh_file_total;

    if (fsel_dir_get_next((uint8*) &eh_cur_ext_name) == TRUE)
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

//获取当前目录名
//strdir: 存放目录名称的指针
//name_len: 要获取的长名字符数（包括长名标记和结束符）
//location: 当前路径
bool fsel_get_dir_name(uint8 *strdir, uint8 name_len, file_location_t *location)
{
    bool ret_len = 0;

    if (strdir == NULL)
    {
        return FALSE;
    }
    //获取当前文件的路径信息
    if (location->dir_layer == 0)
    {
        libc_memcpy(strdir, root_str, sizeof(root_str));
        return TRUE;
    }
    //save current path
    vfs_file_dir_offset(eh_vfs_mount, &pdir_layer_total, &pfile_offset_total, 0);

    //set to cur dir entry
    if (vfs_cd(eh_vfs_mount, CD_BACK, NULL) == TRUE)
    {
        ret_len = (bool)vfs_get_name(eh_vfs_mount, strdir, name_len);
    }

    //back up path
    vfs_file_dir_offset(eh_vfs_mount, &pdir_layer_total, &pfile_offset_total, 1);

    return ret_len;

}
