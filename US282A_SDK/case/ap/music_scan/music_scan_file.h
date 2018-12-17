/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：音频文件扫描相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __MUSIC_SCAN_H__
#define __MUSIC_SCAN_H__

#include "music_scan_cmd.h"
#include "dir_list.h"
#include "enhanced.h"
#include "case_type.h"
#include "music_common.h"

//每次写入vram的大小
#define VAM_BUF_SIZE    (DIR_INFO_SECTOR_SIZE)
typedef struct
{
    /* music scan线程上下文 */
    //pthread_param_t scan_thread_param;
    /* music scan和music ui交互的互斥量 */
    os_event_t * mutex;
    /* music scan线程索引 */
    int32 process_struct_index;
    /*线程结束标志*/
    int32 thread_end_flag;
} music_scan_param_t;

extern music_scan_param_t g_scan_param;
extern uint32 g_file_newest_time;
extern uint32 g_vm_file_newest_time;
extern bool g_vfs_changed;
extern uint8 *g_scan_thread_task_addr;

void * music_scan_open(void* param);
int32 music_scan_close(music_scan_param_t *p_music_scan);
void* music_scan_file(void* param);
void get_newest_file_time(int32 fs_id);



#endif //__MUSIC_SCAN_H__
