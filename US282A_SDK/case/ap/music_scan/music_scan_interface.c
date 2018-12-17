#include "music_scan_file.h"

music_scan_param_t g_scan_param;

//打开音频文件扫描,创建线程
void * music_scan_open(void* param)
{
    music_scan_param_t *p_scan_param = &g_scan_param;
    pthread_ext_param_t scan_thread_param;
    
    libc_memset(&g_scan_param, 0, sizeof(music_scan_param_t));

    /* 尽可能只使用互斥量 */
    libc_pthread_mutex_init(&p_scan_param->mutex);

    //初始化创建线程的参数
    scan_thread_param.pthread_param.start_rtn = music_scan_file;
    scan_thread_param.pthread_param.arg = param;
    //scan_thread_param.pthread_param.ptos = (void *) AP_FRONT_HIGH_STK_POS;
    g_scan_thread_task_addr = sys_malloc_large_data(AP_FRONT_HIGH_STK_SIZE);
    scan_thread_param.pthread_param.ptos = (void *)(g_scan_thread_task_addr + AP_FRONT_HIGH_STK_SIZE);
    scan_thread_param.stk_size = AP_FRONT_HIGH_STK_SIZE;

    libc_print("scan thread addr:", g_scan_thread_task_addr, 2);
    //禁止调度
    sys_os_sched_lock();

    //创建扫描磁盘线程并设置属性
    if (libc_pthread_create(&scan_thread_param, AP_FRONT_HIGH_PRIO, CREATE_NOT_MAIN_THREAD) < 0)
    {
        libc_free_process_struct(p_scan_param->process_struct_index);
        sys_os_sched_unlock();
        return NULL;
    }

    PRINT_INFO_STR("scan begin");

    sys_os_sched_unlock();

    return p_scan_param;
}

//关闭音频文件扫描(杀死线程)
int32 music_scan_close(music_scan_param_t *p_music_scan)
{
    //确认创建成功
    if (p_music_scan != NULL)
    {
        p_music_scan->thread_end_flag = TRUE;

        while (p_music_scan->thread_end_flag == TRUE)
        {
            /* 增加互斥量以保证得到的信息同步了scan线程 */
            libc_pthread_mutex_unlock(p_music_scan->mutex);

            sys_os_time_dly(1);

            /* 增加互斥量以保证得到的信息同步了scan线程 */
            libc_pthread_mutex_lock(p_music_scan->mutex);
        }
        libc_pthread_mutex_unlock(p_music_scan->mutex);
        libc_pthread_mutex_destroy(&p_music_scan->mutex);
    }

    sys_free_large_data(g_scan_thread_task_addr);
    return 0;
}
