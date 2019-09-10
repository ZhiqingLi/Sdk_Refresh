#include <stdlib.h>
#include <nds32_intrinsic.h>
#include "type.h"
#include "debug.h"
#include "dma.h"
#include "dac.h"
#include "audio_adc.h"
#include "adc_interface.h"
#include "dac_interface.h"

//#define AUDIO_FIFO_ADDR_BASE	0x20020000
//
//#define ASDM0_FIFO_START_ADDR 	(AUDIO_FIFO_ADDR_BASE) //0x2000
//#define ASDM0_FIFO_LEN		 	(1024 * 8)
//
//#define ASDM1_FIFO_START_ADDR 	(ASDM0_FIFO_START_ADDR + ASDM0_FIFO_LEN)
//#define ASDM1_FIFO_LEN		 	(1024 * 8)//(1024 * 8)
//
//#define DAC_FIFO_START_ADDR 	(ASDM1_FIFO_START_ADDR + ASDM1_FIFO_LEN)
//#define DAC_FIFO_LEN		 	(1024 * 8)
//
//#define I2S0_TX_FIFO_START_ADDR (DAC_FIFO_START_ADDR + DAC_FIFO_LEN)
//#define	I2S0_TX_FIFO_LEN        (1024 * 8)
//
//#define I2S0_RX_FIFO_START_ADDR (I2S0_TX_FIFO_START_ADDR + I2S0_TX_FIFO_LEN)
//#define	I2S0_RX_FIFO_LEN        (1024 * 8)
uint32_t AudioADC0Buf[1024] = {0}; //1024 * 4 = 4K
uint32_t AudioADC1Buf[1024] = {0}; //1024 * 4 = 4K
uint32_t AudioDACBuf[1024] = {0}; //1024 * 4 = 4K

void UsbAudioMicDacInit()
{
    uint32_t  SampleRate = 44100;


    //DAC init
	AudioDAC_Init(DAC0, SampleRate, (void*)AudioDACBuf, sizeof(AudioDACBuf), NULL, 0);

	//Mic1 Mic2  analog
	AudioADC_AnaInit();

	//AudioADC_MicBias2Enable(TRUE);
	AudioADC_VcomConfig(1);//MicBias en
	//模拟通道先配置为NONE，防止上次配置通道残留，然后再配置需要的模拟通道

	AudioADC_DynamicElementMatch(ADC1_MODULE, TRUE, TRUE);
	AudioADC_MicBias1Enable(TRUE);

	AudioADC_PGASel(ADC1_MODULE, CHANNEL_RIGHT, LINEIN_NONE);
	AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN_NONE);

	AudioADC_PGASel(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2);
	AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1);

	AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2, 15, 4);
	AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1, 15, 4);

	//Mic1 Mic2  digital
	AudioADC_DigitalInit(ADC1_MODULE, SampleRate, (void*)AudioADC0Buf, sizeof(AudioADC0Buf));
}

void UsbAudioLineInDacInit()
{
    uint32_t  SampleRate = 44100;

    //DAC init
    AudioDAC_Init(DAC0, SampleRate, (void*)AudioDACBuf, sizeof(AudioDACBuf), NULL, 0);

	//LineIn1  analog
	AudioADC_AnaInit();
	//模拟通道先配置为NONE，防止上次配置通道残留，然后再配置需要的模拟通道
	AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN_NONE);
	AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN_NONE);

	AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN1_LEFT);
	AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN1_RIGHT);

	AudioADC_PGAGainSet(ADC0_MODULE, CHANNEL_RIGHT, LINEIN1_LEFT, 0, 4);
	AudioADC_PGAGainSet(ADC0_MODULE, CHANNEL_LEFT, LINEIN1_RIGHT, 0, 4);

	//LineIn1  digital
	AudioADC_DigitalInit(ADC0_MODULE, SampleRate, (void*)AudioADC1Buf, sizeof(AudioADC1Buf));
}


void AudioADC0_DMARestart()
{
	AudioADC_DMARestart(ADC0_MODULE, (void*)AudioADC0Buf, sizeof(AudioADC0Buf));
}

void AudioADC1_DMARestart()
{
	AudioADC_DMARestart(ADC1_MODULE, (void*)AudioADC1Buf, sizeof(AudioADC1Buf));
}
