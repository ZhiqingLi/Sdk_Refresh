/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#include <rtos.h>




volatile u8 gOsFromISR;



OS_APIs s32  OS_Init( void )
{
    gOsFromISR = 0;
    return OS_SUCCESS;
}
OS_APIs unsigned long OS_Random(void)
{
	return xTaskGetTickCount()%65536+54*18;
}


OS_APIs void OS_Terminate( void )
{
    vTaskEndScheduler();
}

OS_APIs u32 OS_EnterCritical(void)
{
    vPortEnterCritical();
    return 0;
}

OS_APIs void OS_ExitCritical(u32 val)
{
   vPortExitCritical();
   return;
}

/* Task: */
OS_APIs s32 OS_TaskCreate( OsTask task, const char *name, u32 stackSize, void *param, u32 pri, OsTaskHandle *taskHandle )
{

	if(strncmp(name,"ssv_tmr_task",sizeof("ssv_tmr_task"-1)) == 0)
	{
		stackSize = stackSize * 2;//增加栈大小
	}


    return (xTaskCreate(
            task,                   /* The task to create */
            (const signed char * const)name,     /* Task name */
            stackSize,              /* Stack Size (in WORD (4bytes)) */
            param,                  /* Parameter for Task */
            pri,                    /* Priority: 0 (low) */
            taskHandle) == pdPASS) ? OS_SUCCESS : OS_FAILED;           /* Task Handle */
}


OS_APIs void OS_TaskDelete(OsTaskHandle taskHandle)
{
    vTaskDelete(taskHandle);
}



OS_APIs void OS_StartScheduler( void )
{
    vTaskStartScheduler();  
}

OS_APIs u32 OS_GetSysTick(void)
{  
	
    return xTaskGetTickCount();
}


/* Mutex APIs: */
OS_APIs s32 OS_MutexInit( OsMutex *mutex )
{
    *mutex = xSemaphoreCreateMutex();
    if ( NULL == *mutex )
        return OS_FAILED;
    else return OS_SUCCESS;
}


OS_APIs void OS_MutexDelete( OsMutex mutex )
{
    vSemaphoreDelete(mutex);
}


OS_APIs void OS_MutexLock( OsMutex mutex )
{
    xSemaphoreTake( mutex, portMAX_DELAY);
}



OS_APIs void OS_MutexUnLock( OsMutex mutex )
{
    xSemaphoreGive( mutex );
}


OS_APIs void OS_MsDelay(u32 ms)
{
    u32  ticks = ms / portTICK_RATE_MS;
    if (ticks == 0)
        ticks = 1;
    vTaskDelay(ticks);
}

OS_APIs void OS_TickDelay(u32 ticks)
{
        vTaskDelay(ticks);
}



/* Message Queue: */
OS_APIs s32 OS_MsgQCreate( OsMsgQ *MsgQ, u32 QLen )
{
    *MsgQ = xQueueCreate( QLen, sizeof( OsMsgQEntry ) );
    if ( NULL == *MsgQ )
        return OS_FAILED;
    return OS_SUCCESS;
}


OS_APIs s32 OS_MsgQEnqueue( OsMsgQ MsgQ, OsMsgQEntry *MsgItem, bool fromISR )
{
    s32 err, val;
    if (fromISR == false)
        err = xQueueSendToBack( MsgQ, (void *)MsgItem, portMAX_DELAY );
    else 
        err = xQueueSendToBackFromISR( MsgQ, (void *)MsgItem, &val );
    return ( pdPASS!=err )? OS_FAILED: OS_SUCCESS;
}

OS_APIs s32 OS_MsgQEnqueueTry( OsMsgQ MsgQ, OsMsgQEntry *MsgItem, bool fromISR )
{
    s32 err, val;
    if (fromISR == false)
        err = xQueueSendToBack( MsgQ, (void *)MsgItem, 0UL);
    else 
        err = xQueueSendToBackFromISR( MsgQ, (void *)MsgItem, &val );
    return ( pdPASS!=err )? OS_FAILED: OS_SUCCESS;
}


OS_APIs s32 OS_MsgQDequeue( OsMsgQ MsgQ, OsMsgQEntry *MsgItem, u32 timeOut,bool fromISR )
{
    s32 err, val;
	u32 BlockTick;
	
	//BlockTick = timeOut;
	BlockTick= (timeOut == 0)?portMAX_DELAY : timeOut;
	
    if (fromISR == false)
        err = xQueueReceive( MsgQ, (void *)MsgItem, BlockTick );
    else
        err = xQueueReceiveFromISR( MsgQ, (void *)MsgItem, &val );
    return ( pdPASS!=err )? OS_FAILED: OS_SUCCESS;
}


OS_APIs s32 OS_MsgQWaitingSize( OsMsgQ MsgQ )
{
    return ( uxQueueMessagesWaiting( MsgQ ) );
}



/* Timer: */
OS_APIs s32 OS_TimerCreate( OsTimer *timer, u32 ms, u8 autoReload, void *args, OsTimerHandler timHandler )
{
#if ( configUSE_TIMERS == 1 )
    ms = ( 0 == ms )? 1: ms;
    *timer = xTimerCreate( NULL, OS_MS2TICK(ms), autoReload, args, timHandler);
    if ( NULL == *timer )
        return OS_FAILED;
#endif

    return OS_SUCCESS;
}

OS_APIs s32 OS_TimerSet( OsTimer timer, u32 ms, u8 autoReload, void *args )
{
#if ( configUSE_TIMERS == 1 )
    if ( pdFAIL == xTimerChangeSetting( timer, OS_MS2TICK(ms), autoReload, args) )
        return OS_FAILED;
#endif    
    return OS_SUCCESS;    
}

OS_APIs s32 OS_TimerStart( OsTimer timer )
{
#if ( configUSE_TIMERS == 1 )
    return xTimerStart( timer, 0 );
#endif
    return OS_SUCCESS;
}

OS_APIs s32 OS_TimerStop( OsTimer timer )
{
#if ( configUSE_TIMERS == 1 )
    return xTimerStop( timer, 0 );
#endif
    return OS_SUCCESS;
}

OS_APIs void *OS_TimerGetData( OsTimer timer )
{
#if ( configUSE_TIMERS == 1 )
    return pvTimerGetTimerID(timer);
#endif
    return OS_SUCCESS;
}

OS_APIs s32 OS_SemInit( OsSemaphore* Sem , u16 maxcnt , u16 cnt)
{
//	 u8 xReturn = OS_FAILED;
    if ((cnt > maxcnt) || (maxcnt == 0))
        return OS_FAILED;
    
	(*Sem) = xSemaphoreCreateCounting(maxcnt, cnt);
	
	if ( NULL != *Sem )    
		return OS_SUCCESS;        
	else
		return OS_FAILED;
}

OS_APIs bool OS_SemWait( OsSemaphore Sem , u16 timeout)
{
    if(timeout)
        return ((xSemaphoreTake( Sem, timeout)== pdPASS)?OS_SUCCESS:OS_FAILED);
        
    else
        return ((xSemaphoreTake( Sem, portMAX_DELAY)== pdPASS)?OS_SUCCESS:OS_FAILED);

}

OS_APIs u8 OS_SemSignal( OsSemaphore Sem)
{
	return ((xSemaphoreGive( Sem ) == pdPASS) ? OS_SUCCESS:OS_FAILED);
	
}

OS_APIs u32 OS_SemCntQuery( OsSemaphore Sem)
{
    return 0;
}

OS_APIs u8 OS_SemSignal_FromISR( OsSemaphore Sem)
{
	return ((xSemaphoreGiveFromISR( Sem , pdFALSE) == pdPASS) ? OS_SUCCESS:OS_FAILED);
}

OS_APIs void OS_SemDelete(OsSemaphore Sem)
{
	vSemaphoreDelete( Sem );
}

OS_APIs s32 OS_MsgQDelete( OsMsgQ MsgQ)
{
    vQueueDelete(MsgQ);
    return OS_SUCCESS;
}

OS_APIs s32 OS_SysProfiling(void *pTextBuf)
{
#if configGENERATE_RUN_TIME_STATS
#ifndef OS_NO_SUPPORT_PROFILING
    if(pTextBuf == NULL)
        return OS_FAILED;

    vTaskGetRunTimeStats(pTextBuf);
#endif
#endif
    return OS_SUCCESS;
}


#define MEM_LIMIT_CNT_MAX  8  //大概1.6K一个包
s32 memLimitCnt = 0;

void *OS_pMalloc( size_t xWantedSize )
{
	void * ptr = NULL;

	if(memLimitCnt < MEM_LIMIT_CNT_MAX)
	{
		ptr = pvPortMalloc(xWantedSize);
		if(ptr != NULL)
		{
			taskENTER_CRITICAL();
			memLimitCnt++;
			taskEXIT_CRITICAL();
		}
	}
	else
	{
//		printf("ov\n");
	}

	return ptr;
}

void OS_vFree( void *pv )
{

	vPortFree(pv);
	if(pv != NULL)
	{
		taskENTER_CRITICAL();
		memLimitCnt--;
		if(memLimitCnt < 0) memLimitCnt = 0;
		taskEXIT_CRITICAL();
	}
}
