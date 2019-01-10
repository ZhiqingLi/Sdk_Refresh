//#include "mbed.h"
#include <stdio.h>
#include <assert.h>

#include "cmsis_os.h"
#include "tgt_hardware.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_overlay.h"
#include "analog.h"
#include "app_bt_stream.h"
#include "app_overlay.h"
#include "app_audio.h"
#include "app_utils.h"
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"
#include "hal_codec.h"

#ifdef MEDIA_PLAYER_SUPPORT
#include "resources.h"
#include "app_media_player.h"
#endif
#ifdef __FACTORY_MODE_SUPPORT__
#include "app_factory_audio.h"
#endif

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
#include "app_ai_voice.h"
#endif
#include "app_ring_merge.h"

#include "bt_drv.h"
#include "bt_xtal_sync.h"
#include "besbt.h"
extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "btalloc.h"
}

#include "cqueue.h"
#include "btapp.h"

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
#include "audio_eq.h"
#endif

#include "app_bt_media_manager.h"

#include "string.h"
#include "hal_location.h"

#include "bt_drv_interface.h"

#include "audio_resample_ex.h"

#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
#include"anc_process.h"
#endif

//#define __PCM_FFT__

#ifdef __PCM_FFT__
int pcm_fft_calc_init(void);
int pcm_fft_calc_deinit(void);
int pcm_fft_calc_run(short *pcm_buf);
#endif


enum PLAYER_OPER_T
{
    PLAYER_OPER_START,
    PLAYER_OPER_STOP,
    PLAYER_OPER_RESTART,
};

#if (AUDIO_OUTPUT_VOLUME_DEFAULT < 1) || (AUDIO_OUTPUT_VOLUME_DEFAULT > 17)
#error "AUDIO_OUTPUT_VOLUME_DEFAULT out of range"
#endif
int8_t stream_local_volume = (AUDIO_OUTPUT_VOLUME_DEFAULT);
#ifdef AUDIO_LINEIN
int8_t stream_linein_volume = (AUDIO_OUTPUT_VOLUME_DEFAULT);
#endif

struct btdevice_volume *btdevice_volume_p;
//#ifdef __BT_ANC__
#if 1
uint8_t bt_sco_samplerate_ratio = 0;
static uint8_t *bt_anc_sco_dec_buf;
extern void us_fir_init(void);
extern uint32_t voicebtpcm_pcm_resample (short* src_samp_buf, uint32_t src_smpl_cnt, short* dst_samp_buf);

#if defined(AUDIO_ANC_FB_MC) && !defined(__AUDIO_RESAMPLE__)
static enum AUD_BITS_T sample_size_play_bt;
static enum AUD_SAMPRATE_T sample_rate_play_bt;
static uint32_t data_size_play_bt;

static uint8_t *playback_buf_bt;
static uint32_t playback_size_bt;
static int32_t playback_samplerate_ratio_bt;

static uint8_t *playback_buf_mc;
static uint32_t playback_size_mc;
static enum AUD_CHANNEL_NUM_T  playback_ch_num_bt;
#endif

#endif

#if defined(__THIRDPARTY)||defined(__BT_ANC__)||defined(__DMA_VOICE__)
extern "C" uint8_t is_sbc_mode (void);
uint8_t bt_sbc_mode;
extern "C" uint8_t __attribute__((section(".fast_text_sram")))  is_sbc_mode(void)
{
    return bt_sbc_mode;
}

extern "C" uint8_t is_sco_mode (void);

uint8_t bt_sco_mode;
extern "C"   uint8_t __attribute__((section(".fast_text_sram")))  is_sco_mode(void)
{
    return bt_sco_mode;
}

#endif
APP_BT_STREAM_T gStreamplayer = APP_BT_STREAM_NUM;

uint32_t a2dp_audio_more_data(uint8_t codec_type, uint8_t *buf, uint32_t len);
int a2dp_audio_init(void);
int a2dp_audio_deinit(void);
enum AUD_SAMPRATE_T a2dp_sample_rate = AUD_SAMPRATE_48000;

#ifdef MEDIA_PLAYER_RBCODEC
extern int app_rbplay_audio_onoff(bool onoff, uint16_t aud_id);
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
int app_a2dp_source_linein_on(bool on);
#endif
#if defined(APP_I2S_A2DP_SOURCE)
#include "app_status_ind.h"
#include "app_a2dp_source.h"
//player channel should <= capture channel number
//player must be 2 channel
#define LINEIN_PLAYER_CHANNEL (2)
#ifdef __AUDIO_INPUT_MONO_MODE__
#define LINEIN_CAPTURE_CHANNEL (1)
#else
#define LINEIN_CAPTURE_CHANNEL (2)
#endif

#if (LINEIN_CAPTURE_CHANNEL == 1)
#define LINEIN_PLAYER_BUFFER_SIZE (1024*LINEIN_PLAYER_CHANNEL)
#define LINEIN_CAPTURE_BUFFER_SIZE (LINEIN_PLAYER_BUFFER_SIZE/2)
#elif (LINEIN_CAPTURE_CHANNEL == 2)
#define LINEIN_PLAYER_BUFFER_SIZE (1024*LINEIN_PLAYER_CHANNEL)
//#define LINEIN_CAPTURE_BUFFER_SIZE (LINEIN_PLAYER_BUFFER_SIZE)
#define LINEIN_CAPTURE_BUFFER_SIZE (1024*10)

#endif

static int16_t *app_linein_play_cache = NULL;

int8_t app_linein_buffer_is_empty(void)
{
    if (app_audio_pcmbuff_length())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint32_t app_linein_pcm_come(uint8_t * pcm_buf, uint32_t len)
{
    //DUMP16("%d ", pcm_buf, 10);
    DUMP8("0x%02x ", pcm_buf, 10);
    TRACE("app_linein_pcm_come");
    app_audio_pcmbuff_put(pcm_buf, len);

    return len;
}

uint32_t app_linein_need_pcm_data(uint8_t* pcm_buf, uint32_t len)
{

#if (LINEIN_CAPTURE_CHANNEL == 1)
    app_audio_pcmbuff_get((uint8_t *)app_linein_play_cache, len/2);
    //app_play_audio_lineinmode_more_data((uint8_t *)app_linein_play_cache,len/2);
    app_bt_stream_copy_track_one_to_two_16bits((int16_t *)pcm_buf, app_linein_play_cache, len/2/2);
#elif (LINEIN_CAPTURE_CHANNEL == 2)
    app_audio_pcmbuff_get((uint8_t *)pcm_buf, len);
    //app_play_audio_lineinmode_more_data((uint8_t *)pcm_buf, len);
#endif

#if defined(__AUDIO_OUTPUT_MONO_MODE__) && !defined(AUDIO_EQ_DRC_MONO)
    merge_stereo_to_mono_16bits((int16_t *)buf, (int16_t *)pcm_buf, len/2);
#endif

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__AUDIO_DRC__) || defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__)
    audio_eq_run(pcm_buf, len);
#endif
    return len;
}
extern "C" void pmu_linein_onoff(unsigned char en);
extern "C" int hal_analogif_reg_read(unsigned short reg, unsigned short *val);
int app_a2dp_source_I2S_onoff(bool onoff)
{
    static bool isRun =  false;
    uint8_t *linein_audio_cap_buff = 0;
    uint8_t *linein_audio_play_buff = 0;
    uint8_t *linein_audio_loop_buf = NULL;
    struct AF_STREAM_CONFIG_T stream_cfg;

    TRACE("app_a2dp_source_I2S_onoff work:%d op:%d", isRun, onoff);

    if (isRun == onoff)
        return 0;

    if (onoff)
    {
#if defined( __AUDIO_DRC__) && (defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) )
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
#else
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
//      app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_208M);
#endif
        app_overlay_select(APP_OVERLAY_A2DP);
        app_audio_mempool_init();
        app_audio_mempool_get_buff(&linein_audio_cap_buff, LINEIN_CAPTURE_BUFFER_SIZE);
//        app_audio_mempool_get_buff(&linein_audio_play_buff, LINEIN_PLAYER_BUFFER_SIZE);
//        app_audio_mempool_get_buff(&linein_audio_loop_buf, LINEIN_PLAYER_BUFFER_SIZE<<2);
//        app_audio_pcmbuff_init(linein_audio_loop_buf, LINEIN_PLAYER_BUFFER_SIZE<<2);

#if (LINEIN_CAPTURE_CHANNEL == 1)
        app_audio_mempool_get_buff((uint8_t **)&app_linein_play_cache, LINEIN_PLAYER_BUFFER_SIZE/2/2);
        //app_play_audio_lineinmode_init(LINEIN_CAPTURE_CHANNEL, LINEIN_PLAYER_BUFFER_SIZE/2/2);
#elif (LINEIN_CAPTURE_CHANNEL == 2)
        //app_play_audio_lineinmode_init(LINEIN_CAPTURE_CHANNEL, LINEIN_PLAYER_BUFFER_SIZE/2);
#endif

        memset(&stream_cfg, 0, sizeof(stream_cfg));

        stream_cfg.bits = AUD_BITS_16;
        stream_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)LINEIN_PLAYER_CHANNEL;
        stream_cfg.sample_rate = AUD_SAMPRATE_44100;

#if 0
#if FPGA==0
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#else
        stream_cfg.device = AUD_STREAM_USE_EXT_CODEC;
#endif

        stream_cfg.vol = 10;//stream_linein_volume;
        //TRACE("vol = %d",stream_linein_volume);
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.handler = app_linein_need_pcm_data;
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(linein_audio_play_buff);
        stream_cfg.data_size = LINEIN_PLAYER_BUFFER_SIZE;
        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#endif

#if 1
        stream_cfg.device = AUD_STREAM_USE_I2S_SLAVE;
//     stream_cfg.io_path = AUD_INPUT_PATH_LINEIN;
//      stream_cfg.handler = app_linein_pcm_come;
        stream_cfg.handler = a2dp_source_linein_more_pcm_data;
//      stream_cfg.handler = app_linein_pcm_come;
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(linein_audio_cap_buff);
        stream_cfg.data_size = LINEIN_CAPTURE_BUFFER_SIZE;//2k

//        pmu_linein_onoff(1);
        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__AUDIO_DRC__) || defined(__HW_CODEC_IIR_EQ_PROCESS__)
        audio_eq_init();
        // LINEIN_PLAYER_BUFFER_SIZE/2/4
        audio_eq_open(stream_cfg.sample_rate, stream_cfg.bits, stream_cfg.channel_num, NULL, 0);
#endif

        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
#endif
        //app_status_indication_set(APP_STATUS_INDICATION_LINEIN_ON);
    }
    else
    {
//       clear buffer data
        a2dp_source.pcm_queue.write=0;
        a2dp_source.pcm_queue.len=0;
        a2dp_source.pcm_queue.read=0;
//       pmu_linein_onoff(0);
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);

        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        //app_status_indication_set(APP_STATUS_INDICATION_LINEIN_OFF);
        app_overlay_unloadall();
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
    }

    isRun = onoff;
    TRACE("%s end!\n", __func__);
    return 0;
}
#endif


enum AUD_SAMPRATE_T bt_parse_sbc_sample_rate(uint8_t sbc_samp_rate)
{
    enum AUD_SAMPRATE_T sample_rate;
    sbc_samp_rate = sbc_samp_rate & A2D_SBC_IE_SAMP_FREQ_MSK;

    switch (sbc_samp_rate)
    {
        case A2D_SBC_IE_SAMP_FREQ_16:
//            sample_rate = AUD_SAMPRATE_16000;
//            break;
        case A2D_SBC_IE_SAMP_FREQ_32:
//            sample_rate = AUD_SAMPRATE_32000;
//            break;
        case A2D_SBC_IE_SAMP_FREQ_48:
            sample_rate = AUD_SAMPRATE_48000;
            break;
        case A2D_SBC_IE_SAMP_FREQ_44:
            sample_rate = AUD_SAMPRATE_44100;
            break;
        default:
            ASSERT(0, "[%s] 0x%x is invalid", __func__, sbc_samp_rate);
            break;
    }
    return sample_rate;
}

void bt_store_sbc_sample_rate(enum AUD_SAMPRATE_T sample_rate)
{
    a2dp_sample_rate = sample_rate;
}

enum AUD_SAMPRATE_T bt_get_sbc_sample_rate(void)
{
    return a2dp_sample_rate;
}

enum AUD_SAMPRATE_T bt_parse_store_sbc_sample_rate(uint8_t sbc_samp_rate)
{
    enum AUD_SAMPRATE_T sample_rate;

    sample_rate = bt_parse_sbc_sample_rate(sbc_samp_rate);
    bt_store_sbc_sample_rate(sample_rate);

    return sample_rate;
}

int bt_sbc_player_setup(uint8_t freq)
{
    struct AF_STREAM_CONFIG_T *stream_cfg = NULL;
    static uint8_t sbc_samp_rate = 0xff;
    uint32_t ret;

    if (sbc_samp_rate == freq)
        return 0;

    switch (freq)
    {
        case A2D_SBC_IE_SAMP_FREQ_16:
        case A2D_SBC_IE_SAMP_FREQ_32:
        case A2D_SBC_IE_SAMP_FREQ_48:
            a2dp_sample_rate = AUD_SAMPRATE_48000;
            break;
#ifdef  A2DP_SCALABLE_ON
        case A2D_SBC_IE_SAMP_FREQ_96:
            a2dp_sample_rate = AUD_SAMPRATE_96000;
            break;
#endif
        case A2D_SBC_IE_SAMP_FREQ_44:
            a2dp_sample_rate = AUD_SAMPRATE_44100;
            break;
        default:
            break;
    }

    ret = af_stream_get_cfg(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg, true);
    if (ret == 0) {
        stream_cfg->sample_rate = a2dp_sample_rate;
#if defined(SW_PLAYBACK_RESAMPLE_441TO48K)
		stream_cfg->sample_rate = AUD_SAMPRATE_48000;
#else
		stream_cfg->sample_rate = a2dp_sample_rate;
#endif
        af_stream_setup(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, stream_cfg);
    }

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
    ret = af_stream_get_cfg(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK, &stream_cfg, true);
    if (ret == 0) {
        stream_cfg->sample_rate = a2dp_sample_rate;
        sample_rate_play_bt=stream_cfg->sample_rate;        
        af_stream_setup(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK, stream_cfg);
        anc_mc_run_setup(sample_rate_play_bt);
    }
#endif
    
    sbc_samp_rate = freq;

    return 0;
}

void merge_stereo_to_mono_16bits(int16_t *src_buf, int16_t *dst_buf,  uint32_t src_len)
{
    uint32_t i = 0;
    for (i = 0; i < src_len; i+=2)
    {
        dst_buf[i] = (src_buf[i]>>1) + (src_buf[i+1]>>1);
        dst_buf[i+1] = dst_buf[i];
    }
}

#ifdef __HEAR_THRU_PEAK_DET__
#include "peak_detector.h"
// Depend on codec_dac_vol
const float pkd_vol_multiple[18] = {0.281838, 0.000010, 0.005623, 0.007943, 0.011220, 0.015849, 0.022387, 0.031623, 0.044668, 0.063096, 0.089125, 0.125893, 0.177828, 0.251189, 0.354813, 0.501187, 0.707946, 1.000000};
int app_bt_stream_local_volume_get(void);
#endif

#ifdef __BT_ANC__
#include "app_anc.h"
#endif

#ifdef __ANKER_UI__
extern "C" void af_codec_playback_post_handler_ext_proc(void);
#endif

static uint8_t app_fft_flag = 0;
static void app_fft_connect_handler(void const *param)
{
	
 //   TRACE("app_fft_connect_handler\n");
	app_fft_flag = 0;

}
osTimerDef (APP_FFT, app_fft_connect_handler);
static osTimerId app_fft_timer = NULL;


static uint32_t sbc_inter_time;

FRAM_TEXT_LOC uint32_t bt_sbc_player_more_data(uint8_t *buf, uint32_t len)
{
	uint32_t s_time,e_time;
	//TRACE("SBC INTERUPT TIME=%d",hal_sys_timer_get()-sbc_inter_time);
	sbc_inter_time=hal_sys_timer_get();

#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined(__DMA_VOICE__)
	if (app_voicepath_get_stream_pending_state(VOICEPATH_STREAMING))
	{		   
		af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
		app_voicepath_set_stream_state(VOICEPATH_STREAMING, true);
		app_voicepath_set_pending_started_stream(VOICEPATH_STREAMING, false);
	}
#endif
    uint8_t codec_type = AVDTP_CODEC_TYPE_SBC;
    uint32_t overlay_id = 0;
#ifdef BT_XTAL_SYNC
    bt_xtal_sync(BT_XTAL_SYNC_MODE_MUSIC);
#endif
    codec_type = bt_sbc_player_get_codec_type();
    if(codec_type ==  AVDTP_CODEC_TYPE_MPEG2_4_AAC)
    {
        overlay_id = APP_OVERLAY_A2DP_AAC;
    }
#if defined(A2DP_SCALABLE_ON)
    else if(codec_type ==  AVDTP_CODEC_TYPE_NON_A2DP)
    {
        overlay_id = APP_OVERLAY_A2DP_SCALABLE;
    }
#endif
    else
        overlay_id = APP_OVERLAY_A2DP;

#if FPGA==0
    if(app_get_current_overlay() != overlay_id)
    {
        memset(buf, 0, len);
        return len;
    }
#endif
	s_time=hal_sys_timer_get();

#ifdef A2DP_EQ_24BIT
    // Change len to 16-bit sample buffer length
    len = len / (sizeof(int32_t) / sizeof(int16_t));
#endif

    //--------------------------------------------------------------
    // Start of normal 16-bit processing
    //--------------------------------------------------------------

    a2dp_audio_more_data(codec_type, buf, len);
    app_ring_merge_more_data(buf, len);
#ifdef __AUDIO_OUTPUT_MONO_MODE__
    merge_stereo_to_mono_16bits((int16_t *)buf, (int16_t *)buf, len/2);
#endif

    //--------------------------------------------------------------
    // End of normal 16-bit processing
    //--------------------------------------------------------------

#ifdef A2DP_EQ_24BIT
    int samp_cnt = len / sizeof(int16_t);
    int16_t *samp16 = (int16_t *)buf;
    int32_t *samp32 = (int32_t *)buf;

    // Convert 16-bit sample to 24-bit sample
    for (int i = samp_cnt - 1; i >= 0; i--) {
        samp32[i] = samp16[i] << 8;
    }

    // Restore len to 24-bit sample buffer length
    len = len * (sizeof(int32_t) / sizeof(int16_t));
#endif

#ifdef __HEAR_THRU_PEAK_DET__
#ifdef __BT_ANC__
    if(app_anc_work_status())
#endif
    {
        int vol_level = 0;
        vol_level = app_bt_stream_local_volume_get();
        peak_detector_run(buf, len, pkd_vol_multiple[vol_level]);
    }
#endif
#ifdef __ANKER_UI__
	af_codec_playback_post_handler_ext_proc();
#endif
#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
    audio_eq_run(buf, len);
#endif
	e_time=hal_sys_timer_get();
	//TRACE("SBC COST TIME %d",e_time-s_time);

#ifdef __PCM_FFT__
	if (app_fft_timer == NULL)
			app_fft_timer = osTimerCreate(osTimer(APP_FFT), osTimerOnce, NULL);

		if(app_fft_flag == 0)
		{
			app_fft_flag = 1;
			osTimerStart(app_fft_timer, 50);
			pcm_fft_calc_run((short*)buf);
		}
#endif

    OS_NotifyEvm();

    return len;
}
#ifdef __THIRDPARTY
bool start_by_sbc = false;
#endif

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
static uint32_t audio_mc_data_playback_a2dp(uint8_t *buf, uint32_t mc_len_bytes)
{
   uint32_t begin_time;
   //uint32_t end_time;
   begin_time = hal_sys_timer_get();
   TRACE("music cancel: %d",begin_time);

   float left_gain;
   float right_gain;
   int32_t playback_len_bytes;
   int32_t i,j,k;   

   hal_codec_get_dac_gain(&left_gain,&right_gain);

   //TRACE("playback_samplerate_ratio:  %d",playback_samplerate_ratio);
   
  // TRACE("left_gain:  %d",(int)(left_gain*(1<<12)));
  // TRACE("right_gain: %d",(int)(right_gain*(1<<12)));
  
   playback_len_bytes=mc_len_bytes/playback_samplerate_ratio_bt;

    if (sample_size_play_bt == AUD_BITS_16)
    {
        int16_t *sour_p=(int16_t *)(playback_buf_bt+playback_size_bt/2);
        int16_t *mid_p=(int16_t *)(buf+mc_len_bytes-playback_len_bytes);
        int16_t *dest_p=(int16_t *)buf;
        int i,j,k;

        if(buf == playback_buf_mc)
        {
            sour_p=(int16_t *)playback_buf_bt;
        }

        for(i=0,j=0;i<playback_len_bytes/2;i=i+2)
        {
            mid_p[j++]=sour_p[i];
            mid_p[j++]=sour_p[i+1];
         
        }
       
        anc_mc_run_stereo((uint8_t *)mid_p,playback_len_bytes,left_gain,right_gain,AUD_BITS_16);        

        for(i=0,j=0;i<playback_len_bytes/2;i=i+2)
        {
            for(k=0;k<playback_samplerate_ratio_bt;k++)
            {
                dest_p[j++]=mid_p[i];
                dest_p[j++]=mid_p[i+1];
            }
        }


        
    }
    else if (sample_size_play_bt == AUD_BITS_24)
    {
        int32_t *sour_p=(int32_t *)(playback_buf_bt+playback_size_bt/2);
        int32_t *mid_p=(int32_t *)(buf+mc_len_bytes-playback_len_bytes);        
        int32_t *dest_p=(int32_t *)buf;
        int32_t val_shift;

#if (CODEC_PLAYBACK_BIT_DEPTH < 24)
        val_shift = (24 - CODEC_PLAYBACK_BIT_DEPTH);
#else
        val_shift = 0;
#endif
        if(buf == (playback_buf_mc))
        {
            sour_p=(int32_t *)playback_buf_bt;
        }
        
        for(i=0,j=0;i<playback_len_bytes/4;i=i+2)
        {
            mid_p[j++]=sour_p[i]>> val_shift;
            mid_p[j++]=sour_p[i+1]>> val_shift;
         
        }

        anc_mc_run_stereo((uint8_t *)mid_p,playback_len_bytes,left_gain,right_gain,AUD_BITS_24);

        for(i=0,j=0;i<playback_len_bytes/4;i=i+2)
        {
            for(k=0;k<playback_samplerate_ratio_bt;k++)
            {
                dest_p[j++]=mid_p[i];
                dest_p[j++]=mid_p[i+1];
            }
        }

    }

  //  end_time = hal_sys_timer_get();

 //   TRACE("%s:run time: %d", __FUNCTION__, end_time-begin_time);

    return 0;
}
#endif



int bt_sbc_player(enum PLAYER_OPER_T on, enum APP_SYSFREQ_FREQ_T freq)
{
    struct AF_STREAM_CONFIG_T stream_cfg;
    enum AUD_SAMPRATE_T sample_rate;
    static bool isRun =  false;
    uint8_t* bt_audio_buff = NULL;
#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
    uint8_t* bt_eq_buff = NULL;
    uint32_t eq_buff_size;
    uint8_t POSSIBLY_UNUSED play_samp_size;
#endif

    TRACE("bt_sbc_player work:%d op:%d freq:%d :sample:%d \n", isRun, on, freq,a2dp_sample_rate);

    if ((isRun && on == PLAYER_OPER_START) || (!isRun && on == PLAYER_OPER_STOP))
        return 0;

    if (on == PLAYER_OPER_STOP || on == PLAYER_OPER_RESTART)
    {
#ifdef __THIRDPARTY	
    start_by_sbc = false;
#endif
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        af_stream_stop(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK);
#endif

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
        audio_eq_close();
#endif
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        af_stream_close(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK);
#endif

#ifdef __PCM_FFT__
		pcm_fft_calc_deinit();
#endif


#if defined(__THIRDPARTY)||defined(__BT_ANC__)
        bt_sbc_mode = 0;
#endif
#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined(__DMA_VOICE__)
		app_voicepath_set_stream_state(AUDIO_OUTPUT_STREAMING, false);
		app_voicepath_set_pending_started_stream(AUDIO_OUTPUT_STREAMING, false);
#endif

        if (on == PLAYER_OPER_STOP)
        {
#ifndef FPGA
#ifdef BT_XTAL_SYNC
            osDelay(50);
            bt_term_xtal_sync();
#endif
#endif
            a2dp_audio_deinit();
            app_overlay_unloadall();
#ifdef __THIRDPARTY
            app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_STOP);
            app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_START);
#endif
		   
			app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
			af_set_priority(osPriorityAboveNormal);

        }
    }

    if (on == PLAYER_OPER_START || on == PLAYER_OPER_RESTART)
    {
#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_STOP);
#endif
        af_set_priority(osPriorityHigh);
        bt_media_volume_ptr_update_by_mediatype(BT_STREAM_SBC);
        stream_local_volume = btdevice_volume_p->a2dp_vol;
        app_audio_mempool_init();

#ifdef __PCM_FFT__
		pcm_fft_calc_init();
#endif

#ifdef __BT_ONE_BRING_TWO__
        if (MEC(activeCons)>1)
        {
            if (freq < APP_SYSFREQ_104M) {
                freq = APP_SYSFREQ_104M;
            }
        }
#endif
#ifdef __PC_CMD_UART__
        if (freq < APP_SYSFREQ_104M) {
            freq = APP_SYSFREQ_104M;
        }
#endif
#if defined(__SW_IIR_EQ_PROCESS__)&&defined(__HW_FIR_EQ_PROCESS__)
        if (audio_eq_hw_fir_cfg.len > 128)
        {
            if (freq < APP_SYSFREQ_104M) {
                freq = APP_SYSFREQ_104M;
            }
        }
#endif
#if defined(A2DP_SCALABLE_ON)
        if (bt_sbc_player_get_codec_type() == AVDTP_CODEC_TYPE_NON_A2DP)
        {
            if(a2dp_sample_rate==44100)
            {
                if(freq < APP_SYSFREQ_78M) {
                    freq = APP_SYSFREQ_78M;
                }
            }
            else if(a2dp_sample_rate==96000)
            {
                if (freq < APP_SYSFREQ_208M) {
                    freq = APP_SYSFREQ_208M;
                }
            }
        }
        TRACE("a2dp_sample_rate=%d",a2dp_sample_rate);
#endif
#if defined(APP_MUSIC_26M) && !defined(__SW_IIR_EQ_PROCESS__)
        if (freq < APP_SYSFREQ_26M) {
            freq = APP_SYSFREQ_26M;
        }
#else
        if (freq < APP_SYSFREQ_52M) {
            freq = APP_SYSFREQ_52M;
        }
#endif

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        if (freq < APP_SYSFREQ_104M) {
            freq = APP_SYSFREQ_104M;
        }
#endif
		if (freq < APP_SYSFREQ_208M) {
			freq = APP_SYSFREQ_208M;
		}

        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, freq);
        TRACE("bt_sbc_player: app_sysfreq_req %d", freq);

        if (on == PLAYER_OPER_START)
        {
            if (0)
            {
            }
#if defined(A2DP_AAC_ON)
            else if (bt_sbc_player_get_codec_type() == AVDTP_CODEC_TYPE_MPEG2_4_AAC)
            {
                app_overlay_select(APP_OVERLAY_A2DP_AAC);
            }
#endif
#if defined(A2DP_SCALABLE_ON)
            else if (bt_sbc_player_get_codec_type() == AVDTP_CODEC_TYPE_NON_A2DP)
            {
                app_overlay_select(APP_OVERLAY_A2DP_SCALABLE);
            }
#endif
            else
            {
                app_overlay_select(APP_OVERLAY_A2DP);
            }

#ifdef BT_XTAL_SYNC
            bt_init_xtal_sync(BT_XTAL_SYNC_MODE_MUSIC);
#endif
//#ifdef  __FPGA_FLASH__
            //app_overlay_select(APP_OVERLAY_A2DP);
//#endif
#if defined(__THIRDPARTY)||defined(__BT_ANC__)
            bt_sbc_mode = 1;
#endif
        }

#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_STOP);
		start_by_sbc = true;		
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_START);
#endif
#if defined(__AUDIO_RESAMPLE__) && defined(SW_PLAYBACK_RESAMPLE)
#if defined(SW_PLAYBACK_RESAMPLE_441TO48K)
        hal_cmu_audio_resample_disable();
        sample_rate = AUD_SAMPRATE_48000;
#else
        sample_rate = AUD_SAMPRATE_50781;
#endif
#else
        sample_rate = a2dp_sample_rate;
#endif

        memset(&stream_cfg, 0, sizeof(stream_cfg));

        stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
        stream_cfg.sample_rate = sample_rate;
#ifdef FPGA
        stream_cfg.device = AUD_STREAM_USE_EXT_CODEC;
#else
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.vol = stream_local_volume;
        stream_cfg.handler = bt_sbc_player_more_data;

#if defined(A2DP_SCALABLE_ON)
        if (bt_sbc_player_get_codec_type() == AVDTP_CODEC_TYPE_NON_A2DP)
        {
            ASSERT(BT_AUDIO_BUFF_SIZE >= (8*SCALABLE_FRAME_SIZE), "BT_AUDIO_BUFF_SIZE should >= (8*SCALABLE_FRAME_SIZE)");
            stream_cfg.data_size = SCALABLE_FRAME_SIZE*8;
        }
        else
#endif
        {
            stream_cfg.data_size = BT_AUDIO_BUFF_SIZE;
        }
#ifdef A2DP_EQ_24BIT
        stream_cfg.data_size *= 2;
        stream_cfg.bits = AUD_BITS_24;
#else
        stream_cfg.bits = AUD_BITS_16;
#endif

        app_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);


#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        sample_size_play_bt=stream_cfg.bits;
        sample_rate_play_bt=stream_cfg.sample_rate;
        data_size_play_bt=stream_cfg.data_size;
        playback_buf_bt=stream_cfg.data_ptr;
        playback_size_bt=stream_cfg.data_size;  
        playback_samplerate_ratio_bt=8;
        playback_ch_num_bt=stream_cfg.channel_num;
#endif

        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);

#ifdef __HEAR_THRU_PEAK_DET__
        PEAK_DETECTOR_CFG_T peak_detector_cfg;
        peak_detector_cfg.fs = stream_cfg.sample_rate;
        peak_detector_cfg.bits = stream_cfg.bits;
        peak_detector_cfg.factor_up = 0.6;
        peak_detector_cfg.factor_down = 2.0;
        peak_detector_cfg.reduce_dB = -30;
        peak_detector_init();
        peak_detector_setup(&peak_detector_cfg);
#endif

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
        audio_eq_init();
#ifdef __HW_FIR_EQ_PROCESS__
        play_samp_size = (stream_cfg.bits <= AUD_BITS_16) ? 2 : 4;
#if defined(CHIP_BEST2000)
        eq_buff_size = stream_cfg.data_size * sizeof(int32_t) / play_samp_size;
#else
        eq_buff_size = stream_cfg.data_size * sizeof(int16_t) / play_samp_size;
#endif
        app_audio_mempool_get_buff(&bt_eq_buff, eq_buff_size);
#else
        //eq_buff_size = 0;
        bt_eq_buff = NULL;
		if(stream_cfg.bits==AUD_BITS_16)
			eq_buff_size=BT_AUDIO_BUFF_SIZE;
		else if(stream_cfg.bits==AUD_BITS_24)
			eq_buff_size=BT_AUDIO_BUFF_SIZE*2;
#endif
        audio_eq_open(stream_cfg.sample_rate, stream_cfg.bits, stream_cfg.channel_num, bt_eq_buff, eq_buff_size);
#endif

#if defined(__AUDIO_RESAMPLE__) && !defined(SW_PLAYBACK_RESAMPLE)
        af_codec_tune_resample_rate(AUD_STREAM_PLAYBACK, 1.0f);
#endif

        if (on == PLAYER_OPER_START)
        {
            // This might use all of the rest buffer in the mempool,
            // so it must be the last configuration before starting stream.
            a2dp_audio_init();
        }

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        stream_cfg.bits = sample_size_play_bt;
        stream_cfg.channel_num = playback_ch_num_bt;
        stream_cfg.sample_rate = sample_rate_play_bt;
        stream_cfg.device = AUD_STREAM_USE_MC;
        stream_cfg.vol = 0;
        stream_cfg.handler = audio_mc_data_playback_a2dp;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;

        app_audio_mempool_get_buff(&bt_audio_buff, data_size_play_bt*8);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);      
        stream_cfg.data_size = data_size_play_bt*8;
        
        playback_buf_mc=stream_cfg.data_ptr;
        playback_size_mc=stream_cfg.data_size;  

        anc_mc_run_init(sample_rate_play_bt);

        af_stream_open(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK, &stream_cfg);
        //ASSERT(ret == 0, "af_stream_open playback failed: %d", ret);
#endif        

        
#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined(__DMA_VOICE__)
        if (app_voicepath_get_stream_state(VOICEPATH_STREAMING))
        {
            app_voicepath_set_pending_started_stream(AUDIO_OUTPUT_STREAMING, true);
        }
        else
        {
            app_voicepath_set_stream_state(AUDIO_OUTPUT_STREAMING, true);
            af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        }
#else
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#endif
        

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        af_stream_start(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK);
#endif


#ifdef __THIRDPARTY
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_OTHER_EVENT);
#endif

    }

    isRun = (on != PLAYER_OPER_STOP);
    return 0;
}


void speech_tx_aec_set_frame_len(int len);
int voicebtpcm_pcm_echo_buf_queue_init(uint32_t size);
void voicebtpcm_pcm_echo_buf_queue_reset(void);
void voicebtpcm_pcm_echo_buf_queue_deinit(void);
int voicebtpcm_pcm_audio_init(void);
int voicebtpcm_pcm_audio_deinit(void);
uint32_t voicebtpcm_pcm_audio_data_come(uint8_t *buf, uint32_t len);
uint32_t voicebtpcm_pcm_audio_more_data(uint8_t *buf, uint32_t len);
int store_voicebtpcm_m2p_buffer(unsigned char *buf, unsigned int len);
int get_voicebtpcm_p2m_frame(unsigned char *buf, unsigned int len);
uint8_t btapp_hfp_need_mute(void);
bool btapp_hfp_mic_need_skip_frame(void);
static uint32_t mic_force_mute = 0;
static uint32_t spk_force_mute = 0;
static uint32_t bt_sco_player_code_type = 0;

static enum AUD_CHANNEL_NUM_T sco_play_chan_num;
static enum AUD_CHANNEL_NUM_T sco_cap_chan_num;

bool bt_sco_codec_is_msbc(void)
{
    bool en = false;
#ifdef HFP_1_6_ENABLE
    if (app_audio_manager_get_scocodecid() == HF_SCO_CODEC_MSBC)
    {
        en = true;
    }
    else
#endif
    {
        en = false;
    }

    return en;
}

#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)

#ifdef CHIP_BEST1000
#error "Unsupport SW_SCO_RESAMPLE on best1000 by now"
#endif
#ifdef NO_SCO_RESAMPLE
#error "Conflicted config: NO_SCO_RESAMPLE and SW_SCO_RESAMPLE"
#endif

// The decoded playback data in the first irq is output to DAC after the second irq (PING-PONG buffer)
#define SCO_PLAY_RESAMPLE_ALIGN_CNT     2

static uint8_t sco_play_irq_cnt;
static bool sco_dma_buf_err;
static struct APP_RESAMPLE_T *sco_capture_resample;
static struct APP_RESAMPLE_T *sco_playback_resample;

static int bt_sco_capture_resample_iter(uint8_t *buf, uint32_t len)
{
    voicebtpcm_pcm_audio_data_come(buf, len);
    return 0;
}

static int bt_sco_playback_resample_iter(uint8_t *buf, uint32_t len)
{
    voicebtpcm_pcm_audio_more_data(buf, len);
    return 0;
}

#endif

//( codec:mic-->btpcm:tx
// codec:mic

#ifdef KNOWLES_I2S_INTERFACE
#define SCO_USE_I2S

static void merge_stereo_to__16bits(int16_t *src_buf, int16_t *dst_buf,  uint32_t src_len)
{
    uint32_t i = 0;
    for (i = 0; i < src_len; i+=2)
    {
		dst_buf[i/2] = (src_buf[i]);
    }
}

#endif

static uint32_t bt_sco_codec_capture_data(uint8_t *buf, uint32_t len)
{
#ifdef SCO_USE_I2S
	merge_stereo_to__16bits((int16_t *)buf, (int16_t *)buf, len/2);
	len=len/2;
#endif
    if (btapp_hfp_mic_need_skip_frame()||mic_force_mute)
    {
        memset(buf, 0, len);
    }

#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)
    if(hal_cmu_get_audio_resample_status())
    {
        if (af_stream_buffer_error(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE)) {
            sco_dma_buf_err = true;
        }
        // The decoded playback data in the first irq is output to DAC after the second irq (PING-PONG buffer),
        // so it is aligned with the capture data after 2 playback irqs.
        if (sco_play_irq_cnt < SCO_PLAY_RESAMPLE_ALIGN_CNT) {
            // Skip processing
            return len;
        }
        app_capture_resample_run(sco_capture_resample, buf, len);
    }
    else
#endif
    {
        voicebtpcm_pcm_audio_data_come(buf, len);
    }

    return len;
}

// btpcm:tx
static uint32_t bt_sco_btpcm_playback_data(uint8_t *buf, uint32_t len)
{
    get_voicebtpcm_p2m_frame(buf, len);

    if (btapp_hfp_need_mute())
    {
        memset(buf, 0, len);
    }
    return len;
}
//)

//( btpcm:rx-->codec:spk
// btpcm:rx
static uint32_t bt_sco_btpcm_capture_data(uint8_t *buf, uint32_t len)
{
#if defined(HFP_1_6_ENABLE)
    if (app_audio_manager_get_scocodecid() == HF_SCO_CODEC_MSBC)
    {
        uint16_t * temp_buf = NULL;
        temp_buf=(uint16_t *)buf;
        len /= 2;
        for(uint32_t i =0; i<len; i++)
        {
            buf[i]=temp_buf[i]>>8;
        }
    }
#endif
    store_voicebtpcm_m2p_buffer(buf, len);

    return len;
}

#ifdef __BT_ANC__
static void bt_anc_sco_down_sample_16bits(int16_t *dst, int16_t *src, uint32_t dst_cnt)
{
    for (uint32_t i = 0; i < dst_cnt; i++) {
        dst[i] = src[i * bt_sco_samplerate_ratio * sco_play_chan_num];
    }
}
#endif


static uint32_t bt_sco_codec_playback_data(uint8_t *buf, uint32_t len)
{
    uint8_t *dec_buf;
    uint32_t mono_len;

#ifdef BT_XTAL_SYNC
    bt_xtal_sync(BT_XTAL_SYNC_MODE_VOICE);
#endif

#ifdef __BT_ANC__
    mono_len = len / sco_play_chan_num / bt_sco_samplerate_ratio;
    dec_buf = bt_anc_sco_dec_buf;
#else
    mono_len = len / sco_play_chan_num;
    dec_buf = buf;
#endif

#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)
    if(hal_cmu_get_audio_resample_status())
    {
        if (sco_play_irq_cnt < SCO_PLAY_RESAMPLE_ALIGN_CNT) {
            sco_play_irq_cnt++;
        }
        if (sco_dma_buf_err || af_stream_buffer_error(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK)) {
            sco_dma_buf_err = false;
            sco_play_irq_cnt = 0;
            app_resample_reset(sco_playback_resample);
            app_resample_reset(sco_capture_resample);
            voicebtpcm_pcm_echo_buf_queue_reset();
            TRACE("%s: DMA buffer error: reset resample", __func__);
        }
        app_playback_resample_run(sco_playback_resample, dec_buf, mono_len);
    }
    else
#endif
    {
#ifdef __BT_ANC__
        bt_anc_sco_down_sample_16bits((int16_t *)dec_buf, (int16_t *)buf, mono_len / 2);
#else
        if (sco_play_chan_num == AUD_CHANNEL_NUM_2) {
            // Convert stereo data to mono data (to save into echo_buf)
            app_bt_stream_copy_track_two_to_one_16bits((int16_t *)dec_buf, (int16_t *)buf, mono_len / 2);
        }
#endif
        voicebtpcm_pcm_audio_more_data(dec_buf, mono_len);


    }

#ifdef __BT_ANC__
    voicebtpcm_pcm_resample((int16_t *)dec_buf, mono_len / 2, (int16_t *)buf);
#endif

    if (sco_play_chan_num == AUD_CHANNEL_NUM_2) {
        // Convert mono data to stereo data
        app_bt_stream_copy_track_one_to_two_16bits((int16_t *)buf, (int16_t *)buf, len / 2 / 2);
    }

    if (spk_force_mute)
    {
        memset(buf, 0, len);
    }

    return len;
}

int bt_sco_player_forcemute(bool mic_mute, bool spk_mute)
{
    mic_force_mute = mic_mute;
    spk_force_mute = spk_mute;
    return 0;
}

int bt_sco_player_get_codetype(void)
{
    if (gStreamplayer == APP_BT_STREAM_HFP_PCM)
    {
        return bt_sco_player_code_type;
    }
    else
    {
        return 0;
    }
}

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
static uint32_t audio_mc_data_playback_sco(uint8_t *buf, uint32_t mc_len_bytes)
{
   uint32_t begin_time;
   //uint32_t end_time;
   begin_time = hal_sys_timer_get();
   TRACE("phone cancel: %d",begin_time);

   float left_gain;
   float right_gain;
   int32_t playback_len_bytes;
   int32_t i,j,k;   

   hal_codec_get_dac_gain(&left_gain,&right_gain);

   TRACE("playback_samplerate_ratio:  %d",playback_samplerate_ratio_bt);
   TRACE("mc_len_bytes:  %d",mc_len_bytes);
   
  // TRACE("left_gain:  %d",(int)(left_gain*(1<<12)));
  // TRACE("right_gain: %d",(int)(right_gain*(1<<12)));
  
   playback_len_bytes=mc_len_bytes/playback_samplerate_ratio_bt;

    if (sample_size_play_bt == AUD_BITS_16)
    {
        int16_t *sour_p=(int16_t *)(playback_buf_bt+playback_size_bt/2);
        int16_t *mid_p=(int16_t *)(buf+mc_len_bytes-playback_len_bytes);
        int16_t *dest_p=(int16_t *)buf;
        int i,j,k;

        if(buf == playback_buf_mc)
        {
            sour_p=(int16_t *)playback_buf_bt;
        }

        if(playback_ch_num_bt==AUD_CHANNEL_NUM_2)
        {
            for(i=0,j=0;j<playback_len_bytes/2;i=i+2)
            {
                mid_p[j++]=sour_p[i];
            }
        }
        else if(playback_ch_num_bt==AUD_CHANNEL_NUM_1)
        {
            for(i=0,j=0;j<playback_len_bytes/2;i=i+1)
            {
                mid_p[j++]=sour_p[i];
            }       
        }
        
        anc_mc_run_mono((uint8_t *)mid_p,playback_len_bytes,left_gain,AUD_BITS_16);        

        for(i=0,j=0;i<playback_len_bytes/2;i=i+1)
        {
            for(k=0;k<playback_samplerate_ratio_bt;k++)
            {
                dest_p[j++]=mid_p[i];
            }
        }        
    }
    else if (sample_size_play_bt == AUD_BITS_24)
    {
        int32_t *sour_p=(int32_t *)(playback_buf_bt+playback_size_bt/2);
        int32_t *mid_p=(int32_t *)(buf+mc_len_bytes-playback_len_bytes);        
        int32_t *dest_p=(int32_t *)buf;
        int32_t val_shift;

#if (CODEC_PLAYBACK_BIT_DEPTH < 24)
        val_shift = (24 - CODEC_PLAYBACK_BIT_DEPTH);
#else
        val_shift = 0;
#endif
        if(buf == playback_buf_mc)
        {
            sour_p=(int32_t *)playback_buf_bt;
        }
        
        if(playback_ch_num_bt==AUD_CHANNEL_NUM_2)
        {        
            for(i=0,j=0;j<playback_len_bytes/4;i=i+2)
            {
                mid_p[j++]=sour_p[i]>> val_shift;            
            }
        }
        else if(playback_ch_num_bt==AUD_CHANNEL_NUM_1)
        {
            for(i=0,j=0;j<playback_len_bytes/4;i=i+1)
            {
                mid_p[j++]=sour_p[i]>> val_shift;
            }
        }
        
        anc_mc_run_mono((uint8_t *)mid_p,playback_len_bytes,left_gain,AUD_BITS_24);

        for(i=0,j=0;i<playback_len_bytes/4;i=i+1)
        {
            for(k=0;k<playback_samplerate_ratio_bt;k++)
            {
                dest_p[j++]=mid_p[i];
            }
        }

    }

  //  end_time = hal_sys_timer_get();

 //   TRACE("%s:run time: %d", __FUNCTION__, end_time-begin_time);

    return 0;
}
#endif

// #define SPEECH_CODEC_FRAME_MS       (16)
// #define SPEECH_SCO_FRAME_MS         (15)

// #define TIME_TO_FRAME_SIZE(fs, ms)      ((fs) / 1000 * (ms))
// #define TIME_TO_BUF_SIZE(fs, ms)        (TIME_TO_FRAME_SIZE(fs, ms)  * 2 * 2)

// #define APP_BT_CODEC_FRAME_SIZE         TIME_TO_FRAME_SIZE(fs, ms)

int speech_get_frame_size(int fs, int ch, int ms)
{
    return (fs / 1000 * ch * ms);
}

int speech_get_af_buffer_size(int fs, int ch, int ms)
{
    return speech_get_frame_size(fs, ch, ms) * 2 * 2;
}

enum AUD_SAMPRATE_T bt_sco_get_sample_rate(void)
{
    enum AUD_SAMPRATE_T sample_rate;

#if defined(HFP_1_6_ENABLE) && defined(MSBC_16K_SAMPLE_RATE)
    if (bt_sco_codec_is_msbc())
    {
        sample_rate = AUD_SAMPRATE_16000;
    }
    else
#endif
    {
        sample_rate = AUD_SAMPRATE_8000;
    }

    return sample_rate;
}


int bt_sco_player(bool on, enum APP_SYSFREQ_FREQ_T freq)
{
    struct AF_STREAM_CONFIG_T stream_cfg;
    static bool isRun =  false;
    uint8_t * bt_audio_buff = NULL;
    enum AUD_SAMPRATE_T sample_rate;

    TRACE("bt_sco_player work:%d op:%d freq:%d", isRun, on, freq);
//  osDelay(1);

    if (isRun==on)
        return 0;

    if (on)
    {
#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_STOP); 
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_MIC_OPEN); 
#endif
        //bt_syncerr set to max(0x0a)
//        BTDIGITAL_REG_SET_FIELD(REG_BTCORE_BASE_ADDR, 0x0f, 0, 0x0f);
//        af_set_priority(osPriorityRealtime);
        af_set_priority(osPriorityHigh);
        bt_media_volume_ptr_update_by_mediatype(BT_STREAM_VOICE);
        stream_local_volume = btdevice_volume_p->hfp_vol;
        app_audio_manager_sco_status_checker();

#if defined(SPEECH_TX_NS) && defined(SPEECH_TX_AEC)
#if  !defined(_SCO_BTPCM_CHANNEL_)
        if (freq < APP_SYSFREQ_104M)
        {
            freq = APP_SYSFREQ_104M;
        }
#else
        if (freq < APP_SYSFREQ_78M)
        {
            freq = APP_SYSFREQ_78M;
        }
#endif
#endif
#ifdef __BT_ONE_BRING_TWO__
        if (MEC(activeCons)>1)
        {
            freq = APP_SYSFREQ_104M;
        }
#endif
#ifdef __BT_ANC__
        if (freq < APP_SYSFREQ_104M)
        {
            freq = APP_SYSFREQ_104M;
        }
#endif
#if defined(HFP_1_6_ENABLE)
        bt_sco_player_code_type = app_audio_manager_get_scocodecid();
        if (bt_sco_player_code_type == HF_SCO_CODEC_MSBC)
        {
            freq = APP_SYSFREQ_104M;
        }
#endif
#if defined(SPEECH_CAPTURE_TWO_CHANNEL)
        if (freq < APP_SYSFREQ_104M)
        {
            freq = APP_SYSFREQ_104M;
        }
#endif
#if defined(SPEECH_TX_2MIC_NS3)
        if (freq < APP_SYSFREQ_208M)
        {
            freq = APP_SYSFREQ_208M;
        }
#endif
        if (freq < APP_SYSFREQ_52M)
        {
            freq = APP_SYSFREQ_52M;
        }
#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        if (freq < APP_SYSFREQ_208M) {
            freq = APP_SYSFREQ_208M;
        }
#endif
        if (freq < APP_SYSFREQ_208M)
        {
            freq = APP_SYSFREQ_208M;
        }

        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, freq);
        TRACE("bt_sco_player: app_sysfreq_req %d", freq);
        //TRACE("sys freq calc : %d\n", hal_sys_timer_calc_cpu_freq(50, 0));

#ifndef FPGA
        app_overlay_select(APP_OVERLAY_HFP);
#ifdef BT_XTAL_SYNC
        bt_init_xtal_sync(BT_XTAL_SYNC_MODE_VOICE);
#endif
#endif

        int aec_frame_len = speech_get_frame_size(bt_sco_get_sample_rate(), 1, SPEECH_CODEC_FRAME_MS);
        speech_tx_aec_set_frame_len(aec_frame_len);

        bt_sco_player_forcemute(false, false);
#ifdef RECORD_LONELY		
		app_full_audio_mempool_init();
#else
        app_audio_mempool_init();
#endif
        voicebtpcm_pcm_audio_init();

#ifndef _SCO_BTPCM_CHANNEL_
        memset(&hf_sendbuff_ctrl, 0, sizeof(hf_sendbuff_ctrl));
#endif

#if defined(SPEECH_CAPTURE_TWO_CHANNEL)
        sample_rate = AUD_SAMPRATE_16000;
        sco_cap_chan_num = AUD_CHANNEL_NUM_2;
#else
        sample_rate = bt_sco_get_sample_rate();  
        sco_cap_chan_num = AUD_CHANNEL_NUM_1;
#endif

#ifdef SCO_USE_I2S
		sco_cap_chan_num = AUD_CHANNEL_NUM_2;
#endif

        memset(&stream_cfg, 0, sizeof(stream_cfg));

        // codec:mic
        stream_cfg.channel_num = sco_cap_chan_num;
        stream_cfg.data_size = speech_get_af_buffer_size(sample_rate, sco_cap_chan_num, SPEECH_CODEC_FRAME_MS);

#if defined(__AUDIO_RESAMPLE__) && defined(NO_SCO_RESAMPLE)
        // When __AUDIO_RESAMPLE__ is defined,
        // resample is off by default on best1000, and on by default on other platforms
#ifndef CHIP_BEST1000
        hal_cmu_audio_resample_disable();
#endif
#endif

#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)
        if (sample_rate == AUD_SAMPRATE_8000)
        {
            stream_cfg.sample_rate = AUD_SAMPRATE_8463;
        }
        else if (sample_rate == AUD_SAMPRATE_16000)
        {
            stream_cfg.sample_rate = AUD_SAMPRATE_16927;
        }
        sco_capture_resample = app_capture_resample_open(sample_rate, stream_cfg.channel_num,
                            bt_sco_capture_resample_iter, stream_cfg.data_size / 2);

        uint32_t mono_cap_samp_cnt = stream_cfg.data_size / 2 / 2 / stream_cfg.channel_num;
        uint32_t cap_irq_cnt_per_frm = ((mono_cap_samp_cnt * stream_cfg.sample_rate + (sample_rate - 1)) / sample_rate +
            (aec_frame_len - 1)) / aec_frame_len;
        if (cap_irq_cnt_per_frm == 0) {
            cap_irq_cnt_per_frm = 1;
        }
#else
        stream_cfg.sample_rate = sample_rate;
#endif

        stream_cfg.bits = AUD_BITS_16;
        stream_cfg.vol = stream_local_volume;
		
#ifdef	SCO_USE_I2S
		stream_cfg.device = AUD_STREAM_USE_I2S_MASTER;
		stream_cfg.io_path = AUD_IO_PATH_NULL;
		stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
#else
#ifdef FPGA
        stream_cfg.device = AUD_STREAM_USE_EXT_CODEC;
#else
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif
        stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
#endif
        stream_cfg.handler = bt_sco_codec_capture_data;
        app_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);

        TRACE("capture sample_rate:%d, data_size:%d",stream_cfg.sample_rate,stream_cfg.data_size);

        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);

#ifdef CHIP_BEST2300
        btdrv_set_bt_pcm_triggler_delay(59);
#else
        btdrv_set_bt_pcm_triggler_delay(55);
#endif

        // codec:spk
        sample_rate = bt_sco_get_sample_rate();  
        sco_play_chan_num = AUD_CHANNEL_NUM_2;
        stream_cfg.channel_num = sco_play_chan_num;
        // stream_cfg.data_size = BT_AUDIO_SCO_BUFF_SIZE * stream_cfg.channel_num;
        stream_cfg.data_size = speech_get_af_buffer_size(sample_rate, sco_play_chan_num, SPEECH_CODEC_FRAME_MS);
#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)
        if (sample_rate == AUD_SAMPRATE_8000)
        {
            stream_cfg.sample_rate = AUD_SAMPRATE_8463;
        }
        else if (sample_rate == AUD_SAMPRATE_16000)
        {
            stream_cfg.sample_rate = AUD_SAMPRATE_16927;
        }
        sco_playback_resample = app_playback_resample_open(sample_rate, AUD_CHANNEL_NUM_1,
                            bt_sco_playback_resample_iter, stream_cfg.data_size / stream_cfg.channel_num / 2);
        sco_play_irq_cnt = 0;
        sco_dma_buf_err = false;

        uint32_t mono_play_samp_cnt = stream_cfg.data_size / 2 / 2 / stream_cfg.channel_num;
        uint32_t play_irq_cnt_per_frm = ((mono_play_samp_cnt * stream_cfg.sample_rate + (sample_rate - 1)) / sample_rate +
            (aec_frame_len - 1)) / aec_frame_len;
        if (play_irq_cnt_per_frm == 0) {
            play_irq_cnt_per_frm = 1;
        }
        uint32_t play_samp_cnt_per_frm = mono_play_samp_cnt * play_irq_cnt_per_frm;
        uint32_t cap_samp_cnt_per_frm = mono_cap_samp_cnt * cap_irq_cnt_per_frm;
        uint32_t max_samp_cnt_per_frm = (play_samp_cnt_per_frm >= cap_samp_cnt_per_frm) ? play_samp_cnt_per_frm : cap_samp_cnt_per_frm;
        uint32_t echo_q_samp_cnt = (((max_samp_cnt_per_frm + mono_play_samp_cnt * SCO_PLAY_RESAMPLE_ALIGN_CNT) *
            // convert to 8K/16K sample cnt
             sample_rate +(stream_cfg.sample_rate - 1)) / stream_cfg.sample_rate +
            // aligned with aec_frame_len
            (aec_frame_len - 1)) / aec_frame_len * aec_frame_len;
        if (echo_q_samp_cnt == 0) {
            echo_q_samp_cnt = aec_frame_len;
        }
        voicebtpcm_pcm_echo_buf_queue_init(echo_q_samp_cnt * 2);
#else
        stream_cfg.sample_rate = sample_rate;
#endif

#ifdef __BT_ANC__
        // Mono channel decoder buffer (8K or 16K sample rate)
        app_audio_mempool_get_buff(&bt_anc_sco_dec_buf, stream_cfg.data_size / 2 / sco_play_chan_num);
        // The playback size for the actual sample rate
        bt_sco_samplerate_ratio = 6/(sample_rate/AUD_SAMPRATE_8000);
        stream_cfg.data_size *= bt_sco_samplerate_ratio;
#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)
        stream_cfg.sample_rate = AUD_SAMPRATE_50781;
#else
        stream_cfg.sample_rate = AUD_SAMPRATE_48000;
#endif
        //damic_init();
        //init_amic_dc_bt();
        //ds_fir_init();
        us_fir_init();
#endif
#if defined(__THIRDPARTY)||defined(__BT_ANC__)||defined(__DMA_VOICE__)
		bt_sco_mode = 1;
#endif
		stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.handler = bt_sco_codec_playback_data;

        app_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);

        TRACE("playback sample_rate:%d, data_size:%d",stream_cfg.sample_rate,stream_cfg.data_size);

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        sample_size_play_bt=stream_cfg.bits;
        sample_rate_play_bt=stream_cfg.sample_rate;
        data_size_play_bt=stream_cfg.data_size;
        playback_buf_bt=stream_cfg.data_ptr;
        playback_size_bt=stream_cfg.data_size;  
        playback_samplerate_ratio_bt=8;
        playback_ch_num_bt=stream_cfg.channel_num;
#endif
        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        stream_cfg.bits = sample_size_play_bt;
        stream_cfg.channel_num = AUD_CHANNEL_NUM_1;
        stream_cfg.sample_rate = sample_rate_play_bt;
        stream_cfg.device = AUD_STREAM_USE_MC;
        stream_cfg.vol = 0;
        stream_cfg.handler = audio_mc_data_playback_sco;
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;

        app_audio_mempool_get_buff(&bt_audio_buff, data_size_play_bt*8/playback_ch_num_bt);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);      
        stream_cfg.data_size = data_size_play_bt*8/playback_ch_num_bt;
        
        playback_buf_mc=stream_cfg.data_ptr;
        playback_size_mc=stream_cfg.data_size;  

        anc_mc_run_init(sample_rate_play_bt);

        af_stream_open(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK, &stream_cfg);
#endif

#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        af_stream_start(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK);
#endif
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);

#ifdef _SCO_BTPCM_CHANNEL_
        stream_cfg.sample_rate = bt_sco_get_sample_rate();
        stream_cfg.channel_num = AUD_CHANNEL_NUM_1;
        // stream_cfg.data_size = BT_AUDIO_SCO_BUFF_SIZE * stream_cfg.channel_num;
        stream_cfg.data_size = speech_get_af_buffer_size(sample_rate, stream_cfg.channel_num, SPEECH_SCO_FRAME_MS);
        // btpcm:rx
        stream_cfg.device = AUD_STREAM_USE_BT_PCM;
        stream_cfg.handler = bt_sco_btpcm_capture_data;
        app_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);

        TRACE("sco btpcm sample_rate:%d, data_size:%d",stream_cfg.sample_rate,stream_cfg.data_size);

        af_stream_open(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE, &stream_cfg);

        // btpcm:tx
        stream_cfg.device = AUD_STREAM_USE_BT_PCM;
        stream_cfg.handler = bt_sco_btpcm_playback_data;
        app_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);
        af_stream_open(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK, &stream_cfg);

        af_stream_start(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK);
        af_stream_start(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE);

#ifdef CHIP_BEST2300
        btdrv_pcm_enable();
#endif
#endif

#ifdef FPGA
        app_bt_stream_volumeset(stream_local_volume);
        //btdrv_set_bt_pcm_en(1);
#endif
        TRACE("bt_sco_player on");
    }
    else
    {    
#ifdef __THIRDPARTY
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_MIC_CLOSE);
#endif
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);       
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        af_stream_stop(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK);
#endif

#ifdef _SCO_BTPCM_CHANNEL_
        af_stream_stop(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE);
        af_stream_stop(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK);

        af_stream_close(AUD_STREAM_ID_1, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_1, AUD_STREAM_PLAYBACK);
#endif
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        af_stream_close(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK);
#endif
        

#if defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE)
        app_capture_resample_close(sco_capture_resample);
        sco_capture_resample = NULL;
        app_capture_resample_close(sco_playback_resample);
        sco_playback_resample = NULL;
#endif

#if defined(__AUDIO_RESAMPLE__) && defined(NO_SCO_RESAMPLE)
#ifndef CHIP_BEST1000
        // When __AUDIO_RESAMPLE__ is defined,
        // resample is off by default on best1000, and on by default on other platforms
        hal_cmu_audio_resample_enable();
#endif
#endif

#if defined(__THIRDPARTY)||defined(__BT_ANC__)||defined(__DMA_VOICE__)
		bt_sco_mode = 0;
#endif

#ifdef __BT_ANC__
        bt_anc_sco_dec_buf = NULL;
        //damic_deinit();
        //app_cap_thread_stop();
#endif
        voicebtpcm_pcm_audio_deinit();

#ifndef FPGA
#ifdef BT_XTAL_SYNC
        osDelay(50);
        bt_term_xtal_sync();
#endif
#endif
#if defined(HFP_1_6_ENABLE)
        bt_sco_player_code_type = HF_SCO_CODEC_NONE;
#endif
        TRACE("bt_sco_player off");
        app_overlay_unloadall();
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
        af_set_priority(osPriorityAboveNormal);

        //bt_syncerr set to default(0x07)
//       BTDIGITAL_REG_SET_FIELD(REG_BTCORE_BASE_ADDR, 0x0f, 0, 0x07);
#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_START);
#endif
    }

    isRun=on;
    return 0;
}

#ifdef AUDIO_LINEIN
#include "app_status_ind.h"
//player channel should <= capture channel number
//player must be 2 channel
#define LINEIN_PLAYER_CHANNEL (2)
#ifdef __AUDIO_OUTPUT_MONO_MODE__
#define LINEIN_CAPTURE_CHANNEL (1)
#else
#define LINEIN_CAPTURE_CHANNEL (2)
#endif

#if (LINEIN_CAPTURE_CHANNEL == 1)
#ifdef A2DP_EQ_24BIT
#define LINEIN_PLAYER_BUFFER_SIZE (512*3*LINEIN_PLAYER_CHANNEL*2)
#else
#define LINEIN_PLAYER_BUFFER_SIZE (512*3*LINEIN_PLAYER_CHANNEL)
#endif
#define LINEIN_CAPTURE_BUFFER_SIZE (LINEIN_PLAYER_BUFFER_SIZE/2)
#elif (LINEIN_CAPTURE_CHANNEL == 2)
#ifdef A2DP_EQ_24BIT
#define LINEIN_PLAYER_BUFFER_SIZE (512*3*LINEIN_PLAYER_CHANNEL*2)
#else
#define LINEIN_PLAYER_BUFFER_SIZE (512*3*LINEIN_PLAYER_CHANNEL)
#endif
#define LINEIN_CAPTURE_BUFFER_SIZE (LINEIN_PLAYER_BUFFER_SIZE)
#endif

static int16_t *app_linein_play_cache = NULL;

int8_t app_linein_buffer_is_empty(void)
{
    if (app_audio_pcmbuff_length()){
        return 0;
    }else{
        return 1;
    }
}

uint32_t app_linein_pcm_come(uint8_t * pcm_buf, uint32_t len)
{
    app_audio_pcmbuff_put(pcm_buf, len);

    return len;
}



uint32_t app_linein_need_pcm_data(uint8_t* pcm_buf, uint32_t len)
{
#ifdef A2DP_EQ_24BIT
	len=len/2;
#endif

#if (LINEIN_CAPTURE_CHANNEL == 1)
    app_audio_pcmbuff_get((uint8_t *)app_linein_play_cache, len/2);
    app_play_audio_lineinmode_more_data((uint8_t *)app_linein_play_cache,len/2);
    app_bt_stream_copy_track_one_to_two_16bits((int16_t *)pcm_buf, app_linein_play_cache, len/2/2);
#elif (LINEIN_CAPTURE_CHANNEL == 2)
    app_audio_pcmbuff_get((uint8_t *)pcm_buf, len);
    app_play_audio_lineinmode_more_data((uint8_t *)pcm_buf, len);  
#endif

#if defined(__AUDIO_OUTPUT_MONO_MODE__) && !defined(AUDIO_EQ_DRC_MONO)
    merge_stereo_to_mono_16bits((int16_t *)buf, (int16_t *)pcm_buf, len/2);
#endif

#ifdef A2DP_EQ_24BIT
	int samp_cnt = len / sizeof(int16_t);
	int16_t *samp16 = (int16_t *)pcm_buf;
	int32_t *samp32 = (int32_t *)pcm_buf;

	// Convert 16-bit sample to 24-bit sample
	for (int i = samp_cnt - 1; i >= 0; i--) {
		samp32[i] = samp16[i] << 8;
	}
	len = len * (sizeof(int32_t) / sizeof(int16_t));
#endif
#ifdef __ANKER_UI__
	af_codec_playback_post_handler_ext_proc();
#endif

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)|| defined(__HW_IIR_EQ_PROCESS__)
    audio_eq_run(pcm_buf, len);
#endif
    return len;
}

int app_play_linein_onoff(bool onoff)
{
    static bool isRun =  false;
    uint8_t *linein_audio_cap_buff = 0;
    uint8_t *linein_audio_play_buff = 0;
    uint8_t *linein_audio_loop_buf = NULL;
    struct AF_STREAM_CONFIG_T stream_cfg;
#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_DAC_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
	uint8_t* bt_eq_buff = NULL;
	uint32_t eq_buff_size;
    uint8_t POSSIBLY_UNUSED play_samp_size;
#endif

    TRACE("app_play_linein_onoff work:%d op:%d", isRun, onoff);

    if (isRun == onoff)
        return 0;

     if (onoff){
 #if defined( __AUDIO_DRC__) && (defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) )   
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
 #else
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
 #endif
        app_overlay_select(APP_OVERLAY_A2DP);         
        app_audio_mempool_init();
        app_audio_mempool_get_buff(&linein_audio_cap_buff, LINEIN_CAPTURE_BUFFER_SIZE);
        app_audio_mempool_get_buff(&linein_audio_play_buff, LINEIN_PLAYER_BUFFER_SIZE);
        app_audio_mempool_get_buff(&linein_audio_loop_buf, LINEIN_PLAYER_BUFFER_SIZE<<2);
        app_audio_pcmbuff_init(linein_audio_loop_buf, LINEIN_PLAYER_BUFFER_SIZE<<2);

#if (LINEIN_CAPTURE_CHANNEL == 1)
        app_audio_mempool_get_buff((uint8_t **)&app_linein_play_cache, LINEIN_PLAYER_BUFFER_SIZE/2/2);
        app_play_audio_lineinmode_init(LINEIN_CAPTURE_CHANNEL, LINEIN_PLAYER_BUFFER_SIZE/2/2);
#elif (LINEIN_CAPTURE_CHANNEL == 2)
        app_play_audio_lineinmode_init(LINEIN_CAPTURE_CHANNEL, LINEIN_PLAYER_BUFFER_SIZE/2);
#endif

        memset(&stream_cfg, 0, sizeof(stream_cfg));
#ifdef A2DP_EQ_24BIT
        stream_cfg.bits = AUD_BITS_24;
#else
		stream_cfg.bits = AUD_BITS_16;
#endif
        stream_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)LINEIN_PLAYER_CHANNEL;
#if defined(__AUDIO_RESAMPLE__)
		stream_cfg.sample_rate = AUD_SAMPRATE_50781;
#else
        stream_cfg.sample_rate = AUD_SAMPRATE_48000;
#endif
#if FPGA==0
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#else
        stream_cfg.device = AUD_STREAM_USE_EXT_CODEC;
#endif
        stream_cfg.vol = stream_linein_volume;
        TRACE("vol = %d",stream_linein_volume);
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.handler = app_linein_need_pcm_data;
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(linein_audio_play_buff);
        stream_cfg.data_size = LINEIN_PLAYER_BUFFER_SIZE;
        
#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_IIR_EQ_PROCESS__)|| defined(__HW_DAC_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
		audio_eq_init();
#ifdef __HW_FIR_EQ_PROCESS__
		play_samp_size = (stream_cfg.bits <= AUD_BITS_16) ? 2 : 4;
#if defined(CHIP_BEST2000)
		eq_buff_size = stream_cfg.data_size * sizeof(int32_t) / play_samp_size;
#else
		eq_buff_size = stream_cfg.data_size * sizeof(int16_t) / play_samp_size;
#endif
		app_audio_mempool_get_buff(&bt_eq_buff, eq_buff_size);
#else
		//eq_buff_size = 0;
		bt_eq_buff = NULL;
		eq_buff_size =LINEIN_PLAYER_BUFFER_SIZE;
#endif
		audio_eq_open(stream_cfg.sample_rate, stream_cfg.bits, stream_cfg.channel_num, bt_eq_buff, eq_buff_size);
#endif
		
		af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);

		stream_cfg.bits = AUD_BITS_16;
        stream_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)LINEIN_CAPTURE_CHANNEL;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)CFG_HW_AUD_INPUT_PATH_LINEIN_DEV;
        stream_cfg.io_path = AUD_INPUT_PATH_LINEIN;
        stream_cfg.handler = app_linein_pcm_come;
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(linein_audio_cap_buff);
        stream_cfg.data_size = LINEIN_CAPTURE_BUFFER_SIZE;

        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
     }else     {
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__) || defined(__HW_IIR_EQ_PROCESS__)|| defined(__HW_DAC_IIR_EQ_PROCESS__) || defined(__AUDIO_DRC__)
		audio_eq_close();
#endif

        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
	
		app_overlay_unloadall();
        app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
     }

    isRun = onoff;
    TRACE("%s end!\n", __func__);
    return 0;
}
#endif

int app_bt_stream_open(APP_AUDIO_STATUS* status)
{
    int nRet = -1;
    APP_BT_STREAM_T player = (APP_BT_STREAM_T)status->id;
    enum APP_SYSFREQ_FREQ_T freq = (enum APP_SYSFREQ_FREQ_T)status->freq;

    TRACE("app_bt_stream_open prev:%d cur:%d freq:%d", gStreamplayer, player, freq);

    if (gStreamplayer != APP_BT_STREAM_NUM)
    {
        TRACE("Close prev bt stream before opening");
        nRet = app_bt_stream_close(gStreamplayer);
        if (nRet)
            return -1;
    }

    switch (player)
    {
        case APP_BT_STREAM_HFP_PCM:
        case APP_BT_STREAM_HFP_CVSD:
        case APP_BT_STREAM_HFP_VENDOR:
            nRet = bt_sco_player(true, freq);
            break;
        case APP_BT_STREAM_A2DP_SBC:
        case APP_BT_STREAM_A2DP_AAC:
        case APP_BT_STREAM_A2DP_VENDOR:
            nRet = bt_sbc_player(PLAYER_OPER_START, freq);
            break;
#ifdef __FACTORY_MODE_SUPPORT__
        case APP_FACTORYMODE_AUDIO_LOOP:
            nRet = app_factorymode_audioloop(true, freq);
            break;
#endif
#ifdef MEDIA_PLAYER_SUPPORT
        case APP_PLAY_BACK_AUDIO:
            nRet = app_play_audio_onoff(true, status);
            break;
#endif

#ifdef MEDIA_PLAYER_RBCODEC
        case APP_BT_STREAM_RBCODEC:
            nRet = app_rbplay_audio_onoff(true, 0);
            break;
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
		case APP_BT_STREAM_AI_VOICE:
			nRet = app_ai_voice_start_mic_stream();
            break;
#endif

#ifdef AUDIO_LINEIN
        case APP_PLAY_LINEIN_AUDIO:
            nRet = app_play_linein_onoff(true);
            break;
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)
        case APP_A2DP_SOURCE_LINEIN_AUDIO:
            nRet = app_a2dp_source_linein_on(true);
            break;
#endif
#if defined(APP_I2S_A2DP_SOURCE)
        case APP_A2DP_SOURCE_I2S_AUDIO:
            nRet = app_a2dp_source_I2S_onoff(true);
            break;
#endif
        default:
            nRet = -1;
            break;
    }

    if (!nRet)
        gStreamplayer = player;

    return nRet;
}

int app_bt_stream_close(APP_BT_STREAM_T player)
{
    int nRet = -1;
    TRACE("app_bt_stream_close prev:%d cur:%d", gStreamplayer, player);
//  osDelay(1);

    if (gStreamplayer != player)
        return -1;

    switch (player)
    {
        case APP_BT_STREAM_HFP_PCM:
        case APP_BT_STREAM_HFP_CVSD:
        case APP_BT_STREAM_HFP_VENDOR:
            nRet = bt_sco_player(false, APP_SYSFREQ_32K);
            break;
        case APP_BT_STREAM_A2DP_SBC:
        case APP_BT_STREAM_A2DP_AAC:
        case APP_BT_STREAM_A2DP_VENDOR:
            nRet = bt_sbc_player(PLAYER_OPER_STOP, APP_SYSFREQ_32K);
            break;
#ifdef __FACTORY_MODE_SUPPORT__
        case APP_FACTORYMODE_AUDIO_LOOP:
            nRet = app_factorymode_audioloop(false, APP_SYSFREQ_32K);
            break;
#endif
#ifdef MEDIA_PLAYER_SUPPORT
        case APP_PLAY_BACK_AUDIO:
            nRet = app_play_audio_onoff(false, NULL);
            break;
#endif
#ifdef MEDIA_PLAYER_RBCODEC
        case APP_BT_STREAM_RBCODEC:
            nRet = app_rbplay_audio_onoff(false, 0);
            break;
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
		case APP_BT_STREAM_AI_VOICE:
			nRet = app_ai_voice_stop_mic_stream();
            break;
#endif

#ifdef AUDIO_LINEIN
        case APP_PLAY_LINEIN_AUDIO:
            nRet = app_play_linein_onoff(false);
            break;
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)
        case APP_A2DP_SOURCE_LINEIN_AUDIO:
            nRet = app_a2dp_source_linein_on(false);
            break;
#endif
#if defined(APP_I2S_A2DP_SOURCE)
        case APP_A2DP_SOURCE_I2S_AUDIO:
            nRet = app_a2dp_source_I2S_onoff(false);
            break;
#endif
        default:
            nRet = -1;
            break;
    }
    if (!nRet)
        gStreamplayer = APP_BT_STREAM_NUM;
    return nRet;
}

int app_bt_stream_setup(APP_BT_STREAM_T player, uint8_t status)
{
    int nRet = -1;

    TRACE("app_bt_stream_setup prev:%d cur:%d sample:%d", gStreamplayer, player, status);

    switch (player)
    {
        case APP_BT_STREAM_HFP_PCM:
        case APP_BT_STREAM_HFP_CVSD:
        case APP_BT_STREAM_HFP_VENDOR:
            break;
        case APP_BT_STREAM_A2DP_SBC:
        case APP_BT_STREAM_A2DP_AAC:
        case APP_BT_STREAM_A2DP_VENDOR:
            bt_sbc_player_setup(status);
            break;
        default:
            nRet = -1;
            break;
    }

    return nRet;
}

int app_bt_stream_restart(APP_AUDIO_STATUS* status)
{
    int nRet = -1;
    APP_BT_STREAM_T player = (APP_BT_STREAM_T)status->id;
    enum APP_SYSFREQ_FREQ_T freq = (enum APP_SYSFREQ_FREQ_T)status->freq;

    TRACE("app_bt_stream_restart prev:%d cur:%d freq:%d", gStreamplayer, player, freq);

    if (gStreamplayer != player)
        return -1;

    switch (player)
    {
        case APP_BT_STREAM_HFP_PCM:
        case APP_BT_STREAM_HFP_CVSD:
        case APP_BT_STREAM_HFP_VENDOR:
            nRet = bt_sco_player(false, freq);
            nRet = bt_sco_player(true, freq);
            break;
        case APP_BT_STREAM_A2DP_SBC:
        case APP_BT_STREAM_A2DP_AAC:
        case APP_BT_STREAM_A2DP_VENDOR:
//            nRet = bt_sbc_player(PLAYER_OPER_RESTART, freq);
#ifdef __BT_ONE_BRING_TWO__
            if (MEC(activeCons)>1)
            {
                app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
                bt_media_volume_ptr_update_by_mediatype(BT_STREAM_SBC);
                app_bt_stream_volumeset(btdevice_volume_p->a2dp_vol);
            }
#endif
            break;
#ifdef __FACTORY_MODE_SUPPORT__
        case APP_FACTORYMODE_AUDIO_LOOP:
            break;
#endif
#ifdef MEDIA_PLAYER_SUPPORT
        case APP_PLAY_BACK_AUDIO:
            break;
#endif
        default:
            nRet = -1;
            break;
    }

    return nRet;
}

void app_bt_stream_volumeup(void)
{
    int8_t vol;
#if defined AUDIO_LINEIN
    if(app_bt_stream_isrun(APP_PLAY_LINEIN_AUDIO))
    {
        stream_linein_volume ++;
        if (stream_linein_volume > TGT_VOLUME_LEVEL_15)
            stream_linein_volume = TGT_VOLUME_LEVEL_15;		
        app_bt_stream_volumeset(stream_linein_volume); 
        TRACE("set linein volume %d\n", stream_linein_volume);
    }
#endif    
    if (app_bt_stream_isrun(APP_BT_STREAM_A2DP_SBC))
    {
        TRACE("%s set audio volume", __func__);
        btdevice_volume_p->a2dp_vol++;
        vol = btdevice_volume_p->a2dp_vol;
        app_bt_stream_volumeset(vol);
    }

    if (app_bt_stream_isrun(APP_BT_STREAM_HFP_PCM))
    {
        TRACE("%s set hfp volume", __func__);
        btdevice_volume_p->hfp_vol++;
        vol = btdevice_volume_p->hfp_vol;
        app_bt_stream_volumeset(vol);
    }

    if (app_bt_stream_isrun(APP_BT_STREAM_NUM))
    {
        TRACE("%s set idle volume", __func__);
        btdevice_volume_p->a2dp_vol++;
    }

    if (btdevice_volume_p->a2dp_vol > TGT_VOLUME_LEVEL_15)
    {
        btdevice_volume_p->a2dp_vol = TGT_VOLUME_LEVEL_15;
#ifdef MEDIA_PLAYER_SUPPORT
        media_PlayAudio(AUD_ID_BT_WARNING,0);
#endif
    }

    if (btdevice_volume_p->hfp_vol > TGT_VOLUME_LEVEL_15)
    {
        btdevice_volume_p->hfp_vol = TGT_VOLUME_LEVEL_15;
#ifdef MEDIA_PLAYER_SUPPORT
        media_PlayAudio(AUD_ID_BT_WARNING,0);
#endif
    }
#ifndef FPGA
    nv_record_touch_cause_flush();
#endif

    TRACE("%s a2dp: %d", __func__, btdevice_volume_p->a2dp_vol);
    TRACE("%s hfp: %d", __func__, btdevice_volume_p->hfp_vol);
}

void app_bt_stream_volumedown(void)
{
    int8_t vol;
#if defined AUDIO_LINEIN
    if(app_bt_stream_isrun(APP_PLAY_LINEIN_AUDIO))
    {
        stream_linein_volume --;
        if (stream_linein_volume < TGT_VOLUME_LEVEL_MUTE)
            stream_linein_volume = TGT_VOLUME_LEVEL_MUTE;
        app_bt_stream_volumeset(stream_linein_volume); 
        TRACE("set linein volume %d\n", stream_linein_volume);
    }
#endif   
    if (app_bt_stream_isrun(APP_BT_STREAM_A2DP_SBC))
    {
        TRACE("%s set audio volume", __func__);
        btdevice_volume_p->a2dp_vol--;
        vol = btdevice_volume_p->a2dp_vol;
        if (vol < TGT_VOLUME_LEVEL_MUTE)
            vol = TGT_VOLUME_LEVEL_MUTE;
        app_bt_stream_volumeset(vol);
    }

    if (app_bt_stream_isrun(APP_BT_STREAM_HFP_PCM))
    {
        TRACE("%s set hfp volume", __func__);
        btdevice_volume_p->hfp_vol--;
        vol = btdevice_volume_p->hfp_vol;
        if (vol < TGT_VOLUME_LEVEL_0)
            vol = TGT_VOLUME_LEVEL_0;
        app_bt_stream_volumeset(vol);
    }

    if (app_bt_stream_isrun(APP_BT_STREAM_NUM))
    {
        TRACE("%s set idle volume", __func__);
        btdevice_volume_p->a2dp_vol--;
    }

    if (btdevice_volume_p->a2dp_vol < TGT_VOLUME_LEVEL_MUTE)
    {
        btdevice_volume_p->a2dp_vol = TGT_VOLUME_LEVEL_MUTE;
#ifdef MEDIA_PLAYER_SUPPORT
        media_PlayAudio(AUD_ID_BT_WARNING,0);
#endif
    }

    if (btdevice_volume_p->hfp_vol < TGT_VOLUME_LEVEL_0)
    {
        btdevice_volume_p->hfp_vol = TGT_VOLUME_LEVEL_0;
#ifdef MEDIA_PLAYER_SUPPORT
        media_PlayAudio(AUD_ID_BT_WARNING,0);
#endif
    }
#ifndef FPGA
    nv_record_touch_cause_flush();
#endif

    TRACE("%s a2dp: %d", __func__, btdevice_volume_p->a2dp_vol);
    TRACE("%s hfp: %d", __func__, btdevice_volume_p->hfp_vol);
}

int app_bt_stream_volumeset(int8_t vol)
{
    uint32_t ret;
    struct AF_STREAM_CONFIG_T *stream_cfg = NULL;
    TRACE("app_bt_stream_volumeset vol=%d", vol);

    if (vol > TGT_VOLUME_LEVEL_15)
        vol = TGT_VOLUME_LEVEL_15;
    if (vol < TGT_VOLUME_LEVEL_MUTE)
        vol = TGT_VOLUME_LEVEL_MUTE;

    stream_local_volume = vol;
    if (!app_bt_stream_isrun(APP_PLAY_BACK_AUDIO))
    {
        ret = af_stream_get_cfg(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg, true);
        if (ret == 0) {
            stream_cfg->vol = vol;
            af_stream_setup(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, stream_cfg);
        }
#if defined(AUDIO_ANC_FB_MC) && defined(__BT_ANC__) && !defined(__AUDIO_RESAMPLE__)
        ret = af_stream_get_cfg(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK, &stream_cfg, true);
        if (ret == 0) {
            stream_cfg->vol = vol;
            af_stream_setup(AUD_STREAM_ID_2, AUD_STREAM_PLAYBACK, stream_cfg);
        }
#endif

        
    }
    return 0;
}

int app_bt_stream_local_volume_get(void)
{
    return stream_local_volume;
}

void app_bt_stream_a2dpvolume_reset(void)
{
    btdevice_volume_p->a2dp_vol = NVRAM_ENV_STREAM_VOLUME_A2DP_VOL_DEFAULT ;
}

void app_bt_stream_hfpvolume_reset(void)
{
    btdevice_volume_p->hfp_vol = NVRAM_ENV_STREAM_VOLUME_HFP_VOL_DEFAULT;
}

void app_bt_stream_volume_ptr_update(uint8_t *bdAddr)
{
    static struct btdevice_volume stream_volume = {NVRAM_ENV_STREAM_VOLUME_A2DP_VOL_DEFAULT,NVRAM_ENV_STREAM_VOLUME_HFP_VOL_DEFAULT};
    nvrec_btdevicerecord *record = NULL;

#ifndef FPGA
    if (!nv_record_btdevicerecord_find((BT_BD_ADDR *)bdAddr,&record))
    {
        btdevice_volume_p = &(record->device_vol);
        DUMP8("0x%02x ", bdAddr, BD_ADDR_SIZE);
        TRACE("%s a2dp_vol:%d hfp_vol:%d ptr:0x%x", __func__, btdevice_volume_p->a2dp_vol, btdevice_volume_p->hfp_vol,btdevice_volume_p);
    }
    else
#endif
    {
        btdevice_volume_p = &stream_volume;
        TRACE("%s default", __func__);
    }
}

struct btdevice_volume * app_bt_stream_volume_get_ptr(void)
{
    return btdevice_volume_p;
}

bool app_bt_stream_isrun(APP_BT_STREAM_T player)
{

    if (gStreamplayer == player)
        return true;
    else
        return false;
}

int app_bt_stream_closeall()
{
    TRACE("app_bt_stream_closeall");

    bt_sco_player(false, APP_SYSFREQ_32K);
    bt_sbc_player(PLAYER_OPER_STOP, APP_SYSFREQ_32K);

#ifdef MEDIA_PLAYER_SUPPORT
    app_play_audio_onoff(false, 0);
#endif
#ifdef MEDIA_PLAYER_RBCODEC
    app_rbplay_audio_onoff(false, 0);
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
    app_ai_voice_stop_mic_stream();
#endif

    gStreamplayer = APP_BT_STREAM_NUM;
    return 0;
}

void app_bt_stream_copy_track_one_to_two_16bits(int16_t *dst_buf, int16_t *src_buf, uint32_t src_len)
{
    // Copy from tail so that it works even if dst_buf == src_buf
    for (int i = (int)(src_len - 1); i >= 0; i--)
    {
        dst_buf[i*2 + 0] = dst_buf[i*2 + 1] = src_buf[i];
    }
}

void app_bt_stream_copy_track_two_to_one_16bits(int16_t *dst_buf, int16_t *src_buf, uint32_t dst_len)
{
    for (uint32_t i = 0; i < dst_len; i++)
    {
        dst_buf[i] = src_buf[i*2];
    }
}

// =======================================================
// APP RESAMPLE
// =======================================================

#include "resample_coef.h"

static float ratio_step_factor = 1.0f;

static void memzero_int16(void *dst, uint32_t len)
{
    int16_t *dst16 = (int16_t *)dst;
    int16_t *dst16_end = dst16 + len / 2;

    while (dst16 < dst16_end)
    {
        *dst16++ = 0;
    }
}

static struct APP_RESAMPLE_T *app_resample_open(enum AUD_STREAM_T stream, const struct RESAMPLE_COEF_T *coef,
        enum AUD_CHANNEL_NUM_T chans, APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len, float ratio_step)
{
    struct APP_RESAMPLE_T *resamp;
    struct RESAMPLE_CFG_T cfg;
    enum RESAMPLE_STATUS_T status;
    uint32_t size, resamp_size;
    uint8_t *buf;

    resamp_size = audio_resample_ex_get_buffer_size(chans, AUD_BITS_16, coef->phase_coef_num);

    size = sizeof(struct APP_RESAMPLE_T);
    size += ALIGN(iter_len, 4);
    size += resamp_size;

    app_audio_mempool_get_buff(&buf, size);

    resamp = (struct APP_RESAMPLE_T *)buf;
    buf += sizeof(*resamp);
    resamp->stream = stream;
    resamp->cb = cb;
    resamp->iter_buf = buf;
    buf += ALIGN(iter_len, 4);
    resamp->iter_len = iter_len;
    resamp->offset = iter_len;
    resamp->ratio_step = ratio_step;

    memset(&cfg, 0, sizeof(cfg));
    cfg.chans = chans;
    cfg.bits = AUD_BITS_16;
    if (stream == AUD_STREAM_PLAYBACK) {
        cfg.ratio_step = ratio_step * ratio_step_factor;
    } else {
        cfg.ratio_step = ratio_step / ratio_step_factor;
    }
    cfg.coef = coef;
    cfg.buf = buf;
    cfg.size = resamp_size;

    status = audio_resample_ex_open(&cfg, (RESAMPLE_ID *)&resamp->id);
    ASSERT(status == RESAMPLE_STATUS_OK, "%s: Failed to open resample: %d", __func__, status);

#ifdef CHIP_BEST1000
    hal_cmu_audio_resample_enable();
#endif

    return resamp;
}

static int app_resample_close(struct APP_RESAMPLE_T *resamp)
{
#ifdef CHIP_BEST1000
    hal_cmu_audio_resample_disable();
#endif

    if (resamp)
    {
        audio_resample_ex_close((RESAMPLE_ID *)resamp->id);
    }

    return 0;
}

struct APP_RESAMPLE_T *app_playback_resample_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_CHANNEL_NUM_T chans,
        APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len)
{
    const struct RESAMPLE_COEF_T *coef = NULL;

    if (sample_rate == AUD_SAMPRATE_8000)
    {
        coef = &resample_coef_8k_to_8p4k;
    }
    else if (sample_rate == AUD_SAMPRATE_16000)
    {
        coef = &resample_coef_8k_to_8p4k;
    }
    else if (sample_rate == AUD_SAMPRATE_32000)
    {
        coef = &resample_coef_32k_to_50p7k;
    }
    else if (sample_rate == AUD_SAMPRATE_44100)
    {
        coef = &resample_coef_44p1k_to_50p7k;
    }
    else if (sample_rate == AUD_SAMPRATE_48000)
    {
#if defined(SW_PLAYBACK_RESAMPLE_441TO48K)
        coef = &resample_coef_44p1k_to_48k_24coef;
#else
        coef = &resample_coef_48k_to_50p7k;
#endif
    }
    else
    {
        ASSERT(false, "%s: Bad sample rate: %u", __func__, sample_rate);
    }

    return app_resample_open(AUD_STREAM_PLAYBACK, coef, chans, cb, iter_len, 0);
}

struct APP_RESAMPLE_T *app_playback_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
        APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
        float ratio_step)
{
    const struct RESAMPLE_COEF_T *coef = &resample_coef_any_up256;

    return app_resample_open(AUD_STREAM_PLAYBACK, coef, chans, cb, iter_len, ratio_step);
}

int app_playback_resample_close(struct APP_RESAMPLE_T *resamp)
{
    return app_resample_close(resamp);
}

int app_playback_resample_run(struct APP_RESAMPLE_T *resamp, uint8_t *buf, uint32_t len)
{
    uint32_t in_size, out_size;
    struct RESAMPLE_IO_BUF_T io;
    enum RESAMPLE_STATUS_T status;
    int ret;
    //uint32_t lock;

    if (resamp == NULL)
    {
        goto _err_exit;
    }

    io.out_cyclic_start = NULL;
    io.out_cyclic_end = NULL;

    if (resamp->offset < resamp->iter_len)
    {
        io.in = resamp->iter_buf + resamp->offset;
        io.in_size = resamp->iter_len - resamp->offset;
        io.out = buf;
        io.out_size = len;

        //lock = int_lock();
        status = audio_resample_ex_run((RESAMPLE_ID *)resamp->id, &io, &in_size, &out_size);
        //int_unlock(lock);
        if (status != RESAMPLE_STATUS_OUT_FULL && status != RESAMPLE_STATUS_IN_EMPTY &&
            status != RESAMPLE_STATUS_DONE)
        {
            goto _err_exit;
        }

        buf += out_size;
        len -= out_size;
        resamp->offset += in_size;

        ASSERT(len == 0 || resamp->offset == resamp->iter_len,
            "%s: Bad resample offset: len=%d offset=%u iter_len=%u",
            __func__, len, resamp->offset, resamp->iter_len);
    }

    while (len)
    {
        ret = resamp->cb(resamp->iter_buf, resamp->iter_len);
        if (ret)
        {
            goto _err_exit;
        }

        io.in = resamp->iter_buf;
        io.in_size = resamp->iter_len;
        io.out = buf;
        io.out_size = len;

        //lock = int_lock();
        status = audio_resample_ex_run((RESAMPLE_ID *)resamp->id, &io, &in_size, &out_size);
        //int_unlock(lock);
        if (status != RESAMPLE_STATUS_OUT_FULL && status != RESAMPLE_STATUS_IN_EMPTY &&
            status != RESAMPLE_STATUS_DONE)
        {
            goto _err_exit;
        }

        ASSERT(out_size <= len, "%s: Bad resample out_size: out_size=%u len=%d", __func__, out_size, len);
        ASSERT(in_size <= resamp->iter_len, "%s: Bad resample in_size: in_size=%u iter_len=%u", __func__, in_size, resamp->iter_len);

        buf += out_size;
        len -= out_size;
        if (in_size != resamp->iter_len)
        {
            resamp->offset = in_size;

            ASSERT(len == 0, "%s: Bad resample len: len=%d out_size=%u", __func__, len, out_size);
        }
    }

    return 0;

_err_exit:
    if (resamp)
    {
        app_resample_reset(resamp);
    }

    memzero_int16(buf, len);

    return 1;
}

struct APP_RESAMPLE_T *app_capture_resample_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_CHANNEL_NUM_T chans,
        APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len)
{
    const struct RESAMPLE_COEF_T *coef = NULL;

    if (sample_rate == AUD_SAMPRATE_8000)
    {
        coef = &resample_coef_8p4k_to_8k;
    }
    else if (sample_rate == AUD_SAMPRATE_16000)
    {
        // Same coef as 8K sample rate
        coef = &resample_coef_8p4k_to_8k;
    }
    else
    {
        ASSERT(false, "%s: Bad sample rate: %u", __func__, sample_rate);
    }

    return app_resample_open(AUD_STREAM_CAPTURE, coef, chans, cb, iter_len, 0);
}

struct APP_RESAMPLE_T *app_capture_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
        APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
        float ratio_step)
{
    const struct RESAMPLE_COEF_T *coef = &resample_coef_any_up256;

    return app_resample_open(AUD_STREAM_CAPTURE, coef, chans, cb, iter_len, ratio_step);
}

int app_capture_resample_close(struct APP_RESAMPLE_T *resamp)
{
    return app_resample_close(resamp);
}

int app_capture_resample_run(struct APP_RESAMPLE_T *resamp, uint8_t *buf, uint32_t len)
{
    uint32_t in_size, out_size;
    struct RESAMPLE_IO_BUF_T io;
    enum RESAMPLE_STATUS_T status;
    int ret;

    if (resamp == NULL)
    {
        goto _err_exit;
    }

    io.out_cyclic_start = NULL;
    io.out_cyclic_end = NULL;

    if (resamp->offset < resamp->iter_len)
    {
        io.in = buf;
        io.in_size = len;
        io.out = resamp->iter_buf + resamp->offset;
        io.out_size = resamp->iter_len - resamp->offset;

        status = audio_resample_ex_run((RESAMPLE_ID *)resamp->id, &io, &in_size, &out_size);
        if (status != RESAMPLE_STATUS_OUT_FULL && status != RESAMPLE_STATUS_IN_EMPTY &&
            status != RESAMPLE_STATUS_DONE)
        {
            goto _err_exit;
        }

        buf += in_size;
        len -= in_size;
        resamp->offset += out_size;

        ASSERT(len == 0 || resamp->offset == resamp->iter_len,
            "%s: Bad resample offset: len=%d offset=%u iter_len=%u",
            __func__, len, resamp->offset, resamp->iter_len);

        if (resamp->offset == resamp->iter_len)
        {
            ret = resamp->cb(resamp->iter_buf, resamp->iter_len);
            if (ret)
            {
                goto _err_exit;
            }
        }
    }

    while (len)
    {
        io.in = buf;
        io.in_size = len;
        io.out = resamp->iter_buf;
        io.out_size = resamp->iter_len;

        status = audio_resample_ex_run((RESAMPLE_ID *)resamp->id, &io, &in_size, &out_size);
        if (status != RESAMPLE_STATUS_OUT_FULL && status != RESAMPLE_STATUS_IN_EMPTY &&
            status != RESAMPLE_STATUS_DONE)
        {
            goto _err_exit;
        }

        ASSERT(in_size <= len, "%s: Bad resample in_size: in_size=%u len=%u", __func__, in_size, len);
        ASSERT(out_size <= resamp->iter_len, "%s: Bad resample out_size: out_size=%u iter_len=%u", __func__, out_size, resamp->iter_len);

        buf += in_size;
        len -= in_size;
        if (out_size == resamp->iter_len)
        {
            ret = resamp->cb(resamp->iter_buf, resamp->iter_len);
            if (ret)
            {
                goto _err_exit;
            }
        }
        else
        {
            resamp->offset = out_size;

            ASSERT(len == 0, "%s: Bad resample len: len=%u in_size=%u", __func__, len, in_size);
        }
    }

    return 0;

_err_exit:
    if (resamp)
    {
        app_resample_reset(resamp);
    }

    memzero_int16(buf, len);

    return 1;
}

void app_resample_reset(struct APP_RESAMPLE_T *resamp)
{
    audio_resample_ex_flush((RESAMPLE_ID *)resamp->id);
    resamp->offset = resamp->iter_len;
}

void app_resample_tune(struct APP_RESAMPLE_T *resamp, uint32_t rate, int32_t sample, uint32_t ms)
{
    float freq_offset;
    float new_step;

    if (resamp == NULL)
    {
        return;
    }

    freq_offset = (float)sample * 1000 / (rate * ms);
    ratio_step_factor *= 1 - freq_offset;

    TRACE("%s: ppb=%d", __FUNCTION__, (int)(freq_offset * 1000 * 1000 * 1000));
    if (resamp->stream == AUD_STREAM_PLAYBACK) {
        new_step = resamp->ratio_step * ratio_step_factor;
    } else {
        new_step = resamp->ratio_step / ratio_step_factor;
    }
    audio_resample_ex_set_ratio_step(resamp->id, new_step);
}

void app_resample_set_tune_factor(struct APP_RESAMPLE_T *resamp, float factor)
{
    float new_step;

    ratio_step_factor = factor;

    if (resamp == NULL)
    {
        return;
    }

    if (resamp->stream == AUD_STREAM_PLAYBACK) {
        new_step = resamp->ratio_step * ratio_step_factor;
    } else {
        new_step = resamp->ratio_step / ratio_step_factor;
    }

    audio_resample_ex_set_ratio_step(resamp->id, new_step);
}

float app_resample_get_tune_factor(void)
{
    return ratio_step_factor;
}


#ifdef __PCM_FFT__

#include "math.h"
#include "med_memory.h"

extern "C" {
extern void *spx_fft_init(int size);
extern void spx_fft_destroy(void *table);
extern void spx_fft(void *table, short *in, short *out);
}

typedef struct {
    /* Basic info */
    int     frame_size;        /**< Number of samples processed each time */
    short   *ft; 
    void    *fft_lookup;       /**< Lookup table for the FFT */
} PCM_FFT_T;
static PCM_FFT_T s_pcm_fft_cntx;
PCM_FFT_T *p_pcm_fft_cntx = &s_pcm_fft_cntx;

#define PCM_FFT_SAMPLE_NUM     (256)
#define PCM_FFT_CNTX_BUFF_SIZE (1024)
static uint32_t p_pcm_fft_cntx_buf[PCM_FFT_CNTX_BUFF_SIZE/sizeof(uint32_t)];
static uint32_t p_pcm_fft_MED_mem[8*PCM_FFT_CNTX_BUFF_SIZE/sizeof(uint32_t)];
//struct med_heap_instance music_fft_heap_inst;
extern struct med_heap_instance voice_med_heap_inst;

int pcm_fft_calc_init(void)
{
    int N;
    PCM_FFT_T *st = NULL;
#if defined(MED_MULTI_INSTANCE)
	med_system_heap_init(&voice_med_heap_inst,&p_pcm_fft_MED_mem[0], &p_pcm_fft_MED_mem[PCM_FFT_CNTX_BUFF_SIZE - 1]);
#else
    med_system_heap_init(&p_pcm_fft_MED_mem[0], &p_pcm_fft_MED_mem[PCM_FFT_CNTX_BUFF_SIZE - 1]);
#endif
    char *pbuf = (char *)p_pcm_fft_cntx_buf;

    uint32_t buf_size = 0;

    TRACE("[%s] Enter...", __func__);

    memset(&s_pcm_fft_cntx, 0, sizeof(PCM_FFT_T));

    st = p_pcm_fft_cntx;

 
    st->frame_size = PCM_FFT_SAMPLE_NUM;
    N = st->frame_size;

    buf_size = 0;

    st->ft = (short*)(pbuf + buf_size);

    buf_size += (N*sizeof(short)+3)/4*4;

    ASSERT(buf_size <= PCM_FFT_CNTX_BUFF_SIZE," %s ",__func__);

    st->fft_lookup = spx_fft_init(N); 
    TRACE("[%s] exit %x...%d", __func__,st->fft_lookup,N);
    return 0;
}

int pcm_fft_calc_deinit(void)
{
    PCM_FFT_T *st = p_pcm_fft_cntx;

    TRACE("[%s] Enter...", __func__);

    spx_fft_destroy(st->fft_lookup);

    return 0;
}

#define POW2_16(a)		(((int32_t)(short)(a))*((int32_t)(short)(a)))

SRAM_TEXT_LOC int pcm_fft_calc_run(short *pcm_buf)
{
    PCM_FFT_T *st = p_pcm_fft_cntx;
	uint32_t i ;
	uint8_t fft_head[2],fft_data[2];
	short fft_buf[PCM_FFT_SAMPLE_NUM];
	int32_t spectrum[12];
	uint16_t fft_data_temp[8];


	
	for(i = 0; i < st->frame_size; i++)
	{
	 	 //fft_buf[i] = pcm_buf[2*i];//get mono left channel only
		 
		 fft_buf[i] = pcm_buf[2*i]/2 + pcm_buf[2*i+1]/2;
	}
	   // /* Perform FFT */
	   /* * Input: pcm[0], pcm[1], pcm[2], ..., pcm[N-1] *
	   Output: real[0], real[1], imag[1], real[2], imag[2], ...,
	   real[N/2-1], imag[N/2-1], real[N/2] */
	 spx_fft(st->fft_lookup, fft_buf, st->ft);


	 spectrum[0] =POW2_16(st->ft[1]) + POW2_16(st->ft[2]);
	 spectrum[1] =POW2_16(st->ft[3]) + POW2_16(st->ft[4]);
	 spectrum[2] =POW2_16(st->ft[5]) + POW2_16(st->ft[6]);
	 spectrum[3] =POW2_16(st->ft[11]) + POW2_16(st->ft[12]);
	 spectrum[4] =POW2_16(st->ft[19]) + POW2_16(st->ft[20]);
	 spectrum[5] =POW2_16(st->ft[31]) + POW2_16(st->ft[32]);
	 spectrum[6] =POW2_16(st->ft[55]) + POW2_16(st->ft[56]);
	 spectrum[7] =POW2_16(st->ft[95]) + POW2_16(st->ft[96]);

	 fft_head[0] = 0x55;
	 fft_head[1] = 0x10;

	// printf("%x",fft_head[0]);
	 //hal_trace_printf("%x",fft_head[0]);


#ifdef  ENA_FFT_UART
	hal_trace_output(fft_head,2);

#endif

	 for(i = 0; i < 8; i++)
	 {

		fft_data_temp[i] = (uint16_t)sqrt(spectrum[i]);

		
		fft_data[0] = (uint8_t)((fft_data_temp[i]&0xff00) >> 8);
		fft_data[1] = (uint8_t)((fft_data_temp[i]&0xff));

#ifdef  ENA_FFT_UART		
		hal_trace_output(fft_data,2);		
#endif

	 }
#ifndef  ENA_FFT_UART
	TRACE("fft: 0x%x;0x%x;0x%x;0x%x;0x%x;0x%x;0x%x;0x%x",fft_data_temp[0],fft_data_temp[1],fft_data_temp[2],fft_data_temp[3],fft_data_temp[4],fft_data_temp[5],fft_data_temp[6],fft_data_temp[7]);
#endif

    return 0;
}



#endif


