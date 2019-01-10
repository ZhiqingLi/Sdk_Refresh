#include "cmsis_os.h"
#include "hal_trace.h"
#include "audioflinger.h"
#include "app_ring_merge.h"
#include "res_audio_ring.h"

// control queue access
osMutexId app_ring_merge_mutex_id = NULL;
osMutexDef(app_ring_merge_mutex);


struct APP_RING_MERGE_CONFIG_T{
    bool running;
    int16_t *pbuf;
    uint32_t len;
    uint32_t next;
    enum APP_RING_MERGE_PLAY_T play;
    AF_STREAM_HANDLER_T handler;
};

struct APP_RING_MERGE_CONFIG_T app_ring_merge_config ={
    .running = false,
    .pbuf = NULL,
    .len = 0,
    .next = 0,
    .play = APP_RING_MERGE_PLAY_QTY,
    .handler = NULL
};

static void app_ring_merge_track_2_in_1_16bits(int16_t *src_buf0, int16_t *src_buf1, int16_t *dst_buf,  uint32_t src_len)
{
    uint32_t i = 0;
    for (i = 0; i < src_len; i++) {
        dst_buf[i] = (src_buf0[i]>>1) + (src_buf1[i]>>1);
    }
}

static uint32_t app_ring_merge_oneshot_more_data(uint8_t *buf, uint32_t len)
{
    uint32_t need_len = len/2;
    int16_t *pBuf = (int16_t *)buf;
    uint32_t curr_size = 0;

    if (app_ring_merge_config.next == app_ring_merge_config.len){
        return len;
    }

    if (need_len > app_ring_merge_config.len){
        app_ring_merge_track_2_in_1_16bits(pBuf, app_ring_merge_config.pbuf, pBuf, app_ring_merge_config.len);
        app_ring_merge_config.next = app_ring_merge_config.len;
        app_ring_merge_config.running = false;
    }else{
        if ((app_ring_merge_config.len - app_ring_merge_config.next) >= need_len)
        {
            app_ring_merge_track_2_in_1_16bits(pBuf, app_ring_merge_config.pbuf+app_ring_merge_config.next, pBuf, need_len);
            app_ring_merge_config.next += need_len;
        }
        else
        {
            curr_size = app_ring_merge_config.len - app_ring_merge_config.next;
            app_ring_merge_track_2_in_1_16bits(pBuf, app_ring_merge_config.pbuf+app_ring_merge_config.next, pBuf, curr_size);
            app_ring_merge_config.next = app_ring_merge_config.len;
            app_ring_merge_config.running = false;
        }
    }
    return len;
}

static uint32_t app_ring_merge_periodic_more_data(uint8_t *buf, uint32_t len)
{
    uint32_t need_len = len/2;
    uint32_t remain_size = len/2;
    uint32_t curr_size = 0;
    int16_t *pBuf = (int16_t *)buf;
    if (need_len > app_ring_merge_config.len)
    {
        do{
            if (app_ring_merge_config.next)
            {
                curr_size = app_ring_merge_config.len-app_ring_merge_config.next;
                app_ring_merge_track_2_in_1_16bits(pBuf, app_ring_merge_config.pbuf + app_ring_merge_config.next, pBuf, curr_size);
                remain_size -= curr_size;
                app_ring_merge_config.next = 0;
            }
            else if (remain_size>app_ring_merge_config.len)
            {
                app_ring_merge_track_2_in_1_16bits(pBuf+curr_size, app_ring_merge_config.pbuf, pBuf+curr_size, app_ring_merge_config.len);
                curr_size += app_ring_merge_config.len;
                remain_size -= app_ring_merge_config.len;
            }
            else
            {
                app_ring_merge_track_2_in_1_16bits(pBuf+curr_size, app_ring_merge_config.pbuf, pBuf+curr_size, remain_size);
                app_ring_merge_config.next = remain_size;
                remain_size = 0;
            }
        }while(remain_size);
    }
    else
    {
        if ((app_ring_merge_config.len - app_ring_merge_config.next) >= need_len)
        {
            app_ring_merge_track_2_in_1_16bits(pBuf, app_ring_merge_config.pbuf+app_ring_merge_config.next, pBuf, need_len);
            app_ring_merge_config.next += need_len;
        }
        else
        {
            curr_size = app_ring_merge_config.len - app_ring_merge_config.next;
            app_ring_merge_track_2_in_1_16bits(pBuf, app_ring_merge_config.pbuf+app_ring_merge_config.next, pBuf, curr_size);
            app_ring_merge_config.next = need_len - curr_size;
            app_ring_merge_track_2_in_1_16bits(pBuf+curr_size, app_ring_merge_config.pbuf, pBuf+curr_size, app_ring_merge_config.next);
        }
    }
    return len;
}

uint32_t app_ring_merge_more_data(uint8_t *buf, uint32_t len)
{
    uint32_t nRet = len;

    osMutexWait(app_ring_merge_mutex_id, osWaitForever);
    if (app_ring_merge_config.handler && app_ring_merge_config.running){
        nRet = app_ring_merge_config.handler(buf, len);        
    }
    osMutexRelease(app_ring_merge_mutex_id);

    return nRet;
}

int app_ring_merge_setup(int16_t *buf, uint32_t len, enum APP_RING_MERGE_PLAY_T play)
{
    osMutexWait(app_ring_merge_mutex_id, osWaitForever);

    app_ring_merge_config.running = false;
    app_ring_merge_config.pbuf = (int16_t *)buf;
    app_ring_merge_config.len = len;
    app_ring_merge_config.next = 0;
    
    app_ring_merge_config.play = play;

    if (play == APP_RING_MERGE_PLAY_ONESHOT){
        app_ring_merge_config.handler = app_ring_merge_oneshot_more_data;
    }else if (play == APP_RING_MERGE_PLAY_PERIODIC){
        app_ring_merge_config.handler = app_ring_merge_periodic_more_data;
    }

    osMutexRelease(app_ring_merge_mutex_id);

    return 0;
}

int app_ring_merge_init(void)
{
    if (app_ring_merge_mutex_id == NULL)
    {
        app_ring_merge_mutex_id = osMutexCreate((osMutex(app_ring_merge_mutex)));
    }    
    return 0;
}

int app_ring_merge_deinit(void)
{
    osMutexWait(app_ring_merge_mutex_id, osWaitForever);

    app_ring_merge_config.running = false;
    app_ring_merge_config.pbuf = NULL;
    app_ring_merge_config.len = 0;
    app_ring_merge_config.next = 0;
    app_ring_merge_config.play = APP_RING_MERGE_PLAY_QTY;
    app_ring_merge_config.handler = NULL;
    
    osMutexRelease(app_ring_merge_mutex_id);
    
    return 0;
}

int app_ring_merge_start(void)
{
    uint32_t ret;
    struct AF_STREAM_CONFIG_T *stream_cfg = NULL;

    ret = af_stream_get_cfg(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg, true);
    ASSERT(ret == 0, "Failed to get stream cfg");
    TRACE("app_ring_merge_start:%d\n", stream_cfg->sample_rate);

    osMutexWait(app_ring_merge_mutex_id, osWaitForever);
#ifdef __BT_WARNING_TONE_MERGE_INTO_STREAM_SBC__
    switch (stream_cfg->sample_rate) {
        case AUD_SAMPRATE_8000:
            app_ring_merge_setup((int16_t *)RES_AUD_RING_SAMPRATE_8000, sizeof(RES_AUD_RING_SAMPRATE_8000)/sizeof(int16_t), APP_RING_MERGE_PLAY_ONESHOT);
            app_ring_merge_config.next = 0;
            app_ring_merge_config.running = true;    
            break;
        case AUD_SAMPRATE_44100:
            app_ring_merge_setup((int16_t *)RES_AUD_RING_SAMPRATE_44100, sizeof(RES_AUD_RING_SAMPRATE_44100)/sizeof(int16_t), APP_RING_MERGE_PLAY_ONESHOT);
            app_ring_merge_config.next = 0;
            app_ring_merge_config.running = true;    
            break;
        case AUD_SAMPRATE_48000:
            app_ring_merge_setup((int16_t *)RES_AUD_RING_SAMPRATE_48000, sizeof(RES_AUD_RING_SAMPRATE_48000)/sizeof(int16_t), APP_RING_MERGE_PLAY_ONESHOT);
            app_ring_merge_config.next = 0;
            app_ring_merge_config.running = true;    
            break;
        case AUD_SAMPRATE_16000:
        case AUD_SAMPRATE_22050:
        case AUD_SAMPRATE_24000:
        case AUD_SAMPRATE_96000:
        case AUD_SAMPRATE_192000:
        case AUD_SAMPRATE_NULL:
        default:
            app_ring_merge_config.next = 0;
            app_ring_merge_config.running = false;    
            break;
    }
#else
    app_ring_merge_setup((int16_t *)RES_AUD_RING_SAMPRATE_8000, sizeof(RES_AUD_RING_SAMPRATE_8000)/sizeof(int16_t), APP_RING_MERGE_PLAY_ONESHOT);
    app_ring_merge_config.next = 0;
    app_ring_merge_config.running = true;        
#endif
    osMutexRelease(app_ring_merge_mutex_id);

    return 0;
}

int app_ring_merge_stop(void)
{
    osMutexWait(app_ring_merge_mutex_id, osWaitForever);
    app_ring_merge_config.running = false;
    app_ring_merge_config.next = 0;
    osMutexRelease(app_ring_merge_mutex_id);
    
    return 0;
}

bool app_ring_merge_isrun(void)
{
    bool running;
    osMutexWait(app_ring_merge_mutex_id, osWaitForever);
    running = app_ring_merge_config.running;
    osMutexRelease(app_ring_merge_mutex_id);
    return running;
}
