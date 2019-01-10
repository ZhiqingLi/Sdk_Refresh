/***
 * intersys hci
 * YuLongWang @2015
*/

#include <stdio.h>
#include "mbed.h"
#include "rtos.h"
#include "app_utils.h"

extern "C"
{
#include "osapi.h"
#include "hal_intersys.h"
#include "sys/hci.h"
#include "hcitrans.h"
}

#include "intersyshci.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "cqueue.h"

#include "besbt.h"

#define __rx_queue_support__ 1
//#define _INTERSYS_DATA_TRACE_
#define BT_THREAD_BUFFER_RETRY_CNT (5)


/* hci buffer mutex */
osMutexId g_hcibuffer_mutex_id;
osMutexDef(g_hcibuffer_mutex);

/* hci send lock thread id */
static osThreadId intersys_tx_thread_id;

osMessageQDef(intersys_rx_queue_msg, 20, NULL);
osMessageQId intersys_rx_queue_id;

#ifdef _INTERSYS_NO_THREAD_
struct  intersys_rx_buf_t {
    const unsigned char *data;
    unsigned int len;
};

struct  intersys_rx_buf_t intersys_rx_buf = {0,0};
#else
#define INTERSYS_STACK_SIZE 1024
void intersys_rx_thread(void const *argument);
static osThreadId intersys_rx_thread_id;
uint32_t os_thread_def_stack_intersys_rx [INTERSYS_STACK_SIZE/ sizeof(uint32_t)];
osThreadDef_t os_thread_def_intersys_rx_thread = { (intersys_rx_thread), (osPriorityHigh), (INTERSYS_STACK_SIZE), (os_thread_def_stack_intersys_rx),{0,}, ("INTERSYS")};
#endif

#if defined(__rx_queue_support__)
#if defined(_INTERSYS_NO_THREAD_)
#define INTERSYS_RX_BUF_MAX (1024*1)
#else
#define INTERSYS_RX_BUF_MAX (1024*4)
#endif
unsigned char intersys_rx_queue_buf[INTERSYS_RX_BUF_MAX];
CQueue intersys_rx_queue;
#endif

#define INTERSYSHCI_LOC __attribute__((section(".fast_text_sram")))

#define HCI_COMMAND_DATA_PACKET 0x01
#define HCI_ACL_DATA_PACKET     0x02
#define HCI_SCO_DATA_PACKET     0x03
#define HCI_EVENT_PACKET        0x04

/* bluedroid */
#define ACL_RX_PKT_START        2
/* Preamble length for HCI Commands:
 * **      2-bytes for opcode and 1 byte for length
 * */
#define HCI_CMD_PREAMBLE_SIZE   3
/* Preamble length for HCI Events:
 * **      1-byte for opcode and 1 byte for length
 * */
#define HCI_EVT_PREAMBLE_SIZE   2
/* Preamble length for SCO Data:
 * **      2-byte for Handle and 1 byte for length
 * */
#define HCI_SCO_PREAMBLE_SIZE   3
/* Preamble length for ACL Data:
 * **      2-byte for Handle and 2 byte for length
 * */
#define HCI_ACL_PREAMBLE_SIZE   4
/*
 *  * incoming buffer align to usb ep size
 *   */
/* Table of HCI preamble sizes for the different HCI message types */
static const unsigned char hci_preamble_table[] =
{
    HCI_CMD_PREAMBLE_SIZE,
    HCI_ACL_PREAMBLE_SIZE,
    HCI_SCO_PREAMBLE_SIZE,
    HCI_EVT_PREAMBLE_SIZE
};
/* H4 Rx States */
typedef enum {
    H4_RX_MSGTYPE_ST,
    H4_RX_LEN_ST,
    H4_RX_DATA_ST,
    H4_RX_IGNORE_ST
} tHCI_H4_RCV_STATE;
typedef struct
{
    unsigned short          event;
    unsigned short          len;
    unsigned short          offset;
    unsigned short          layer_specific;
    unsigned char           data[];
} HC_BT_HDR;


#define BT_HC_HDR_SIZE (sizeof(HC_BT_HDR))

typedef struct
{
    HC_BT_HDR *p_rcv_msg;          /* Buffer to hold current rx HCI message */
    unsigned short rcv_len;               /* Size of current incoming message */
    unsigned char rcv_msg_type;           /* Current incoming message type */
    tHCI_H4_RCV_STATE rcv_state;    /* Receive state of current rx message */
    unsigned char preload_count;          /* Count numbers of preload bytes */
    unsigned char preload_buffer[6];      /* HCI_ACL_PREAMBLE_SIZE + 2 */
} tHCI_H4_CB;


static tHCI_H4_CB  h4_pcb = {0};
static tHCI_H4_CB  *p_cb;

uint32_t intersts_tx_systime = 0;
inline void intersys_sleep_checker_ping(void)
{
    hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_INTERSYS_HCI);
    intersts_tx_systime = hal_sys_timer_get();
}

void intersys_sleep_checker(void)
{
    if(intersts_tx_systime != 0 && ((hal_sys_timer_get()-intersts_tx_systime)>MS_TO_TICKS(15000))){
        hal_sys_wake_unlock(HAL_SYS_WAKE_LOCK_USER_INTERSYS_HCI);
        intersts_tx_systime = 0;
    }
}

#if 0
static inline unsigned short hci_h4_receive_msg(unsigned char byte)
{
    static unsigned char temp_buf[64];
    static RxBuffHandle p_buf;
    unsigned short    msg_len = 0, len = 0;
    unsigned char     msg_received = 0;
    msg_received = FALSE;

    p_cb = &h4_pcb;

    TRACE("%s:%d byte 0x%x\n", __func__, __LINE__, byte);

    switch (p_cb->rcv_state)
    {
        case H4_RX_MSGTYPE_ST:
            if(byte < HCI_COMMAND_DATA_PACKET || byte > HCI_EVENT_PACKET) {
                TRACE("wrong type 0x%x, Ignore", byte);
                return 0;
            }

            /* Initialize rx parameters */
            p_cb->rcv_msg_type = byte;
            p_cb->rcv_len = hci_preamble_table[byte-1];
            memset(p_cb->preload_buffer, 0 , 6);
            p_cb->preload_count = 0;
            p_cb->p_rcv_msg = NULL;
            p_cb->rcv_state = H4_RX_LEN_ST; /* Next, wait for length to come */
            break;

        case H4_RX_LEN_ST:
            /* Receiving preamble */
            p_cb->preload_buffer[p_cb->preload_count++] = byte;
            p_cb->rcv_len--;

            /* Check if we received entire preamble yet */
            if (p_cb->rcv_len == 0)
            {
                if (p_cb->rcv_msg_type == HCI_ACL_DATA_PACKET)
                {
                    /* ACL data lengths are 16-bits */
                    msg_len = p_cb->preload_buffer[3];
                    msg_len = (msg_len << 8) + p_cb->preload_buffer[2];
                }
                else {
                    /* other type */
                    msg_len = byte;
                }

                p_cb->rcv_len = msg_len;
                len = msg_len + p_cb->preload_count;
                p_cb->p_rcv_msg = (HC_BT_HDR*)&temp_buf;

                if (p_cb->p_rcv_msg)
                {
                    /* Initialize buffer with preloaded data */
                    p_cb->p_rcv_msg->offset = 0;
                    p_cb->p_rcv_msg->layer_specific = 0;
                    p_cb->p_rcv_msg->len = p_cb->preload_count;

                    OS_StopHardware();
                    if(p_cb->rcv_msg_type == HCI_EVENT_PACKET) {
                        HCI_GetEventBuffer(&p_buf, len);
                        p_buf->flags = HCI_BUFTYPE_EVENT;
                    }
                    else if(p_cb->rcv_msg_type == HCI_ACL_DATA_PACKET) {
                        HCI_GetAclBuffer(&p_buf, 0, len);
                        p_buf->flags = HCI_BUFTYPE_ACL_DATA;
                    }
                    OS_ResumeHardware();

                    p_buf->len = len;
                    memcpy((unsigned char *)(p_buf->buffer), p_cb->preload_buffer, p_cb->preload_count);
                }

                if (p_cb->p_rcv_msg == NULL)
                {
                    /* Unable to acquire message buffer. */
                    if (msg_len == 0)
                    {
                        /* Wait for next message */
                        p_cb->rcv_state = H4_RX_MSGTYPE_ST;
                    }
                    else
                    {
                        /* Ignore rest of the packet */
                        p_cb->rcv_state = H4_RX_IGNORE_ST;
                    }
                    break;
                }

                /* Message length is valid */
                if (msg_len)
                {
                    /* Read rest of message */
                    p_cb->rcv_state = H4_RX_DATA_ST;
                }
                else
                {
                    msg_received = TRUE;
                    /* Next, wait for next message */
                    p_cb->rcv_state = H4_RX_MSGTYPE_ST;
                }
            }
            break;

        case H4_RX_DATA_ST:
            if (p_cb->rcv_len > 0)
            {
                /* Read in the rest of the message */
                *(((unsigned char *)(p_buf->buffer)) + p_cb->p_rcv_msg->len) = byte;
                p_cb->p_rcv_msg->len += 1;
                p_cb->rcv_len -= 1;
            }

            /* Check if we read in entire message yet */
            if (p_cb->rcv_len == 0)
            {
                msg_received = TRUE;
                /* Next, wait for next message */
                p_cb->rcv_state = H4_RX_MSGTYPE_ST;
            }
            break;

        case H4_RX_IGNORE_ST:
            /* Ignore reset of packet */
            p_cb->rcv_len--;

            /* Check if we read in entire message yet */
            if (p_cb->rcv_len == 0)
            {
                /* Next, wait for next message */
                p_cb->rcv_state = H4_RX_MSGTYPE_ST;
            }
            break;
    }

    /* If we received entire message, then send it to the task */
    if (msg_received)
    {
        OS_StopHardware();
        HCI_RxBufferInd(p_buf, BT_STATUS_SUCCESS);
        OS_ResumeHardware();
        p_cb->p_rcv_msg = NULL;
    }

    return 0;
}
#else
static unsigned short INTERSYSHCI_LOC hci_h4_receive_msg( const uint8_t *buf, uint32_t size)
{
    static unsigned char temp_buf[64];
    static RxBuffHandle p_buf;

    BtStatus status = BT_STATUS_NO_RESOURCES;
    uint32_t bytes_read = 0;
    unsigned short    msg_len = 0, len = 0;
    unsigned char     msg_received = 0, byte = 0;
    uint32_t cnt = 0;

    msg_received = FALSE;

    p_cb = &h4_pcb;

    bytes_read = 0;

    while (bytes_read < size) {

        msg_received = FALSE;
        byte = buf[bytes_read];

        switch (p_cb->rcv_state)
        {
            case H4_RX_MSGTYPE_ST:
                if(byte < HCI_COMMAND_DATA_PACKET || byte > HCI_EVENT_PACKET) {
                    TRACE("wrong type 0x%x, Ignore", byte);
                    return 0;
                }

                /* Initialize rx parameters */
                p_cb->rcv_msg_type = byte;
                p_cb->rcv_len = hci_preamble_table[byte-1];
                memset(p_cb->preload_buffer, 0 , 6);
                p_cb->preload_count = 0;
                p_cb->p_rcv_msg = NULL;
                p_cb->rcv_state = H4_RX_LEN_ST; /* Next, wait for length to come */

                ++bytes_read;
                break;

            case H4_RX_LEN_ST:
                /* Receiving preamble */
                p_cb->preload_buffer[p_cb->preload_count++] = byte;
                p_cb->rcv_len--;

                ++bytes_read;

                /* Check if we received entire preamble yet */
                if (p_cb->rcv_len == 0)
                {
                    if (p_cb->rcv_msg_type == HCI_ACL_DATA_PACKET)
                    {
                        /* ACL data lengths are 16-bits */
                        msg_len = p_cb->preload_buffer[3];
                        msg_len = (msg_len << 8) + p_cb->preload_buffer[2];
                    }
                    else {
                        /* other type */
                        msg_len = byte;
                    }

                    p_cb->rcv_len = msg_len;
                    len = msg_len + p_cb->preload_count;
                    p_cb->p_rcv_msg = (HC_BT_HDR*)&temp_buf;

                    if (p_cb->p_rcv_msg)
                    {
                        /* Initialize buffer with preloaded data */
                        p_cb->p_rcv_msg->offset = 0;
                        p_cb->p_rcv_msg->layer_specific = 0;
                        p_cb->p_rcv_msg->len = p_cb->preload_count;
                        if(p_cb->rcv_msg_type == HCI_EVENT_PACKET) {
                            //TRACE("hci evt\n");
#ifdef _INTERSYS_NO_THREAD_
                            status = HCI_GetEventBuffer(&p_buf, len);
#else
                            do {
                                OS_StopHardware();
                                status = HCI_GetEventBuffer(&p_buf, len);
                                OS_ResumeHardware();

                                if(status == BT_STATUS_SUCCESS){
                                    p_buf->flags = HCI_BUFTYPE_EVENT;
                                    status = BT_STATUS_SUCCESS;
                                    break;
                                }else{
                                    status = BT_STATUS_NO_RESOURCES;
                                    osDelay(1);
                                }
                            }while(++cnt<BT_THREAD_BUFFER_RETRY_CNT);
#endif                            
                        }else if(p_cb->rcv_msg_type == HCI_ACL_DATA_PACKET) {
                            //TRACE("hci acl\n");
#ifdef _INTERSYS_NO_THREAD_
                            status = HCI_GetAclBuffer(&p_buf, 0, len);
#else
                            do {
                                OS_StopHardware();
                                status = HCI_GetAclBuffer(&p_buf, 0, len);
                                OS_ResumeHardware();
                                if(status == BT_STATUS_SUCCESS){
                                    p_buf->flags = HCI_BUFTYPE_ACL_DATA;
                                    status = BT_STATUS_SUCCESS;
                                    break;
                                }else{
                                    status = BT_STATUS_NO_RESOURCES;
                                    osDelay(1);
                                }
                            }while(++cnt<BT_THREAD_BUFFER_RETRY_CNT);
#endif
                        }else if(p_cb->rcv_msg_type == HCI_SCO_DATA_PACKET) {
#ifdef _INTERSYS_NO_THREAD_
                            status = HCI_GetScoBuffer(&p_buf, 0, len);
#else
                            do {
                                OS_StopHardware();
                                status = HCI_GetScoBuffer(&p_buf, 0, len);
                                OS_ResumeHardware();

                                if(status == BT_STATUS_SUCCESS){
                                    p_buf->flags = HCI_BUFTYPE_SCO_DATA;
                                    status = BT_STATUS_SUCCESS;
                                    break;
                                }else{
                                    status = BT_STATUS_NO_RESOURCES;
                                    osDelay(1);
                                }
                            }while(++cnt<BT_THREAD_BUFFER_RETRY_CNT);
#endif
                        }
                        if (status == BT_STATUS_NO_RESOURCES){
                            if(p_cb->rcv_msg_type == HCI_EVENT_PACKET) {
                                TRACE("NO EVENT BUFF");
                                ASSERT(0, "hci_h4_receive_msg type:%d", p_cb->rcv_msg_type, cnt);
                            }else if(p_cb->rcv_msg_type == HCI_ACL_DATA_PACKET) {
                                TRACE("NO ACL BUFF");
                                ASSERT(0, "hci_h4_receive_msg type:%d", p_cb->rcv_msg_type, cnt);
                            }else if(p_cb->rcv_msg_type == HCI_SCO_DATA_PACKET) {
                                TRACE("NO SCO BUFF");
                                //ASSERT(0, "hci_h4_receive_msg type:%d", p_cb->rcv_msg_type, cnt);
                                p_cb->p_rcv_msg = NULL;
                                //TRACE("bytes_read=%x,size=%x,p_cb->rcv_len=%x\n",bytes_read,size,p_cb->rcv_len);
                            }
                        }else{
                            p_buf->len = len;
                            memcpy((unsigned char *)(p_buf->buffer), p_cb->preload_buffer, p_cb->preload_count);
                        }
                    }

                    if (p_cb->p_rcv_msg == NULL)
                    {
                        /* Unable to acquire message buffer. */
                        if (msg_len == 0)
                        {
                            /* Wait for next message */
                            p_cb->rcv_state = H4_RX_MSGTYPE_ST;
                        }
                        else
                        {
                            /* Ignore rest of the packet */
                            p_cb->rcv_state = H4_RX_IGNORE_ST;
                            TRACE(" p_cb->rcv_state = H4_RX_IGNORE_ST\n");

                        }
                        break;
                    }

                    /* Message length is valid */
                    if (msg_len)
                    {
                        /* Read rest of message */
                        p_cb->rcv_state = H4_RX_DATA_ST;
                    }
                    else
                    {
                        msg_received = TRUE;
                        /* Next, wait for next message */
                        p_cb->rcv_state = H4_RX_MSGTYPE_ST;
                    }
                }
                break;

            case H4_RX_DATA_ST:
                /* Read in the rest of the message */
                *(((unsigned char *)(p_buf->buffer)) + p_cb->p_rcv_msg->len) = byte;
                p_cb->p_rcv_msg->len += 1;
                p_cb->rcv_len -= 1;
                ++bytes_read;

                if (p_cb->rcv_len > 0)
                {
                    /* Read in the rest of the message */
                    len = size - bytes_read;
                    if(len > p_cb->rcv_len)
                        len = p_cb->rcv_len;

                    memcpy(((unsigned char *)(p_buf->buffer)) + p_cb->p_rcv_msg->len,
                            buf + bytes_read, len);
                    p_cb->p_rcv_msg->len += len;
                    p_cb->rcv_len -= len;
                    bytes_read += len;
                }

                /* Check if we read in entire message yet */
                if (p_cb->rcv_len == 0)
                {
                    msg_received = TRUE;
                    /* Next, wait for next message */
                    p_cb->rcv_state = H4_RX_MSGTYPE_ST;
                }
                break;

            case H4_RX_IGNORE_ST:
                /* Ignore reset of packet */
                p_cb->rcv_len--;
                ++bytes_read;
               // TRACE(" H4_RX_IGNORE_ST\n");
                /* Check if we read in entire message yet */
                if (p_cb->rcv_len == 0)
                {
                    /* Next, wait for next message */
                    p_cb->rcv_state = H4_RX_MSGTYPE_ST;
                }
                break;
        }

        /* If we received entire message, then send it to the task */
        if (msg_received)
        {
#ifndef _INTERSYS_NO_THREAD_
            OS_StopHardware();
#endif
            HCI_RxBufferInd(p_buf, BT_STATUS_SUCCESS);
#ifndef _INTERSYS_NO_THREAD_
            OS_ResumeHardware();
#endif
            p_cb->p_rcv_msg = NULL;
        }

    }

    return bytes_read;
}
#endif

#if !defined(_INTERSYS_NO_THREAD_)&&defined(__rx_queue_support__)
void INTERSYSHCI_LOC intersys_rx_thread(void const *argument)
{
    int nRet = 0;
    while (1) {
        osEvent  evt;
        uint32_t len;
        unsigned char *e1 = NULL, *e2 = NULL;
        unsigned int len1 = 0, len2 = 0;
        unsigned short usedlen = 0;
        uint32_t lock;

        app_sysfreq_req(APP_SYSFREQ_USER_APP_2, APP_SYSFREQ_32K);
        evt = osMessageGet(intersys_rx_queue_id, osWaitForever);
        app_sysfreq_req(APP_SYSFREQ_USER_APP_2, APP_SYSFREQ_52M);

        if (evt.status == osEventMessage) {
            len = (uint32_t)evt.value.p;
            lock = int_lock();
            nRet = PeekCQueue(&intersys_rx_queue, len, &e1, &len1, &e2, &len2);
            int_unlock(lock);
            ASSERT(nRet == CQ_OK, "rx_thread PeekCQueue error");

            if (len1 && !len2){
#ifdef _INTERSYS_DATA_TRACE_
                TRACE("[RX]:");
                DUMP8("%02x ",e1,len1);
#endif
                usedlen = hci_h4_receive_msg(e1, len1);
                lock = int_lock();
                DeCQueue(&intersys_rx_queue, 0, usedlen);
                int_unlock(lock);
                ASSERT(usedlen == len1, "rx_thread DeCQueue1 error");
            }else if (!len1 && len2){
#ifdef _INTERSYS_DATA_TRACE_
                TRACE("[RX]:");
                DUMP8("%02x ",e2,len2);
#endif
                usedlen = hci_h4_receive_msg(e2, len2);
                lock = int_lock();
                DeCQueue(&intersys_rx_queue, 0, usedlen);
                int_unlock(lock);
                ASSERT(usedlen == len2, "rx_thread DeCQueue2 error");
            }else if (len1 && len2){
#ifdef _INTERSYS_DATA_TRACE_
                TRACE("[RX]:");
                DUMP8("%02x ",e1,len1);
#endif
                usedlen = hci_h4_receive_msg(e1, len1);
                lock = int_lock();
                DeCQueue(&intersys_rx_queue, 0, usedlen);
                int_unlock(lock);
                ASSERT(usedlen == len1, "rx_thread DeCQueue1 error");

#ifdef _INTERSYS_DATA_TRACE_
                TRACE("[RX]:");
                DUMP8("%02x ",e2,len2);
#endif
                usedlen = hci_h4_receive_msg(e2, len2);
                lock = int_lock();
                DeCQueue(&intersys_rx_queue, 0, usedlen);
                int_unlock(lock);
                ASSERT(usedlen == len2, "rx_thread DeCQueue2 error");
            }else{
                //do nothing
            }
        }
    }
}
#endif

static char hci_senddata_buf[1024];
void BESHCI_SendData(void *packet)
{
    HciPacket *pkt = NULL;
    unsigned int index = 0, j = 0;

    pkt = (HciPacket*)packet;

    if(pkt->flags == HCI_BUFTYPE_COMMAND) {
        hci_senddata_buf[0] = HCI_COMMAND_DATA_PACKET;
        memcpy(hci_senddata_buf + 1, pkt->header, 3);
        index = 4;
    }
    else if(pkt->flags == HCI_BUFTYPE_ACL_DATA) {
        hci_senddata_buf[0] = HCI_ACL_DATA_PACKET;
        memcpy(hci_senddata_buf + 1, pkt->header, 4);
        index = 5;
    }
    else if(pkt->flags == HCI_BUFTYPE_SCO_DATA) {
        hci_senddata_buf[0] = HCI_SCO_DATA_PACKET;
        memcpy(hci_senddata_buf + 1, pkt->header, 3);
        index = 4;
    }
    else {
        return;
    }

    for(j = 0; j < pkt->fCount; ++j) {
        memcpy(hci_senddata_buf + index, pkt->f[j].buffer, pkt->f[j].len);
        index += pkt->f[j].len;
    }
#ifdef _INTERSYS_DATA_TRACE_
    if (pkt->flags == HCI_BUFTYPE_SCO_DATA) {
        TRACE("send sco len %d\n", index);
    }

    if (pkt->flags == HCI_BUFTYPE_ACL_DATA || pkt->flags == HCI_BUFTYPE_COMMAND) {
        TRACE("[TX]:");
     	DUMP8("%02x ",hci_senddata_buf,index);
    }
#endif
    if (pkt->flags == HCI_BUFTYPE_COMMAND) {
        //TRACE("cmd\n");
        //for (int k = 0; k < index; ++k) {
        //    TRACE("0x%x - ", hcibuf[k]);
        //}
        //TRACE("\n");
    }

    intersys_sleep_checker_ping();
    osSignalClear(intersys_tx_thread_id, 0x1);
    hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, (uint8_t *)hci_senddata_buf, index);
    osSignalWait(0x1, osWaitForever);

    HciPacketSent(pkt);
}

#ifdef _INTERSYS_NO_THREAD_
static unsigned int INTERSYSHCI_LOC bt_rx(const unsigned char *data, unsigned int len)
{
    intersys_rx_buf.data = data;
    intersys_rx_buf.len = len;
    osMessagePut(intersys_rx_queue_id, 0, 0);
    OS_NotifyEvm();
    return len;
}
#else
static unsigned int INTERSYSHCI_LOC bt_rx(const unsigned char *data, unsigned int len)
{
    int nRet = 0;
    uint32_t lock = int_lock();
#if FPGA==1
    unsigned int available_len = 0;
    unsigned int length = 0;
    available_len = AvailableOfCQueue(&intersys_rx_queue);
    length = LengthOfCQueue(&intersys_rx_queue);

    if(len > available_len || (available_len < intersys_rx_queue.size/3))
    {
        if(*data == 0x03)
        {
            int_unlock(lock);
            hal_intersys_start_recv(HAL_INTERSYS_ID_0);
            return len;

         }
    }
#endif
    nRet = EnCQueue(&intersys_rx_queue, (unsigned char *)data, len);
    ASSERT(nRet == CQ_OK, "intersys_rx_queue overflow input:%d available:%d length:%d", len, AvailableOfCQueue(&intersys_rx_queue), LengthOfCQueue(&intersys_rx_queue));
    int_unlock(lock);
    osMessagePut(intersys_rx_queue_id, len, 0);
    return len;
}
#endif

static uint32_t BESHCI_tx_flag = 0;
static void bt_tx(const unsigned char *data, unsigned int len)
{
    if(BESHCI_tx_flag != 4)
    {
        osSignalSet(intersys_tx_thread_id, 0x1);
    }
    else
    {
        BESHCI_tx_flag = 0;
        osSignalSet(intersys_tx_thread_id, 0x2);
    }
}

void BESHCI_BufferAvai(void *packet)
{
}

static uint8_t hci_ble_acl_buf[1024];
int BESHCI_SendBuffer(U8 packet_type, U8 *packet, int size)
{
    uint8_t* buf;
    buf = hci_ble_acl_buf;
    buf[0] = packet_type;
    memcpy(&buf[1], packet, size);
    //OS_StopHardware();
#ifdef _INTERSYS_DATA_TRACE_
    TRACE("[TX2]:");
    DUMP8("%02x ",buf,size + 1);
#endif
    BESHCI_tx_flag = 4;
    osSignalClear(intersys_tx_thread_id, 0x2);

    hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, buf, size + 1);
#ifndef _INTERSYS_NO_THREAD_
    osSignalWait(0x2, osWaitForever);
#endif
    //OS_ResumeHardware();
    return 0;

}

void BESHCI_Open(void)
{
    int ret = 0;
    
    Besbt_hook_handler_set(BESBT_HOOK_USER_0, intersys_sleep_checker);

    intersys_tx_thread_id = osThreadGetId();
    g_hcibuffer_mutex_id = osMutexCreate((osMutex(g_hcibuffer_mutex)));
    TRACE("intersys_tx_thread_id %x\n", intersys_tx_thread_id);
    intersys_rx_queue_id =  osMessageCreate(osMessageQ(intersys_rx_queue_msg), NULL);
    if (intersys_rx_queue_id == NULL)  {
        TRACE("Failed to Create intersys_rx_queue_id");
    }

#if defined(__rx_queue_support__)
    InitCQueue(&intersys_rx_queue, INTERSYS_RX_BUF_MAX, (unsigned char *)&intersys_rx_queue_buf);
#endif
#ifndef _INTERSYS_NO_THREAD_
    intersys_rx_thread_id = osThreadCreate(osThread(intersys_rx_thread), NULL);
    TRACE("intersys_rx_thread_id %x\n", intersys_rx_thread_id);
    ret = hal_intersys_open(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, bt_rx, bt_tx, false);
#else
    intersys_rx_buf.data = 0;
    intersys_rx_buf.len = 0;
    ret = hal_intersys_open(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, bt_rx, bt_tx, true);
#endif
    if (ret) {
        TRACE("Failed to open intersys");
        return;
    }
    hal_intersys_start_recv(HAL_INTERSYS_ID_0);

}


void BESHCI_Close(void)
{
    hal_intersys_close(HAL_INTERSYS_ID_0,HAL_INTERSYS_MSG_HCI);
}

#ifdef _INTERSYS_NO_THREAD_
#if defined(__rx_queue_support__)
void INTERSYSHCI_LOC BESHCI_Poll(void)
{
    unsigned int length = 0;
    int nRet = 0;
    osEvent  evt;
    unsigned short usedlen = 0;
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    length = LengthOfCQueue(&intersys_rx_queue);

    evt = osMessageGet(intersys_rx_queue_id, 0);
    if (evt.status == osEventMessage) {
        if (!length){

        #ifdef _INTERSYS_DATA_TRACE_
            TRACE("[RX]:");
            DUMP8("%02x ",intersys_rx_buf.data,(intersys_rx_buf.len > 32?32:intersys_rx_buf.len));
        #endif        
            usedlen = hci_h4_receive_msg(intersys_rx_buf.data, intersys_rx_buf.len);            
            ASSERT(usedlen <= intersys_rx_buf.len, "usedlen > intersys_rx_buf.len");
            nRet = EnCQueue(&intersys_rx_queue, (unsigned char *)(intersys_rx_buf.data+usedlen), intersys_rx_buf.len-usedlen);
            ASSERT(nRet == CQ_OK, "intersys_rx_queue overflow input:%d available:%d length:%d", intersys_rx_buf.len, AvailableOfCQueue(&intersys_rx_queue), LengthOfCQueue(&intersys_rx_queue));
        }else{
            nRet = EnCQueue(&intersys_rx_queue, (unsigned char *)intersys_rx_buf.data, intersys_rx_buf.len);
            ASSERT(nRet == CQ_OK, "intersys_rx_queue overflow input:%d available:%d length:%d", intersys_rx_buf.len, AvailableOfCQueue(&intersys_rx_queue), LengthOfCQueue(&intersys_rx_queue));
            nRet = PeekCQueue(&intersys_rx_queue, length, &e1, &len1, &e2, &len2);
            ASSERT(nRet == CQ_OK && !len2, "rx_thread PeekCQueue error");
            usedlen = hci_h4_receive_msg(e1, len1);
            DeCQueue(&intersys_rx_queue, 0, usedlen);
        }
        ASSERT(usedlen == intersys_rx_buf.len, "intersys_rx_thread error");
        intersys_rx_buf.data=0;
        intersys_rx_buf.len=0;
        hal_intersys_rx_done(HAL_INTERSYS_ID_0);
    }else if (length){
        nRet = PeekCQueue(&intersys_rx_queue, length, &e1, &len1, &e2, &len2);
        ASSERT(nRet == CQ_OK && !len2, "rx_thread PeekCQueue error");
        usedlen = hci_h4_receive_msg(e1, len1);
        DeCQueue(&intersys_rx_queue, 0, usedlen);
    }
}
#else
void INTERSYSHCI_LOC BESHCI_Poll(void)
{
    osEvent  evt;
    unsigned short usedlen = 0;
    evt = osMessageGet(intersys_rx_queue_id, 0);
    if (evt.status == osEventMessage) {
        usedlen = hci_h4_receive_msg(intersys_rx_buf.data, intersys_rx_buf.len);
        ASSERT(usedlen == intersys_rx_buf.len, "intersys_rx_thread error");
        hal_intersys_rx_done(HAL_INTERSYS_ID_0);
    }
}
#endif
#else
void INTERSYSHCI_LOC BESHCI_Poll(void)
{
}
#endif

void BESHCI_SCO_Data_Start(void)
{
}
void BESHCI_SCO_Data_Stop(void)
{
}
void BESHCI_LockBuffer(void)
{
    osMutexWait(g_hcibuffer_mutex_id, osWaitForever);
}
void BESHCI_UNLockBuffer(void)
{
    osMutexRelease(g_hcibuffer_mutex_id);
}
