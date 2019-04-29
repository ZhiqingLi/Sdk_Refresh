/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_H_
#define _PORT_H_

#include <config/config.h>
#include <uw_compiler.h>
#include <include/uw_controllers.h>
#include <include/os_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void *os_bus_resource_h;
typedef void *os_bus_h;
typedef void *os_mutex_h;
typedef void *os_timer_h;
typedef void *os_msleep_h;

#ifdef CONFIG_FOPS_SLEEP
typedef void *os_fops_msleep_h;
#endif

typedef void (*jthread_func)(void *arg);

#define ISR_DONT_CALL_SOFT_INTR 0
#define ISR_CALL_SOFT_INTR 1
#define ISR_NOT_RECOGNIZED -1

typedef int_t (*interrupt_handler)(void *arg);

typedef struct os_time {
    int32_t tv_sec;     /* seconds */
    int32_t tv_usec;    /* microseconds */
} os_time_t;

typedef enum {
    CONTROLLER_HOST,
    CONTROLLER_DEVICE,
    CONTROLLER_OTG
} controller_t;

#include "os/port_sync.h"
#include "os/port_mem.h"
#include "os/port_generic.h"
#include "os/port_bus.h"
#ifdef CONFIG_KERNEL_SPACE
#include "os/port_kernel.h"
#elif defined(CONFIG_USER_SPACE)
#include "os/port_user.h"
#endif
#ifdef CONFIG_JFS
#include "os/port_fs.h"
#endif
#include "uw_args.h"

#ifdef CONFIG_BUS_PCI
# include <os_pci.h>
# include "os/port_pci.h"
#endif

#ifdef CONFIG_STATISTICS
# include "os/port_statistics.h"
#endif

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
# include "os/port_test.h"
#endif

#ifdef CONFIG_PORT_SQL
# include "os/port_sql.h"
#endif

/* USB stack APIs to be used by the porting layer */

/* Initialize and uninitialize USBware */
result_t j_stack_init(uw_args_t *args);
void j_stack_uninit(void);

/* Stack loading unloading - called from porting layer */
result_t jstart_stack(uw_args_t *args);
void jstop_stack(void);

/* JOS device manipulation - to be used by drivers */
#include <jos/include/jos_device.h>

/* Attach arguments for bus controller drivers */
typedef struct {
    os_bus_h bus;
    int_t id;
    void *ivars;
} controller_attach_arg_t;

/* Register a controller with the host stack */
result_t add_generic_controller(os_bus_h bus, int_t id, void **handle);

/* Unregister a controller */
result_t del_generic_controller(void *handle);

result_t generic_controller_suspend(void *handle);
result_t generic_controller_resume(void *handle);

#ifdef __cplusplus
}
#endif

#endif

