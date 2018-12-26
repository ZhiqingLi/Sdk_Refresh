/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：
 * 作者：liminxian
 ********************************************************************************/

/*
 *********************************************************************************************************
 *                                                uC/OS-II
 *                                          The Real-Time Kernel
 *
 *                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
 *                                           All Rights Reserved
 *
 *                                  uC/OS-II Configuration File for V2.60
 *
 * File : OS_CFG.H
 * By   : Jean J. Labrosse
 *********************************************************************************************************
 */
#ifndef __OS_CFG_H__
#define __OS_CFG_H__
/* ---------------------- MISCELLANEOUS ----------------------- */
#define OS_ARG_CHK_EN             1    /* Enable (1) or Disable (0) argument checking                  */
/*保留，多一些代码没有关系，都是固化*/
#define OS_CPU_HOOKS_EN           1    /* uC/OS-II hooks are found in the processor port files         */
/*os_cpu_hook.c保留*/

#define OS_EVENT_NAME_SIZE       0    /* Determine the size of the name of a Sem, Mutex, Mbox or Q    */
/*不需要名字标记*/
#define OS_LOWEST_PRIO           15    /* Defines the lowest priority that can be assigned ...         */
/*0到15是16个优先级，已经足够了 OS_TCB *OSTCBPrioTbl[OS_LOWEST_PRIO + 1]*//* ... MUST NEVER be higher than 63!                            */
/* ... MUST be higher than OS_MAX_TASKS!                         */
/*5110*/

#define OS_MAX_EVENTS            15   /* Max. number of event control blocks in your application      */
/*消息队列2个+ 3个，同步互斥量 10个*/
#define OS_MAX_FLAGS             0    /* Max. number of Event Flag Groups    in your application      */
#define OS_MAX_MEM_PART           0    /* Max. number of memory partitions                             */
#define OS_MEMBYTE_EN            0
#define OS_MAX_MEMBYTE_PART       0
#define OS_MAX_QS                 5    /* Max. number of queue control blocks in your application      */
#define OS_MAX_TASKS             8    /* Max. number of tasks in your application, MUST be >= 2       */
/*加上IDLE是9个任务，TCB共9个 OS_TCB OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS]  *//*5116*/

#define OS_SCHED_LOCK_EN          1    /*     Include code for OSSchedLock() and OSSchedUnlock()       */
/*打开禁止调度功能*/
#define OS_TASK_IDLE_STK_SIZE   64   /* Idle task stack size (# of OS_STK wide entries)              */
/*不能调用子函数，如hook*/

#define OS_TASK_STAT_EN           0    /* Enable (1) or Disable(0) the statistics task                 */
#define OS_TASK_STAT_STK_SIZE    64    /* Statistics task stack size (# of OS_STK wide entries)        */
/*OS_TASK_STAT_EN为0时无效*/
#define OS_TASK_STAT_STK_CHK_EN   0    /* Check task stacks from statistic task                        */
/*OS_TASK_STAT_EN为0时无效*/
#define OS_TICKS_PER_SEC        100    /* Set the number of ticks in one second                        */

/*不使用事件标志集*//* ----------------------- EVENT FLAGS ------------------------ */
#define OS_FLAG_EN                0    /* Enable (1) or Disable (0) code generation for EVENT FLAGS    */
#define OS_FLAG_WAIT_CLR_EN       0    /* Include code for Wait on Clear EVENT FLAGS                   */
#define OS_FLAG_ACCEPT_EN         0    /*     Include code for OSFlagAccept()                          */
#define OS_FLAG_DEL_EN            0    /*     Include code for OSFlagDel()                             */
#define OS_FLAG_NAME_SIZE         0    /*     Determine the size of the name of an event flag group    */
#define OS_FLAG_QUERY_EN          0    /*     Include code for OSFlagQuery()                           */
/*5110*/

/*不使用邮箱，邮箱发一次消息，队列就满，相当于队列的长度为1*//* -------------------- MESSAGE MAILBOXES --------------------- */
#define OS_MBOX_EN                0    /* Enable (1) or Disable (0) code generation for MAILBOXES      */
#define OS_MBOX_ACCEPT_EN         0    /*     Include code for OSMboxAccept()                          */
#define OS_MBOX_DEL_EN            0    /*     Include code for OSMboxDel()                             */
#define OS_MBOX_POST_EN           0    /*     Include code for OSMboxPost()                            */
#define OS_MBOX_POST_OPT_EN       0    /*     Include code for OSMboxPostOpt()                         */
#define OS_MBOX_QUERY_EN          0    /*     Include code for OSMboxQuery()                           */

/*自己实现动态堆管理，不使用ucos的内存管理，ucos的内存管理类似于3X的物理内存管理，内存块的大小是一定的，但数据结构的开销小*//* --------------------- MEMORY MANAGEMENT -------------------- */
#define OS_MEM_EN                 0    /* Enable (1) or Disable (0) code generation for MEMORY MANAGER */
#define OS_MEM_QUERY_EN           0    /*     Include code for OSMemQuery()                            */
#define OS_MEM_NAME_SIZE         32    /*     Determine the size of a memory partition name            */

/*不使用互斥量，互斥量可以解决优先级反转的问题，但需要指定一个优先级，不好用。可以在信号量中实现互斥量，但要开发人员保证不死锁*//* ---------------- MUTUAL EXCLUSION SEMAPHORES --------------- */
#define OS_MUTEX_EN               0    /* Enable (1) or Disable (0) code generation for MUTEX          */
#define OS_MUTEX_ACCEPT_EN        0    /*     Include code for OSMutexAccept()                         */
#define OS_MUTEX_DEL_EN           0    /*     Include code for OSMutexDel()                            */
#define OS_MUTEX_QUERY_EN         0    /*     Include code for OSMutexQuery()                          */

/*使用消息队列*//* ---------------------- MESSAGE QUEUES ---------------------- */
#define OS_Q_EN                   0    /* Enable (1) or Disable (0) code generation for QUEUES         */
#define OS_Q_ACCEPT_EN            0    /*     Include code for OSQAccept()                             */
/*使用accept*/
#define OS_Q_DEL_EN               0    /*     Include code for OSQDel()                                */
/*不会删除消息队列，所以可以不要，但由于固化代码，暂保留*/
#define OS_Q_FLUSH_EN             0    /*     Include code for OSQFlush()                              */
#define OS_Q_POST_EN              0    /*     Include code for OSQPost()                               */
/*<<<<<add by wurui for posix queue*/
#define OS_Q_POST_BY_PRIO_EN      0    /*     Include code for OSQPostByPrio()                         */
/*>>>>>*/
/*改为有时间限制的post即可，不需根据优先级调整，用于同步消息发送*/
#define OS_Q_POSTISR_EN           0    /*     Include code for OSQPost()                               */
/*这个不需要，嗲用QSQPost即可*/
#define OS_Q_POST_FRONT_EN        0    /*     Include code for OSQPostFront()                          */
#define OS_Q_POST_OPT_EN          0    /*     Include code for OSQPostOpt()                            */
#define OS_Q_QUERY_EN             0    /*     Include code for OSQQuery()                              */
/*消息都是FIFO，不需实现LIFO*/

/*使用信号量，其可以用于互斥量和信号量*//* ------------------------ SEMAPHORES ------------------------ */
#define OS_SEM_EN                 1    /* Enable (1) or Disable (0) code generation for SEMAPHORES     */
#define OS_SEM_ACCEPT_EN          1    /*    Include code for OSSemAccept()                            */
#define OS_SEM_DEL_EN             1    /*    Include code for OSSemDel()                               */
#define OS_SEM_QUERY_EN           0    /*    Include code for OSSemQuery()                             */

/* --------------------- TASK MANAGEMENT ---------------------- */
#define OS_TASK_CHANGE_PRIO_EN    0    /*     Include code for OSTaskChangePrio()                      */
/*AP使用过程中不会修改优先级*/
#define OS_TASK_CREATE_EN         1    /*     Include code for OSTaskCreate()                          */
/*使用非扩展的创建，任务指向进程管理的数据结构指针可以通过pdata传进来，约定好即可*/
#define OS_TASK_CREATE_EXT_EN     0    /*     Include code for OSTaskCreateExt()                       */
/*浪费数据空间，所以不使用*/
#define OS_TASK_DEL_EN            1    /*     Include code for OSTaskDel()                             */
/*AP退出时需要这个*/
#define OS_TASK_NAME_SIZE        0    /*     Determine the size of a task name                        */
/*5110*/
#define OS_TASK_PROFILE_EN        0    /*     Include variables in OS_TCB for profiling                */
#define OS_TASK_QUERY_EN          1    /*     Include code for OSTaskQuery()                           */
#define OS_TASK_SUSPEND_EN        1    /*     Include code for OSTaskSuspend() and OSTaskResume()      */
#define OS_TASK_SW_HOOK_EN        1    /*     Include code for OSTaskSwHook()                          */
/*需要这个hook，在切换过程做一些工作*/

/* --------------------- TIME MANAGEMENT ---------------------- */
#define OS_TIME_DLY_HMSM_EN       0    /*     Include code for OSTimeDlyHMSM()                         */
/*已经有10ms一个节拍的延时函数，不需要这个时分秒的延时函数*/
#define OS_TIME_DLY_RESUME_EN     1    /*     Include code for OSTimeDlyResume()                       */
/*需要*/
#define OS_TIME_GET_SET_EN        0    /*     Include code for OSTimeGet() and OSTimeSet()             */
/*不需要获取这个时间，系统已经有RTC 日历和CPU时间*//*5110*/

/*!
 *   软中断任务优先级配置，5110不使用软中断
 */
#define SOFTIRQ_TASK_PRIO       0
#define TIMER_TASK_PRIO_LO  (OS_LOWEST_PRIO-2)
#define TIMER_TASK_ID_LO    (TIMER_TASK_PRIO_LO | (TIMER_TASK_PRIO_LO<<8))

/*!
 *   线程栈大小的上限值（64K）
 */
#define MAX_TASK_STACK_SIZE  0x10000

/*!
 *   线程内核栈大小（2K）
 */
#define KERNEL_STACK_SIZE  0x800

/*!
 *   连续物理内存的大小界限(*4KB)
 */
#define LARGE_DMA_MEM_MIN_SIZE  200

/*!
 *   配置MALLOC的的实现方式（0---noya方式；1---kmalloc方式）
 */
#define MALLOC_REALIZE_TYPE 0

/*!
 *   配置是否使能缺页异常中开中断
 */
#define ENABLE_PAGE_INVALID_EI  1

/*!
 *   字符设备管理数组的个数配置（超过的部分将通过动态申请/释放）
 */
#define MAX_CHRDEV      9
/*!
 *   块设备管理数组的个数配置（超过的部分将通过动态申请/释放）
 */
#define MAX_BLKDEV      3

/*!
 *   配置是否使能加载时代码校验
 */
#define ENALBE_LOAD_CODECHECK 1
/*!
 *   配置是否使能chipid
 */
#define ENABLE_CHIPID 0

/*!
 *   配置系统消息池层数
 */
#define MSGPOOL_DEPTH 10

#define SEIRAL_PRINT_ENABLE     (1)
#define SERIAL_PEINT_DISABLE    (0)

#ifndef _ASSEMBLER_

#endif

#endif/*__OS_CFG_H__*/
