#include "plat_types.h"
#include "codec_int.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "stdbool.h"
#include "analog.h"
#include "tgt_hardware.h"

// NOTE: CODEC_PLAY_BEFORE_CAPTURE
// BEST1000 DAC reset will affect ADC, so DAC initialization must be done before ADC.
// BEST1000 version D has fixed this issue, and this workaround can be skipped for future chips.

#define CODEC_TRACE(s, ...)             TRACE(s, ##__VA_ARGS__)

#define CODEC_INT_INST                  HAL_CODEC_ID_0

#ifdef __CODEC_ASYNC_CLOSE__
#include "cmsis_os.h"

#define CODEC_ASYNC_CLOSE_DELAY (5000)

static void codec_timehandler(void const *param);

osTimerDef (CODEC_TIMER, codec_timehandler);
static osTimerId codec_timer;
static CODEC_CLOSE_HANDLER close_hdlr;

enum CODEC_HW_STATE_T {
    CODEC_HW_STATE_CLOSED,
    CODEC_HW_STATE_CLOSE_PENDING,
    CODEC_HW_STATE_OPENED,
};

enum CODEC_HW_STATE_T codec_hw_state = CODEC_HW_STATE_CLOSED;
#endif

struct CODEC_CONFIG_T{
    bool running;
    bool mute_state[AUD_STREAM_NUM];
    struct STREAM_CONFIG_T {
        bool opened;
        bool started;
        struct HAL_CODEC_CONFIG_T codec_cfg;
    } stream_cfg[AUD_STREAM_NUM];
};

static struct CODEC_CONFIG_T codec_int_cfg = {
    .running = false,
    .mute_state = { false, false, },
    //playback
    .stream_cfg[AUD_STREAM_PLAYBACK] = {
        .opened = false,
        .started = false,
        .codec_cfg = {
            .sample_rate = HAL_CODEC_CONFIG_NULL,
        }
    },
    //capture
    .stream_cfg[AUD_STREAM_CAPTURE] = {
        .opened = false,
        .started = false,
        .codec_cfg = {
            .sample_rate = HAL_CODEC_CONFIG_NULL,
        }
    }
};

static enum AUD_SAMPRATE_T codec_anc_samp_rate[AUD_STREAM_NUM];
static CODEC_ANC_HANDLER codec_anc_handler;

uint32_t codec_int_stream_open(enum AUD_STREAM_T stream)
{
    CODEC_TRACE("%s: stream=%d", __func__, stream);
    codec_int_cfg.stream_cfg[stream].opened = true;
    return 0;
}

uint32_t codec_int_stream_setup(enum AUD_STREAM_T stream, struct HAL_CODEC_CONFIG_T *cfg)
{
    enum AUD_CHANNEL_MAP_T ch_map;

    CODEC_TRACE("%s: stream=%d", __func__, stream);

    if (codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate == AUD_SAMPRATE_NULL) {
        // Codec uninitialized -- all config items should be set
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = HAL_CODEC_CONFIG_ALL;
    } else {
        // Codec initialized before -- only different config items need to be set
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = HAL_CODEC_CONFIG_NULL;
#if defined(CHIP_BEST1000) && defined(__AUDIO_RESAMPLE__)
        // Different clock source for audio resample -- Must be reconfigured here
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_SAMPLE_RATE;
#endif
    }

    // Check ANC sample rate
    if (codec_anc_handler && hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC)) {
        if (cfg->sample_rate != codec_anc_samp_rate[stream]) {
            enum AUD_SAMPRATE_T old_play, old_cap;

            old_play = codec_anc_samp_rate[AUD_STREAM_PLAYBACK];
            old_cap = codec_anc_samp_rate[AUD_STREAM_CAPTURE];

            codec_anc_handler(stream, cfg->sample_rate, &codec_anc_samp_rate[AUD_STREAM_PLAYBACK], &codec_anc_samp_rate[AUD_STREAM_CAPTURE]);

            TRACE("%s: ANC sample rates change from %u/%u to %u/%u", __func__, old_play, old_cap,
                codec_anc_samp_rate[AUD_STREAM_PLAYBACK], codec_anc_samp_rate[AUD_STREAM_CAPTURE]);
        }
    }

    // Always config sample rate, for the pll setting might have been changed by the other stream
    CODEC_TRACE("[sample_rate]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate, cfg->sample_rate);
    codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate = cfg->sample_rate;
    codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_SAMPLE_RATE;

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.bits != cfg->bits)
    {
        CODEC_TRACE("[bits]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.bits, cfg->bits);
        codec_int_cfg.stream_cfg[stream].codec_cfg.bits = cfg->bits;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_BITS;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num != cfg->channel_num)
    {
        CODEC_TRACE("[channel_num]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num, cfg->channel_num);
        if (hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC)) {
            ASSERT(cfg->channel_num == AUD_CHANNEL_NUM_2,
                "Invalid audio channel %d for stream %d (ANC on)", cfg->channel_num, stream);
        }
        codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num = cfg->channel_num;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_CHANNEL_NUM;
    }

    ch_map = cfg->channel_map;
    if (ch_map == 0) {
        if (stream == AUD_STREAM_PLAYBACK) {
            ch_map = (enum AUD_CHANNEL_MAP_T)CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV;
        } else {
            if (cfg->io_path == AUD_INPUT_PATH_MAINMIC) {
                ch_map = (enum AUD_CHANNEL_MAP_T)CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV;
            } else if (cfg->io_path == AUD_INPUT_PATH_LINEIN) {
                ch_map = (enum AUD_CHANNEL_MAP_T)CFG_HW_AUD_INPUT_PATH_LINEIN_DEV;
            }
        }
        ch_map &= AUD_CHANNEL_MAP_ALL;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map != ch_map)
    {
        CODEC_TRACE("[channel_map]old = 0x%x, new = 0x%x", codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map, cfg->channel_map);
        codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map = ch_map;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_CHANNEL_MAP;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma != cfg->use_dma)
    {
        CODEC_TRACE("[use_dma]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma, cfg->use_dma);
        codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma = cfg->use_dma;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.vol != cfg->vol)
    {
        CODEC_TRACE("[vol]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.vol, cfg->vol);
        codec_int_cfg.stream_cfg[stream].codec_cfg.vol = cfg->vol;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_VOL;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path != cfg->io_path)
    {
        CODEC_TRACE("[io_path]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.io_path, cfg->io_path);
        if (hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC) && stream == AUD_STREAM_CAPTURE) {
            ASSERT(cfg->io_path == AUD_INPUT_PATH_MAINMIC,
                "Invalid capture io path (ANC on): %d", cfg->io_path);
        }
        codec_int_cfg.stream_cfg[stream].codec_cfg.io_path = cfg->io_path;
    }

    // Not to change vol if muted
    if (codec_int_cfg.mute_state[stream]) {
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag &= ~HAL_CODEC_CONFIG_VOL;
    }

    CODEC_TRACE("[%s]stream = %d, set_flag = %x",__func__,stream,codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag);
    hal_codec_setup_stream(CODEC_INT_INST, stream, &(codec_int_cfg.stream_cfg[stream].codec_cfg));

    return 0;
}

void codec_int_stream_mute(enum AUD_STREAM_T stream, bool mute)
{
    int anc_on;

    CODEC_TRACE("%s: stream=%d mute=%d", __func__, stream, mute);

    if (mute == codec_int_cfg.mute_state[stream]) {
        CODEC_TRACE("[%s] Codec already in mute status: %d", __func__, mute);
        return;
    }

    anc_on = hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC);

    if (stream == AUD_STREAM_PLAYBACK) {
        if (mute) {
            if (!anc_on) {
                analog_aud_codec_mute();
            }
            hal_codec_dac_mute(CODEC_INT_INST);
        } else {
            codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = HAL_CODEC_CONFIG_VOL;
            hal_codec_setup_stream(CODEC_INT_INST, stream, &(codec_int_cfg.stream_cfg[stream].codec_cfg));
            if (!anc_on) {
                analog_aud_codec_nomute();
            }
        }
    }

    codec_int_cfg.mute_state[stream] = mute;
}

static void codec_hw_start(enum AUD_STREAM_T stream)
{
    CODEC_TRACE("%s: stream=%d", __func__, stream);

    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef CODEC_PLAY_BEFORE_CAPTURE
        ASSERT(codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].started == false,
            "DAC should be started before ADC");
#endif

        // Enable DAC before starting stream (specifically before enabling PA)
        analog_aud_codec_dac_enable(true);
    }

    hal_codec_start_stream(CODEC_INT_INST, stream);
}

static void codec_hw_stop(enum AUD_STREAM_T stream)
{
    CODEC_TRACE("%s: stream=%d", __func__, stream);

    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef CODEC_PLAY_BEFORE_CAPTURE
        ASSERT(codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].started == false,
            "DAC should be stopped after ADC");
#endif
    }

    hal_codec_stop_stream(CODEC_INT_INST, stream);

    if (stream == AUD_STREAM_PLAYBACK) {
        // Disable DAC after stopping stream (specifically after disabling PA)
        analog_aud_codec_dac_enable(false);
    }
}

uint32_t codec_int_stream_start(enum AUD_STREAM_T stream)
{
    CODEC_TRACE("%s: stream=%d", __func__, stream);

    codec_int_cfg.stream_cfg[stream].started = true;

    if (stream == AUD_STREAM_CAPTURE) {
        analog_aud_codec_adc_enable(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path,
            codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map, true);
    }
#ifndef ANC_L_R_MISALIGN_WORKAROUND
    else if (stream == AUD_STREAM_PLAYBACK && hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC)) {
        hal_codec_stop_playback_stream(CODEC_INT_INST);
    }
#endif

    hal_codec_start_interface(CODEC_INT_INST, stream,
        codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma);

#ifndef ANC_L_R_MISALIGN_WORKAROUND
    if (!hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC))
#endif
    {
        codec_hw_start(stream);
    }
#ifndef ANC_L_R_MISALIGN_WORKAROUND
    else if (stream == AUD_STREAM_PLAYBACK) {
        hal_codec_start_playback_stream(CODEC_INT_INST);
    }
#endif

    return 0;
}

uint32_t codec_int_stream_stop(enum AUD_STREAM_T stream)
{
    CODEC_TRACE("%s: stream=%d", __func__, stream);

    hal_codec_stop_interface(CODEC_INT_INST, stream);

#ifndef ANC_L_R_MISALIGN_WORKAROUND
    if (!hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC))
#endif
    {
        codec_hw_stop(stream);
    }

    if (stream == AUD_STREAM_CAPTURE) {
        analog_aud_codec_adc_enable(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path,
            codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map, false);
    }

    codec_int_cfg.stream_cfg[stream].started = false;

    return 0;
}

uint32_t codec_int_stream_close(enum AUD_STREAM_T stream)
{
    //close all stream
    CODEC_TRACE("%s: stream=%d", __func__, stream);
    if (codec_int_cfg.stream_cfg[stream].started) {
        codec_int_stream_stop(stream);
    }
    codec_int_cfg.stream_cfg[stream].opened = false;
    return 0;
}

static void codec_hw_open(void)
{
    CODEC_TRACE("%s", __func__);

#ifdef __CODEC_ASYNC_CLOSE__
    if (codec_timer == NULL) {
        codec_timer = osTimerCreate (osTimer(CODEC_TIMER), osTimerOnce, NULL);
    }
    osTimerStop(codec_timer);
#endif

    // Audio resample: Might have different clock source, so must be reconfigured here
    hal_codec_open(CODEC_INT_INST);

#ifdef __CODEC_ASYNC_CLOSE__
    CODEC_TRACE("%s: codec_hw_state=%d", __func__, codec_hw_state);

    if (codec_hw_state == CODEC_HW_STATE_CLOSED) {
        codec_hw_state = CODEC_HW_STATE_OPENED;
        // Continue to open the codec hardware
    } else if (codec_hw_state == CODEC_HW_STATE_CLOSE_PENDING) {
        // Still opened
        codec_hw_state = CODEC_HW_STATE_OPENED;
        return;
    } else {
        // Already opened
        return;
    }
#endif

    hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_CODEC);

    analog_aud_codec_open();
}

static void codec_hw_close(void)
{
    CODEC_TRACE("%s", __func__);

    // Audio resample: Might have different clock source, so close now and reconfigure when open
    hal_codec_close(CODEC_INT_INST);

#ifdef __CODEC_ASYNC_CLOSE__
    CODEC_TRACE("%s: codec_hw_state=%d", __func__, codec_hw_state);

    if (codec_hw_state == CODEC_HW_STATE_OPENED) {
        // Start a timer to close the codec hardware
        codec_hw_state = CODEC_HW_STATE_CLOSE_PENDING;

        osTimerStop(codec_timer);
        osTimerStart(codec_timer, CODEC_ASYNC_CLOSE_DELAY);

        return;
    } else if (codec_hw_state == CODEC_HW_STATE_CLOSE_PENDING) {
        codec_hw_state = CODEC_HW_STATE_CLOSED;
        // Continue to close the codec hardware
    } else {
        // Already closed
        return;
    }
#endif

    analog_aud_codec_close();

    hal_sys_wake_unlock(HAL_SYS_WAKE_LOCK_USER_CODEC);
}

uint32_t codec_int_open(void)
{
    CODEC_TRACE("%s: running=%d", __func__, codec_int_cfg.running);

    if (!codec_int_cfg.running){
        codec_int_cfg.running = true;
#ifndef ANC_L_R_MISALIGN_WORKAROUND
        if (hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC)) {
            CODEC_TRACE("ANC working. Skip codec open");
        } else
#endif
        {
            CODEC_TRACE("trig codec open");
            codec_hw_open();
        }
    }

    return 0;
}

uint32_t codec_int_close(enum CODEC_CLOSE_TYPE_T type)
{
    CODEC_TRACE("%s: type=%d running=%d", __func__, type, codec_int_cfg.running);

    if (0) {
#ifdef __CODEC_ASYNC_CLOSE__
    } else if (type == CODEC_CLOSE_ASYNC_REAL) {
        codec_hw_close();
#endif
    } else if (type == CODEC_CLOSE_FORCED) {
#ifdef __CODEC_ASYNC_CLOSE__
        codec_hw_state = CODEC_HW_STATE_CLOSE_PENDING;
#endif
        codec_hw_close();
    } else {
        if (codec_int_cfg.running &&
                (codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].opened == false) &&
                (codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].opened == false)){
            codec_int_cfg.running = false;
#ifndef ANC_L_R_MISALIGN_WORKAROUND
            if (hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC)) {
                CODEC_TRACE("ANC working. Skip codec close");
            } else
#endif
            {
                CODEC_TRACE("trig codec close");
                codec_hw_close();
            }
        }
    }

    return 0;
}

#ifdef __CODEC_ASYNC_CLOSE__
void codec_int_set_close_handler(CODEC_CLOSE_HANDLER hdlr)
{
    close_hdlr = hdlr;
}

static void codec_timehandler(void const *param)
{
    if (close_hdlr) {
        close_hdlr();
    }
}
#endif

int codec_anc_open(enum ANC_TYPE_T type, enum AUD_SAMPRATE_T dac_rate, enum AUD_SAMPRATE_T adc_rate, CODEC_ANC_HANDLER hdlr)
{
    CODEC_TRACE("%s: type=%d dac_rate=%d adc_rate=%d", __FUNCTION__, type, dac_rate, adc_rate);

    if (hdlr && codec_int_cfg.running) {
        enum AUD_SAMPRATE_T old_play, old_cap;

        old_play = dac_rate;
        old_cap = adc_rate;

        if (codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].opened &&
                dac_rate != codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].codec_cfg.sample_rate) {
            dac_rate = codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].codec_cfg.sample_rate;
            hdlr(AUD_STREAM_PLAYBACK, dac_rate, &dac_rate, &adc_rate);
        }
        if (codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].opened &&
                adc_rate != codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].codec_cfg.sample_rate) {
            adc_rate = codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].codec_cfg.sample_rate;
            hdlr(AUD_STREAM_CAPTURE, adc_rate, &dac_rate, &adc_rate);
        }
        if (old_play != dac_rate || old_cap != adc_rate) {
            TRACE("%s: ANC sample rates change from %u/%u to %u/%u", __func__, old_play, old_cap, dac_rate, adc_rate);
        }
    }

    codec_anc_samp_rate[AUD_STREAM_PLAYBACK] = dac_rate;
    codec_anc_samp_rate[AUD_STREAM_CAPTURE] = adc_rate;
    codec_anc_handler = hdlr;

    hal_cmu_anc_enable(HAL_CMU_ANC_CLK_USER_ANC);

#ifndef ANC_L_R_MISALIGN_WORKAROUND
    enum AUD_STREAM_T stream;

    if (!codec_int_cfg.running) {
        codec_hw_open();
    }

    for (stream = AUD_STREAM_PLAYBACK; stream <= AUD_STREAM_CAPTURE; stream++) {
        if (codec_int_cfg.stream_cfg[stream].opened) {
            ASSERT(
                codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate == ((stream == AUD_STREAM_PLAYBACK) ? dac_rate : adc_rate),
                "Invalid existing ANC sample rate %d for stream %d",
                codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate,
                stream);
            ASSERT(codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num == AUD_CHANNEL_NUM_2,
                "Invalid existing ANC channel num %d for stream %d",
                codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num,
                stream);
            if (stream == AUD_STREAM_CAPTURE) {
                ASSERT(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path == AUD_INPUT_PATH_MAINMIC,
                    "Invalid ANC capture io path: %d",
                    codec_int_cfg.stream_cfg[stream].codec_cfg.io_path);
            }
        } else {
            codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = 0;
            codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate = ((stream == AUD_STREAM_PLAYBACK) ? dac_rate : adc_rate);
            codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_SAMPLE_RATE;
            if (stream == AUD_STREAM_PLAYBACK) {
                codec_int_cfg.stream_cfg[stream].codec_cfg.vol = TGT_VOLUME_LEVEL_10;
            } else {
                codec_int_cfg.stream_cfg[stream].codec_cfg.vol = CODEC_SADC_VOL;
            }
            codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_VOL;
            codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num = AUD_CHANNEL_NUM_2;
            codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;
            codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_CHANNEL_NUM;
            if (stream == AUD_STREAM_CAPTURE) {
                codec_int_cfg.stream_cfg[stream].codec_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
            }

            hal_codec_setup_stream(CODEC_INT_INST, stream, &(codec_int_cfg.stream_cfg[stream].codec_cfg));
        }
    }

    // Start play first, then start capture last
    for (stream = AUD_STREAM_PLAYBACK; stream <= AUD_STREAM_CAPTURE; stream++) {
        if (!codec_int_cfg.stream_cfg[stream].started) {
            codec_hw_start(stream);
        }
    }
#endif

    analog_aud_codec_anc_enable(type, true);

    // Enable pa if dac muted before
    if (codec_int_cfg.mute_state[AUD_STREAM_PLAYBACK]) {
        analog_aud_codec_nomute();
    }

    return 0;
}

int codec_anc_close(enum ANC_TYPE_T type)
{
    CODEC_TRACE("%s", __FUNCTION__);

    hal_codec_anc_dma_disable(CODEC_INT_INST);

    analog_aud_codec_anc_enable(type, false);

#ifndef ANC_L_R_MISALIGN_WORKAROUND
    enum AUD_STREAM_T stream;

    // Stop capture first, then stop play last
    for (stream = AUD_STREAM_CAPTURE; stream >= AUD_STREAM_PLAYBACK && stream <= AUD_STREAM_CAPTURE; stream--) {
        if (!codec_int_cfg.stream_cfg[stream].started) {
            codec_hw_stop(stream);
        }
    }

    if (!codec_int_cfg.running) {
        codec_hw_close();
    }
#endif

    hal_cmu_anc_disable(HAL_CMU_ANC_CLK_USER_ANC);

    // Disable pa if dac muted before
    if (codec_int_cfg.mute_state[AUD_STREAM_PLAYBACK]) {
        analog_aud_codec_mute();
    }

    codec_anc_handler = NULL;

    return 0;
}

int codec_anc_init_close(void)
{
    analog_aud_codec_anc_init_close();
    return 0;
}

