/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			timeout.h
* maintainer        Halley
* Description:		declare	varable and function here
*
******************************************************************************/
#ifndef __TIMEROUT_H__
#define __TIMEROUT_H__

#include <stdint.h>
#include "app_config.h"

/*
 * define timer check for init task period event
 */
 #define MaxSystickCount   0xffffffff
 
typedef void (*TIMER_CALLBACK)(void);

typedef struct _SW_TIMER_
{
	uint16_t TimerPeriod;  // ms,【注意】最小定时间隔与应用模式主循环的其他功能阻塞时间有关，建议不要低于主循环的最大阻塞时间；
	uint32_t LastTickets;  // 用于timeout判断
	TIMER_CALLBACK Callback;
	FlagStatus  IsRunning;
} SW_TIMER;

// define softtimer structure.
typedef struct	_TIMER
{
	uint16_t	TimeOutVal; 								//time out value
	uint32_t	TickValCache;			  					//soft timer setting value
} TIMER;

// 获得当前Timer实际运行时间，单位ms
uint16_t GetPassTime(SW_TIMER* TimerHandle);

// 应用模式的循环中，调用该函数，轮询Timer（定时调用CallBack）
void CheckTimer(SW_TIMER* TimerHandle);

// 初始化Timer
FlagStatus InitTimer(SW_TIMER* TimerHandle,
	               uint32_t TimerPeriod, /*定时间隔ms*/
	               TIMER_CALLBACK CallbackFunc/*定时回调函数*/);

// 启动Timer
FlagStatus StartTimer(SW_TIMER* TimerHandle);


// 更改Timer定时间隔
FlagStatus ChangeTimerPeriod(SW_TIMER* TimerHandle, uint32_t TimerPeriod/*定时间隔*/);

// Get time of some softtimer from setting to now.
uint16_t PastTimeGet(TIMER* timer);

// Set time out value of softtimer.
void TimeOutSet(TIMER* timer, uint32_t timeout);

// Check whether time out.
FlagStatus IsTimeOut(TIMER* timer);
void WaitMs(uint32_t Time);
#endif

