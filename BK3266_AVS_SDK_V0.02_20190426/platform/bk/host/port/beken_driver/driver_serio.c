#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "tra_hcit.h"

extern void uart_send (unsigned char *buff, unsigned int len);
uint16 uart_send_poll( uint8 * buff, int len )
{
    int i;

#ifdef HCI_DEBUG
    /* os_printf("uart_send_poll: "); */
	/* for( i = 0; i < len; i++) */
    /*     os_printf("0x%02x ", buff[i]); */
    /* os_printf("\r\n"); */
#endif
    if (BT_HOST_MODE == NONE_CONTROLLER)
        uart_send(buff, len);
    else if(BT_HOST_MODE == JUNGO_HOST){
        for( i = 0; i < len; i++)
            host_write_virtual_uart(buff[i]);
	}
    return len;
}

// EOF
