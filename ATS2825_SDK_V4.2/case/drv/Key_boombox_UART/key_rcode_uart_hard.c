#include "key_inner.h"

uint8 g_uart_rx_buf[8]; //uart接收物理通信buffer
uint8 g_uart_tx_buf[8]; //uart发送物理通信buffer
uint8 g_receive_buf[K_VARBUF_SIZE];
uint8 g_transmit_buf[MAX_SEGMENT_LEN]; //应答数据buffer
//read buffer ptr
uint8 *g_read_ptr;
//write buffer ptr
uint8 *g_write_ptr;
//read buffer offset
uint8 g_read_buf_offset;

uint16 long_answer_len; //应答不定长参数长度
bool long_answer_reply; //正在应答不定长参数

uint8 *pmasterdata; //接收命令不定长参数指针
uint8 *pmasterdata_head;         //接收命令指针头，用于最终发送RCP对象传参

uint16 long_command_len; //接收命令不定长参数长度
bool long_command_recv; //正在接收长命令不定长参数
//spp_command_t cur_spp_cmd; //当前接收到的命令体
//spp_command_t reply_spp_cmd; //应答命令体

//uint8 *pmasterdata;              //接收命令当前指针

uint16 g_rw_index; //读写数据序号
uint8 g_bus_status; //总线类型，分为写命令，读数据，读处理状态，和空闲

//send infinate machine
send_status_e g_cmd_status = SEND_STATUS_START; //命令处理状态，空闲状态，忙状态，有应答成功状态，无应答成功状态，

uint8 g_retry_times; //重传次数
uint8 g_send_error;//出错标志

//for send cont flag
uint8 g_send_cont = TRUE;
//发送数据长度
uint16 g_send_data_len;

receive_status_e g_recive_status = RECEIVE_STATUS_IDLE;

const uint8 start_flag_serial[] =
{ 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55 };

const uint8 end_flag_serial[] =
{ 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa };

const uint8 valid_flag_serial[] =
{ 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 };

const uint8 invalid_flag_serial[] =
{ 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

///////////////////////////////////////////////////////////////////////////////////

void uart1_get_data(void)
{
    //uint32 reg_bak;
    libc_memset(g_uart_rx_buf, 0, sizeof(g_uart_rx_buf));
    while(g_rw_index < 8) {
        //if empty break
        if((act_readl(UART1_STA) & (1 << UART1_STA_RFEM1)) != 0) {
            break;
        }
        //get data from  uart fifo
        g_uart_rx_buf[g_rw_index++] = (uint8) act_readl(UART1_RXDAT);
    }
    //clear pending bit
    act_writel((1 << UART1_STA_RIP1), UART1_STA);

}

void uart1_send_data(void)
{
    uint32 data_tx;
    uint8 i;
    //send tx buf data to fifo
    sys_os_sched_lock();
    for(i = 0; i < g_rw_index; i++) {
        //set data
        data_tx = g_uart_tx_buf[i];
        //set data to  tx fifo
        act_writel(data_tx, UART1_TXDAT);
    }
    while((act_readl(UART1_STA) & (1 << UART1_STA_UTBB1)) != 0) {
        ;//wait over
    }
    sys_os_sched_unlock();

    g_send_cont = FALSE;
    //}

}

void uart1_receive_deal(void)
{
    //uart1_hold_bus();

    g_bus_status = UART_RECEIVE;
    //set read index
    g_rw_index = 0;
    //receive data from fifo
    uart1_get_data();
    //是否是无效中断
    if(g_rw_index == 0) {
        //uart1_release_bus();
        return;
    }

    //chcek data
    if(libc_memcmp(g_uart_rx_buf, start_flag_serial, sizeof(start_flag_serial)) == 0) {
        //start of commond searial

        //set buffer offset
        g_read_buf_offset = 0;

        g_recive_status = RECEIVE_STATUS_START;

    } else if(libc_memcmp(g_uart_rx_buf, end_flag_serial, sizeof(end_flag_serial)) == 0) {
        //end flag
        //deal commd
        store_command_data();

        g_bus_status = UART_BUSY;

        g_recive_status = RECEIVE_STATUS_IDLE;
    } else if(libc_memcmp(g_uart_rx_buf, valid_flag_serial, sizeof(valid_flag_serial)) == 0) {
        //set send error
        //g_send_error = 0;

        g_send_cont = TRUE;

        g_bus_status = UART_IDLE;
    } else if(libc_memcmp(g_uart_rx_buf, invalid_flag_serial, sizeof(invalid_flag_serial)) == 0) {
        //set send error
        //g_send_error = 1;
        //we can send data
        g_send_cont = TRUE;

        g_bus_status = UART_IDLE;
    } else {
        if(g_recive_status == RECEIVE_STATUS_START) {
            //pointer
            g_read_ptr = &g_receive_buf[g_read_buf_offset];

            //set data to buffer
            libc_memcpy(g_read_ptr, g_uart_rx_buf, g_rw_index);
            //offset add
            g_read_buf_offset += g_rw_index;
        }
    }

    if((g_bus_status != UART_BUSY) && (g_bus_status != UART_IDLE)) {

        if(g_recive_status == RECEIVE_STATUS_START) {
            //communicate avoid interrupt insert
            g_bus_status = UART_SEND;

            g_rw_index = PYH_WRBUF_SIZE;
            //send valid check code
            libc_memcpy(g_uart_tx_buf, valid_flag_serial, sizeof(valid_flag_serial));
            //send data to fifo
            uart1_send_data();
        }
    }

}

void uart1_transmit_deal(send_packet_e mode, uint8 send_len)
{
    uint8 buf_offset = 0;

    //pointer set
    g_rw_index = PYH_WRBUF_SIZE;
    //send
    g_bus_status = UART_SEND;

    //while(1)
    //{
    switch(mode) {
        case SEND_CMD_DATA:

            libc_memcpy(g_uart_tx_buf, g_write_ptr, send_len);

            g_write_ptr += send_len;

            g_send_data_len -= send_len;
            //check if send over
            if(g_send_data_len == 0) {
                g_write_ptr = g_transmit_buf;
            }
            g_rw_index = send_len;

            break;
        case SEND_CMD_START:
            libc_memcpy(g_uart_tx_buf, start_flag_serial, sizeof(start_flag_serial));
            //send start flag
            g_send_data_len = 0;

            g_write_ptr = g_transmit_buf;

            break;
        case SEND_CMD_END:
            //send end flag
            libc_memcpy(g_uart_tx_buf, end_flag_serial, sizeof(end_flag_serial));

            break;
        case SEND_VALID_FLAG:
            //send valid check code
            libc_memcpy(g_uart_tx_buf, valid_flag_serial, sizeof(valid_flag_serial));

            break;
        case SEND_INVALID_FLAG:
        default:
            libc_memcpy(g_uart_tx_buf, invalid_flag_serial, sizeof(invalid_flag_serial));

            break;
    }
    //send data to fifo
    uart1_send_data();
    //}

}

void uart1_int_process(void)
{
    // uart1 controller interrupt pending bit
    //check interrupt type
    if((act_readl(UART1_STA) & (1 << UART1_STA_RIP1)) != 0) {
        //clear pending bit
        //act_writel((1<<UART1_STA_RIP1),UART1_STA);
        //check receive data status if error do nothing
        //receive data deal
        uart1_receive_deal();

    } else if((act_readl(UART1_STA) & (1 << UART1_STA_TIP1)) != 0) {
        //clear tx irq pending bit
        act_writel(act_readl(UART1_STA) | (1 << UART1_STA_TIP1), UART1_STA);
        //transfor data deal if need continue trans
    } else {
        ;//do nothing
    }

}

