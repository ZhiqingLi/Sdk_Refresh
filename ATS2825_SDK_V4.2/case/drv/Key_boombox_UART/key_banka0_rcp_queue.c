/*******************************************************************************
 *                              us211A
 *                            Module: case
 *                 Copyright(c) 2003-2008 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       cailizhen  2013-10-29 11:43     1.0             build this file
 *******************************************************************************/


#include "key_inner.h"

/*! SPP命令队列 */
//#if (SUPPORT_SPP_COMMAND == 1)
void *key_rcp_queue_p;

uint8 key_rcp_queue_readptr;

uint8 key_rcp_queue_writeptr;

uint8 key_rcp_queue_depth_empty;

uint8 key_rcp_queue_depth;

uint8 key_rcp_entity_head;

key_rcp_alloc_t head;

key_rcp_alloc_t tail;


/* 初始化 RCP 队列 */
bool key_inner_rcp_queue_init(uint8 queue_depth, uint8 head_size, void *null2)
{
    //#if (SUPPORT_REMOTE_CONTROL == 1)
    //创建 RCP 队列
    key_rcp_queue_p = NULL;
#if(SYS_BOARD_TYPE >= FOR_IC_EVB)
    key_rcp_queue_p = sys_malloc(sizeof(rmt_ctrl_pkg_t) * queue_depth);
    if(key_rcp_queue_p == NULL)
#else
    sys_malloc(&key_rcp_queue_p, sizeof(rmt_ctrl_pkg_t) * queue_depth);
#endif
        key_rcp_queue_readptr = 0;
    key_rcp_queue_writeptr = 0;
    key_rcp_queue_depth_empty = queue_depth;
    key_rcp_queue_depth = queue_depth;
    key_rcp_entity_head = head_size;
    //#endif

    return TRUE;
}

/* 初始化 RCP 队列堆空间 */
bool key_inner_rcp_queue_init_heap(void *heap_addr, uint32 heap_size, void *null2)
{
    //#if (SUPPORT_REMOTE_CONTROL == 1)

    void *tmp_heap_addr;

    uint32 tmp_heap_size;

    if(heap_addr == NULL) {
        //使用系统默认堆空间
        tmp_heap_addr = NULL;
        tmp_heap_size = heap_size;
#if(SYS_BOARD_TYPE >= FOR_IC_EVB)
        tmp_heap_addr = sys_malloc(tmp_heap_size);
        if(tmp_heap_addr == NULL)
#else
        if(sys_malloc(&tmp_heap_addr, tmp_heap_size) == -1)
#endif
        {
            return FALSE;
        }
    } else {
        //客户自行指定堆空间
        tmp_heap_addr = heap_addr;
        tmp_heap_size = heap_size;
    }

    //初始化内存分配管理数据
    head.size = tail.size = 0;
    head.address = tmp_heap_addr;
    tail.address = tmp_heap_addr + tmp_heap_size;
    head.next = &tail;
    tail.next = NULL;
    //#endif

    return TRUE;
}

/* 清空 RCP 队列 */
bool key_inner_rcp_queue_flush(void *null0, void *null1, void *null2)
{
    //#if (SUPPORT_REMOTE_CONTROL == 1)
    rmt_ctrl_pkg_t tmp_rcp;

    if(key_rcp_queue_depth_empty >= key_rcp_queue_depth) {
        return TRUE;
    }

    sys_os_sched_lock();
    while(1) {
        libc_memcpy(&tmp_rcp, (rmt_ctrl_pkg_t *)key_rcp_queue_p + key_rcp_queue_readptr, sizeof(rmt_ctrl_pkg_t));
        key_inner_rcp_queue_free(&tmp_rcp, 0, 0);
        if(key_rcp_queue_depth != 0) {
            key_rcp_queue_readptr = (key_rcp_queue_readptr + 1) % key_rcp_queue_depth;
        }
        key_rcp_queue_depth_empty++;
        if(key_rcp_queue_depth_empty >= key_rcp_queue_depth) {
            break;
        }
    }
    sys_os_sched_unlock();
    //#endif

    return TRUE;
}

/* 发送一个 RCP 对象到 RCP 队列中，RCP 数据放到堆中 */
bool key_inner_rcp_queue_send(rmt_ctrl_pkg_t *p_rcp, void *null1, void *null2)
{
    // #if (SUPPORT_REMOTE_CONTROL == 1)
    rmt_ctrl_pkg_t *p_cur_rcp;
    void *p_cur_rcp_data;

    if(key_rcp_queue_depth_empty == 0) {
        return FALSE;
    }

    sys_os_sched_lock();
    p_cur_rcp_data = key_inner_rcp_malloc(p_rcp->rcp_len);
    if(p_cur_rcp_data == NULL) {
        //空间申请失败，丢掉该消息
        sys_os_sched_unlock();
        return FALSE;
    }
    libc_memcpy(p_cur_rcp_data, p_rcp->rcp_data, p_rcp->rcp_len);

    p_cur_rcp = (rmt_ctrl_pkg_t *)key_rcp_queue_p + key_rcp_queue_writeptr;
    p_cur_rcp->rcp_data = p_cur_rcp_data;
    p_cur_rcp->rcp_len = p_rcp->rcp_len;

    if(key_rcp_queue_depth != 0) {
        key_rcp_queue_writeptr = (key_rcp_queue_writeptr + 1) % key_rcp_queue_depth;
    }
    key_rcp_queue_depth_empty--;
    sys_os_sched_unlock();
    //#endif

    return TRUE;
}

/* 从 RCP 队列中读取一个 RCP 对象 */
bool key_inner_rcp_queue_recv(rmt_ctrl_pkg_t *p_rcp, void *null1, void *null2)
{
    //#if (SUPPORT_REMOTE_CONTROL == 1)
    if(key_rcp_queue_depth_empty >= key_rcp_queue_depth) {
        return FALSE;
    }

    sys_os_sched_lock();
    libc_memcpy(p_rcp, (rmt_ctrl_pkg_t *)key_rcp_queue_p + key_rcp_queue_readptr, sizeof(rmt_ctrl_pkg_t));

    if(key_rcp_queue_depth != 0) {
        key_rcp_queue_readptr = (key_rcp_queue_readptr + 1) % key_rcp_queue_depth;
    }
    key_rcp_queue_depth_empty++;
    sys_os_sched_unlock();
    //#endif

    return TRUE;
}

/* RCP 处理完后必须调用该接口释放堆空间，否则会内存泄漏 */
bool key_inner_rcp_queue_free(rmt_ctrl_pkg_t *p_rcp, void *null1, void *null2)
{
    //#if (SUPPORT_REMOTE_CONTROL == 1)
    if(p_rcp->rcp_data != NULL) {
        key_inner_rcp_free(p_rcp->rcp_data);
    }
    //#endif

    return TRUE;
}

/* I2C/SPI/UART 通道应答
 * mode: I2C_REPLY_HEADER 表示起始命令头，这时 data_len 表示命令头+数据总长度
 *       I2C_REPLY_DATA 表示后续数据包，这时 data_len 表示本次数据长度
 */
bool key_inner_rcp_queue_reply(uint8 mode, void *data, uint16 data_len)
{
    uint8 *data_stream = (uint8 *) data;
    uint32 temp_flag;
    uint8 last_send = FALSE;


    //关中断
    temp_flag = sys_local_irq_save();

    //命令包
    if(mode == PHY_REPLY_HEADER) {
        libc_memcpy(g_transmit_buf, data_stream, key_rcp_entity_head);

        long_answer_len = data_len;

    }
    //数据包
    else {
        libc_memcpy(g_transmit_buf, data_stream, data_len);

    }

    //i2c_cmd_status = I2C_SLAVE_SUCCEED_DATA;
    set_communicate_status(COMUNICATE_REPLY);

    //开中断
    sys_local_irq_restore(temp_flag);

    while(get_communicate_status() == COMUNICATE_REPLY) {
        //clear watch dog //sys_os_time_dly(1);//sys delay
        act_writel(act_readl(WD_CTL) | (0x01), WD_CTL);
        //load_answer_data();
    }
    //set data send end flag
    if(get_communicate_status() == COMUNICATE_IDLE) {
        //clear send infinate machine
        set_send_status(SEND_STATUS_START);
    }
}