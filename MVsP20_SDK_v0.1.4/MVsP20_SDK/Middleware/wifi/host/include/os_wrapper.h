/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _OS_WRAPPER_H
#define _OS_WRAPPER_H

//#if defined(WIN32)
//#include <stdio.h>
//#include <stdlib.h>
//#include <memory.h>
//#endif

#include <ssv_types.h>
#include <rtos.h>
#include <pbuf.h>
#include <ssv_lib.h>
#include <ssv_frame.h>

#define OS_TIMER_DESP_NUM 15




#define FREEIF( _p )  \
	if( _p )              \
{                     \
	OS_MemFree( _p );   \
	_p = NULL;    \
}


#define os_ms2tick OS_MS2TICK

//portTICK_RATE_MS    ==>   ?ms/1ticks
#define os_tick2ms(_tick) ((_tick)*TICK_RATE_MS)
#define os_getSysTime (os_tick2ms(os_sys_jiffies())/MSEC_PER_SEC)




#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC (1000L)
#endif

// static __inline unsigned int jiffies_to_msecs(const unsigned long j)
// {
// #if HZ <= MSEC_PER_SEC && !(MSEC_PER_SEC % HZ)
// 	return (MSEC_PER_SEC / HZ) * j;
// #elif HZ > MSEC_PER_SEC && !(HZ % MSEC_PER_SEC)
// 	return (j + (HZ / MSEC_PER_SEC) - 1)/(HZ / MSEC_PER_SEC);
// #else
// 	return (j * MSEC_PER_SEC) / HZ;
// #endif
// }







typedef u32 os_time_t;
//Current time
#define os_sys_jiffies OS_GetSysTick




#if defined(WIN32)

#define typecheck

#else
/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})
#endif




/*
 *	These inlines deal with timer wrapping correctly. You are 
 *	strongly encouraged to use them
 *	1. Because people otherwise forget
 *	2. Because if the timer wrap changes in future you won't have to
 *	   alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#if 1
#define time_after(a,b)		\
	((long)(b) - (long)(a) < 0)
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	((long)(a) - (long)(b) >= 0)

#else
#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(a) - (long)(b) >= 0))

#endif

#define time_before_eq(a,b)	time_after_eq(b,a)

/*
 * Calculate whether a is in the range of [b, c].
 */
#define time_in_range(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before_eq(a,c))

/*
 * Calculate whether a is in the range of [b, c).
 */
#define time_in_range_open(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before(a,c))
	 
/*
 * These four macros compare jiffies and 'a' for convenience.
 */

/* time_is_before_jiffies(a) return true if a is before jiffies */
#define time_is_before_jiffies(a) time_after(os_sys_jiffies(), a)

/* time_is_after_jiffies(a) return true if a is after jiffies */
#define time_is_after_jiffies(a) time_before(os_sys_jiffies(), a)

/* time_is_before_eq_jiffies(a) return true if a is before or equal to jiffies*/
#define time_is_before_eq_jiffies(a) time_after_eq(os_sys_jiffies(), a)

/* time_is_after_eq_jiffies(a) return true if a is after or equal to jiffies*/
#define time_is_after_eq_jiffies(a) time_before_eq(os_sys_jiffies(), a)

//-----------------------------------------------------------------------------------------------------------------------------------------------
//																Network related
//-----------------------------------------------------------------------------------------------------------------------------------------------


void os_net_wake_queue(void);
void os_net_stop_queue(void);

//-----------------------------------------------------------------------------------------------------------------------------------------------
//																Mutex related
//-----------------------------------------------------------------------------------------------------------------------------------------------




/**
 * enum ssv6xxx_tx_type - The type of tx frame request
 *
 * @ SSV6XXX_TX_REQ0
 * @ SSV6XXX_TX_REQ1
 * @ SSV6XXX_TX_REQ2
 */
enum ssv6xxx_tx_type {
    SSV6XXX_TX_REQ0,
    SSV6XXX_TX_REQ1,
    SSV6XXX_TX_REQ2,
    SSV6XXX_TX_MAX,
};


//-----------------------------------------------------------------------------------------------------------------------------------------------
//																msg related
//-----------------------------------------------------------------------------------------------------------------------------------------------


//Use to send command to host command engine
void os_msg_free(void *msg);
void* os_msg_alloc(void);
s32 os_msg_send(void* msg, void *pBuffer);
s32 os_msg_send_tx_drv(void* msg, void *pBuffer);


#endif	/* _OS_WRAPPER_H */
