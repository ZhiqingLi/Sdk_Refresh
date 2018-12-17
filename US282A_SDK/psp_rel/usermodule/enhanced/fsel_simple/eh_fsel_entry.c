#include "eh_fsel_entry.h"
#include "dir_list.h"

static const fsel_get_next_p fsel_get_next_array[2] =
{
    fsel_dir_get_next, //“磁盘顺序”模式下选择下一个文件
    fsel_sortlist_get_next //“节目单”模式下选择下一个文件
};

static const fsel_get_prev_p fsel_get_prev_array[2] =
{
    fsel_dir_get_prev, //“磁盘顺序”模式下选择上一个文件
    fsel_sortlist_get_prev //“节目单”模式下选择上一个文件
};

static const fsel_get_byno_p fsel_get_byno_array[2] =
{
    fsel_dir_get_byno_all, //“磁盘顺序”模式下根据序号选择文件
    fsel_sortlist_get_byno_all //“节目单”模式下根据序号选择文件
};

static const fsel_set_location_p fsel_set_location_array[2] =
{
    (void *) fsel_dir_set_location, //“磁盘顺序”模式下设置路径信息
    (void *) fsel_sortlist_set_location //“节目单”模式下设置路径信息
};

static const fsel_get_location_p fsel_get_location_array[2] =
{
    (void *) fsel_dir_get_location, //“磁盘顺序”模式下获取路径信息
    (void *) fsel_sortlist_get_location //“节目单”模式下获取路径信息
};

static const fsel_get_nextdir_p fsel_get_nextdir_array[2] =
{
    fsel_dir_next_dir, //“磁盘顺序”模式下切换下一个目录
    fsel_sortlist_next_dir //“节目单”模式下切换下一个目录
};
static const fsel_get_prevdir_p fsel_get_prevdir_array[2] =
{
    fsel_dir_prev_dir, //“磁盘顺序”模式下切换上一个目录
    fsel_sortlist_prev_dir //“节目单”模式下切换上一个目录
};

static const fsel_get_total_p fsel_get_total_array[2] =
{
    fsel_get_all_dirs_total_files, //“磁盘顺序”模式下获取文件总数
    fsel_total_file_sortlist //“节目单”模式下获取文件总数
};

//获取下一个文件
bool fsel_get_nextfile(char *strfile)
{
    fsel_error = FSEL_NO_ERR;
    if (eh_vfs_mount == -1)
    {
        return 0;
    }
    return fsel_get_next_array[eh_fsel_type](strfile);
}

//获取上一个文件
bool fsel_get_prevfile(char *strfile)
{
    fsel_error = FSEL_NO_ERR;
    if (eh_vfs_mount == -1)
    {
        return 0;
    }
    return fsel_get_prev_array[eh_fsel_type](strfile);
}

//按指定序号获取文件
//num 在磁盘顺序模式下，为磁盘序号；在节目单顺序下为节目序号
bool fsel_get_byno(char *strfile, uint16 num)
{
    fsel_error = FSEL_NO_ERR;

    if (num == 0)
    {
        return FALSE;
    }
    return fsel_get_byno_array[eh_fsel_type](strfile, num);
}

//文件总数
uint16 fsel_get_total(void)
{
    fsel_error = FSEL_NO_ERR;

    if (eh_file_total_all != 0)
    {
        return eh_file_total_all;
    }
    else
    {
        return fsel_get_total_array[eh_fsel_type]();
    }
}

//当前文件的磁盘序号
uint16 fsel_get_fileno(void)
{
    uint16 file_no = eh_file_no_all;
    if (eh_fsel_type == FSEL_TYPE_LISTDIR)
    {
        file_no = eh_disk_file_no;
    }
    return file_no;
}

//重设播放模式
bool fsel_set_mode(uint8 mode)
{
    fsel_error = FSEL_NO_ERR;
    //check shuffle
    if (((uint8) mode & 0xf0) > 0)
    {
        if (eh_shuffle_sw == FALSE)
        {
            //之前不是shuffle,要初始化
            eh_shuffle_sw = TRUE;
            //if(eh_file_total_all == 0)
            //{
            eh_file_total_all = fsel_get_total();
            //}
            fsel_init_shuffle(eh_file_total_all);
            filtrate_shuffle_no = eh_file_no_all;
        }
    }
    else
    {
        eh_shuffle_sw = FALSE;
    }

    fsel_init_val.fsel_mode = (uint8) (mode & 0x0f);

    return TRUE;
}

//定位文件
bool fsel_set_location(void *location)
{
    fsel_error = FSEL_NO_ERR;

    if (location == NULL)
    {
        return FALSE;
    }

    return fsel_set_location_array[eh_fsel_type](location);
}

//获取当前选择文件的location
bool fsel_get_location(void *location)
{
    fsel_error = FSEL_NO_ERR;

    if ((eh_file_no == 0) || (location == NULL))
    {
        return FALSE;
    }

    return fsel_get_location_array[eh_fsel_type](location);
}

//获取下一个目录
bool fsel_get_nextdir(uint8 *strdir)
{
    fsel_error = FSEL_NO_ERR;

    return fsel_get_nextdir_array[eh_fsel_type](strdir);
}

//获取上一个目录
bool fsel_get_prevdir(uint8 *strdir)
{
    fsel_error = FSEL_NO_ERR;

    return fsel_get_prevdir_array[eh_fsel_type](strdir);
}

//获取当前文件路径信息
//location 存放路径信息指针
bool fsel_dir_get_location(file_location_t *location)
{
    vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);

    *(uint32*) &location->file_info.file_extend_info.file_ext = eh_cur_ext_name;
    //获取当前文件的路径信息
    if (FALSE == vfs_file_dir_offset(eh_vfs_mount, &location->dir_layer_info, \
        &location->file_info.file_extend_info.cluster_no, 0))
    {
        return FALSE;
    }

    location->disk = fsel_init_val.disk;
    location->dir_layer = eh_dir_layer;

    if (eh_file_total_all == 0)
    {
        //当前目录文件总数
        location->file_total = eh_file_total;
    }
    else
    {
        location->file_total = eh_file_total_all;
    }
    //文件序号
    location->file_num = eh_file_no_all;

    return TRUE;
}

