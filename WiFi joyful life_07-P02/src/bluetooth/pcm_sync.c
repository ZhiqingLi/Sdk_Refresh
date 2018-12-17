/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			pcm_sync.c
* Description:		PCM MODULE DRIVER FOR BLUETOOTH
*
*  maintainer: Aissen
*
* Change History:
*        Justin	- 05/30/2013 - V0.1
*					- created
******************************************************************************/

#include <string.h>
#include "type.h"
#include "app_config.h"
#include "pcm_sync.h"
#include "pcm_fifo.h"
#include "pcm_transfer.h"
#include "i2s.h"
#include "audio_path.h"
#include "audio_adc.h"
#include "dac.h"
#include "clk.h"
#include "gpio.h"
#include "blue_aec.h"
#include "rst.h"
#include "timeout.h"
#include "debug.h"
#include "bt_app_func.h"
#include "uart.h"
#include "eq.h"
#include "sys_vol.h"
#include "sample_rate_convert.h"
#include "sys_app.h"

#define BTHF_PCM_RECV_BUF_SIZE      (BLK_LEN*2)

#define BTHF_ADC_DATA_BUF_ADDR      (VMEM_ADDR)
#define BTHF_ADC_DATA_BUF_SIZE      (BLK_LEN*2)

#define BTHF_I2SIN_DATA_BUF_ADDR    (BTHF_ADC_DATA_BUF_ADDR + BTHF_ADC_DATA_BUF_SIZE)
#define BTHF_I2SIN_DATA_BUF_SIZE    (BLK_LEN*2) 

#define BTHF_SAMPLE_ADDR            (BTHF_I2SIN_DATA_BUF_ADDR + BTHF_I2SIN_DATA_BUF_SIZE)
#define BTHF_SAMPLE_SIZE            (1 * 1024)

#define BTHF_SAMPLE_RECV_ADDR       (BTHF_SAMPLE_ADDR + BTHF_SAMPLE_SIZE)
#define BTHF_SAMPLE_RECV_SIZE       (1 * 1024)

#define BTHF_UPSAMPLE_W_ADDR        (BTHF_SAMPLE_RECV_ADDR + BTHF_SAMPLE_RECV_SIZE)
#define BTHF_UPSAMPLE_W_SIZE        (1 * 1536)

#define BTHF_AEC_ADDR               (BTHF_UPSAMPLE_W_ADDR + BTHF_UPSAMPLE_W_SIZE)
#define BTHF_AEC_SIZE               (sizeof(BlueAECContext))



#define BTHF_AEC_SAVE_ADDR          (PMEM_ADDR + 10*1024)
#define BTHF_AEC_SAVE_SIZE          (1 * 1024)

#define BTHF_AEC_SEND_ADDR          (BTHF_AEC_SAVE_ADDR + BTHF_AEC_SAVE_SIZE)
#define BTHF_AEC_SEND_SIZE          (1 * 1024)

#define BTHF_PCM_FIFO_ADDR          (BTHF_AEC_SEND_ADDR + BTHF_AEC_SEND_SIZE)
#define BTHF_PCM_FIFO_SIZE          (4 * 1024)

#define BTHF_ADC_FIFO_ADDR          (BTHF_PCM_FIFO_ADDR + BTHF_PCM_FIFO_SIZE)
#define BTHF_ADC_FIFO_SIZE          (1 * 1024)

#define BTHF_I2S_FIFO_ADDR			(BTHF_ADC_FIFO_ADDR + BTHF_ADC_FIFO_SIZE)
#define BTHF_I2S_FIFO_SIZE			(1 * 1024)

uint8_t AdcDataBuffer[BTHF_ADC_DATA_BUF_SIZE]__ATTRIBUTE__(AT(BTHF_ADC_DATA_BUF_ADDR)); //adc buffer
uint8_t I2sInDataBuffer[BTHF_I2SIN_DATA_BUF_SIZE]__ATTRIBUTE__(AT(BTHF_I2SIN_DATA_BUF_ADDR)); //i2s in data in buffer
uint8_t *PcmOutDataBufer = (uint8_t *)BTHF_SAMPLE_ADDR;

#ifdef BT_HF_RECORD_FUNC
uint16_t  PcmRecvBuffer[BTHF_PCM_RECV_BUF_SIZE];
uint16_t* PcmRecvBufferWPtr = PcmRecvBuffer;
uint16_t* PcmRecvBufferRPtr = PcmRecvBuffer;
#endif

BlueAECContext* gBlueAecContext = (BlueAECContext*)(BTHF_AEC_ADDR); // for aec

static bool     PcmSyncInitFlag = FALSE;
#ifdef FUNC_BT_HF_UART
static uint8_t  SCOCount = 0;

static uint32_t PCMSample = 0;
static uint32_t AECSample = 0;
static uint32_t ScoSendSize = 0;

static uint8_t  *ScoSendTmpBuf = (uint8_t *)BTHF_AEC_SEND_ADDR;
static uint8_t  *ScoSaveTmpBuf = (uint8_t *)BTHF_AEC_SAVE_ADDR;
#endif

//
// AecStatus
//0:disable phone and mv aec;
//1:disable phone ace,enable mv ace;
//2:enable phone aec, disable mv aec
//3:enable phone aec, enable mv aec
//
static uint8_t AecStatus = 3;  //default value, app-callback 函数内部enable开关可以打开
void PcmSyncAecSetStatus(uint8_t Status)
{
	AecStatus = Status;
}

uint8_t PcmSyncAecGetStatus(void)
{
	return  AecStatus;
}
//void AecDebug(void)
//{
//	DBG("aec  current aec status\n");
//    DBG("# AecStatus aec%d #\n", AecStatus);
//}
//
//void Aec0Debug(void)
//{
//    if((AecStatus == 2) && PcmSyncInitFlag)
//    {
//        DBG("change from aec2 to aec0. fail\n");
//    }
//    else
//    {
//        DBG("aec0  disable phone and mv aec\n");
//        AecStatus = 0;
//    }
//
//}
//
//void Aec1Debug(void)
//{
//    if((AecStatus == 2) && PcmSyncInitFlag)
//    {
//        DBG("change from aec2 to aec1. fail\n");
//    }
//    else
//    {
//    	DBG("aec1  disable phone ace, enable mv ace\n");
//        AecStatus = 1;
//    }
//}
//
//void Aec2Debug(void)
//{
//
//    if((!PcmSyncInitFlag) || (AecStatus == 2))
//    {
//    	DBG("aec2  enable phone aec, disable mv aec\n");
//        AecStatus = 2;
//    }
//    else
//    {
//        DBG("change from aec%d to aec2. fail\n", AecStatus);
//    }
//}
//
//SHELL_CMD(
//	aec, 0, AecDebug,
//	"current aec status\n",
//	"Usage:\n"
//	"aec\n"
//);
//
//SHELL_CMD(
//	aec0, 0, Aec0Debug,
//	"disable phone aec,disable mv aec\n",
//	"Usage:\n"
//	"aec0\n"
//);
//
//SHELL_CMD(
//	aec1, 0, Aec1Debug,
//	"disable phone aec,enable mv aec\n",
//	"Usage:\n"
//	"aec1\n"
//);
//
//SHELL_CMD(
//	aec2, 0, Aec2Debug,
//	"enable phone aec,disable mv aec\n",
//	"Usage:\n"
//	"aec2\n"
//);

/////////Example: set phone call mic out digital gain, added by zhouyi @ 20140515/////////////
static int32_t I2SMicOutVolumGain = 0x100; /*x100 => 1.0*/

/////---API: One can change the gain online----///////////////////////////
void SetI2SMicOutVolumGain(int32_t Gain/*0x100 => 1.0*/)
{
    I2SMicOutVolumGain = Gain;
    
    if(I2SMicOutVolumGain > 32767)
    {
        I2SMicOutVolumGain = 32767;
    }
}

void DoI2SMicOutDigitalGain(int16_t *DataInOut, int32_t Samples, int32_t Gain)
{
    int32_t i;
    
    for(i = 0 ; i < Samples ; i ++)
    {
        DataInOut[i] = __ssat( (((int32_t)DataInOut[i]) * Gain) >> 8, 16);
    }
}

void PcmSyncPlayInit(void)
{
	memset(((uint8_t*)I2sInDataBuffer), 0, BLK_LEN * 6);

	NVIC_DisableIRQ(DECODER_IRQn);
	// Transfer PCM data to FIFO
	PcmTxInitWithDefaultPcmFifoSize(DATA_MODE_MONO_LOW);

	PcmFifoInit(BTHF_PCM_FIFO_ADDR - PMEM_ADDR, BTHF_PCM_FIFO_SIZE);

	PcmTxTransferData((void*)BTHF_SAMPLE_ADDR, (void*)BTHF_SAMPLE_ADDR, BLK_LEN);
}

void PcmSyncbyUartInit(void)
{
    PcmFifoClear();
    PhubPathClose(ALL_PATH_CLOSE);
	RstPhubModule();//reset 相关通路寄存器值，消除pop音，phub会复位寄存器，不要随便调用
    RstMclk();      //reset 相关通路寄存器值，消除pop音    
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE	
	if(gSys.OutputChannelSelect == CLASSD_OUTPUT_CHANNEL)
	{
		DacSoftMuteSet(TRUE, TRUE);
	    AdcClose();
	    #ifdef FUNC_I2S_MASTER_EN
	    I2sSetMasterMode(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_FORMAT_I2S);
	    #else
	    I2sSetSlaveMode(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_FORMAT_I2S);
	    #endif
	    AdcDacClkModeSet(NORMAL_MODE);
	    I2sSampleRateSet(32000, NORMAL_MODE); 
	    DacAdcSampleRateSet(8000,NORMAL_MODE);
	    Rst12p288GeneratorModule();
	    AdcConfig(NORMAL_MODE);
	    WaitMs(50); 
	    PhubPathSel(ADCIN2PMEM_PCMFIFOBEEP2_IISOUT);  
	    
	    SampleRateUpSampleby4Init((int16_t *)BTHF_UPSAMPLE_W_ADDR, BTHF_UPSAMPLE_W_SIZE / 2, 1);
	}
	else
	{
		CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R);
		DacSoftMuteSet(TRUE, TRUE);	
		DacConfig(MDAC_DATA_FROM_DEC, NORMAL_MODE);
		DacAdcSampleRateSet(8000, NORMAL_MODE);	 //九种采样
		AdcConfig(NORMAL_MODE);
		WaitMs(50); 
		PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT);
		DacSoftMuteSet(FALSE, FALSE);
	}
#elif defined(OUTPUT_CHANNEL_DAC)
	CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R);
	DacSoftMuteSet(TRUE, TRUE);	
	DacConfig(MDAC_DATA_FROM_DEC, USB_MODE);
	DacAdcSampleRateSet(8000, USB_MODE);	 //九种采样
	WaitMs(50); 
	PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT);
	DacSoftMuteSet(FALSE, FALSE);
    
#else//暂时不支持classd+dac同时输出  
    DacSoftMuteSet(TRUE, TRUE);
    AdcClose();
    #ifdef FUNC_I2S_MASTER_EN
    I2sSetMasterMode(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_FORMAT_I2S);
    #else
    I2sSetSlaveMode(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_FORMAT_I2S);
    #endif
    AdcDacClkModeSet(NORMAL_MODE);
    I2sSampleRateSet(32000, NORMAL_MODE); 
    DacAdcSampleRateSet(8000,NORMAL_MODE);
    Rst12p288GeneratorModule();
    AdcConfig(NORMAL_MODE);
    WaitMs(50); 
    PhubPathSel(ADCIN2PMEM_PCMFIFOBEEP2_IISOUT);  
    
    SampleRateUpSampleby4Init((int16_t *)BTHF_UPSAMPLE_W_ADDR, BTHF_UPSAMPLE_W_SIZE / 2, 1);
#endif    
}

void PcmSyncInit(uint8_t I2sClkType)
{
#ifdef FUNC_BT_HF_EN
	if(!PcmSyncInitFlag)
	{
		NVIC_DisableIRQ(DECODER_IRQn);
        CodecDacMuteSet(TRUE, TRUE);
        RstDecModule();
        PhubPathClose(ALL_PATH_CLOSE);
        #if (EQ_STYLE_MODE == 1) || (EQ_STYLE_MODE == 3)
        EqDisable();
        #endif

        CodecDacInit(1);
        DacEnable();       
        CodecAdcAnaInit();
        AdcConfig(USB_MODE);
        CodecAdcChannelSel(ADC_CH_MIC_L|ADC_CH_MIC_R);
        CodecAdcMicGainConfig(20,20); //模拟端调到最大 默认是20，0是最大
        AdcVolumeSet(0xff0, 0xff0);
        
#ifdef FUNC_BT_HF_UART
        AdcPmemWriteDis();
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE	
	AdcToPmem(NORMAL_MODE, (uint16_t)(BTHF_ADC_FIFO_ADDR - PMEM_ADDR), BTHF_ADC_FIFO_SIZE);
#else
    #ifdef OUTPUT_CHANNEL_DAC
        AdcToPmem(USB_MODE, (uint16_t)(BTHF_ADC_FIFO_ADDR - PMEM_ADDR), BTHF_ADC_FIFO_SIZE);
    #else
        AdcToPmem(NORMAL_MODE, (uint16_t)(BTHF_ADC_FIFO_ADDR - PMEM_ADDR), BTHF_ADC_FIFO_SIZE);
    #endif
#endif
        AdcPmemWriteEn();
        
        PcmSyncbyUartInit();
#else
        AdcPmemWriteDis();
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE	
		AdcToPmem(NORMAL_MODE, (uint16_t)(BTHF_ADC_FIFO_ADDR - PMEM_ADDR), BTHF_ADC_FIFO_SIZE);
#else
#ifdef OUTPUT_CHANNEL_DAC
		AdcToPmem(USB_MODE, (uint16_t)(BTHF_ADC_FIFO_ADDR - PMEM_ADDR), BTHF_ADC_FIFO_SIZE);
#else
		AdcToPmem(NORMAL_MODE, (uint16_t)(BTHF_ADC_FIFO_ADDR - PMEM_ADDR), BTHF_ADC_FIFO_SIZE);
#endif
#endif
        AdcPmemWriteEn();
        WaitMs(200);
        if(!PhubPathSel(ADCIN2PMEM_IISIN2DACOUT_PCMFIFO2IISOUT)) // config bluetooth mic path
		{
			return;
		}
        I2sWritePmemDis();
		I2sInToPmemConfig(BTHF_I2S_FIFO_ADDR - PMEM_ADDR, BTHF_I2S_FIFO_SIZE);
		I2sWritePmemEn();
        
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE			
		if(MV_MCU_AS_PCMSYNC_CLK_MASTER == I2sClkType)
		{
			I2sSampleRateSet(8000, NORMAL_MODE);
			I2sBluetoothMasterModeSet(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_DSP_MODEA);
		}
		else
		{
			I2sSampleRateSet(8000, NORMAL_MODE);
			I2sBluetoothSlaveModeSet(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_DSP_MODEA);
		}
#else
#ifdef OUTPUT_CHANNEL_DAC
        if(MV_MCU_AS_PCMSYNC_CLK_MASTER == I2sClkType)
		{
			I2sSampleRateSet(8000, USB_MODE);
			I2sBluetoothMasterModeSet(I2S_DATA_FROM_DEC, USB_MODE, I2S_DSP_MODEA);
		}
		else
		{
			I2sSampleRateSet(8000, USB_MODE);
			I2sBluetoothSlaveModeSet(I2S_DATA_FROM_DEC, USB_MODE, I2S_DSP_MODEA);
		}
#else
		if(MV_MCU_AS_PCMSYNC_CLK_MASTER == I2sClkType)
		{
			I2sSampleRateSet(8000, NORMAL_MODE);
			I2sBluetoothMasterModeSet(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_DSP_MODEA);
		}
		else
		{
			I2sSampleRateSet(8000, NORMAL_MODE);
			I2sBluetoothSlaveModeSet(I2S_DATA_FROM_DEC, NORMAL_MODE, I2S_DSP_MODEA);
		}
#endif
#endif
        
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE	
		DacConfig(MDAC_DATA_FROM_I2S, NORMAL_MODE);
#else
#ifdef OUTPUT_CHANNEL_DAC
		DacConfig(MDAC_DATA_FROM_I2S, USB_MODE);
#else
		DacConfig(MDAC_DATA_FROM_I2S, NORMAL_MODE);
#endif
#endif
        DacSoftMuteSet(TRUE, TRUE); 
        CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R); // select dac channel
        DacSoftMuteSet(TRUE, TRUE); //add by sam @2014-08-07，再次Mute，CodecDacChannelSel()函数会有Unmute操作
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE		
		DacAdcSampleRateSet(8000, NORMAL_MODE);
#else
#ifdef OUTPUT_CHANNEL_DAC		
		DacAdcSampleRateSet(8000, USB_MODE);
#else 
		DacAdcSampleRateSet(8000, NORMAL_MODE);
#endif
#endif
#endif
        DacVolumeSet(0x400,0x400);
        WaitMs(500);
        
#ifdef FUNC_BT_HF_UART
        PcmTxInitWithDefaultPcmFifoSize(DATA_MODE_MONO_S); 
        PcmFifoInitialize(BTHF_PCM_FIFO_ADDR - PMEM_ADDR,BTHF_PCM_FIFO_SIZE,0,0);
        memset((unsigned char *)BTHF_SAMPLE_ADDR,0,BTHF_SAMPLE_SIZE);
        PcmFifoClear();
        PcmTxTransferData((void*)(BTHF_SAMPLE_ADDR), (void*)(BTHF_SAMPLE_ADDR), 64);
        SCOCount = 0;
        PCMSample = 0;
        AECSample = 0;
        ScoSendSize = 0;
        
#else
        PcmSyncPlayInit();
#endif
        
        blue_aec_init(gBlueAecContext, NON_LINEAR_PROCESS, NOISE_SUPPRESSION_LEVEL);
       
        DacSoftMuteSet(FALSE, FALSE);
		SetI2SMicOutVolumGain(0x200);
	}

	PcmSyncInitFlag = TRUE;
#endif
}

//
//Get PcmSyncInit Flag
//
bool PcmSyncGetInitFlag(void)
{
	return PcmSyncInitFlag;
}


void RTL8761ScoProcessPCM(uint8_t *ScoBuffer)
{
#ifdef FUNC_BT_HF_UART
    uint8_t  ScoHanderBuf[4];
    memcpy(ScoHanderBuf, ScoBuffer, 4);
    memset(ScoBuffer, 0, 4);
    
    if(PcmSyncInitFlag)
    {
        //1、获取对方数据
        memcpy((unsigned char*)(BTHF_SAMPLE_RECV_ADDR) + SCOCount*48,ScoBuffer+4,48);
        
        SCOCount++;
        
        if((AecStatus == 3) || (AecStatus == 1))        
        {
            memcpy((void *)(ScoSaveTmpBuf + PCMSample * 2), ScoBuffer+4,48);
            PCMSample += 24;
        }
        
        
        //是否需要用MV AEC
        if((AecStatus == 0) || (AecStatus == 2))
        {
            if(AdcPmemPcmRemainLenGet() >= 24)
            {  
                AdcPcmDataRead(AdcDataBuffer, 24, 1);
                
#ifdef BT_HF_RECORD_FUNC   
                if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
                {
                    WriteDataToPcmBuffer((void *)(ScoBuffer + 4) ,AdcDataBuffer);
                }
#endif    
                memcpy(ScoBuffer, ScoHanderBuf, 4);
                memcpy((void *)(ScoBuffer+4), AdcDataBuffer, 48);
            }  
        }
        else if((AecStatus == 3) || (AecStatus == 1))
        {              
            if(PCMSample - AECSample >= 64)
            {   
                if(AdcPmemPcmRemainLenGet() >= 64)
                {
                    AdcPcmDataRead(AdcDataBuffer, 64, 1);
                    blue_aec_run(gBlueAecContext, (int16_t*)(ScoSaveTmpBuf + AECSample * 2), (int16_t*)AdcDataBuffer, (int16_t*)(ScoBuffer+4));

#ifdef BT_HF_RECORD_FUNC                    
                    if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
                    {  
                        WriteDataToPcmBuffer((void *)(ScoSaveTmpBuf + AECSample * 2), (void *)(ScoBuffer+4));
                    }
#endif
                    
                    if(ScoSendSize >= 512)
                    {
                        ScoSendSize = 0;
                    }
                    memcpy((void *)(ScoSendTmpBuf + ScoSendSize), (void *)(ScoBuffer+4), 128);
                    ScoSendSize += 128;
                    
                    AECSample += 64;
                }

            }
            
            if((PCMSample == AECSample) || (PCMSample >= 500))
            {
                PCMSample = 0;
                AECSample = 0;
            }
            
            if(ScoSendSize >= 48)
            {
                memcpy(ScoBuffer, ScoHanderBuf, 4);
                memcpy(&ScoBuffer[4], ScoSendTmpBuf,48);
                ScoSendSize -= 48;
                if(ScoSendSize > 0)
                {
                   memcpy(ScoSendTmpBuf, &ScoSendTmpBuf[48], ScoSendSize); 
                }
            } 
                
        }
        
        //发送
        if(SCOCount >= 5)
        {
            if(PcmTxIsTransferDone())
            {
                PcmTxDoneSignalClear();
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE
				if(gSys.OutputChannelSelect == CLASSD_OUTPUT_CHANNEL)
				{					
					SampleRateUpSampleby4((int16_t* )BTHF_SAMPLE_RECV_ADDR, (int16_t *)BTHF_SAMPLE_ADDR, 24 * 5);
					PcmTxTransferData((unsigned char*)(BTHF_SAMPLE_ADDR), (unsigned char*)(BTHF_SAMPLE_ADDR), 24*5*4);
				}
				else
				{
					PcmTxTransferData((unsigned char*)(BTHF_SAMPLE_RECV_ADDR), (unsigned char*)(BTHF_SAMPLE_RECV_ADDR), 24*5);				   				
				}
#elif defined(OUTPUT_CHANNEL_DAC)	
                PcmTxTransferData((unsigned char*)(BTHF_SAMPLE_RECV_ADDR), (unsigned char*)(BTHF_SAMPLE_RECV_ADDR), 24*5);                 
#elif defined(OUTPUT_CHANNEL_CLASSD)
                SampleRateUpSampleby4((int16_t* )BTHF_SAMPLE_RECV_ADDR, (int16_t *)BTHF_SAMPLE_ADDR, 24 * 5);
                PcmTxTransferData((unsigned char*)(BTHF_SAMPLE_ADDR), (unsigned char*)(BTHF_SAMPLE_ADDR), 24*5*4);
#endif
                memcpy((unsigned char *)BTHF_SAMPLE_RECV_ADDR, (unsigned char *)(BTHF_SAMPLE_RECV_ADDR + 5*48), (SCOCount-5)*48);
                if((AecStatus == 3) || (AecStatus == 1))
                { 
                    if(PCMSample > AECSample)
                    {
                        memcpy(ScoSaveTmpBuf, &ScoSaveTmpBuf[AECSample * 2], (PCMSample - AECSample) * 2);
                        PCMSample -=  AECSample;
                        AECSample = 0;
                    }
                }
                SCOCount -= 5;
                
            }
        }
        else if(SCOCount >= 10)
        {
            SCOCount = 0;
        }
    }

#endif    
}

//
//Process Pcm Data
//
void PcmSyncDataProcess(void)
{
#ifdef FUNC_BT_HF_UART
        return;
#else
    
    if(PcmSyncInitFlag)
	{

		if(PcmTxIsTransferDone())
		{
			uint32_t Size = 0;
			uint32_t i = 0, pos = 0;
			TIMER timer;
			TimeOutSet(&timer, 20);

			PcmTxDoneSignalClear();

			while((Size < BLK_LEN) && (!IsTimeOut(&timer)))
			{
				Size += AdcPcmDataRead(AdcDataBuffer + Size * 2, BLK_LEN - Size, 1);
			}

			Size = 0;
			TimeOutSet(&timer, 20);
			while((Size < BLK_LEN) && (!IsTimeOut(&timer)))
			{
				Size += I2sPcmDataRead(I2sInDataBuffer + Size * 2, BLK_LEN - Size, 1);
			}

			if((AecStatus == 3) || (AecStatus == 1))
			{
				blue_aec_run(gBlueAecContext, (int16_t*)I2sInDataBuffer, (int16_t*)AdcDataBuffer, (int16_t*)PcmOutDataBufer);
			}
			else
			{
				memcpy((void*)PcmOutDataBufer, AdcDataBuffer, BLK_LEN * 2);
			}

#ifdef BT_HF_RECORD_FUNC
			if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
			{
                WriteDataToPcmBuffer(I2sInDataBuffer, PcmOutDataBufer);
			}
#endif

			DoI2SMicOutDigitalGain((int16_t*)(PcmOutDataBufer), BLK_LEN, I2SMicOutVolumGain);
			
			pos = BTHF_I2SIN_DATA_BUF_SIZE - 2;
			
			for(i = BTHF_I2SIN_DATA_BUF_SIZE * 2 - 4; i > 0; i -= 4, pos -= 2)
			{
				*((uint16_t*)&PcmOutDataBufer[i]) = *((uint16_t*)&PcmOutDataBufer[pos]);
			}
			
			PcmTxTransferData((void*)PcmOutDataBufer, (void*)PcmOutDataBufer, BLK_LEN);
		}

	}
#endif
}

//
//Uninitialize I2s data in/out、adc
//
void PcmSyncUnInit(void)
{
	if(PcmSyncInitFlag)
	{
		CodecAdcAnaDeInit();
		AdcClose();
		DacVolumeSet(DAC_DIGITAL_VOL, DAC_DIGITAL_VOL);
		AdcVolumeSet(ADC_DIGITAL_VOL, ADC_DIGITAL_VOL);
        AdcPmemWriteDis();
        I2sWritePmemDis();

		NVIC_EnableIRQ(DECODER_IRQn);

		#if (EQ_STYLE_MODE == 1) || (EQ_STYLE_MODE == 3)
        EqEnable();
		#endif
		
		PcmSyncInitFlag = FALSE;
	}

}

#ifdef BT_HF_RECORD_FUNC
void InitBtHfRecordPcm(void)
{
	memset(PcmRecvBuffer, 0, BTHF_PCM_RECV_BUF_SIZE);
	PcmRecvBufferWPtr = PcmRecvBuffer;
	PcmRecvBufferRPtr = PcmRecvBuffer;
}

uint32_t GetAvailablePcmSyncDataSize(void)
{
	if(PcmRecvBufferWPtr >= PcmRecvBufferRPtr)
	{
		return (PcmRecvBufferWPtr - PcmRecvBufferRPtr);
	}
	else
	{
		return (BTHF_PCM_RECV_BUF_SIZE - (PcmRecvBufferRPtr - PcmRecvBufferWPtr));
	}
}

uint32_t GetAvailablePcmBufSize(void)
{
	if(PcmRecvBufferWPtr >= PcmRecvBufferRPtr)
	{
		return BTHF_PCM_RECV_BUF_SIZE - (PcmRecvBufferWPtr - PcmRecvBufferRPtr);
	}
	else
	{
		return (PcmRecvBufferRPtr - PcmRecvBufferWPtr);
	}
}


bool WriteDataToPcmBuffer(uint8_t *I2sData, uint8_t *PcmData)
{
	uint32_t	i;
    uint32_t    SampleDepth = 64;
    
    if((AecStatus == 3) || (AecStatus == 1))
    {
        SampleDepth = 64;
    }
    else
    {
        SampleDepth = 24;
    }

	if(GetAvailablePcmBufSize() >= SampleDepth)
	{
		for(i = 0; i < SampleDepth; i++)
		{
			*(PcmRecvBufferWPtr++) = __SSAT((int32_t)((int16_t*)I2sData)[i] + (int32_t)((int16_t*)PcmData)[i], 16);
            if(PcmRecvBufferWPtr > PcmRecvBuffer + BTHF_PCM_RECV_BUF_SIZE)
			{
				PcmRecvBufferWPtr = PcmRecvBuffer;
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint32_t ReadDataFromPcmBuffer(uint16_t* RecvBuf, uint32_t ReadLen)
{
	uint32_t		CpyLen = ReadLen;
	uint32_t		i;
	uint32_t		AvailableDataSize = 0;
	
	if(RecvBuf == NULL || ReadLen == 0)
		return 0;

	AvailableDataSize = GetAvailablePcmSyncDataSize();
	if(AvailableDataSize < ReadLen)
	{
		CpyLen = AvailableDataSize;
	}

	for(i = 0; i < CpyLen; i++)
	{
		*(RecvBuf++) = *(PcmRecvBufferRPtr++);
		if(PcmRecvBufferRPtr > PcmRecvBuffer + BTHF_PCM_RECV_BUF_SIZE)
			PcmRecvBufferRPtr = PcmRecvBuffer;
	}

	return CpyLen;
}
#endif
//
