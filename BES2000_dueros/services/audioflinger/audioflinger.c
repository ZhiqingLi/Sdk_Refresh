#include "audioflinger.h"
#include "hal_dma.h"
#include "hal_i2s.h"
#include "hal_codec.h"
#include "hal_spdif.h"
#include "hal_btpcm.h"
#include "codec_tlv32aic32.h"
#include "codec_int.h"

#include "string.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "pmu.h"
#include "analog.h"
#include "tgt_hardware.h"

#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#else
#include "hal_sleep.h"
#endif

#define AF_TRACE_DEBUG()    //TRACE("%s:%d\n", __func__, __LINE__)

//#define AF_STREAM_ID_0_PLAYBACK_FADEOUT

//#define CORRECT_SAMPLE_VALUE

#ifdef AUDIO_OUTPUT_DC_CALIB
#ifdef CHIP_BEST1000
#define AUDIO_OUTPUT_DC_CALIB_SW
#define AUDIO_OUTPUT_PA_ON_FADE_IN
#if !defined(__TWS__)
#define AUDIO_OUTPUT_PA_OFF_FADE_OUT
#endif
#elif defined(__TWS__) && !defined(__BT_ANC__)
#define AUDIO_OUTPUT_PA_ON_FADE_IN
#endif // !CHIP_BEST1000 && __TWS__
#elif defined(AUDIO_OUTPUT_DC_CALIB_ANA) && defined(__TWS__) && !defined(__BT_ANC__)
#define AUDIO_OUTPUT_PA_ON_FADE_IN
#endif // !AUDIO_OUTPUT_DC_CALIB && AUDIO_OUTPUT_DC_CALIB_ANA && __TWS__

#define AF_FADE_OUT_SIGNAL_ID           15
#define AF_FADE_IN_SIGNAL_ID            14

/* config params */
#define AF_I2S_INST HAL_I2S_ID_0
#define AF_CODEC_INST HAL_CODEC_ID_0
#define AF_SPDIF_INST HAL_SPDIF_ID_0
#define AF_BTPCM_INST HAL_BTPCM_ID_0

#define AF_CPU_WAKE_USER                HAL_CPU_WAKE_LOCK_USER_AUDIOFLINGER

#define AF_CODEC_RIGHT_CHAN_ATTN        0.968277856 // -0.28 dB

#define AF_CODEC_VOL_UPDATE_STEP        0.00002

#define AF_CODEC_DC_MAX_SCALE           32767

#define AF_CODEC_DC_STABLE_INTERVAL     MS_TO_TICKS(4)
#ifdef AUDIO_OUTPUT_PA_OFF_FADE_OUT
#define AF_CODEC_PA_RESTART_INTERVAL    MS_TO_TICKS(160)
#else
#define AF_CODEC_PA_RESTART_INTERVAL    MS_TO_TICKS(8)
#endif

// The following might have been defined in tgt_hardware.h
#ifndef AF_CODEC_FADE_IN_MS
#define AF_CODEC_FADE_IN_MS             20
#endif
#ifndef AF_CODEC_FADE_OUT_MS
#define AF_CODEC_FADE_OUT_MS            20
#endif
#define AF_CODEC_FADE_MIN_SAMPLE_CNT    200

#define PP_PINGPANG(v)                  (v == PP_PING ? PP_PANG : PP_PING)

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
#ifdef KNOWLES_UART_DATA
#define AF_STACK_SIZE                   (4*1024)
#else
#define AF_STACK_SIZE                   (12*1024)
#endif
#else
#define AF_STACK_SIZE                   (1024*6)
#endif

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
#define DYNAMIC_AUDIO_BUFFER_COUNT
#endif

#ifdef DYNAMIC_AUDIO_BUFFER_COUNT
#define MAX_AUDIO_BUFFER_COUNT          16
#define MIN_AUDIO_BUFFER_COUNT          2
#define AUDIO_BUFFER_COUNT              (role->dma_desc_cnt)
#else
#define MAX_AUDIO_BUFFER_COUNT          4
#define AUDIO_BUFFER_COUNT              MAX_AUDIO_BUFFER_COUNT
#if (AUDIO_BUFFER_COUNT & 0x1)
#error "AUDIO_BUFFER_COUNT must be an even number"
#endif
#endif

/* internal use */
enum AF_BOOL_T{
    AF_FALSE = 0,
    AF_TRUE = 1
};

enum AF_RESULT_T{
    AF_RES_SUCCESS = 0,
    AF_RES_FAILD = 1
};

enum AF_DAC_PA_STATE_T {
    AF_DAC_PA_NULL,
    AF_DAC_PA_ON_TRIGGER,
    AF_DAC_PA_ON_SOFT_START,
    AF_DAC_PA_OFF_TRIGGER,
    AF_DAC_PA_OFF_SOFT_END,
    AF_DAC_PA_OFF_SOFT_END_DONE,
    AF_DAC_PA_OFF_DC_START,
};

//status machine
enum AF_STATUS_T{
    AF_STATUS_NULL = 0x00,
    AF_STATUS_OPEN_CLOSE = 0x01,
    AF_STATUS_STREAM_OPEN_CLOSE = 0x02,
    AF_STATUS_STREAM_START_STOP = 0x04,
    AF_STATUS_STREAM_PAUSE_RESTART = 0x08,
    AF_STATUS_MASK = 0x0F,
};

struct af_stream_ctl_t{
    enum AF_PP_T pp_index;		//pingpong operate
    uint8_t pp_cnt;				//use to count the lost signals
    uint8_t status;				//status machine
    enum AUD_STREAM_USE_DEVICE_T use_device;
};

struct af_stream_cfg_t {
    //used inside
    struct af_stream_ctl_t ctl;

    //dma buf parameters, RAM can be alloced in different way
    uint8_t *dma_buf_ptr;
    uint32_t dma_buf_size;

    //store stream cfg parameters
    struct AF_STREAM_CONFIG_T cfg;

    //dma cfg parameters
#ifdef DYNAMIC_AUDIO_BUFFER_COUNT
    uint8_t dma_desc_cnt;
#endif
    struct HAL_DMA_DESC_T dma_desc[MAX_AUDIO_BUFFER_COUNT];
    struct HAL_DMA_CH_CFG_T dma_cfg;

    //callback function
    AF_STREAM_HANDLER_T handler;
};

static struct af_stream_cfg_t af_stream[AUD_STREAM_ID_NUM][AUD_STREAM_NUM];
static bool af_buf_err[AUD_STREAM_ID_NUM][AUD_STREAM_NUM];

#ifdef AUDIO_OUTPUT_SW_GAIN
static float cur_output_coef[2];
static float new_output_coef[2];
#elif defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
static uint8_t cur_data_shift[2];
static uint8_t new_data_shift[2];
static int32_t prev_data[2];
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
static bool dac_dc_valid;
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
static int16_t dac_dc[2];
#endif
#endif

#if defined(AUDIO_OUTPUT_PA_ON_FADE_IN) || defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT)
static volatile enum AF_DAC_PA_STATE_T dac_pa_state;
static uint32_t dac_dc_start_time;
static uint32_t dac_pa_stop_time;

#ifdef AUDIO_OUTPUT_SW_GAIN
static const float fade_curve[] = {
    0.00000000, 0.00001334, 0.00005623, 0.00017783, 0.00056234, 0.00177828, 0.00562341, 0.01778279,
    0.05623413, 0.17782794, 0.56234133, 0.80560574, 0.86305408, 0.89733331, 0.92184771, 0.94094446,
    0.95659002, 0.96984292, 0.98133882, 0.99148953, 1.00000000,
};
STATIC_ASSERT(ARRAY_SIZE(fade_curve) >= 2, "Invalid fade curve");
static uint16_t fade_curve_idx[2];
static uint16_t fade_step_idx[2];
static float fade_gain_step[2];
static float fade_gain_range[2];
static uint16_t fade_sample_cnt;
static uint16_t fade_sample_proc;
static uint16_t fade_step_cnt;
static float saved_output_coef;
#endif
#endif

#if defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT) && defined(RTOS)
static osThreadId fade_thread_id;
#ifdef AF_STREAM_ID_0_PLAYBACK_FADEOUT
#error "Cannot enable 2 kinds of fade out codes at the same time"
#endif
#endif

#ifdef RTOS

#ifdef AF_STREAM_ID_0_PLAYBACK_FADEOUT
struct af_stream_fade_out_t{
    bool stop_on_process;
    uint8_t stop_process_cnt;
    osThreadId stop_request_tid;
    uint32_t need_fadeout_len;
    uint32_t need_fadeout_len_processed;
};

static struct af_stream_fade_out_t af_stream_fade_out ={
                                                .stop_on_process = false,
                                                .stop_process_cnt = 0,
                                                .stop_request_tid = NULL,
                                                .need_fadeout_len = 0,
                                                .need_fadeout_len_processed = 0,
};
#endif

static osThreadId af_thread_tid;

static void af_thread(void const *argument);
osThreadDef(af_thread, osPriorityAboveNormal, AF_STACK_SIZE);
static int af_default_priority;

osMutexId audioflinger_mutex_id = NULL;
osMutexDef(audioflinger_mutex);

#else // !RTOS

static volatile uint32_t af_flag_lock;

#endif // RTOS

void af_lock_thread(void)
{
    register uint32_t r14 asm("r14");
    uint32_t POSSIBLY_UNUSED lr = r14;
#ifdef RTOS
    osMutexWait(audioflinger_mutex_id, osWaitForever);
#else
    static uint32_t POSSIBLY_UNUSED locked_lr;
    ASSERT(af_flag_lock == 0, "audioflinger has been locked by %p. LR=%p", locked_lr, lr);
    af_flag_lock = 1;
    locked_lr = lr;
#endif
}

void af_unlock_thread(void)
{
    register uint32_t r14 asm("r14");
    uint32_t POSSIBLY_UNUSED lr = r14;
#ifdef RTOS
    osMutexRelease(audioflinger_mutex_id);
#else
    static uint32_t POSSIBLY_UNUSED unlocked_lr;
    ASSERT(af_flag_lock == 1, "audioflinger not locked before (lastly unlocked by %p). LR=%p", unlocked_lr, lr);
    af_flag_lock = 0;
    unlocked_lr = lr;
#endif
}

#if defined(RTOS) && defined(AF_STREAM_ID_0_PLAYBACK_FADEOUT)
int af_stream_fadeout_start(uint32_t sample)
{
    TRACE("fadein_config sample:%d", sample);
    af_stream_fade_out.need_fadeout_len = sample;
    af_stream_fade_out.need_fadeout_len_processed = sample;
    return 0;
}

int af_stream_fadeout_stop(void)
{
    af_stream_fade_out.stop_process_cnt = 0;
    af_stream_fade_out.stop_on_process = false;
    return 0;
}

uint32_t af_stream_fadeout(int16_t *buf, uint32_t len, enum AUD_CHANNEL_NUM_T num)
{
    uint32_t i;
    uint32_t j = 0;
    uint32_t start;
    uint32_t end;

    start = af_stream_fade_out.need_fadeout_len_processed;
    end = af_stream_fade_out.need_fadeout_len_processed  > len ?
           af_stream_fade_out.need_fadeout_len_processed - len :  0;

    if (start <= end){
//        TRACE("skip fadeout");
        memset(buf, 0, len*2);
        return len;
    }
//    TRACE("fadeout l:%d start:%d end:%d", len, start, end);
//    DUMP16("%05d ", buf, 10);
//    DUMP16("%05d ", buf+len-10, 10);


    if (num == AUD_CHANNEL_NUM_1){
        for (i = start; i > end; i--){
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
        }
    }else if (num == AUD_CHANNEL_NUM_2){
        for (i = start; i > end; i-=2){
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
        }
    }else if (num == AUD_CHANNEL_NUM_4){
        for (i = start; i > end; i-=4){
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
        }
    }else if (num == AUD_CHANNEL_NUM_8){
        for (i = start; i > end; i-=8){
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
            *(buf+j) = *(buf+j)*i/af_stream_fade_out.need_fadeout_len;
            j++;
        }

    }
    af_stream_fade_out.need_fadeout_len_processed -= j;

//    TRACE("out i:%d process:%d x:%d", i, af_stream_fade_out.need_fadeout_len_processed, end+((start-end)/AUD_CHANNEL_NUM_2));
//    DUMP16("%05d ", buf, 10);
//    DUMP16("%05d ", buf+len-10, 10);

    return len;
}

void af_stream_stop_wait_finish()
{
    af_stream_fade_out.stop_on_process = true;
    af_stream_fade_out.stop_request_tid = osThreadGetId();
    osSignalClear(af_stream_fade_out.stop_request_tid, (1 << AF_FADE_OUT_SIGNAL_ID));
    af_unlock_thread();
    osSignalWait((1 << AF_FADE_OUT_SIGNAL_ID), 300);
    af_lock_thread();
}

void af_stream_stop_process(struct af_stream_cfg_t *af_cfg, uint8_t *buf, uint32_t len)
{
    af_lock_thread();
    if (af_stream_fade_out.stop_on_process){
//        TRACE("%s num:%d size:%d len:%d cnt:%d", __func__, af_cfg->cfg.channel_num, af_cfg->cfg.data_size, len,  af_stream_fade_out.stop_process_cnt);
        af_stream_fadeout((int16_t *)buf, len/2, af_cfg->cfg.channel_num);

//        TRACE("process ret:%d %d %d", *(int16_t *)(buf+len-2-2-2), *(int16_t *)(buf+len-2-2), *(int16_t *)(buf+len-2));
        if (af_stream_fade_out.stop_process_cnt++>3){
            TRACE("stop_process end");
            osSignalSet(af_stream_fade_out.stop_request_tid, (1 << AF_FADE_OUT_SIGNAL_ID));
        }
    }
    af_unlock_thread();
}
#endif // RTOS && AF_STREAM_ID_0_PLAYBACK_FADEOUT

//used by dma irq and af_thread
static inline struct af_stream_cfg_t *af_get_stream_role(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    ASSERT(id < AUD_STREAM_ID_NUM, "[%s] Bad id=%d", __func__, id);
    ASSERT(stream < AUD_STREAM_NUM, "[%s] Bad stream=%d", __func__, stream);

    return &af_stream[id][stream];
}

static void af_set_status(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, enum AF_STATUS_T status)
{
    struct af_stream_cfg_t *role = NULL;

    role = af_get_stream_role(id, stream);

    role->ctl.status |= status;
}

static void af_clear_status(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, enum AF_STATUS_T status)
{
    struct af_stream_cfg_t *role = NULL;

    role = af_get_stream_role(id, stream);

    role->ctl.status &= ~status;
}

//get current stream config parameters
uint32_t af_stream_get_cfg(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, struct AF_STREAM_CONFIG_T **cfg, bool needlock)
{
    AF_TRACE_DEBUG();

    struct af_stream_cfg_t *role;
    enum AF_RESULT_T ret;

    if (needlock) {
        af_lock_thread();
    }
    role = af_get_stream_role(id, stream);
    //check stream is open
    if (role->ctl.status & AF_STATUS_STREAM_OPEN_CLOSE) {
        *cfg = &(role->cfg);
        ret = AF_RES_SUCCESS;
    } else {
        ret = AF_RES_FAILD;
    }
    if (needlock) {
        af_unlock_thread();
    }
    return ret;
}

#if 0
static void af_dump_cfg()
{
    struct af_stream_cfg_t *role = NULL;

    TRACE("dump cfg.........start");
    //initial parameter
    for(uint8_t id=0; id< AUD_STREAM_ID_NUM; id++)
    {
        for(uint8_t stream=0; stream < AUD_STREAM_NUM; stream++)
        {
            role = af_get_stream_role((enum AUD_STREAM_ID_T)id, (enum AUD_STREAM_T)stream);

            TRACE("id = %d, stream = %d:", id, stream);
            TRACE("ctl.use_device = %d", role->ctl.use_device);
            TRACE("cfg.device = %d", role->cfg.device);
            TRACE("dma_cfg.ch = %d", role->dma_cfg.ch);
        }
    }
    TRACE("dump cfg.........end");
}
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
static void af_codec_output_gain_changed(float coef)
{
#if defined(AUDIO_OUTPUT_PA_ON_FADE_IN) || defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT)
    saved_output_coef = coef;
    if (dac_pa_state != AF_DAC_PA_NULL) {
        return;
    }
#endif

    new_output_coef[0] = coef;
#ifdef CHIP_BEST1000
    new_output_coef[1] = new_output_coef[0] * AF_CODEC_RIGHT_CHAN_ATTN;
#else
    new_output_coef[1] = coef;
#endif
}

static float af_codec_update_output_gain(int16_t val, int chan)
{
    float cur_gain;
    float tgt_gain;

    cur_gain = cur_output_coef[chan];
    tgt_gain = new_output_coef[chan];

#if defined(AUDIO_OUTPUT_PA_ON_FADE_IN) || defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT)
    float step;

    if (dac_pa_state == AF_DAC_PA_ON_SOFT_START) {
        step = fade_gain_step[chan] * fade_gain_range[chan];
        fade_step_idx[chan]++;
        if (fade_step_idx[chan] >= fade_step_cnt) {
            fade_step_idx[chan] = 0;
            fade_curve_idx[chan]++;
            if (fade_curve_idx[chan] < ARRAY_SIZE(fade_curve) - 1) {
                fade_gain_step[chan] = (fade_curve[fade_curve_idx[chan] + 1] - fade_curve[fade_curve_idx[chan]]) / fade_step_cnt;
            }
        }
    } else if (dac_pa_state == AF_DAC_PA_OFF_SOFT_END) {
        step = fade_gain_step[chan] * fade_gain_range[chan];
        fade_step_idx[chan]--;
        if (fade_step_idx[chan] == 0) {
            fade_step_idx[chan] = fade_step_cnt;
            if (fade_curve_idx[chan] > 0) {
                fade_curve_idx[chan]--;
            }
            if (fade_curve_idx[chan] > 0) {
                fade_gain_step[chan] = (fade_curve[fade_curve_idx[chan]] - fade_curve[fade_curve_idx[chan] - 1]) / fade_step_cnt;
            }
        }
    } else if (dac_pa_state == AF_DAC_PA_NULL) {
        step = AF_CODEC_VOL_UPDATE_STEP;
    } else {
        return cur_gain;
    }
#else
    static const float step = AF_CODEC_VOL_UPDATE_STEP;
#endif

    if (cur_gain > tgt_gain) {
        cur_gain -= step;
        if (cur_gain < tgt_gain) {
            cur_gain = tgt_gain;
        }
    } else {
        cur_gain += step;
        if (cur_gain > tgt_gain) {
            cur_gain = tgt_gain;
        }
    }

    return cur_gain;
}
#endif // AUDIO_OUTPUT_SW_GAIN

static void af_codec_post_data_loop(uint8_t *buf, uint32_t size, enum AUD_BITS_T bits, enum AUD_CHANNEL_NUM_T chans)
{
#if defined(AUDIO_OUTPUT_DC_CALIB_SW) || defined(AUDIO_OUTPUT_SW_GAIN) || defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)

    uint32_t cnt;

    if (bits <= AUD_BITS_16) {
        int16_t *ptr16 = (int16_t *)buf;
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
        int16_t dc_l = dac_dc[0];
        int16_t dc_r = dac_dc[1];
#endif
        if (chans == AUD_CHANNEL_NUM_1) {
            cnt = size / sizeof(int16_t);
            while (cnt-- > 0) {
#ifdef AUDIO_OUTPUT_SW_GAIN
                // Update the gain
                if (new_output_coef[0] != cur_output_coef[0]) {
                    cur_output_coef[0] = af_codec_update_output_gain(*ptr16, 0);
                }
                // Apply the gain
                *ptr16 = (int16_t)(*ptr16 * cur_output_coef[0]);
#elif defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
                // Zero-crossing detection
                if (new_data_shift[0] != cur_data_shift[0]) {
                    if ((prev_data[0] >= 0 && ptr16[0] <= 0) || (prev_data[0] <= 0 && ptr16[0] >= 0)) {
                        cur_data_shift[0] = new_data_shift[0];
                    }
                }
                // Save previous data and shift data
                prev_data[0] = *ptr16;
                *ptr16 >>= cur_data_shift[0];
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
                *ptr16 = __SSAT(*ptr16 + dc_l, 16);
#endif
                ptr16++;
            }
        } else {
            cnt = size / sizeof(int16_t) / 2;
            while (cnt-- > 0) {
#ifdef AUDIO_OUTPUT_SW_GAIN
                // Update the gain
                if (new_output_coef[0] != cur_output_coef[0]) {
                    cur_output_coef[0] = af_codec_update_output_gain(*ptr16, 0);
                }
                if (new_output_coef[1] != cur_output_coef[1]) {
                    cur_output_coef[1] = af_codec_update_output_gain(*(ptr16 + 1), 1);
                }
                // Apply the gain
                *ptr16 = (int16_t)(*ptr16 * cur_output_coef[0]);
                *(ptr16 + 1) = (int16_t)(*(ptr16 + 1) * cur_output_coef[1]);
#elif defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
                // Zero-crossing detection
                if (new_data_shift[0] != cur_data_shift[0]) {
                    if ((prev_data[0] >= 0 && ptr16[0] <= 0) || (prev_data[0] <= 0 && ptr16[0] >= 0)) {
                        cur_data_shift[0] = new_data_shift[0];
                    }
                }
                if (new_data_shift[1] != cur_data_shift[1]) {
                    if ((prev_data[1] >= 0 && ptr16[1] <= 0) || (prev_data[1] <= 0 && ptr16[1] >= 0)) {
                        cur_data_shift[1] = new_data_shift[1];
                    }
                }
                // Save previous data and shift data
                prev_data[0] = *ptr16;
                *ptr16 >>= cur_data_shift[0];
                prev_data[1] = *(ptr16 + 1);
                *(ptr16 + 1) >>= cur_data_shift[1];
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
                *ptr16 = __SSAT(*ptr16 + dc_l, 16);
                *(ptr16 + 1) = __SSAT(*(ptr16 + 1) + dc_r, 16);
#endif
                ptr16 += 2;
            }
        }
    } else {
        int32_t *ptr32 = (int32_t *)buf;
        int32_t dac_bits =
#ifdef CHIP_BEST1000
            CODEC_PLAYBACK_BIT_DEPTH;
#else
            24;
#endif
        int32_t val_shift;

        if (dac_bits < 24) {
            val_shift = 24 - dac_bits;
        } else {
            val_shift = 0;
        }

#ifdef AUDIO_OUTPUT_DC_CALIB_SW
        int32_t dc_l = dac_dc[0] << 8;
        int32_t dc_r = dac_dc[1] << 8;
#endif
        if (chans == AUD_CHANNEL_NUM_1) {
            cnt = size / sizeof(int32_t);
            while (cnt-- > 0) {
#ifdef CORRECT_SAMPLE_VALUE
                *ptr32 = ((*ptr32) << (32 - dac_bits)) >> (32 - dac_bits);
#endif
#ifdef AUDIO_OUTPUT_SW_GAIN
                // Update the gain
                if (new_output_coef[0] != cur_output_coef[0]) {
                    cur_output_coef[0] = af_codec_update_output_gain(*ptr32, 0);
                }
                // Apply the gain
                *ptr32 = (int32_t)(*ptr32 * cur_output_coef[0]);
#elif defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
                // Zero-crossing detection
                if (new_data_shift[0] != cur_data_shift[0]) {
                    if ((prev_data[0] >= 0 && ptr32[0] <= 0) || (prev_data[0] <= 0 && ptr32[0] >= 0)) {
                        cur_data_shift[0] = new_data_shift[0];
                    }
                }
                // Save previous data and shift data
                prev_data[0] = *ptr32;
                *ptr32 >>= cur_data_shift[0];
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
                *ptr32 = __SSAT((*ptr32 + dc_l) >> val_shift, dac_bits);
#elif defined(CORRECT_SAMPLE_VALUE)
                *ptr32 = __SSAT(*ptr32 >> val_shift, dac_bits);
#else
                *ptr32 = *ptr32 >> val_shift;
#endif
                ptr32++;
            }
        } else {
            cnt = size / sizeof(int32_t) / 2;
            while (cnt-- > 0) {
#ifdef CORRECT_SAMPLE_VALUE
                *ptr32 = ((*ptr32) << (32 - dac_bits)) >> (32 - dac_bits);
                *(ptr32 + 1) = ((*(ptr32 + 1)) << (32 - dac_bits)) >> (32 - dac_bits);
#endif
#ifdef AUDIO_OUTPUT_SW_GAIN
                // Update the gain
                if (new_output_coef[0] != cur_output_coef[0]) {
                    cur_output_coef[0] = af_codec_update_output_gain(*ptr32 >> 2, 0);
                }
                if (new_output_coef[1] != cur_output_coef[1]) {
                    cur_output_coef[1] = af_codec_update_output_gain(*(ptr32 + 1) >> 2, 1);
                }
                // Apply the gain
                *ptr32 = (int32_t)(*ptr32 * cur_output_coef[0]);
                *(ptr32 + 1) = (int32_t)(*(ptr32 + 1) * cur_output_coef[1]);
#elif defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
                // Zero-crossing detection
                if (new_data_shift[0] != cur_data_shift[0]) {
                    if ((prev_data[0] >= 0 && ptr32[0] <= 0) || (prev_data[0] <= 0 && ptr32[0] >= 0)) {
                        cur_data_shift[0] = new_data_shift[0];
                    }
                }
                if (new_data_shift[1] != cur_data_shift[1]) {
                    if ((prev_data[1] >= 0 && ptr32[1] <= 0) || (prev_data[1] <= 0 && ptr32[1] >= 0)) {
                        cur_data_shift[1] = new_data_shift[1];
                    }
                }
                // Save previous data and shift data
                prev_data[0] = *ptr32;
                *ptr32 >>= cur_data_shift[0];
                prev_data[1] = *(ptr32 + 1);
                *(ptr32 + 1) >>= cur_data_shift[1];
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
                *ptr32 = __SSAT((*ptr32 + dc_l) >> val_shift, dac_bits);
                *(ptr32 + 1) = __SSAT((*(ptr32 + 1) + dc_r) >> val_shift, dac_bits);
#elif defined(CORRECT_SAMPLE_VALUE)
                *ptr32 = __SSAT(*ptr32 >> val_shift, dac_bits);
                *(ptr32 + 1) = __SSAT(*(ptr32 + 1) >> val_shift, dac_bits);
#else
                *ptr32 = *ptr32 >> val_shift;
                *(ptr32 + 1) = *(ptr32 + 1) >> val_shift;
#endif
                ptr32 += 2;
            }
        }
    }

#else // !(dc calib or sw gain or small gain attn)

#ifdef CHIP_BEST1000
    if (bits == AUD_BITS_24 || bits == AUD_BITS_32) {
        uint32_t cnt;
        int32_t *ptr32 = (int32_t *)buf;
        int32_t val_shift;

        if (bits == AUD_BITS_24) {
            val_shift = 24 - CODEC_PLAYBACK_BIT_DEPTH;
        } else {
            val_shift = 32 - CODEC_PLAYBACK_BIT_DEPTH;
        }

        cnt = size / sizeof(int32_t);
        while (cnt-- > 0) {
            *ptr32 >>= val_shift;
            ptr32++;
        }
    }
#endif

#endif // !(dc calib or sw gain or small gain attn)
}

static uint32_t POSSIBLY_UNUSED af_codec_get_sample_count(uint32_t size, enum AUD_BITS_T bits, enum AUD_CHANNEL_NUM_T chans)
{
    uint32_t div;

    if (bits <= AUD_BITS_16) {
        div = 2;
    } else {
        div = 4;
    }
    div *= chans;
    return size / div;
}

static void POSSIBLY_UNUSED af_zero_mem(void *dst, unsigned int size)
{
    uint32_t *d = dst;
    uint32_t count = size / 4;

    while (count--) {
        *d++ = 0;
    }
}

static inline bool af_codec_playback_pre_handler(uint8_t *buf, const struct af_stream_cfg_t *role)
{
    uint32_t POSSIBLY_UNUSED time;

    if (0) {

#ifdef AUDIO_OUTPUT_PA_ON_FADE_IN
    } else if (dac_pa_state == AF_DAC_PA_ON_TRIGGER &&
            (time = hal_sys_timer_get()) - dac_dc_start_time >= AF_CODEC_DC_STABLE_INTERVAL &&
            time - dac_pa_stop_time >= AF_CODEC_PA_RESTART_INTERVAL) {
        analog_aud_codec_speaker_enable(true);
#ifdef AUDIO_OUTPUT_SW_GAIN
        dac_pa_state = AF_DAC_PA_ON_SOFT_START;
        fade_curve_idx[0] = 0;
        fade_step_idx[0] = 0;
        fade_gain_step[0] = (fade_curve[1] - fade_curve[0]) / fade_step_cnt;
        fade_curve_idx[1] = fade_curve_idx[0];
        fade_step_idx[1] = fade_step_idx[0];
        fade_gain_step[1] = fade_gain_step[0];
        fade_sample_proc = 0;
    } else if (dac_pa_state == AF_DAC_PA_ON_SOFT_START) {
        fade_sample_proc += af_codec_get_sample_count(role->dma_buf_size / 2, role->cfg.bits, role->cfg.channel_num);
        if (fade_sample_proc >= fade_sample_cnt) {
            dac_pa_state = AF_DAC_PA_NULL;
            af_codec_output_gain_changed(saved_output_coef);
        }
#else
        dac_pa_state = AF_DAC_PA_NULL;
#endif
#endif // AUDIO_OUTPUT_PA_ON_FADE_IN

#ifdef AUDIO_OUTPUT_PA_OFF_FADE_OUT
    } else if (dac_pa_state == AF_DAC_PA_OFF_TRIGGER) {
#ifdef AUDIO_OUTPUT_SW_GAIN
        dac_pa_state = AF_DAC_PA_OFF_SOFT_END;
        fade_curve_idx[0] = ARRAY_SIZE(fade_curve) - 1;
        fade_step_idx[0] = fade_step_cnt;
        fade_gain_step[0] = (fade_curve[ARRAY_SIZE(fade_curve) - 1] - fade_curve[ARRAY_SIZE(fade_curve) - 2]) / fade_step_cnt;
        fade_curve_idx[1] = fade_curve_idx[0];
        fade_step_idx[1] = fade_step_idx[0];
        fade_gain_step[1] = fade_gain_step[0];
        fade_sample_proc = 0;
    } else if (dac_pa_state == AF_DAC_PA_OFF_SOFT_END) {
        fade_sample_proc += af_codec_get_sample_count(role->dma_buf_size / 2, role->cfg.bits, role->cfg.channel_num);
        if (fade_sample_proc >= fade_sample_cnt) {
            dac_pa_state = AF_DAC_PA_OFF_SOFT_END_DONE;
        }
    } else if (dac_pa_state == AF_DAC_PA_OFF_SOFT_END_DONE) {
#endif
        dac_dc_start_time = hal_sys_timer_get();
        dac_pa_state = AF_DAC_PA_OFF_DC_START;
    } else if (dac_pa_state == AF_DAC_PA_OFF_DC_START) {
        time = hal_sys_timer_get();
        if (time - dac_dc_start_time >= AF_CODEC_DC_STABLE_INTERVAL) {
            dac_pa_state = AF_DAC_PA_NULL;
            analog_aud_codec_speaker_enable(false);
            dac_pa_stop_time = time;
#ifdef AUDIO_OUTPUT_SW_GAIN
            af_codec_output_gain_changed(saved_output_coef);
#endif
#ifdef RTOS
            osSignalSet(fade_thread_id, (1 << AF_FADE_OUT_SIGNAL_ID));
#endif
        }
#endif // AUDIO_OUTPUT_PA_OFF_FADE_OUT

    }

#if defined(AUDIO_OUTPUT_PA_ON_FADE_IN) || defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT)
    if (dac_pa_state == AF_DAC_PA_ON_TRIGGER || dac_pa_state == AF_DAC_PA_OFF_DC_START) {
        af_zero_mem(buf, role->dma_buf_size / 2);
        return true;
    }
#endif

    return false;
}

struct af_stream_cfg_t *post_handler_ext_proc_role = NULL;
uint8_t *post_handler_ext_proc_buf = NULL;


static inline void af_codec_playback_post_handler(uint8_t *buf, const struct af_stream_cfg_t *role)
{
#if !defined(AUDIO_OUTPUT_SW_GAIN) && defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
    // Shift Data right
    new_data_shift[0] = hal_codec_get_alg_dac_shift();
    new_data_shift[1] = new_data_shift[0];
#endif

    af_codec_post_data_loop(buf, role->dma_buf_size / 2, role->cfg.bits, role->cfg.channel_num);

#if defined(CHIP_BEST1000) && defined(AUDIO_OUTPUT_GAIN_M60DB_CHECK)
    hal_codec_dac_gain_m60db_check(HAL_CODEC_PERF_TEST_M60DB);
#endif
}

static inline void af_thread_stream_handler(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t lock;
    struct af_stream_cfg_t *role;
    uint32_t dma_addr, hw_pos;
    uint8_t *buf;
    uint32_t pp_cnt;
    bool codec_playback;
    bool skip_handler;
    enum AF_PP_T pp_index;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    if (role->handler && (role->ctl.status & AF_STATUS_STREAM_START_STOP)) {
        lock = int_lock();
        pp_cnt = role->ctl.pp_cnt;
        role->ctl.pp_cnt = 0;
        int_unlock(lock);

        if (pp_cnt > 1) {
            TRACE("af_thread:WARNING: id=%d stream=%d lost %u signals", id, stream, pp_cnt-1);
            af_buf_err[id][stream] = true;
        } else {
            af_buf_err[id][stream] = false;
        }

        pp_index = PP_PINGPANG(role->ctl.pp_index);

        // Get PP index from accurate DMA pos
        dma_addr = af_stream_get_cur_dma_addr(id, stream);
        hw_pos = dma_addr - (uint32_t)role->dma_buf_ptr;
        if (hw_pos > role->dma_buf_size) {
            TRACE("af_thread: Failed to get valid dma addr for id=%d stream=%d: 0x%08x", id, stream, dma_addr);
        }
#ifndef CHIP_BEST1000
        if (role->cfg.chan_sep_buf && role->cfg.channel_num > AUD_CHANNEL_NUM_1) {
            uint32_t chan_size;

            chan_size = role->dma_buf_size / role->cfg.channel_num;

            if (hw_pos <= role->dma_buf_size) {
                hw_pos = hw_pos % chan_size;
                if (hw_pos < chan_size / 2) {
                    pp_index = PP_PANG;
                } else if (hw_pos < chan_size) {
                    pp_index = PP_PING;
                }
            }
            if (pp_index == PP_PING) {
                buf = role->dma_buf_ptr;
            } else {
                buf = role->dma_buf_ptr + chan_size / 2;
            }
        } else
#endif
        {
            if (hw_pos < role->dma_buf_size / 2) {
                pp_index = PP_PANG;
            } else if (hw_pos < role->dma_buf_size) {
                pp_index = PP_PING;
            }
            if (pp_index == PP_PING) {
                buf = role->dma_buf_ptr;
            } else {
                buf = role->dma_buf_ptr + role->dma_buf_size / 2;
            }
        }

        if (stream == AUD_STREAM_PLAYBACK && role->ctl.use_device == AUD_STREAM_USE_INT_CODEC) {
            codec_playback = true;
#ifdef __ANKER_UI__			
			post_handler_ext_proc_buf = buf;
	        post_handler_ext_proc_role = role;
#endif
        } else {
            codec_playback = false;
#ifdef 	__ANKER_UI__		
			post_handler_ext_proc_buf = NULL;
	        post_handler_ext_proc_role = NULL;
#endif
        }

        skip_handler = false;
        if (codec_playback) {
            skip_handler = af_codec_playback_pre_handler(buf, role);
        }

        if (!skip_handler) {
            role->handler(buf, role->dma_buf_size / 2);
        }

        if (codec_playback) {
#ifndef __ANKER_UI__			
            af_codec_playback_post_handler(buf, role);
#endif
        }

        if (role->ctl.pp_cnt) {
            TRACE("af_thread:WARNING: id=%d stream=%d hdlr ran too long (pp_cnt=%u)", id, stream, role->ctl.pp_cnt);
        }
    }

    af_unlock_thread();
}

#ifdef RTOS

void af_codec_playback_post_handler_ext_proc(void)
{
	if (post_handler_ext_proc_buf && post_handler_ext_proc_role){        
		//TRACE("enter");        
		//DUMP16("%d ", post_handler_ext_proc_buf, 6);    	
		af_codec_playback_post_handler(post_handler_ext_proc_buf, post_handler_ext_proc_role);
		//TRACE("exit");        
		//DUMP16("%d ", post_handler_ext_proc_buf, 6);    
	}
}


static void af_thread(void const *argument)
{
    osEvent evt;
    uint32_t signals = 0;
    enum AUD_STREAM_ID_T id;
    enum AUD_STREAM_T stream;

    while(1)
    {
        //wait any signal
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;
//        TRACE("[%s] status = %x, signals = %d", __func__, evt.status, evt.value.signals);

        if(evt.status == osEventSignal)
        {
            for(uint8_t i=0; i<AUD_STREAM_ID_NUM * AUD_STREAM_NUM; i++)
            {
                if(signals & (1 << i))
                {
                    id = (enum AUD_STREAM_ID_T)(i >> 1);
                    stream = (enum AUD_STREAM_T)(i & 1);

                    af_thread_stream_handler(id, stream);

#ifdef AF_STREAM_ID_0_PLAYBACK_FADEOUT
                    af_stream_stop_process(role, role->dma_buf_ptr + PP_PINGPANG(role->ctl.pp_index) * (role->dma_buf_size / 2), role->dma_buf_size / 2);
#endif
                }
            }
        }
        else
        {
            TRACE("[%s] ERROR: evt.status = %d", __func__, evt.status);
            continue;
        }
    }
}

#else // !RTOS

#include "cmsis.h"
static volatile uint32_t af_flag_open;
static volatile uint32_t af_flag_signal;

static void af_set_flag(volatile uint32_t *flag, uint32_t set)
{
    uint32_t lock;

    lock = int_lock();
    *flag |= set;
    int_unlock(lock);
}

static void af_clear_flag(volatile uint32_t *flag, uint32_t clear)
{
    uint32_t lock;

    lock = int_lock();
    *flag &= ~clear;
    int_unlock(lock);
}

static bool af_flag_active(volatile uint32_t *flag, uint32_t bits)
{
    return !!(*flag & bits);
}

void af_thread(void const *argument)
{
    uint32_t lock;
    uint32_t i;
    enum AUD_STREAM_ID_T id;
    enum AUD_STREAM_T stream;

    if (af_flag_open == 0) {
        return;
    }

    for (i = 0; i < AUD_STREAM_ID_NUM * AUD_STREAM_NUM; i++) {
        if (!af_flag_active(&af_flag_signal, 1 << i)) {
            continue;
        }
        af_clear_flag(&af_flag_signal, 1 << i);

        id = (enum AUD_STREAM_ID_T)(i >> 1);
        stream = (enum AUD_STREAM_T)(i & 1);

        af_thread_stream_handler(id, stream);
    }

    lock = int_lock();
    if (af_flag_signal == 0) {
        hal_cpu_wake_unlock(AF_CPU_WAKE_USER);
    }
    int_unlock(lock);
}

#endif // !RTOS

static void af_dma_irq_handler(uint8_t ch, uint32_t remain_dst_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    struct af_stream_cfg_t *role = NULL;

    //initial parameter
    for(uint8_t id=0; id< AUD_STREAM_ID_NUM; id++)
    {
        for(uint8_t stream=0; stream < AUD_STREAM_NUM; stream++)
        {
            role = af_get_stream_role((enum AUD_STREAM_ID_T)id, (enum AUD_STREAM_T)stream);
//            role = &af_stream[id][stream];

            if(role->dma_cfg.ch == ch)
            {
                role->ctl.pp_index = PP_PINGPANG(role->ctl.pp_index);
                role->ctl.pp_cnt++;
//                TRACE("[%s] id = %d, stream = %d, ch = %d", __func__, id, stream, ch);
//                TRACE("[%s] PLAYBACK pp_cnt = %d", __func__, role->ctl.pp_cnt);
#ifdef RTOS
                osSignalSet(af_thread_tid, 0x01 << (id * 2 + stream));
#else
                af_set_flag(&af_flag_signal, 0x01 << (id * 2 + stream));
                hal_cpu_wake_lock(AF_CPU_WAKE_USER);
#endif
                return;
            }
        }
    }

    //invalid dma irq
    ASSERT(0, "[%s] ERROR: channel id = %d", __func__, ch);
}

#ifdef __CODEC_ASYNC_CLOSE__
static void af_codec_async_close(void)
{
    af_lock_thread();
    codec_int_close(CODEC_CLOSE_ASYNC_REAL);
    af_unlock_thread();
}
#endif

uint32_t af_open(void)
{
    AF_TRACE_DEBUG();
    struct af_stream_cfg_t *role = NULL;

#ifdef RTOS
    if (audioflinger_mutex_id == NULL)
    {
        audioflinger_mutex_id = osMutexCreate((osMutex(audioflinger_mutex)));
    }
#endif

    af_lock_thread();

#ifdef AUDIO_OUTPUT_DC_CALIB
    if (!dac_dc_valid) {
        int16_t dc_l, dc_r;
        uint16_t max_dc;
        float attn;

        dac_dc_valid = true;
        analog_aud_get_dc_calib_value(&dc_l, &dc_r);
        if (ABS(dc_l) >= ABS(dc_r)) {
            max_dc = ABS(dc_l);
        } else {
            max_dc = ABS(dc_r);
        }
        ASSERT(max_dc + 1 < AF_CODEC_DC_MAX_SCALE, "Bad dc values: (%d, %d)", dc_l, dc_r);
        if (max_dc) {
            attn = 1 - (float)(max_dc + 1) / AF_CODEC_DC_MAX_SCALE;
        } else {
            attn = 1;
        }
        hal_codec_set_dac_dc_gain_attn(attn);
#ifdef AUDIO_OUTPUT_DC_CALIB_SW
        dac_dc[0] = dc_l;
        dac_dc[1] = dc_r;
#else
        hal_codec_set_dac_dc_offset(dc_l, dc_r);
#endif
    }
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_output_coef_callback(af_codec_output_gain_changed);
#endif

#ifdef __CODEC_ASYNC_CLOSE__
    codec_int_set_close_handler(af_codec_async_close);
#endif

    //initial parameters
    for(uint8_t id=0; id< AUD_STREAM_ID_NUM; id++)
    {
        for(uint8_t stream=0; stream < AUD_STREAM_NUM; stream++)
        {
            role = af_get_stream_role((enum AUD_STREAM_ID_T)id, (enum AUD_STREAM_T)stream);

            if(role->ctl.status == AF_STATUS_NULL)
            {
                role->dma_buf_ptr = NULL;
                role->dma_buf_size = 0;
                role->ctl.pp_index = PP_PING;
                role->ctl.status = AF_STATUS_OPEN_CLOSE;
                role->ctl.use_device = AUD_STREAM_USE_DEVICE_NULL;
                role->dma_cfg.ch = HAL_DMA_CHAN_NONE;
            }
            else
            {
                ASSERT(0, "[%s] ERROR: id = %d, stream = %d", __func__, id, stream);
            }
        }
    }

#ifdef RTOS
    af_thread_tid = osThreadCreate(osThread(af_thread), NULL);
    af_default_priority = af_get_priority();
    osSignalSet(af_thread_tid, 0x0);
#endif

    af_unlock_thread();

    return AF_RES_SUCCESS;
}

static void af_stream_update_dma_buffer(enum AUD_STREAM_T stream, struct af_stream_cfg_t *role, const struct AF_STREAM_CONFIG_T *cfg)
{
    int i;
    enum HAL_DMA_RET_T dma_ret;
    struct HAL_DMA_DESC_T *dma_desc, *next_desc;
    struct HAL_DMA_CH_CFG_T *dma_cfg;
    int irq;
    uint32_t desc_xfer_size;
    uint32_t align;
    uint8_t samp_size;
    enum HAL_DMA_WDITH_T width;
#ifndef CHIP_BEST1000
    bool dma_2d_en;
    uint32_t chan_desc_xfer_size = 0;
#endif

    dma_desc = &role->dma_desc[0];
    dma_cfg = &role->dma_cfg;

    role->dma_buf_ptr = cfg->data_ptr;
    role->dma_buf_size = cfg->data_size;

    if (cfg->bits == AUD_BITS_24 || cfg->bits == AUD_BITS_32) {
        width = HAL_DMA_WIDTH_WORD;
        samp_size = 4;
    } else if (cfg->bits == AUD_BITS_16) {
        width = HAL_DMA_WIDTH_HALFWORD;
        samp_size = 2;
    } else {
        ASSERT(false, "%s: Invalid stream config bits=%d", __func__, cfg->bits);
        width = HAL_DMA_WIDTH_BYTE;
        samp_size = 1;
    }

#ifdef DYNAMIC_AUDIO_BUFFER_COUNT
    uint32_t desc_cnt;

    desc_cnt = (cfg->data_size / samp_size + HAL_DMA_MAX_DESC_XFER_SIZE - 1) / HAL_DMA_MAX_DESC_XFER_SIZE;
    if (desc_cnt < 2) {
        desc_cnt = 2;
    } else if (desc_cnt & (desc_cnt - 1)) {
        desc_cnt = 1 << (31 - __CLZ(desc_cnt) + 1);
    }
    TRACE("%s: desc_cnt=%u data_size=%u samp_size=%u", __func__, desc_cnt, cfg->data_size, samp_size);
    ASSERT(MIN_AUDIO_BUFFER_COUNT <= desc_cnt && desc_cnt <= MAX_AUDIO_BUFFER_COUNT, "%s: Bad desc_cnt=%u", __func__, desc_cnt);
    role->dma_desc_cnt = desc_cnt;
#endif

    desc_xfer_size = cfg->data_size / AUDIO_BUFFER_COUNT;

#ifndef CHIP_BEST1000
    if (cfg->chan_sep_buf && cfg->channel_num > AUD_CHANNEL_NUM_1) {
        dma_2d_en = true;
    } else {
        dma_2d_en = false;
    }

    if (dma_2d_en) {
        enum HAL_DMA_BSIZE_T bsize;
        uint8_t burst_size;

        chan_desc_xfer_size = desc_xfer_size / cfg->channel_num;

        if (stream == AUD_STREAM_PLAYBACK) {
            bsize = dma_cfg->src_bsize;
        } else {
            bsize = dma_cfg->dst_bsize;
        }
        if (bsize == HAL_DMA_BSIZE_1) {
            burst_size = 1;
        } else if (bsize == HAL_DMA_BSIZE_4) {
            burst_size = 4;
        } else {
            burst_size = 8;
        }
        align = burst_size * samp_size * cfg->channel_num;
        // Ensure word-aligned too
        if (align & 0x1) {
            align *= 4;
        } else if (align & 0x2) {
            align *= 2;
        }
    } else
#endif
    {
        align = 4;
    }
    ASSERT(desc_xfer_size * AUDIO_BUFFER_COUNT == cfg->data_size && (desc_xfer_size % align) == 0,
        "%s: Dma data size is not aligned: data_size=%u AUDIO_BUFFER_COUNT=%u align=%u",
        __func__, cfg->data_size, AUDIO_BUFFER_COUNT, align);

    dma_cfg->dst_width = width;
    dma_cfg->src_width = width;
    dma_cfg->src_tsize = desc_xfer_size / samp_size;

    for (i = 0; i < AUDIO_BUFFER_COUNT; i++) {
        if (i == AUDIO_BUFFER_COUNT - 1) {
            next_desc = &dma_desc[0];
            irq = 1;
        } else {
            next_desc = &dma_desc[i + 1];
            if (i == AUDIO_BUFFER_COUNT / 2 - 1) {
                irq = 1;
            } else {
                irq = 0;
            }
        }

        if (stream == AUD_STREAM_PLAYBACK) {
#ifndef CHIP_BEST1000
            if (dma_2d_en) {
                dma_cfg->src = (uint32_t)(role->dma_buf_ptr + chan_desc_xfer_size * i);
            } else
#endif
            {
                dma_cfg->src = (uint32_t)(role->dma_buf_ptr + desc_xfer_size * i);
            }
            dma_ret = hal_audma_init_desc(&dma_desc[i], dma_cfg, next_desc, irq);
        } else {
#ifndef CHIP_BEST1000
            if (dma_2d_en) {
                dma_cfg->dst = (uint32_t)(role->dma_buf_ptr + chan_desc_xfer_size * i);
            } else
#endif
            {
                dma_cfg->dst = (uint32_t)(role->dma_buf_ptr + desc_xfer_size * i);
            }
            dma_ret = hal_audma_init_desc(&dma_desc[i], dma_cfg, next_desc, irq);
        }

        ASSERT(dma_ret == HAL_DMA_OK, "[%s] Failed to init dma desc for stream %d: ret=%d", __func__, stream, dma_ret);
    }
}

// Support memory<-->peripheral
// Note: Do not support peripheral <--> peripheral
uint32_t af_stream_open(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, const struct AF_STREAM_CONFIG_T *cfg)
{
    AF_TRACE_DEBUG();
    struct af_stream_cfg_t *role;
    enum AF_RESULT_T ret;
    enum AUD_STREAM_USE_DEVICE_T device;
    struct HAL_DMA_CH_CFG_T *dma_cfg = NULL;

    role = af_get_stream_role(id, stream);
    TRACE("[%s] id = %d, stream = %d", __func__, id, stream);

    ASSERT(cfg->data_ptr != NULL, "[%s] ERROR: data_ptr == NULL!!!", __func__);
    ASSERT(cfg->data_size != 0, "[%s] ERROR: data_size == 0!!!", __func__);
#ifndef CHIP_BEST1000
    if (cfg->chan_sep_buf && cfg->channel_num > AUD_CHANNEL_NUM_1) {
        ASSERT(cfg->device == AUD_STREAM_USE_INT_CODEC || cfg->device == AUD_STREAM_USE_I2S_MASTER ||
            cfg->device == AUD_STREAM_USE_I2S_SLAVE, "[%s] ERROR: Unsupport chan_sep_buf for device %d", cfg->device);
    }
#endif

    af_lock_thread();

    //check af is open
    if(role->ctl.status != AF_STATUS_OPEN_CLOSE)
    {
        TRACE("[%s] ERROR: status = %d",__func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

    role->cfg = *cfg;

    device = cfg->device;
    role->ctl.use_device = device;

    dma_cfg = &role->dma_cfg;
    memset(dma_cfg, 0, sizeof(*dma_cfg));
    dma_cfg->dst_bsize = HAL_DMA_BSIZE_4;
#ifndef CHIP_BEST1000
    if (device == AUD_STREAM_USE_INT_CODEC &&
            ((stream == AUD_STREAM_CAPTURE && cfg->chan_sep_buf == false) ||
            (stream == AUD_STREAM_PLAYBACK && cfg->chan_sep_buf && cfg->channel_num > AUD_CHANNEL_NUM_1))) {
        dma_cfg->src_bsize = HAL_DMA_BSIZE_1;
    } else
#endif
    {
        dma_cfg->src_bsize = HAL_DMA_BSIZE_4;
    }
    dma_cfg->try_burst = 1;
    dma_cfg->handler = af_dma_irq_handler;

    if(stream == AUD_STREAM_PLAYBACK)
    {
        AF_TRACE_DEBUG();
        dma_cfg->src_periph = (enum HAL_DMA_PERIPH_T)0;
        dma_cfg->type = HAL_DMA_FLOW_M2P_DMA;

        //open device and stream
        if(0)
        {
        }
#ifdef AF_DEVICE_EXT_CODEC
        else if(device == AUD_STREAM_USE_EXT_CODEC)
        {
            AF_TRACE_DEBUG();
            tlv32aic32_open();
            tlv32aic32_stream_open(stream);

            dma_cfg->dst_periph = HAL_AUDMA_I2S_TX;
        }
#endif
#ifdef AF_DEVICE_I2S
        else if(device == AUD_STREAM_USE_I2S_MASTER || device == AUD_STREAM_USE_I2S_SLAVE)
        {
            AF_TRACE_DEBUG();
            hal_i2s_open(AF_I2S_INST, stream,
                (device == AUD_STREAM_USE_I2S_MASTER) ? HAL_I2S_MODE_MASTER : HAL_I2S_MODE_SLAVE);
            dma_cfg->dst_periph = HAL_AUDMA_I2S_TX;
        }
#endif
#ifdef AF_DEVICE_INT_CODEC
        else if(device == AUD_STREAM_USE_INT_CODEC)
        {
            AF_TRACE_DEBUG();
            codec_int_open();
            codec_int_stream_open(stream);

            dma_cfg->dst_periph = HAL_AUDMA_CODEC_TX;
        }
#endif
#ifdef AF_DEVICE_SPDIF
        else if(device == AUD_STREAM_USE_INT_SPDIF)
        {
            AF_TRACE_DEBUG();
            hal_spdif_open(AF_SPDIF_INST);
            dma_cfg->dst_periph = HAL_AUDMA_SPDIF0_TX;
        }
#endif
#ifdef AF_DEVICE_BT_PCM
        else if(device == AUD_STREAM_USE_BT_PCM)
        {
            AF_TRACE_DEBUG();
            hal_btpcm_open(AF_BTPCM_INST);
            dma_cfg->dst_periph = HAL_AUDMA_BTPCM_TX;
        }
#endif
#ifdef AUDIO_ANC_FB_MC
        else if(device == AUD_STREAM_USE_MC)
        {
            AF_TRACE_DEBUG();
            dma_cfg->dst_periph = HAL_AUDMA_MC_RX;
        }
#endif
		else
		{
			ASSERT(0, "[%s] ERROR: device %d is not defined!", __func__, device);
		}

        dma_cfg->ch = hal_audma_get_chan(dma_cfg->dst_periph, HAL_DMA_HIGH_PRIO);
    }
    else
    {
        AF_TRACE_DEBUG();
        dma_cfg->dst_periph = (enum HAL_DMA_PERIPH_T)0;
        dma_cfg->type = HAL_DMA_FLOW_P2M_DMA;

        //open device and stream
        if(0)
        {
        }
#ifdef AF_DEVICE_EXT_CODEC
        else if(device == AUD_STREAM_USE_EXT_CODEC)
        {
            AF_TRACE_DEBUG();
            tlv32aic32_open();
            tlv32aic32_stream_open(stream);

            dma_cfg->src_periph = HAL_AUDMA_I2S_RX;
        }
#endif
#ifdef AF_DEVICE_I2S
        else if(device == AUD_STREAM_USE_I2S_MASTER || device == AUD_STREAM_USE_I2S_SLAVE)
        {
            AF_TRACE_DEBUG();
            hal_i2s_open(AF_I2S_INST, stream,
                (device == AUD_STREAM_USE_I2S_MASTER) ? HAL_I2S_MODE_MASTER : HAL_I2S_MODE_SLAVE);

            dma_cfg->src_periph = HAL_AUDMA_I2S_RX;
        }
#endif
#ifdef AF_DEVICE_INT_CODEC
        else if(device == AUD_STREAM_USE_INT_CODEC)
        {
            AF_TRACE_DEBUG();
            codec_int_open();
            codec_int_stream_open(stream);

            dma_cfg->src_periph = HAL_AUDMA_CODEC_RX;
        }
#endif
#ifdef AF_DEVICE_SPDIF
        else if(device == AUD_STREAM_USE_INT_SPDIF)
        {
            AF_TRACE_DEBUG();
            hal_spdif_open(AF_SPDIF_INST);

            dma_cfg->src_periph = HAL_AUDMA_SPDIF0_RX;
        }
#endif
#ifdef AF_DEVICE_BT_PCM
        else if(device == AUD_STREAM_USE_BT_PCM)
        {
            AF_TRACE_DEBUG();
            hal_btpcm_open(AF_BTPCM_INST);

            dma_cfg->src_periph = HAL_AUDMA_BTPCM_RX;
        }
#endif
#ifdef AF_DEVICE_DPD_RX
        else if(device == AUD_STREAM_USE_DPD_RX)
        {
            AF_TRACE_DEBUG();
            dma_cfg->src_periph = HAL_AUDMA_DPD_RX;
        }
#endif
        else
        {
            ASSERT(0, "[%s] ERROR: device %d is not defined!", __func__, device);
        }

        dma_cfg->ch = hal_audma_get_chan(dma_cfg->src_periph, HAL_DMA_HIGH_PRIO);
    }

    af_stream_update_dma_buffer(stream, role, cfg);

    role->handler = cfg->handler;

    af_set_status(id, stream, AF_STATUS_STREAM_OPEN_CLOSE);

#ifndef RTOS
    af_clear_flag(&af_flag_signal, 1 << (id * 2 + stream));
    af_set_flag(&af_flag_open, 1 << (id * 2 + stream));
#endif

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    if (ret == AF_RES_SUCCESS) {
        af_stream_setup(id, stream, &role->cfg);
    }

    return ret;
}

//volume, path, sample rate, channel num ...
uint32_t af_stream_setup(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, const struct AF_STREAM_CONFIG_T *cfg)
{
    AF_TRACE_DEBUG();

    struct af_stream_cfg_t *role;
    enum AF_RESULT_T ret;
    enum AUD_STREAM_USE_DEVICE_T device;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    //check stream is open
    if(!(role->ctl.status & AF_STATUS_STREAM_OPEN_CLOSE))
    {
        TRACE("[%s] ERROR: status = %d", __func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

    device = role->ctl.use_device;

    if (&role->cfg != cfg) {
        bool update_dma = false;

        if (role->cfg.bits != cfg->bits) {
            TRACE("[%s] Change bits from %d to %d for stream %d", __func__, role->cfg.bits, cfg->bits, stream);
            update_dma = true;
        }
        if (role->cfg.data_ptr != cfg->data_ptr) {
            TRACE("[%s] Change data_ptr from %p to %p for stream %d", __func__, role->cfg.data_ptr, cfg->data_ptr, stream);
            update_dma = true;
        }
        if (role->cfg.data_size != cfg->data_size) {
            TRACE("[%s] Change data_size from %d to %d for stream %d", __func__, role->cfg.data_size, cfg->data_size, stream);
            update_dma = true;
        }
        if (update_dma) {
            // To avoid FIFO corruption, streams must be stopped before changing sample size
            ASSERT((role->ctl.status & AF_STATUS_STREAM_START_STOP) == 0,
                "[%s] ERROR: Update dma while stream %d started", __func__, stream);

            af_stream_update_dma_buffer(stream, role, cfg);
        }

        if (role->cfg.sample_rate != cfg->sample_rate) {
            TRACE("[%s] Change sample rate from %d to %d for stream %d", __func__, role->cfg.sample_rate, cfg->sample_rate, stream);

            // To avoid L/R sample misalignment, streams must be stopped before changing sample rate
            ASSERT((role->ctl.status & AF_STATUS_STREAM_START_STOP) == 0,
                "[%s] ERROR: Change sample rate from %d to %d while stream %d started", __func__, role->cfg.sample_rate, cfg->sample_rate, stream);
        }

        role->cfg = *cfg;
    }

    AF_TRACE_DEBUG();
    if(0)
    {
    }
#ifdef AF_DEVICE_EXT_CODEC
    else if(device == AUD_STREAM_USE_EXT_CODEC)
    {
        AF_TRACE_DEBUG();

        struct tlv32aic32_config_t tlv32aic32_cfg;

        memset(&tlv32aic32_cfg, 0, sizeof(tlv32aic32_cfg));
        tlv32aic32_cfg.bits = cfg->bits;
        tlv32aic32_cfg.channel_num = cfg->channel_num;
        tlv32aic32_cfg.channel_map = cfg->channel_map;
        tlv32aic32_cfg.sample_rate = cfg->sample_rate;
        tlv32aic32_cfg.use_dma = AF_TRUE;
        tlv32aic32_cfg.chan_sep_buf = cfg->chan_sep_buf;
        tlv32aic32_cfg.i2s_master_clk_wait = cfg->i2s_master_clk_wait;
        tlv32aic32_cfg.i2s_sample_cycles = cfg->i2s_sample_cycles;
        tlv32aic32_stream_setup(stream, &tlv32aic32_cfg);
    }
#endif
#ifdef AF_DEVICE_I2S
    else if(device == AUD_STREAM_USE_I2S_MASTER || device == AUD_STREAM_USE_I2S_SLAVE)
    {
        AF_TRACE_DEBUG();

        struct HAL_I2S_CONFIG_T i2s_cfg;

        memset(&i2s_cfg, 0, sizeof(i2s_cfg));
        i2s_cfg.use_dma = AF_TRUE;
        i2s_cfg.chan_sep_buf = cfg->chan_sep_buf;
        i2s_cfg.master_clk_wait = cfg->i2s_master_clk_wait;
        i2s_cfg.cycles = cfg->i2s_sample_cycles;
        i2s_cfg.bits = cfg->bits;
        i2s_cfg.channel_num = cfg->channel_num;
        i2s_cfg.channel_map = cfg->channel_map;
        i2s_cfg.sample_rate = cfg->sample_rate;
        hal_i2s_setup_stream(AF_I2S_INST, stream, &i2s_cfg);
    }
#endif
#ifdef AF_DEVICE_INT_CODEC
    else if(device == AUD_STREAM_USE_INT_CODEC)
    {
        AF_TRACE_DEBUG();
        struct HAL_CODEC_CONFIG_T codec_cfg;

        memset(&codec_cfg, 0, sizeof(codec_cfg));
        codec_cfg.bits = cfg->bits;
        codec_cfg.sample_rate = cfg->sample_rate;
        codec_cfg.channel_num = cfg->channel_num;
        codec_cfg.channel_map = cfg->channel_map;
        codec_cfg.use_dma = AF_TRUE;
        codec_cfg.vol = cfg->vol;
        codec_cfg.io_path = cfg->io_path;

        AF_TRACE_DEBUG();
        codec_int_stream_setup(stream, &codec_cfg);
    }
#endif
#ifdef AF_DEVICE_SPDIF
    else if(device == AUD_STREAM_USE_INT_SPDIF)
    {
        AF_TRACE_DEBUG();
        struct HAL_SPDIF_CONFIG_T spdif_cfg;

        memset(&spdif_cfg, 0, sizeof(spdif_cfg));
        spdif_cfg.use_dma = AF_TRUE;
        spdif_cfg.bits = cfg->bits;
        spdif_cfg.channel_num = cfg->channel_num;
        spdif_cfg.sample_rate = cfg->sample_rate;
        hal_spdif_setup_stream(AF_SPDIF_INST, stream, &spdif_cfg);
    }
#endif
#ifdef AF_DEVICE_BT_PCM
    else if(device == AUD_STREAM_USE_BT_PCM)
    {
        AF_TRACE_DEBUG();
        struct HAL_BTPCM_CONFIG_T btpcm_cfg;

        memset(&btpcm_cfg, 0, sizeof(btpcm_cfg));
        btpcm_cfg.use_dma = AF_TRUE;
        btpcm_cfg.bits = cfg->bits;
        btpcm_cfg.channel_num = cfg->channel_num;
        btpcm_cfg.sample_rate = cfg->sample_rate;
        hal_btpcm_setup_stream(AF_BTPCM_INST, stream, &btpcm_cfg);
    }
#endif
#ifdef AF_DEVICE_DPD_RX
    else if(device == AUD_STREAM_USE_DPD_RX)
    {
        AF_TRACE_DEBUG();
    }
#endif
#ifdef AUDIO_ANC_FB_MC
    else if(device == AUD_STREAM_USE_MC)
    {
        AF_TRACE_DEBUG();
        hal_codec_setup_mc(cfg->channel_num, cfg->bits);
    }
#endif
	else
	{
		ASSERT(0, "[%s] ERROR: device %d is not defined!", __func__, device);
	}

    AF_TRACE_DEBUG();

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    return ret;
}

uint32_t af_stream_mute(enum AUD_STREAM_USE_DEVICE_T device, enum AUD_STREAM_T stream, bool mute)
{
    AF_TRACE_DEBUG();

    af_lock_thread();

    if (device == AUD_STREAM_USE_INT_CODEC) {
        codec_int_stream_mute(stream, mute);
    }

    af_unlock_thread();

    return AF_RES_SUCCESS;
}

#ifdef CODEC_PLAY_BEFORE_CAPTURE
static struct af_stream_cfg_t *codec_capture_role;

static void af_codec_stream_pre_start(enum AUD_STREAM_T stream)
{
    struct af_stream_cfg_t *role = NULL;

    if (stream == AUD_STREAM_CAPTURE) {
        return;
    }

    for(uint8_t id=0; id< AUD_STREAM_ID_NUM; id++)
    {
        role = af_get_stream_role((enum AUD_STREAM_ID_T)id, AUD_STREAM_CAPTURE);
        if (role->cfg.device == AUD_STREAM_USE_INT_CODEC &&
                role->ctl.status == (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP)) {
            hal_audma_stop(role->dma_cfg.ch);
            codec_int_stream_stop(AUD_STREAM_CAPTURE);
            codec_capture_role = role;
            return;
        }
    }
}

static void af_codec_stream_post_start(enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_CAPTURE) {
        return;
    }

    if (codec_capture_role) {
        hal_audma_sg_start(&codec_capture_role->dma_desc[0], &codec_capture_role->dma_cfg);
        codec_int_stream_start(AUD_STREAM_CAPTURE);
        codec_capture_role = NULL;
    }
}

static void af_codec_stream_pre_stop(enum AUD_STREAM_T stream)
{
    struct af_stream_cfg_t *role = NULL;

    if (stream == AUD_STREAM_CAPTURE) {
        return;
    }

    for(uint8_t id=0; id< AUD_STREAM_ID_NUM; id++)
    {
        role = af_get_stream_role((enum AUD_STREAM_ID_T)id, AUD_STREAM_CAPTURE);
        if (role->cfg.device == AUD_STREAM_USE_INT_CODEC &&
                role->ctl.status == (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP)) {
            hal_audma_stop(role->dma_cfg.ch);
            codec_int_stream_stop(AUD_STREAM_CAPTURE);
            codec_capture_role = role;
            return;
        }
    }
}

static void af_codec_stream_post_stop(enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_CAPTURE) {
        return;
    }

    if (codec_capture_role) {
        hal_audma_sg_start(&codec_capture_role->dma_desc[0], &codec_capture_role->dma_cfg);
        codec_int_stream_start(AUD_STREAM_CAPTURE);
        codec_capture_role = NULL;
    }
}
#endif

uint32_t af_stream_start(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    AF_TRACE_DEBUG();
    struct af_stream_cfg_t *role;
    enum AF_RESULT_T ret;
    enum AUD_STREAM_USE_DEVICE_T device;
    enum HAL_DMA_RET_T dma_ret;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    //check stream is open and not start.
    if(role->ctl.status != (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE))
    {
        TRACE("[%s] ERROR: status = %d",__func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

    device = role->ctl.use_device;

    role->ctl.pp_index = PP_PING;
    role->ctl.pp_cnt = 0;

#ifndef RTOS
    af_clear_flag(&af_flag_signal, 1 << (id * 2 + stream));
#endif

    if (device == AUD_STREAM_USE_INT_CODEC && stream == AUD_STREAM_PLAYBACK) {
#ifdef AUDIO_OUTPUT_SW_GAIN
#if defined(AUDIO_OUTPUT_PA_ON_FADE_IN) || defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT)
        // Fade in
        cur_output_coef[0] = 0;
        cur_output_coef[1] = 0;
#else
        cur_output_coef[0] = new_output_coef[0];
        cur_output_coef[1] = new_output_coef[1];
#endif
#elif defined(AUDIO_OUTPUT_SMALL_GAIN_ATTN)
        // Avoid pop sound when enabling zero-crossing gain adjustment
        if (role->dma_buf_ptr && role->dma_buf_size >= 16) {
            int32_t *buf = (int32_t *)role->dma_buf_ptr;
            buf[0] = buf[1] = -1;
            buf[2] = buf[3] = 0;
        }
        // Init data shift values
        new_data_shift[0] = hal_codec_get_alg_dac_shift();
        new_data_shift[1] = new_data_shift[0];
        cur_data_shift[0] = new_data_shift[0];
        cur_data_shift[1] = new_data_shift[1];
        prev_data[0] = 0;
        prev_data[1] = 0;
#endif

#ifdef AUDIO_OUTPUT_PA_ON_FADE_IN
#ifdef AUDIO_OUTPUT_SW_GAIN
        fade_sample_cnt = (role->cfg.sample_rate + (1000 - 1)) / 1000 * AF_CODEC_FADE_IN_MS;
        if (fade_sample_cnt < AF_CODEC_FADE_MIN_SAMPLE_CNT) {
            fade_sample_cnt = AF_CODEC_FADE_MIN_SAMPLE_CNT;
        }
        fade_step_cnt = fade_sample_cnt / (ARRAY_SIZE(fade_curve) - 1);
        if (fade_step_cnt == 0) {
            fade_step_cnt = 1;
        }

        fade_gain_range[0] = new_output_coef[0];
        fade_gain_range[1] = new_output_coef[1];
#endif
        dac_pa_state = AF_DAC_PA_ON_TRIGGER;
        // Has the buffer been zero-ed out?
        af_zero_mem(role->dma_buf_ptr, role->dma_buf_size);
        af_codec_post_data_loop(role->dma_buf_ptr, role->dma_buf_size, role->cfg.bits, role->cfg.channel_num);
        dac_dc_start_time = hal_sys_timer_get();
#endif
    }

#ifndef CHIP_BEST1000
    if (role->cfg.chan_sep_buf && role->cfg.channel_num > AUD_CHANNEL_NUM_1) {
        struct HAL_DMA_2D_CFG_T *src, *dst;
        struct HAL_DMA_2D_CFG_T dma_2d_cfg;
        uint8_t burst_size = 1;
        uint32_t chan_xfer_cnt;

        src = NULL;
        dst = NULL;
        if (stream == AUD_STREAM_PLAYBACK) {
            if (role->cfg.channel_num == AUD_CHANNEL_NUM_2) {
                ASSERT(role->dma_cfg.src_bsize == HAL_DMA_BSIZE_1,
                    "Play 2D DMA: Bad src burst size: %d", role->dma_cfg.src_bsize);
                burst_size = 1;
                src = &dma_2d_cfg;
            }
        } else {
            if (role->cfg.channel_num > AUD_CHANNEL_NUM_1) {
                ASSERT(role->dma_cfg.src_bsize == role->dma_cfg.dst_bsize,
                    "Cap 2D DMA: src burst size (%d) != dst (%d)", role->dma_cfg.src_bsize, role->dma_cfg.dst_bsize);
                if (role->dma_cfg.dst_bsize == HAL_DMA_BSIZE_1) {
                    burst_size = 1;
                } else if (role->dma_cfg.dst_bsize == HAL_DMA_BSIZE_4) {
                    burst_size = 4;
                } else if (role->dma_cfg.dst_bsize == HAL_DMA_BSIZE_8) {
                    burst_size = 8;
                } else {
                    ASSERT(false, "Cap 2D DMA: Bad dst burst size: %d", role->dma_cfg.dst_bsize);
                }
                dst = &dma_2d_cfg;
            }
        }

        if (src || dst) {
            chan_xfer_cnt = role->dma_cfg.src_tsize * AUDIO_BUFFER_COUNT / role->cfg.channel_num;
            dma_2d_cfg.xcount = role->cfg.channel_num;
            dma_2d_cfg.xmodify = chan_xfer_cnt - burst_size;
            dma_2d_cfg.ycount = chan_xfer_cnt / burst_size;
            dma_2d_cfg.ymodify = -chan_xfer_cnt * (role->cfg.channel_num - 1);
        }

        dma_ret = hal_dma_sg_2d_start(&role->dma_desc[0], &role->dma_cfg, src, dst);
    } else
#endif
    {
        dma_ret = hal_dma_sg_start(&role->dma_desc[0], &role->dma_cfg);
    }
    ASSERT(dma_ret == HAL_DMA_OK, "[%s] Failed to start dma for stream %d: ret=%d", __func__, stream, dma_ret);

    AF_TRACE_DEBUG();
    if(0)
    {
    }
#ifdef AF_DEVICE_EXT_CODEC
    else if(device == AUD_STREAM_USE_EXT_CODEC)
    {
        AF_TRACE_DEBUG();
        tlv32aic32_stream_start(stream);
    }
#endif
#ifdef AF_DEVICE_I2S
    else if(device == AUD_STREAM_USE_I2S_MASTER || device == AUD_STREAM_USE_I2S_SLAVE)
    {
        hal_i2s_start_stream(AF_I2S_INST, stream);
    }
#endif
#ifdef AF_DEVICE_INT_CODEC
    else if(device == AUD_STREAM_USE_INT_CODEC)
    {
        AF_TRACE_DEBUG();
#ifdef CODEC_PLAY_BEFORE_CAPTURE
        af_codec_stream_pre_start(stream);
#endif
        codec_int_stream_start(stream);
#ifdef CODEC_PLAY_BEFORE_CAPTURE
        af_codec_stream_post_start(stream);
#endif
    }
#endif
#ifdef AF_DEVICE_SPDIF
    else if(device == AUD_STREAM_USE_INT_SPDIF)
    {
        AF_TRACE_DEBUG();
        hal_spdif_start_stream(AF_SPDIF_INST, stream);
    }
#endif
#ifdef AF_DEVICE_BT_PCM
    else if(device == AUD_STREAM_USE_BT_PCM)
    {
        AF_TRACE_DEBUG();
        hal_btpcm_start_stream(AF_BTPCM_INST, stream);
    }
#endif
#ifdef AF_DEVICE_DPD_RX
    else if(device == AUD_STREAM_USE_DPD_RX)
    {
        AF_TRACE_DEBUG();
        hal_btpcm_start_stream(AF_BTPCM_INST, stream);
    }
#endif
#ifdef AUDIO_ANC_FB_MC
    else if(device == AUD_STREAM_USE_MC)
    {
        AF_TRACE_DEBUG();
    }
#endif
    else
    {
        ASSERT(0, "[%s] ERROR: device %d is not defined!", __func__, device);
    }

    AF_TRACE_DEBUG();
    af_set_status(id, stream, AF_STATUS_STREAM_START_STOP);

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    return ret;
}

uint32_t af_stream_stop(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    AF_TRACE_DEBUG();
    struct af_stream_cfg_t *role;
    enum AF_RESULT_T ret;
    enum AUD_STREAM_USE_DEVICE_T device;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    device = role->ctl.use_device;

    if (device == AUD_STREAM_USE_INT_CODEC &&
            role->ctl.status == (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP | AF_STATUS_STREAM_PAUSE_RESTART))
    {
        af_clear_status(id, stream, AF_STATUS_STREAM_PAUSE_RESTART);
        goto _pause_stop;
    }

    //check stream is start and not stop
    if (role->ctl.status != (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP))
    {
        TRACE("[%s] ERROR: status = %d",__func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

#if defined(RTOS) && defined(AF_STREAM_ID_0_PLAYBACK_FADEOUT)
    if (id == AUD_STREAM_ID_0 && stream == AUD_STREAM_PLAYBACK){
        af_stream_fadeout_start(512);
        af_stream_stop_wait_finish();
    }
#endif

    if (device == AUD_STREAM_USE_INT_CODEC && stream == AUD_STREAM_PLAYBACK) {
#ifdef AUDIO_OUTPUT_PA_OFF_FADE_OUT
#ifdef AUDIO_OUTPUT_SW_GAIN
        // Fade out
        fade_sample_cnt = (role->cfg.sample_rate + (1000 - 1)) / 1000 * AF_CODEC_FADE_OUT_MS;
        if (fade_sample_cnt < AF_CODEC_FADE_MIN_SAMPLE_CNT) {
            fade_sample_cnt = AF_CODEC_FADE_MIN_SAMPLE_CNT;
        }
        fade_step_cnt = fade_sample_cnt / (ARRAY_SIZE(fade_curve) - 1);
        if (fade_step_cnt == 0) {
            fade_step_cnt = 1;
        }

        new_output_coef[0] = 0;
        new_output_coef[1] = 0;
        fade_gain_range[0] = cur_output_coef[0];
        fade_gain_range[1] = cur_output_coef[1];
#ifdef RTOS
        fade_thread_id = osThreadGetId();
        osSignalClear(fade_thread_id, (1 << AF_FADE_OUT_SIGNAL_ID));
#endif
#endif
        dac_pa_state = AF_DAC_PA_OFF_TRIGGER;
        af_unlock_thread();
        while (dac_pa_state != AF_DAC_PA_NULL) {
#ifdef RTOS
            osSignalWait((1 << AF_FADE_OUT_SIGNAL_ID), 300);
            osSignalClear(fade_thread_id, (1 << AF_FADE_OUT_SIGNAL_ID));
#else
            af_thread(NULL);
#endif
        }
        af_lock_thread();
#elif defined(AUDIO_OUTPUT_PA_ON_FADE_IN)
        dac_pa_state = AF_DAC_PA_NULL;
        analog_aud_codec_speaker_enable(false);
        dac_pa_stop_time = hal_sys_timer_get();
#endif // !AUDIO_OUTPUT_PA_OFF_FADE_OUT && AUDIO_OUTPUT_PA_ON_FADE_IN
    }

    hal_audma_stop(role->dma_cfg.ch);

#if defined(RTOS) && defined(AF_STREAM_ID_0_PLAYBACK_FADEOUT)
    if (id == AUD_STREAM_ID_0 && stream == AUD_STREAM_PLAYBACK){
        af_stream_fadeout_stop();
    }
#endif

    if(0)
    {
    }
#ifdef AF_DEVICE_EXT_CODEC
    else if(device == AUD_STREAM_USE_EXT_CODEC)
    {
        AF_TRACE_DEBUG();
        tlv32aic32_stream_stop(stream);
    }
#endif
#ifdef AF_DEVICE_I2S
    else if(device == AUD_STREAM_USE_I2S_MASTER || device == AUD_STREAM_USE_I2S_SLAVE)
    {
        AF_TRACE_DEBUG();
        hal_i2s_stop_stream(AF_I2S_INST, stream);
    }
#endif
#ifdef AF_DEVICE_INT_CODEC
    else if(device == AUD_STREAM_USE_INT_CODEC)
    {
        AF_TRACE_DEBUG();
#ifdef CODEC_PLAY_BEFORE_CAPTURE
        af_codec_stream_pre_stop(stream);
#endif
        codec_int_stream_stop(stream);
#ifdef CODEC_PLAY_BEFORE_CAPTURE
        af_codec_stream_post_stop(stream);
#endif
    }
#endif
#ifdef AF_DEVICE_SPDIF
    else if(device == AUD_STREAM_USE_INT_SPDIF)
    {
        AF_TRACE_DEBUG();
        hal_spdif_stop_stream(AF_SPDIF_INST, stream);
    }
#endif
#ifdef AF_DEVICE_BT_PCM
    else if(device == AUD_STREAM_USE_BT_PCM)
    {
        AF_TRACE_DEBUG();
        hal_btpcm_stop_stream(AF_BTPCM_INST, stream);
    }
#endif
#ifdef AF_DEVICE_DPD_RX
    else if(device == AUD_STREAM_USE_DPD_RX)
    {
        AF_TRACE_DEBUG();
        hal_btpcm_stop_stream(AF_BTPCM_INST, stream);
    }
#endif
#ifdef AUDIO_ANC_FB_MC
    else if(device == AUD_STREAM_USE_MC)
    {
        AF_TRACE_DEBUG();
    }
#endif
    else
    {
        ASSERT(0, "[%s] ERROR: device %d is not defined!", __func__, device);
    }

_pause_stop:
    AF_TRACE_DEBUG();
    af_clear_status(id, stream, AF_STATUS_STREAM_START_STOP);

#ifndef RTOS
    af_clear_flag(&af_flag_signal, 1 << (id * 2 + stream));
#endif

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    return ret;
}

uint32_t af_stream_pause(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
#ifdef CHIP_BEST1000

    struct af_stream_cfg_t *role = NULL;
    enum AF_RESULT_T ret;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    if (role->cfg.device != AUD_STREAM_USE_INT_CODEC)
    {
        ret = AF_RES_FAILD;
        goto _exit;
    }
    if (role->ctl.status != (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP))
    {
        TRACE("[%s] ERROR: status = %d",__func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

#if defined(AUDIO_OUTPUT_PA_ON_FADE_IN) || defined(AUDIO_OUTPUT_PA_OFF_FADE_OUT)
    dac_pa_state = AF_DAC_PA_NULL;
    analog_aud_codec_speaker_enable(false);
    dac_pa_stop_time = hal_sys_timer_get();
#endif

    hal_audma_stop(role->dma_cfg.ch);

#ifndef FPGA
    codec_int_stream_stop(stream);
#endif

    af_set_status(id, stream, AF_STATUS_STREAM_PAUSE_RESTART);

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    return ret;

#else

    return af_stream_stop(id, stream);

#endif
}

uint32_t af_stream_restart(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
#ifdef CHIP_BEST1000

    struct af_stream_cfg_t *role = NULL;
    enum AF_RESULT_T ret;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    if (role->cfg.device != AUD_STREAM_USE_INT_CODEC)
    {
        ret = AF_RES_FAILD;
        goto _exit;
    }
    if (role->ctl.status != (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP | AF_STATUS_STREAM_PAUSE_RESTART))
    {
        TRACE("[%s] ERROR: status = %d",__func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

    role->ctl.pp_index = PP_PING;
    role->ctl.pp_cnt = 0;

#ifdef AUDIO_OUTPUT_PA_ON_FADE_IN
    dac_pa_state = AF_DAC_PA_ON_TRIGGER;
    af_zero_mem(role->dma_buf_ptr, role->dma_buf_size);
    af_codec_post_data_loop(role->dma_buf_ptr, role->dma_buf_size, role->cfg.bits, role->cfg.channel_num);
    dac_dc_start_time = hal_sys_timer_get();
#endif

    hal_audma_sg_start(&role->dma_desc[0], &role->dma_cfg);

#ifndef FPGA
    codec_int_stream_start(stream);
#endif

    af_clear_status(id, stream, AF_STATUS_STREAM_PAUSE_RESTART);

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    return ret;

#else

    return af_stream_start(id, stream);

#endif
}

uint32_t af_stream_close(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    AF_TRACE_DEBUG();
    struct af_stream_cfg_t *role;
    enum AF_RESULT_T ret;
    enum AUD_STREAM_USE_DEVICE_T device;

    role = af_get_stream_role(id, stream);

    af_lock_thread();

    //check stream is stop and not close.
    if(role->ctl.status != (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE))
    {
        TRACE("[%s] ERROR: status = %d",__func__, role->ctl.status);
        ret = AF_RES_FAILD;
        goto _exit;
    }

    device = role->ctl.use_device;

    memset(role->dma_buf_ptr, 0, role->dma_buf_size);
    hal_audma_free_chan(role->dma_cfg.ch);

    //	TODO: more parameter should be set!!!
//    memset(role, 0xff, sizeof(struct af_stream_cfg_t));
    role->handler = NULL;
    role->ctl.pp_index = PP_PING;
    role->ctl.use_device = AUD_STREAM_USE_DEVICE_NULL;
    role->dma_buf_ptr = NULL;
    role->dma_buf_size = 0;

    role->dma_cfg.ch = HAL_DMA_CHAN_NONE;

    if(0)
    {
    }
#ifdef AF_DEVICE_EXT_CODEC
    else if(device == AUD_STREAM_USE_EXT_CODEC)
    {
        AF_TRACE_DEBUG();
        tlv32aic32_stream_close(stream);
    }
#endif
#ifdef AF_DEVICE_I2S
    else if(device == AUD_STREAM_USE_I2S_MASTER || device == AUD_STREAM_USE_I2S_SLAVE)
    {
        AF_TRACE_DEBUG();
        hal_i2s_close(AF_I2S_INST, stream);
    }
#endif
#ifdef AF_DEVICE_INT_CODEC
    else if(device == AUD_STREAM_USE_INT_CODEC)
    {
        AF_TRACE_DEBUG();
		codec_int_stream_close(stream);
		codec_int_close(CODEC_CLOSE_NORMAL);
    }
#endif
#ifdef AF_DEVICE_SPDIF
    else if(device == AUD_STREAM_USE_INT_SPDIF)
    {
        AF_TRACE_DEBUG();
        hal_spdif_close(AF_SPDIF_INST);
    }
#endif
#ifdef AF_DEVICE_BT_PCM
    else if(device == AUD_STREAM_USE_BT_PCM)
    {
        AF_TRACE_DEBUG();
        hal_btpcm_close(AF_BTPCM_INST);
    }
#endif
#ifdef AF_DEVICE_DPD_RX
    else if(device == AUD_STREAM_USE_DPD_RX)
    {
        AF_TRACE_DEBUG();
    }
#endif
#ifdef AUDIO_ANC_FB_MC
    else if(device == AUD_STREAM_USE_MC)
    {
        AF_TRACE_DEBUG();
    }
#endif
    else
    {
        ASSERT(0, "[%s] ERROR: device %d is not defined!", __func__, device);
    }

    AF_TRACE_DEBUG();
    af_clear_status(id, stream, AF_STATUS_STREAM_OPEN_CLOSE);

#ifndef RTOS
    af_clear_flag(&af_flag_open, 1 << (id * 2 + stream));
    if (af_flag_open == 0) {
        hal_cpu_wake_unlock(AF_CPU_WAKE_USER);
    }
#endif

    ret = AF_RES_SUCCESS;

_exit:
    af_unlock_thread();

    return ret;
}

uint32_t af_close(void)
{
    struct af_stream_cfg_t *role;

    // Avoid blocking shutdown process
    //af_lock_thread();

    for (uint8_t id=0; id < AUD_STREAM_ID_NUM; id++)
    {
        for (uint8_t stream=0; stream < AUD_STREAM_NUM; stream++)
        {
            role = af_get_stream_role((enum AUD_STREAM_ID_T)id, (enum AUD_STREAM_T)stream);
            role->ctl.status = AF_STATUS_NULL;
        }
    }

    codec_int_close(CODEC_CLOSE_FORCED);

    //af_unlock_thread();

    return AF_RES_SUCCESS;
}

uint32_t af_stream_get_cur_dma_addr(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct af_stream_cfg_t *role;
    int i;
    uint32_t addr = 0;

    role = af_get_stream_role(id, stream);

    //check stream is start and not stop
    if (role->ctl.status == (AF_STATUS_OPEN_CLOSE | AF_STATUS_STREAM_OPEN_CLOSE | AF_STATUS_STREAM_START_STOP)) {
        if (role->dma_cfg.ch != HAL_DMA_CHAN_NONE) {
            for (i = 0; i < 2; i++) {
                if (stream == AUD_STREAM_PLAYBACK) {
                    addr = hal_audma_get_cur_src_addr(role->dma_cfg.ch);
                } else {
                    addr = hal_audma_get_cur_dst_addr(role->dma_cfg.ch);
                }
                if (addr) {
                    break;
                }
                // Previous link list item was just finished. Read current DMA address again.
            }
        }
    }

    return addr;
}

int af_stream_get_cur_dma_pos(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    struct af_stream_cfg_t *role;
    uint32_t addr;
    int pos;

    role = af_get_stream_role(id, stream);

    addr = af_stream_get_cur_dma_addr(id, stream);

    pos = addr - (uint32_t)role->dma_buf_ptr;

    if (pos < 0 || pos > role->dma_buf_size) {
        return -1;
    }

#ifndef CHIP_BEST1000
    if (role->cfg.chan_sep_buf && role->cfg.channel_num > AUD_CHANNEL_NUM_1) {
        uint32_t chan_size;
        uint8_t chan_idx;
        uint8_t desc_idx;
        uint16_t chan_desc_offset;
        uint16_t chan_desc_xfer_size;
        uint32_t chan_offset;

        chan_size = role->dma_buf_size / role->cfg.channel_num;
        chan_desc_xfer_size = chan_size / AUDIO_BUFFER_COUNT;

        chan_idx = pos / chan_size;
        chan_offset = pos % chan_size;
        desc_idx = chan_offset / chan_desc_xfer_size;
        chan_desc_offset = chan_offset % chan_desc_xfer_size;

        pos = desc_idx * (role->dma_buf_size / AUDIO_BUFFER_COUNT) + chan_idx * chan_desc_xfer_size + chan_desc_offset;
    }
#endif

    return pos;
}

int af_stream_buffer_error(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
    return af_buf_err[id][stream];
}

#ifdef RTOS
void af_set_priority(int priority)
{
    osThreadSetPriority(af_thread_tid, priority);
}

int af_get_priority(void)
{
    return (int)osThreadGetPriority(af_thread_tid);
}

int af_get_default_priority(void)
{
    return af_default_priority;
}

void af_reset_priority(void)
{
    osThreadSetPriority(af_thread_tid, af_default_priority);
}
#endif

void af_codec_tune_resample_rate(enum AUD_STREAM_T stream, float factor)
{
    ASSERT(stream < AUD_STREAM_NUM, "[%s] Bad stream=%d", __func__, stream);

    af_lock_thread();

    hal_codec_tune_resample_rate(stream, factor);

    af_unlock_thread();
}

void af_codec_set_perf_test_power(int type)
{
    af_lock_thread();

    hal_codec_dac_gain_m60db_check((enum HAL_CODEC_PERF_TEST_POWER_T)type);

    af_unlock_thread();
}

void af_codec_set_noise_reduction(bool enable)
{
    af_lock_thread();

    hal_codec_set_noise_reduction(enable);

    af_unlock_thread();
}

void af_codec_sync_config(enum AUD_STREAM_T stream, enum AF_CODEC_SYNC_TYPE_T type, bool enable)
{
#ifndef BEST1000
    af_lock_thread();

    if (stream == AUD_STREAM_PLAYBACK) {
        if (enable) {
            hal_codec_sync_dac_enable((enum HAL_CODEC_SYNC_TYPE_T)type);
        } else {
            hal_codec_sync_dac_disable();
        }
    } else {
        if (enable) {
            hal_codec_sync_adc_enable((enum HAL_CODEC_SYNC_TYPE_T)type);
        } else {
            hal_codec_sync_adc_disable();
        }
    }

    af_unlock_thread();
#endif
}

int af_anc_open(enum ANC_TYPE_T type, enum AUD_SAMPRATE_T play_rate, enum AUD_SAMPRATE_T capture_rate, AF_ANC_HANDLER handler)
{
    AF_TRACE_DEBUG();

    af_lock_thread();

    codec_anc_open(type, play_rate, capture_rate, handler);

    af_unlock_thread();

    return AF_RES_SUCCESS;
}

int af_anc_close(enum ANC_TYPE_T type)
{
    AF_TRACE_DEBUG();

    af_lock_thread();

    codec_anc_close(type);

    af_unlock_thread();

    return AF_RES_SUCCESS;
}

int af_anc_init_close(void)
{
    AF_TRACE_DEBUG();

    af_lock_thread();

    codec_anc_init_close();

    af_unlock_thread();

    return AF_RES_SUCCESS;
}

