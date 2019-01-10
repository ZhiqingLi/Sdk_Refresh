/***
 * lock cqueue
 * YuLongWang @2015
*/

#include "stdio.h"
#include "cmsis_os.h"
#include "lockcqueue.h"

int lockcqueue_init(struct lockcqueue *q, uint32_t size, uint8_t *buf)
{
    InitCQueue(&q->cqueue, size, buf);
    q->os_mutex_def_name.mutex = q->os_mutex_cb_name;
    q->queue_mutex_id = osMutexCreate(&q->os_mutex_def_name);
}

int lockcqueue_enqueue(struct lockcqueue *q, uint8_t *buf, uint32_t size)
{
    int ret = 0;
	osMutexWait(q->queue_mutex_id, osWaitForever);
    ret = EnCQueue(&q->cqueue, buf, size);
	osMutexRelease(q->queue_mutex_id);

    return ret;
}

int lockcqueue_dequeue(struct lockcqueue *q, uint8_t *buf, uint32_t size)
{
    int ret = 0;
	osMutexWait(q->queue_mutex_id, osWaitForever);
    ret = DeCQueue(&q->cqueue, buf, size);
	osMutexRelease(q->queue_mutex_id);

    return ret;
}
