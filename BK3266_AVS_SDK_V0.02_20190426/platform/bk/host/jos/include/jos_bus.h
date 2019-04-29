/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_BUS_H_
#define _JOS_BUS_H_

#include <jos/include/jos_sync.h>

typedef enum {
    /* Soft Interrupts */
    J_TASK_CONTROLLER =         THREAD_PRIORITY_CONTROLLER,

# ifndef CONFIG_TASK_SINGLE
#  ifdef CONFIG_DEFERRED_COMPLETION
    J_TASK_DEFERRED_COMP =      THREAD_PRIORITY_DEFERRED_COMPLETION,
#  endif
    /* Timeout Tasks */
    J_TASK_TIMEOUT =            THREAD_PRIORITY_CORE,

    /* Plug n' Play tasks - Used for attach/detach of devices (explore task) */
    J_TASK_PNP =                THREAD_PRIORITY_DRIVER,

#endif /* CONFIG_TASK_SINGLE */
    /* Must be last! */
    TASK_TYPE_COUNT
} jtask_type;

/* Backward Compatabilities Macros (<= uw-3.6) */
#define TASK_CONTROLLER         J_TASK_CONTROLLER
#define TASK_CORE               J_TASK_TIMEOUT
#define TASK_CLASS_DRIVER       J_TASK_PNP
#ifndef CONFIG_SINGLE_THREADED
j_scheduler_h j_get_thread_ctx(jtask_type type);							 
/* In single threaded configuration jtask_init defined in os_port.h */
#define jtask_init(task_h, type) j_task_init(task_h, j_get_thread_ctx(type))
#endif /* CONFIG_SINGLE_THREADED */

#ifndef CONFIG_USERSPACE_CLASS
os_bus_h j_controller_get_bus(j_device_h dev);
#endif

#endif

