#include "voice_enhancement.h"
#include "coherent_denoise.h"
#include "lc_mmse_ns.h"
#include "agc.h"
#include "med_memory.h"
#include "hal_trace.h"

#undef SPEECH_TX_2MIC_NS2
#undef SPEECH_TX_NS2
#undef SPEECH_TX_AGC

#if defined(SPEECH_TX_2MIC_NS2)
extern COHERENT_DENOISE_CFG_T coherent_denoise_cfg;
#endif

#if defined(SPEECH_TX_NS2)
static LC_MMSE_NS_State *lc_mmse_ns_st = NULL; 
#endif

#if defined(SPEECH_TX_AGC)
static AgcState *agc_capture = NULL;
#endif

#define MED_MEM_POOL_SIZE (50 * 1024)

static uint8_t *g_medMemPool = NULL;

extern int app_audio_mempool_get_buff(uint8_t **buff, uint32_t size);

#if defined(MED_MULTI_INSTANCE)
struct med_heap_instance ai_voice_med_heap_inst;
#endif

void sv_enhancement_init(int sample_rate, int frame_size, int channels)
{
    ASSERT(sample_rate == 16000, "[%s] Only support 16k voice, current samplerate is %d", __FUNCTION__, sample_rate);
    ASSERT(frame_size == SV_ENHANCEMENT_FRAME_SIZE, "[%s] Only support 10ms frame, current frame size is %d", __FUNCTION__, frame_size);

    TRACE("[%s] sample rate - %d, frame size - %d, channels - %d",
        __FUNCTION__, sample_rate, frame_size, channels);

#if MED_MEM_POOL_SIZE > 0
    app_audio_mempool_get_buff(&g_medMemPool, MED_MEM_POOL_SIZE);
#if defined(MED_MULTI_INSTANCE)
	med_system_heap_init(&ai_voice_med_heap_inst, &g_medMemPool[0], &g_medMemPool[MED_MEM_POOL_SIZE - 1]);
#else
	med_system_heap_init(&g_medMemPool[0], &g_medMemPool[MED_MEM_POOL_SIZE - 1]);
#endif
#endif

#if defined(SPEECH_TX_2MIC_NS2)
    coherent_denoise_init(frame_size / channels / 2, &coherent_denoise_cfg);
#endif

#if defined(SPEECH_TX_NS2)
    lc_mmse_ns_init(&lc_mmse_ns_st, frame_size / channels, sample_rate, -10);
#endif

#if defined(SPEECH_TX_AGC)
    agc_capture = agc_state_init(sample_rate, frame_size / channels);
    agc_set_config(agc_capture, 2, 3, 1);
#endif
}

void sv_enhancement_destroy(void)
{
#if defined(SPEECH_TX_AGC)
    agc_state_destory(agc_capture);
#endif

#if defined(SPEECH_TX_NS2)
    lc_mmse_ns_deinit(lc_mmse_ns_st);
#endif

#if defined(SPEECH_TX_2MIC_NS2)
    coheren_denoise_deinit();
#endif

#if MED_MEM_POOL_SIZE > 0
    uint32_t total = 0, used = 0, max_used = 0;
#if defined(MED_MULTI_INSTANCE)
	med_memory_info(&ai_voice_med_heap_inst, &total, &used, &max_used);
#else
	med_memory_info(&total, &used, &max_used);
#endif
    TRACE("[%s] MED MALLOC MEM: total - %d, used - %d, max_used - %d.",
        __FUNCTION__, total, used, max_used);
    ASSERT(used == 0, "[%s] used != 0", __FUNCTION__);
#endif
}

static void capture_step(int16_t *buf, int16_t *out, int frame_size)
{
    ASSERT(frame_size == SV_ENHANCEMENT_FRAME_SIZE, "[%s] Only support 10ms frame, current frame size is %d",
        __FUNCTION__, frame_size);

#ifdef SPEECH_TX_2MIC_NS2
    coherent_denoise_run((int16_t *)buf, frame_size, (int16_t *)out);
    // Channel num: two-->one
    frame_size >>= 1;
#elif defined(SPEECH_CAPTURE_TWO_CHANNEL)
    for (uint32_t i = 0; i < frame_size / 2; i++)
    {
        out[i] = buf[2 * i];      // Left channel
        // buf[i] = buf[2 * i + 1]; // Right channel
    }
    // Channel num: two-->one
    frame_size >>= 1;
#else // single channel
    memcpy(out, buf, sizeof(int16_t) * frame_size);
#endif

#ifdef SPEECH_TX_NS2
    lc_mmse_ns_run(lc_mmse_ns_st, (int16_t *)out, frame_size);
#endif

#if defined(SPEECH_TX_AGC)
    agc_process(agc_capture, (int16_t *)out);
#endif
}

uint32_t sv_enhancement_process_capture(uint8_t *buf, uint32_t len)
{
    int16_t *buf16 = (int16_t *)buf;
    int frame_size = len / 2;
    ASSERT(frame_size % SV_ENHANCEMENT_FRAME_SIZE == 0, "[%s] Only support 10ms * N frame, current frame size is %d",
        __FUNCTION__, frame_size);

// pass through
#if 0
#ifdef SPEECH_CAPTURE_TWO_CHANNEL
    // Test MIC: Get one channel data
    // Enable this, should bypass SPEECH_TX_2MIC_NS and SPEECH_TX_2MIC_NS2
    for (uint32_t i = 0; i < frame_size / 2; i++)
    {
        buf16[i] = buf16[2 * i + 1];      // Left channel
        // buf16[i] = buf16[2 * i + 1]; // Right channel
    }

    frame_size >>= 1;

    return len / 2;
#else
    return len;
#endif
#endif
    
    for (int offset = 0; offset < frame_size; offset += SV_ENHANCEMENT_FRAME_SIZE) {
    #if defined(SPEECH_CAPTURE_TWO_CHANNEL)
        capture_step(buf16 + offset, buf16 + offset / 2, SV_ENHANCEMENT_FRAME_SIZE);
    #else
        capture_step(buf16 + offset, buf16 + offset, SV_ENHANCEMENT_FRAME_SIZE);
    #endif
    }

    return len;
}
