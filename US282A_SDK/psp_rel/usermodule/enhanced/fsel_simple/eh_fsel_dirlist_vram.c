//依赖目录列表，节目单的文件操作
//执行时会占用PLIST_INFO_ADDR开始的0x1000空间(因为目录列表读到这个内存里)
//执行时会占用PLIST_INDEX_ADDR开始的0x1000空间(因为节目单读到这个内存里)
#include "eh_fsel_entry.h"
#include "dir_list.h"
#include "vm_fwsp_def.h"

//根据节目序号(第几个播出)得到磁盘序号。
//第src_num个播放的文件的磁盘中的序号(按照FAT数出来的序号)
//顺便：将目录的信息也读到了内存.
//注意：因为使用到了0x2e000开始的8k内存，
//  所以，本函数执行时，这块内存一定不能被其他程序使用!!!!
uint16 fsel_sortlsit_vram_read(uint16 src_num)
{
    uint8* read_ptr = (uint8 *) PLIST_INFO_ADDR;
    uint16* index_ptr = (uint16 *) PLIST_INDEX_ADDR;
    uint16 dst_num;
    //文件偏移

    //读出目录列表
    sys_vm_read(read_ptr, VM_PLIST_INFO_START0, 0x1000);
    //读节目单
//    sys_vm_read(index_ptr, VM_PLIST_INDEX_START0, 0x1000);

//    if (src_num > 0)
//    {
//        src_num--;
//    }
//    if (src_num < SUPPORT_MUSIC_FILE)
//    {
//        //文件序号
//        dst_num = index_ptr[src_num];
//    }
//    else
//    {
//        src_num++;
//        dst_num = src_num;
//    }  
    libc_print("######num change",0,0);

    return src_num;
}

//根据节目序号定位到文件
bool fsel_sortlist_get_byno_all(uint8 *strfile, uint16 num)
{
    //磁盘中的位置
    uint16 real_num;
    if (num > eh_file_total_all)
    {
        return FALSE;
    }

    //得到磁盘序号
    real_num = fsel_sortlsit_vram_read(num);

    return fsel_list_get_byno_all(strfile, real_num, num);
}

//获取当前文件路径信息
bool fsel_sortlist_get_location(file_location_t *location)
{
    //读vram
    fsel_sortlsit_vram_read(location->file_num);

    return fsel_list_get_location(location);
}

//设置当前路径信息
bool fsel_sortlist_set_location(file_location_t *location)
{
    //读vram
    fsel_sortlsit_vram_read(location->file_num);

    return fsel_list_set_location(location);
}

//获取当前文件路径信息
//location: 存放目录路径信息指针
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
bool fsel_list_get_location(file_location_t *location)
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
    //目录层级
    location->dir_layer = plist_info->cur_layer;
    //磁盘中文件总数
    if (eh_file_total_all == 0)
    {
        eh_file_total_all = fsel_total_file_alldir();
    }
    location->file_total = eh_file_total_all;
    //当前文件序号
    location->file_num = eh_file_no_all;

    return TRUE;
}

//设置当前路径信息
//location 存放路径信息指针
//前提：目录列表已经建立
//本函数执行时会占用PLIST_INFO_ADDR开始的0x1000空间
bool fsel_list_set_location(file_location_t *location)
{
    bool ret_val;

    //如果输入的location为无效值
    if ((location == NULL) || (*(uint32*) &location->file_info.file_extend_info.file_ext == 0))
    {
        //当前目录下的文件总数
        eh_file_total = fsel_total_file_curdir();
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
            eh_file_total = fsel_total_file_curdir();
            //eh_file_no 赋值
            eh_file_no = 0;
        }
    }

    if ((ret_val == FALSE) || (eh_file_no == 0))
    {
        //目录空或目录信息有误，回到根目录
        fsel_init_sysdata();

        eh_file_total = fsel_total_file_curdir();

        return FALSE;
    }

    //赋值更新eh_cur_ext_name
    libc_memcpy(&eh_cur_ext_name, location->file_info.file_extend_info.file_ext, 4);

    return TRUE;
}


