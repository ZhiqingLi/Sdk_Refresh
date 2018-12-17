/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：kernel 接口定义
 * 作者：liminxian
 ********************************************************************************/


#ifndef __KERNEL_DEFINE_H__
#define __KERNEL_DEFINE_H__
#include <attr.h>
#include <typeext.h>
#include "sys_board_def.h"
#include <mem_manager.h>

typedef enum
{
    SYS_BASE_NOR,
    SYS_BASE_CARD,    
}base_type_stg;


typedef enum
{
    SYS_INFO_DISK_CONFIG,
    SYS_INFO_SDREAD_CALLBACK,
    SYS_INFO_PAGEMISS_CALLBACK,    
    SYS_HIGH_PW_MODE_SWITCH,
    SYS_PRINT_ONOFF,
    SYS_FREQ_LOCK_SWITCH,
    SYS_CPU_WORK_MONITOR,
    SYS_HOSC_PARAMETER,
    SYS_RESET_TIMER,
    SYS_PRINT_EXCEPTION_INFO,
    SYS_AVOID_LINEIN_NOISE,
    SYS_MEM_USE_INFO,
    SYS_SET_MEM_USE_INFO,
    SYS_CLEAR_TEMP_POOL,
    SYS_DISABLE_USE_TEMP_POOL,
    SYS_ENABLE_USE_TEMP_POOL,
    SYS_DISABLE_USE_SEND_BUFFER,
    SYS_READ_PAGEMISS_CALLBACK,
}sys_info_type_e;

typedef enum
{
    EXECE_AP = 0,
    LOAD_CODEC,
    LOAD_MMM,
    LOAD_DSP_CODEC,
    FREE_AP,
    FREE_CODEC,
    FREE_MMM,
    FREE_DSP_CODEC,
    BANK_FLUSH,
    DETECT_DISK,
    DRV_INSTALL,
    DRV_UNINSTALL,
    GET_DRV_INSTALL_INFO,
    
    SD_FOPEN,
    SD_FCLOSE,
    SD_FSEEK,
    SD_FTELL,
    SD_FREAD,
    
    VM_READ,
    VM_WRITE,
    
    REQUEST_IRQ,
    FREE_IRQ,
    
    MEM_MALLOC,
    MEM_FREE,
    
    MQ_SEND,
    MQ_RECEIVE,
    MQ_FLUSH,
    
    GET_FW_INFO,
    GET_AB_TIMER,
    
    MDELAY,
    UDELAY,
    ACTIONS_SLEEP,
    USLEEP,
    
    SET_IRQ_TIMER1,
    DEL_IRQ_TIMER1,
    
    
    SET_TIME,
    GET_TIME,
    SET_DATE,
    GET_DATE,
    SET_ALARM_TIME,
    GET_ALARM_TIME,
    
    MOUNT_FS,
    UNMOUNT_FS,
    GET_FAT_TYPE_AFTER_MOUNT,
    FORMAT_DISK,
    
    LOCAL_IRQ_SAVE,
    LOCAL_IRQ_RESTORE,
    
    OS_TASK_RESUME,
    OS_TASK_SUSPEND,
    OS_TIME_DLY,
    OS_TIME_DLY_RESUME,
    OS_SCHED_LOCK,
    OS_SCHED_UNLOCK,
    
    RANDOM,
    SET_SYS_INFO,
    GET_SYS_INFO,
    
    REQUEST_CLKADJUST,
    FREE_CLKADJUST,
    ADJUST_CLK,
    RESPOND_DSP_CMD,
    REQUEST_DSP_IRQ,
    SET_DRV_SETTING,
    
    BASE_SET_INFO,
    MQ_TRAVERSE,
    DSP_PRINT,
    IRQ_PRINT,
    RECORD_ABTIME_US,
    
    SHARE_QUERY_CREAT,
    SHARE_QUERY_DESTROY,
    SHARE_QUERY_READ,
    SHARE_QUERY_UPDATE,
    
    SHM_CTRL_CREAT,
    SHM_CTRL_DESTROY,
    SHM_CTRL_MOUNT,
    
    PTHREAD_CREATE_FIX,
    SYSTEM_CPU_PRINT,
    SYSTEM_DMA_PRINT,

    GET_TIME_OF_DAY,

    PRINT_EXCEPTION_INFO,
    MEM_POOL_CREATE,
    MEM_POOL_DESTROY,
    MEM_SET_LOCK_INFO,
    MEM_ENABLE_USE,
	MEM_DISABLE_USE,

    LOAD_DSP_FADE,
    FREE_DSP_FADE,
}kernel_cmd_e;

/*
KERNEL向上层提供统一的接口，即只提供一个API，并以ID的方式区分
*/
extern void* sys_op_entry(void *param1, void *param2, void *param3, kernel_cmd_e cmd_type) __FAR__;

typedef void* (*sys_op_i)(void*, void*, void*);

struct sys_operations {
    sys_op_i exece_ap;
    sys_op_i load_codec;
    sys_op_i load_mmm;
    sys_op_i load_dsp_codec;  
    sys_op_i free_ap;
    sys_op_i free_codec;
    sys_op_i free_mmm;
    sys_op_i free_dsp_codec;
    sys_op_i bank_flush;
    sys_op_i detect_disk;
    sys_op_i drv_install;
    sys_op_i drv_uninstall;
    sys_op_i get_drv_install_info;
    
    sys_op_i sd_fopen;
    sys_op_i sd_fclose;
    sys_op_i sd_fseek;
    sys_op_i sd_ftell;
    sys_op_i sd_fread;
    
    sys_op_i vm_read;
    sys_op_i vm_write;
    
    sys_op_i request_irq;
    sys_op_i free_irq;
    
    sys_op_i malloc;
    sys_op_i free;
    
    sys_op_i mq_send;
    sys_op_i mq_receive;
    sys_op_i mq_flush;
    
    sys_op_i get_fw_info;
    sys_op_i get_ab_timer;
    
    sys_op_i mdelay;
    sys_op_i udelay;
    sys_op_i sleep;
    sys_op_i usleep;
    
    sys_op_i set_irq_timer1;
    sys_op_i del_irq_timer1;
    
    
    sys_op_i set_time;
    sys_op_i get_time;
    sys_op_i set_date;
    sys_op_i get_date;
    sys_op_i set_alarm_time;
    sys_op_i get_alarm_time;
    
    sys_op_i mount_fs;
    sys_op_i unmount_fs;
    sys_op_i get_fat_type_after_mount;
    sys_op_i format_disk;
    
    sys_op_i local_irq_save;
    sys_op_i local_irq_restore;
    
    sys_op_i os_task_resume;
    sys_op_i os_task_suspend;
    sys_op_i os_time_dly;
    sys_op_i os_time_dly_resume;
    sys_op_i os_sched_lock;
    sys_op_i os_sched_unlock;
    
    sys_op_i random;
    sys_op_i set_sys_info;
    sys_op_i get_sys_info;
    
    sys_op_i request_clkadjust;
    sys_op_i free_clkadjust;
    sys_op_i adjust_clk;
    sys_op_i respond_dsp_cmd;
    sys_op_i request_dsp_irq;
    
    sys_op_i set_drv_setting;
    sys_op_i base_set_info;
    sys_op_i mq_traverse;
    sys_op_i dsp_print;
    sys_op_i irq_print;
    sys_op_i record_abtime_us;
    
    sys_op_i share_query_creat;
    sys_op_i share_query_destroy;
    sys_op_i share_query_read;
    sys_op_i share_query_update;
    
    sys_op_i shm_ctrl_creat;
    sys_op_i shm_ctrl_destroy;
    sys_op_i shm_ctrl_mount;
    
    sys_op_i pthread_create_fix;
    
    sys_op_i system_cpu_print;
    sys_op_i system_dma_print;
    sys_op_i get_time_of_day;
    sys_op_i print_exception_info;
    sys_op_i mem_pool_create;
    sys_op_i mem_pool_destroy;
    sys_op_i mem_set_lock_info;
    sys_op_i enable_mem_use;
    sys_op_i disable_mem_use;

    sys_op_i load_dsp_fade; 
    sys_op_i free_dsp_fade; 	
};

/*
int sys_exece_ap(char *name,uint32 type,void *argc)
该接口用于加载AP应用
name：应用的文件名XXX.AP
type：应用类型
argc：传入应用的参数，调用者子定义
返回值int：0-成功，非0失败
note:bank代码，禁止在中断调用
*/
#define sys_exece_ap(name,type,argc)     \
(int)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)(argc), EXECE_AP) 

/*
int sys_load_codec(char *name, uint32 type)
该接口用于加载预处理库
name：预处理库的文件名XXX.al
type：预处理库类型
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_load_codec(name,type)     \
(int)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)0, LOAD_CODEC)

/*
int sys_load_mmm(char *name, uint32 type)
该接口用于加载中间件
name：中间件的文件名XXX.al
type：中间件类型
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_load_mmm(name,type)      \
(int)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)0, LOAD_MMM)

/*
int sys_load_dsp_codec(char *name, uint32 type)
该接口用于加载DSP库
name：DSP库的文件名XXX.dsp
type：DSP库类型

返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_load_dsp_codec(name,type)     \
(int)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)0, LOAD_DSP_CODEC)

/*
int sys_load_dsp_fade(char *name, uint32 type, uint32 address)
该接口用于加载DSP库
name：DSP库的文件名XXX.dsp
type：DSP库类型
address:获得dsp库text0_addr

返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_load_dsp_fade(name,type,address)     \
(int)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)(uint32)(address), LOAD_DSP_FADE)


/*
void sys_free_ap(uint32 type)
该接口用于卸载应用
type：应用类型
返回值void
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_ap(type)        \
(void)sys_op_entry((void*)(uint32)(type), (void*)0, (void*)0, FREE_AP)

/*
void sys_free_codec(uint32 type)
该接口用于卸载预处理库
type：预处理库类型
返回值void
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_codec(type)     \
(void)sys_op_entry((void*)(uint32)(type), (void*)0, (void*)0, FREE_CODEC)

/*
void sys_free_mmm(uint32 type)
该接口用于卸载中间件
type：中间件类型
返回值void
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_mmm(type)     \
(void)sys_op_entry((void*)(uint32)(type), (void*)0, (void*)0, FREE_MMM)

/*
void sys_free_dsp_codec(uint32 type)
该接口用于卸载DSP库
type：DSP库类型
返回值void
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_dsp_codec(name,type)    \
(void)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)0, FREE_DSP_CODEC)

/*
sys_free_dsp_fade(name,type) 
该接口用于卸载DSP库
type：DSP库类型为1
返回值void
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_dsp_fade(name,type)    \
(void)sys_op_entry((void*)(name), (void*)(uint32)(type), (void*)0, FREE_DSP_FADE)


/*
int sys_bank_flush(uint8 page_index)
该接口用于清空page页的TLB
page_index：page页
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_bank_flush(page_index)  \
(int)sys_op_entry((void*)(uint32)(uint8)(page_index), (void*)0, (void*)0, BANK_FLUSH)

/*
int sys_detect_disk(uint8 drv_type)
该接口检测外部存储设备 (card,u盘)是否插入
drv_type：设备类型
返回值int：0-插入，非0-没有插入
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_detect_disk(drv_type)   \
(int)sys_op_entry((void*)(uint32)(uint8)(drv_type), (void*)0, (void*)0, DETECT_DISK)

/*
int drv_install(uint8 drv_type, void *drv_para, char* drv_name)
该接口用于加载外设设备驱动
drv_type：设备类型
drv_para：驱动传参，调用者定义,对应驱动init入口的第3个参数
drv_name：驱动文件名，xxx.drv
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_drv_install(drv_type,drv_para,drv_name)   \
(int)sys_op_entry((void*)(uint32)(uint8)(drv_type), (void*)(drv_para), (void*)(drv_name), DRV_INSTALL)

/*
int sys_drv_uninstall(uint8 drv_type)
该接口用于卸载外设设备驱动
drv_type：设备类型
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_drv_uninstall(drv_type)   \
(int)sys_op_entry((void*)(uint32)(uint8)(drv_type), (void*)0, (void*)0, DRV_UNINSTALL)

/*
int sys_get_drv_install_info(uint8 drv_type)
该接口获取设备驱动信息
drv_type：设备类型
返回值int：-1-失败，其他值--驱动信息
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_get_drv_install_info(drv_type) \
(int)sys_op_entry((void*)(uint32)(uint8)(drv_type), (void*)0, (void*)0, GET_DRV_INSTALL_INFO)

/*
sd_file_t *sys_sd_fopen (char *name)
该接口打开sd区文件
name：sd区文件xxx.yyy
返回值文件句柄：NULL--为失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_sd_fopen(name)             \
(void*)sys_op_entry((void*)(name), (void*)0, (void*)0, SD_FOPEN)

/*
sd_file_t *sys_sd_fopen (char *name, uint32 part_id)
该接口打开sd区文件
name：sd区文件xxx.yyy
返回值文件句柄：NULL--为失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_sd_fopen_ext(name, part_id)             \
(void*)sys_op_entry((void*)(name), (void*)(part_id), (void*)0, SD_FOPEN)


/*
int sys_sd_fclose(sd_file_t *fp)
该接口用于关闭已打开的sd区文件
fp：文件句柄
返回值int：0-成功，非0失败
*/
#define sys_sd_fclose(fp)            \
(int)sys_op_entry((void*)(fp), (void*)0, (void*)0, SD_FCLOSE)

/*
int sys_sd_fseek(sd_file_t *fp, uint8 type, int32 offset)
该接口用于定位文件读位置
fp：文件句柄
type：seek类型，SEEK_SET-从文件头，SEEK_CUR--当前位置，SEEK_END--文件末尾
offset：偏移位置，可为负数
返回值int：0-成功，非0失败
*/
#define sys_sd_fseek(fp,type,offset)   \
(int)sys_op_entry((void*)(fp), (void*)(uint32)(uint8)(type), (void*)(offset), SD_FSEEK)

/*
uint32 sys_sd_ftell(sd_file_t *fp)
该接口用于获取文件当前读位置
fp：文件句柄
返回值uint32:文件读指针
*/
#define sys_sd_ftell(fp)           \
(uint32)sys_op_entry((void*)(fp), (void*)0, (void*)0, SD_FTELL) 

/*
int32 sys_sd_fread(sd_file_t *fp, void *buf, uint32 len)
该接口用于读取文件内容
fp：文件句柄
buffer：存放的buffer
len：读取的长度(字节单位)
返回值int32：真实读取的长度，-1--失败
*/
#define sys_sd_fread(fp,buf,len)         \
(int32)sys_op_entry((void*)(fp), (void*)(buf), (void*)(uint32)(len), SD_FREAD)


/*
int sys_vm_read(void *buf, uint32 offset, uint32 len)
该接口用于读取VRAM的数据
buffer：存放的buffer
offset：读取的位置偏移
len：读取的长度(字节单位)
返回值int：0-成功，非0失败
*/
#define sys_vm_read(buf,offset,len)    \
(int)sys_op_entry((void*)(buf), (void*)(uint32)(offset), (void*)(uint32)(len), VM_READ)


/*
int sys_vm_write(void *buf, uint32 offset, uint32 len)
该接口用于写入VRAM的数据
buffer：写入数据的buffer
offset：写入的位置偏移
len：写入的长度(字节单位)
返回值int：0-成功，非0失败
*/
#define sys_vm_write(buf,offset,len)         \
(int)sys_op_entry((void*)(buf), (void*)(uint32)(offset), (void*)(uint32)(len), VM_WRITE)


/*
int sys_request_irq(uint32 type, void *handle)
该接口用于注册中断服务程序
type：中断类型
handle：中断服务程序入口
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_request_irq(type,handle)        \
(int)sys_op_entry((void*)(uint32)(type), (void*)(handle), (void*)0, REQUEST_IRQ)

/*
void sys_free_irq(uint32 type)
该接口用于注销中断
type：中断类型
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_irq(type)             \
(void)sys_op_entry((void*)(uint32)(type), (void*)0, (void*)0, FREE_IRQ) 

/*
void* sys_malloc(uint32 size)
该接口用于申请内存
size：申请内存大小(字节单位)
返回值 内存地址
*/
#define sys_malloc(size)        \
(void *) sys_op_entry((void *)(uint32)(size), (void *)(MEM_DATA), (void *)0, MEM_MALLOC)


#define sys_malloc_bt_data(size) \
(void *) sys_op_entry((void *)(uint32)(size), (void *)(MEM_BT_DATA), (void *)0, MEM_MALLOC) 


#define sys_malloc_bank_data(size) \
(void *) sys_op_entry((void *)(uint32)(size), (void *)(MEM_BANK_DATA), (void *)0, MEM_MALLOC)

#define sys_malloc_large_data(size) \
(void *) sys_op_entry((void *)(uint32)(size), (void *)(MEM_LDATA), (void *)0, MEM_MALLOC)

/*
void sys_free(void* addr)
该接口用于释放申请内存
addr：内存地址
返回值int：0-成功，非0失败
*/    
#define sys_free(addr)             \
(int) sys_op_entry((void *)(addr), (void *)(MEM_DATA), (void *)0, MEM_FREE)

#define sys_free_bt_data(addr)             \
(int) sys_op_entry((void *)(addr), (void *)(MEM_BT_DATA), (void *)0, MEM_FREE)

#define sys_free_bank_data(addr)             \
(int) sys_op_entry((void *)(addr), (void *)(MEM_BANK_DATA), (void *)0, MEM_FREE)

#define sys_free_large_data(addr)             \
(int) sys_op_entry((void *)(addr), (void *)(MEM_LDATA), (void *)0, MEM_FREE)

/*
int mq_send(uint8 queue_id, void *msg)
该接口用于发送系统消息
queue_id：消息类型id
msg：存放消息内容
返回值int：0-成功，非0失败
*/
#define sys_mq_send(queue_id,msg)            \
(int)sys_op_entry((void*)(uint32)(uint8)(queue_id), (void*)(msg), (void*)0, MQ_SEND)

/*
int sys_mq_receive(uint8 queue_id, void *msg)
该接口用于接收系统消息
queue_id：消息类型id
msg：存放消息内容
返回值int：0-成功，非0失败
*/
#define sys_mq_receive(queue_id,msg)         \
(int)sys_op_entry((void*)(uint32)(uint8)(queue_id), (void*)(msg), (void*)0, MQ_RECEIVE) 

/*
int sys_mq_flush(uint8 queue_id)
该接口用于清空系统消息池
queue_id：消息类型id
返回值int：0-成功，非0失败
*/
#define sys_mq_flush(queue_id)             \
(int)sys_op_entry((void*)(uint32)(uint8)(queue_id), (void*)0, (void*)0, MQ_FLUSH)

/*
int sys_mq_traverse(uint8 queue_id, void* msg, uint32 msg_index)
该接口用于 查询/获取 系统消息
queue_id：消息类型id
msg：存放消息内容，若为NULL则只查询消息总数，不获取消息内容
msg_index：获取指定的消息序号
返回值int：消息池消息的总数，为-1 - 操作失败
*/
#define sys_mq_traverse(queue_id,msg,msg_index)      \
(int)sys_op_entry((void*)(uint32)(uint8)(queue_id), (void*)(msg), (void*)(uint32)(msg_index), MQ_TRAVERSE)

/*
int get_fw_info (uint8 *buf, uint32 info_addr, uint32 info_len)
该接口用于获取SD区指定位置数据
ptr_fw_info：存放信息内容
info_addr：sd位置
info_len：数据长度(字节)
返回值int：0-成功，非0失败
*/
#define sys_get_fw_info(buf,info_addr,info_len)      \
(int)sys_op_entry((void*)(buf), (void*)(uint32)(info_addr), (void*)(uint32)(info_len), GET_FW_INFO)

/*
uint32 sys_get_ab_timer(void)
该接口用于系统计算绝对时间，用作时间点统计
返回值 时间，单位ms
*/
#define sys_get_ab_timer()      \
(uint32)sys_op_entry((void*)0, (void*)0, (void*)0, GET_AB_TIMER)

/*
void sys_mdelay(uint32 ms)
该接口用于毫秒级延时
ms：延时时间ms
返回值NULL
*/
#define sys_mdelay(ms)      \
(void)sys_op_entry((void*)(uint32)(ms), (void*)(0), (void*)(0), MDELAY)

/*
void sys_udelay(uint32 us)
该接口用于微秒级延时
us：延时时间us
返回值NULL
*/
#define sys_udelay(us)      \
(void)sys_op_entry((void*)(uint32)(us), (void*)0, (void*)(0), UDELAY) 

/*
void sys_sleep(uint32 s)
该接口用于挂起线程睡眠，秒级延时
s：睡眠时间s
返回值NULL
*/
#define sys_sleep(s)       \
(void)sys_op_entry((void*)(uint32)(s), (void*)(0), (void*)0, ACTIONS_SLEEP)

/*
void sys_usleep(uint32 us)
该接口用于挂起线程睡眠，微级延时
us：睡眠时间us
返回值NULL
*/
#define sys_usleep(us)    \
(void)sys_op_entry((void*)(uint32)(us), (void*)(0), (void*)(0), USLEEP)

/*
int8 sys_set_irq_timer1(void* time_handle, uint32 ms_count)
该接口用于注册毫秒级的定时器
time_handle：定时服务程序入口
ms_count：定时时间(ms)
返回值int：定时器ID，-1为注册失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_irq_timer1(time_handle,ms_count)     \
(int8)(int)sys_op_entry((void*)(time_handle), (void*)(uint32)(ms_count), (void*)0, SET_IRQ_TIMER1)

/*
int sys_del_irq_timer1(int8 timer_id)
该接口用于注销定时器
timer_id：定时器ID
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_del_irq_timer1(timer_id)       \
(int)sys_op_entry((void*)(uint32)(timer_id), (void*)(0), (void*)(0), DEL_IRQ_TIMER1)

/*
void sys_set_time(time_t *time)
该接口用于设置系统时间
time：设置时间值
返回值NULL
*/
#define sys_set_time(time)      \
(void)sys_op_entry((void*)(time), (void*)(0), (void*)(0), SET_TIME)

/*
void sys_get_time(time_t *time)
该接口用于获取系统时间
time：存放时间值
返回值NULL
*/
#define sys_get_time(time)      \
(void)sys_op_entry((void*)(time), (void*)(0), (void*)(0), GET_TIME)

/*
int sys_set_date(date_t *date)
该接口用于设置系统日期
time：设置日期值
返回值int：0-成功，非0失败
*/
#define sys_set_date(date)       \
(int)sys_op_entry((void*)(date), (void*)(0), (void*)(0), SET_DATE)

/*
int sys_get_date(date_t *date)
该接口用于设置系统日期
time：存放日期值
返回值int：0-成功，非0失败
*/
#define sys_get_date(date)      \
(int)sys_op_entry((void*)(date), (void*)(0), (void*)(0), GET_DATE)

/*
void set_alarm_time(time_t *time)
该接口用于设置闹铃时间
time：设置时间值
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_alarm_time(time)   \
(void)sys_op_entry((void*)(time), (void*)(0), (void*)(0), SET_ALARM_TIME)

/*
void sys_get_alarm_time(time_t *time)
该接口用于获取闹铃时间
time：存放时间值
返回值NULL
*/
#define sys_get_alarm_time(time)     \
(void)sys_op_entry((void*)(time), (void*)(0), (void*)(0), GET_ALARM_TIME)

/*
int8 sys_mount_fs(uint8 drv_type, uint8 disk, uint8 work_mode)
该接口用于挂载设备的文件系统
drv_type：设备类型
disk：设备盘符
work_mode：工作模式，默认为0
返回值int：文件系统ID，为-1表示失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_mount_fs(drv_type,disk,work_mode)     \
(int8)(int)sys_op_entry((void*)(uint32)(uint8)(drv_type), (void*)(uint32)(disk), (void*)(uint32)(uint8)(work_mode), MOUNT_FS)

/*
int sys_unmount_fs(int8 id)
该接口用于卸载已挂载的文件系统
id：文件系统ID
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_unmount_fs(id)           \
(int)sys_op_entry((void*)(uint32)(id), (void*)0, (void*)0, UNMOUNT_FS)

/*
uint8 sys_get_fat_type_after_mount(uint32 id)
该接口用于获取挂载后的文件系统的类型 
id：文件系统ID
返回值uint8：文件系统类型 FAT16/FAT32/EXFAT
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_get_fat_type_after_mount(id) \
(uint8)(uint32)sys_op_entry((void*)(uint32)(id), (void*)0, (void*)0, GET_FAT_TYPE_AFTER_MOUNT)

/*
int sys_format_disk(uint8 dry_type, uint8 reserver, uint8 fat_type)
该接口用于格式化指定设备，以及文件系统类型
dry_type：设备类型
reserver：无用，默认为0
fat_type：文件系统类型 FAT16/FAT32/EXFAT
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_format_disk(dry_type,reserver,fat_type)      \
(int)sys_op_entry((void*)(uint32)(dry_type), (void*)(uint32)(reserver), (void*)(uint32)(fat_type), FORMAT_DISK)

/*
uint32 sys_local_irq_save(void)
该接口用于关闭系统中断，并保存中断配置
返回值uint32：保存中断配置
*/
#define sys_local_irq_save()        \
(uint32)sys_op_entry((void*)0, (void*)0, (void*)0, LOCAL_IRQ_SAVE)

/*
void sys_local_irq_restore(uint32 irq_save)
该接口用于恢复中断配置
irq_save：中断配置
返回值NULL
*/
#define sys_local_irq_restore(irq_save)    \
(void) sys_op_entry((void*)(uint32)(irq_save), (void*)0, (void*)0, LOCAL_IRQ_RESTORE)

/*
uint8 sys_os_task_resume(uint8 prio)
该接口用于删除任务
prio：任务优先级
返回值uint8：0-成功，非0失败
*/
#define sys_os_task_resume(prio)       \
(uint8)(uint32)sys_op_entry((void*)(uint32)(prio), (void*)0, (void*)0, OS_TASK_RESUME)

/*
uint8 sys_os_task_suspend(uint8 prio)
该接口用于挂起任务
prio：任务优先级
返回值uint8：0-成功，非0失败
*/
#define sys_os_task_suspend(prio)      \
(uint8)(uint32)sys_op_entry((void*)(uint32)(prio), (void*)0, (void*)0, OS_TASK_SUSPEND)

/*
void sys_os_time_dly(uint16 ticks)
该接口用于任务释放调度时间片
ticks：时钟节拍(10ms为单位)
返回值NULL
*/
#define sys_os_time_dly(ticks)   \
(void)sys_op_entry((void*)(uint32)(ticks), (void*)0, (void*)0, OS_TIME_DLY)

/*
uint8 sys_os_time_dly_resume(uint8 prio)
该接口用于挂起任务，并延时
prio：任务优先级
返回值uint8：0-成功，非0失败
*/
#define sys_os_time_dly_resume(prio)   \
(uint8)(uint32)sys_op_entry((void*)(uint32)(prio), (void*)0, (void*)0, OS_TIME_DLY_RESUME)

/*
void sys_os_sched_lock(void)
该接口用于系统调度上锁
返回值NULL
*/
#define sys_os_sched_lock()    \
(void) sys_op_entry((void*)0, (void*)0, (void*)0, OS_SCHED_LOCK)

/*
void sys_os_sched_unlock(void)
该接口用于系统调度解锁
返回值NULL
*/
#define sys_os_sched_unlock()   \
(void) sys_op_entry((void*)0, (void*)0, (void*)0, OS_SCHED_UNLOCK)


/*
uint32 sys_random(void)
该接口用于获取系统的随机数据
返回值uint32：随机数 0~0xFFFFFFFF
*/
#define sys_random()      \
(uint32)sys_op_entry((void*)0, (void*)0, (void*)0, RANDOM)


/*
int sys_set_sys_info(void *info, uint32 sys_info_type)
该接口用于设置系统信息
info：信息内容
sys_info_type：信息类型
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_sys_info(info,sys_info_type)	     \
(int)sys_op_entry((void*)(uint32)(info), (void*)(uint32)(sys_info_type), (void*)0, SET_SYS_INFO)			

/*
int sys_get_sys_info(void *info, uint32 sys_info_type)
该接口用于获取系统信息
info：存放信息内容
sys_info_type：信息类型
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_get_sys_info(info,sys_info_type)	     \
(int)sys_op_entry((void*)(uint32)(info), (void*)(uint32)(sys_info_type), (void*)0, GET_SYS_INFO)			

/*
int8 sys_request_clkadjust(void *call_back, uint32 pll_range)
该接口用于注册调频回调函数
call_back：回调函数入口
pll_range：限制PLL频率范围，低16位为最小值，高16位为最大值；若值为0表示不限制PLL频率
返回值int：回调id号，-1为失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_request_clkadjust(call_back,pll_range)	\
(int8)(int)sys_op_entry((void*)(call_back), (void*)(uint32)(pll_range), (void*)0, REQUEST_CLKADJUST)

/*
int sys_free_clkadjust(int8 id)
该接口用于注销调频回调函数
id：回调id号
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_free_clkadjust(id)	    \
(int)sys_op_entry((void*)(uint32)(id), (void*)0, (void*)0, FREE_CLKADJUST)			

/*
uint32 sys_adjust_clk(uint32 freq, uint32 type)
该接口用于调整MISP 和 dsp频率
freq：低8位为MISP频率，高8位为DSP频率
type：调用者身份,暂未使用，0-前台，1-后台引擎，2-其他
返回值uint32：之前的频率，低8位为MISP频率，高8位为DSP频率
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_adjust_clk(freq, type)	    \
(uint32)sys_op_entry((void*)(uint32)(freq), (void*)(uint32)(type), (void*)0, ADJUST_CLK)

/*
uint32 sys_get_adjustclk(uint32 *lock_sw)
该接口用于获取MISP 和 dsp频率
lock_sw: 获取当前频率上锁状态， 1--lock，0--unlock
返回值uint32：当前的频率，低8位为MISP频率，高8位为DSP频率
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_get_adjustclk(lock_sw)	        \
(uint32)sys_op_entry((void*)(uint32)(0), (void*)(uint32)(4), (void*)lock_sw, ADJUST_CLK)

/*
uint32 sys_adjust_asrc_clk(uint32 freq)
该接口用于调整ASRC频率
freq：ASRC频率
返回值uint32：之前的频率，ASRC频率
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_adjust_asrc_clk(freq)	    \
(uint32)sys_op_entry((void*)(uint32)(freq), (void*)(uint32)(3), (void*)0, ADJUST_CLK)


/*
int32 sys_respond_dsp_cmd(void)
该接口用于回应DSP服务请求，表示服务已完成，DSP可以开始工作
返回值int：0-成功，非0失败
*/
#define sys_respond_dsp_cmd()      \
(int32)sys_op_entry((void*)0, (void*)0, (void*)0, RESPOND_DSP_CMD)

/*
int32 sys_request_dsp_irq(uint8 in_use, uint8 *cmd_context)
该接口用于请求DSP中断
in_use：请求中断类型信息
cmd_context：命令内容，为NULL--认为不需要传输信息
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_request_dsp_irq(in_use,cmd_context)   \
(int32)sys_op_entry((void*)(uint32)(in_use), (void*)(cmd_context), (void*)0, REQUEST_DSP_IRQ)

/*
int32 sys_set_mpu_irq(sys_mpu_param_t *param)
该接口用于注册MPU中断
param：注册参数类型
返回值int：中断注册ID号
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_mpu_irq(param)        \
(int32)sys_op_entry((void*)(uint32)(IRQ_MPU), (void*)(param), (void*)(0), REQUEST_IRQ)

/*
void sys_del_mpu_irq(id)
该接口用于注销MPU中断
id：中断注册ID号
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_del_mpu_irq(id)         \
(void)sys_op_entry((void*)(IRQ_MPU), (void*)(uint32)(id), (void*)0, FREE_IRQ) 

/*
int sys_set_drv_setting(void *set_info)
该接口用于设置设备驱动配置，如card驱动能力等
set_info：设置信息内容 card_pm_cfg_t
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_drv_setting(set_info)   \
(int)sys_op_entry((void*)(uint32)(set_info), (void*)0, (void*)0, SET_DRV_SETTING)

/*
int sys_set_drv_ops(uint8 drv_type, uint32 ops_addr)
该接口用于重定位驱动的ops地址
drv_type: 驱动类型
ops_addr：驱动地址表
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_drv_ops(drv_type, ops_addr)   \
(int)sys_op_entry((void*)(uint32)(drv_type), (void*)(uint32)(ops_addr), (void*)1, SET_DRV_SETTING)

/*
int sys_get_delay_val(uint32 delay_ms)
该接口用于获取延时计数值
delay_ms: 延时时间
返回值int：延时计数值
*/
#define sys_get_delay_val(delay_ms, div_val)   \
(int)sys_op_entry((void*)(uint32)(delay_ms), (void*)(uint32)(div_val), (void*)2, SET_DRV_SETTING)

/*
int sys_base_set_info(void *info, uint32 type)
该接口用于sd区配置信息
info：配置信息
type：设置类型
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_base_set_info(info,type)     \
(int)sys_op_entry((void*)(info), (void*)(uint32)(type), (void*)0, BASE_SET_INFO)

//标志进入S3BT场景，此时需要对VRAM做特殊处理，不丢弃尾部index
#define sys_base_set_enter_s3bt_scene()  (base_enter_s3bt_scene())

//标志退出S3BT场景，此时不对VRAM做特殊处理，丢弃尾部index
#define sys_base_set_exit_S3bt_scene()   (base_exit_s3bt_scene())
/*
void sys_dsp_print(void)
该接口用于打印dsp缓存数据
返回值int：0-成功，非0失败
*/
#define sys_dsp_print()            \
(void)sys_op_entry((void*)(0), (void*)(0), (void*)(0), DSP_PRINT)

/*
void sys_irq_print(char *str, uint32 data,uint8 mode)
该接口用于系统中断打印
str:字符串
data:数据
mode:打印模式，0-只打印字符串，1-只打印数据，2-打印字符串+数据
返回值int：0-成功，非0失败
*/
#define sys_irq_print(str,data,mode)       \
(void)sys_op_entry((void*)(str), (void*)(uint32)(data), (void*)(uint32)(mode), IRQ_PRINT)


/*
uint32 sys_read_c0count(void)
该接口用于获取系统的cpu计数器值
返回值uint32：范围 0~0xFFFFFFFF
*/
#define sys_read_c0count()         \
(uint32)sys_op_entry((void*)0, (void*)0, (void*)0, RANDOM)

#if 0
/*
void sys_us_timer_start(void)
该接口用于初始微秒计时器
*/
#define sys_us_timer_start()    \
    (void)sys_op_entry((void*)(0), (void*)(0), (void*)(0), RECORD_ABTIME_US)

/*
uint32 sys_us_timer_break(void)
该接口用于获取微秒计时器值
返回值uint32：范围 0~0xFFFFFFFF
!!!NOTE：与sys_us_timer_start配合使用，中间不允许有调频动作，否则计时不准
*/
#define sys_us_timer_break()    \
    (uint32)sys_op_entry((void*)(1), (void*)(0), (void*)(0), RECORD_ABTIME_US)
#endif
/*
void* sys_share_query_creat(int8 query_id, uint8 *mem_addr, uint16 size)
该接口用于创建共享内存查询管理队列，并返回可被写入的内存地址
query_id：队列ID
mem_addr：内存地址
size：内存大小
返回值void*：可写入的内存地址，NULL-为创建失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_share_query_creat(query_id,mem_addr,size)       \
(void*)sys_op_entry((void*)(int32)(query_id), (void*)(mem_addr), (void*)(uint32)(size), SHARE_QUERY_CREAT)

/*
int sys_share_query_destroy(int8 query_id)
该接口用于删除已创建的队列
query_id：队列ID
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_share_query_destroy(query_id)         \
(int)sys_op_entry((void*)(int32)(query_id), (void*)0, (void*)0, SHARE_QUERY_DESTROY)

/*
int sys_share_query_read(int8 query_id, uint8 *read_addr)
该接口用于获取共享内存消息内容
query_id：队列ID
read_addr：存入消息内容
返回值int：0-成功，非0失败
*/
#define sys_share_query_read(query_id,read_addr)   \
(int)sys_op_entry((void*)(int32)(query_id), (void*)(read_addr), (void*)0, SHARE_QUERY_READ)

/*
void* sys_share_query_update(int8 query_id)
该接口用于更新已写入共享内存消息内容，并返回可被写入的内存地址
query_id：队列ID
返回值void*：可被写入的内存地址，NULL-为更新失败
*/
#define sys_share_query_update(query_id)          \
(void*)sys_op_entry((void*)(int32)(query_id), (void*)0, (void*)0, SHARE_QUERY_UPDATE)

/*
int sys_share_query_creat(int8 shm_id, uint8 *shm_addr, uint16 shm_size)
该接口用于创建共享内存空间
shm_id：共享内存标识ID
shm_addr：内存地址
shm_size：内存大小
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_shm_creat(shm_id,shm_addr,shm_size)       \
(int)sys_op_entry((void*)(int32)(shm_id), (void*)(shm_addr), (void*)(uint32)(shm_size), SHM_CTRL_CREAT)

/*
int sys_shm_destroy(int8 shm_id)
该接口用于删除共享内存空间
shm_id：共享内存标识ID
返回值int：0-成功，非0失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_shm_destroy(shm_id)         \
(int)sys_op_entry((void*)(int32)(shm_id), (void*)0, (void*)0, SHM_CTRL_DESTROY)

/*
uint8* sys_shm_mount(int8 shm_id)
该接口用于请求共享内存地址
shm_id：共享内存标识ID
返回值uint8*：共享内存地址，NULL-为请求失败
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_shm_mount(shm_id)           \
(uint8*)sys_op_entry((void*)(int32)(shm_id), (void*)0, (void*)0, SHM_CTRL_MOUNT)

/*
void sys_enter_high_powered(int up_type)
该接口用于启动系统电气特性 性能最优模式
up_type --提示类型，1--VDD, 2--VCC, 3--(VCC + VDD)
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_enter_high_powered(up_type)	 \
(void)sys_op_entry((void*)(uint32)1, (void*)(uint32)SYS_HIGH_PW_MODE_SWITCH, (void*)(uint32)(up_type), SET_SYS_INFO)			

/*
void sys_exit_high_powered(void)
该接口用于退出系统电气特性 性能最优模式
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_exit_high_powered()	     \
(void)sys_op_entry((void*)(uint32)0, (void*)(uint32)SYS_HIGH_PW_MODE_SWITCH, (void*)0, SET_SYS_INFO)			
	

/*
void sys_lock_adjust_freq(uint32 lock_freq)
该接口用于锁定系统当前的MISP&DSP频率，不接受 sys_adjust_clk 频率的调节
lock_freq：低8位为MISP频率，高8位为DSP频率，
           若lock_freq = 0,则锁定为当前跑的频率
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_lock_adjust_freq(lock_freq)	       \
(void)sys_op_entry((void*)(uint32)1, (void*)(uint32)SYS_FREQ_LOCK_SWITCH, (void*)(uint32)(lock_freq), SET_SYS_INFO)			

/*
void sys_unlock_adjust_freq(void)
该接口用于解锁系统的MISP&DSP频率调节,接收sys_adjust_clk 频率的调节
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_unlock_adjust_freq()	     \
(void)sys_op_entry((void*)(uint32)0, (void*)(uint32)SYS_FREQ_LOCK_SWITCH, (void*)0, SET_SYS_INFO)			


/*
void sys_cpu_monitor_start(void)
该接口用于启动cpu使用占空比监控
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_cpu_monitor_start()	       \
(void)sys_op_entry((void*)(uint32)1, (void*)(uint32)SYS_CPU_WORK_MONITOR, (void*)0, SET_SYS_INFO)			

/*
void sys_cpu_monitor_end(void)
该接口用于结束cpu使用占空比监控，查看结果
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_cpu_monitor_end()	     \
(void)sys_op_entry((void*)(uint32)0, (void*)(uint32)SYS_CPU_WORK_MONITOR, (void*)0, SET_SYS_INFO)			


/*
void sys_set_hosc_param(uint16 param)
该接口用于设置高频电容参数值
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_hosc_param(param)	     \
(void)sys_op_entry((void*)(uint32)(uint16)param, (void*)(uint32)SYS_HOSC_PARAMETER, (void*)0, SET_SYS_INFO)			

/*
int sys_read_hosc_param(void)
该接口用于获取高频电容参数值
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_read_hosc_param()	     \
(int)sys_op_entry((void*)0, (void*)(uint32)SYS_HOSC_PARAMETER, (void*)1, SET_SYS_INFO)			

/*
void sys_reset_timer(void)
该接口用于重置系统的定时计数器
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_reset_timer()	     \
(void)sys_op_entry((void*)0, (void*)(uint32)SYS_RESET_TIMER, (void*)0, SET_SYS_INFO)

/*
void sys_avoid_linein_noise(channel_num)
该接口用于开机消除linein底噪
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_avoid_linein_noise(a)	     \
(void)sys_op_entry((void*)(a), (void*)(uint32)SYS_AVOID_LINEIN_NOISE, (void*)0, SET_SYS_INFO)


/*
void sys_print_exception_info(exc_code)
该接口用于主动陷入系统异常并打印异常信息
返回值NULL
!!!exc_code 错误代码，参考exception.h头文件定义
*/
#define sys_print_exception_info(a)	     \
(void)sys_op_entry((void*)(a), (void*)0, (void*)0, PRINT_EXCEPTION_INFO)			

/*
void sys_get_time_of_day(struct timeval* tm)
该接口用于读取系统自开机之后的精确时间，精度微秒
tm：时间
!!!常驻代码，可中断调用
*/							
#define sys_get_time_of_day(a)	     \
(void)sys_op_entry((void*)(a), (void*)0, (void*)0, GET_TIME_OF_DAY)			

/*
void sys_read_mem_use_info(void)
该接口用于读取内存使用信息结构体地址
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_read_mem_use_info()	     \
(int)sys_op_entry((void*)0, (void*)(uint32)SYS_MEM_USE_INFO, (void*)0, SET_SYS_INFO)	

/*
void sys_set_cache_info(void)
该接口用于设置某个ap场景下各子场景cache使用情况
返回值NULL
!!!NOTE:bank代码，禁止在中断调用
*/
#define sys_set_mem_use_info(a)	     \
(int)sys_op_entry((void*)(a), (void*)(uint32)SYS_SET_MEM_USE_INFO, (void*)0, SET_SYS_INFO)	

#define sys_clear_temp_pool()	     \
(int)sys_op_entry((void*)0, (void*)(uint32)SYS_CLEAR_TEMP_POOL, (void*)0, SET_SYS_INFO)	

#define sys_disable_use_temp_pool()	     \
    (int)sys_op_entry((void*)0, (void*)(uint32)SYS_DISABLE_USE_TEMP_POOL, (void*)0, SET_SYS_INFO) 

#define sys_enable_use_temp_pool()	     \
(int)sys_op_entry((void*)0, (void*)(uint32)SYS_ENABLE_USE_TEMP_POOL, (void*)0, SET_SYS_INFO)

#define sys_disable_use_send_buffer()	     \
(int)sys_op_entry((void*)0, (void*)(uint32)SYS_DISABLE_USE_SEND_BUFFER, (void*)0, SET_SYS_INFO)	

#define sys_read_pagemiss_cbk()	     \
(int)sys_op_entry((void*)0, (void*)(uint32)SYS_READ_PAGEMISS_CALLBACK, (void*)0, SET_SYS_INFO)	

#define sys_mem_pool_create(a)  \
(void)sys_op_entry((void*)(a), (void*)0, (void*)0, MEM_POOL_CREATE)

#define sys_mem_pool_clear(a, b)  \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)(MEM_POOL_OP_CLEAR), MEM_POOL_DESTROY)

#define sys_mem_pool_destroy(a, b)  \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)(MEM_POOL_OP_DESTROY), MEM_POOL_DESTROY)

#define sys_mem_pool_force_destroy(a, b)  \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)(MEM_POOL_OP_FORCE_DESTROY), MEM_POOL_DESTROY)

#define sys_set_mem_lock(a, b)  \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)SET_MEM_LOCK, MEM_SET_LOCK_INFO)

#define sys_clr_mem_lock(a, b)  \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)CLR_MEM_LOCK, MEM_SET_LOCK_INFO)

#define sys_enable_mem_use(a,b) \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)(0), MEM_ENABLE_USE)

#define sys_disable_mem_use(a,b) \
(int)sys_op_entry((void*)(a), (void*)(b), (void*)(0), MEM_DISABLE_USE)

#endif




