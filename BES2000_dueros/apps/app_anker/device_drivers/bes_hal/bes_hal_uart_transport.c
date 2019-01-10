#include "bes_hal_uart.h"
#include "app_thread.h"
#include "string.h"
#include "hal_trace.h"
//#include "middle_transport.h"

#define TRANSPORT_RX_BUF_LEN       (1024)
#ifdef CHIP_HAS_PSRAM
#include "hal_location.h"
PSRAM_BSS_LOC uint8_t uart_comm_buf[TRANSPORT_RX_BUF_LEN];
#else
uint8_t uart_comm_buf[TRANSPORT_RX_BUF_LEN];
#endif

static unsigned int comm_buf_len = TRANSPORT_RX_BUF_LEN;
static unsigned int extra_buf_len = TRANSPORT_RX_BUF_LEN;
static uint8_t uartRxDmaFlag = 1;

void bes_hal_uart_transport_event_notify(UART_DMA_STATE rxDmaState, uint32_t rxDmaSize)
{
    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODUAL_UART_TRANSPORT_SVR;
    msg.msg_body.message_Param0 = rxDmaState;
    msg.msg_body.message_Param1 = rxDmaSize;
    app_mailbox_put(&msg);
//  BES_HAL_UART_DEBUG("%s: event notify: param0 = 0x%08x, param1 = 0x%08x",__func__,msg.msg_body.message_Param0,msg.msg_body.message_Param1);
}

static void init_comm_state(unsigned char *buf, unsigned int len)
{
    memset(buf, 0, len);
}


int bes_hal_uart_transport_proc(APP_MESSAGE_BODY *msg_body)
{
    unsigned int comm_real_rcv_len = 0;
    bool rx_need_reset = false;    
    bool rx_err = false;
    signed int retErr = 0;
    
//  BES_HAL_UART_DEBUG("%s: communication_proc enter",__func__);
    switch (msg_body->message_Param0) {
        case UART_DMA_IDLE:
            comm_real_rcv_len = 0;
            if (uartRxDmaFlag != 0) 
            {
                // the second param is not used by this function. so don't need init the second param;
                bes_hal_uart_recv_dma(uart_comm_buf, extra_buf_len, comm_buf_len);
            } else {
                BES_HAL_UART_INFO("%s: communication_proc-->recv_data 0x%08x, %d",__func__, uart_comm_buf, comm_buf_len);
                bes_hal_uart_recv(uart_comm_buf, comm_buf_len);
            }
            break;
        case UART_DMA_START:        
            break;
        case UART_DMA_DONE: 
            comm_real_rcv_len = msg_body->message_Param1;
            BES_HAL_UART_DEBUG("%s: comm_real_rcv_len:%d",__func__, comm_real_rcv_len);
  
            if (comm_real_rcv_len <= comm_buf_len)
            {
                BES_HAL_UART_DEBUG("%s: Receiving part of the data: expect=%d, real=%d",__func__, comm_buf_len,comm_real_rcv_len);
            //    BES_HAL_UART_DEBUG("%s: uart rcv data:",__func__);
            //    bes_hal_dump8(uart_comm_buf,comm_real_rcv_len);

            //    retErr = app_mem_enqueue_rcv_data((uint8_t*)uart_comm_buf,(uint32_t)comm_real_rcv_len);
                if(retErr < 0)
                {
                    BES_HAL_UART_ERROR("%s: app_mem_enqueue_rcv_data failed : ret = %d",__func__, retErr);
                }   
            //    else
            //    {
                //    BES_HAL_UART_DEBUG("%s: app_mem_enqueue_rcv_data succeed, notify receave thread!",__func__);
            //    }    
                rx_err = false;
                rx_need_reset = true;
                goto exit;
            }

            rx_err = true;
            rx_need_reset = true;
            break;
        case UART_DMA_ERROR:
        default:            
            BES_HAL_UART_ERROR("Uart failed");
            rx_err = true;
            rx_need_reset = true;
            goto exit;
            break;
    }
    
exit:
    if(rx_err){        
        bes_hal_uart_reinit();
        init_comm_state(uart_comm_buf, comm_buf_len);    
    }
    
    if (rx_need_reset){
        bes_hal_uart_recv_reset();
    }    
//    BES_HAL_UART_DEBUG("communication_proc exit");
    return 0;
}

int bes_hal_uart_transport_init(void)
{
#if UART_OTA_PORT_SEL == UART_OTA_PORT_SAME_SYS_DEBUG
#ifdef DEBUG
    hal_trace_close();
#endif
    hal_trace_log_flag(0);
#endif
    
    bes_hal_set_log_level(BES_LOG_LVL_DUEBU);
    init_comm_state(uart_comm_buf, comm_buf_len);
    app_set_threadhandle(APP_MODUAL_UART_TRANSPORT_SVR, bes_hal_uart_transport_proc);

    bes_hal_uart_init();
    bes_hal_uart_recv(uart_comm_buf, comm_buf_len); // clear rx dma buf & intr
    BES_HAL_UART_DEBUG("%s: uart transport init",__func__);
    return 0;
}