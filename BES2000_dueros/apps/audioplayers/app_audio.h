#ifndef __APP_AUDIO_H__
#define __APP_AUDIO_H__

#include "app_bt_stream.h"
//should solve this problem
//#include "./../../utils/cqueue/cqueue.h"
//#include "cqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(A2DP_SCALABLE_ON)
#define SCALABLE_FRAME_SIZE (864)
#define SCALABLE_HEAD_SIZE	(4)	/* pick something big enough to hold a bunch of frames */
#define SCALABLE_READBUF_SIZE	(500)	/* pick something big enough to hold a bunch of frames */
#define SCALABLE_DECODER_SIZE (17456)	/* pick something big enough to hold a bunch of frames */
#endif

#define MAX_AUDIO_BUF_LIST 5
enum APP_AUDIO_CACHE_T {
    APP_AUDIO_CACHE_CACHEING= 0,
    APP_AUDIO_CACHE_OK,
    APP_AUDIO_CACHE_QTY,
};

#define APP_AUDIO_SET_MESSAGE(appevt, id, status) (appevt = (((uint32_t)id&0xffff)<<16)|(status&0xffff))
#define APP_AUDIO_GET_ID(appevt, id) (id = (appevt>>16)&0xffff)
#define APP_AUDIO_GET_STATUS(appevt, status) (status = appevt&0xffff)
#define APP_AUDIO_GET_AUD_ID(appevt, aud_id) (aud_id = appevt)
#define APP_AUDIO_GET_FREQ(appevt, freq) (freq = appevt)

#define APP_AUDIO_InitCQueue(a, b, c)   InitCQueue(a, b, c)
#define APP_AUDIO_AvailableOfCQueue(a)     AvailableOfCQueue(a)
#define APP_AUDIO_LengthOfCQueue(a)     LengthOfCQueue(a)
#define APP_AUDIO_PeekCQueue(a, b, c, d, e, f)   PeekCQueue(a, b, c, d, e, f)
#define APP_AUDIO_EnCQueue(a, b, c)     EnCQueue(a, b, c)
#define APP_AUDIO_DeCQueue(a, b, c)     DeCQueue(a, b, c)

void UNLOCK_APP_AUDIO_QUEUE();

void LOCK_APP_AUDIO_QUEUE();

int app_audio_mempool_init();

uint32_t app_audio_mempool_free_buff_size();

int app_audio_mempool_get_buff(uint8_t **buff, uint32_t size);

int app_capture_audio_mempool_init();
int app_capture_audio_mempool_get_buff(uint8_t **buff, uint32_t size);
int app_full_audio_mempool_init(void);


uint32_t app_audio_lr_balance(uint8_t *buf, uint32_t len, int8_t balance);

int app_audio_pcmbuff_init(uint8_t *buff, uint16_t len);

int app_audio_pcmbuff_length(void);

int app_audio_pcmbuff_put(uint8_t *buff, uint16_t len);

int app_audio_pcmbuff_get(uint8_t *buff, uint16_t len);

int app_audio_pcmbuff_discard(uint16_t len);

void app_audio_memcpy_16bit(int16_t *des, int16_t *src, int len);

void app_audio_memset_16bit(int16_t *des, int16_t val, int len);

int app_audio_sendrequest(uint8_t id, uint8_t status, uint32_t ptr);

int app_audio_sendrequest_param(uint8_t id, uint8_t status, uint32_t ptr, uint32_t param);

void app_audio_open(void);

void app_audio_close(void);

bool app_audio_list_append(APP_AUDIO_STATUS* aud_status);

bool app_audio_list_rmv_callback(APP_AUDIO_STATUS* status, APP_AUDIO_STATUS* list_status);

void app_audio_list_clear();

#ifdef __cplusplus
}
#endif

#endif//__FMDEC_H__
