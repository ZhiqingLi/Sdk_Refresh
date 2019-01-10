#include "cmsis_os.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "cmsis_nvic.h"
#include "tool_msg.h"
#include "hal_timer_raw.h"
#include "hal_uart.h"
#include "hal_cmu.h"
#include "hal_bootmode.h"
#include "hal_iomux.h"
#include "pmu.h"
#include "app_thread.h"
#include "communication_sysapi.h"


#define FLASH_BASE                              0x6C000000
#define FLASH_TO_FLASHX(d)                      ((uint32_t)(d) & 0x0FFFFFFF)

#define WAIT_TRACE_TIMEOUT                      MS_TO_TICKS(200)

static uint32_t send_timeout = MS_TO_TICKS(500);

static const struct HAL_UART_CFG_T uart_cfg = {
    HAL_UART_PARITY_NONE,
#ifdef __KNOWLES
	HAL_UART_STOP_BITS_2,
#else
    HAL_UART_STOP_BITS_1,
#endif
    HAL_UART_DATA_BITS_8,
    HAL_UART_FLOW_CONTROL_NONE, // RTC/CTS pins might be unavailable for some chip packages
    HAL_UART_FIFO_LEVEL_1_2,
    HAL_UART_FIFO_LEVEL_1_2,
#ifdef __PC_CMD_UART__ 
	115200,
#else
    921600,
#endif
    true,
    false,
    false,
};
#ifdef __PC_CMD_UART__
static const enum HAL_UART_ID_T comm_uart = HAL_UART_ID_0;
#else
static const enum HAL_UART_ID_T comm_uart = HAL_UART_ID_1;
#endif
static volatile enum UART_DMA_STATE uart_dma_rx_state = UART_DMA_IDLE;
static volatile uint32_t uart_dma_rx_size = 0;
static bool uart_opened = false;


static volatile bool cancel_xfer = false;

static uint32_t xfer_err_time = 0;
static uint32_t xfer_err_cnt = 0;

#ifdef KNOWLES_UART_DATA

#include "audio_dump.h"
#include "kfifo.h"
#include "hal_location.h"
static osMutexId uart_audio_pcmbuff_mutex_id = NULL;
osMutexDef(uart_audio_pcmbuff_mutex);



typedef enum {
    PP_PING = 0,
    PP_PANG = 1
} FIR_PP_T;


#define PP_PINGPANG(v) \
    (v == PP_PING ? PP_PANG: PP_PING)

static FIR_PP_T pp_index=PP_PING;
static uint32_t uart_dma_interval;

#define knowles_uart_packet_len 640//20MS
static uint8_t audio_data_receive_buf[knowles_uart_packet_len*2];//ping_pong_buffer





#ifdef SYSTEM_USE_PSRAM
#include "hal_location.h"
#define AMA_STREAM_UART_FIFO_SIZE (1024*32)
PSRAM_BSS_LOC  static unsigned char ama_uart_fifo_buff[AMA_STREAM_UART_FIFO_SIZE];
#else
#define AMA_STREAM_UART_FIFO_SIZE (1024*16)
static unsigned char ama_uart_fifo_buff[AMA_STREAM_UART_FIFO_SIZE];
#endif
static struct kfifo ama_uart_fifo;

extern osThreadId knowles_uart_audio_tid;

void ama_uart_stream_fifo_init()
{
    if (uart_audio_pcmbuff_mutex_id == NULL)
        uart_audio_pcmbuff_mutex_id = osMutexCreate((osMutex(uart_audio_pcmbuff_mutex)));

	pp_index=PP_PING;

	osSignalClear(knowles_uart_audio_tid, UART_OUT_SIGNAL_ID);

	memset(audio_data_receive_buf,0,sizeof(audio_data_receive_buf));
	kfifo_init(&ama_uart_fifo, ama_uart_fifo_buff, sizeof(ama_uart_fifo_buff));
}

uint32_t ama_uart_get_fifo_data(uint8_t *buf)
{
	uint32_t avail = 0;
	avail = kfifo_len(&ama_uart_fifo);
    if (avail > 0) {
        if (avail <knowles_uart_packet_len){
            TRACE("%s: Invalid  UART STREAM SIZE: %d", __FUNCTION__, avail);
			return 0;
        }
        else{
            TRACE("UART_GET_len %d",knowles_uart_packet_len);
        	return kfifo_get(&ama_uart_fifo, buf, knowles_uart_packet_len);
        }
    }else{
		TRACE("uart no stream to get");
		return 0;
    }
}

uint32_t avil_len_of_the_fifo()
{
	return kfifo_len(&ama_uart_fifo);
}

extern "C" void OS_NotifyEvm(void);

void send_message()
{
	OS_NotifyEvm();
}
#endif
static void uart_event_notify(void)
{
    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODUAL_COMMUNICATION_SVR;
    msg.msg_body.message_Param0 = uart_dma_rx_state;
    msg.msg_body.message_Param1 = uart_dma_rx_size;
	
    app_mailbox_put(&msg);
}


#ifdef __KNOWLES

uint32_t kw_start_index = 0;
uint32_t kw_end_index = 0;

#ifdef KNOWLES_UART_DATA

#define FRAME_SIZE  (640)// Bytes
#define HEADER_SIZE	( 8 )
#define FOOTER_SIZE ( 12 )

uint32_t kw_curr_fc = 0;
uint32_t kw_end_fc = 0;
uint32_t kw_start_fc = 0;

int32_t kw_previous_fc = -1;
char prev_counter =-1;

bool footer_read_done = true;
short payload_length_to_read = 0;		   //holds number of stream header payload bytes to read
char header_length_to_read = HEADER_SIZE;	//holds number of header bytes to read
char footer_length_to_read = FOOTER_SIZE;


//local buffer to hold parsed data
uint8_t local_buf[ FRAME_SIZE ];
uint8_t outBuf[ FRAME_SIZE ];
int found = 0;


/*
inBuf		- Input Buffer
outBuf		- Output Buffer
inBufSize 	- inBuf size
found 		- true if complete packet is available on Outbuff
			- false complete packet is not available
*/
int knowles_stream_header_parser( uint8_t *inBuf, uint8_t *outBuf, size_t inBufSize, int * found )
{
	static char sync_byte[ 2 ] = { 0x12, 0x34 };		//stream header sync byte
	static char local_buf[ FRAME_SIZE ];				//local buffer to hold parsed data
	static unsigned int local_buf_ptr = 0;				//local buffer pointer

    static char temp_header[ HEADER_SIZE ];				//temp_header to hold the header
	static unsigned char temp_footer[FOOTER_SIZE];
	int i;

	unsigned int inBuf_ptr = 0;							//input buffer pointer
	char endpoint_id, samplerate, packet_counter;		//variables to hold header info
	short payload_length;								//variable to hold payload length


	while( inBuf_ptr < inBufSize )	// loop through complete inBuf
	{
		if( payload_length_to_read )	// if we have payload to read
		{
			//TRACE("payload length to read %d\n",payload_length_to_read);
			if ( ( inBuf_ptr + payload_length_to_read ) <= inBufSize )	//complete payload is available in inBuf
			{
				//TRACE("complete payload available\n");
				if( ( local_buf_ptr + payload_length_to_read ) < FRAME_SIZE ) //complete payload available in inBuf can fit in local buffer
				{
					//printf("complete payload can fit in local buf\n");
					memcpy( local_buf + local_buf_ptr, inBuf + inBuf_ptr, payload_length_to_read );
					inBuf_ptr += payload_length_to_read;
					local_buf_ptr += payload_length_to_read;
					payload_length_to_read = 0;
					memset( temp_header, 0 , HEADER_SIZE );
				}
				else //complete payload available in inBuf cannot fit in local buffer.
				{
					//TRACE("complete payload can not fit in local buf %d \n",inBuf_ptr);
					memcpy( local_buf + local_buf_ptr, inBuf + inBuf_ptr, ( FRAME_SIZE - local_buf_ptr ) );
					memcpy( outBuf, local_buf, FRAME_SIZE );
					( *found )++;
					inBuf_ptr += ( FRAME_SIZE - local_buf_ptr );
					payload_length_to_read -= ( FRAME_SIZE - local_buf_ptr );
					local_buf_ptr = 0;
					memcpy( local_buf + local_buf_ptr, inBuf + inBuf_ptr, payload_length_to_read );
					local_buf_ptr += payload_length_to_read;
					inBuf_ptr += payload_length_to_read;
					payload_length_to_read = 0;
					memset( temp_header, 0, HEADER_SIZE );
				}

				//complete payload is available so check for footer
				memset( temp_footer, 0, FOOTER_SIZE );
				footer_read_done = false;
			}
			else // if complete payload is not available in inBuf only (inBufSize - inBuf_ptr)
			{
				//TRACE("half payload available\n");
				if( ( local_buf_ptr + ( inBufSize - inBuf_ptr ) )  < FRAME_SIZE ) // if data in inBuff can fit in local buffer
				{
					//TRACE("half payload can fit in local buf %d\n",inBuf_ptr);
					memcpy( local_buf + local_buf_ptr, inBuf + inBuf_ptr, ( inBufSize - inBuf_ptr ) );
					local_buf_ptr += ( inBufSize - inBuf_ptr );
					payload_length_to_read -= ( ( inBufSize - inBuf_ptr ) );
					inBuf_ptr += ( inBufSize - inBuf_ptr );
				}
				else //if data in inBuff can not fit in local buffer
				{
					//TRACE("half payload can not fit in local buf %d \n",inBuf_ptr);
					memcpy( local_buf + local_buf_ptr, inBuf + inBuf_ptr, ( FRAME_SIZE - local_buf_ptr ) );
					memcpy( outBuf, local_buf, FRAME_SIZE );
					payload_length_to_read -= ( FRAME_SIZE - local_buf_ptr );
					inBuf_ptr += ( FRAME_SIZE - local_buf_ptr );
					local_buf_ptr = 0;
					memcpy( local_buf + local_buf_ptr, inBuf + inBuf_ptr, ( inBufSize - inBuf_ptr ) );
					payload_length_to_read -= ( inBufSize - inBuf_ptr );
					local_buf_ptr += ( inBufSize - inBuf_ptr );
					inBuf_ptr += ( inBufSize - inBuf_ptr );
					( *found )++;
					//TRACE("payload length to read %d ibuf ptr %d\n",payload_length_to_read,inBuf_ptr);
				}
			}
		}
		else //we don't have payload to read, check for header
		{

			if(!footer_read_done)
			{
				//TRACE("footer_length_to_read %d\n",footer_length_to_read);
				if( (inBuf_ptr + footer_length_to_read) < inBufSize) //complete footer available
				{
					memcpy( temp_footer + ( FOOTER_SIZE - footer_length_to_read ), inBuf + inBuf_ptr, footer_length_to_read );
					inBuf_ptr += footer_length_to_read;
					footer_length_to_read = FOOTER_SIZE;
					footer_read_done = true;

					kw_curr_fc = (short) temp_footer[3];
					kw_curr_fc <<= 8;
					kw_curr_fc |= (short) temp_footer[2];
					kw_curr_fc <<= 8;
					kw_curr_fc |= (short) temp_footer[1];
					kw_curr_fc <<= 8;
					kw_curr_fc |= (short) temp_footer[0];

					kw_end_fc = (short) temp_footer[7];
					kw_end_fc <<= 8;
					kw_end_fc |= (short) temp_footer[6];
					kw_end_fc <<= 8;
					kw_end_fc |= (short) temp_footer[5];
					kw_end_fc <<= 8;
					kw_end_fc |= (short) temp_footer[4];

					kw_start_fc = (short) temp_footer[11];
					kw_start_fc <<= 8;
					kw_start_fc |= (short) temp_footer[10];
					kw_start_fc <<= 8;
					kw_start_fc |= (short) temp_footer[9];
					kw_start_fc <<= 8;
					kw_start_fc |= (short) temp_footer[8];

					//DUMP8("%02x ",temp_footer,12);
					if(kw_curr_fc != (kw_previous_fc+1))
					{
						//TRACE("\n\n###### missing frame counter cur %d pre %d #######\n\n",kw_curr_fc,kw_previous_fc);
					}
					kw_previous_fc = kw_curr_fc;

					//TRACE( "#### < CUR_FC : END_FC : START_FC > = <%5d : %5d : %5d> ####\n", kw_curr_fc, kw_end_fc, kw_start_fc );

					kw_start_index = kw_start_fc * 16 * 16;
					kw_end_index = kw_end_fc * 16 * 16;

				}
				else
				{
					//TRACE("#####Got Half Footer available %d \n",inBuf_ptr);
					memcpy( temp_footer + ( FOOTER_SIZE - footer_length_to_read ), inBuf + inBuf_ptr, inBufSize - inBuf_ptr );
					footer_length_to_read -= (inBufSize - inBuf_ptr);
					inBuf_ptr += (inBufSize - inBuf_ptr);
					continue;
					//return 0;
				}

			}

			if( header_length_to_read != HEADER_SIZE ) //Check if we got half header in the previous frame
			{
				//TRACE("reading rest header %d\n",header_length_to_read);
				memcpy( temp_header + ( HEADER_SIZE - header_length_to_read ), inBuf + inBuf_ptr, header_length_to_read );
				inBuf_ptr += header_length_to_read;
				header_length_to_read = HEADER_SIZE;	//reset  header_length_to_read
				goto decode_header_bytes;				//we read complete header jump to decode
			}
			//printf("looking for header\n");
			do // look for sync bytes
			{
				temp_header[0] = temp_header[1];
				if( ( inBuf_ptr + 1 ) < inBufSize )
				{
					memcpy( &temp_header[1], inBuf+inBuf_ptr, 1 );
					inBuf_ptr += 1;
				}
				else
				{
					return 0;
				}
			}while ( !( ( temp_header[0] == sync_byte[0] ) && ( temp_header[1] == sync_byte[1] ) ) );

			// got the sync bytes
			header_length_to_read -= 2;

			if( ( inBuf_ptr + header_length_to_read ) < inBufSize ) //check remaining header is available in inBUf
			{
				//TRACE("got full Header\n");
				memcpy( ( temp_header + ( HEADER_SIZE - header_length_to_read ) ), inBuf+inBuf_ptr, header_length_to_read );
				inBuf_ptr += header_length_to_read;
				header_length_to_read = HEADER_SIZE;
			}
			else // only half header is available in inBuf
			{
				//TRACE("Half Header\n");
				memcpy( temp_header + ( HEADER_SIZE - header_length_to_read ), inBuf + inBuf_ptr, ( inBufSize - inBuf_ptr ) );
				header_length_to_read -= ( inBufSize - inBuf_ptr );
				inBuf_ptr += ( inBufSize - inBuf_ptr );
				//return 0;
				continue;
			}

	decode_header_bytes:
			/* Get the endpoint ID */
			endpoint_id = temp_header[2];

			/* Get the stream sample rate */
			samplerate = ( ( temp_header[3] & 0xF0 ) >> 4 );

			/* Get stream packet counter */
			packet_counter = temp_header[3] & 0x0F;

			if(packet_counter != ((prev_counter +1) % 16))
			{
				TRACE("\n\n###### missing Packets cur %d pre %d #######\n\n",packet_counter,prev_counter);
			}
			prev_counter = packet_counter;
			/* Get the stream payload length */
			payload_length = ( temp_header[5] << 8 ) | temp_header[4];

			switch ( payload_length )
			{
				case 80:
				case 128:
				case 160:
				case 256:
				case 512:
				case 320:
				case 480:
				case 640:
				case 1280:
					break;

				default:
					payload_length = 0;
			} // switch ( payload_length )
			payload_length_to_read = payload_length;
			//TRACE( "#### Frame found - <ID : SMPL_RATE : PKT # : LEN > = <%5d : %5d : %5d : %5d> ####\n", endpoint_id, samplerate, packet_counter, payload_length );
		}
	}
	return 0;
}
#endif
#endif




static void uart_rx_dma_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    // The DMA transfer has been cancelled
#ifndef KNOWLES_UART_DATA    
    if (uart_dma_rx_state != UART_DMA_START) {
        return;
    }
#endif
    uart_dma_rx_size = xfer_size;
    if (dma_error || status.FE || status.OE || status.PE || status.BE) {
        TRACE("UART-RX Error: dma_error=%d, status=0x%08x", dma_error, status.reg);
        uart_dma_rx_state = UART_DMA_ERROR;
#ifndef KNOWLES_UART_DATA
        uart_event_notify();
#endif
    } else {
        //TRACE("UART-RX OK: rx_size=%d", uart_dma_rx_size);
        uart_dma_rx_state = UART_DMA_DONE;		
#ifdef KNOWLES_UART_DATA
		int ret;
    	//TRACE("UART DMA INTERUPT TIME=%d",hal_sys_timer_get()-uart_dma_interval);
		uart_dma_interval=hal_sys_timer_get();

		//TRACE("DMA UART INDEX=%d",pp_index);
		
		knowles_stream_header_parser( (uint8_t *)&audio_data_receive_buf[pp_index*knowles_uart_packet_len], outBuf, FRAME_SIZE, &found );
		if(found)
		{
			ret = kfifo_put(&ama_uart_fifo, (uint8_t *)outBuf, uart_dma_rx_size);
		

			//ret = kfifo_put(&ama_uart_fifo, (uint8_t *)&audio_data_receive_buf[pp_index*knowles_uart_packet_len], uart_dma_rx_size);
			if (ret <= 0) {
				TRACE(false, "%s:WARNING !!! UART STREAM OVERFLOW", __FUNCTION__);
			}
			osSignalSet(knowles_uart_audio_tid, UART_OUT_SIGNAL_ID);
			DUMP8("%02x ",&audio_data_receive_buf[pp_index*knowles_uart_packet_len],10);
			//audio_dump_add_channel_data(0, (short *)Outbuffer,uart_dma_rx_size/2);
			//audio_dump_run();
			found = 0;
		}
		pp_index=PP_PINGPANG(pp_index);	

#else
		uart_event_notify();
#endif       
    }
}

static void uart_break_handler(void)
{
    TRACE("****** Handle break ******");

    cancel_xfer = true;
    hal_uart_stop_dma_recv(comm_uart);
    hal_uart_stop_dma_send(comm_uart);
    uart_dma_rx_state = UART_DMA_ERROR;
    uart_event_notify();
}

void recv_data_state_get(enum UART_DMA_STATE *state)
{
    *state = uart_dma_rx_state;
}

void recv_data_reset(void)
{
    union HAL_UART_IRQ_T mask;
    mask.reg = 0;
    mask.BE = 1;	
    hal_uart_irq_set_mask(comm_uart, mask);	
    uart_dma_rx_state = UART_DMA_IDLE;
    uart_event_notify();
}

void init_transport(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP POSSIBLY_UNUSED pinmux_uart1[] = {
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };
    union HAL_UART_IRQ_T mask;
    struct HAL_UART_CFG_T comm_uart_cfg;
#ifdef KNOWLES_UART_DATA
	//audio_dump_init(320, 1);
	ama_uart_stream_fifo_init();
#endif
    if (!uart_opened) {
#ifdef __PC_CMD_UART__		
		hal_iomux_set_uart0();
#else
		hal_iomux_set_uart1();
#endif
        memcpy(&comm_uart_cfg, &uart_cfg, sizeof(comm_uart_cfg));	
        hal_uart_open(comm_uart, &comm_uart_cfg);
        mask.reg = 0;
        mask.BE = 1;
        mask.FE = 1;
        mask.OE = 1;
        mask.PE = 1;
		
        hal_uart_irq_set_dma_handler(comm_uart, uart_rx_dma_handler, NULL, uart_break_handler);
        hal_uart_irq_set_mask(comm_uart, mask);
        uart_opened = true;
    }

    cancel_xfer = false;

    hal_uart_flush(comm_uart,0);
#ifdef KNOWLES_UART_DATA
	recv_data(&audio_data_receive_buf[0], knowles_uart_packet_len*2);


	kw_curr_fc = 0;
	kw_end_fc = 0;
	kw_start_fc = 0;
	kw_previous_fc = -1;
	prev_counter =-1;
	footer_read_done = true;
	payload_length_to_read = 0;
	header_length_to_read = HEADER_SIZE;	//holds number of header bytes to read
 	footer_length_to_read = FOOTER_SIZE;
	TRACE("#####resetting variables\n");

#endif
}


void deinit_transport(void)
{
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
	
    hal_uart_stop_dma_recv(comm_uart);//punith
    uart_dma_rx_state = UART_DMA_IDLE;
    hal_uart_irq_set_mask(comm_uart, mask);
    hal_uart_irq_set_dma_handler(comm_uart, NULL, NULL, NULL);
    hal_uart_close(comm_uart);


	hal_uart_flush(comm_uart,0);
    uart_opened = false;
	cancel_xfer = true;
}




void reinit_transport(void)
{
    union HAL_UART_IRQ_T mask;
    struct HAL_UART_CFG_T comm_uart_cfg;

    uart_opened = false;
    memcpy(&comm_uart_cfg, &uart_cfg, sizeof(comm_uart_cfg));
    hal_uart_open(comm_uart, &comm_uart_cfg);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;        
    hal_uart_irq_set_dma_handler(comm_uart, uart_rx_dma_handler, NULL, uart_break_handler);
    hal_uart_irq_set_mask(comm_uart, mask);
    uart_opened = true;

    cancel_xfer = false;

    hal_uart_flush(comm_uart,0);
}

void set_send_timeout(uint32_t timeout)
{
    send_timeout = MS_TO_TICKS(timeout);
}

int debug_read_enabled(void)
{
    return !!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_READ_ENABLED);
}

int debug_write_enabled(void)
{
    return !!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_WRITE_ENABLED);
}

static int uart_send_data(const unsigned char *buf, unsigned int len)
{
    uint32_t start;
    uint32_t sent = 0;

    start = hal_sys_timer_get();
    while (sent < len) {
        while (!cancel_xfer && !hal_uart_writable(comm_uart) &&
                hal_sys_timer_get() - start < send_timeout);
        if (cancel_xfer) {
            break;
        }
        if (hal_uart_writable(comm_uart)) {
            hal_uart_putc(comm_uart, buf[sent++]);
        } else {
            break;
        }
    }

    if (sent != len) {
        return 1;
    }

    return 0;
}

int send_data(const unsigned char *buf, unsigned int len)
{
    if (cancel_xfer) {
        return -1;
    }

    return uart_send_data(buf, len);
}


static struct HAL_DMA_DESC_T dma_desc[17];

static int uart_recv_data_dma(unsigned char *buf, unsigned int len, unsigned int expect)
{
    int ret;
    union HAL_UART_IRQ_T mask;
    
    unsigned int desc_cnt = ARRAY_SIZE(dma_desc);

    if (uart_dma_rx_state != UART_DMA_IDLE) {
        ret = -3;
        goto _no_state_exit;
    }

    uart_dma_rx_state = UART_DMA_START;
    uart_dma_rx_size = 0;

#ifdef KNOWLES_UART_DATA
	ret = hal_uart_dma_recv_pingpang(comm_uart, buf, expect, &dma_desc[0], &desc_cnt);
#else
	ret = hal_uart_dma_recv(comm_uart, buf, expect, &dma_desc[0], &desc_cnt);
#endif
	
    if (ret) {
        uart_dma_rx_state = UART_DMA_ERROR;
        goto err_exit;
    }

    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    //mask.RT = 1;
    hal_uart_irq_set_mask(comm_uart, mask);

_no_state_exit:
    return ret;
err_exit:
#ifndef KNOWLES_UART_DATA
    uart_event_notify();
#endif
    return -1;
}

int recv_data(unsigned char *buf, unsigned int len)
{
	TRACE("%s",__func__);
    if (cancel_xfer) {
        return -1;
    }
    return uart_recv_data_dma(buf, len, len);
}

int recv_data_dma(unsigned char *buf, unsigned int len, unsigned int expect)
{
    if (cancel_xfer) {
        return -1;
    }
    
    return uart_recv_data_dma(buf, len, expect);
}

static int uart_handle_error(void)
{
    TRACE("****** Send break ******");

    // Send break signal, to tell the peer to reset the connection
    hal_uart_break_set(comm_uart);
    osDelay(100);
    hal_uart_break_clear(comm_uart);

    return 0;
}

int handle_error(void)
{
    int ret = 0;
    uint32_t err_time;

    osDelay(200);

    if (!cancel_xfer) {
        ret = uart_handle_error();
    }

    xfer_err_cnt++;
    err_time = hal_sys_timer_get();
    if (err_time - xfer_err_time > MS_TO_TICKS(5000)) {
        xfer_err_cnt = 0;
    }
    xfer_err_time = err_time;
    if (xfer_err_cnt < 2) {
        osDelay(500);
    } else if (xfer_err_cnt < 5) {
        osDelay(1000);
    } else {
        osDelay(2000);
    }

    return ret;
}

static int uart_cancel_input(void)
{
    hal_uart_flush(comm_uart,0);
    return 0;
}

int cancel_input(void)
{
    return uart_cancel_input();
}

void system_reboot(void)
{
    hal_cmu_sys_reboot();
}

void system_shutdown(void)
{
    pmu_shutdown();
}

void system_set_bootmode(unsigned int bootmode)
{
    bootmode &= ~(HAL_SW_BOOTMODE_READ_ENABLED | HAL_SW_BOOTMODE_WRITE_ENABLED);
    hal_sw_bootmode_set(bootmode);
}

void system_clear_bootmode(unsigned int bootmode)
{
    bootmode &= ~(HAL_SW_BOOTMODE_READ_ENABLED | HAL_SW_BOOTMODE_WRITE_ENABLED);
    hal_sw_bootmode_clear(bootmode);
}

unsigned int system_get_bootmode(void)
{
    return hal_sw_bootmode_get();
}

void wait_trace_finished(void)
{
    uint32_t time;
    int idle_cnt = 0;

    time = hal_sys_timer_get();

    while (idle_cnt < 2 && hal_sys_timer_get() - time < WAIT_TRACE_TIMEOUT) {
        osDelay(10);
        idle_cnt = hal_trace_busy() ? 0 : (idle_cnt + 1);
    }
}

unsigned int get_current_time(void)
{
    return hal_sys_timer_get();
}

