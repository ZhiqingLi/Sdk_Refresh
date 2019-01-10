#include "cmsis_os.h"
#include "string.h"
#include "cqueue.h"
#include "list.h"

#include "hal_trace.h"
#include "hal_aud.h"

#include "resources.h"
#include "app_thread.h"
#include "app_audio.h"
#include "app_media_player.h"
#include "app_ring_merge.h"
#include "nvrecord.h"
#include <assert.h>

extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
}
#include "btapp.h"
#include "app_bt_media_manager.h"

//This buff is used as queue
#if (RAM_SIZE >= 0x40000)

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
#define APP_AUDIO_BUFFER_SIZE (1024 * 140)
#elif defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
#ifdef RECORD_LONELY
#ifdef A2DP_EQ_24BIT
#define APP_AUDIO_BUFFER_SIZE (1024 * 40)
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 32)
#endif
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 100)
#endif
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 100)
#endif
#else // RAM_SIZE
#ifdef APP_TEST_AUDIO
#define APP_AUDIO_BUFFER_SIZE (1024 * 10)
#elif defined(SPEECH_RX_NS)
#define APP_AUDIO_BUFFER_SIZE (1024 * 37)
#else
#define APP_AUDIO_BUFFER_SIZE (1024 * 32)
#endif
#endif // RAM_SIZE

#ifdef RECORD_LONELY
#define APP_CAPTURE_AUDIO_BUFFER_SIZE   (64*1024)//72
static uint8_t app_audio_buff[APP_AUDIO_BUFFER_SIZE + APP_CAPTURE_AUDIO_BUFFER_SIZE];
#else
static uint8_t app_audio_buff[APP_AUDIO_BUFFER_SIZE];
#endif


static uint32_t buff_size_used;
static bool app_audio_init = false;

#ifdef RECORD_LONELY


static uint8_t* app_capture_audio_buff = &app_audio_buff[APP_AUDIO_BUFFER_SIZE];
static uint32_t capture_audio_buff_size_used,audio_buff_size;

int app_capture_audio_mempool_init(void)//ai voice
{
    TRACE("init the app capture audio mem pool.");
    
    capture_audio_buff_size_used = 0;

    memset(app_capture_audio_buff, 0, APP_CAPTURE_AUDIO_BUFFER_SIZE);
    return 0;
}

uint32_t app_capture_audio_mempool_free_buff_size()
{
    return APP_CAPTURE_AUDIO_BUFFER_SIZE - capture_audio_buff_size_used;
}

int app_capture_audio_mempool_get_buff(uint8_t **buff, uint32_t size)
{
    uint32_t buff_size_free;

    buff_size_free = app_capture_audio_mempool_free_buff_size();

    if (size % 4){
        size = size + (4 - size % 4);
    }

    TRACE("Get capture buf, current free %d to allocate %d", buff_size_free, size);

    ASSERT(size <= buff_size_free, "[%s] size = %d > free size = %d", __func__, size, buff_size_free);

    *buff = app_capture_audio_buff + capture_audio_buff_size_used;

    capture_audio_buff_size_used += size;
    TRACE("Allocate %d, now used %d left %d", 
        size, capture_audio_buff_size_used, app_capture_audio_mempool_free_buff_size());

    return 0;
}

#endif


osPoolDef (app_audio_status_mempool, 20, APP_AUDIO_STATUS);
osPoolId   app_audio_status_mempool = NULL;

// control queue access
osMutexId g_app_audio_queue_mutex_id = NULL;
osMutexDef(g_app_audio_queue_mutex);

// control pcmbuff access
static CQueue app_audio_pcm_queue;
static osMutexId app_audio_pcmbuff_mutex_id = NULL;
osMutexDef(app_audio_pcmbuff_mutex);

#ifdef __AUDIO_QUEUE_SUPPORT__
//#define DEBUG_AUDIO
#ifdef DEBUG_AUDIO
#define debug_trace TRACE
#else
#define debug_trace(...)
#endif

typedef struct {
  list_t *audio_list;
}APP_AUDIO_CONFIG;

APP_AUDIO_CONFIG app_audio_conifg = {
    .audio_list = NULL
};

#endif

void LOCK_APP_AUDIO_QUEUE()
{
    osMutexWait(g_app_audio_queue_mutex_id, osWaitForever);
}

void UNLOCK_APP_AUDIO_QUEUE()
{
    osMutexRelease(g_app_audio_queue_mutex_id);
}

#ifdef RECORD_LONELY
int app_full_audio_mempool_init(void)//make a phone call
{
    TRACE("init the app full audio mem pool size %d.", sizeof(app_audio_buff));
    
    buff_size_used = 0;
    audio_buff_size = sizeof(app_audio_buff);

    //if do not memset, memset in app level
    memset(app_audio_buff, 0, sizeof(app_audio_buff));
    return 0;
}
#endif


int app_audio_mempool_init()//play music
{
    buff_size_used = 0;
#ifdef RECORD_LONELY
	audio_buff_size = APP_AUDIO_BUFFER_SIZE;
#endif
    //if do not memset, memset in app level
    memset(app_audio_buff, 0, APP_AUDIO_BUFFER_SIZE);

    return 0;
}

uint32_t app_audio_mempool_free_buff_size()
{
#ifdef RECORD_LONELY
    return audio_buff_size - buff_size_used;
#else
	return APP_AUDIO_BUFFER_SIZE - buff_size_used;
#endif
}

int app_audio_mempool_get_buff(uint8_t **buff, uint32_t size)
{
    uint32_t buff_size_free;

    buff_size_free = app_audio_mempool_free_buff_size();

    if (size % 4){
        size = size + (4 - size % 4);
    }

    ASSERT(size <= buff_size_free, "[%s] size = %d > free size = %d", __func__, size, buff_size_free);

    *buff = app_audio_buff + buff_size_used;

    buff_size_used += size;

    return 0;
}

uint32_t app_audio_lr_balance(uint8_t *buf, uint32_t len, int8_t balance)
{
	short *balance_buf=(short *)buf;
	uint32_t balance_len = len/2;
	float factor;

	ASSERT((balance >= -100) && (balance <= 100), "balance = %d is invalid!", balance);

	if(balance > 0)
	{
		//reduce L channel
		factor = 1 - 0.01 * balance;
		for(uint32_t i=0; i<balance_len;i+=2)
		{
			balance_buf[i] = (short) (factor * balance_buf[i]);
		}
	}
	else if(balance < 0)
	{
		//reduce R channel
		factor = 1 + 0.01 * balance;
		for(uint32_t i=0; i<balance_len;i+=2)
		{
			balance_buf[i+1] = (short) (factor * balance_buf[i+1]);
		}
	}
    return 0;
}

int app_audio_pcmbuff_init(uint8_t *buff, uint16_t len)
{
    if (app_audio_pcmbuff_mutex_id == NULL)
        app_audio_pcmbuff_mutex_id = osMutexCreate((osMutex(app_audio_pcmbuff_mutex)));

    if ((buff == NULL)||(app_audio_pcmbuff_mutex_id == NULL))
        return -1;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    InitCQueue(&app_audio_pcm_queue, len, buff);
    memset(buff, 0x00, len);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return 0;
}

int app_audio_pcmbuff_length(void)
{
    int len;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    len = LengthOfCQueue(&app_audio_pcm_queue);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return len;
}

int app_audio_pcmbuff_put(uint8_t *buff, uint16_t len)
{
    int status;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    status = EnCQueue(&app_audio_pcm_queue, buff, len);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return status;
}

int app_audio_pcmbuff_get(uint8_t *buff, uint16_t len)
{
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;
    int status;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    status = PeekCQueue(&app_audio_pcm_queue, len, &e1, &len1, &e2, &len2);
    if (len==(len1+len2)){
        memcpy(buff,e1,len1);
        memcpy(buff+len1,e2,len2);
        DeCQueue(&app_audio_pcm_queue, 0, len1);
        DeCQueue(&app_audio_pcm_queue, 0, len2);
    }else{
        memset(buff, 0x00, len);
        status = -1;
    }
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return status;
}

int app_audio_pcmbuff_discard(uint16_t len)
{
    int status;

    osMutexWait(app_audio_pcmbuff_mutex_id, osWaitForever);
    status = DeCQueue(&app_audio_pcm_queue, 0, len);
    osMutexRelease(app_audio_pcmbuff_mutex_id);

    return status;
}

void __attribute__((section(".fast_text_sram"))) app_audio_memcpy_16bit(int16_t *des, int16_t *src, int len)
{
    // Check input
    for(int i=0; i<len; i++)
    {
        des[i] = src[i];
    }
}

void __attribute__((section(".fast_text_sram"))) app_audio_memset_16bit(int16_t *des, int16_t val, int len)
{
    // Check input
    for(int i=0; i<len; i++)
    {
        des[i] = val;
    }
}


#ifdef __AUDIO_QUEUE_SUPPORT__
int app_audio_sendrequest_param(uint8_t id, uint8_t status, uint32_t ptr, uint32_t param)
{
    uint32_t audevt;
    APP_MESSAGE_BLOCK msg;

    if(app_audio_init == false)
        return -1;

    msg.mod_id = APP_MODUAL_AUDIO;
    APP_AUDIO_SET_MESSAGE(audevt, id, status);
    msg.msg_body.message_id = audevt;
    msg.msg_body.message_ptr = ptr;
    msg.msg_body.message_Param0 = param;
    app_mailbox_put(&msg);

    return 0;
}

int app_audio_sendrequest(uint8_t id, uint8_t status, uint32_t ptr)
{
    return app_audio_sendrequest_param(id, status, ptr, 0);
}

extern bool app_audio_list_playback_exist(void);

static uint8_t app_audio_get_list_playback_num(void)
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;
    uint8_t num=0;
    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node))
    {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
#ifdef MEDIA_PLAYER_SUPPORT
        if (audio_handle->id == APP_PLAY_BACK_AUDIO)
            num++;
#endif
    }
    return num;
}
#endif

static bool need_flush_flash_switch_audio = false;
void app_audio_switch_flash_flush_req(void)
{
    uint32_t lock;
    
    lock = int_lock();
    need_flush_flash_switch_audio = true;
    int_unlock(lock);
}

static void app_audio_switch_flash_proc(void)
{
    uint32_t lock;
    bool need_flush_flash = false;

    lock = int_lock();
    if (need_flush_flash_switch_audio){
        need_flush_flash_switch_audio = false;
        need_flush_flash = true;
    }
    int_unlock(lock);

    if (need_flush_flash){
        nv_record_flash_flush();
    }
}

static int app_audio_handle_process(APP_MESSAGE_BODY *msg_body)
{
    int nRet = -1;

    APP_AUDIO_STATUS aud_status;

    if(app_audio_init == false)
        return -1;

    APP_AUDIO_GET_ID(msg_body->message_id, aud_status.id);
    APP_AUDIO_GET_STATUS(msg_body->message_id, aud_status.status);
    APP_AUDIO_GET_AUD_ID(msg_body->message_ptr, aud_status.aud_id);
    APP_AUDIO_GET_FREQ(msg_body->message_Param0, aud_status.freq);

    switch (aud_status.status ) {
        case APP_BT_SETTING_OPEN:
        #ifdef __AUDIO_QUEUE_SUPPORT__
            debug_trace("=======>APP_BT_SETTING_OPEN,##before>>>>status_id: %d, aud_id: %d, len = %d", aud_status.id, aud_status.aud_id, list_length(app_audio_conifg.audio_list));
                if (app_audio_list_append(&aud_status))
                    {
                    app_bt_stream_open(&aud_status);

                debug_trace("=======>APP_BT_SETTING_OPEN, ##after: status_id: %d, len = %d", aud_status.id, list_length(app_audio_conifg.audio_list));
                    }
        #else
            app_bt_stream_open(&aud_status);
        #endif

            break;
        case APP_BT_SETTING_CLOSE:
            app_audio_switch_flash_proc();
        #ifdef __AUDIO_QUEUE_SUPPORT__
            APP_AUDIO_STATUS next_status;
            debug_trace("=======>APP_BT_SETTING_CLOSE, current id: %d", aud_status.id);
            app_bt_stream_close((APP_BT_STREAM_T)aud_status.id);            
            app_audio_switch_flash_proc();
#ifdef MEDIA_PLAYER_SUPPORT
            if((app_audio_get_list_playback_num()==1) && aud_status.id == APP_PLAY_BACK_AUDIO)
            {
                app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_MEDIA,BT_DEVICE_ID_1,0);
            }
#endif
            if (app_audio_list_rmv_callback(&aud_status, &next_status))
            {
                debug_trace("=======>APP_BT_SETTING_CLOSE, %x, next id: %d, status %d",&next_status, next_status.id, next_status.status);
                app_bt_stream_open(&next_status);
            }
        #else
            app_bt_stream_close((APP_BT_STREAM_T)aud_status.id);     
            app_audio_switch_flash_proc();
        #endif
            break;
        case APP_BT_SETTING_SETUP:
            app_bt_stream_setup((APP_BT_STREAM_T)aud_status.id, msg_body->message_ptr);
            break;
        case APP_BT_SETTING_RESTART:
            app_bt_stream_restart(&aud_status);
            break;
        case APP_BT_SETTING_CLOSEALL:
            app_bt_stream_closeall();
    #ifdef __AUDIO_QUEUE_SUPPORT__
            app_audio_list_clear();
    #endif
            app_audio_switch_flash_proc();
            break;
        default:
            break;
    }

    return nRet;
}

#ifdef __AUDIO_QUEUE_SUPPORT__
static void app_audio_handle_free(void* data)
{
#if 0
    APP_AUDIO_STATUS * status = (APP_AUDIO_STATUS*)audio_handle;

    if (status->id == APP_PLAY_BACK_AUDIO)
        media_FreeMemory(audio_handle);
    else
#endif
    APP_AUDIO_STATUS * status = (APP_AUDIO_STATUS * )data;

#ifdef MEDIA_PLAYER_SUPPORT
    if(status->id == APP_PLAY_BACK_AUDIO)
    {
        debug_trace("=======>app_audio_handle_free playback , aud_id: %d, type = %d", status->aud_id);
    }
#endif
    osPoolFree (app_audio_status_mempool, data);
}


#ifdef __AUDIO_QUEUE_SUPPORT__
void app_audio_list_create()
{
    if (app_audio_conifg.audio_list == NULL)
        app_audio_conifg.audio_list = list_new(app_audio_handle_free);

}
#endif

bool app_audio_list_stream_exist()
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;

    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {

        audio_handle = (APP_AUDIO_STATUS *)list_node(node);

        if (audio_handle->id == APP_BT_STREAM_HFP_PCM ||
        audio_handle->id == APP_BT_STREAM_HFP_CVSD ||
        audio_handle->id == APP_BT_STREAM_HFP_VENDOR ||
        audio_handle->id == APP_BT_STREAM_A2DP_SBC ||
        audio_handle->id == APP_BT_STREAM_A2DP_AAC ||
        audio_handle->id == APP_BT_STREAM_A2DP_VENDOR
    #if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
	     ||audio_handle->id == APP_BT_STREAM_AI_VOICE
    #endif
        )
            return true;
    }

    return false;
}

#if 0  //change begin by cai.zhong by Press the talk button continuously will crash
bool app_audio_list_filter_exist(APP_AUDIO_STATUS* aud_status)
{

    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;
#ifdef MEDIA_PLAYER_SUPPORT
    uint8_t cnt = 0;
    if (aud_status->id == APP_PLAY_BACK_AUDIO && aud_status->aud_id == AUD_ID_BT_CALL_INCOMING_CALL){
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO && audio_handle->aud_id == AUD_ID_BT_CALL_INCOMING_CALL)
                return true;
        }
    }
    if (aud_status->id == APP_PLAY_BACK_AUDIO && aud_status->aud_id == AUD_ID_BT_WARNING){
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO && audio_handle->aud_id == AUD_ID_BT_WARNING)
                cnt++;
            if (cnt>3)
                return true;
        }
    }
    if (aud_status->id == APP_PLAY_BACK_AUDIO && aud_status->aud_id == AUD_ID_LANGUAGE_SWITCH){
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO && audio_handle->aud_id == AUD_ID_LANGUAGE_SWITCH)
                cnt++;
            if (cnt>3)
                return true;
        }
    }
#endif
    return false;
}
#else
bool app_audio_list_filter_exist(APP_AUDIO_STATUS* aud_status)
{
	APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)
	uint16_t audio_id_for_once = AUDIO_ID_BT_ALEXA_START;
#elif defined(__DMA_VOICE__)
	uint16_t audio_id_for_once = AUDIO_ID_BT_DUEROS_START;
#endif
	
#ifdef MEDIA_PLAYER_SUPPORT
    uint8_t cnt = 0;

    if (aud_status->id == APP_PLAY_BACK_AUDIO && aud_status->aud_id == AUD_ID_BT_CALL_INCOMING_CALL){
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO && audio_handle->aud_id == AUD_ID_BT_CALL_INCOMING_CALL)
                return true;
        }
    }

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
    if (aud_status->id == APP_PLAY_BACK_AUDIO && aud_status->aud_id == audio_id_for_once){
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO && audio_handle->aud_id == audio_id_for_once)
                return true;
        }
    }
#endif

    if (aud_status->id == APP_PLAY_BACK_AUDIO){
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);
            if (audio_handle->id == APP_PLAY_BACK_AUDIO && audio_handle->aud_id == aud_status->aud_id)
                cnt++;
            if (cnt>3)
                return true;
        }
    }
#endif	
}
#endif
//change end by cai.zhong by Press the talk button continuously will crash


bool app_audio_list_playback_exist(void)
{
    APP_AUDIO_STATUS *audio_handle = NULL;
    list_node_t *node = NULL;

    for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node)) {
        audio_handle = (APP_AUDIO_STATUS *)list_node(node);
#ifdef MEDIA_PLAYER_SUPPORT
        if (audio_handle->id == APP_PLAY_BACK_AUDIO)
            return true;
#endif
    }

    return false;
}

bool app_audio_list_append(APP_AUDIO_STATUS* aud_status)
{
#ifdef MEDIA_PLAYER_SUPPORT
    if (aud_status->id == APP_PLAY_BACK_AUDIO)
    {
      //ignore redundant ring ind from hfp...
      if (app_audio_list_filter_exist(aud_status))
          return false;

      if (app_audio_list_playback_exist() )
      {
        if (list_length(app_audio_conifg.audio_list) >= MAX_AUDIO_BUF_LIST)
        {
            APP_AUDIO_STATUS* id_ptr = (APP_AUDIO_STATUS*)osPoolCAlloc (app_audio_status_mempool);
            if (app_audio_list_stream_exist()) {
                memcpy(id_ptr, (const void *)list_front(app_audio_conifg.audio_list), sizeof(APP_AUDIO_STATUS));
                app_audio_list_clear();
                list_prepend(app_audio_conifg.audio_list, id_ptr);
            }
            else {
                memcpy(id_ptr, (const void *)aud_status, sizeof(APP_AUDIO_STATUS));
                app_audio_list_clear();
                list_append(app_audio_conifg.audio_list,  (void*)id_ptr);
            }
            ASSERT(0 , "#####app_audio_list_append error!!! FIXME!!!! ID = %d, status = %d\n ", id_ptr->id, id_ptr->status);
            return true;
        }
        APP_AUDIO_STATUS* id_ptr = (APP_AUDIO_STATUS*)osPoolCAlloc (app_audio_status_mempool);
        memcpy(id_ptr, (const void *)aud_status, sizeof(APP_AUDIO_STATUS));

        debug_trace( "#####app_audio_list_append 11111##0x%x, ID = %d, status = %d\n ",id_ptr,  id_ptr->id, id_ptr->status);

         list_append(app_audio_conifg.audio_list, (void*)id_ptr);
         return false;
      }
      else
      {
         APP_AUDIO_STATUS* id_ptr = (APP_AUDIO_STATUS*)osPoolCAlloc (app_audio_status_mempool);
        memcpy(id_ptr, (const void *)aud_status, sizeof(APP_AUDIO_STATUS));

           debug_trace( "#####app_audio_list_append 22222222 ##0x%x,ID = %d, status = %d\n ", id_ptr, id_ptr->id, id_ptr->status);

         list_append(app_audio_conifg.audio_list,  (void*)id_ptr);
         return true;
      }
    }
    else
    {
#endif
    app_audio_list_clear();
    APP_AUDIO_STATUS* dest_audio_handle = (APP_AUDIO_STATUS *)osPoolCAlloc (app_audio_status_mempool);
       memcpy(dest_audio_handle, aud_status, sizeof(APP_AUDIO_STATUS));

      debug_trace( "#####app_audio_list_append add 3333333333:len = %d, id = %d\n ", list_length(app_audio_conifg.audio_list), dest_audio_handle->id);

     list_prepend(app_audio_conifg.audio_list, dest_audio_handle);

    return true;
#ifdef MEDIA_PLAYER_SUPPORT
    }
#endif
}


bool app_audio_list_rmv_callback(APP_AUDIO_STATUS* status_close, APP_AUDIO_STATUS* status_next)
{
    APP_AUDIO_STATUS *audio_handle = NULL;

    list_node_t *node = NULL;

     debug_trace( "######app_audio_list_rmv_callbackr list_num:%d \n ", list_length(app_audio_conifg.audio_list));

    //for status: first bt_a2dp->APP_BT_SETTING_CLOSE,then ring-> APP_BT_SETTING_CLOSE
    if(list_length(app_audio_conifg.audio_list) == 0)
        return false;
#ifdef MEDIA_PLAYER_SUPPORT
    if (status_close->id == APP_PLAY_BACK_AUDIO)
    {
        for (node = list_begin(app_audio_conifg.audio_list); node != list_end(app_audio_conifg.audio_list); node = list_next(node))
        {
            audio_handle = (APP_AUDIO_STATUS *)list_node(node);

            if (audio_handle->id == APP_PLAY_BACK_AUDIO)
             {
             list_node_t * nod_next = list_next(node);

             debug_trace( "######app_audio_list_rmv_callbackr 0000 id:%d \n ", audio_handle->id);

            if (nod_next)
            {
                 memcpy(status_next, list_node(nod_next), sizeof(APP_AUDIO_STATUS));

                 ASSERT(status_next->id == APP_PLAY_BACK_AUDIO, "[%s] 111ERROR: status_next->id != APP_PLAY_BACK_AUDIO", __func__);

                 list_remove(app_audio_conifg.audio_list, list_node(node));
                    debug_trace( "######app_audio_list_rmv_callback 1111 len = %d\n ", list_length(app_audio_conifg.audio_list));

                return true;
            }
            else  if (app_audio_list_stream_exist())
            {
                void*indata = list_front(app_audio_conifg.audio_list);
                memcpy(status_next, indata, sizeof(APP_AUDIO_STATUS));

                ASSERT(status_next->id != APP_PLAY_BACK_AUDIO, "[%s] 222ERROR: status_next->id != APP_PLAY_BACK_AUDIO", __func__);
                list_remove(app_audio_conifg.audio_list, list_node(node));

                   debug_trace( "######app_audio_list_rmv_callback 2222 status_next = %x, list_back = %x, list_num:%d \n ",status_next, list_back(app_audio_conifg.audio_list),  list_length(app_audio_conifg.audio_list));

                return true;
            }
            else
            {
                list_remove(app_audio_conifg.audio_list, list_node(node));
                   debug_trace( "######app_audio_list_rmv_callback 33333 len = %d\n ", list_length(app_audio_conifg.audio_list));

                return false;
            }

            break;
         }
        }

        }
    else //maybe...a2dp send >> APP_BT_SETTING_CLOSE,  when ring
    {
#endif
        if (app_audio_list_stream_exist())
        {
            list_remove(app_audio_conifg.audio_list, list_front(app_audio_conifg.audio_list));
            //app_audio_list_clear();
        }

        debug_trace( "######app_audio_list_rmv_callback stream>>44444  list_num:%d \n ", list_length(app_audio_conifg.audio_list));

        return false;
#ifdef MEDIA_PLAYER_SUPPORT
    }
#endif
    return false;
}

void app_audio_list_clear()
{
    list_clear(app_audio_conifg.audio_list);
}
#endif

void app_audio_open(void)
{

    if (g_app_audio_queue_mutex_id == NULL)
    {
        g_app_audio_queue_mutex_id = osMutexCreate((osMutex(g_app_audio_queue_mutex)));
    }
    else
    {
        ASSERT(0, "[%s] ERROR: g_app_audio_queue_mutex_id != NULL", __func__);
    }

    if (app_audio_status_mempool == NULL)
        app_audio_status_mempool = osPoolCreate(osPool(app_audio_status_mempool));
    ASSERT(app_audio_status_mempool, "[%s] ERROR: app_audio_status_mempool != NULL", __func__);

#ifdef __AUDIO_QUEUE_SUPPORT__
    app_audio_list_create();
#endif
    app_ring_merge_init();

    app_set_threadhandle(APP_MODUAL_AUDIO, app_audio_handle_process);

    app_audio_init = true;
}

void app_audio_close(void)
{
    app_set_threadhandle(APP_MODUAL_AUDIO, NULL);
    app_audio_init = false;
}
