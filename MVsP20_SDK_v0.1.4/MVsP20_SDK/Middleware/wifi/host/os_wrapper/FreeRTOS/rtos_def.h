/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _RTOS_DEF_H_
#define _RTOS_DEF_H_
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

/*============OS parameter setting===================*/
typedef pdTASK_CODE				    OsTask;
typedef xTaskHandle				    OsTaskHandle;
typedef xSemaphoreHandle			OsMutex;
typedef xSemaphoreHandle			OsSemaphore;
typedef xQueueHandle				OsMsgQ;
typedef xTimerHandle                OsTimer;
typedef tmrTIMER_CALLBACK           OsTimerHandler;

#define OS_TASK_ENTER_CRITICAL()        taskENTER_CRITICAL()
#define OS_TASK_EXIT_CRITICAL()         taskEXIT_CRITICAL()

#define TICK_RATE_MS portTICK_RATE_MS
#define TASK_IDLE_STK_SIZE configMINIMAL_STACK_SIZE

#endif
