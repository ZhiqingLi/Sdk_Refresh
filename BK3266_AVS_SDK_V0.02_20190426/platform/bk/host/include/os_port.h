/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _OS_PORT_H_
#define _OS_PORT_H_

/*
 * Functions and prototypes for use by the USBware stack only
 */

#include <os_common.h>
#include <port.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define jbus_dma_channel_h      os_bus_dma_channel_h
#define jbus_resource_h         os_bus_resource_h
#define jbus_h                  os_bus_h
#define jtime_t                 os_time_t

#define j_get_curr_context      os_get_context

#ifdef CONFIG_SINGLE_THREADED
#define jmsleep_h              os_msleep_h
#define jmutex_h               os_mutex_h
#define jthread_create(a,b,c,d,e)        UWE_INVAL
#ifdef CONFIG_THREAD_DESTROY
#define jthread_destroy(a)    do { } while (0)
#endif
#define je_thread_create(a,b,c,d,e)      UWE_OK
#define jmutex_init(a)         0
#define jmutex_uninit(a)       do { } while (0)
#define jmutex_lock(a)         do { } while (0)
#define jmutex_unlock(a)       do { } while (0)

#define je_mutex_init(a)       0
#define je_mutex_uninit(a)     do { } while (0)
#define je_mutex_lock          do { } while (0)
#define je_mutex_unlock        do { } while (0)

#define je_cnt_mutex_init(a)   UWE_OK
#define je_cnt_mutex_uninit(a) do { } while (0)
#define je_cnt_mutex_lock(a)   do { } while (0)
#define je_cnt_mutex_unlock(a) do { } while (0)
#define j_cnt_mutex_init(a)    UWE_OK
#define j_cnt_mutex_uninit(a)  do { } while (0)
#define j_cnt_mutex_lock(a)    do { } while (0)
#define j_cnt_mutex_unlock(a)  do { } while (0)


#define j_delay_ns             os_delay_ns
#define j_delay_us             os_delay_us
#define j_delay_ms(a)          os_delay_us((a) * 1000)
#define je_delay_ms            j_delay_ms

#define jtask_h                os_timer_h
#define jtask_init(a,b)        os_timer_init(a)
#define jtask_uninit           os_timer_uninit
#define jtask_schedule         os_timer_reset
#define jtask_stop             os_timer_stop
#define jtask_is_pending       os_timer_is_pending

#endif

#define jget_time               os_get_time
#define jget_hz                 os_get_hz

#define jprintf                (void)os_printf
/* Do not allow use of j_panic in release */
#define j_memcmp                os_memcmp
#define j_snprintf              os_snprintf
#define j_strcmp                os_strcmp
#define j_strncmp               os_strncmp
#define j_strcat                os_strcat
#define j_strncat               os_strncat
#define j_memset                os_memset
#define j_memcpy                os_memcpy
#define j_strlen                os_strlen

#define jhw_init                os_hw_init
#define jhw_uninit              os_hw_uninit

#define j_bus_alloc_resource    os_bus_alloc_resource
#define jbus_release_resource   os_bus_release_resource

#define j_bus_space_read_1      os_bus_space_read_1
#define j_bus_space_read_2      os_bus_space_read_2
#define j_bus_space_read_4      os_bus_space_read_4
#define j_bus_space_write_1     os_bus_space_write_1
#define j_bus_space_write_2     os_bus_space_write_2
#define j_bus_space_write_4     os_bus_space_write_4
#define j_bus_space_write_8     os_bus_space_write_8

#define jinterrupt_teardown     jos_interrupt_teardown
#define jinterrupt_setup        jos_interrupt_setup
void jos_interrupt_teardown(jbus_resource_h res, void *handle);
result_t jos_interrupt_setup(jbus_resource_h res,
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg, void **handle);

#define j_bus_get_transceiver   os_bus_get_transceiver
#define j_bus_get_controller_id os_bus_get_controller_id
#define j_bus_dma_channel_alloc os_bus_dma_channel_alloc
#define j_bus_dma_channel_free  os_bus_dma_channel_free
#define j_bus_dma_start_transfer os_bus_dma_start_transfer
#define j_bus_dma_stop_transfer os_bus_dma_stop_transfer
#define j_bus_dma_channel_params_t os_bus_dma_channel_params_t
#define j_bus_dma_transfer_params_t os_bus_dma_transfer_params_t

/* Flush & Invalidate operations are performed per region */
#define j_cache_invalidate             jos_cache_invalidate
#define j_cache_flush                  jos_cache_flush
void jos_cache_flush(mem_desc_h membuf, uint32_t size);
void jos_cache_invalidate(mem_desc_h membuf, uint32_t size);

enum jdrv_ids {
    JDRV_ID_INVAL = 0,
    JDRV_ID_TEST,
    JDRV_ID_PTP,
    JDRV_ID_SCO,
    JDRV_ID_L2CAP,
    JDRV_ID_RFCOMM,
    JDRV_ID_PAN,
    JDRV_ID_IPOD,
    JDRV_ID_IPOD_CTRL,
    JDRV_ID_MEDIACORE,
    JDRV_ID_BLUETOOTH,
    JDRV_ID_MEM,
    JDRV_ID_QMI,
    JDRV_ID_NETWORK_IFC,
    JDRV_ID_AOAP,
    JDRV_ID_SPP,
    JDRV_ID_EVENT_IPC,
    JDRV_ID_IPC_TEST
};
typedef uint32_t jdrv_id_t;

#define jdrv_id_str(id) \
    ((id == JDRV_ID_INVAL)      ? "JDRV_ID_INVAL"       : \
    (id == JDRV_ID_TEST)        ? "JDRV_ID_TEST"        : \
    (id == JDRV_ID_PTP)         ? "JDRV_ID_PTP"         : \
    (id == JDRV_ID_SCO)         ? "JDRV_ID_SCO"         : \
    (id == JDRV_ID_L2CAP)       ? "JDRV_ID_L2CAP"       : \
    (id == JDRV_ID_RFCOMM)      ? "JDRV_ID_RFCOMM"      : \
    (id == JDRV_ID_PAN)         ? "JDRV_ID_PAN"         : \
    (id == JDRV_ID_IPOD)        ? "JDRV_ID_IPOD"        : \
    (id == JDRV_ID_MEDIACORE)   ? "JDRV_ID_MEDIACORE"   : \
    (id == JDRV_ID_BLUETOOTH)   ? "JDRV_ID_BLUETOOTH"   : \
    (id == JDRV_ID_MEM)         ? "JDRV_ID_MEM"         : \
    (id == JDRV_ID_QMI)         ? "JDRV_ID_QMI"         : \
    (id == JDRV_ID_NETWORK_IFC) ? "JDRV_ID_NETWORK_IFC" : \
    (id == JDRV_ID_AOAP)        ? "JDRV_ID_AOAP"        : \
    (id == JDRV_ID_SPP)         ? "JDRV_ID_SPP"         : \
    (id == JDRV_ID_EVENT_IPC)   ? "JDRV_ID_EVENT_IPC"   : \
    (id == JDRV_ID_IPC_TEST)    ? "JDRV_ID_IPC_TEST"    : \
    "Unknown")

typedef struct {
    jdrv_id_t jdrv_id;
} jdrv_open_info_t;

#define JDRV_POLL_MSG  0x01
#define JDRV_POLL_READ 0x02
typedef uint32_t jdrv_poll_flags_t;

#define j_set_vbus_state(enable)       do {} while (0)

#ifdef __cplusplus
}
#endif

#endif

