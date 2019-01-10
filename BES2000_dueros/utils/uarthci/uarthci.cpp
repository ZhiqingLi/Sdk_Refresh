/***
 * uart hci
 * YuLongWang @2015
*/

#include <stdio.h>
#include "mbed.h"
#include "rtos.h"

#include "hal_uart.h"

extern "C"
{
#include "sys/hci.h"
#include "hcitrans.h"
}

#include "uarthci.h"

/* thread */
#define UART_STACK_SIZE 8192
static osThreadId uartrxtx_tid;
//uint32_t os_thread_def_stack_uartrxtx [UART_STACK_SIZE/ sizeof(uint32_t)] __attribute__((section("AHBSDRAM1"),aligned(32)));
uint32_t os_thread_def_stack_uartrxtx [UART_STACK_SIZE/ sizeof(uint32_t)];
osThreadDef_t os_thread_def_uartrxtx = { (uartrxtx), (osPriorityHigh), (UART_STACK_SIZE), (os_thread_def_stack_uartrxtx)};

typedef struct {
	unsigned char d[16]; // uart fifo is 16bytes
	char count;
} mail_t;

osMailQDef(mail_box, 1024, mail_t);
osMailQId  mail_box;

/* mutex */
osMutexId g_hcibuffer_mutex_id;
osMutexDef(g_hcibuffer_mutex);

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

static unsigned char temp_buf[64];
static unsigned char packet_buf[2048];

unsigned short hci_h4_receive_msg(unsigned char byte)
{
	static RxBuffHandle p_buf;
	unsigned short    msg_len = 0, len = 0;
	unsigned char     msg_received = 0;
	msg_received = FALSE;

	p_cb = &h4_pcb;

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

					BESHCI_LockBuffer();
					if(p_cb->rcv_msg_type == HCI_EVENT_PACKET) {
						HCI_GetEventBuffer(&p_buf, len);
						p_buf->flags = HCI_BUFTYPE_EVENT;
					}
					else if(p_cb->rcv_msg_type == HCI_ACL_DATA_PACKET) {
						HCI_GetAclBuffer(&p_buf, 0, len);
						p_buf->flags = HCI_BUFTYPE_ACL_DATA;
					}
					BESHCI_UNLockBuffer();

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
		BESHCI_LockBuffer();
		HCI_RxBufferInd(p_buf, BT_STATUS_SUCCESS);
		BESHCI_UNLockBuffer();
		p_cb->p_rcv_msg = NULL;
	}

	return 0;
}

int ch = 0;

void uartrxtx(void const *argument)
{
	int i = 0;
	osEvent evt;
	mail_t *mail = NULL;
	TRACE("uart hci rx thread\n");
	while(1) {
		evt = osMailGet(mail_box, osWaitForever);
		if (evt.status == osEventMail) {
			mail = (mail_t*)evt.value.p;
			for(i = 0; i < mail->count; ++i) {
				hci_h4_receive_msg(mail->d[i]);
			}
            osMailFree(mail_box, mail);
		}
	}
}

void uart_rx_irq_handler(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{
	int count = 0;
	mail_t *mail = NULL;

    if (status.RX || status.RT) {
        mail = (mail_t*)osMailAlloc(mail_box, 0);

        while(hal_uart_readable(HAL_UART_ID_1)) {
            mail->d[count] = hal_uart_getc(HAL_UART_ID_1);
            ++count;
        }

        mail->count = count;
        osMailPut(mail_box, mail);
    }
}

void BESHCI_SendData(void *packet)
{
	char hcibuf[2049];
	HciPacket *pkt = 0;
	unsigned int i = 0, index = 0, j = 0;

	pkt = (HciPacket*)packet;

	if(pkt->flags == HCI_BUFTYPE_COMMAND) {
        //TRACE("%s:%d\n", __func__, __LINE__);
		hcibuf[0] = HCI_COMMAND_DATA_PACKET;
		//serial.putc(hcibuf[0]);
		hal_uart_putc(HAL_UART_ID_1,hcibuf[0]);
		for(i = 0; i < 3; ++i)
			//serial.putc(pkt->header[i]);
			hal_uart_putc(HAL_UART_ID_1,pkt->header[i]);
		//memcpy(hcibuf + 1, pkt->header, 3);
		index = 4;
	}
	else if(pkt->flags == HCI_BUFTYPE_ACL_DATA) {
        //TRACE("%s:%d\n", __func__, __LINE__);
		hcibuf[0] = HCI_ACL_DATA_PACKET;
		//serial.putc(hcibuf[0]);
		hal_uart_putc(HAL_UART_ID_1,hcibuf[0]);
		for(i = 0; i < 4; ++i)
			//serial.putc(pkt->header[i]);
			hal_uart_putc(HAL_UART_ID_1,pkt->header[i]);
		//memcpy(hcibuf + 1, pkt->header, 4);
		index = 5;
	}
	else if(pkt->flags == HCI_BUFTYPE_SCO_DATA) {
        //TRACE("%s:%d\n", __func__, __LINE__);
		hcibuf[0] = HCI_SCO_DATA_PACKET;
		//serial.putc(hcibuf[0]);
		hal_uart_putc(HAL_UART_ID_1,hcibuf[0]);
		for(i = 0; i < 3; ++i)
			//serial.putc(pkt->header[i]);
			hal_uart_putc(HAL_UART_ID_1,pkt->header[i]);
		//memcpy(hcibuf + 1, pkt->header, 3);
		index = 4;
	}
	else {
		return;
	}

	for(j = 0; j < pkt->fCount; ++j) {
		//memcpy(hcibuf + index, pkt->f[j].buffer, pkt->f[j].len);
		for(i = 0; i < pkt->f[j].len; ++i)
			//serial.putc(pkt->f[j].buffer[i]);
			hal_uart_putc(HAL_UART_ID_1,pkt->f[j].buffer[i]);
		index += pkt->f[j].len;
	}

	//for(i = 0; i < index; ++i) {
//		serial.putc(hcibuf[i]);
//	}

	HciPacketSent(pkt);
}

void BESHCI_BufferAvai(void *packet)
{
}
void BESHCI_Open(void)
{
    union HAL_UART_IRQ_T mask;

    g_hcibuffer_mutex_id = osMutexCreate((osMutex(g_hcibuffer_mutex)));
    mail_box = osMailCreate(osMailQ(mail_box), NULL);
	uartrxtx_tid = osThreadCreate(osThread(uartrxtx), NULL);

    mask.RT = 1;
    mask.RX = 1;
    hal_uart_irq_set_mask(HAL_UART_ID_1, mask);
    hal_uart_irq_set_handler(HAL_UART_ID_1, uart_rx_irq_handler);
}
void BESHCI_Poll(void)
{
}
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
