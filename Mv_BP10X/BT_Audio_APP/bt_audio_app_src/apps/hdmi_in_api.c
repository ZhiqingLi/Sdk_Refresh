/**
 **************************************************************************************
 * @file    hdmi_in_API.c
 * @brief
 *
 * @author  Cecilia Wang
 * @version V1.0.0
 *
 * $Created: 2018-03-23 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdlib.h>
#include "string.h"
#include <nds32_intrinsic.h>
#include "app_config.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "timeout.h"
#include "clk.h"
#include "dma.h"
#include "timer.h"
#include "watchdog.h"
#include "spi_flash.h"
#include "remap.h"
#include "gpio.h"
#include "chip_info.h"
#include "irqn.h"
#include "dac.h"
#include "dac_interface.h"
#include "hdmi_in_api.h"
#include "i2c.h"
#include "i2c_interface.h"
#include "app_config.h"
#include "mcu_circular_buf.h"
#include "audio_vol.h"
#include "main_task.h"
#include "delay.h"
#include "cec.h"
#include "reset.h"

#ifdef  CFG_APP_HDMIIN_MODE_EN
#ifdef DDC_USE_SW_I2C
static void* DDCI2cHandler = NULL;
#endif

#define HDMI_CEC_TX_FIFO_LEN				(1024 * 4)
#define HDMI_CEC_RX_FIFO_LEN				(1024)

extern bool    gIsVolSetEnable;
extern int32_t SetChannel ;

HDMIInfo  			 *gHdmiCt   = NULL;
uint32_t        	 *gCECRxFIFO;
uint32_t       	     *gCECTxFIFO;

MCU_CIRCULAR_CONTEXT *hdmiMCUCt = NULL;
CECInitTypeDef       *gCecInitDef = NULL;
const uint8_t    	  MVDDCHeader[8] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

uint8_t HDMI_DDC_GetInfo(void);

void HDMI_CEC_DDC_Init(void)
{

	if(gHdmiCt)   		osPortFree(gHdmiCt);
	gHdmiCt = (HDMIInfo *)osPortMalloc(sizeof(HDMIInfo));
	if(gHdmiCt == NULL)	return;
	memset(gHdmiCt, 0, sizeof(HDMIInfo));

	if(gCECRxFIFO )		osPortFree(gCECRxFIFO);
	gCECRxFIFO = (uint32_t *)osPortMalloc(HDMI_CEC_RX_FIFO_LEN);
	if(gCECRxFIFO== NULL) return;
	memset(gCECRxFIFO, 0, HDMI_CEC_RX_FIFO_LEN);

	if(gCECTxFIFO)		osPortFree(gCECTxFIFO);
	gCECTxFIFO = (uint32_t *)osPortMalloc(HDMI_CEC_TX_FIFO_LEN);
	if(gCECTxFIFO== NULL) return;
	memset(gCECTxFIFO, 0, HDMI_CEC_TX_FIFO_LEN);

	if(gCecInitDef)		osPortFree(gCecInitDef);
	gCecInitDef = (CECInitTypeDef *)osPortMalloc(sizeof(CECInitTypeDef));
	if(gCecInitDef== NULL) return;
	memset(gCecInitDef, 0, sizeof(CECInitTypeDef));


	//交换与RX初始化的顺序，使其第一次发送没有毛刺
	gCecInitDef->pwm_io							=  HDMI_CEC_SEND_IO;
	gCecInitDef->pwm_io_pin						=  HDMI_CEC_SEND_IO_PIN;
	gCecInitDef->pwm_timer_id					=  HDMI_CEC_SEND_TIMER_ID;
	gCecInitDef->dma_tx_id						=  HDMI_CEC_SEND_DMA_ID;
	gCecInitDef->pwm_tx_fifo					=  (uint16_t *)gCECTxFIFO;
	gCecInitDef->dma_tx_param.Dir  				=  DMA_CHANNEL_DIR_MEM2PERI;
	gCecInitDef->dma_tx_param.Mode 				=  DMA_BLOCK_MODE;
	gCecInitDef->dma_tx_param.ThresholdLen 		=  0;
	gCecInitDef->dma_tx_param.SrcDataWidth 		=  DMA_SRC_DWIDTH_HALF_WORD;
	gCecInitDef->dma_tx_param.DstDataWidth 		=  DMA_DST_DWIDTH_HALF_WORD;
	gCecInitDef->dma_tx_param.SrcAddrIncremental=  DMA_SRC_AINCR_SRC_WIDTH;
	gCecInitDef->dma_tx_param.DstAddrIncremental=  DMA_DST_AINCR_NO;
	gCecInitDef->dma_tx_param.DstAddress        =  HDMI_CEC_SEND_DATA_ADDR;
	gCecInitDef->dma_tx_param.BufferLen         =  0;
	gCecInitDef->pwm_param.CounterMode   		=  PWM_COUNTER_MODE_UP;
	gCecInitDef->pwm_param.OutputType    		=  PWM_OUTPUT_SINGLE_1;

	if(HDMI_CEC_CLK_MODE == RC_CLK_MODE)
	{
		uint32_t tmp_clk = Clock_RcFreqGet(0);
		gCecInitDef->pwm_param.FreqDiv 			= (uint32_t)((120.0 / 10) * tmp_clk / (120 * 1000 * 1000 / 10) * 100 +0.5);
	}
	else
	{
		gCecInitDef->pwm_param.FreqDiv 			= 14400;
	}
	gCecInitDef->pwm_param.Duty          		=  100;
	gCecInitDef->pwm_param.DMAReqEnable			=  1;

	//gCecInitDef->pwc_io							=   HDMI_CEC_RECV_IO;
	//gCecInitDef->pwc_io_pin						=   HDMI_CEC_RECV_IO_PIN;
	gCecInitDef->pwc_timer_id					=   HDMI_CEC_RECV_TIMER_ID;
	gCecInitDef->dma_rx_id						=   HDMI_CEC_RECV_DMA_ID;
	gCecInitDef->dma_rx_param.Dir				=	DMA_CHANNEL_DIR_PERI2MEM;
	gCecInitDef->dma_rx_param.Mode  			=   DMA_CIRCULAR_MODE;
	gCecInitDef->dma_rx_param.ThresholdLen		=   2;//TaoWen; for rc clk
	gCecInitDef->dma_rx_param.SrcDataWidth   	=   DMA_SRC_DWIDTH_HALF_WORD;
	gCecInitDef->dma_rx_param.DstDataWidth   	=   DMA_SRC_DWIDTH_HALF_WORD;
	gCecInitDef->dma_rx_param.SrcAddrIncremental=   DMA_SRC_AINCR_NO;
	gCecInitDef->dma_rx_param.DstAddrIncremental=   DMA_DST_AINCR_DST_WIDTH;
	gCecInitDef->dma_rx_param.SrcAddress		=	HDMI_CEC_RECV_DATA_ADDR;
	gCecInitDef->dma_rx_param.DstAddress		=	(uint32_t)gCECRxFIFO;
	gCecInitDef->dma_rx_param.BufferLen			=	HDMI_CEC_RX_FIFO_LEN;
	gCecInitDef->pwc_param.Polarity     		=   PWC_POLARITY_BOTH;
	gCecInitDef->pwc_param.SingleGet   			=   0;
	gCecInitDef->pwc_param.DMAReqEnable 		=   1;
	gCecInitDef->pwc_param.PwcSourceSelect		=   0;
	gCecInitDef->pwc_param.PwcOpt				=	1;

	if(HDMI_CEC_CLK_MODE == RC_CLK_MODE)//RC Clk 精度/使用10us
	{
		gCecInitDef->pwc_param.TimeScale 		=  (uint32_t)((120.0 / 10) * Clock_RcFreqGet(0) / (120 * 1000 * 1000 / 10) * 10 + 0.5) ;//RC时钟唤醒使用：10us步长
	}
	else
	{
		gCecInitDef->pwc_param.TimeScale 		=  1440;//10us
	}

	gCecInitDef->pwc_param.FilterTime   		=   12;

	gCecInitDef->cec_io_type					=  HDMI_CEC_IO_TYPE;
	gCecInitDef->cec_io_index					=  HDMI_CEC_IO_INDEX;
	gCecInitDef->hpd_status_io   	     		=  HDMI_HPD_CHECK_STATUS_IO;
	gCecInitDef->hpd_status_io_pin				=  HDMI_HPD_CHECK_STATUS_IO_PIN;
	gCecInitDef->arbitration_timer_id			=  HDMI_CEC_ARBITRATION_TIMER_ID;
	gCecInitDef->arbitration_timer_irq			=  HDMI_CEC_ARBITRATION_TIMER_IRQ;
	memset(&gCecInitDef->cec_info, 0, sizeof(CECInfo));
	HDMI_CEC_Init(gCecInitDef);

	HDMI_HPD_CHECK_IO_INIT();

	#ifdef DDC_USE_SW_I2C
	GPIO_PortAModeSet((1 << SclIndex), 0x0);
	GPIO_PortAModeSet((1 << SdaIndex), 0x0);
	if(!DDCI2cHandler)
	{
		DDCI2cHandler = I2cMasterCreate(DDCSclPortSel, SclIndex, DDCSdaPortSel, SdaIndex);
		if(!DDCI2cHandler)
		{
			DBG("DDC I2C CREATE FAIL,PLS CHECK\n");
		}
	}
	#else
	HDMI_DDC_DATA_IO_INIT();
	HDMI_DDC_CLK_IO_INIT();
	I2C_Init(0x3F, HDMI_DDC_IO_PIN, 0xA0);
	#endif

	gHdmiCt->hdmi_poweron_flag = 0;
	gHdmiCt->hdmiReportStatus = 0;
	gHdmiCt->hdmi_arc_flag = 0;
	gHdmiCt->hdmiReportARCStatus = 0;

	//HPD检测到高电平之后再读取EDID信息
	gHdmiCt->hdmiHPDCheck = GPIO_RegOneBitGet(HDMI_HPD_CHECK_STATUS_IO, HDMI_HPD_CHECK_STATUS_IO_PIN);
	if(gHdmiCt->hdmiHPDCheck)
	{
		WaitMs(5);//确保HPD的高电平状态
		gHdmiCt->hdmiHPDCheck = GPIO_RegOneBitGet(HDMI_HPD_CHECK_STATUS_IO, HDMI_HPD_CHECK_STATUS_IO_PIN);
		if(gHdmiCt->hdmiHPDCheck)
		{
			if(HDMI_DDC_GetInfo())//更新为读取所有的DDC信息
			{
				gHdmiCt->hdmiReportStatus = 1;
			}
		}
	}
}

//进入standby装填
void HDMI_CEC_StandbyStatus(void)
{
//	Reset_McuSystem();
#ifdef CFG_PARA_WAKEUP_SOURCE_CEC
	MessageContext		msgSend;
	msgSend.msgId		= MSG_DEVICE_SERVICE_DEEPSLEEP;
	MessageSend(mainAppCt.msgHandle, &msgSend);
#endif

}

void HDMI_CEC_Scan(void)
{
	uint32_t flag;
	uint8_t  operValue[16];
	uint8_t  operLen;


	if(GPIO_INTFlagGet(HDMI_HPD_CHECK_INI_IO) & HDMI_HPD_CHECK_STATUS_IO_PIN)
	{
		gHdmiCt->hdmiHPDCheck = GPIO_RegOneBitGet(HDMI_HPD_CHECK_STATUS_IO, HDMI_HPD_CHECK_STATUS_IO_PIN);
		if(gHdmiCt->hdmiHPDCheck)
		{
			WaitMs(5);//确保HPD的高电平状态
			gHdmiCt->hdmiHPDCheck = GPIO_RegOneBitGet(HDMI_HPD_CHECK_STATUS_IO, HDMI_HPD_CHECK_STATUS_IO_PIN);
			if(gHdmiCt->hdmiHPDCheck)
			{
				if(HDMI_DDC_GetInfo())//更新为读取所有的DDC信息
				{
					gHdmiCt->hdmiReportStatus = 1;
					gHdmiCt->hdmi_poweron_flag = 0;
					gHdmiCt->hdmi_arc_flag = 0;
					gHdmiCt->hdmiReportARCStatus = 0;
				}
			}
		}
		GPIO_INTFlagClear(HDMI_HPD_CHECK_INI_IO, HDMI_HPD_CHECK_STATUS_IO_PIN);
	}

	if(HDMI_CEC_IsWorking() == CEC_IS_IDLE)
    {
		flag = 0;
		memset(operValue, 0, sizeof(operValue));
		operLen = 0;
		operLen = HDMI_CEC_MessageDataGet(operValue);
		if(operLen > 0)
		{
			DBG("#0x%x\n", operValue[0]);
			switch(operValue[0])
			{
				case 0xa0:
					if(gHdmiCt->hdmi_poweron_flag == 0)
						gHdmiCt->hdmiReportStatus = 4;
					break;

				case 0x36:
					//if(hdmi_sleep_flag)
						HDMI_CEC_StandbyStatus();
					gHdmiCt->hdmiReportStatus = 0;
					gHdmiCt->hdmi_poweron_flag = 0;
					gHdmiCt->hdmi_arc_flag = 0;
					gHdmiCt->hdmiReportARCStatus = 0;
					flag = 1;
					break;
				case 0x46:
					HDMI_CEC_ReportOSDName();
					flag = 1;
					break;

				case 0x80:
					gHdmiCt->hdmi_poweron_flag = 1;
					gHdmiCt->hdmiReportStatus  = 0;
					HDMI_CEC_RoutingInfo();
					if(gHdmiCt->hdmi_arc_flag == 0)
						gHdmiCt->hdmiReportARCStatus = 1;
					flag = 1;
					break;

				case 0x82:
					if(gHdmiCt->hdmi_poweron_flag == 0)
					{
						HDMI_CEC_SetSystemAudioModeOn();
						gHdmiCt->hdmi_poweron_flag = 1;

					}
					if((gHdmiCt->hdmi_poweron_flag == 1) && (gHdmiCt->hdmi_arc_flag == 0))
					{
						gHdmiCt->hdmiReportARCStatus = 1;
					}
					flag = 1;
					break;

				case 0x83://give physical address
					HDMI_CEC_ReportPhysicalAddress();
					if(gHdmiCt->hdmi_poweron_flag == 0)
						gHdmiCt->hdmiReportStatus = 4;
					flag = 1;
					break;

				case 0x70://system audio mode request
					//if(operLen > 1)
					{
						HDMI_CEC_SetSystemAudioModeOn();
						gHdmiCt->hdmi_poweron_flag = 1;
						gHdmiCt->hdmiReportStatus  = 0;
						if(gHdmiCt->hdmi_arc_flag == 0)
							gHdmiCt->hdmiReportARCStatus = 1;
					}
					/*else
					{
						HDMI_CEC_SetSystemAudioModeoff();
						gHdmiCt->hdmi_poweron_flag = 1;
						gHdmiCt->hdmiReportStatus  = 1;
					}*/
					flag = 1;
					break;
					flag = 1;
					break;

				case 0x71://system audio status request
					HDMI_CEC_SystemAudioModeStatus(mainAppCt.gSysVol.AudioSinkVol[0] * 100 / 32);
					flag = 1;
					break;

				case 0x8c://Give Device Vendor ID
					HDMI_CEC_DeviceVendorID();
					if(gHdmiCt->hdmi_poweron_flag == 0)
						gHdmiCt->hdmiReportStatus = 4;
					flag = 1;
					break;

				case 0x7d://Give System Audio Mode Status
					if(gHdmiCt->hdmi_poweron_flag == 1)
					{
						HDMI_CEC_SystemAudioModeStatusOn();
						if(gHdmiCt->hdmi_arc_flag == 0)
							gHdmiCt->hdmiReportARCStatus = 1;
					}
					else
						HDMI_CEC_SystemAudioModeStatusOff();
					flag = 1;
					break;

				case 0xC1:
					gHdmiCt->hdmi_arc_flag = 1;
					gHdmiCt->hdmiReportARCStatus = 0;
					break;

				case 0xC2:
					gHdmiCt->hdmi_arc_flag = 0;
					gHdmiCt->hdmiReportARCStatus = 0;
					break;

				case 0xC3://Request ARC Initiation
//					if(gHdmiCt->hdmi_poweron_flag == 0)
//					{
//						gHdmiCt->hdmi_poweron_flag = 1;
//						HDMI_CEC_SetSystemAudioModeOn();
//						gHdmiCt->hdmiReportARCStatus = 1;
//					}
//					else
//					{
//						HDMI_CEC_InitiateARC();
//					}
					HDMI_CEC_InitiateARC();
					flag = 1;
					break;

				case 0xC4://Request ARC Termination
					HDMI_CEC_TerminationARC();
					flag = 1;
					break;

				case 0x44:
					gIsVolSetEnable = TRUE;
					SetChannel = VOL_SET_CHANNEL_SINK0;

					if(operValue[1] == 0x41)//vol up
					{
						SystemVolUp();
						HDMI_CEC_SystemAudioModeStatus(mainAppCt.gSysVol.AudioSinkVol[0] * 100 / 32);
						printf("cec vol up key\n");
					}
					else if(operValue[1] == 0x42)//vol DOWN
					{
						SystemVolDown();
						HDMI_CEC_SystemAudioModeStatus(mainAppCt.gSysVol.AudioSinkVol[0] * 100 / 32);
						printf("cec vol down key\n");
					}
					else if(operValue[1] == 0x43)//vol MUTE
					{
						AudioPlayerMute();
						//if(mainAppCt.gSysVol.MuteFlag)
						if(IsAudioPlayerMute() == TRUE)
						{
							HDMI_CEC_SystemAudioModeStatus((mainAppCt.gSysVol.AudioSinkVol[0] * 100 / 32) | 0x80);
						}
						else
						{
							HDMI_CEC_SystemAudioModeStatus((mainAppCt.gSysVol.AudioSinkVol[0] * 100 / 32) & 0x7f);
						}
						printf("cec vol mute key\n");
					}
					flag = 1;
					break;
				case 0x8f:
					HDMI_CEC_SystemPowerOn();
					printf("cec HDMI_CEC_SystemPowerOn\n");
					flag = 1;
					break;
				case 0x9f:
					HDMI_CEC_Version();
					//printf("cec HDMI_CEC_SystemPowerOn\n");
					flag = 1;
					break;

				case 0xA4:
					if((operValue[1] & 0xf) != 1)
						HDMI_CEC_FeatureAbort(0xA4, 0x3);
					else
						HDMI_CEC_AudioFormatCode();
					flag = 1;
					break;

				default:
					break;
			}
		}

		if(flag == 0)
		{
			if(gHdmiCt->hdmiReportStatus == 1)//两次allocate逻辑地址(1,2)
			{
				HDMI_CEC_AllocateLogicalAddress();
				gHdmiCt->hdmiReportStatus = 2;
			}
			else if(gHdmiCt->hdmiReportStatus == 2)
			{
				HDMI_CEC_AllocateLogicalAddress();
				gHdmiCt->hdmiReportStatus = 3;
			}
			else if(gHdmiCt->hdmiReportStatus == 3)
			{
				HDMI_CEC_ReportPhysicalAddress();
				gHdmiCt->hdmiReportStatus = 4;
			}
			else if(gHdmiCt->hdmiReportStatus == 4)//增加重要信息的重发机制
			{
				HDMI_CEC_RequestActiveSource();
				gHdmiCt->hdmiReportStatus = 5;
			}
			else if(gHdmiCt->hdmiReportStatus == 5)
			{
				HDMI_CEC_RequestActiveSource();
				gHdmiCt->hdmiReportStatus = 0;
			}
			else if(gHdmiCt->hdmiReportARCStatus == 1)
			{
				HDMI_CEC_InitiateARC();
				gHdmiCt->hdmiReportARCStatus = 0;
			}
		}
	}
	else if(HDMI_CEC_IsWorking() == CEC_IS_WORKING)
	{
		gHdmiCt->hdmiBusyTime = 1000;
	}
}

void HDMI_CEC_DDC_DeInit(void)
{
	if(gCecInitDef)
	{//保障通信时序完整性，留意潜在风险。
		HDMI_CEC_DeInit(20);
		osPortFree(gCecInitDef);
	}
	gCecInitDef	= NULL;

	if(gHdmiCt)
		osPortFree(gHdmiCt);
	gHdmiCt = NULL;

	if(gCECRxFIFO )
		osPortFree(gCECRxFIFO);
	gCECRxFIFO = NULL;

	if(gCECTxFIFO)
		osPortFree(gCECTxFIFO);
	gCECTxFIFO = NULL;
}


//获取EDID所有信息,返回值 ：1 -- 正确 ， 0 -- 错误
uint8_t HDMI_DDC_GetInfo(void)
{
	const uint8_t IEEERegist[3] = {0x03, 0x0c, 0x00};
	uint8_t i;
	uint8_t flag;
	uint16_t check_sum;
	bool ret_GIE;
	#ifdef DDC_USE_SW_I2C
	#else
	I2C_ErrorState ret;
	#endif
	uint8_t MVDDCReadAddr    = 0x50;
	uint8_t MVDDCReadAddr_sf = 0xA0;

	WDG_Feed();
	memset(&gHdmiCt->edid_buf[0], 0, 256);
	#ifdef DDC_USE_SW_I2C
	I2cReadNByte(DDCI2cHandler, MVDDCReadAddr_sf, 0, &gHdmiCt->edid_buf[0], 128);
	#else
	I2C_MasterReceiveData(MVDDCReadAddr, 0x00, &gHdmiCt->edid_buf[0], 128, 1000);
	#endif

	flag = memcmp(gHdmiCt->edid_buf, MVDDCHeader, 8);
	check_sum = 0;
	for(i=0; i<127; i++)
	{
		check_sum += gHdmiCt->edid_buf[i];
	}
	check_sum = (0x100 - (check_sum & 0xff)) & 0xff;

	if((flag == 0) && (check_sum == gHdmiCt->edid_buf[127]))
	{
		if(gHdmiCt->edid_buf[126] >= 1)
		{
			#ifdef DDC_USE_SW_I2C
			I2cReadNByte(DDCI2cHandler, MVDDCReadAddr_sf, 128, &gHdmiCt->edid_buf[128], 128);
			#else
			I2C_MasterReceiveData(MVDDCReadAddr, 0x80, &gHdmiCt->edid_buf[128], 128, 1000);
			#endif

			check_sum = 0;
			for(i=128; i<255; i++)
			{
				check_sum += gHdmiCt->edid_buf[i];
			}
			check_sum = (0x100 - (check_sum & 0xff)) & 0xff;
			if(check_sum == gHdmiCt->edid_buf[255])
			{
				for(i=128; i<255; i++)
				{
					flag = memcmp(&gHdmiCt->edid_buf[i], IEEERegist, 3);
					if(flag == 0)
					{
						gCecInitDef->cec_info.physical_addr[0] = gHdmiCt->edid_buf[i+3];
						gCecInitDef->cec_info.physical_addr[1] = gHdmiCt->edid_buf[i+4];
						return 1;
					}
				}
			}
		}
	}

	return 0;
}


__attribute__((section(".driver.isr"))) void HDMI_CEC_ARBITRATION_TIMER_FUNC(void)
{
	HDMI_Arbitration_Time_Process();
	if(gHdmiCt->hdmiBusyTime > 0) gHdmiCt->hdmiBusyTime--;
    Timer_InterruptFlagClear(HDMI_CEC_ARBITRATION_TIMER_ID, UPDATE_INTERRUPT_SRC);
}


//------------------------------------------------------------------------

void HDMI_ARC_Init(uint16_t *DMAFifoAddr, uint32_t DMAFifoSize, MCU_CIRCULAR_CONTEXT *ct)
{
	GPIO_RegOneBitClear(HDMI_ARC_RECV_IO_OE, HDMI_ARC_RECV_IO_PIN);
	GPIO_RegOneBitClear(HDMI_ARC_RECV_IO_IE, HDMI_ARC_RECV_IO_PIN);
	GPIO_RegOneBitSet(HDMI_ARC_RECV_IO_ANA,  HDMI_ARC_RECV_IO_PIN);
	SPDIF_AnalogModuleEnable(HDMI_ARC_RECV_ANA_PIN, SPDIF_ANA_LEVEL_200mVpp);

	SPDIF_RXInit(1, 0, 0);
	DMA_ChannelDisable(PERIPHERAL_ID_SPDIF_RX);
	Reset_FunctionReset(DMAC_FUNC_SEPA);
	DMA_CircularConfig(PERIPHERAL_ID_SPDIF_RX, DMAFifoSize/2, DMAFifoAddr, DMAFifoSize);
	SPDIF_ModuleEnable();
	DMA_ChannelEnable(PERIPHERAL_ID_SPDIF_RX);

	hdmiMCUCt = ct;
}

uint8_t HDMI_ARC_LockStatusGet(void)
{
	return SPDIF_FlagStatusGet(LOCK_FLAG_STATUS);
}

//0: not ready, 1: ready
bool HDMI_ARC_IsReady(void)
{
	//待cec通信完成，开启ARC，防止CEC对ARC的干扰
	//1. 有通讯； 2. cec没有收到arc开启的标志； 3. 非cec交互时间低于100ms
	if((gHdmiCt->hdmiReportStatus != 0) || (gHdmiCt->hdmi_arc_flag != 1) || (gHdmiCt->hdmiBusyTime != 0))
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

uint16_t HDMI_ARC_DataLenGet(void)
{

	if(hdmiMCUCt != NULL)
	{
		return (MCUCircular_GetDataLen(hdmiMCUCt) / 4);
	}
	else
	{
		return 0;
	}
}

uint16_t HDMI_ARC_DataGet(void *pcm_out, uint16_t MaxSize)
{
	uint16_t len;

	if(hdmiMCUCt != NULL)
	{
		len = MCUCircular_GetData(hdmiMCUCt, pcm_out, MaxSize * 4);
		len = (len >> 2);
		return len;
	}
	else
	{
		return 0;
	}
}

void HDMI_ARC_DeInit(void)
{
	SPDIF_ModuleDisable();
	DMA_ChannelDisable(PERIPHERAL_ID_SPDIF_RX);
	SPDIF_AnalogModuleDisable();
	GPIO_PortAModeSet(HDMI_ARC_RECV_IO_PIN, 0);
}

void HDMI_ARC_OnOff(uint8_t ArcOnFlag)
{
	if(ArcOnFlag == 1)
	{
		HDMI_CEC_SetSystemAudioModeOn();
		DBG("startARC\n");	
	}
	else
	{
		HDMI_CEC_SetSystemAudioModeoff();		
		DBG("STopARC\n");
	}
}
#endif  //CFG_APP_HDMIIN_MODE_EN



