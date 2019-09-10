/**
 **************************************************************************************
 * @file    voice_app_demo.c
 * @brief
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-10-30 11:30:00$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <nds32_intrinsic.h>
//#include <string.h>
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "timeout.h"
#include "clk.h"
#include "dma.h"
#include "dac.h"
#include "timer.h"
#include "adc.h"
#include "i2s.h"
#include "watchdog.h"
#include "spi_flash.h"
#include "remap.h"
#include "audio_adc.h"
#include "gpio.h"
#include "chip_info.h"
#include "irqn.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "rtos_api.h"
#include "ff.h"
#include "mvstdio.h"
#include "sd_card.h"
#include "sbcenc_api.h"

#define ADC_MIC_BLOCK 256 //samples

//定义2个全局buf，用于缓存ADC和DAC的数据，注意单位
uint32_t AudioADCBuf[1024] = {0}; //1024 * 4 = 4K
uint32_t AudioDACBuf[1024] = {0}; //1024 * 4 = 4K

uint8_t		gRecordFlg = FALSE;
uint8_t		gKeyValue = 0;

static int16_t PcmBuf1[ADC_MIC_BLOCK*2] = {0};
static int16_t FileIOBuf[ADC_MIC_BLOCK*2] = {0};
static uint8_t data[1024] = {0};
uint8_t pcm_buf_raw[4095];
MemHandle pcm_buffer;

FATFS 	gFatfs_sd;   				/* File system object */
FIL 	gFil;

static SBCEncoderContext msbc_ct;

static uint8_t DmaChannelMap[28] = {
	255,//PERIPHERAL_ID_SPIS_RX = 0,		//0
	255,//PERIPHERAL_ID_SPIS_TX,			//1
	255,//PERIPHERAL_ID_TIMER3,				//2
	255,//PERIPHERAL_ID_SDIO0_RX,			//3
	255,//PERIPHERAL_ID_SDIO0_TX,			//4
	255,//PERIPHERAL_ID_UART0_RX,			//5
	4,//PERIPHERAL_ID_SDIO1_RX,			//6
	5,//PERIPHERAL_ID_SDIO1_TX,			//7
	255,//PERIPHERAL_ID_SDPIF_RX,			//8 SPDIF_RX /TX需使用同一通道
	255,//PERIPHERAL_ID_SDPIF_TX,			//9
	255,//PERIPHERAL_ID_SPIM_RX,			//10
	255,//PERIPHERAL_ID_SPIM_TX,			//11
	255,//PERIPHERAL_ID_UART0_TX,			//12
	255,//PERIPHERAL_ID_UART1_RX,			//13
	255,//PERIPHERAL_ID_UART1_TX,			//14
	255,//PERIPHERAL_ID_PSRAM_CONTROL_TX,	//15
	255,//PERIPHERAL_ID_PSRAM_CONTROL_RX,	//16
	255,//PERIPHERAL_ID_TIMER4,				//17
	1,//PERIPHERAL_ID_AUDIO_ADC0_RX,		//18
	2,//PERIPHERAL_ID_AUDIO_ADC1_RX,		//19
	3,//PERIPHERAL_ID_AUDIO_DAC0_TX,		//20
	4,//PERIPHERAL_ID_AUDIO_DAC1_TX,		//21
	255,//PERIPHERAL_ID_I2S0_TX,				//22
	255,//PERIPHERAL_ID_I2S0_RX,				//23
	255,//PERIPHERAL_ID_I2S1_TX,				//24
	255,//PERIPHERAL_ID_I2S1_RX,				//25
	255,//PERIPHERAL_ID_ADC,     			//26
	255,//PERIPHERAL_ID_SOFTWARE,			//27
};

bool SdInitAndFsMount(void)
{
    uint16_t  Cnt = 2;//上电的时候SD卡检测2次

    bool IsFsMounted = FALSE;

	//SDIO config
	GPIO_PortAModeSet(GPIOA15, 1);
	GPIO_PortAModeSet(GPIOA16, 2);
	GPIO_PortAModeSet(GPIOA17, 1);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIO_INDEX15);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIO_INDEX15);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIO_INDEX16);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIO_INDEX16);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIO_INDEX17);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIO_INDEX17);

	while(Cnt--)
	{
		if(SDCard_Init() == NONE_ERR)
		{
			DBG("SDCard Init Success!\n");
			break;
		}
		else
		{
			DBG("SdInit Failed!\n");
			goto end;
		}
	}


	DBG("rb: %d\n", SDCard_ReadBlock(0, (uint8_t*)AudioADCBuf, 1));
	DBG("%X:%X\n", AudioADCBuf[0], AudioADCBuf[1]);


	if(f_mount(&gFatfs_sd, "0:/", 1) == 0)
	{
		DBG("SD卡挂载到 0:/--> 成功\n");
	}
	else
	{
		DBG("SD卡挂载到 0:/--> 失败\n");
		goto end;
	}

	IsFsMounted = TRUE;

end:
    return IsFsMounted;
}

static void AudioMicTask(void)
{
  uint32_t  SampleRate = 16000;

  uint16_t  i;

  //DAC init
  AudioDAC_Init(DAC0, SampleRate, (void*)AudioDACBuf, sizeof(AudioDACBuf), NULL, 0);

//	//Mic1 Mic2  analog
	AudioADC_AnaInit();

	//AudioADC_MicBias1Enable(TRUE);
	//AudioADC_MicBias2Enable(TRUE);
	AudioADC_VcomConfig(1);//MicBias en
	//模拟通道先配置为NONE，防止上次配置通道残留，然后再配置需要的模拟通道
	AudioADC_MicBias1Enable(TRUE);

	AudioADC_PGASel(ADC1_MODULE, CHANNEL_RIGHT, LINEIN_NONE);
	AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN_NONE);

	AudioADC_PGASel(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2);
	AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_RIGHT_OR_MIC2);

	AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2, 0, 4);
	AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_RIGHT_OR_MIC2, 0, 4);

//	//Mic1 Mic2  digital
	AudioADC_DigitalInit(ADC1_MODULE, SampleRate, (void*)AudioADCBuf, sizeof(AudioADCBuf));

	while(1)
	{
		if(AudioADC_DataLenGet(ADC1_MODULE) >= ADC_MIC_BLOCK)
		{
			AudioADC_DataGet(ADC1_MODULE, PcmBuf1, ADC_MIC_BLOCK);
			AudioDAC_DataSet(DAC0, PcmBuf1, ADC_MIC_BLOCK);

			if(gRecordFlg)
			{
				for(i=1; i<ADC_MIC_BLOCK; i++)
				{
					PcmBuf1[i] = PcmBuf1[i*2];
				}

				if(mv_mremain(&pcm_buffer) > ADC_MIC_BLOCK*2)
				{
					mv_mwrite(PcmBuf1, 1, ADC_MIC_BLOCK*2, &pcm_buffer);
				}
			}//write buffer
			else
			{
				//reset buffer
				if(mv_msize(&pcm_buffer) != 0)
				{
					pcm_buffer.mem_len = 0;
					pcm_buffer.p = 0;
				}
			}
		}
		else
		{
			vTaskDelay(2);
		}
	}
}

void DumpTask(void)
{
	uint8_t c = 0x00;
	int32_t spf, ret;
	uint32_t msbclen;
	UINT br;

	if(SdInitAndFsMount())
	{
		ret = sbc_encoder_initialize(&msbc_ct, 1, 16000, 15, SBC_ENC_QUALITY_HIGH, &spf);
		DBG("msbc: %d, %d\n", ret, spf);

		while(1)
		{
			if(gRecordFlg)
			{
				if(mv_msize(&pcm_buffer) >= spf*2)
				{
					mv_mread(FileIOBuf, 1, spf*2, &pcm_buffer);
					sbc_encoder_encode(&msbc_ct, FileIOBuf, data, &msbclen);
					f_write(&gFil, data, msbclen, &br);
					//DBG("%d\n", msbclen);
				}
			}//save file


			if(UART1_RecvByte(&c))
			{
				if(c == 'r')
				{
					gRecordFlg = !gRecordFlg;
					if(gRecordFlg)
					{
						if(f_open(&gFil, "0:/aaa.sbc", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
						{
							DBG("Open file error!\n");
							gRecordFlg = FALSE;
						}
						else
						{
							DBG("Open File ok, start rec\n");
						}
					}
					else
					{
						f_close(&gFil);
						DBG("\n[INFO]: stop record\n");
					}
				}
			}//uart

		}//while(1)
	}
	else
	{
		DBG("SD card error!\n");
		while(1)
		{
			vTaskDelay(1);
		}
	}
}

//语音处理演示Demo
//需要fatfs，RTOS中间件
int main(void)
{
	Chip_Init(1);
	WDG_Disable();

	Clock_Module1Enable(ALL_MODULE1_CLK_SWITCH);
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	Clock_Module3Enable(ALL_MODULE3_CLK_SWITCH);

	Clock_Config(1, 24000000);
	Clock_PllLock(288000);
	Clock_APllLock(288000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(PLL_CLK_MODE);

	GPIO_PortAModeSet(GPIOA24, 1);//Rx, A24:uart1_rxd_0
	GPIO_PortAModeSet(GPIOA25, 6);//Tx, A25:uart1_txd_0
	DbgUartInit(1, 256000, 8, 0, 1);

	Remap_InitTcm(0, 12);
	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	DMA_ChannelAllocTableSet(DmaChannelMap);

	prvInitialiseHeap();
  osSemaphoreMutexCreate();

	DBG("\n");
  DBG("/-----------------------------------------------------\\\n");
  DBG("|                     AudioADC Example                     |\n");
  DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
  DBG("\\-----------------------------------------------------/\n");
  DBG("\n");

	NVIC_EnableIRQ(SWI_IRQn);
	GIE_ENABLE();	//开启总中断


	mv_mopen(&pcm_buffer, pcm_buf_raw, sizeof(pcm_buf_raw)-4, NULL);

	xTaskCreate( (TaskFunction_t)AudioMicTask, "AudioMicTask", 512, NULL, 2, NULL );
	xTaskCreate( (TaskFunction_t)DumpTask, "DumpTask", 1024, NULL, 1, NULL );
	vTaskStartScheduler();

	while(1);
}
