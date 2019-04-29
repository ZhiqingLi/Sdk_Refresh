/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "includes.h"
#include <jos.h>
#include <os/port_uart.h>


typedef struct {
    j_device_h uart_dev;
    port_uart_owner_ops_t *uart_owner_ops;
} uart_softc_t;

static result_t uart_write(j_device_h dev, const uint8_t *buf, uint32_t len,
    uint32_t *done)
{
    int16_t written;
    DECLARE_FNAME("uart_write");

    DBG_I(DPORT_BT_TTY, ("%s: entered, dev %p, len %u\n", FNAME, dev, len));

    if (!len)
    {
        *done = 0;
        return UWE_OK;
    }

    written = uart_send_poll((uint8_t *)buf, len);

    if (written == 0 && len != 0)
    {
        DBG_E(DPORT_BT_TTY, ("%s: Fifo full\n", FNAME));
        return UWE_BUSY;
    }
    if (written < 0)
        return UWE_IO;

    *done = (uint32_t)written;

    return UWE_OK;
}

static port_uart_creator_ops_t uart_creator_ops = {
    uart_write
};

result_t uart_own(j_device_h dev, owner_cbs_h owner_cbs,
    creator_methods_h *creator_methods)
{
    uart_softc_t *sc = (uart_softc_t *)j_device_get_creator_ctx(dev);
    DECLARE_FNAME("ldisc_own");

    DBG_I(DPORT_BT_TTY, ("%s: entered, dev %p\n", FNAME, dev));

    if (!creator_methods)
        return UWE_INVAL;

    sc->uart_owner_ops = (port_uart_owner_ops_t *)owner_cbs;
    *creator_methods = (creator_methods_h)&uart_creator_ops;

    return UWE_OK;
}

void uart_disown(j_device_h dev)
{
    uart_softc_t *sc = (uart_softc_t *)j_device_get_creator_ctx(dev);
    DECLARE_FNAME("uart_disown");

    DBG_I(DPORT_BT_TTY, ("%s: entered, dev %p\n", FNAME, dev));

    sc->uart_owner_ops = NULL;
}

static uart_softc_t *sc_stat = NULL;

void uart_uninit(uart_softc_t *sc)
{
    DECLARE_FNAME("uart_uninit");

    DBG_E(DPORT_BT_TTY, ("%s: Entered\n", FNAME));
    if (!sc)
        return;

    if(sc->uart_dev)
    {
        CORE_SAFE_ENTER;
        j_device_delete(sc->uart_dev);
        CORE_SAFE_LEAVE;
    }

    jfree(sc);
    sc = NULL;
    
    sc_stat = NULL;
}

result_t uart_init_and_attach()
{
    result_t rc;

    DECLARE_FNAME("uart_init_and_attach");

    DBG_V(DPORT_BT_TTY, ("%s: Entry\n", FNAME));

    sc_stat = jmalloc(sizeof(uart_softc_t), M_ZERO);

    CORE_SAFE_ENTER;
    sc_stat->uart_dev = j_device_add(J_DEVICE_TYPE_TTY, NULL, NULL, uart_own,
        uart_disown);
    if (!sc_stat->uart_dev)
    {
        DBG_E(DPORT_BT_TTY, ("%s: Failed to add device\n"));
        rc = UWE_NOMEM;
        goto Exit;
    }

    j_device_set_creator_ctx(sc_stat->uart_dev, sc_stat);
    rc = j_device_probe_and_attach(sc_stat->uart_dev);
    if (rc)
    {
        DBG_E(DPORT_BT_TTY, ("%s: probe and attach failed, %s\n", FNAME,
            uwe_str(rc)));
    }

Exit:
    CORE_SAFE_LEAVE;
    if (rc)
        uart_uninit(sc_stat);

    DBG_IF(rc)
        DBG_E(DPORT_BT_TTY, ("%s: done %d\n", FNAME, rc));

    return rc;
}

void juart_receive(uint8_t *buf, uint16_t size)
{
    DECLARE_FNAME("juart_recieve");

    jsafe_enter();

    if (!sc_stat || !j_device_is_owned(sc_stat->uart_dev))
    {
        DBG_E(DPORT_BT_TTY, ("%s: %p has no owner size %d\n", FNAME, sc_stat, 
            size));
        goto Exit;
    }

#ifdef HCI_DEBUG
	/* { */
    /*     int i; */
   	/*     os_printf("juart_receive: "); */
    /*     for (i=0; i<size; i++) */
    /*         os_printf("0x%02x ", buf[i]); */
    /*     os_printf("\r\n"); */
	/* } */
#endif

    sc_stat->uart_owner_ops->input(sc_stat->uart_dev, buf, size);

Exit:
    jsafe_leave();
}

