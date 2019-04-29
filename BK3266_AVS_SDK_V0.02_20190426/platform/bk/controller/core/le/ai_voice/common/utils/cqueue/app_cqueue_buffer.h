#ifndef __APP_CQUEUE_BUFFER_H__
#define __APP_CQUEUE_BUFFER_H__

#include "cqueue.h"

void app_queue_buffer_init(CQueue *data_queue, uint8_t *data_queue_buf, uint32_t data_len);

int app_queue_buffer_length(CQueue *data_queue);

int app_queue_buffer_set(CQueue *data_queue, uint8_t *buff, uint16_t len);

int app_queue_buffer_get(CQueue *data_queue, uint8_t *buff, uint16_t len);

int app_queue_buffer_peek(CQueue *data_queue, uint8_t *buff, uint16_t len);

int app_queue_buffer_delete(CQueue *data_queue, uint16_t len);

void app_queue_buffer_reset(CQueue *data_queue);

#endif // __APP_CQUEUE_BUFFER_H__

