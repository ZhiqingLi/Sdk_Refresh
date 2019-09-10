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
#include "dump.h"
#include "irqn.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "adc_key.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "rtos_api.h"
#include "sadc_interface.h"
#include "ff.h"

//定义2个全局buf，用于缓存ADC和DAC的数据，注意单位
uint32_t AudioADCBuf[1024] = {0}; //1024 * 4 = 4K
uint32_t AudioDACBuf[1024] = {0}; //1024 * 4 = 4K

uint8_t		gRecordFlg = FALSE;
uint8_t		gKeyValue = 0;

#define   GetKeyValue()              gKeyValue
#define   SetKeyValue(v)             gKeyValue = v

static uint32_t PcmBuf1[512] = {0};

static uint8_t DmaChannelMap[29] = {
	255,//PERIPHERAL_ID_SPIS_RX = 0,		//0
	255,//PERIPHERAL_ID_SPIS_TX,			//1
	255,//PERIPHERAL_ID_TIMER3,			//2
	4,//PERIPHERAL_ID_SDIO_RX,			//3
	5,//PERIPHERAL_ID_SDIO_TX,			//4
	255,//PERIPHERAL_ID_UART0_RX,			//5
	255,//PERIPHERAL_ID_TIMER1,				//6
	255,//PERIPHERAL_ID_TIMER2,				//7
	255,//PERIPHERAL_ID_SDPIF_RX,			//8 SPDIF_RX /TX需使用同一通道
	255,//PERIPHERAL_ID_SDPIF_TX,			//9
	255,//PERIPHERAL_ID_SPIM_RX,			//10
	255,//PERIPHERAL_ID_SPIM_TX,			//11
	255,//PERIPHERAL_ID_UART0_TX,			//12
	255,//PERIPHERAL_ID_UART1_RX,			//13
	255,//PERIPHERAL_ID_UART1_TX,			//14
	255,//PERIPHERAL_ID_TIMER4,				//15
	255,//PERIPHERAL_ID_TIMER5,				//16
	255,//PERIPHERAL_ID_TIMER6,				//17
	2,//PERIPHERAL_ID_AUDIO_ADC0_RX,		//18
	3,//PERIPHERAL_ID_AUDIO_ADC1_RX,		//19
	0,//PERIPHERAL_ID_AUDIO_DAC0_TX,		//20
	1,//PERIPHERAL_ID_AUDIO_DAC1_TX,		//21
	255,//PERIPHERAL_ID_I2S0_RX,			//22
	255,//PERIPHERAL_ID_I2S0_TX,			//23
	255,//PERIPHERAL_ID_I2S1_RX,			//24
	255,//PERIPHERAL_ID_I2S1_TX,			//25
	255,//PERIPHERAL_ID_PPWM,				//26
	255,//PERIPHERAL_ID_ADC,     			//27
	255,//PERIPHERAL_ID_SOFTWARE,			//28
};

//extern void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );
//
//void prvInitialiseHeap(void)
//{
//	extern char _end;
//	HeapRegion_t xHeapRegions[2];
//
//	xHeapRegions[0].pucStartAddress = (uint8_t*)&_end;
//	xHeapRegions[0].xSizeInBytes = gSramEndAddr-(uint32_t)&_end;
//
//	xHeapRegions[1].pucStartAddress = NULL;
//	xHeapRegions[1].xSizeInBytes = 0;
//
//	vPortDefineHeapRegions( (HeapRegion_t *)xHeapRegions );
//}


void ADCKeyScanProcess(void)
{
    AdcKeyMsg KeyMsg;

    KeyMsg = AdcKeyScan();

    if(KeyMsg.index != ADC_KEY_UNKNOW && KeyMsg.type != KEY_UNKOWN_TYPE)
    {
        if(KeyMsg.type == KEY_PRESSED || KeyMsg.type == KEY_LONG_PRESS_HOLD)
        {
            switch(KeyMsg.index)
            {
            case ADC_KEY_1:
                SetKeyValue('A');
                break;
            case ADC_KEY_2:
                SetKeyValue('a');
                break;
            case ADC_KEY_3:
                SetKeyValue('D');
                break;
            case ADC_KEY_4:
                SetKeyValue('d');
                break;
            case ADC_KEY_5:
                SetKeyValue('r');
                break;

            default:
                break;
            }
        }
    }
}

void KeyScanProcess(void)
{
    //adc key scan
    ADCKeyScanProcess();

    switch(gKeyValue)
    {
        case 'A':
//            AdcMicinGain = AdcMicinGain > 41? 42 : AdcMicinGain + 1;
//            CodecAdcLineInGainConfig(AdcMicinGain, AdcMicinGain);
//            CodecAdcMicGainConfig(AdcMicinGain, AdcMicinGain);
//            CodecAdcFmGainConfig(AdcMicinGain, AdcMicinGain);
//            DBG("[INFO]: ADC ANA MIC IN VOL- %d\n", AdcMicinGain);
            break;

        case 'a':
//            AdcMicinGain = AdcMicinGain < 1? 0 : AdcMicinGain - 1;
//            CodecAdcLineInGainConfig(AdcMicinGain, AdcMicinGain);
//            CodecAdcMicGainConfig(AdcMicinGain, AdcMicinGain);
//            CodecAdcFmGainConfig(AdcMicinGain, AdcMicinGain);
//            DBG("[INFO]: ADC ANA MIC IN VOL+ %d\n", AdcMicinGain);
            break;

        case 'D':
//            AdcDigGain = AdcDigGain < 0x10? 0 : AdcDigGain - 0x10;
//            AdcVolumeSet(AdcDigGain, AdcDigGain);
//            DBG("[INFO]: ADC Digital VOL- %d\n", AdcDigGain);
            break;

        case 'd':
//            AdcDigGain = AdcDigGain > 0xFEF? 0xFFF : AdcDigGain + 0x10;
//            AdcVolumeSet(AdcDigGain, AdcDigGain);
//            DBG("[INFO]: ADC Digital VOL+ %d\n", AdcDigGain);
            break;

        case 'r':
            gRecordFlg = 1 - gRecordFlg;
            if(gRecordFlg)
            {
                DBG("[INFO]: Start record\n");
            }
            else
            {
                DBG("\n[INFO]: stop record\n");
            }
            //TimeOutSet(&SaveTimer, 1000);
            break;

        default:

            break;
    }
    SetKeyValue(0);
}

static void AudioMicTask(void)
{
  uint32_t  SampleRate = 16000;
  uint16_t  RealLen;
  uint16_t  n;


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
	AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1);

	AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2, 0, 4);
	AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1, 0, 4);

//	//Mic1 Mic2  digital
	AudioADC_DigitalInit(ADC1_MODULE, SampleRate, (void*)AudioADCBuf, sizeof(AudioADCBuf));

	while(1)
	{
		//KeyScanProcess();
		uint8_t c = 0x00;
		if(UART1_RecvByte(&c))
		{
			if(c == 'r')
			{
				gRecordFlg = 1 - gRecordFlg;
				if(gRecordFlg)
				{
						DBG("[INFO]: Start record\n");
				}
				else
				{
						DBG("\n[INFO]: stop record\n");
				}
			}
		}

		if(AudioADC_DataLenGet(ADC1_MODULE) >= 512)
		{
			n = AudioDAC_DataSpaceLenGet(DAC0);
			RealLen = AudioADC_DataGet(ADC1_MODULE, PcmBuf1, 512);

			if(n > RealLen)
			{
				n = RealLen;
			}
			if(gRecordFlg == TRUE)
			{
				DumpBufferPush((uint8_t*)PcmBuf1, n * 4);//立体声，乘以4（单位Byte）
			}
			AudioDAC_DataSet(DAC0, PcmBuf1, n);
		}
		else
		{
			//DBG("-");
			vTaskDelay(1);
		}
	}
}

void DumpTask(void)
{
	//DBG("DumpTask...\n");
	bool  ret;

  ret = SdInitAndFsMount();

  if(ret == TRUE)
  {
  	ret = OpenDumpDataFile();
  }
  gRecordFlg = ret;

	while(1)
	{
		//DBG("*");
		if(gRecordFlg)
		{
			//DBG("*");
			if(0 == DumpBufferPopByBlock())
			{
				vTaskDelay(2);
			}
		}
		else
		{
			CloseDumpDataFile();
			vTaskDelay(10);
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

	//SarADC_Init();
	//AdcKeyInit();

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
	//DBG("AAA\n");
	xTaskCreate( (TaskFunction_t)AudioMicTask, "AudioMicTask", 512, NULL, 2, NULL );
	xTaskCreate( (TaskFunction_t)DumpTask, "DumpTask", 512, NULL, 1, NULL );

	//DBG("AAA\n");
	vTaskStartScheduler();

	while(1);
}
