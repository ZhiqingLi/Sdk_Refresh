#ifndef __APP_BT_STREAM_H__
#define __APP_BT_STREAM_H__

#include "hal_aud.h"

#if defined(A2DP_AAC_ON)
#define BT_AUDIO_BUFF_SIZE		(2048*2*2)
#else
#define BT_AUDIO_BUFF_SIZE		(512*5*2*2)
#endif

#define BT_AUDIO_CACHE_2_UNCACHE(addr) \
    ((unsigned char *)((unsigned int)addr & ~(0x04000000)))

// #if BT_AUDIO_BUFF_SIZE < BT_AUDIO_SCO_BUFF_SIZE * 4
// #error BT_AUDIO_BUFF_SIZE must be at least BT_AUDIO_SCO_BUFF_SIZE * 4
// #endif

enum APP_BT_STREAM_T {
	APP_BT_STREAM_HFP_PCM = 0,
	APP_BT_STREAM_HFP_CVSD,
	APP_BT_STREAM_HFP_VENDOR,
	APP_BT_STREAM_A2DP_SBC,
	APP_BT_STREAM_A2DP_AAC,
	APP_BT_STREAM_A2DP_VENDOR,
#ifdef __FACTORY_MODE_SUPPORT__
	APP_FACTORYMODE_AUDIO_LOOP,
#endif	
#ifdef MEDIA_PLAYER_SUPPORT
	APP_PLAY_BACK_AUDIO,
#endif
#ifdef MEDIA_PLAYER_RBCODEC
	APP_BT_STREAM_RBCODEC,
#endif
#ifdef AUDIO_LINEIN
	APP_PLAY_LINEIN_AUDIO,
#endif
#ifdef __APP_A2DP_SOURCE__
    APP_A2DP_SOURCE_LINEIN_AUDIO,
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
	APP_BT_STREAM_AI_VOICE,
#endif

	APP_BT_STREAM_NUM,
};

enum APP_BT_SETTING_T {
	APP_BT_SETTING_OPEN = 0,
	APP_BT_SETTING_CLOSE,
	APP_BT_SETTING_SETUP,
	APP_BT_SETTING_RESTART,
	APP_BT_SETTING_CLOSEALL,
	APP_BT_SETTING_CLOSEMEDIA,
	APP_BT_SETTING_NUM,
};


typedef struct {
    uint16_t id;
    uint16_t status;

    uint16_t aud_type;
    uint16_t aud_id;

    uint8_t freq;
}APP_AUDIO_STATUS;

int app_bt_stream_open(APP_AUDIO_STATUS* status);

int app_bt_stream_close(enum APP_BT_STREAM_T player);

int app_bt_stream_setup(enum APP_BT_STREAM_T player, uint8_t status);

int app_bt_stream_restart(APP_AUDIO_STATUS* status);

int app_bt_stream_closeall();

bool app_bt_stream_isrun(enum APP_BT_STREAM_T player);

void app_bt_stream_volumeup(void);

void app_bt_stream_volumedown(void);

void app_bt_stream_volume_ptr_update(uint8_t *bdAddr);

struct btdevice_volume * app_bt_stream_volume_get_ptr(void);

int app_bt_stream_volumeset(int8_t vol);

void app_bt_stream_a2dpvolume_reset(void);

void app_bt_stream_hfpvolume_reset(void);

void app_bt_stream_copy_track_one_to_two_16bits(int16_t *dst_buf, int16_t *src_buf, uint32_t src_len);

void app_bt_stream_copy_track_two_to_one_16bits(int16_t *dst_buf, int16_t *src_buf, uint32_t dst_len);

enum AUD_SAMPRATE_T bt_get_sbc_sample_rate(void);

void bt_store_sbc_sample_rate(enum AUD_SAMPRATE_T sample_rate);

enum AUD_SAMPRATE_T bt_parse_store_sbc_sample_rate(uint8_t sbc_samp_rate);

// =======================================================
// APP RESAMPLE
// =======================================================

#include "hal_aud.h"

typedef int (*APP_RESAMPLE_ITER_CALLBACK)(uint8_t *buf, uint32_t len);

struct APP_RESAMPLE_T {
    enum AUD_STREAM_T stream;
    void *id;
    APP_RESAMPLE_ITER_CALLBACK cb;
    uint8_t *iter_buf;
    uint32_t iter_len;
    uint32_t offset;
    float ratio_step;
};

struct APP_RESAMPLE_T *app_playback_resample_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_CHANNEL_NUM_T chans,
                                                  APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len);
struct APP_RESAMPLE_T *app_playback_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
                                                      APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
                                                      float ratio_step);
int app_playback_resample_close(struct APP_RESAMPLE_T *resamp);
int app_playback_resample_run(struct APP_RESAMPLE_T *resamp, uint8_t *buf, uint32_t len);

struct APP_RESAMPLE_T *app_capture_resample_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_CHANNEL_NUM_T chans,
                                                 APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len);
struct APP_RESAMPLE_T *app_capture_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
                                                     APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
                                                     float ratio_step);
int app_capture_resample_close(struct APP_RESAMPLE_T *resamp);
int app_capture_resample_run(struct APP_RESAMPLE_T *resamp, uint8_t *buf, uint32_t len);

void app_resample_reset(struct APP_RESAMPLE_T *resamp);
void app_resample_tune(struct APP_RESAMPLE_T *resamp, uint32_t rate, int32_t sample, uint32_t ms);
void app_resample_set_tune_factor(struct APP_RESAMPLE_T *resamp, float factor);
float app_resample_get_tune_factor(void);

#endif
