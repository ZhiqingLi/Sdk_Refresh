/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#include "includes.h"

/* interrupt service routine */
void os_generic_isr(void)
{
}

/* Setup interrupt handler functions for an interrupt resource */
result_t os_interrupt_setup(os_bus_resource_h res_h, interrupt_handler isr_func,
    void *isr_arg, jthread_func dsr_func, void *dsr_arg)
{
    return UWE_OK;
}

/* Remove interrupt handler */
result_t os_interrupt_teardown(os_bus_resource_h res_h)
{
    return UWE_OK;
}

/* Critical Section enter/leave */
void os_critical_enter(void)
{
    /* lock */
    DBG_I(DPORT_SYNC, ("not implemented\n"));
}

void os_critical_leave(void)
{
    /* unlock */
    DBG_I(DPORT_SYNC, ("not implemented\n"));
}

extern result_t uart_init_and_attach(void);

/* Discover all controllers on bus and call add_generic_controller for each */
result_t os_hw_init(void)
{
    return uart_init_and_attach();
}

/* Remove all controllers, call del_generic_controller for each */
void os_hw_uninit(void)
{
}

/* Find a particular type of resource on the bus */
os_bus_resource_h os_bus_alloc_resource(os_bus_h bus_h,
    os_resource_type_t type, int_t id)
{
    return NULL;
}

/* Release resource handler */
void os_bus_release_resource(os_bus_h bus_h, os_bus_resource_h res_h)
{
}

/* Read a Byte from the resource */
uint8_t os_bus_space_read_1(os_bus_resource_h res_h, uint32_t offset)
{
    return *(uint8_t *)(((os_bus_resource_t *)(res_h))->rsrc.mem_addr + offset);
}

/* Read a Word from the resource */
uint16_t os_bus_space_read_2(os_bus_resource_h res_h, uint32_t offset)
{
    return *(uint16_t *)(((os_bus_resource_t *)
        (res_h))->rsrc.mem_addr + offset);
}

/* Read a DWord from the resource */
uint32_t os_bus_space_read_4(os_bus_resource_h res_h, uint32_t offset)
{
    return *(uint32_t *)(((os_bus_resource_t *)
        (res_h))->rsrc.mem_addr + offset);
}

/* Write a Byte to the resource */
void os_bus_space_write_1(os_bus_resource_h res_h, uint32_t offset,
    uint8_t value)
{
    *(uint8_t *)(((os_bus_resource_t *)
        (res_h))->rsrc.mem_addr + offset) = value;
}

/* Write a Word to the resource */
void os_bus_space_write_2(os_bus_resource_h res_h, uint32_t offset,
    uint16_t value)
{
    *(uint16_t *)(((os_bus_resource_t *)
        (res_h))->rsrc.mem_addr + offset) = value;
}

/* Write a DWord to the resource */
void os_bus_space_write_4(os_bus_resource_h res_h, uint32_t offset,
    uint32_t value)
{
    *(uint32_t *)(((os_bus_resource_t *)
        (res_h))->rsrc.mem_addr + offset) = value;
}

