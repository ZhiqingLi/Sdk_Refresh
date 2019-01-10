#include "cmsis_os.h"
#include "stdint.h"
#include "stdbool.h"
#include "plat_types.h"
#include "string.h"
#include "stdio.h"
#include "crc32.h"
#include "tool_msg.h"
#include "cmsis_nvic.h"
#include "hal_trace.h"
#include "communication_svr.h"
#include "communication_sysapi.h"
#include "communication_cmd_handle.h"
#include "app_thread.h"
#include "hal_location.h"
//======================================================================================================

#define COMMAND_PARSER_VERSION          0x0100
#define BULK_READ_STEP_SIZE             0x1000


enum COMM_STATE {
    COMM_NONE,
    COMM_SYNC,
};

static enum COMM_STATE comm_state;

static enum PARSE_STATE parse_state;
static struct message_t recv_msg;
static struct message_t send_msg = { { PREFIX_CHAR, }, };
static unsigned char send_seq = 0;

static unsigned int extra_buf_len;

static unsigned char *comm_buf = NULL;
static unsigned int comm_buf_len = 0;


#ifdef __PC_CMD_UART__ 

#include "app_bt_stream.h"
extern eq_keyword_table eq_table[EQ_KEY_WORD_NUM];

#ifdef SYSTEM_USE_PSRAM
PSRAM_DATA_LOC static eq_msg EQ_MSG_RX;
PSRAM_DATA_LOC static eq_reply EQ_MSG_TX;
#else
static eq_msg EQ_MSG_RX;
static eq_reply EQ_MSG_TX;
#endif
void eq_reply_payload_updata(unsigned char *buf, unsigned int len)
{
	EQ_MSG_TX.head.len=len;
	memcpy(EQ_MSG_TX.tx_buf,buf,len);
}
	
#endif

static void POSSIBLY_UNUSED dump_buffer(unsigned char *buf, unsigned int len)
{
    DUMP8("0x%02x ", buf, len);
}

static unsigned char check_sum(unsigned char *buf, unsigned char len)
{
    int i;
    unsigned char sum = 0;

    for (i = 0; i < len; i++) {
        sum += buf[i];
    }

    return sum;
}

static int send_sync_msg(void)
{
    int ret;

    send_msg.hdr.type = TYPE_SYNC;
    send_msg.hdr.seq = send_seq++;
    send_msg.hdr.len = 3;
    send_msg.data[0] = 0x00;
    send_msg.data[1] = COMMAND_PARSER_VERSION & 0xFF;
    send_msg.data[2] = (COMMAND_PARSER_VERSION >> 8) & 0xFF;
    send_msg.data[3] = ~check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = send_data((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));

    return ret;
}

static int send_sync_confirm(void)
{
    int ret;

    send_msg.hdr.type = TYPE_SYNC;
    send_msg.hdr.seq = recv_msg.hdr.seq;
    send_msg.hdr.len = 3;
    send_msg.data[0] = 0x02;
    send_msg.data[1] = COMMAND_PARSER_VERSION & 0xFF;
    send_msg.data[2] = (COMMAND_PARSER_VERSION >> 8) & 0xFF;
    send_msg.data[3] = ~check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = send_data((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));

    return ret;
}

static int send_reply(const unsigned char *payload, unsigned int len)
{
    int ret = 0;
    TRACE("send_reply len: %d", len);
#ifdef __PC_CMD_UART__
	ret = send_data(payload, len);
#else
    if (len > sizeof(send_msg.data)) {
        TRACE("Packet length too long: %d", len);
        return -1;
    }

    send_msg.hdr.type = recv_msg.hdr.type;
    send_msg.hdr.seq = recv_msg.hdr.seq;
    send_msg.hdr.len = len;
    memcpy(&send_msg.data[0], payload, len);
    send_msg.data[len] = ~check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = send_data((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));
#endif
    return ret;
}

static void init_parse_state(unsigned char **buf, unsigned int *len)
{
    parse_state = PARSE_HEADER;
    memset(&recv_msg.hdr, 0, sizeof(recv_msg.hdr));

    *buf = (unsigned char *)&recv_msg.hdr;
    *len = sizeof(recv_msg.hdr);
}

static void init_comm_state(unsigned char **buf, unsigned int *len)
{
    comm_state = COMM_NONE;
    init_parse_state(buf, len);
}

static enum ERR_CODE check_msg_hdr(void)
{
    enum ERR_CODE errcode = ERR_NONE;

    switch (recv_msg.hdr.type) {
        case TYPE_SYNC:
            if (recv_msg.hdr.len != 1) {
                //TRACE("SYNC msg length error: %d", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            // Not to check sequence number for sync msg
            break;
        case TYPE_SYS:
            if (recv_msg.hdr.len != 1 && recv_msg.hdr.len != 5) {
                //TRACE("SYS msg length error: %d", recv_msg.hdr.len);
                errcode = ERR_LEN;
            } else if (comm_state < COMM_SYNC) {
                //TRACE("Not sync yet - cannot accept SYS msg");
                errcode = ERR_NOT_SYNC;
            }
            break;
        case TYPE_READ:
            if (recv_msg.hdr.len != 5) {
                //TRACE("READ msg length error: %d", recv_msg.hdr.len);
                errcode = ERR_LEN;
            } else if (comm_state < COMM_SYNC) {
                //TRACE("Not sync yet - cannot accept READ msg");
                errcode = ERR_NOT_SYNC;
            }
            break;
        case TYPE_WRITE:
            if (recv_msg.hdr.len <= 4 || recv_msg.hdr.len > 20) {
                //TRACE("WRITE msg length error: %d", recv_msg.hdr.len);
                errcode = ERR_LEN;
            } else if (comm_state < COMM_SYNC) {
                //TRACE("Not sync yet - cannot accept WRITE msg");
                errcode = ERR_NOT_SYNC;
            }
            break;
        case TYPE_BULK_READ:
            if (recv_msg.hdr.len != 8) {
                //TRACE("BULK_READ msg length error: %d", recv_msg.hdr.len);
                errcode = ERR_LEN;
            } else if (comm_state < COMM_SYNC) {
                //TRACE("Not sync yet - cannot accept BULK_READ msg");
                errcode = ERR_NOT_SYNC;
            }
            break;
        case TYPE_COMMUNICATION_CMD:
            {
                errcode = communication_cmd_check_msg_hdr(&recv_msg);
                break;
            }
            break;
        default:
            TRACE("Invalid message type: 0x%x", recv_msg.hdr.type);
            errcode = ERR_TYPE_INVALID;
    }

    return errcode;
}

static enum ERR_CODE handle_sys_cmd(enum SYS_CMD_TYPE cmd, unsigned char *param, unsigned int len)
{
    unsigned char cret[5];
    unsigned int bootmode;

    cret[0] = ERR_NONE;

    if (cmd == SYS_CMD_SET_BOOTMODE || cmd == SYS_CMD_CLR_BOOTMODE) {
        if (len != 4) {
            TRACE("Invalid SYS CMD len %d for cmd: 0x%x", len, cmd);
            return ERR_DATA_LEN;
        }
    } else {
        if (len != 0) {
            TRACE("Invalid SYS CMD len %d for cmd: 0x%x", len, cmd);
            return ERR_DATA_LEN;
        }
    }

    switch (cmd) {
        case SYS_CMD_REBOOT: {
            TRACE("--- Reboot---");
            send_reply(cret, 1);
            system_reboot();
            break;
        }
        case SYS_CMD_SHUTDOWN: {
            TRACE("--- Shutdown ---");
            send_reply(cret, 1);
            system_shutdown();
            break;
        }
        case SYS_CMD_SET_BOOTMODE: {
            TRACE("--- Set bootmode ---");
            memcpy(&bootmode, param, 4);
            system_set_bootmode(bootmode);
            send_reply(cret, 1);
            break;
        }
        case SYS_CMD_CLR_BOOTMODE: {
            TRACE("--- Clear bootmode ---");
            memcpy(&bootmode, param, 4);
            system_clear_bootmode(bootmode);
            send_reply(cret, 1);
            break;
        }
        case SYS_CMD_GET_BOOTMODE: {
            TRACE("--- Get bootmode ---");
            bootmode = system_get_bootmode();
            memcpy(&cret[1], &bootmode, 4);
            send_reply(cret, 5);
            break;
        }
        default: {
            TRACE("Invalid command: 0x%x", recv_msg.data[0]);
            return ERR_SYS_CMD;
        }
    }

    return ERR_NONE;
}

static enum ERR_CODE handle_read_cmd(unsigned int addr, unsigned int len)
{
    union {
            unsigned int data[4];
            unsigned char buf[19];
        } d;
    int i;
#ifndef _WIN32
    int cnt;
    unsigned short *p;
#endif

    TRACE("[READ] addr=0x%08x, len=%d", addr, len);

    if (debug_read_enabled() == 0) {
        TRACE("[READ] No access right");
        return ERR_ACCESS_RIGHT;
    }
    if (len > 16) {
        TRACE("[READ] Length error: %d", len);
        return ERR_DATA_LEN;
    }

#ifdef _WIN32
    for (i = 0; i < 4; i++) {
        d.data[i] = (0x11 + i) | ((0x22 + i) << 8) | ((0x33 + i) << 16) | ((0x44 + i ) << 24);
    }
#else
    // Handle half-word and word register reading
    if ((len & 0x03) == 0 && (addr & 0x03) == 0) {
        cnt = len / 4;
        for (i = 0; i < cnt; i++) {
            d.data[i] = *((unsigned int *)addr + i);
        }
    } else if ((len & 0x01) == 0 && (addr & 0x01) == 0) {
        cnt = len / 2;
        p = (unsigned short *)&d.data[0];
        for (i = 0; i < cnt; i++) {
            p[i] = *((unsigned short *)addr + i);
        }
    } else {
        memcpy(&d.data[0], (unsigned char *)addr, len);
    }
#endif

    memmove(&d.buf[1], &d.buf[0], len);
    d.buf[0] = ERR_NONE;
    send_reply((unsigned char *)&d.buf[0], 1 + len);

    return ERR_NONE;
}

static enum ERR_CODE handle_write_cmd(unsigned int addr, unsigned int len, unsigned char *wdata)
{
    unsigned int data;
#ifdef _WIN32
    unsigned char d[16];
#else
    int i;
    int cnt;
#endif

    TRACE("[WRITE] addr=0x%08x, len=%d", addr, len);

    if (debug_write_enabled() == 0) {
        TRACE("[WRITE] No access right");
        return ERR_ACCESS_RIGHT;
    }
    if (len > 16) {
        TRACE("[WRITE] Length error: %d", len);
        return ERR_DATA_LEN;
    }

#ifdef _WIN32
    memcpy(d, wdata, len);
    dump_buffer(d, len);
#else
    // Handle half-word and word register writing
    if ((len & 0x03) == 0 && (addr & 0x03) == 0) {
        cnt = len / 4;
        for (i = 0; i < cnt; i++) {
            data = wdata[4 * i] | (wdata[4 * i + 1] << 8) | (wdata[4 * i + 2] << 16) | (wdata[4 * i + 3] << 24);
            *((unsigned int *)addr + i) = data;
        }
    } else if ((len & 0x01) == 0 && (addr & 0x01) == 0) {
        cnt = len / 2;
        for (i = 0; i < cnt; i++) {
            data = wdata[2 * i] | (wdata[2 * i + 1] << 8);
            memcpy(&data, wdata + 2 * i, 2);
            *((unsigned short *)addr + i) = (unsigned short)data;
        }
    } else {
        memcpy((unsigned char *)addr, wdata, len);
    }
#endif

    data = ERR_NONE;
    send_reply((unsigned char *)&data, 1);

    return ERR_NONE;
}

static enum ERR_CODE handle_bulk_read_cmd(unsigned int addr, unsigned int len)
{
    int ret;
    unsigned int sent;
    unsigned char cret[1];

    TRACE("[BULK_READ] addr=0x%08x, len=%d", addr, len);

    if (debug_read_enabled() == 0) {
        TRACE("[READ] No access right");
        return ERR_ACCESS_RIGHT;
    }

    cret[0] = ERR_NONE;
    send_reply(cret, 1);

    ret = 0;
    while (ret == 0 && len > 0) {
        sent = (len > BULK_READ_STEP_SIZE) ? BULK_READ_STEP_SIZE : len;
        ret = send_data((unsigned char *)addr, sent);
        addr += sent;
        len -= sent;
    }

    if (ret) {
        TRACE("[BULK_READ] Failed to send data: %d", ret);
        // Just let the peer timeout
    }

    return ERR_NONE;
}



static enum ERR_CODE handle_data(unsigned char **buf, unsigned int *len, int *extra)
{
    enum ERR_CODE errcode;

    *extra = 0;

    // Checksum
    if (check_sum((unsigned char *)&recv_msg, MSG_TOTAL_LEN(&recv_msg)) != 0xFF) {
        TRACE("Checksum error");
        return ERR_CHECKSUM;
    }

    switch (recv_msg.hdr.type) {
        case TYPE_SYNC: {
            if (recv_msg.data[0] == 0x01) {
                TRACE("[SYNC]: done");
                comm_state = COMM_SYNC;
                TRACE("------ Send SYNC confirm ------");
                send_sync_confirm();
            } else {
                TRACE("[SYNC]: Unrecoganized sync word: 0x%02x", recv_msg.data[0]);
                comm_state = COMM_NONE;
                TRACE("------ Send SYNC msg ------");
                send_sync_msg();
            }
            break;
        }
        case TYPE_SYS: {
            TRACE("------ SYS CMD ------");
            errcode = handle_sys_cmd((enum SYS_CMD_TYPE)recv_msg.data[0], &recv_msg.data[1], recv_msg.hdr.len - 1);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_READ: {
            unsigned int addr;
            unsigned int len;

            TRACE("------ READ CMD ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            len = recv_msg.data[4];

            errcode = handle_read_cmd(addr, len);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_WRITE: {
            unsigned int addr;
            unsigned int len;
            unsigned char *wdata;

            TRACE("------ WRITE CMD ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            len = recv_msg.hdr.len - 4;
            wdata = &recv_msg.data[4];

            errcode = handle_write_cmd(addr, len, wdata);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_BULK_READ: {
            unsigned int addr;
            unsigned int len;

            TRACE("------ BULK READ CMD ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            len = recv_msg.data[4] | (recv_msg.data[5] << 8) | (recv_msg.data[6] << 16) | (recv_msg.data[7] << 24);

            errcode = handle_bulk_read_cmd(addr, len);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_COMMUNICATION_CMD: {			
            errcode = communication_cmd_handle_cmd((enum COMMUNICATION_CMD_TYPE)recv_msg.data[0], &recv_msg.data[1], recv_msg.hdr.len - 1);
            TRACE_TIME("------ EXTEND COMMUNICATION CMD ------");
			if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        default:
            TRACE("Invalid message type: 0x%x", recv_msg.hdr.type);
            return ERR_TYPE_INVALID;
    }

    return ERR_NONE;
}

static enum ERR_CODE handle_extra(unsigned char **buf, unsigned int *len, int *extra)
{
    *extra = 0;

    switch (recv_msg.hdr.type) {
        case TYPE_SIG_INFO:
            TRACE("------ Recv sig ------");
            return ERR_TYPE_INVALID;
            break;
        case TYPE_CODE_INFO:
             TRACE("------ Recv code ------");
            return ERR_TYPE_INVALID;
            break;
        default:
            TRACE("[EXTRA] Invalid message type: %d", recv_msg.hdr.type);
            return ERR_TYPE_INVALID;
    }
    return ERR_NONE;
}
#ifdef __PC_CMD_UART__
static int parse_packet(unsigned char *buf, unsigned int len)
{
    enum ERR_CODE errcode;
    unsigned char *data;
	eq_msg *eq_ptr;

	eq_ptr=(eq_msg *)buf;

	enum PARSE_STATE cur_par_state=PARSE_HEADER;
	
	switch(cur_par_state){
        case PARSE_HEADER:
            ASSERT(len > 0 && len > sizeof(EQ_MSG_RX.head), "Invalid EQ len!");

            if (eq_ptr->head.prefix != PREFIX_CHAR) {
				TRACE("eq head error %c",eq_ptr->head.prefix);
            } else {
            	TRACE("CRC=%d",eq_ptr->head.crc);
				TRACE("LEN=%d",eq_ptr->head.len);
				TRACE("NAME=%s",eq_ptr->head.name);
				memcpy(&EQ_MSG_TX.head,&eq_ptr->head,sizeof(eq_head));
				EQ_MSG_TX.head.len=0;//reply payload len set 0 first
				cur_par_state=PARSE_DATA;
            }
        case PARSE_DATA:
				for(uint8_t i=0;i<EQ_KEY_WORD_NUM;i++ ){
					if (strcmp((char *)eq_ptr->head.name,(char *)eq_table[i].eq_keywords) == 0){
						//TRACE("spec NAME=%s",(char *)eq_table[i].eq_keywords);
						if(eq_table[i].eq_callback!=NULL){
							errcode=(enum ERR_CODE)eq_table[i].eq_callback(eq_ptr->rx_buf,eq_ptr->head.len);//some callback may updata tx payload 
						}
						if (errcode != ERR_NONE) {
            				 	goto _err;
            			}else{
            				if(EQ_MSG_TX.head.len){//need reply payload
            			 		errcode = communication_cmd_handle_cmd(COMMUNICATION_CMD_EQ_OP, (uint8_t *)&EQ_MSG_TX, EQ_MSG_TX.head.len+sizeof(eq_head));
            				}else{
								errcode = communication_cmd_handle_cmd(COMMUNICATION_CMD_EQ_OP, (uint8_t *)&EQ_MSG_TX, sizeof(eq_head)-4);//reply prefix crc name only
            				}
							if(errcode != ERR_NONE){
								goto _err;
							}else{
								return 0;
							}
            			}
					}
				}

            break;
        default:
            TRACE("Invalid parse_state: %d", cur_par_state);
            break;
		}

    return 0;

_err:
    cancel_input();
    return 1;
}


#else
static int parse_packet(unsigned char **buf, unsigned int *len)
{
    enum ERR_CODE errcode;
    unsigned int rlen = *len;
    unsigned char *data;
    unsigned int i;
    int extra;
    unsigned char cret;

    switch (parse_state) {
        case PARSE_HEADER:
            ASSERT(rlen > 0 && rlen <= sizeof(recv_msg.hdr), "Invalid rlen!");

            if (recv_msg.hdr.prefix != PREFIX_CHAR) {
                data = (unsigned char *)&recv_msg.hdr.prefix;
                for (i = 1; i < rlen; i++) {
                    if (data[i] == PREFIX_CHAR) {
                        memmove(&recv_msg.hdr.prefix, &data[i], rlen - i);
                        break;
                    }
                }
                *buf = &data[rlen - i];
                *len = sizeof(recv_msg.hdr) + i - rlen;
            } else {
                errcode = check_msg_hdr();
                if (errcode != ERR_NONE) {
                    goto _err;
                }
                parse_state = PARSE_DATA;
                *buf = &recv_msg.data[0];
                *len = recv_msg.hdr.len + 1;
            }
            break;
        case PARSE_DATA:
        case PARSE_EXTRA:
            if (parse_state == PARSE_DATA) {
                errcode = handle_data(buf, len, &extra);
            } else {
                errcode = handle_extra(buf, len, &extra);
            }
            if (errcode != ERR_NONE) {
                goto _err;
            }
            if (extra) {
                parse_state = PARSE_EXTRA;
            } else {
                // Receive next message
                init_parse_state(buf, len);
            }
            break;
        default:
            TRACE("Invalid parse_state: %d", parse_state);
            break;
    }

    return 0;

_err:
    cancel_input();
    if (comm_state != COMM_NONE) {
        cret = (unsigned char)errcode;
        send_reply(&cret, 1);
    }

    return 1;
}
#endif

int communication_proc(APP_MESSAGE_BODY *msg_body)
{
    unsigned int comm_real_rcv_len = 0;
    bool rx_need_reset = false;    
    bool rx_err = false;
    int ret;
    TRACE("communication_proc enter");
    switch (msg_body->message_Param0) {
        case UART_DMA_IDLE:
            comm_real_rcv_len = 0;
#ifdef __PC_CMD_UART__ 
			memset((uint8_t *)&EQ_MSG_RX,0,sizeof(eq_msg));
			recv_data((uint8_t *)&EQ_MSG_RX, sizeof(eq_msg));
#else
			if (parse_state == PARSE_EXTRA) {
				recv_data_dma(comm_buf, extra_buf_len, sizeof(recv_msg));
			} else {
				TRACE("communication_proc-->recv_data 0x%08x, %d", comm_buf, sizeof(recv_msg));
				recv_data(comm_buf, sizeof(recv_msg));
			}			
#endif			
            break;
        case UART_DMA_START:        
            break;
        case UART_DMA_DONE: 
            comm_real_rcv_len = msg_body->message_Param1;
            TRACE("comm_real_rcv_len:%d", comm_real_rcv_len);
#ifdef 	__PC_CMD_UART__
#if 0
			if ((app_bt_stream_isrun(APP_BT_STREAM_A2DP_SBC))
#ifdef AUDIO_LINEIN				
				||(app_bt_stream_isrun(APP_PLAY_LINEIN_AUDIO))
#endif

			)
#endif	
			if(1)	
			{	
				if (comm_real_rcv_len < sizeof(EQ_MSG_RX.head)){
					TRACE("Receiving part of the data: real=%d", comm_real_rcv_len); 			   
					rx_err = true;
					rx_need_reset = true;
					goto exit;
				}
				//DUMP32("%08x ",(uint32_t *)&EQ_MSG_RX.rx_buf,20);
				ret = parse_packet((uint8_t *)&EQ_MSG_RX, comm_real_rcv_len);
				if (ret) {
					TRACE("Parsing packet failed"); 			   
					rx_err = true;
					rx_need_reset = true;
					goto exit;
				}	

				rx_need_reset = true;
				comm_real_rcv_len == 0;
			}else{
				TRACE("EQ TOOL CANNOT USE!!!");
				return 0;
			}
#else
			//DUMP8("0x%02x ", comm_buf, comm_real_rcv_len);
			if (parse_state == PARSE_HEADER){
				if (comm_real_rcv_len < sizeof(recv_msg.hdr)){
					TRACE("Receiving part of the data: expect=%d, real=%d", comm_real_rcv_len); 			   
					rx_err = true;
					rx_need_reset = true;
					goto exit;
				}
				ret = parse_packet(&comm_buf, &comm_buf_len);
				if (ret) {
					TRACE("Parsing packet failed"); 			   
					rx_err = true;
					rx_need_reset = true;
					goto exit;
				}			 
				comm_real_rcv_len -= sizeof(recv_msg.hdr);
			}
			if (comm_buf_len != comm_real_rcv_len) {
				TRACE("Receiving part of the data: expect=%d, real=%d", comm_buf_len, comm_real_rcv_len);				 
				rx_err = true;
				rx_need_reset = true;
				goto exit;
			}

			ret = parse_packet(&comm_buf, &comm_buf_len);
			if (ret) {
				TRACE("Parsing packet failed"); 			   
				rx_err = true;
				rx_need_reset = true;
				goto exit;
			}
#endif			
            rx_need_reset = true;
            break;
        case UART_DMA_ERROR:
        default:            
            TRACE("Uart failed");
            rx_err = true;
            rx_need_reset = true;
            goto exit;
            break;
    }
    
exit:
    if(rx_err){        
        reinit_transport();
#ifndef __PC_CMD_UART__	
		init_comm_state(&comm_buf, &comm_buf_len);
#endif
    }
    
    if (rx_need_reset){
        recv_data_reset();
    }    
    TRACE("communication_proc exit");
    return 0;
}

void communication_init(void)
{
    app_set_threadhandle(APP_MODUAL_COMMUNICATION_SVR, communication_proc);
    communication_cmd_init(send_reply);
    init_transport(); 
#ifdef __PC_CMD_UART__
	memset((uint8_t *)&EQ_MSG_RX,0,sizeof(eq_msg));
	recv_data((uint8_t *)&EQ_MSG_RX, sizeof(eq_msg));
#else
	init_comm_state(&comm_buf, &comm_buf_len);
	recv_data(comm_buf, sizeof(recv_msg));
#endif
}

void communication_deinit(void)
{
	deinit_transport();
}



#ifdef KNOWLES_UART_DATA 

void uart_audio_init()
{
	init_transport();
}

void uart_audio_deinit()
{
	deinit_transport();
}

#endif


