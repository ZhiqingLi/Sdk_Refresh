/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_BUS_
#define _PORT_BUS_

result_t os_interrupt_setup(os_bus_resource_h res,
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg);

result_t os_interrupt_teardown(os_bus_resource_h res);

void os_critical_enter(void);
void os_critical_leave(void);

result_t os_hw_init(void);
void os_hw_uninit(void);

typedef enum {
    SYS_RES_IRQ  = 1,   /* Interrupt lines */
    SYS_RES_MEMORY,     /* I/O memory */
    SYS_RES_IOPORT,     /* I/O ports */
    SYS_RES_DMA         /* DMA Controller */
} os_resource_type_t;

os_bus_resource_h os_bus_alloc_resource(os_bus_h bus,
    os_resource_type_t type, int_t id);
void os_bus_release_resource(os_bus_h bus, os_bus_resource_h res);

uint8_t  os_bus_space_read_1(os_bus_resource_h res, uint32_t offset);
uint16_t os_bus_space_read_2(os_bus_resource_h res, uint32_t offset);
uint32_t os_bus_space_read_4(os_bus_resource_h res, uint32_t offset);

void os_bus_space_write_1(os_bus_resource_h res, uint32_t offset,
    uint8_t value);
void os_bus_space_write_2(os_bus_resource_h res, uint32_t offset,
    uint16_t value);
void os_bus_space_write_4(os_bus_resource_h res, uint32_t offset,
    uint32_t value);
#ifdef CONFIG_NATIVE_UINT64
void os_bus_space_write_8(os_bus_resource_h res, uint32_t offset,
    uint64_t value);
#endif

typedef void *os_bus_dma_channel_h;

typedef void (*os_bus_dma_cb_t)(void *arg, uint32_t length, result_t status);

typedef struct {
    uint8_t ep_index;
    bool_t is_rx;
    uint32_t packet_size;
} os_bus_dma_channel_params_t;

typedef struct {
    phys_addr cpu_addr;
    phys_addr usb_addr;
    uint32_t size;
    int_t (*isr)(void *arg);
    void *isr_arg;
    void (*softintr)(void *arg);
    void *softintr_arg;
    /* TODO remove the following pair */
    os_bus_dma_cb_t callback;
    void *arg;
    bool_t send_zero_length_packet;
} os_bus_dma_transfer_params_t;

result_t os_bus_dma_channel_alloc(os_bus_resource_h res,
    os_bus_dma_channel_h *dma_channel, os_bus_dma_channel_params_t *params);
void os_bus_dma_channel_free(os_bus_resource_h res,
    os_bus_dma_channel_h dma_channel);
result_t os_bus_dma_start_transfer(os_bus_resource_h res,
    os_bus_dma_channel_h dma_channel,
    os_bus_dma_transfer_params_t *params);
uint32_t os_bus_dma_stop_transfer(os_bus_resource_h res,
    os_bus_dma_channel_h dma_channel);

os_bus_h os_bus_get_transceiver(os_bus_h bus, int_t id);
int_t os_bus_get_controller_id(os_bus_h bus);

#define ENABLE_VBUS 1
#define DISABLE_VBUS 0
void os_set_vbus_state(bool_t enable);

#endif

