#include "../key_inner.h"

uint8 g_msg_status;

/*存储最后一个命令包到rcP命令队列*/
void save_last_command_data(void)
{
    rmt_ctrl_pkg_t tmp_rcp;

    //无格式远程控制数据包，将命令头也实时申请
    libc_memcpy(pmasterdata, g_receive_buf, key_rcp_entity_head);

    tmp_rcp.rcp_data = pmasterdata_head;

    tmp_rcp.rcp_len = long_command_len;

    //long_command_len_remain = long_command_len = 0;
    //receive data lenth
    long_command_len = 0;

    key_inner_rcp_queue_send_rcode(&tmp_rcp);

    //need deal commond(and wait deal end)
    g_msg_status = COMUNICATE_BUSY;
}

/*存储命令到rcP命令队列*/
void store_command_data(void)
{

    //first receive?
    if(long_command_recv == FALSE) {
        //
        long_command_len = COMMAND_LENGTH(g_receive_buf);
        //malloc data
        pmasterdata_head = key_inner_rcp_malloc(long_command_len);

        if(pmasterdata_head == NULL) {
            //空间申请失败，丢掉该消息
            uart1_transmit_deal(SEND_INVALID_FLAG, 0);
            //set send continue
            g_send_cont = TRUE;
            //reset finite state machine
            g_msg_status = COMUNICATE_IDLE;
            return;
        }
        //data
        pmasterdata = pmasterdata_head;
        //check length
        //long_command_len_remain = long_command_len;

        //长命令
        if(long_command_len > key_rcp_entity_head) {

            //长命令第一个命令包

            //无格式远程控制数据包，将命令头也实时申请
            libc_memcpy(pmasterdata, g_receive_buf, key_rcp_entity_head);
            //get data
            pmasterdata += key_rcp_entity_head;

            //long_command_len_remain -= key_rcp_entity_head;
            long_command_len -= key_rcp_entity_head;
            //set long commond flag
            long_command_recv = TRUE;

            //we need get parameter next
            g_msg_status = COMUNICATE_COM_CONT;
        } else {
            //save commond data
            save_last_command_data();

        }
    } else {
        //长命令后续数据包

        //长命令中间分批包处理
        if(long_command_len > MAX_SEGMENT_LEN) {
            libc_memcpy(pmasterdata, g_receive_buf, MAX_SEGMENT_LEN);

            pmasterdata += MAX_SEGMENT_LEN;

            long_command_len -= MAX_SEGMENT_LEN;
        }
        //长命令最后一个参数包
        else {
            //save data
            save_last_command_data();
            //clear long commond flag
            long_command_recv = FALSE;

        }
    }
    if(g_msg_status == COMUNICATE_BUSY) {
        uart1_transmit_deal(SEND_VALID_FLAG, 0);
        //set send continue
        g_send_cont = TRUE;
    }

}

//send data


/*加载应答包到通信缓冲区，准备发送给主机*/
void load_answer_data(void)
{
    uint8 send_lenth = 8;

    //if we haven't recived valid packet don't send this time
    if(g_send_cont == FALSE) {
        return;
    }
    //first replay send start flag
    if(g_cmd_status == SEND_STATUS_START) {
        //send start packet
        uart1_transmit_deal(SEND_CMD_START, 0);

        g_cmd_status = SEND_STATUS_CONT;

        return;
    }
    //if((g_end_flag == TRUE)&&(g_send_data_len == 0))
    if(g_cmd_status == SEND_STATUS_FLAG) {
        uart1_transmit_deal(SEND_CMD_END, 0);

        g_msg_status = COMUNICATE_IDLE;
        //set end flag
        g_cmd_status = SEND_STATUS_END;

        return;
    }
    if(g_send_data_len == 0) {
        if(long_answer_reply == FALSE) {
            //长答复
            if(long_answer_len > key_rcp_entity_head) {
                //长答复第一个命令包
                long_answer_len -= key_rcp_entity_head;
                //long answer
                long_answer_reply = TRUE;

            } else {
                //短答复
                long_answer_len = 0;
            }

            //first send cmd header
            g_send_data_len = key_rcp_entity_head;
            //send start packet

        }
        //长答复后续数据包
        else {
            //长答复中间分批包处理
            if(long_answer_len > MAX_SEGMENT_LEN) {
                //send_lenth = MAX_SEGMENT_LEN;
                g_send_data_len = MAX_SEGMENT_LEN;

                long_answer_len -= MAX_SEGMENT_LEN;
            }
            //长答复最后一个数据包
            else if(long_answer_len != 0) {

                g_send_data_len = long_answer_len;
                //end this cmd
                long_answer_len = 0;

            } else {
                //end this cmd
                g_send_data_len = 0;
            }

        }
    }
    //else
    //{
    //send continue data already in transmit buffer

    //check if it is last frame
    if(g_send_data_len <= 8) {
        //last data serial transimits
        send_lenth = g_send_data_len;
    }

    uart1_transmit_deal(SEND_CMD_DATA, send_lenth);

    if(g_send_data_len == 0) {
        //packet over
        if(long_answer_len == 0) {
            long_answer_reply = FALSE;
            g_cmd_status = SEND_STATUS_FLAG;
        } else {
            //need sequencial packet
            g_msg_status = COMUNICATE_COM_CONT;
        }
    }
}

uint8 get_communicate_status(void)
{
    return g_msg_status;
}

void set_communicate_status(uint8 status)
{
    g_msg_status = status;
}


uint8 get_send_status(void)
{
    return g_cmd_status;
}

void set_send_status(uint8 status)
{
    g_cmd_status = status;
}
