/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_SYNC_H_
#define _PORT_SYNC_H_

/* Thread priorities enumeration.
 * The first three values must correspond to jtask_type and are for use by
 * the stack's task threads.
 * THREAD_PRIORITY_OTHER is used by all additional threads required for
 * specific driver purposes.
 * The priorities are ordered from highest to lowest (0 - highest) */
typedef enum {
    THREAD_PRIORITY_CONTROLLER = 0,
#ifdef CONFIG_DEFERRED_COMPLETION
    THREAD_PRIORITY_DEFERRED_COMPLETION,
#endif
    THREAD_PRIORITY_CORE,
    THREAD_PRIORITY_DRIVER,
#ifdef CONFIG_MEDIACORE
    THREAD_PRIORITY_MEDIA_CONTROL,
    THREAD_PRIORITY_MEDIA_SCAN,
#ifndef CONFIG_KERNEL_SPACE
    THREAD_PRIORITY_MEDIA_DB,
#endif
#endif
    THREAD_PRIORITY_OTHER
#ifdef CONFIG_MEDIACORE_APP
    ,THREAD_PRIORITY_MEDIACORE_UI
#endif
} os_thread_priority;

result_t os_thread_create(jthread_func func, void *arg,
    os_thread_priority priority, const char *name, void **handle);
#ifdef CONFIG_THREAD_DESTROY
void os_thread_destroy(void *handle);
#endif

#ifdef CONFIG_ALLOW_STACK_EXTERNAL
result_t ose_thread_create(jthread_func func, void *arg,
    os_thread_priority priority, const char *name, void **handle);
void ose_delay_ms(uint32_t msec);
result_t ose_msleep(os_msleep_h msleep_h, uint32_t timeout);
#endif

uint32_t os_get_context(void);

result_t os_mutex_init(os_mutex_h *mtx);
void os_mutex_uninit(os_mutex_h mtx);
void os_mutex_lock(os_mutex_h mtx);
void os_mutex_unlock(os_mutex_h mtx);

result_t os_timer_init(os_timer_h *timer);
void os_timer_uninit(os_timer_h timer);
result_t os_timer_reset(os_timer_h timer, uint32_t msec, jthread_func func, void *arg);
void os_timer_stop(os_timer_h timer);
bool_t os_timer_pending(os_timer_h timer);

void os_delay_ns(uint32_t nsec);
#ifndef CONFIG_LOOPS_PER_MSEC
void os_delay_us(uint32_t usec);
#endif
void os_delay_ms(uint32_t msec);

result_t os_msleep_init(os_msleep_h *msleep_h);
void  os_msleep_uninit(os_msleep_h msleep_h);
result_t os_msleep(os_msleep_h msleep_h, uint32_t timeout);
result_t os_wakeup(os_msleep_h msleep_h);

#ifdef CONFIG_FOPS_SLEEP
result_t os_fops_msleep_init(os_fops_msleep_h *msleep_h);
void  os_fops_msleep_uninit(os_fops_msleep_h msleep_h);
result_t os_fops_msleep(os_fops_msleep_h msleep_h, uint32_t timeout);
result_t os_fops_wakeup(os_fops_msleep_h msleep_h);
#endif
__INLINE__ void set_system_idle_cnt(int16 cnt_10ms);
__INLINE__ int16 get_system_idle_cnt(void);

void os_get_time(os_time_t *time);
uint32_t os_get_hz(void);

/* context tracker (ISR, DSR or normal) */
#define ISR_CONTEXT     0x1
#define DSR_CONTEXT     0x2
#define NORMAL_CONTEXT  0x4

#define ASSERT_CONTEXT(valid_ctx) do {} while (0)

#endif

