/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_INTERNAL_H_
#define _JOS_INTERNAL_H_

#define JOS_IMPLEMENT
#include <include/jos.h>

struct j_device_s {
    /* A pointer to the driver that owns the device */
    j_driver_h          jdrv;
    /* Own/Disown methods */
    device_own_t        own;
    device_disown_t     disown;

    /* Link in the driver's device list */
    TAILQ_ENTRY(j_device_s) next_in_drv_list;

    struct j_device_s   *parent;
    void                *owner_ctx;
    j_device_type_t     type;

    void                *creator_ctx;
    void                *ivars;
    j_device_aa_h       dev_aa;

    uint32_t            unit;
    char                *nameunit;
    char                *desc;

    TAILQ_HEAD(, j_device_s) children_list;
    TAILQ_ENTRY(j_device_s) next_child;
};

result_t jos_safe_init(void);
void jos_safe_uninit(void);
result_t jos_sync_init(void);
void jos_sync_uninit(void);
result_t mbuf_init(void);
void mbuf_uninit(void);
result_t domain_init(void);
void domain_uninit(void);
result_t soinit(void);
void souninit(void);
result_t selinit(void);
void seluninit(void);
result_t jos_driver_init(void);
void jos_driver_uninit(void);
result_t j_bus_init(uw_args_t *args);
void j_bus_uninit(void);
result_t j_threads_init(void);
void j_threads_uninit(void);

/* Drivers & devices internal functions */
void j_device_retry_attach(void);
void j_device_set_nameunit(j_device_h dev, const char *name, uint32_t unit);

/* Event Handler */
result_t j_event_init(void);
void j_event_uninit(void);

/* DMA block descriptor */
typedef struct dma_block_s {
    mem_desc_t  buffer;
    uint32_t    size;
    uint16_t    align;
#define BLOCK_CACHABLE   0x01
#define BLOCK_PAGE_ALIGN 0x02
#define BLOCK_OWNER      0x04
#define BLOCK_DUMMY      0x08
#define BLOCK_KERNEL     0x10
    uint8_t     flags;
    void        *os_handle;
#ifdef J_DEBUG
    bool_t      split;
    const char  *file;
    int_t       line;
    int_t       freed;
    LIST_ENTRY(dma_block_s) next_alloc;
#endif
    LIST_ENTRY(dma_block_s) next_free;
} jdma_t;

result_t jos_mem_init(uw_args_t *args);
void jos_mem_uninit(void);
void jdma_free_all(void);

struct jtask_s {
    struct jtask_s      *next;
    jtime_t             timeout;
    j_scheduler_h       thread;
    jthread_func        cb;
    void                *arg;
#define JTASK_STOPPED           0
#define JTASK_RUNNING           1
#define JTASK_PENDING           2
#define JTASK_RUN_N_PEND        3
#define JTASK_STOPPING          4
    uint8_t             state;
    bool_t              pend_free;
#ifdef J_DEBUG
    const char          *file;
    int_t               line;
#endif
};

#ifdef J_DEBUG
void debug_init(void);
#endif

#ifdef CONFIG_USER_SPACE
result_t jos_umem_init(void);
void jos_umem_uninit(void);

result_t ju_init(void);
void ju_uninit(void);

result_t ju_event_init(void);
void ju_event_uninit(void);
#endif
#ifdef CONFIG_KERNEL_SPACE
result_t jos_kmem_init(void);
void jos_kmem_uninit(void);

result_t jk_init(void);
void jk_uninit(void);

result_t jk_event_init(void);
void jk_event_uninit(void);
#endif

#endif

