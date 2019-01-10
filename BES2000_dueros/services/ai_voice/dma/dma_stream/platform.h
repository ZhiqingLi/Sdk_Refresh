#ifdef __BES__
#include<plat_types.h>
#include<cmsis_os.h>
#include<hal_trace.h>
#define DUEROS_TID     osThreadId
#define DUEROS_MDELAY  osdelay

#define DUEROS_THREAD_DEF osThreadDef
#define DUEROS_CREAT_THREAD  osThreadCreate
#define DUEROS_OS_THREAD   osThread
//#define DUEROS_PRINT(str,...) TRACE(str,##__VA_ARGS__)
#define DUEROS_PRINT //TRACE

#define AF_FADE_OUT_SIGNAL_ID  (1<<15)

#define DUEROS_WAIT(x)  osSignalWait((1 << AF_FADE_OUT_SIGNAL_ID), osWaitForever)
#define DUEROS_WAKEUP(tid) osSignalSet(tid, AF_FADE_OUT_SIGNAL_ID)   

#define __LE_PLATFORM__

#ifdef  __LE_PLATFORM__

#define DUEROS_StoreBE8(buff,num) ( ((buff)[0] = (U8) ((num)) ) )

#define DUEROS_StoreBE16(buff,num) ( ((buff)[0] = (U8) ((num)>>8)),    \
            ((buff)[1] = (U8) (num)) )

#define DUEROS_StoreBE32(buff,num) ( ((buff)[0] = (U8) ((num)>>24)),  \
                              ((buff)[1] = (U8) ((num)>>16)),  \
                              ((buff)[2] = (U8) ((num)>>8)),   \
                              ((buff)[3] = (U8) (num)) )

#else

#define DUEROS_StoreBE8(buff,num) ( *((typeof(num)*)(buff)) = (num) )

#define DUEROS_StoreBE16(buff,num) ( *((typeof(num)*)(buff)) = (num) )

#define DUEROS_StoreBE32(buff,num) ( *((typeof(num)*)(buff)) = (num) )

#endif


#define DUEROS_MUTEX_ID osMutexId
#define DUEROS_MUTEX_CREATE osMutexCreate
#define DUEROS_MUTEX_DEF  osMutexDef
#define DUEROS_MUTEX osMutex
#define DUEROS_MUTEX_P(mutex_id) osMutexWait(mutex_id,osWaitForever)
#define DUEROS_MUTEX_V(mutex_id) osMutexRelease(mutex_id)

void *dueros_dma_heap_malloc(size_t size);
void dueros_dma_heap_free(void *ptr);

#define DUEROS_MALLOC(size)  dueros_dma_heap_malloc(size)
#define DUEROS_FREE(ptr) dueros_dma_heap_free(ptr)

#endif 
