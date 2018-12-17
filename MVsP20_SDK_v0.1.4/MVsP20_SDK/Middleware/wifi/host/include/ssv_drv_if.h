/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _SSV_DRV_IF_H_
#define _SSV_DRV_IF_H_

#include <ssv_types.h>
#include "ssv_drv_config.h"
#include "ssv_drv.h"

typedef enum
{
	SSV6XXX_DRV_SIM = 1,
	SSV6XXX_DRV_UART,
	SSV6XXX_DRV_USB,
	SSV6XXX_DRV_SDIO,
	SSV6XXX_DRV_SPI,
	SSV6XXX_DRV_NUM
} SSV6XXX_DRV_TYPE;

struct ssv6xxx_drv_ops
{
	char	name[32];
	// SSV6XXX_DRV_TYPE		type;
	bool	(* open)(void);
	bool	(* close)(void);
	bool	(* init)(void);
	// return
	//  < 0 : fail
	// >= 0 : # of bytes recieve
	s32 	(* recv)(u8 *dat, size_t len);
    // return
	//  < 0 : fail
	// >= 0 : # of bytes send
	s32 	(* send)(void *dat, size_t len);
	bool	(* get_name)(char name[32]);
	bool	(* ioctl)(u32 ctl_code,
					  void *in_buf, size_t in_size,
					  void *out_buf, size_t out_size,
					  size_t *bytes_ret);
	u32		(*handle)(void);
    bool	(*ack_int)(void);
    bool    (*write_sram)(u32 addr, u8 *data, u32 size);
    bool    (*read_sram)(u32 addr, u8 *data, u32 size);
    bool    (*write_reg)(u32 addr, u32 data);
    u32     (*read_reg)(u32 addr);
    bool    (*write_byte)(u8 func,u32 addr, u8 data);
    u32    (*read_byte)(u8 func,u32 addr);
	u32    (*write_fw_to_sram)(u8 *bin, u32 bin_len, u32 block_size);
    s32    (*start)(void);
    s32    (*stop)(void);
	void    (*irq_enable)(void);
    void    (*irq_disable)(void);
    bool    (*wakeup_wifi)(bool sw);
    bool	(* detect_card)(void);
};
#if (SDRV_INCLUDE_SIM)
	extern struct ssv6xxx_drv_ops	g_drv_sim;
#endif

#if (SDRV_INCLUDE_UART)
	extern struct ssv6xxx_drv_ops	g_drv_uart;
#endif

#if (SDRV_INCLUDE_USB)
	extern struct ssv6xxx_drv_ops	g_drv_usb;
#endif

#if (SDRV_INCLUDE_SDIO)
	extern struct ssv6xxx_drv_ops	g_drv_sdio;
#endif

#if (SDRV_INCLUDE_SPI)
	extern struct ssv6xxx_drv_ops	g_drv_spi;
#endif

#endif	/* _HDRV_SIM_IF_H_ */
