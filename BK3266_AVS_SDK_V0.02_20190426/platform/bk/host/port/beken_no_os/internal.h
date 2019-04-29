/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _INTERNAL_H_
#define _INTERNAL_H_

/* Generic resource on the bus */
typedef struct os_bus_resource {
    int_t                       id;
    os_resource_type_t          type;
    union {
        uint32_t mem_addr;
        uint32_t irq_num;
    } rsrc;
    void                        *data;
    struct os_bus_resource      *next;
} os_bus_resource_t;


/* Bus structure (one per controller) */
typedef struct os_bus {
    int_t               type;
    os_bus_resource_h   res_list;
    void                *device_handle;
    void                *data;
    struct os_bus       *next;
} os_bus_t;

#define MSEC_IN_TICK               (10)

typedef struct os_timer {
    bool_t is_pending;
    uint64_t ticks;
    jthread_func f;
    void *arg;
    struct os_timer *next;
} os_timer_t;
#endif

