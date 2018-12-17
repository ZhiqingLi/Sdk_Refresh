#include "music_scan_file.h"
#include "case_independent.h"
#include "common_func.h"

//调试开关，scan的路径
//#define __SCAN_DEBUG__

//将扫描到的目录记入vram
//每个目录对应一个list_dir_info_t
//在list_dir_info_t的最前面是“头”(list_head_info_t)

static uint8 s_buf[536]; //前面12个字节放list_head_info_t，后面的空间是连续的list_dir_info_t结构
//要处理的list_dir_info_t的指针(指向s_buf的某个位置)
static list_dir_info_t *s_plist;

//头的指针(记录共有多少个文件)(指向s_buf的某个位置)
static list_head_info_t *s_phead;

//当前时间，用于控制250ms释放10ms时间片给前台应用
uint32 g_scan_cur_timer;

//文件总数
uint16 g_file_total = 0;

//当前目录下的文件序号
uint16 g_dir_file_no = 0;

//目录总数
uint16 g_dir_total = 0;

//目录层级
uint16 g_dir_layer = 0;

//存放文件系统id
int32 g_fs_mount_id = -1;

//文件层级
pdir_layer_t g_dir_layer_info;

//存放文件位置信息
//pfile_offset_t g_file_offset;

//是否扫描完成
bool g_scan_end = FALSE;

//是否出错 0x80为出错
uint8 g_scan_error = 0;

//线程指针
music_scan_param_t *g_pscan_handle;

//扫描的目录：0xff表示缺省方式，0x80表示录音目录， 0表示特殊目录1， 1表示特殊目录2
uint8 g_scan_dir_index;

//record cur disk flag;
uint8 g_scan_disk;

uint8 *g_scan_thread_task_addr;

static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";

//录音目录的名字
static uint8 s_rec_folder_short[8 + 1] =
{ 0 };
static uint8 s_rec_folder_long[16 + 1] =
{ 0 };

static const uint32 info_vram_addr[] =
{
    VM_PLIST_INFO_START0,
    VM_PLIST_INFO_START1,
    VM_PLIST_INFO_START2,
    VM_PLIST_INFO_START3,
    VM_PLIST_INFO_START4,
    VM_PLIST_INFO_START5,
    VM_PLIST_INFO_START6,
    VM_PLIST_INFO_START7
};

//注意：下面的这些定义，都要跟DIR_INFO_SECTOR_COUNT相对应
//即：如果这里增加一个元素，DIR_INFO_SECTOR_COUNT也要加1
//同理，也要跟DIR_INFO_SECTOR_SIZE相对应

//第一个512字节,当dir达到25个时开始写
//第二个512字节,当dir到达51个是开始写
//...
//第一个元素：(VAM_BUF_SIZE - sizeof(list_head_info_t)) / sizeof(list_dir_info_t)
static const uint8 dir_count[ARR_COUNT(info_vram_addr)] =
{ 25, 51, 77, 102, 128, 153, 179, SUPPORT_MUSIC_DIR };
//第一个512字节,当dir达到25个时开始写512字节。写后还有0个字节的信息没写进去。
//第二个512字节,当dir到达51个是开始写512字节。写后还有8个字节(的"尾巴")没写进去(留着写到下一个扇区)
//...
//ps.最后一个(第204个)list_dir_info_t能完整的存到vram(还多4个字节)
static const uint8 tail[ARR_COUNT(dir_count)] =
{ 0, 8, 16, 4, 12, 0, 8, 0 };

//排序用
//  第一个上台表演的是FAT里面的第几个文件，
//  第二个上台表演的是FAT里面的第几个文件，
//  第三个上台表演的是FAT里面的第几个文件，
//  ...
static const uint32 index_vram_addr[] =
{
    VM_PLIST_INDEX_START0,
    VM_PLIST_INDEX_START1,
    VM_PLIST_INDEX_START2,
    VM_PLIST_INDEX_START3,
    VM_PLIST_INDEX_START4,
    VM_PLIST_INDEX_START5,
    VM_PLIST_INDEX_START6,
    VM_PLIST_INDEX_START7
};

void music_scan_time_dly(uint32 dly)
{
    uint32 tmp_cur_timer = sys_get_ab_timer();

    if ((tmp_cur_timer - g_scan_cur_timer) >= 250)
    {
        g_scan_cur_timer = tmp_cur_timer;

        sys_os_time_dly(dly);
    }
    //sys_os_time_dly(dly);
}

#ifdef FILTER_RECORD_DIR

//过滤根目录下的RECORD目录
//dir_type: DIR_NEXT等
//返回值：如果当前目录RECORD, 且切换到另一个目录(按照dir_type切换)不成功，
//  才返回TRUE,
//  其他返回FALSE.
//  也就是说：成功跳过了RECORD目录，就返回FALSE；没有跳过，就返回TRUE
bool music_filtrate_dir(uint8 dir_type)
{
    uint8 dir_name_buffer[32];
    uint16 name_len;
    uint8 add_len, i, j;
    uint8 *buf;

    if (g_dir_layer != 0)
    {
        return FALSE;
    }

    name_len = vfs_get_name(g_fs_mount_id, dir_name_buffer, 16);

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
        if (s_rec_folder_short[j] != buf[i])
        {
            return FALSE;
        }
        i = i + add_len;
    }

    //过滤成功，找上/下一个文件夹返回
    if (0 != vfs_dir(g_fs_mount_id, dir_type, NULL, EXT_NAME_ALL_DIR))
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

#endif //#ifdef FILTER_RECORD_DIR

//装载物理介质和文件系统
int32 __section__(".init") music_scan_fs_init(uint8 disk_type, uint8 drv_mode)
{
    uint8 drv_type;
    uint8 *drv_name;
    int32 vfs_mount;

    g_scan_disk = disk_type;

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8*) card_drv_name;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
        drv_name = (uint8*) uhost_drv_name;
    }
    else
    {
        return -1;
    }

    //安装存储驱动
    if (sys_drv_install(drv_type, (uint32)drv_mode, drv_name) != 0)
    {
        return -1;
    }

    //挂载FS
    vfs_mount = sys_mount_fs(drv_type, disk_type, 0);

    if (vfs_mount == -1)
    {
        sys_drv_uninstall(drv_type);
    }

    return vfs_mount;
}

//卸载物理介质和文件系统
bool __section__(".exit") music_scan_exit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val = 0;

    uint8 drv_type;

    //降频
    adjust_freq_set_level(AP_FRONT_HIGH_PRIO, FREQ_NULL, FREQ_NULL);

    if (file_sys_id != -1)
    {
        ret_val = sys_unmount_fs(file_sys_id);
        file_sys_id = -1;
        if (ret_val == 0)
        {
            if (disk_type == DISK_H)
            {
                drv_type = DRV_GROUP_STG_CARD;
            }
            else if (disk_type == DISK_U)
            {
                drv_type = DRV_GROUP_STG_UHOST;
            }
            else
            {
                return FALSE;
            }

            ret_val = sys_drv_uninstall(drv_type);
        }
    }
    if (0 > ret_val)
    {
        return FALSE;
    }

    return TRUE;
}

//初始化文件选择器
//disk_flag: 磁盘盘符
bool __section__(".init") music_file_init(uint8 disk_flag)
{
    bool init_ret = FALSE;

    //文件选择器初始化
    g_fs_mount_id = music_scan_fs_init(disk_flag, 0);

    if (g_fs_mount_id != -1)
    {
        init_ret = TRUE;
        //默认进入根目录
        vfs_cd(g_fs_mount_id, CD_ROOT, 0);

        if (g_scan_dir_index != 0xff)
        {
            //查找目录 record
            if (g_scan_dir_index == 0x80)
            {
                //init_ret = TRUE;

                //record dir
                if (vfs_dir(g_fs_mount_id, DIR_HEAD, s_rec_folder_long, EXT_NAME_ALL_DIR) == FALSE)
                {
                    if (vfs_dir(g_fs_mount_id, DIR_HEAD, s_rec_folder_short, EXT_NAME_ALL_DIR) == FALSE)
                    {
                        init_ret = FALSE;
                    }
                }

            }
#if (SUPPORT_MUSIC_FOLDER == 1)
            else
            {
                //special folder
                //init_ret = vfs_dir(g_fs_mount_id, DIR_HEAD, g_music_folder_name_long[g_scan_dir_index], EXT_NAME_ALL_DIR);
                if(vfs_dir(g_fs_mount_id, DIR_HEAD, g_music_folder_name_long[g_scan_dir_index], EXT_NAME_ALL_DIR)
                        == FALSE)
                {
                    //init_ret = TRUE;
                    if(vfs_dir(g_fs_mount_id, DIR_HEAD, g_music_folder_name_short[g_scan_dir_index], EXT_NAME_ALL_DIR)
                            == FALSE)
                    {
                        init_ret = FALSE;
                    }
                }
            }
#endif
            if (init_ret == TRUE)
            {
                //进入子文件夹，获得该子文件夹的文件
                vfs_cd(g_fs_mount_id, CD_SUB, NULL);
                //g_dir_layer++;
            }
        }
    }
    return init_ret;
}

//将扫描到的目录写到vram
void __section__(".save") music_dirinfo_tovm(uint32 vm_offset, uint8 head_offset)
{
    sys_vm_write(s_buf, vm_offset, VAM_BUF_SIZE);
    //move to buffer header
    if (head_offset != 0)
    {
        libc_memcpy(s_buf, &s_buf[VAM_BUF_SIZE], (uint32 )head_offset);
    }
    s_plist = (list_dir_info_t*) (s_buf + head_offset);
}

//存放扫描到的目录
//如果时机到了，就存，返回TRUE
//如果时机还没到，但还能存，也返回TRUE
//如果已经存不下了，就返回FALSE
bool __section__(".save") music_store_dirinfo(void)
{
    uint8 cur_layer, i;
    pfile_offset_t g_file_offset;
    
    if (g_dir_total >= dir_count[ARR_COUNT(dir_count) - 1])
    {
        return FALSE;
    }

    //目录个数增加
    g_dir_total++;
    //在此处获取当前文件的路径层级信息存储
    s_plist->begin_index = (g_file_total + 1);
    vfs_file_dir_offset(g_fs_mount_id, &g_dir_layer_info, &g_file_offset, 0);
    //层级数(buffer的最后一个字节，存的是“当前层级”)
    cur_layer = g_dir_layer_info.buffer[MAX_DIR_LAYER * 8];
    s_plist->cur_layer = cur_layer;
    if (cur_layer > 0)
    {
        cur_layer--;
    }
    //父目录位置信息
    libc_memcpy(&s_plist->far_cluster, &g_dir_layer_info.buffer[cur_layer * 8], 8);
    //文件信息
    libc_memcpy(&s_plist->cur_cluster, &g_file_offset, sizeof(pfile_offset_t));

    //check if need store vram
    for (i = 0; i < ARR_COUNT(dir_count); i++)
    {
        if (g_dir_total == dir_count[i])
        {
            break;
        }
    }
    if (i == ARR_COUNT(dir_count))
    {
        s_plist++;
    }
    else
    {
        music_dirinfo_tovm((uint32) info_vram_addr[i], tail[i]);
    }
    return TRUE;
}


//名词解释：
//  磁盘中的序号：
//      按照FAT表得出来的序号。
//      假设：FAT表的第一项是一个TXT文件，第二项是一个mp3文件，
//      第三项是一个TXT，第四项是一个mp3文件，
//      那么第一个mp3文件的磁盘中的序号就是1，第二个mp3文件的磁盘中的序号就是2。
//
//  节目单：
//      记录在vram里面的一张表。
//      假设这张表的内容是：5, 3, 6...
//      表示第一个“上台表演”的是磁盘中的序号为5的文件；
//      第二个“上台表演”的是磁盘中的序号为3的文件；
//      第三个“上台表演”的是磁盘中的序号为6的文件；
//      ...
//
//  目录列表
//      记录在vram里面的一张表。
//      它有表头、表项；
//      每个表项对应一个目录；
//      包含了音频文件的目录才会进这张表。(“包含”指的是“直接包含”，
//          如果这个目录下没有音频文件，但它的子目录下有，
//          那么，这个目录不能进表)

//初始化节目单
void __section__(".save") music_save_init_list(void)
{
    uint16 i, j;
    //磁盘中的序号
    uint16 n = 1;
    uint16 *p_index_list; //磁盘中的序号都是2个字节
    //要存放的扇区个数
    uint16 wr_cnt;
    //每个扇区存放的歌曲个数
    uint16 cap_bum;

    //每个扇区存放的歌曲个数
    cap_bum = (VAM_BUF_SIZE / sizeof(uint16));
    //歌曲序号需要存放的扇区个数
    wr_cnt = g_file_total / cap_bum;
    //个数++
    if ((g_file_total % cap_bum) != 0)
    {
        wr_cnt++;
    }
    if (wr_cnt > ARR_COUNT(index_vram_addr))
    {
        //只有8个扇区
        wr_cnt = ARR_COUNT(index_vram_addr);
    }

    //每次写1 sector
    for (i = 0; i < wr_cnt; i++)
    {
        libc_memset(s_buf, 0, VAM_BUF_SIZE);
        p_index_list = (uint16 *) s_buf;
        //序号写为n
        for (j = 0; j < cap_bum; j++)
        {
            //just write 1~g_file_total
            if (n <= g_file_total)
            {
                *p_index_list = n;
                n++;
            }
            else
            {
                break;
            }
            p_index_list++;
        }
        sys_vm_write(s_buf, index_vram_addr[i], j * sizeof(uint16));
    }
}

//将列表头写入vram， 同时初始化“节目单”
void __section__(".save") music_save_header(void)
{
    uint8 i = 0;
    //最后一次写list_dir_info_t的时候，写不写list_head_info_t
    //  如果不写，这个变量就为0
    uint8 head_size = 0;
    //要写入几个字节
    uint32 wr_bytes;
    //写入的vram的地址
    uint32 wr_addr;
    //要写几个list_dir_info_t
    uint16 wr_dir;

    for (i = 0; i < ARR_COUNT(dir_count); i++)
    {
        if (g_dir_total <= dir_count[i])
        {
            break;
        }
    }
    if (i == ARR_COUNT(dir_count))
    {
        //不会进入这里，因为前面的代码(music_get_nextfile)已经保证了
        i = ARR_COUNT(dir_count) - 1;
    }

    //最后一笔需要写入
    if (g_dir_total == dir_count[i])
    {
        //这种情况下，已经在music_get_nextfile->music_store_dirinfo->
        //  music_dirinfo_tovm中写了vram了
        //  只是还有一点尾巴没有写
        wr_bytes = tail[i];
        if(i < (ARR_COUNT(dir_count) - 1))
        {
            wr_addr = info_vram_addr[i + 1];
        }
        else
        {
            //这时wr_bytes为tail[最后一个]，为0
            wr_addr = 0;//为避免警告
        }
    }
    else
    {
        wr_addr = info_vram_addr[i];

        if (i > 0)
        {
            i--;
            wr_dir = g_dir_total - dir_count[i];
        }
        else
        {
            head_size = sizeof(list_head_info_t);
            wr_dir = g_dir_total;
        }
        //要写入多少个字节
        wr_bytes = head_size + tail[i] + wr_dir * sizeof(list_dir_info_t);
    }

    if (wr_bytes != 0)
    {
        sys_vm_write(s_buf, wr_addr, wr_bytes);
    }

    //写“头”
    s_phead = (list_head_info_t *) s_buf;
    //dir num bigger than bufer save
    if (g_dir_total >= dir_count[0])
    {
        //把第一个扇区读出来
        wr_bytes = VAM_BUF_SIZE;
        sys_vm_read(s_buf, info_vram_addr[0], VAM_BUF_SIZE);
    }
    else
    {
        //不用读了，因为s_buf里面的东西就是第一个扇区的
        wr_bytes = g_dir_total * sizeof(list_dir_info_t) + sizeof(list_head_info_t);
    }
    s_phead->file_total = g_file_total;
    s_phead->dir_total = g_dir_total;
    s_phead->cur_disk = g_scan_disk;

    if(g_vm_file_newest_time < g_file_newest_time)
    {
        g_vfs_changed = TRUE;
        PRINT_INFO("music files changed.");
    }
    
    s_phead->file_newest_time = g_file_newest_time;
    
    sys_vm_write(s_buf, info_vram_addr[0], wr_bytes);
    //初始化节目单
    music_save_init_list();
}

#ifdef __SCAN_DEBUG__
static void _print_name(void)
{
    uint8 buf[16];

    vfs_get_name(g_fs_mount_id, buf, sizeof(buf) / 2 - 2);
    PRINT_DATA(buf, sizeof(buf));
}
#endif

//获取当前目录下的下个文件
//如果成功，返回TRUE;否则返回FALSE
static bool _dir_next(void)
{
    uint8 dir_type;
    if (g_dir_file_no == 0)
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
    if (vfs_dir(g_fs_mount_id, dir_type, NULL, MUSIC_BITMAP) != 0)
    {
        //找到文件则序号增加
        //sys_os_time_dly(1);
        music_scan_time_dly(1);
        g_dir_file_no++;

#ifdef __SCAN_DEBUG__
        PRINT_INFO("*");
        //_print_name();
#endif
        get_newest_file_time(g_fs_mount_id);

        return TRUE;
    }

    return FALSE;
}

//如果找到了，返回1
//如果文件系统出错了，返回-1
//如果vram出错了，返回-2
//如果没有找到，返回0
static int8 _sub(void)
{
    //进入子文件夹，获得该子文件夹的文件总数
    if (FALSE == vfs_cd(g_fs_mount_id, CD_SUB, NULL))
    {
        //能DIR_HEAD，却不能CD_SUB
        g_scan_error = 0x80;
        return -1;
    }

#ifdef __SCAN_DEBUG__
    PRINT_INFO(">");
    _print_name();
#endif

    //目录层级增加
    g_dir_layer++;
    //当前目录下的文件序号归零
    g_dir_file_no = 0;
    // 检查当前目录是否有合适的文件
    if (_dir_next() == FALSE)
    {
        return 0;
    }

    if(music_store_dirinfo() == TRUE)
    {
        return 1;
    }

    return -2;
}

//遍历目录树
//1.如果目录下有音乐文件，就记入vram
//2.如果没有，它就不会记录(即使它的子孙目录有)
//
//目录最深MAX_DIR_LAYER
//最多能记录dir_count[ARR_COUNT(dir_count) - 1]个目录
//返回值：无法继续时，就返回FALSE
bool music_get_nextfile(void)
{
#ifdef FILTER_RECORD_DIR
    bool dir_ret;
#endif
    bool dir_val;
    int8 n;
    //在当前目录下找
    bool cur_dir_flag;

    //在当前目录向下找文件
    dir_val = _dir_next();
    if(dir_val == TRUE)
    {
        return TRUE;
    }

    if(g_scan_dir_index != 0xff)
    {
        //特殊目录
        //不扫描子目录
        return FALSE;
    }

    cur_dir_flag = TRUE;

    for(;;)
    {
        if (g_pscan_handle->thread_end_flag == TRUE)
        {
            return FALSE;
        }

        //sys_os_time_dly(1);
        music_scan_time_dly(1);

        if(cur_dir_flag == TRUE)
        {
            if (g_dir_layer >= (MAX_DIR_LAYER - 1)) //如果当前是8级目录就不找子文件夹
            {
                cur_dir_flag = FALSE;
                continue;
            }

#ifdef FILTER_RECORD_DIR
            //寻找该目录的第一个子文件夹
            dir_ret = (bool)vfs_dir(g_fs_mount_id, DIR_HEAD, NULL, EXT_NAME_ALL_DIR);
            if ((dir_ret != 0) && (music_filtrate_dir(DIR_NEXT) == FALSE))
#else
            if (vfs_dir(g_fs_mount_id, DIR_HEAD, NULL, EXT_NAME_ALL_DIR) != 0)
#endif
            {

                n = _sub();
                if(n == 0)
                {
                    //没找到
                    //检查子目录
                    continue;
                }

                if(n < 0)
                {
                    return FALSE;
                }

                return TRUE;
            }
            cur_dir_flag = FALSE;
            continue;
        }

        //如果没有子文件夹，则向上跳一层，寻找与当前目录平级的下一个目录（平级寻找）
        //如果当前是根目录则返回
        if (g_dir_layer == 0)
        {
            return FALSE;
        }
        //跳到父目录
        if (FALSE == vfs_cd(g_fs_mount_id, CD_BACK, NULL))
        {
            //异常出错
            g_scan_error = 0x80;
            return FALSE;
        }

#ifdef __SCAN_DEBUG__
        PRINT_INFO("<");
#endif

        //目录层级减
        g_dir_layer--;
#ifdef FILTER_RECORD_DIR
        dir_ret = (bool)vfs_dir(g_fs_mount_id, DIR_NEXT, NULL, EXT_NAME_ALL_DIR);
        if ((dir_ret == 0) || (TRUE == music_filtrate_dir(DIR_NEXT)))
#else
        if (vfs_dir(g_fs_mount_id, DIR_NEXT, NULL, EXT_NAME_ALL_DIR) == 0)
#endif
        {
            continue;
        }

#ifdef __SCAN_DEBUG__
        PRINT_INFO("-");
        //_print_name();
#endif

        n = _sub();
        if(n == 0)
        {
            //没找到
            //检查子目录
            cur_dir_flag = TRUE;
            continue;
        }

        if(n < 0)
        {
            return FALSE;
        }

        return TRUE;
    }
}

bool __section__(".init") music_scan_init(void *param)
{
    //初始化结果
    bool init_result;

    app_param_e scan_mode;

    uint8 parm_val;

    //升频
    adjust_freq_set_level(AP_FRONT_HIGH_PRIO, FREQ_LEVEL11, FREQ_NULL);

    //disk flag
    parm_val = *(uint8 *) param;
    //scan recope judge
    scan_mode = *((uint8 *) param + 1);
    //scan mode selcet

    switch (scan_mode)
    {
#if (SUPPORT_MUSIC_FOLDER == 1)
        //sepcial folder1
        case PARAM_MUSIC_FOLDER1:
        g_scan_dir_index = 0;
        break;
        //sepcial folder2
        case PARAM_MUSIC_FOLDER2:
        g_scan_dir_index = 1;
        break;
#endif
        //record folder
        case PARAM_RECORD_CPLAY:
        case PARAM_RECORD_UPLAY:
        g_scan_dir_index = 0x80;
        break;
        //scan all disk
        default:
        g_scan_dir_index = 0xff;
        break;
    }

    s_plist = (list_dir_info_t *) (s_buf + sizeof(list_head_info_t));

    sys_vm_read(&g_vm_file_newest_time, info_vram_addr[0]+(((uint32)&s_phead->file_newest_time) - (uint32)s_phead), 4);
    //清空buffer
    libc_memset(s_buf, 0, sizeof(s_buf));

    com_load_share_data(COM_SHARE_DATA_REC_FOLDER, s_rec_folder_short,
            sizeof(s_rec_folder_short) - 1, 0);
    com_load_share_data(COM_SHARE_DATA_REC_FOLDER, s_rec_folder_long,
            sizeof(s_rec_folder_long) - 1, 1);

    //初始化文件选择器
    init_result = music_file_init(parm_val);

    //清空出错标志
    g_scan_error = 0;
    g_scan_end = FALSE;
    //g_pscan_handle->thread_end_flag = FALSE;

    return init_result;
}

//文件扫描的线程函数
void* music_scan_file(void* param)
{
    uint8 parm_val;
    uint8 scan_mode;
    bool init_result;

    //disk flag
    parm_val = *(uint8 *) param;
    scan_mode = *((uint8 *) param + 1);

    init_result = music_scan_init(param);
    while (1)
    {
        /* 判断所有需要这个线程运行起来的功能标志位 */
        libc_pthread_mutex_lock(g_pscan_handle->mutex);
        if (g_pscan_handle->thread_end_flag == TRUE)
        {
            goto loop_exit;
        }

        //初始化失败则不再扫描
        if (init_result == FALSE)
        {
            g_scan_error = 0x80;
            g_scan_end = TRUE;
        }

        if (g_scan_end == TRUE)
        {
            goto loop_continue;
        }

        //下面都是g_scan_end == FALSE的情况

        //扫描磁盘
        if (music_get_nextfile() == FALSE)
        {
            //扫描完成保存总数
            g_scan_end = TRUE;
            music_save_header();
//#ifdef __SCAN_DEBUG__
//          PRINT_INFO_INT("scan over:", g_file_total);
//#endif
            goto loop_continue;
        }

        if ((g_file_total == 0) && (g_dir_layer == 0))
        {
            //当前是根目录且是第一个文件则保存
            music_store_dirinfo();
        }
        g_file_total++;

        if ((g_dir_total > dir_count[ARR_COUNT(dir_count) - 1])
                //这个条件不会成立，前面的代码(music_get_nextfile)已经保证了
                || (g_file_total >= SUPPORT_MUSIC_FILE))
        {
            g_scan_end = TRUE;
            //扫描完成保存总数
            music_save_header();
        }

        loop_continue:
        libc_pthread_mutex_unlock(g_pscan_handle->mutex);
        //挂起10ms 多任务交互
        //sys_os_time_dly(2);
        music_scan_time_dly(2);
    }
    loop_exit:
    //打印扫描到的歌曲总数
    PRINT_INFO_INT("scan over:", g_file_total);
    //文件选择器退出
    music_scan_exit(parm_val, g_fs_mount_id);
    g_pscan_handle->thread_end_flag = FALSE;

    libc_pthread_mutex_unlock(g_pscan_handle->mutex);

    /*注意：libc_pthread_exit无法返回*/
    libc_pthread_exit();
    return 0;
}

//跟客户端的接口函数
int __section__(".cmd") music_scan_cmd(void* ms_handle, music_scan_cmd_t cmd, unsigned int param)
{
    int32 ret = 0;

    g_pscan_handle = &g_scan_param;

    //确保mutex已经初始化
    if (cmd == MUSIC_SCAN_OPEN)
    {
        if (music_scan_open((void*) param) == NULL)
        {
            *((music_scan_param_t **) ms_handle) = NULL;
            ret = -1;
        }
        else
        {
            *((music_scan_param_t **) ms_handle) = g_pscan_handle;
        }
        goto music_scan_cmd_exit;
    }
    /* 增加互斥量以保证得到的信息同步了scan线程 */
    libc_pthread_mutex_lock(g_pscan_handle->mutex);
    switch (cmd)
    {
        case MUSIC_SCAN_GET_STATUS:
        //查看是否扫描完成
        *((uint8*) param) = (g_scan_end | g_scan_error | (g_vfs_changed << 1));
        if (g_scan_error == 0x80)
        {
            //清空buffer
            libc_memset(s_buf, 0, sizeof(s_buf));
        }
        break;

        case MUSIC_SCAN_CLOSE:
        ret = music_scan_close(ms_handle);
        /*mutex destroy, can not unlock*/
        goto music_scan_cmd_exit;
        default:
        ret = -1;
        break;
    }

    libc_pthread_mutex_unlock(g_pscan_handle->mutex);

    music_scan_cmd_exit:
    return ret;
}
