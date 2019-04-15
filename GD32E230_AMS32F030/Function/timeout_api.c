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
#include "timeout_api.h"
#include "app_config.h"
#include <stdint.h>

volatile uint32_t CORET_Systick_Count;			//CORET Systick count

// 获得当前Timer实际运行时间，单位ms
uint16_t GetPassTime(SW_TIMER* TimerHandle)
{
	uint64_t LastSystickVal;

	if(CORET_Systick_Count < TimerHandle->LastTickets)
	{
		LastSystickVal = (MaxSystickCount + CORET_Systick_Count);
	}
	else
	{
		LastSystickVal = CORET_Systick_Count;
	}

	return (LastSystickVal - TimerHandle->LastTickets);
}

// 应用模式的循环中，调用该函数，轮询Timer（定时调用CallBack）
void CheckTimer(SW_TIMER* TimerHandle)
{
	uint64_t LastSystickVal;

	if(!TimerHandle->IsRunning)
	{
		return;
	}

	if(CORET_Systick_Count < TimerHandle->LastTickets)
	{
		LastSystickVal = (MaxSystickCount + CORET_Systick_Count);
	}
	else
	{
		LastSystickVal = CORET_Systick_Count;
	}

	if(LastSystickVal >= (uint64_t)(TimerHandle->LastTickets + TimerHandle->TimerPeriod))
	{
		TimerHandle->Callback();
		TimerHandle->LastTickets = CORET_Systick_Count;
	}
}

// 初始化Timer
FlagStatus InitTimer(SW_TIMER* TimerHandle,uint32_t TimerPeriod, /*定时间隔ms*/TIMER_CALLBACK CallbackFunc/*定时回调函数*/)
{
	TimerHandle->TimerPeriod = TimerPeriod;
	TimerHandle->IsRunning = RESET;
	TimerHandle->Callback = CallbackFunc;
	return SET;
}

// 启动Timer
FlagStatus StartTimer(SW_TIMER* TimerHandle)
{
	TimerHandle->LastTickets = CORET_Systick_Count;
	TimerHandle->IsRunning = SET;
	return SET;
}

// 更改Timer定时间隔
FlagStatus ChangeTimerPeriod(SW_TIMER* TimerHandle, uint32_t TimerPeriod/*定时间隔*/)
{
	TimerHandle->IsRunning = RESET;
	TimerHandle->TimerPeriod = TimerPeriod;
	TimerHandle->LastTickets = CORET_Systick_Count;
	TimerHandle->IsRunning = SET;
	return SET;
}

// Get time of some softtimer from setting to now.
uint16_t PastTimeGet(TIMER* timer)
{
	uint64_t LastSystickVal;

	if(CORET_Systick_Count < timer->TickValCache)
	{
		LastSystickVal = (MaxSystickCount + CORET_Systick_Count);
	}
	else
	{
		LastSystickVal = CORET_Systick_Count;
	}

	return (LastSystickVal - timer->TickValCache);
}

// Set time out value of softtimer.
void TimeOutSet(TIMER* timer, uint32_t timeout)
{
	timer->TimeOutVal = timeout;
    timer->TickValCache = CORET_Systick_Count;
}

// Check whether time out.
FlagStatus IsTimeOut(TIMER* timer)
{
	uint64_t LastSystickVal;

	if(CORET_Systick_Count < timer->TickValCache)
	{
		LastSystickVal = (MaxSystickCount + CORET_Systick_Count);
	}
	else
	{
		LastSystickVal = CORET_Systick_Count;
	}

	if(LastSystickVal >= (timer->TickValCache +timer->TimeOutVal))
	{
		timer->TickValCache = CORET_Systick_Count;
		return SET;
	}

	return RESET;
}
/*****************************************************************************
 函 数 名  : WaitMs
 功能描述  : CPU空等一段时间，单位Ms
 输入参数  : uint32_t Time  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月17日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void WaitMs(uint32_t Time)
{
    Time = (Time+CORET_Systick_Count)%MaxSystickCount;
    while(CORET_Systick_Count != Time);    
}

