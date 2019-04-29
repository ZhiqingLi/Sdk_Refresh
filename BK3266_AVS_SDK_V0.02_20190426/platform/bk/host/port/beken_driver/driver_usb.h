#ifndef _DRIVER_USB_H
#define _DRIVER_USB_H

#define USB_RET_OK			0
#define USB_RET_ERROR 		1
#define USB_RET_CONNECT 	2
#define USB_RET_DISCONNECT  3
#define USB_RET_READ_OK 	4
#define USB_RET_WRITE_OK 	5

typedef struct __driver_udisk_s
{
    uint32        total_block;
	uint16		  block_size;
	uint16		  InitFlag;
}driver_udisk_t;

void pre_usb_init(void);
void post_usb_init(void);
uint8 udisk_is_attached(void);
uint8 udisk_init(void);
void udisk_uninit(void);
uint32 udisk_get_size(void);
void usb_isr(void);
int udisk_rd_blk_sync(uint32 first_block, uint32 block_num, uint8 *dest);

#endif
