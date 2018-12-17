/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：进程管理定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include <typeext.h>
#include <ucos/os.h>

#define CREATE_NOT_MAIN_THREAD 0xff
#define WAITPID_ONLY_PROCESS_MANAGER -1
#define WAITPID_NO_PROCESS_EXIT 0xf

/*!
 *     线程内部结构:用于保存线程的入口及参数构
 */
struct pthread_start_args {
	/* function to run */
	void * (*start_routine)(void *);
	/* its argument */
	void * arg;
};

/*!
 *     线程内部结构:线程描述符指向的结构
 */
typedef struct _pthread_descr_struct {
	/* 描述符链表结构指针 */
	struct _pthread_descr_struct *next;
	/* 线程创建的参数 */
	struct pthread_start_args p_start_args;
	/* 所属的进程结构索引 */
	uint8 process_descr_index;
	/* 被pthread_exit()终止,则为true */
	int8 p_terminated;
	/* 优先级 */
	uint8 prio;
} pthread_descr_t;

/*!
 *  \brief
 *     pthread_once参数定义
 */
typedef int pthread_once_t;

/* 进程数据结构 */
typedef struct {
	pthread_descr_t *descr_head; /* 线程描述符链表头 */
	pthread_once_t exit_once; /* 确保exit只被调用一次 */
	int8 visible_thread_num; /*当前活跃线程数，非僵死线程*/
	int8 is_used;
	int8 exit_val; /* 进程退出返回值 */
} process_struct_t;

#define PROCESS_STRUCT_NUM 5
#define WNOHANG 0xff

typedef struct {
	void *(*start_rtn)(void*);
	void *arg;
	OS_STK *ptos;
} pthread_param_t;

typedef struct
{
    pthread_param_t pthread_param;
    uint16   stk_size;
} pthread_ext_param_t;

extern void pthread_exit(void* null0, void* null1, void* null2);
extern int pthread_create(pthread_param_t *pthread_pram, INT8U prio,
		uint8 process_descr_index);
extern int pthread_create_fix(pthread_ext_param_t *pthread_pram, INT8U prio,
		uint8 process_descr_index);		
extern void exit(int8 exitval, void* null1, void* null2);
extern int waitpid(int8 *stat_loc, int options, void* null2);
extern int8 get_process_struct(void* null0, void* null1, void* null2);
extern void free_process_struct(int8 index, void* null1, void* null2);

extern int sem_init(os_event_t ** sem, unsigned int value, void* null2);
extern int sem_destroy(os_event_t ** sem, void* null1, void* null2);
extern int sem_wait(os_event_t *sem, unsigned short timeout, void* null2);
extern int sem_trywait(os_event_t *sem, void* null1, void* null2);
extern int sem_post(os_event_t *sem, void* null1, void* null2);

extern int pthread_mutex_init(os_event_t ** mutex, void* null1, void* null2);
extern int pthread_mutex_destroy(os_event_t ** mutex, void* null1, void* null2);
extern int pthread_mutex_lock(os_event_t *mutex, void* null1, void* null2);
extern int pthread_mutex_unlock(os_event_t *mutex, void* null1, void* null2);
extern int pthread_mutex_trylock(os_event_t *mutex, void* null1, void* null2);

extern int pthread_cond_init(os_event_t ** cond, uint32 init_value, void* null2);
extern int pthread_cond_destroy(os_event_t ** cond, void* null1, void* null2);
extern int pthread_cond_wait(os_event_t *cond, os_event_t *mutex,
		unsigned short timeout);
extern int pthread_cond_signal(os_event_t *cond, void* null1, void* null2);

extern struct _pthread_descr_struct *__thread_special(uint8 prio);
extern int _thread_kill(uint8 prio);
extern struct _pthread_descr_struct *__thread_self(void);
extern int
		pthread_once(pthread_once_t *once_control, void(*init_routine)(void));
extern int thread_remove_from_process(pthread_descr_t *desrc,
		uint8 process_descr_index);
extern void pthread_init(void);

#endif

