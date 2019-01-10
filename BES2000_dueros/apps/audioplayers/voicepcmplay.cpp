#ifdef MBED
#include "mbed.h"
#endif
// Standard C Included Files
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtos.h"
#ifdef MBED
#include "SDFileSystem.h"
#endif
#include "cqueue.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_trace.h"

// BT
extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
}

/* mutex */
osMutexId g_voicepcm_queue_mutex_id;
osMutexDef(g_voicepcm_queue_mutex);

/* voicepcm queue */
#define VOICEPCM_TEMP_BUFFER_SIZE 2048
#define VOICEPCM_QUEUE_SIZE (VOICEPCM_TEMP_BUFFER_SIZE*4)
unsigned char voicepcm_queue_buf[VOICEPCM_QUEUE_SIZE];
CQueue voicepcm_queue;
static uint32_t ok_to_decode = 0;

#define LOCK_VOICEPCM_QUEUE() \
	osMutexWait(g_voicepcm_queue_mutex_id, osWaitForever)

#define UNLOCK_VOICEPCM_QUEUE() \
	osMutexRelease(g_voicepcm_queue_mutex_id)

static void copy_one_trace_to_two_track_16bits(uint16_t *src_buf, uint16_t *dst_buf, uint32_t src_len)
{
    uint32_t i = 0;
    for (i = 0; i < src_len; ++i) {
        dst_buf[i*2 + 0] = dst_buf[i*2 + 1] = src_buf[i];
    }
}

int store_voicepcm_buffer(unsigned char *buf, unsigned int len)
{
	LOCK_VOICEPCM_QUEUE();
	EnCQueue(&voicepcm_queue, buf, len);
    if (LengthOfCQueue(&voicepcm_queue) > VOICEPCM_TEMP_BUFFER_SIZE*2) {
        ok_to_decode = 1;
    }
	UNLOCK_VOICEPCM_QUEUE();

	return 0;
}

int decode_voicepcm_frame(unsigned char *pcm_buffer, unsigned int pcm_len)
{
	uint32_t r = 0, got_len = 0;
	unsigned char *e1 = NULL, *e2 = NULL;
	unsigned int len1 = 0, len2 = 0;
get_again:
	LOCK_VOICEPCM_QUEUE();
	r = PeekCQueue(&voicepcm_queue, (pcm_len - got_len)/2, &e1, &len1, &e2, &len2);
	UNLOCK_VOICEPCM_QUEUE();

	if(r == CQ_ERR || len1 == 0) {
		Thread::wait(2);
		goto get_again;
	}

    //memcpy(pcm_buffer + got_len, e1, len1);
    copy_one_trace_to_two_track_16bits((uint16_t *)e1, (uint16_t *)(pcm_buffer + got_len), len1/2);

    LOCK_VOICEPCM_QUEUE();
    DeCQueue(&voicepcm_queue, 0, len1);
    UNLOCK_VOICEPCM_QUEUE();

    got_len += len1*2;

    if (len2 != 0) {
        //memcpy(pcm_buffer + got_len, e2, len2);
        copy_one_trace_to_two_track_16bits((uint16_t *)e2, (uint16_t *)(pcm_buffer + got_len), len2/2);

        LOCK_VOICEPCM_QUEUE();
        DeCQueue(&voicepcm_queue, 0, len2);
        UNLOCK_VOICEPCM_QUEUE();
    }

    got_len += len2*2;

    if (got_len < pcm_len)
        goto get_again;

    return pcm_len;
}

uint32_t voicepcm_audio_data_come(uint8_t *buf, uint32_t len)
{
    TRACE("data come %d\n", len);

    return 0;
}

uint32_t voicepcm_audio_more_data(uint8_t *buf, uint32_t len)
{
    uint32_t l = 0;
    //uint32_t cur_ticks = 0, ticks = 0;

    if (ok_to_decode == 0)
        return 0;

    //ticks = hal_sys_timer_get();
    l = decode_voicepcm_frame(buf, len);
    //cur_ticks = hal_sys_timer_get();
//    TRACE("voicepcm %d t\n", (cur_ticks-ticks));

    return l;
}

int voicepcm_audio_init(void)
{
    g_voicepcm_queue_mutex_id = osMutexCreate((osMutex(g_voicepcm_queue_mutex)));
	/* voicepcm queue*/
	InitCQueue(&voicepcm_queue, VOICEPCM_QUEUE_SIZE, (unsigned char *)&voicepcm_queue_buf);

	return 0;
}
