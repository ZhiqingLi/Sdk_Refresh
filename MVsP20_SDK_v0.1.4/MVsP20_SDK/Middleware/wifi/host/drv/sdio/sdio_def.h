/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _SDIO_DEF_H_
#define _SDIO_DEF_H_

/* -------------- h/w register ------------------- */
#define BASE_SDIO	0

/* Note :
	For now, the reg of SDIO Host & Card Controller are the same.
If it changes in the future, you should define again.
*/
#define REG_DATA_IO_PORT_0		(BASE_SDIO + 0x00)		// 0
#define REG_DATA_IO_PORT_1	    (BASE_SDIO + 0x01)		// 0
#define REG_DATA_IO_PORT_2		(BASE_SDIO + 0x02)		// 0
#define REG_INT_MASK			(BASE_SDIO + 0x04)		// 0
#define REG_INT_STATUS			(BASE_SDIO + 0x08)		// 0
#define REG_INT_TRIGGER			(BASE_SDIO + 0x09)		// 0
#define REG_Fn1_STATUS			(BASE_SDIO + 0x0c)		// 0
#define REG_CARD_PKT_LEN_0		(BASE_SDIO + 0x10)		// 0
#define REG_CARD_PKT_LEN_1		(BASE_SDIO + 0x11)		// 0
#define REG_CARD_FW_DL_STATUS	(BASE_SDIO + 0x12)		// 0
#define REG_CARD_SELF_TEST		(BASE_SDIO + 0x13)		// 0
#define REG_CARD_RCA_0			(BASE_SDIO + 0x20)		// 0
#define REG_CARD_RCA_1			(BASE_SDIO + 0x21)		// 0
#define REG_SDIO_FIFO_WR_THLD_0	(BASE_SDIO + 0x24)		// 80
#define REG_SDIO_FIFO_WR_THLD_1	(BASE_SDIO + 0x25)		// 0
#define REG_OUTPUT_TIMING_REG	(BASE_SDIO + 0x55)		// 0
#define REG_PMU_WAKEUP			(BASE_SDIO + 0x67)		// 0
#define REG_REG_IO_PORT_0		(BASE_SDIO + 0x70)		// 0
#define REG_REG_IO_PORT_1	    (BASE_SDIO + 0x71)		// 0
#define REG_REG_IO_PORT_2		(BASE_SDIO + 0x72)		// 0
//SDIO TX ALLOCATE FUNCTION
#define REG_SDIO_TX_ALLOC_SIZE	(BASE_SDIO + 0x98)		// 0
#define REG_SDIO_TX_ALLOC_SHIFT	(BASE_SDIO + 0x99)		// 0
#define REG_SDIO_TX_ALLOC_STATE	(BASE_SDIO + 0x9a)		// 0

#define SDIO_TX_ALLOC_SUCCESS	0x01
#define SDIO_TX_NO_ALLOC		0x02
#define SDIO_TX_DULPICATE_ALLOC	0x04

#define SDIO_TX_ALLOC_SIZE_SHIFT	0x07
#define SDIO_TX_ALLOC_ENABLE		0x10

#define REG_CCCR_07H_REG (BASE_SDIO + 0x07)		// 0

#if(RECOVER_MECHANISM == 1)
#define SDIO_REG_INT_MASK		0xfa
#else
#define SDIO_REG_INT_MASK		0xfe
#endif


/* -------------- debug        ------------------- */
#define	_SDIO_DEBUG	    1
#define _SDIO_TRACE		0
#define _SDIO_WARN		1

#define SDIO_DEBUG(fmt, ...)		do { if (_SDIO_DEBUG) LOG_PRINTF("[DEBUG ] " fmt, ##__VA_ARGS__); } while (0)
#define SDIO_TRACE(fmt, ...)		do { if (_SDIO_TRACE) LOG_PRINTF("[TRACE ] " fmt, ##__VA_ARGS__); } while (0)
#define SDIO_WARN(fmt, ...)         do { if (_SDIO_WARN)  LOG_PRINTF("[WARN  ] " fmt, ##__VA_ARGS__); } while (0)
#define SDIO_ERROR(fmt, ...)		{ LOG_PRINTF("[Error!] %s() : ", __FUNCTION__); LOG_PRINTF(fmt, ##__VA_ARGS__); }
#define SDIO_FATAL(fmt, ...)		{ LOG_PRINTF("[Fatal!] %s (#%d) : ", __FILE__, __LINE__); LOG_PRINTF(fmt, ##__VA_ARGS__); LOG_PRINTF("program halt!!!\n"); exit(1); }
#define SDIO_FAIL(fmt, ...)		    { LOG_PRINTF("[Fail! ] %s() : ", __FUNCTION__); LOG_PRINTF(fmt, ##__VA_ARGS__); }
#define SDIO_FAIL_RET(r, fmt, ...)  { LOG_PRINTF("[Fail! ] %s() : ", __FUNCTION__); LOG_PRINTF(fmt, ##__VA_ARGS__); return (r); }
/* -------------- default      ------------------- */
//Option[50MHz 25MHz 12.5MHz 6.25MHz] 
#define SDIO_DEF_CLOCK_RATE			25000		// 25MHz
#define SDIO_DEVICE_NAME			_T("\\\\.\\SSVCabrio")
#define SDIO_DEF_BLOCK_SIZE			0x80		// should be the multiple of 8 bytes 
#if (SDIO_DEF_BLOCK_SIZE % 8)
#error Wrong SDIO_DEF_BLOCK_SIZE value!! Should be the multiple of 8 bytes!!!!!!!!!!!!!!!!!!!!!!
#endif

// block mode threshold
#define SDIO_DEF_BLOCK_MODE_THRD	128
#if (SDIO_DEF_BLOCK_MODE_THRD % 8)
#error Wrong SDIO_DEF_BLOCK_MODE_THRD value!! Should be the multiple of 8 bytes!!!!!!!!!!!!!!!!!!!!!!
#endif

// 0: false, 1: true
#define SDIO_DEF_FORCE_BLOCK_MODE	0

#endif /* _SDIO_DEF_H_ */
