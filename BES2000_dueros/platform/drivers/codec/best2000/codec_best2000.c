#include "plat_types.h"
#include "codec_int.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "stdbool.h"
#include "analog.h"
#include "tgt_hardware.h"
#ifdef __ANKER_UI__
#include "bes_hal_board.h"
#endif
#define CODEC_TRACE(s, ...)             TRACE(s, ##__VA_ARGS__)

#define CODEC_INT_INST                  HAL_CODEC_ID_0

#ifndef CFG_HW_AUD_INPUT_PATH_LINEIN_DEV
#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV    0
#endif

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

static bool anc_ff_enabled;
static bool anc_fb_enabled;
static enum AUD_SAMPRATE_T codec_anc_samp_rate;
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
    }

    // Always config sample rate, for the pll setting might have been changed by the other stream
    CODEC_TRACE("[sample_rate]old = %d, new = %d", codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate, cfg->sample_rate);
    if (hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC)) {
        ASSERT(stream == AUD_STREAM_CAPTURE || cfg->sample_rate >= AUD_SAMPRATE_44100,
            "Invalid audio samp rate %d for stream %d (ANC on)", cfg->sample_rate, stream);

        // Check ANC sample rate
        if (codec_anc_handler) {
            enum AUD_SAMPRATE_T cfg_rate;
            enum AUD_SAMPRATE_T old_rate;

            cfg_rate = hal_codec_anc_convert_rate(cfg->sample_rate);
            if (cfg_rate != codec_anc_samp_rate) {
                old_rate = codec_anc_samp_rate;

                codec_anc_handler(stream, cfg_rate, NULL, NULL);

                codec_anc_samp_rate = cfg_rate;
                TRACE("%s: ANC sample rate changes from %u to %u due to stream=%d samp_rate=%u",
                    __func__, old_rate, codec_anc_samp_rate, stream, cfg->sample_rate);
            }
        }
    }
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
        // Enable DAC before starting stream (specifically before enabling PA)
        analog_aud_codec_dac_enable(true);
    }

    hal_codec_start_stream(CODEC_INT_INST, stream);
}

static void codec_hw_stop(enum AUD_STREAM_T stream)
{
    CODEC_TRACE("%s: stream=%d", __func__, stream);

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

    hal_codec_start_interface(CODEC_INT_INST, stream,
        codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma);

#ifndef ANC_L_R_MISALIGN_WORKAROUND
    if (!hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_ANC))
#endif
    {
        codec_hw_start(stream);
    }

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
#ifdef HW_INT_CODEC_POWER_FROM_EXTRAL
    hal_codec_extral_power_ctrl(1);
#endif
    hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_CODEC);

    hal_codec_open(CODEC_INT_INST);
    analog_aud_codec_open();
}

static void codec_hw_close(void)
{
    CODEC_TRACE("%s", __func__);

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
    hal_codec_close(CODEC_INT_INST);

#ifdef HW_INT_CODEC_POWER_FROM_EXTRAL
    hal_codec_extral_power_ctrl(0);
#endif

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
    bool anc_running = false;

    CODEC_TRACE("%s: type=%d dac_rate=%d adc_rate=%d", __func__, type, dac_rate, adc_rate);

    dac_rate = hal_codec_anc_convert_rate(dac_rate);
    adc_rate = hal_codec_anc_convert_rate(adc_rate);
    ASSERT(dac_rate == adc_rate, "%s: Unmatched rates: dac_rate=%u adc_rate=%u", __func__, dac_rate, adc_rate);

    if (hdlr && codec_int_cfg.running) {
        enum AUD_SAMPRATE_T cfg_dac_rate, cfg_adc_rate;

        if (codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].opened) {
            cfg_dac_rate = hal_codec_anc_convert_rate(codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].codec_cfg.sample_rate);
        } else {
            cfg_dac_rate = dac_rate;
        }
        if (codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].opened) {
            cfg_adc_rate = hal_codec_anc_convert_rate(codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].codec_cfg.sample_rate);
        } else {
            cfg_adc_rate = adc_rate;
        }
        if (codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].opened && codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].opened) {
            ASSERT(cfg_adc_rate == cfg_adc_rate, "%s: Unmatched cfg rates: dac_rate=%u adc_rate=%u", __func__, cfg_dac_rate, cfg_adc_rate);
        } else if (codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].opened) {
            cfg_dac_rate = cfg_adc_rate;
        }
        if (dac_rate != cfg_dac_rate) {
            hdlr(AUD_STREAM_PLAYBACK, cfg_dac_rate, NULL, NULL);
            TRACE("%s: ANC sample rate changes from %u to %u", __func__, dac_rate, cfg_dac_rate);
            dac_rate = cfg_dac_rate;
        }
    }

    codec_anc_samp_rate = dac_rate;
    codec_anc_handler = hdlr;

    if (anc_ff_enabled || anc_fb_enabled) {
        anc_running = true;
    }

    if (type == ANC_FEEDFORWARD) {
        anc_ff_enabled = true;
    } else if (type == ANC_FEEDBACK) {
        anc_fb_enabled = true;
    }

    if (!anc_running) {
        hal_cmu_anc_enable(HAL_CMU_ANC_CLK_USER_ANC);

#ifndef ANC_L_R_MISALIGN_WORKAROUND
        enum AUD_STREAM_T stream;
        enum AUD_CHANNEL_NUM_T play_chan_num;

        if (!codec_int_cfg.running) {
            codec_hw_open();
        }

        if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV == (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
            play_chan_num = AUD_CHANNEL_NUM_2;
        } else {
            play_chan_num = AUD_CHANNEL_NUM_1;
        }

        for (stream = AUD_STREAM_PLAYBACK; stream <= AUD_STREAM_CAPTURE; stream++) {
            if (codec_int_cfg.stream_cfg[stream].opened) {
                if (stream == AUD_STREAM_PLAYBACK) {
                    ASSERT(
                        codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate >= AUD_SAMPRATE_44100,
                        "Invalid existing ANC sample rate %d for stream %d",
                        codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate,
                        stream);
                    ASSERT(codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num == play_chan_num,
                        "Invalid existing ANC channel num %d != %d for stream %d",
                        codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num,
                        play_chan_num,
                        stream);
                }
            } else {
                codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = 0;
                codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate = codec_anc_samp_rate;
                codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_SAMPLE_RATE;
                if (stream == AUD_STREAM_PLAYBACK) {
                    codec_int_cfg.stream_cfg[stream].codec_cfg.vol = TGT_VOLUME_LEVEL_10;
                } else {
                    codec_int_cfg.stream_cfg[stream].codec_cfg.vol = CODEC_SADC_VOL;
                }
                codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_VOL;
                if (stream == AUD_STREAM_PLAYBACK) {
                    codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num = play_chan_num;
                    codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map = CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV;
                    codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_CHANNEL_NUM;
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
    }

    hal_codec_anc_adc_enable(type);

    analog_aud_codec_anc_enable(type, true);

    if (!anc_running) {
        // Enable pa if dac muted before
        if (codec_int_cfg.mute_state[AUD_STREAM_PLAYBACK]) {
            analog_aud_codec_nomute();
        }
    }

    return 0;
}

int codec_anc_close(enum ANC_TYPE_T type)
{
    CODEC_TRACE("%s: type=%d", __func__, type);

    if (type == ANC_FEEDFORWARD) {
        anc_ff_enabled = false;
    } else if (type == ANC_FEEDBACK) {
        anc_fb_enabled = false;
    }

    hal_codec_anc_adc_disable(type);

    analog_aud_codec_anc_enable(type, false);

    if (anc_ff_enabled || anc_fb_enabled) {
        return 0;
    }

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

