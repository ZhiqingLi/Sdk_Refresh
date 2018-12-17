/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：将ucos代码规范化后的的数据结构和函数声明，提供给应用和内核模块使用
 * 作者：liminxian
 ********************************************************************************/

/*
 *********************************************************************************************************
 *                                             MISCELLANEOUS
 *********************************************************************************************************
 */
#ifndef __OS_H__
#define __OS_H__

#include <ucos/os_cfg.h>
#include <typeext.h>

#define  OS_VERSION              252                    /* Version of uC/OS-II (Vx.yy mult. by 100)    */

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif

#define  OS_PRIO_SELF           0xFF                    /* Indicate SELF priority                      */

#if OS_TASK_STAT_EN > 0
#define  OS_N_SYS_TASKS            2                    /* Number of system tasks                      */
#else
#define  OS_N_SYS_TASKS            1
#endif

#define  OS_STAT_PRIO       (OS_LOWEST_PRIO - 1)        /* Statistic task priority                     */
#define  OS_IDLE_PRIO       (OS_LOWEST_PRIO)            /* IDLE      task priority                     */

#define  OS_EVENT_TBL_SIZE (((OS_LOWEST_PRIO) / 8) + 1)   /* Size of event table                         */
#define  OS_RDY_TBL_SIZE   (((OS_LOWEST_PRIO) / 8) + 1)   /* Size of ready table                         */

#define  OS_TASK_IDLE_ID       65535                    /* I.D. numbers for Idle and Stat tasks        */
#define  OS_TASK_STAT_ID       65534

#define  OS_EVENT_EN       (((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))

/*
 *********************************************************************************************************
 *                              TASK STATUS (Bit definition for OSTCBStat)
 *********************************************************************************************************
 */
#define  OS_STAT_RDY            0x00        /* Ready to run                                            */
#define  OS_STAT_SEM            0x01        /* Pending on semaphore                                    */
#define  OS_STAT_MBOX           0x02        /* Pending on mailbox                                      */
#define  OS_STAT_Q              0x04        /* Pending on queue                                        */
#define  OS_STAT_SUSPEND        0x08        /* Task is suspended                                       */
#define  OS_STAT_MUTEX          0x10        /* Pending on mutual exclusion semaphore                   */
#define  OS_STAT_FLAG           0x20        /* Pending on event flag group                             */

/*
 *********************************************************************************************************
 *                                        OS_EVENT types
 *********************************************************************************************************
 */
#define  OS_EVENT_TYPE_UNUSED      0
#define  OS_EVENT_TYPE_MBOX        1
#define  OS_EVENT_TYPE_Q           2
#define  OS_EVENT_TYPE_SEM         3
#define  OS_EVENT_TYPE_MUTEX       4
#define  OS_EVENT_TYPE_FLAG        5

/*
 *********************************************************************************************************
 *                                         EVENT FLAGS
 *********************************************************************************************************
 */
#define  OS_FLAG_WAIT_CLR_ALL      0        /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_AND      0

#define  OS_FLAG_WAIT_CLR_ANY      1        /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_OR       1

#define  OS_FLAG_WAIT_SET_ALL      2        /* Wait for ALL    the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_AND      2

#define  OS_FLAG_WAIT_SET_ANY      3        /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_OR       3

#define  OS_FLAG_CONSUME        0x80        /* Consume the flags if condition(s) satisfied             */

#define  OS_FLAG_CLR               0
#define  OS_FLAG_SET               1

/*
 *********************************************************************************************************
 *       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
 *********************************************************************************************************
 */
#define  OS_DEL_NO_PEND            0
#define  OS_DEL_ALWAYS             1

/*
 *********************************************************************************************************
 *                                     OS???PostOpt() OPTIONS
 *
 * These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
 *********************************************************************************************************
 */
#define  OS_POST_OPT_NONE       0x00        /* Post to highest priority task waiting                   */
#define  OS_POST_OPT_BROADCAST  0x01        /* Broadcast message to ALL tasks waiting                  */
#define  OS_POST_OPT_FRONT      0x02        /* Post to highest priority task waiting                   */

/*
 *********************************************************************************************************
 *                                 TASK OPTIONS (see OSTaskCreateExt())
 *********************************************************************************************************
 */
#define  OS_TASK_OPT_STK_CHK  0x0001        /* Enable stack checking for the task                      */
#define  OS_TASK_OPT_STK_CLR  0x0002        /* Clear the stack when the task is create                 */
#define  OS_TASK_OPT_SAVE_FP  0x0004        /* Save the contents of any floating-point registers       */

/*
 *********************************************************************************************************
 *                                             ERROR CODES
 *********************************************************************************************************
 */
#define OS_NO_ERR                 0

#define OS_ERR_EVENT_TYPE         1
#define OS_ERR_PEND_ISR           2
#define OS_ERR_POST_NULL_PTR      3
#define OS_ERR_PEVENT_NULL        4
#define OS_ERR_POST_ISR           5
#define OS_ERR_QUERY_ISR          6
#define OS_ERR_INVALID_OPT        7
#define OS_ERR_TASK_WAITING       8

#define OS_TIMEOUT               10
#define OS_TASK_NOT_EXIST        11
#define OS_PNAME_NULL            12
#define OS_MBOX_FULL             20

#define OS_Q_FULL                30
#define OS_Q_EMPTY               31

#define OS_PRIO_EXIST            40
#define OS_PRIO_ERR              41
#define OS_PRIO_INVALID          42

#define OS_SEM_OVF               50

#define OS_TASK_DEL_ERR          60
#define OS_TASK_DEL_IDLE         61
#define OS_TASK_DEL_REQ          62
#define OS_TASK_DEL_ISR          63
#define OS_TASK_NAME_TOO_LONG    64

#define OS_NO_MORE_TCB           70

#define OS_TIME_NOT_DLY          80
#define OS_TIME_INVALID_MINUTES  81
#define OS_TIME_INVALID_SECONDS  82
#define OS_TIME_INVALID_MILLI    83
#define OS_TIME_ZERO_DLY         84

#define OS_TASK_SUSPEND_PRIO     90
#define OS_TASK_SUSPEND_IDLE     91

#define OS_TASK_RESUME_PRIO     100
#define OS_TASK_NOT_SUSPENDED   101

#define OS_MEM_INVALID_PART     110
#define OS_MEM_INVALID_BLKS     111
#define OS_MEM_INVALID_SIZE     112
#define OS_MEM_NO_FREE_BLKS     113
#define OS_MEM_FULL             114
#define OS_MEM_INVALID_PBLK     115
#define OS_MEM_INVALID_PMEM     116
#define OS_MEM_INVALID_PDATA    117
#define OS_MEM_INVALID_ADDR     118

#define OS_ERR_NOT_MUTEX_OWNER  120

#define OS_TASK_OPT_ERR         130

#define OS_ERR_DEL_ISR          140
#define OS_ERR_CREATE_ISR       141

#define OS_FLAG_INVALID_PGRP    150
#define OS_FLAG_ERR_WAIT_TYPE   151
#define OS_FLAG_ERR_NOT_RDY     152
#define OS_FLAG_INVALID_OPT     153
#define OS_FLAG_GRP_DEPLETED    154

#define USER_DEFINE_NO_RUNNING	200

#ifndef _ASSEMBLER_
/*
 *********************************************************************************************************
 *                                          EVENT CONTROL BLOCK
 *********************************************************************************************************
 */

#if (OS_EVENT_EN > 0) && (OS_MAX_EVENTS > 0)
#define OS_EVENT_EXT_TYPE_COND 0
#define OS_EVENT_EXT_TYPE_MUTEX 1
#define OS_EVENT_EXT_TYPE_SEM 2
#define OS_EVENT_EXT_TYPE_MQ 3

typedef struct {
	unsigned char os_event_type; /* Type of event control block (see OS_EVENT_TYPE_???)      */
	unsigned char os_event_grp; /* Group corresponding to tasks waiting for event to occur  */
	unsigned short os_event_cnt; /* Semaphore Count (not used if other EVENT type)           */
	void *os_event_ptr; /* Pointer to message or queue structure                    */
	unsigned char os_event_tbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
#if OS_EVENT_NAME_SIZE > 1
	unsigned char os_event_name[OS_EVENT_NAME_SIZE];
#endif
}os_event_t;
#endif

/*
 *********************************************************************************************************
 *                                          MESSAGE QUEUE DATA
 *********************************************************************************************************
 */

#if 1
typedef enum {
	MQ_ID_MNG = 0, /*进程管理应用消息队列*/
	MQ_ID_DESK,    /*UI（前台）应用消息队列*/
	MQ_ID_EGN,     /*引擎（后台）应用消息队列*/
	MQ_ID_BT,      /*蓝牙后台消息队列*/
	MQ_ID_RES,     /*reserve*/
	MQ_ID_SYS,     /*系统消息队列*/
	MQ_ID_GUI,     /*GUI消息队列*/	
	MQ_ID_MAX
} mq_id_e;

typedef UINT16 os_q_msg_sys_t;
#define OS_Q_MSG_SYS_LEN 4
typedef UINT32 os_q_msg_gui_t;
#define OS_Q_MSG_GUI_LEN 12
typedef struct {
	UINT32 type; /* 消息类型，4B */
	union {
		UINT8 data[4];
		void *addr;
	} content; /* 消息内容或其地址，由消息自己约定，4B*/
} os_q_msg_apps_t;

typedef struct {
	os_q_msg_apps_t msg; /*消息内容，8B*/
	os_event_t *sem; /*信号量，4B*/
	void *reply;
} os_q_msg_private_t;

#define OS_Q_MSG_PRIVATE_LEN 5

typedef struct {
	os_q_msg_apps_t msg; /*消息内容，8B*/
	os_event_t *sem; /*信号量，4B*/
	void *reply;
    UINT8 reserve[2];
    UINT8 bd_addr[6];
} os_q_msg_btstack_t;

#define OS_Q_MSG_BTSTACK_LEN 5

typedef struct {
	os_q_msg_apps_t msg; /*消息内容，8B*/
    UINT8 reserve[2];
    UINT8 bd_addr[6];
} os_q_event_btstack_t;

#define OS_Q_EVENT_BTSTACK_LEN 12

typedef enum {
	SYS_MSG = 0, GUI_MSG, PRIVATE_MSG
} os_q_msg_type_e;

#endif

#if OS_Q_EN > 0
#endif

/*
 *********************************************************************************************************
 *                                           SEMAPHORE DATA
 *********************************************************************************************************
 */

#if OS_SEM_EN > 0
typedef struct {
	unsigned short os_cnt; /* Semaphore count                                         */
	unsigned char os_event_tbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                */
	unsigned char os_event_grp; /* Group corresponding to tasks waiting for event to occur */
}os_sem_data_t;
#endif

/*
 *********************************************************************************************************
 *                                          TASK CONTROL BLOCK
 *********************************************************************************************************
 */

typedef struct os_tcb_s {
	unsigned int *os_tcb_stk_ptr; /* Pointer to current top of stack                              */

	void *os_tcb_ext_ptr; /* Pointer to user definable data for TCB extension             */

	struct os_tcb_s *os_tcb_next; /* Pointer to next     TCB in the TCB list                      */
	struct os_tcb_s *os_tcb_prev; /* Pointer to previous TCB in the TCB list                      */

#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0)
	os_event_t *os_tcb_event_ptr; /* Pointer to event control block                               */
#endif

#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0)
	void *os_tcb_msg; /* Message received from OSMboxPost() or OSQPost()              */
#endif

	unsigned short os_tcb_dly; /* Nbr ticks to delay task or, timeout waiting for event        */
	unsigned char os_tcb_stat; /* Task status                                                  */
	unsigned char os_tcb_prio; /* Task priority (0 == highest, 63 == lowest)                   */

	unsigned char os_tcb_x; /* Bit position in group  corresponding to task priority (0..7) */
	unsigned char os_tcb_y; /* Index into ready table corresponding to task priority        */
	unsigned char os_tcb_bit_x; /* Bit mask to access bit position in ready table               */
	unsigned char os_tcb_bit_y; /* Bit mask to access bit position in ready group               */

#if OS_TASK_DEL_EN > 0
unsigned char os_tcb_del_req; /* Indicates whether a task needs to delete itself              */
#endif
} os_tcb_t;

#define os_task_create OSTaskCreate
#define os_task_del OSTaskDel
#define os_task_del_request OSTaskDelReq
#define os_task_query OSTaskQuery
#define os_task_resume OSTaskResume
#define os_task_suspend OSTaskSuspend
#define os_sem_accept OSSemAccept
#define os_sem_create OSSemCreate
#define os_sem_del OSSemDel
#define os_sem_pend OSSemPend
#define os_sem_post OSSemPost

#define os_time_dly  OSTimeDly
#define os_time_dly_resume  OSTimeDlyResume
#define os_sched_lock  OSSchedLock
#define os_sched_unlock  OSSchedUnlock

/*
 *********************************************************************************************************
 *                                         MESSAGE QUEUE MANAGEMENT
 *********************************************************************************************************
 */

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
#endif

/*
 *********************************************************************************************************
 *                                          SEMAPHORE MANAGEMENT
 *********************************************************************************************************
 */
#if OS_SEM_EN > 0

#if OS_SEM_ACCEPT_EN > 0
extern unsigned short os_sem_accept(os_event_t *pevent, void* null1, void* null2);
#endif

extern os_event_t *os_sem_create(unsigned short cnt, void* null1, void* null2);

#if OS_SEM_DEL_EN > 0
extern os_event_t *os_sem_del(os_event_t *pevent, unsigned char opt, unsigned char *err);
#endif

extern void os_sem_pend(os_event_t *pevent, unsigned short timeout, unsigned char *err);
extern unsigned char os_sem_post(os_event_t *pevent, void* null1, void* null2);

#if OS_SEM_QUERY_EN > 0
extern unsigned char os_sem_query(os_event_t *pevent, os_sem_data_t *pdata);
#endif

#endif

/*
 *********************************************************************************************************
 *                                            TASK MANAGEMENT
 *********************************************************************************************************
 */
#if OS_TASK_CHANGE_PRIO_EN > 0
extern unsigned char os_task_change_prio(unsigned char oldprio, unsigned char newprio);
#endif

#if OS_TASK_CREATE_EN > 0
typedef struct
{
	void (*task)(void *pd);
	void *pdata;
	unsigned int *ptos;
}task_param_t;

extern unsigned char os_task_create (task_param_t *p_task_param, unsigned char prio, void *pext);

#endif

#if OS_TASK_DEL_EN > 0
extern unsigned char os_task_del(unsigned char prio, void* null1, void* null2);
extern unsigned char os_task_del_request(unsigned char prio , void* null1, void* null2);
#endif

#if OS_TASK_SUSPEND_EN > 0
extern unsigned char os_task_resume(unsigned char prio , void* null1, void* null2);
extern unsigned char os_task_suspend(unsigned char prio , void* null1, void* null2);
#endif

#if OS_TASK_QUERY_EN > 0
extern unsigned char os_task_query(unsigned char prio, os_tcb_t *pdata, void* null2);
#endif

/*
 *********************************************************************************************************
 *                                            TIME MANAGEMENT
 *********************************************************************************************************
 */

extern void os_time_dly(unsigned short ticks, void* null1, void* null2);

#if OS_TIME_DLY_HMSM_EN > 0
#endif

#if OS_TIME_DLY_RESUME_EN > 0
extern unsigned char os_time_dly_resume(unsigned char prio, void* null1, void* null2);
#endif

#if OS_TIME_GET_SET_EN > 0
extern unsigned int os_time_get(void);
extern void os_time_set(unsigned int ticks);
#endif

/*
 *********************************************************************************************************
 *                                             MISCELLANEOUS
 *********************************************************************************************************
 */

#if OS_SCHED_LOCK_EN > 0

extern void os_sched_lock(void* null0, void* null1, void* null2);
extern void os_sched_unlock(void* null0, void* null1, void* null2);
#endif

extern void os_sched(unsigned int cause, void *event);

/*
 *********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 *                                  (Compiler Specific ISR prototypes)
 *********************************************************************************************************
 */

extern void cp0_v_enable_im(unsigned int wcp0_im);

#endif/*_ASSEMBLER_*/
#endif/*__OS_H__*/
