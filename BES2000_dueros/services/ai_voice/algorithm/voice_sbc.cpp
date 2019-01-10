extern "C" {
#include "sbc.h"
}
#include "voice_sbc.h"
#include "hal_trace.h"

static VOICE_SBC_CONFIG_T	voice_sbc_config =
{
    VOICE_SBC_CHANNEL_COUNT     ,
    VOICE_SBC_CHANNEL_MODE 	    ,
    VOICE_SBC_BIT_POOL	 	    ,
    VOICE_SBC_SIZE_PER_SAMPLE   ,
    VOICE_SBC_SAMPLE_RATE	    ,
    VOICE_SBC_NUM_BLOCKS	    ,
    VOICE_SBC_NUM_SUB_BANDS	    ,
    VOICE_SBC_MSBC_FLAG		    ,
    VOICE_SBC_ALLOC_METHOD	    ,
};

static int voice_sbc_init_encoder(void);

static SbcEncoder voice_sbc_encoder;
static SbcDecoder voice_sbc_decoder;

static float voice_sbc_eq_band_gain[8] = {1, 1, 1, 1, 1, 1, 1, 1};

static int voice_sbc_init_encoder(void)
{
    SBC_InitEncoder(&voice_sbc_encoder);
    voice_sbc_encoder.streamInfo.numChannels = voice_sbc_config.channelCnt;
    voice_sbc_encoder.streamInfo.channelMode = voice_sbc_config.channelMode;
    voice_sbc_encoder.streamInfo.bitPool     = voice_sbc_config.bitPool;
    voice_sbc_encoder.streamInfo.sampleFreq  = voice_sbc_config.sampleRate;
    voice_sbc_encoder.streamInfo.allocMethod = voice_sbc_config.allocMethod;
    voice_sbc_encoder.streamInfo.numBlocks   = voice_sbc_config.numBlocks;
    voice_sbc_encoder.streamInfo.numSubBands = voice_sbc_config.numSubBands;
    voice_sbc_encoder.streamInfo.mSbcFlag    = voice_sbc_config.mSbcFlag;
    return 0;
}

static int voice_sbc_init_decoder(void)
{
    SBC_InitDecoder(&voice_sbc_decoder);
    voice_sbc_decoder.streamInfo.numChannels = voice_sbc_config.channelCnt;
    voice_sbc_decoder.streamInfo.channelMode = voice_sbc_config.channelMode;
    voice_sbc_decoder.streamInfo.bitPool     = voice_sbc_config.bitPool;
    voice_sbc_decoder.streamInfo.sampleFreq  = voice_sbc_config.sampleRate;
    voice_sbc_decoder.streamInfo.allocMethod = voice_sbc_config.allocMethod;
    voice_sbc_decoder.streamInfo.numBlocks   = voice_sbc_config.numBlocks;
    voice_sbc_decoder.streamInfo.numSubBands = voice_sbc_config.numSubBands;
    voice_sbc_decoder.streamInfo.mSbcFlag    = voice_sbc_config.mSbcFlag;
    return 0;
}

uint32_t voice_sbc_encode(uint8_t *input, uint32_t inputBytes, uint32_t* purchasedBytes, uint8_t *output, uint8_t isReset)
{
	if (isReset)
	{
		voice_sbc_init_encoder();
	}
    SbcPcmData PcmEncData;

    PcmEncData.data = input;
    PcmEncData.dataLen = inputBytes;
    PcmEncData.numChannels = voice_sbc_encoder.streamInfo.numChannels;
    PcmEncData.sampleFreq = voice_sbc_encoder.streamInfo.sampleFreq;

    uint16_t outputSbcBytes, bytes_encoded;
    SBC_EncodeFrames(&voice_sbc_encoder, &PcmEncData, &bytes_encoded,
                                    output, &outputSbcBytes, 0xFFFF) ;

    TRACE("Encode %d bytes PCM data into %d bytes SBC data.", inputBytes, outputSbcBytes);
    TRACE("bytes_encoded is %d", bytes_encoded);
    *purchasedBytes = bytes_encoded;
	return outputSbcBytes;
}

uint32_t voice_sbc_decode(uint8_t* pcm_buffer, CQueue* encodedDataQueue, uint32_t expectedOutputSize, uint8_t isReset)
{
    if (isReset)
    {
        voice_sbc_init_decoder();
    }
    int r = 0;
    unsigned char *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;
    uint32_t sbcDataBytesToDecode;
    unsigned int pcm_offset = 0;
    uint16_t byte_decode;
    int8_t ret;
    SbcPcmData voice_PcmDecData;
    
get_again:    
    voice_PcmDecData.data = pcm_buffer+pcm_offset;
    voice_PcmDecData.dataLen = 0;
    
    if (LengthOfCQueue(encodedDataQueue) > VOICE_SBC_ENCODED_DATA_SIZE_PER_FRAME)
    {
        sbcDataBytesToDecode = VOICE_SBC_ENCODED_DATA_SIZE_PER_FRAME;
    }
    else
    {
        sbcDataBytesToDecode = LengthOfCQueue(encodedDataQueue);
    }
    
    len1 = len2 = 0;
    r = PeekCQueue(encodedDataQueue, sbcDataBytesToDecode, &e1, &len1, &e2, &len2);
    if ((r == CQ_ERR) || (0 == len1)) {
        goto exit;
    }   

    ret = SBC_DecodeFrames(&voice_sbc_decoder, (unsigned char *)e1, len1, &byte_decode,
                &voice_PcmDecData, expectedOutputSize-pcm_offset, voice_sbc_eq_band_gain);

    TRACE("len1 %d byte_decode %d expectedOutputSize %d pcm_offset %d",
        len1, byte_decode, expectedOutputSize, pcm_offset);
    TRACE("voice_PcmDecData.dataLen %d", voice_PcmDecData.dataLen);

    DeCQueue(encodedDataQueue, 0, byte_decode);
    
    pcm_offset += voice_PcmDecData.dataLen;
    
    if (expectedOutputSize == pcm_offset)
    {
        goto exit;
    }
    
    if ((ret == XA_STATUS_CONTINUE) || (ret == XA_STATUS_SUCCESS))  {
        goto get_again;
    }
    
exit:
    TRACE("Get pcm data size %d", pcm_offset);
    return pcm_offset;
}

int voice_sbc_init(VOICE_SBC_CONFIG_T* pConfig)
{
    voice_sbc_config = *pConfig;
    return 0;
}

