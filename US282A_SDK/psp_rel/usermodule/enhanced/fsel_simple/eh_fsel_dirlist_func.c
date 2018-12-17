//依赖目录列表的文件操作
//执行时会占用PLIST_INFO_ADDR开始的0x1000空间(因为目录列表读到这个内存里)
#include "eh_fsel_entry.h"
#include "dir_list.h"

//统计当前目录下文件的总数
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
uint16 fsel_total_file_curdir(void)
{
    uint16 total = 0;

    list_dir_info_t* next_entry_ptr;

    next_entry_ptr = (plist_info + 1);

    if (eh_dir_no == plist_head->dir_total)
    {
        total = plist_head->file_total - (plist_info->begin_index - 1);
    }
    else
    {
        //获取列表中文件总数
        total = (next_entry_ptr->begin_index - plist_info->begin_index);
    }

    return total;
}

//统计磁盘文件的总数，顺便进行shuffle操作
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
uint16 fsel_total_file_alldir(void)
{
    uint16 total = 0;
    //磁盘文件总数
    total = plist_head->file_total;
    //shuffle模式需要初始化
    if ((TRUE == eh_shuffle_sw) && (total > 0))
    {
        fsel_init_shuffle(total);
    }
    return total;
}

//定位最后一个目录
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
bool fsel_last_list_entry(void)
{
    uint16 i = 0;

    list_dir_info_t* cur_entry_info = plist_info;

    //定位到最后一个目录位置
    eh_dir_no = plist_head->dir_total;

    //指向最后一个列表项
    plist_info = (list_dir_info_t*) (plist_start_addr + sizeof(list_head_info_t));

    while (i < (eh_dir_no - 1))
    {
        plist_info++;
        i++;
    }

    //当前目录文件总数
    eh_file_total = fsel_total_file_curdir();

    eh_file_no = 0;

    vfs_jump_to_direntry(eh_vfs_mount, 0, plist_info, cur_entry_info);

    return TRUE;
}



//磁盘序号是cur_num的文件，在节目单上是第几个
//
//注意：因为这里用到了0x2e000开始的0x2000空间(此空间被解码器所用)，
//所以，只能在解码器不工作的时候，才能调用。
//(比如切歌的时候，因为先STOP了，所以，就可以调用)
//
uint16 fsel_check_index_num(uint16 cur_num)
{
    uint16 list_file_total, i;
    list_head_info_t* head_ptr = (list_head_info_t*) PLIST_INFO_ADDR;
    uint16* index_ptr = (uint16*) PLIST_INDEX_ADDR;

    fsel_sortlsit_vram_read(0);
    list_file_total = head_ptr->file_total;

    for (i = 1; i <= list_file_total; i++)
    {
        if (i > SUPPORT_MUSIC_FILE)
        {
            return cur_num;
        }

        if (((*index_ptr) == cur_num))
        {
            break;
        }
        index_ptr++;
    }
    return cur_num;
}

//设置文件播放的顺序
//type: 播放顺序(磁盘顺序, 节目单)
//cur_num: 磁盘序号
//注意：
//  因为调用了fsel_check_index_num的原因，
//  所以，本函数执行时会占用0x2e000开始的0x2000空间
bool fsel_set_seltype(fsel_type_e type, uint16 cur_num)
{
    fsel_error = FSEL_NO_ERR;

    //如果输入参数出错,则默认进入普通目录模式
    if (type >= FSEL_TYPE_MAX)
    {
        type = FSEL_TYPE_COMMONDIR;
    }
    if (type != eh_fsel_type)
    {
        eh_fsel_type = type;
    }

    //设置为节目单模式
    if (eh_fsel_type == FSEL_TYPE_LISTDIR)
    {
        eh_disk_file_no = cur_num;

        eh_file_no_all = fsel_check_index_num(cur_num);

        //if it has alredy update to file list
        /*if(cur_num!= eh_file_no_all)
        {
            //have already set
            //no need to check again
            eh_check_list_update = TRUE;
        }*/
        //表头指针
        plist_head = (list_head_info_t *) plist_start_addr;
        //列表项指针
        plist_info = (list_dir_info_t *) (plist_start_addr + sizeof(list_head_info_t));

        eh_dir_no = 1;
        //no scan correctly
        if (plist_info->begin_index == 0)
        {
            //除非没有文件(但没有文件也不会到这里来)
            //出错了
            eh_fsel_type = FSEL_TYPE_COMMONDIR;
            eh_file_no_all = cur_num;

            return FALSE;
        }

        //更新total值(之前的可能为第一个目录的文件总数)
        eh_file_total_all = plist_head->file_total;
        //check file index valid
        if (eh_file_no_all > eh_file_total_all)
        {
            if (plist_head->dir_total == 1)
            {
                eh_file_no_all = eh_file_total_all;
            }
            else
            {
                eh_file_no_all = 0;
                //默认进入第一个目录项
                vfs_jump_to_direntry(eh_vfs_mount, 1, plist_info, plist_info);
            }
            eh_disk_file_no = eh_file_no_all;

            return TRUE;
        }

        if (cur_num != plist_info->begin_index)
        {
            //匹配当前是哪个文件夹
            while (1)
            {
                if ((cur_num >= plist_info->begin_index) && (cur_num < (plist_info + 1)->begin_index))
                {
                    break;
                }
                else if (eh_dir_no == plist_head->dir_total)
                {
                    //最后一个
                    break;
                }
                else
                {
                    eh_dir_no++;
                    plist_info++;
                }
            }
        }
    }

    return TRUE;
}


//定位到下一个目录的第一个文件
//strdir：存放文件名的指针
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
bool fsel_list_next_dir(uint8 *strdir)
{
    bool jump_val;

    list_dir_info_t* cur_dirinfo = plist_info;

    if (plist_head->dir_total == 1)
    {
        //gap_set = (eh_disk_file_no - plist_info->begin_index);
        jump_val = TRUE; //do nothing
    }
    //到达最后一个目录
    else if (eh_dir_no == plist_head->dir_total)
    {
        //切换回到第一个目录
        eh_dir_no = 1;
        //指针指向第一个列表项
        plist_info = (list_dir_info_t*) (plist_start_addr + sizeof(list_head_info_t));
        jump_val = vfs_jump_to_direntry(eh_vfs_mount, PREV_FLAG, plist_info, cur_dirinfo);
    }
    else
    {
        plist_info++;

        eh_dir_no++;
        //切换到下一个目录
        jump_val = vfs_jump_to_direntry(eh_vfs_mount, 1, plist_info, cur_dirinfo);
    }
    //当前目录的第一个文件
    if (jump_val == TRUE)
    {
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, 0, plist_info, fsel_init_val.file_type_bit);
    }
    if (eh_cur_ext_name != 0)
    {
        if (strdir != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strdir = eh_cur_ext_name;
        }
        eh_file_no = 1;
        eh_file_total = fsel_total_file_curdir();
        eh_disk_file_no = (eh_file_no + plist_info->begin_index - 1);
        eh_file_no_all = fsel_check_index_num(eh_disk_file_no);
    }
    else
    {
        jump_val = FALSE;
    }

    return jump_val;
}

//定位到上一个目录的上一个文件
//strdir：存放文件名的指针
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
bool fsel_list_prev_dir(uint8 *strdir)
{
    bool jump_retval;

    list_dir_info_t* cur_dir_info = plist_info;

    //到达第一个目录
    if (eh_dir_no == 1)
    {
        jump_retval = fsel_last_list_entry();
    }
    else
    {
        //切换到上一个目录
        eh_dir_no--;
        plist_info--;
        jump_retval = vfs_jump_to_direntry(eh_vfs_mount, PREV_FLAG | 0x01, plist_info, cur_dir_info);
    }
    //当前目录的第一个文件后缀名
    if (jump_retval == TRUE)
    {
        //进入该目录,查找该目录最后一个文件
        eh_cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount, PREV_FLAG, plist_info, fsel_init_val.file_type_bit);
    }
    if (eh_cur_ext_name != 0)
    {
        if (strdir != NULL)
        {
            vfs_get_name(eh_vfs_mount, &eh_cur_ext_name, 0);
            *(uint32*) strdir = eh_cur_ext_name;
        }
        eh_file_total = fsel_total_file_curdir();
        eh_file_no = eh_file_total;

        eh_disk_file_no = (eh_file_no + plist_info->begin_index - 1);

        eh_file_no_all = fsel_check_index_num(eh_disk_file_no);
    }
    else
    {
        jump_retval = FALSE;
    }
    return jump_retval;
}

