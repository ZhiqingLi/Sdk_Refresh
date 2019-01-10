#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_aud.h"
#include "audioflinger.h"
#include "app_overlay.h"
#include "app_audio.h"
#include "app_utils.h"
#include "apps.h"
#include "app_thread.h"
#include "hal_location.h"
#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif

#include "app_ai_voice.h"
#include "voice_enhancement.h"
#include "voice_compression.h"

#ifdef __SMARTVOICE__
#include "app_sv_handle.h"
#endif

#ifdef __AMA_VOICE__
#include "app_ama_handle.h"
#include "ama_stream.h"
#endif

#ifdef __KNOWLES
#include "knowles_uart.h"

#ifndef KNOWLES_UART_DATA
extern uint32_t kw_start_index;
extern uint32_t kw_end_index;
#endif
#endif

#ifdef __DMA_VOICE__
#include "app_dma_handle.h"
#endif

#include "audio_dump.h"

//#define AI_VOICE_NOISE_CANCEL

#ifdef AI_VOICE_NOISE_CANCEL

#include "lc_mmse_ns.h"
#include "med_memory.h"
LC_MMSE_NS_State *speech_tx_ns2_st = NULL;
#define NS_BUFF_SIZE  1024 * 21
static uint8_t ns_mem[NS_BUFF_SIZE];
extern struct med_heap_instance voice_med_heap_inst;
#endif

/**< configure the voice settings   */
#define VOB_VOICE_BIT_NUMBER                    (AUD_BITS_16)
#define VOB_VOICE_SAMPLE_RATE                   (AUD_SAMPRATE_16000)
#define VOB_VOICE_CAPTURE_VOLUME                (10)
#ifdef SPEECH_CAPTURE_TWO_CHANNEL
#define VOB_MIC_CHANNEL_COUNT					2
#else
#define VOB_MIC_CHANNEL_COUNT					1
#endif

#define VOB_PCM_SIZE_TO_SAMPLE_CNT(size)		((size)*8/VOB_VOICE_BIT_NUMBER)
#define VOB_SAMPLE_CNT_TO_PCM_SIZE(sample_cnt)	((sample_cnt)*VOB_VOICE_BIT_NUMBER/8)



#define VOB_IGNORED_FRAME_CNT                   (0)

// ping-pong buffer for codec
#define VOB_VOICE_PCM_DATA_SIZE                 (VOB_VOICE_PCM_CAPTURE_CHUNK_SIZE*2)

#define APP_SYSFREQ_USER_SMARTVOICE             APP_SYSFREQ_USER_APP_4



//static APP_AI_STREAM_ENV   ai_voice_config;
APP_AI_STREAM_ENV   ai_voice_config;

static uint32_t ignoredPcmDataRounds = 0; 

static void merge_stereo_to__16bits(int16_t *src_buf, int16_t *dst_buf,  uint32_t src_len)
{
    uint32_t i = 0;
    for (i = 0; i < src_len; i+=2)
    {
		dst_buf[i/2] = (src_buf[i]);
    }
}

#include "communication_svr.h"
#include "communication_sysapi.h"


static uint32_t mic_data_come(uint8_t* ptrBuf, uint32_t length)
{
	uint32_t stime,etime;
	stime=hal_sys_timer_get();
	//DUMP8("%02x ",ptrBuf,8);
	
    // if the voice stream is already on, we won't start the a2dp stream right-now,
	// otherwise there will be confliction between the mic&speaker audio callback
	// triggering. So we put set the pending flag when opening a2dp stream and 
	// trigger the a2dp stream at the time of catpure stream callback function here.
    if (app_voicepath_get_stream_pending_state(AUDIO_OUTPUT_STREAMING))
    {
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        app_voicepath_set_stream_state(AUDIO_OUTPUT_STREAMING, true);
        app_voicepath_set_pending_started_stream(AUDIO_OUTPUT_STREAMING, false);
    }

	if (voice_compression_validity_check())
	{
		TRACE("voice over ble encoded data buffer is full!!!");
		voice_compression_reset_encode_buf();
		return length;
	}
#ifdef __SMARTVOICE__
    if (!app_smartvoice_get_stream_mic_opended_status())
    {
        return length;
    }
#endif
#if defined (KNOWLES_I2S_INTERFACE)||defined (__ANKER_UI__)
	merge_stereo_to__16bits((int16_t *)ptrBuf,(int16_t *)ptrBuf,length/2);
	length=length/2;
#endif
	audio_dump_add_channel_data(0, (short *)ptrBuf, length/2);

    if (ignoredPcmDataRounds < VOB_IGNORED_FRAME_CNT)
    {
        ignoredPcmDataRounds++;
		return length;
        memset(ptrBuf, 0, length);
    }
	
#if APP_SMARTVOICE_VOICE_ENHANCEMENT
    //sv_enhancement_process_capture(ptrBuf, length);
#endif
	//audio_dump_add_channel_data(0, (short *)ptrBuf, length/2);
#ifdef AI_VOICE_NOISE_CANCEL
	lc_mmse_ns_run(speech_tx_ns2_st,(short *)ptrBuf);
#endif
	//audio_dump_add_channel_data(1, (short *)ptrBuf, length/2);

    // encode the voice PCM data
    //TRACE("%s len:%d", __func__, length);
	voice_compression_handle(ptrBuf, VOB_PCM_SIZE_TO_SAMPLE_CNT(length), ai_voice_config.isAudioAlgorithmEngineReseted);
	ai_voice_config.isAudioAlgorithmEngineReseted = false;      
#ifdef __SMARTVOICE__
	app_smartvoice_send_encoded_audio_data();
#endif
#ifdef __AMA_VOICE__
	app_ama_send_voice_stream();
#endif
#ifdef __DMA_VOICE__
	app_dma_send_voice_stream();
#endif
	etime=hal_sys_timer_get();
	//TRACE("OPUS COST TIME %d",etime-stime);
	audio_dump_run();
    return length;
}

#ifdef __AMA_VOICE__

#ifdef __KNOWLES
#ifndef KNOWLES_UART_DATA

uint32_t getKWStartMarker()
{
	uint32 cmd, resp;
	int rc;
	uint16_t marker;

	cmd = 0x803A0000;
	if (iacore_uart_cmd(iacore, cmd, &resp))
	{
	    TRACE("%s Error reading KW Start marker %d\n", __func__, rc);
	    return -1;
	}

	marker = (uint16_t)(resp & 0x0000FFFF);

	return marker;
}


uint32_t getKWEndMarker()
{
	uint32 cmd, resp;
	int rc;
	uint16_t marker;

	cmd = 0x803A0001;
	if (iacore_uart_cmd(iacore, cmd, &resp))
	{
	              TRACE("%s Error reading KW Start marker %d\n", __func__, rc);
	              return -1;
	}

	marker = (uint16_t)(resp & 0x0000FFFF);

	return marker;
}
#endif
#endif
#endif

int app_ai_voice_start_mic_stream(void)
{
    TRACE("%s %d",__func__,__LINE__);

    if (!ai_voice_config.isSvStreamRunning)
    {
        struct AF_STREAM_CONFIG_T stream_cfg;
        
        uint8_t* bt_audio_buff = NULL;

        TRACE("%s", __func__);

		audio_dump_init(320, 1);
#ifdef __THIRDPARTY
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_MIC_OPEN);
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_STOP);
#endif
        //app_overlay_select(APP_OVERLAY_HFP);
#ifdef RECORD_LONELY
		app_capture_audio_mempool_init();
#else
		app_audio_mempool_init();
#endif
        
#ifdef __SMARTVOICE__
		app_smartvoice_stream_init();
#endif
#ifdef __AMA_VOICE__
 		app_ama_stream_init();//get memory for transfer
#endif

#if APP_SMARTVOICE_VOICE_ENHANCEMENT
        //sv_enhancement_init(VOB_VOICE_SAMPLE_RATE, SV_ENHANCEMENT_FRAME_SIZE, VOB_MIC_CHANNEL_COUNT);
#endif

#ifdef AI_VOICE_NOISE_CANCEL
#if defined(MED_MULTI_INSTANCE)
		med_system_heap_init(&voice_med_heap_inst, &ns_mem[0], &ns_mem[NS_BUFF_SIZE - 1]);
#else
		med_system_heap_init(&ns_mem[0], &ns_mem[NS_BUFF_SIZE - 1]);
#endif
		speech_tx_ns2_st = lc_mmse_ns_init(320, 16000, -15);
#endif
		voice_compression_init();

		app_sysfreq_req(APP_SYSFREQ_USER_SMARTVOICE, APP_SYSFREQ_208M);  

		TRACE("====>sys freq calc : %d\n", hal_sys_timer_calc_cpu_freq(50, 0));

        ai_voice_config.isAudioAlgorithmEngineReseted = true;

        // create the audio flinger stream
        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.bits = VOB_VOICE_BIT_NUMBER;
        stream_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)VOB_MIC_CHANNEL_COUNT;
 		
#if   defined (KNOWLES_I2S_INTERFACE)
		stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
		stream_cfg.device = AUD_STREAM_USE_I2S_MASTER;
		stream_cfg.io_path = AUD_IO_PATH_NULL;
		stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
#elif defined  (__ANKER_UI__)
		stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
		stream_cfg.device = AUD_STREAM_USE_I2S_SLAVE;
		stream_cfg.io_path = AUD_IO_PATH_NULL;
		stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
#else
		stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
		stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif

#if defined(__AUDIO_RESAMPLE__) && defined(SW_CAPTURE_RESAMPLE)
        if (VOB_VOICE_SAMPLE_RATE == AUD_SAMPRATE_8000) {
            stream_cfg.sample_rate = AUD_SAMPRATE_8463;
        } else if (VOB_VOICE_SAMPLE_RATE == AUD_SAMPRATE_16000) {
            stream_cfg.sample_rate = AUD_SAMPRATE_16927;
        }
#else
        stream_cfg.sample_rate = VOB_VOICE_SAMPLE_RATE;
#endif
    
        stream_cfg.vol = VOB_VOICE_CAPTURE_VOLUME;
        stream_cfg.data_size = VOB_VOICE_PCM_DATA_SIZE * stream_cfg.channel_num;
        ignoredPcmDataRounds = 0;
#ifdef RECORD_LONELY
		app_capture_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
#else
        app_audio_mempool_get_buff(&bt_audio_buff, stream_cfg.data_size);
#endif        
        stream_cfg.handler = mic_data_come;
        stream_cfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);

        af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);
        // if a2dp streaming is not on, start the playback stream
        if (!app_voicepath_get_stream_state(AUDIO_OUTPUT_STREAMING))
        {              
            app_voicepath_set_stream_state(VOICEPATH_STREAMING, true);
            af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        }
        else
        {            
            app_voicepath_set_pending_started_stream(VOICEPATH_STREAMING, true);       
        }
        ai_voice_config.isSvStreamRunning = true;

#ifdef __KNOWLES
#ifdef __AMA_VOICE__		
#ifndef KNOWLES_UART_DATA
#if AMA_WAKEUP_TYPE==AMA_KEYWORD_WAKEUP
uint8_t	markers_captured = 0;

		if (markers_captured == 0)
		{
			uint16_t start_marker, end_marker;
			//uint32_t start_index, end_index;
													 
#define SAMPLES_PER_FRAME (256)
		
			start_marker = getKWStartMarker();
			end_marker   = getKWEndMarker();
			 
			TRACE("%s Start Marker frame# 0x%08lx\n\r", __func__, start_marker);
			TRACE("%s End Marker frame# 0x%08lx\n\r", __func__, end_marker);

			/* Convert the markers from frame count to sample numbers */
			kw_start_index = start_marker * SAMPLES_PER_FRAME;
			kw_end_index = end_marker * SAMPLES_PER_FRAME;
			 
			//ama_fillwakeword_indices(start_index, end_index);

			/* Send StartSpeech command */
			ama_start_speech_request();

			markers_captured = 1;
		}
#endif
#endif     
#endif
#endif
        return 0;
    }
    else
    {
        return -1;
    }
}


//close stream
int app_ai_voice_stop_mic_stream(void)
{
    TRACE("%s %d ",__func__,__LINE__);

    if (ai_voice_config.isSvStreamRunning)
    {
    	audio_dump_clear_up();
#ifdef __THIRDPARTY
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_MIC_CLOSE);
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_START);
#endif
#ifdef __AMA_VOICE__
		app_ama_stream_deinit();
#endif
		voice_compression_reset_encode_buf();

		voice_compression_deinit();
#ifdef AI_VOICE_NOISE_CANCEL		
		lc_mmse_ns_deinit(speech_tx_ns2_st);

		uint32_t total = 0, used = 0, max_used = 0;
#if defined(MED_MULTI_INSTANCE)
		med_memory_info(&voice_med_heap_inst, &total, &used, &max_used);
#else
		med_memory_info(&total, &used, &max_used);
#endif
#endif
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
        
        app_sysfreq_req(APP_SYSFREQ_USER_SMARTVOICE, APP_SYSFREQ_32K);

        //app_overlay_unloadall();

        app_voicepath_set_pending_started_stream(VOICEPATH_STREAMING, false);
        app_voicepath_set_stream_state(VOICEPATH_STREAMING, false);

        ai_voice_config.isSvStreamRunning = false;
        TRACE("%s %d isSvStreamRunning set false",__func__,__LINE__);

        return 0;
    }
    else
    {
        return -1;
    }
}


#ifdef KNOWLES_UART_DATA


//1.creat an encode and send thread

//2.add an init function to init the opus encode

//3.add a deinit function to stop encode and transfer

osThreadId knowles_uart_audio_tid;
void uart_data_process_thread(void const *argument);
osThreadDef(uart_data_process_thread, osPriorityAboveNormal,1024*12);


int app_ai_voice_uart_audio_init()
{
	knowles_uart_audio_tid  = osThreadCreate(osThread(uart_data_process_thread),NULL);
	if (knowles_uart_audio_tid == NULL)  {
		TRACE("[UART AUDIO]Create thread error\n");
		return -1;
	}
}


uint8_t *uart_stream_buf;
bool send_start_speech = false;

int app_ai_voice_start_stream_by_uart()
{
	if (!ai_voice_config.isSvStreamRunning)
	{
		TRACE("%s", __func__);

#ifdef RECORD_LONELY
		app_capture_audio_mempool_init();
#else
		app_audio_mempool_init();
#endif
		
#ifdef __SMARTVOICE__
		app_smartvoice_stream_init();
#endif
#ifdef __AMA_VOICE__
		app_ama_stream_init();//get memory for transfer
#endif
		voice_compression_init();

#ifdef RECORD_LONELY
		app_capture_audio_mempool_get_buff(&uart_stream_buf, 640);
#else
		app_audio_mempool_get_buff(&uart_stream_buf, 640);
#endif        

#ifdef AI_VOICE_NOISE_CANCEL
#if defined(MED_MULTI_INSTANCE)
		med_system_heap_init(&voice_med_heap_inst, &ns_mem[0], &ns_mem[NS_BUFF_SIZE - 1]);
#else
		med_system_heap_init(&ns_mem[0], &ns_mem[NS_BUFF_SIZE - 1]);
#endif
		speech_tx_ns2_st = lc_mmse_ns_init(320, 16000, -15);
#endif

		app_sysfreq_req(APP_SYSFREQ_USER_SMARTVOICE, APP_SYSFREQ_208M);  
		TRACE("====>sys freq calc : %d\n", hal_sys_timer_calc_cpu_freq(50, 0));
		ai_voice_config.isAudioAlgorithmEngineReseted = true;

		uart_audio_init();//open uart DMA receive,send bust stream cmd

		ai_voice_config.isSvStreamRunning = true;

		send_start_speech = true;
		return 0;
	}
	else
	{
		return -1;
	}
}

int app_ai_voice_stop_stream_by_uart()
{
	if (ai_voice_config.isSvStreamRunning)
	{
		TRACE("=======>>>>%s", __func__);
		uart_audio_deinit();
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_MIC_CLOSE);
#ifdef __AMA_VOICE__
		app_ama_stream_deinit();
#endif
		voice_compression_deinit();
#ifdef AI_VOICE_NOISE_CANCEL
		lc_mmse_ns_deinit(speech_tx_ns2_st);
		uint32_t total = 0, used = 0, max_used = 0;
#if defined(MED_MULTI_INSTANCE)
		med_memory_info(&voice_med_heap_inst, &total, &used, &max_used);
#else
		med_memory_info(&total, &used, &max_used);
#endif
		TRACE("MED MALLOC MEM: total - %d, used - %d, max_used - %d.", total, used, max_used);
		ASSERT(used == 0, "[%s] used != 0", __func__);
#endif

		ai_voice_config.isSvStreamRunning = false;

		app_sysfreq_req(APP_SYSFREQ_USER_SMARTVOICE, APP_SYSFREQ_32K);

		return 0;
	}
	else
	{
		return -1;
	}
}





void uart_data_process_thread(void const *argument)
{
	uint32_t ret = 0;
	uint32_t frame_len=0;
	while(1)
	{
		TRACE("---in uart_process_thread\n");
		osEvent evt;
		
		if(avil_len_of_the_fifo()<=640){
			evt = osSignalWait(0x0, osWaitForever);
		}

		//TRACE("#####Available len of fifo  %d####",avil_len_of_the_fifo());
		//get role from signal value
		if(evt.status == osEventSignal)
		{
			if(evt.value.signals & UART_OUT_SIGNAL_ID)
			{
#ifdef __AMA_VOICE__ //only for AMA, DMA does not need any markers so we can send the start speech after keyword detection only
				if(send_start_speech)
				{
					ama_start_speech_request();
					send_start_speech = false;
				}
#endif
				frame_len=ama_uart_get_fifo_data(uart_stream_buf);
				if(frame_len != 0)
				{
					//TRACE("--data start encode %d",frame_len);
					//DUMP8("%02x ",uart_stream_buf,10);
		
					if (voice_compression_validity_check())
					{
						TRACE("voice over ble encoded data buffer is full!!!");
						voice_compression_reset_encode_buf();
					}
#ifdef AI_VOICE_NOISE_CANCEL
					lc_mmse_ns_run(speech_tx_ns2_st,(short *)uart_stream_buf);
#endif
					ret=voice_compression_handle(uart_stream_buf, VOB_PCM_SIZE_TO_SAMPLE_CNT(640), ai_voice_config.isAudioAlgorithmEngineReseted);
					ai_voice_config.isAudioAlgorithmEngineReseted = false; 
					osDelay(1);
#ifdef __SMARTVOICE__
					app_smartvoice_send_encoded_audio_data();
#endif
#ifdef __AMA_VOICE__
					app_ama_send_voice_stream();
#endif
#ifdef __DMA_VOICE__
					app_dma_send_voice_stream();
#endif

					//audio_dump_add_channel_data(0, (short *)uart_stream_buf,frame_len/2); 
					//audio_dump_run();	
					send_message();
					if(ret < 0)
					{
						goto __EXCEPTION;
					}

				}
			}

		}

		continue;

		__EXCEPTION:
		app_ai_voice_stop_stream_by_uart();
	}
}


#endif


static uint8_t app_voicepath_pending_streaming = 0;
static uint8_t app_voicepath_stream_state = 0;


void app_voicepath_set_pending_started_stream(uint8_t pendingStream, bool isEnabled)
{
    if (isEnabled)
    {
        app_voicepath_pending_streaming |= (1 << pendingStream);
    }
    else
    {
        app_voicepath_pending_streaming &= (~(1 << pendingStream));
    }
}

bool app_voicepath_get_stream_pending_state(uint8_t pendingStream)
{
	return 0;
    //return app_voicepath_pending_streaming & (1 << pendingStream);
}

bool app_voicepath_get_stream_state(uint8_t stream)
{
	return 0;
    //return (app_voicepath_stream_state & (1 << stream));
}

void app_voicepath_set_stream_state(uint8_t stream, bool isEnabled)
{
    if (isEnabled)
    {
        app_voicepath_stream_state |= (1 << stream);
    }
    else
    {
        app_voicepath_stream_state &= (~(1 << stream));
    }
}



