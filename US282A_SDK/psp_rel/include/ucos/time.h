/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：时间头文件
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __OS_TIME_H__
#define __OS_TIME_H__

#define HZ 100

#ifndef _ASSEMBLER_

#include <ucos/types.h>
#include <ucos/init.h>

#define	TIME_DIFF(cur, prev, diff)						\
	do {								\
		(diff)->tv_sec = (cur)->tv_sec - (prev)->tv_sec;		\
		(diff)->tv_usec = (cur)->tv_usec - (prev)->tv_usec;	\
		if ((diff)->tv_usec < 0) {				\
			(diff)->tv_sec--;				\
			(diff)->tv_usec += 1000000;			\
		}							\
	} while (0)

/*! time, second */
typedef long _time_t;

/*! time, micro second */
typedef long _suseconds_t;

/*!
 * \brief
 *      timeval, 系统时间，包括m和us
 */
struct timeval {
	/*! seconds */
	_time_t tv_sec;
	/*! microseconds */
	_suseconds_t tv_usec;
};

/* Parameters used to convert the timespec values */
#define MSEC_PER_SEC (1000L)
#define USEC_PER_SEC (1000000L)
//#define NSEC_PER_SEC (1000000000L)
//#define NSEC_PER_USEC (1000L)

extern void hw_timer_setup();

typedef struct {
	uint16 year;
	uint8 month;
	uint8 day;
} date_t;

typedef struct {
	uint8 hour;
	uint8 minute;
	uint8 second;
} time_t;


#endif /*_ASSEMBLER_*/
#endif /*__OS_TIME_H__*/
