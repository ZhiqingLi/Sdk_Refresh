#include "hal_trace.h"
#include "hal_timer.h"
#include "app_audio.h"

#include "cmsis_os.h"
#include "g726.h"
#include "voice_opus.h"
#include "voice_sbc.h"
#include "cqueue.h"
#include <string.h>
#include <stdlib.h>
#include "voice_compression.h"
#ifdef __AMA_VOICE__
#include "ama_stream.h"
#endif

#define VOB_ENCODED_DATA_STORAGE_BUF_SIZE       8000// 40byte(20ms) 20*100=2000ms(2s)
#define TMP_BUF_FOR_ENCODING_DECODING_SIZE      1024



typedef struct
{
    CQueue      encodedDataBufQueue;
    uint8_t*    encodedDataBuf;
    uint8_t*    tmpBufForEncoding;
} APP_VOICE_COMPRESSION_ENV_T;   

static APP_VOICE_COMPRESSION_ENV_T voice_compression_mem;


static void app_pop_data_from_cqueue(CQueue* ptrQueue, uint8_t *buff, uint32_t len)
{
    uint8_t *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    PeekCQueue(ptrQueue, len, &e1, &len1, &e2, &len2);
    if (len==(len1+len2)){
        memcpy(buff,e1,len1);
        memcpy(buff+len1,e2,len2);
        DeCQueue(ptrQueue, 0, len);
    }else{
        memset(buff, 0x00, len);
    }
}


void voice_compression_init()
{
	// encoded data buffer
#ifdef RECORD_LONELY
	app_capture_audio_mempool_get_buff(&(voice_compression_mem.encodedDataBuf), 
		VOB_ENCODED_DATA_STORAGE_BUF_SIZE);//used to save encoded data
	InitCQueue(&(voice_compression_mem.encodedDataBufQueue), VOB_ENCODED_DATA_STORAGE_BUF_SIZE, 
				(CQItemType *)(voice_compression_mem.encodedDataBuf));
	app_capture_audio_mempool_get_buff(&(voice_compression_mem.tmpBufForEncoding),	 
		TMP_BUF_FOR_ENCODING_DECODING_SIZE);
#else	
	app_audio_mempool_get_buff(&(voice_compression_mem.encodedDataBuf),	
		VOB_ENCODED_DATA_STORAGE_BUF_SIZE);
	InitCQueue(&(voice_compression_mem.encodedDataBufQueue), VOB_ENCODED_DATA_STORAGE_BUF_SIZE, 
				(CQItemType *)(voice_compression_mem.encodedDataBuf));
	app_audio_mempool_get_buff(&(voice_compression_mem.tmpBufForEncoding),   
		TMP_BUF_FOR_ENCODING_DECODING_SIZE);
#endif


#if VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_OPUS
#if !defined(VAR_ARRAYS) && !defined(USE_ALLOCA) && defined(NONTHREADSAFE_PSEUDOSTACK)
	uint8_t* opus_voice_heap_ptr, *opus_voice_stackalloc_ptr;
#ifdef RECORD_LONELY
	app_capture_audio_mempool_get_buff(&opus_voice_heap_ptr,   
			VOICE_OPUS_HEAP_SIZE);
	app_capture_audio_mempool_get_buff(&opus_voice_stackalloc_ptr,	 
			VOICE_OPUS_STACKALLOC_SIZE);
#else
	app_audio_mempool_get_buff(&opus_voice_heap_ptr,   
			VOICE_OPUS_HEAP_SIZE);
	app_audio_mempool_get_buff(&opus_voice_stackalloc_ptr,	 
			VOICE_OPUS_STACKALLOC_SIZE);
#endif
#else
	uint8_t* opus_voice_heap_ptr;
#ifdef RECORD_LONELY
	app_capture_audio_mempool_get_buff(&opus_voice_heap_ptr,   
		VOICE_OPUS_HEAP_SIZE);
#else
	app_audio_mempool_get_buff(&opus_voice_heap_ptr,   
		VOICE_OPUS_HEAP_SIZE);
#endif
#endif


	VOICE_OPUS_CONFIG_T opusConfig = 
	{
		VOICE_OPUS_HEAP_SIZE,
		VOICE_OPUS_CHANNEL_COUNT,
		VOICE_OPUS_COMPLEXITY,
		VOICE_OPUS_PACKET_LOSS_PERC,
		VOICE_SIZE_PER_SAMPLE,
		VOICE_OPUS_APP,
		VOICE_OPUS_BANDWIDTH,
		VOICE_OPUS_BITRATE,
		VOICE_OPUS_SAMPLE_RATE,
		VOICE_SIGNAL_TYPE,
		VOICE_FRAME_PERIOD,
		
		VOICE_OPUS_USE_VBR,
		VOICE_OPUS_CONSTRAINT_USE_VBR,
		VOICE_OPUS_USE_INBANDFEC,
		VOICE_OPUS_USE_DTX,
	};
	
#if !defined(VAR_ARRAYS) && !defined(USE_ALLOCA) && defined(NONTHREADSAFE_PSEUDOSTACK)
	voice_opus_init(&opusConfig, opus_voice_heap_ptr, opus_voice_stackalloc_ptr);
#else
	voice_opus_init(&opusConfig, opus_voice_heap_ptr);
#endif


#elif VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_SBC
	static VOICE_SBC_CONFIG_T	sbcConfig =
	{
		VOICE_SBC_CHANNEL_COUNT 	,
		VOICE_SBC_CHANNEL_MODE		,
		VOICE_SBC_BIT_POOL			,
		VOICE_SBC_SIZE_PER_SAMPLE	,
		VOICE_SBC_SAMPLE_RATE		,
		VOICE_SBC_NUM_BLOCKS		,
		VOICE_SBC_NUM_SUB_BANDS 	,
		VOICE_SBC_MSBC_FLAG 		,
		VOICE_SBC_ALLOC_METHOD		,
	};

	voice_sbc_init(&sbcConfig);
#endif

}


void voice_compression_deinit()
{
#if VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_OPUS
	voice_opus_deinit();
#endif

}

uint32_t voice_compression_handle(uint8_t *input_buf, uint32_t sampleCount, uint8_t isReset)
{
	uint32_t outputSize = 0;
	uint8_t isneed_Reset=isReset;

#if VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_ADPCM
		outputSize = g726_Encode(input_buf,
				((char*)(voice_compression_mem.tmpBufForEncoding)), 
				sampleCount, 
				isneed_Reset);
		TRACE("adpcm outputs %d bytes", outputSize);		
#elif VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_OPUS
		uint32_t slices = 0;
		uint32_t generatedDataSize = 0;
		outputSize = 0;
		for (slices = 0;slices < VOB_VOICE_CAPTURE_FRAME_CNT;slices++)
		{
			generatedDataSize = voice_opus_encode(input_buf,
					(voice_compression_mem.tmpBufForEncoding + outputSize), 
					sampleCount, 
				isneed_Reset);
	
			isneed_Reset = false;
		 
			input_buf += VOB_VOICE_PCM_FRAME_SIZE;
			outputSize += generatedDataSize;
			osDelay(2);
		}
		TRACE("opus outputs %d bytes", outputSize);	
#elif VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_SBC
		uint32_t purchasedBytes;
		outputSize = voice_sbc_encode(input_buf, sampleCount*2, &purchasedBytes,
				voice_compression_mem.tmpBufForEncoding, 
				isneed_Reset);
		TRACE("SBC outputs %d bytes", outputSize);	
#else
		return sampleCount*2;
#endif

		//TRACE("encoded data in the buffer are %d bytes.", 
			//LengthOfCQueue(&(voice_compression_mem.encodedDataBufQueue)));
		EnCQueue(&(voice_compression_mem.encodedDataBufQueue), 
				voice_compression_mem.tmpBufForEncoding, 
				outputSize);
		return outputSize;

}

uint8_t voice_compression_validity_check()
{
	if (0 == AvailableOfCQueue(&(voice_compression_mem.encodedDataBufQueue))){
		return 1;
	}else{
		return 0;
	}
}

uint32_t voice_compression_get_encode_buf_size()
{
	return LengthOfCQueue(&(voice_compression_mem.encodedDataBufQueue));
}

void voice_compression_reset_encode_buf()
{
	ResetCqueue(&(voice_compression_mem.encodedDataBufQueue));
}

int voice_compression_get_encoded_data(uint8_t *out_buf,uint32_t len)
{
	app_pop_data_from_cqueue(&(voice_compression_mem.encodedDataBufQueue),out_buf,len); 
	return len;
}
