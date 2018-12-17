/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _SSV_DRV_H_
#define _SSV_DRV_H_

#if (defined _WIN32)
	#include "types.h"
	#include "ssv_drv_config.h"
#else //(defined __SSV_UNIX_SIM__ || defined __linux__)
    #include <ssv_types.h>
#endif

#include <log.h>
#include <host_config.h>

#include <ssv_hal.h>



//#ifndef MAX_BUF
//#define MAX_BUF			(8*1024)
//#endif

#define	_SDRV_DEBUG		0
#define _SDRV_TRACE		0
#define _SDRV_WARN		1

#if (_SDRV_DEBUG)
#define SDRV_DEBUG					LOG_DEBUG
#else
#define SDRV_DEBUG(fmt, ...)		((void)0)
#endif

#if (_SDRV_TRACE)
#define SDRV_TRACE					LOG_TRACE
#else
#define SDRV_TRACE(fmt, ...)		((void)0)
#endif

#if (_SDRV_WARN)
#define SDRV_WARN			         LOG_WARN
#else
#define SDRV_WARN(fmt, ...)			((void)0)
#endif

#define SDRV_INFO					LOG_INFO
#define SDRV_ERROR		            LOG_ERROR
#define SDRV_FAIL	                LOG_FAIL
#define SDRV_FAIL_RET(r, fmt, ...)	{ LOG_FAIL(fmt, ##__VA_ARGS__); return r; }
#define SDRV_FATAL					LOG_FATAL

#define PRI_Q_NUM 5

bool	ssv6xxx_drv_module_init(void);
void	ssv6xxx_drv_module_release(void);

/* ---------------------------------------------------------------
Note :
	if success!, this func will automatically do these for you
		1. automatically close() the previous ssv_drv
		2. automatically open()  the current  ssv_drv
		3. automatically init()	 the current  ssv_drv

	if Fail!, this func will do nothing.
--------------------------------------------------------------- */
bool	ssv6xxx_drv_select(char name[32]);
void	ssv6xxx_drv_list(void);


// Caution! : the function prototype must be the same with whose in 'struct ssv6xxx_drv_ops'!!!
bool	ssv6xxx_drv_open(void);
bool	ssv6xxx_drv_close(void);
bool	ssv6xxx_drv_init(void);
s32 	ssv6xxx_drv_recv(u8 *dat, size_t len);
s32 	ssv6xxx_drv_send(void *dat, size_t len);
bool	ssv6xxx_drv_get_name(char name[32]);
bool	ssv6xxx_drv_ioctl(u32 ctl_code,
					  void *in_buf, size_t in_size,
					  void *out_buf, size_t out_size,
					  size_t *bytes_ret);

u32	ssv6xxx_drv_read_reg(u32 addr);
bool	ssv6xxx_drv_write_reg(u32 addr, u32 data);

bool	ssv6xxx_drv_write_byte(u32 addr, u32 data);
bool	ssv6xxx_drv_read_byte(u32 addr);

bool    ssv6xxx_drv_write_sram (u32 addr, u8 *data, u32 size);
bool    ssv6xxx_drv_read_sram (u32 addr, u8 *data, u32 size);
bool    ssv6xxx_drv_start (void);
bool    ssv6xxx_drv_stop (void);
bool    ssv6xxx_drv_irq_enable(bool is_isr);
bool    ssv6xxx_drv_irq_disable(bool is_isr);

s32 ssv6xxx_drv_send_sim(void *dat, size_t len);
u32 ssv6xxx_drv_get_handle(void);
bool ssv6xxx_drv_ack_int(void);
u32 ssv6xxx_drv_get_TRX_time_stamp(void);
bool ssv6xxx_drv_wakeup_wifi(bool sw);
bool ssv6xxx_drv_tx_resource_enough(u32 frame_len);

#endif /* _SSV_DRV_H_ */

