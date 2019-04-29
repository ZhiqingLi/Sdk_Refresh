#include "app_cqueue_buffer.h"


/**
 ****************************************************************************************
 * @brief       function used to Initialize the data queue
 *
 * @param[in]   data_queue      (Data queue header)
 *              data_queue_buf  (Array length)
 *
 * @return      void
 ****************************************************************************************
 */
void app_queue_buffer_init(CQueue *data_queue, uint8_t *data_queue_buf, uint32_t data_len)
{
	InitCQueue(data_queue, data_len, data_queue_buf);
	memset(data_queue_buf, 0x00, data_len);
}

/**
 ****************************************************************************************
 * @brief       function used to Invert the array
 *
 * @param[in]   data_queue      (Data queue header)
 *
 * @return      The length of the data queue
 ****************************************************************************************
 */
int app_queue_buffer_length(CQueue *data_queue)
{
	int len;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    
	len = LengthOfCQueue(data_queue);
    
    VICMR_restore_interrupts(interrupts_info, mask);

	return len;
}

/**
 ****************************************************************************************
 * @brief       function used to Add data to the queue.
 *
 * @param[in]   data_queue      (Data queue header)
 *              buff            (The array to be added)
 *              len             (Array length)
 *
 * @return      0(successful) or -1(fail)
 ****************************************************************************************
 */
int app_queue_buffer_set(CQueue *data_queue, uint8_t *buff, uint16_t len)
{
	int status;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    
	status = EnCQueue(data_queue, buff, len);

    VICMR_restore_interrupts(interrupts_info, mask);
    
	return status;
}

/**
 ****************************************************************************************
 * @brief       function used to Gets the data in the queue.
 *
 * @param[in]   buff      (Gets the buffer of the data.)
 *              len       (Array length)
 *
 * @return      0(successful) or -1(fail)
 ****************************************************************************************
 */
int app_queue_buffer_get(CQueue *data_queue, uint8_t *buff, uint16_t len)
{
	uint8_t *e1 = NULL, *e2 = NULL;
	unsigned int len1 = 0, len2 = 0;
	int status;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
	status = PeekCQueue(data_queue, len, &e1, &len1, &e2, &len2);
	if (len==(len1+len2)){
		memcpy(buff,e1,len1);	
		memcpy(buff+len1,e2,len2);
		DeCQueue(data_queue, 0, len);
        DeCQueue(data_queue, 0, len2);
	}else{
		memset(buff, 0x00, len);
		status = -1;
	}
    VICMR_restore_interrupts(interrupts_info, mask);
    
	return status;
}


/**
 ****************************************************************************************
 * @brief       function used to Peek at the data in the buffer.
 *
 * @param[in]   data_queue      (Data queue header)
 *              buff            (Gets the buffer of the data.)
 *              len             (Array length)
 *
 * @return      The length of the data queue
 ****************************************************************************************
 */
int app_queue_buffer_peek(CQueue *data_queue, uint8_t *buff, uint16_t len)
{
	uint8_t *e1 = NULL, *e2 = NULL;
	unsigned int len1 = 0, len2 = 0;
	int status;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    status = PeekCQueue(data_queue, len, &e1, &len1, &e2, &len2);
	if (len==(len1+len2)){
		memcpy(buff,e1,len1);	
		memcpy(buff+len1,e2,len2);
	}else{
		memset(buff, 0x00, len);
		status = -1;
	}
    VICMR_restore_interrupts(interrupts_info, mask);

	return status;
}

/**
 ****************************************************************************************
 * @brief       function used to Delete the data in the queue;
 *
 * @param[in]   data_queue      (Data queue header)
 *              len             (Array length)
 *
 * @return      0(successful) or -1(fail)
 ****************************************************************************************
 */
int app_queue_buffer_delete(CQueue *data_queue, uint16_t len)
{
	int status;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    
	status = DeCQueue(data_queue, 0, len);
    
    VICMR_restore_interrupts(interrupts_info, mask);

	return status;
}

/**
 ****************************************************************************************
 * @brief       function used to Clear queue data
 *
 * @param[in]   data_queue      (Data queue header)
 *
 * @return      0(successful) or -1(fail)
 ****************************************************************************************
 */
void app_queue_buffer_reset(CQueue *data_queue)
{
	int status;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    
	ResetCqueue(data_queue);
    
    VICMR_restore_interrupts(interrupts_info, mask);

	return ;
}


