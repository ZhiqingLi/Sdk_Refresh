/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：libc 接口定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef __LIBC_DEFINE_H__
#define __LIBC_DEFINE_H__

#include <attr.h>
#include <typeext.h>

typedef enum
{
	MEMCPY = 0,
	MEMSET,
	MEMCMP,
	STRLEN,
	STRNCAT,
	STRNCMP,
	STRNCPY,
	STRLENUNI,
	STRNCPYUNI,
	STRNCATUNI,
	ITOA,
	PRINT,

	PTHREAD_EXIT,
	PTHREAD_CREATE,
	EXIT,
	WAITPID,
	GET_PROCESS_STRUCT,
	FREE_PROCESS_STRUCT,
	SEM_INIT,
	SEM_DESTROY,
	SEM_WAIT,
	SEM_TRYWAIT,
	SEM_POST,
	PTHREAD_MUTEX_INIT,
	PTHREAD_MUTEX_DESTROY,
	PTHREAD_MUTEX_LOCK,
	PTHREAD_MUTEX_UNLOCK,
	PTHREAD_MUTEX_TRYLOCK,
	PTHREAD_COND_INIT,
	PTHREAD_COND_DESTROY,
	PTHREAD_COND_WAIT,
	PTHREAD_COND_SIGNAL,
	DMA_PRINT,
	
	C_RESERVE_FUN1,
	C_RESERVE_FUN2,

} libc_cmd_e;

/*
 KERNEL向上层提供统一的接口，即只提供一个API，并以ID的方式区分
 */
// dma print define 
typedef struct
{
    union
    {
        uint32 value;
        uint8  *buf;    
    }data;    
    uint32 len; 
    uint8  dma_channel;       
}dma_print_info_t; 

#define DMA_PRINT_VAL              0x01
//PRINT BUF
#define DMA_PRINT_BUF1             0x02
#define DMA_PRINT_BUF2             0x03



extern void* libc_op_entry(void *param1, void *param2, void *param3, libc_cmd_e cmd_type) __FAR__;

typedef void* (*libc_op_i)(void*, void*, void*);

struct libc_operations {
	libc_op_i memcpy;
	libc_op_i memset;
	libc_op_i memcmp;
	libc_op_i strlen;
	libc_op_i strncat;
	libc_op_i strncmp;
	libc_op_i strncpy;
	libc_op_i strlenuni;
	libc_op_i strncpyuni;
	libc_op_i strncatuni;
	libc_op_i itoa;
	libc_op_i print;

	libc_op_i pthread_exit;
	libc_op_i pthread_create;
	libc_op_i exit;
	libc_op_i waitpid;
	libc_op_i get_process_struct;
	libc_op_i free_process_struct;
	libc_op_i sem_init;
	libc_op_i sem_destroy;
	libc_op_i sem_wait;
	libc_op_i sem_trywait;
	libc_op_i sem_post;
	libc_op_i pthread_mutex_init;
	libc_op_i pthread_mutex_destroy;
	libc_op_i pthread_mutex_lock;
	libc_op_i pthread_mutex_unlock;
	libc_op_i pthread_mutex_trylock;
	libc_op_i pthread_cond_init;
	libc_op_i pthread_cond_destroy;
	libc_op_i pthread_cond_wait;
	libc_op_i pthread_cond_signal;
    libc_op_i dma_print;
	
	libc_op_i c_reserve1;
	libc_op_i c_reserve2;

};

/*
void* libc_memcpy(void* dest, void* src, uint32 count)
该接口用于内存拷贝
dest:目标地址
src:源地址
count:拷贝字节个数
返回值非0
*/
#define libc_memcpy(dest,src,count)          \
(void*)libc_op_entry((void*)(dest), (void*)(src), (void*)(uint32)(count), MEMCPY)

/*
void* libc_memset(void * dest, uint8 value, uint32 count)
该接口用于写入指定值到内存
dest:目标地址
value:地址值
count:写入字节个数
返回值非0
*/
#define libc_memset(dest,value,count)          \
(void*)libc_op_entry((void*)(dest), (void*)(uint32)(uint8)(value), (void*)(uint32)(count), MEMSET)

/*
int libc_memcmp(void* src1, void* src2, uint32 count)
该接口用于比较指定长度数据是否相同
src1:数据1地址
src2:数据2地址
count:比较字节个数
返回值int:0-相同，非0-不相同
*/
#define libc_memcmp(src1,src2,count)          \
(int)libc_op_entry((void*)(src1), (void*)(src2), (void*)(uint32)(count), MEMCMP)

/*
uint32 libc_strlen(char *str)
该接口用于统计字符串的长度,长度不包含结束符
str:字符串
返回值uint32:字符串长度
*/
#define libc_strlen(str)              \
(uint32)libc_op_entry((void*)(str), (void*)0, (void*)0, STRLEN)

/*
char* libc_strncat(char *str1, char *str2, uint32 count)
该接口用于合并两个字符串，把str2指定长度的字符串合并到str1中，末尾自动填入结束符
str1:字符串1
str2:字符串2
count:合并str2中的个数
返回值(char*):合并后的字符串地址，即str1
*/
#define libc_strncat(str1,str2,count)         \
(char *)libc_op_entry((void*)(str1), (void*)(str2), (void*)(uint32)(count), STRNCAT)

/*
int libc_strncmp(char *str1, char *str2, uint32 count)
该接口用于比较两个字符串的指定长度的大小
str1:字符串1
str2:字符串2
count:比较字符串长度
返回值int:          > 0:  str1 > str2
                    = 0:  str1 = str2
                    < 0:  str1 < str2
*/
#define libc_strncmp(str1,str2,count)         \
(int)libc_op_entry((void*)(str1), (void*)(str2), (void*)(uint32)(count), STRNCMP)

/*
char* libc_strncpy(char *dest_str, char *src_str, uint32 count)
该接口用于从src_str中拷贝指定长度字符到dest_str字符串中，末尾自动填入结束符
dest_str:目的字符串地址
src_str:源字符串地址
count:拷贝str2中的字符个数
返回值(char*):拷贝后的字符串地址，即str1
*/
#define libc_strncpy(dest_str,src_str,count)         \
(char *)libc_op_entry((void*)(dest_str), (void*)(src_str), (void*)(uint32)(count), STRNCPY)

/*
uint32 libc_strlenuni(uint8 *unicode)
该接口用于计算unicode字符的长度，不包含结束符
unicode:存放unicode内容
返回值uint32:长度，字节单位
*/
#define libc_strlenuni(unicode)           \
(uint32)libc_op_entry((void*)(unicode), (void*)0, (void*)0, STRLENUNI)

/*
uint8* libc_strncpyuni(uint8 *dest_uni, uint8 *src_uni,uint32 count_uni)
该接口用于unicode的拷贝，末尾自动填入结束符
dest_uni:目标unicode
src_uni:源unicode
count_uni:拷贝unicode个数(2字节为单位)
返回值(uint8*):拷贝后的unicode地址，即dest_uni
*/
#define libc_strncpyuni(dest_uni,src_uni,count_uni)      \
(uint8*)libc_op_entry((void*)(dest_uni), (void*)(src_uni), (void*)(uint32)(count_uni), STRNCPYUNI)

/*
uint8* libc_strncatuni(uint8 *dest_uni, uint8 *src_uni,uint32 count_uni)
该接口用于unicode的合并,合并指定个数的unicode，末尾自动填入结束符
dest_uni:目标unicode
src_uni:源unicode
count_uni:拷贝unicode个数(2字节为单位)
返回值(uint8*):拷贝后的unicode地址，即dest_uni
*/
#define libc_strncatuni(dest_uni,src_uni,count_uni)		\
(uint8*)libc_op_entry((void*)(dest_uni), (void*)(src_uni), (void*)(uint32)(count_uni), STRNCATUNI)

/*
uint8 libc_itoa(uint32 data, uint8 *str, uint8 str_count)
该接口用于把整数转换出指定位数的的字符串，字符串末尾自动填入结束符
data:待转换的整数
str:存放转换后的字符串，至少9byte长度
str_count:指定位数，从低到高 1~8
返回值uint8:实际转换成字符的个数
*/
#define libc_itoa(data,str,str_count)            \
(uint8)(uint32)libc_op_entry((void*)(uint32)(data), (void*)(str), (void*)(uint32)(uint8)(str_count), ITOA)

/*
void libc_print(char *str, uint32 data,uint8 mode)
该接口用于非中断模式下调试打印
str:字符串
data:数据
mode:打印模式: 0-只打印字符串，1-只打印数据，2-打印字符串+数据
                3-打印缓存，str为缓存地址，data为打印字节长度
返回值void
*/
#define libc_print(str,data,mode)           \
(void)sys_op_entry((void*)(str), (void*)(uint32)(data), (void*)(uint32)(uint8)(mode), SYSTEM_CPU_PRINT)//PRINT

/*
void libc_pthread_exit(void)
该接口用于终止线程运行
返回值void
*/
#define libc_pthread_exit()         \
(void)libc_op_entry((void*)0, (void*)0, (void*)0, PTHREAD_EXIT)

/*
int libc_pthread_create(pthread_ext_param_t *pthread_param, uint8 prio, uint8 process_descr_index)
该接口用于创建线程运行
pthread_param:线程信息
prio:优先级
process_descr_index：线程id
返回值int:0-成功，非0失败
*/

#define libc_pthread_create(pthread_param,prio,process_descr_index)  \
(int)sys_op_entry((void*)(pthread_param), (void*)(uint32)(prio), (void*)(uint32)(process_descr_index), PTHREAD_CREATE_FIX)

/*
void libc_exit(int8 no)
该接口用于进程退出结束
no:退出标识
返回值void
*/
#define libc_exit(no)                \
(void)libc_op_entry((void*)(int32)(no), (void*)0, (void*)0, EXIT)

/*
int libc_waitpid(int8 *stat_loc, int options)
该接口用于等待进程结束是否
stat_loc:填入进程退出返回值
options:等待条件，0--死等，非0--等不到退出
返回值int:-1-没有等待结束进程，0~3对应结束进程ID
*/
#define libc_waitpid(stat_loc,options)           \
(int)libc_op_entry((void*)(stat_loc), (void*)(int)(options), (void*)0, WAITPID)

/*
int8 libc_get_process_struct(void)
该接口用于申请空闲进程ID
返回值int8:-1-没有等待结束进程，0~3对应结束进程ID
*/
#define libc_get_process_struct()   \
(int8)(int32)libc_op_entry((void*)0, (void*)0, (void*)0, GET_PROCESS_STRUCT)

/*
void libc_free_process_struct(int8 id)
该接口用于释放进程
id:进程标识
返回值void
*/
#define libc_free_process_struct(id) \
(void)libc_op_entry((void*)(int32)(id), (void*)0, (void*)0, FREE_PROCESS_STRUCT)

/*
int libc_sem_init(os_event_t ** sem, uint32 value)
该接口用于创建信号量
*sem:存放信号量
value:初始值
返回值int:0-成功，非0失败
*/
#define libc_sem_init(sem,value)          \
(int)libc_op_entry((void*)(sem), (void*)(uint32)(value), (void*)0, SEM_INIT)

/*
int libc_sem_destroy(os_event_t ** sem)
该接口用于销毁信号量
*sem:存放信号量
返回值int:0-成功，非0失败
*/
#define libc_sem_destroy(sem)         \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, SEM_DESTROY)

/*
int libc_sem_wait(os_event_t *sem, uint16 timeout)
该接口用于等待信号量
sem:信号量类型
timeout:超时时间，0-为死等，>0 为时钟节拍个数
返回值int:0-成功，非0失败
*/
#define libc_sem_wait(sem,timeout)          \
(int)libc_op_entry((void*)(sem), (void*)(uint32)(timeout), (void*)0, SEM_WAIT)

/*
int libc_sem_trywait(os_event_t *sem)
该接口用于无等待地请求一个信号量
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_sem_trywait(sem)         \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, SEM_TRYWAIT)

/*
int libc_sem_post(os_event_t *sem)
该接口用于发送一个信号量
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_sem_post(sem)            \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, SEM_POST)

/*
int libc_pthread_mutex_init(os_event_t ** sem)
该接口用于创建访问互斥信号量
*sem:存放信号量
value:初始值
返回值int:0-成功，非0失败
*/
#define libc_pthread_mutex_init(sem)      \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_MUTEX_INIT)

/*
int libc_pthread_mutex_destroy(os_event_t ** sem)
该接口用于销毁访问互斥信号量
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_pthread_mutex_destroy(sem)   \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_MUTEX_DESTROY)

/*
int libc_pthread_mutex_lock(os_event_t ** sem)
该接口用于访问互斥信号量上锁
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_pthread_mutex_lock(sem)      \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_MUTEX_LOCK)

/*
int libc_pthread_mutex_unlock(os_event_t ** sem)
该接口用于访问互斥信号量解锁
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_pthread_mutex_unlock(sem)    \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_MUTEX_UNLOCK)

/*
int libc_pthread_mutex_trylock(os_event_t *sem)
该接口用于无等待地请求一个互斥信号量
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_pthread_mutex_trylock(sem)   \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_MUTEX_TRYLOCK)

/*
int libc_pthread_cond_init(os_event_t ** sem，uint32 value)
该接口用于等待访问信号量解锁
sem:信号量类型
value：初始值
返回值int:0-成功，非0失败
*/
#define libc_pthread_cond_init(sem,value)       \
(int)libc_op_entry((void*)(sem), (void*)(value), (void*)0, PTHREAD_COND_INIT)

/*
int libc_pthread_cond_destroy(os_event_t ** sem)
该接口用于销毁访问互斥信号量
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_pthread_cond_destroy(sem)    \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_COND_DESTROY)

/*
int libc_sem_wait(os_event_t *sem, os_event_t *mutex, uint16 timeout)
该接口用于等待信号量
sem:信号量类型
mutex：互斥信号量
timeout:超时时间，0-为死等，>0 为时钟节拍个数
返回值int:0-成功，非0失败
*/
#define libc_pthread_cond_wait(sem,mutex,timeout)   \
(int)libc_op_entry((void*)(sem), (void*)(mutex), (void*)(uint32)(timeout), PTHREAD_COND_WAIT)

/*
int libc_pthread_cond_signal(os_event_t *sem)
该接口用于发送一个等待信号量
sem:信号量类型
返回值int:0-成功，非0失败
*/
#define libc_pthread_cond_signal(sem)     \
(int)libc_op_entry((void*)(sem), (void*)0, (void*)0, PTHREAD_COND_SIGNAL)

/*
void libc_dma_print(uint8 *str_buf, dma_print_info_t *data_info, uint8 mode)
该接口用于获取系统信息
str_buf:打印字符内容
data_info:打印配置
mode：打印模式,DMA_PRINT_VAL/DMA_PRINT_BUF1/DMA_PRINT_BUF2
返回值void
*/
#define libc_dma_print(str_buf,data_info,mode)           \
(void)sys_op_entry((void*)(str_buf), (void*)(data_info), (void*)(uint32)(mode), SYSTEM_DMA_PRINT)//DMA_PRINT



#include <debug_interface.h>
#endif

