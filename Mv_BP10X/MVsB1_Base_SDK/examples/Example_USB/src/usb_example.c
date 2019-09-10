/**
 **************************************************************************************
 * @file    usb_example.c
 * @brief   usb example
 *
 * @author  owen
 * @version V1.0.0
 *
 * $Created: 2018-6-7 10:16:10$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "gpio.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "pwm.h"
#include "delay.h"
#include "rtc.h"
#include "sd_card.h"
#include "watchdog.h"
#include "irqn.h"
#include "spi_flash.h"
#include "remap.h"
#include "otg_host_udisk.h"
#include "otg_device_hcd.h"
#include "otg_device_standard_request.h"
#include "otg_device_stor.h"
#include "chip_info.h"
#include "otg_detect.h"
#include "otg_device_audio.h"
#include "dac.h"

extern void SysTickInit(void);
extern void UsbAudioMicDacInit(void);
extern void OTG_DeviceAudioInit();

extern void UsbAudioTimer1msProcess(void);
//__attribute__((section(".driver.isr")))
void Timer2Interrupt(void)
{
	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
	OTG_PortLinkCheck();
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	UsbAudioTimer1msProcess(); //1ms中断监控
#endif
}

static uint8_t DmaChannelMap[29] = {
	255,//PERIPHERAL_ID_SPIS_RX = 0,		//0
	255,//PERIPHERAL_ID_SPIS_TX,			//1
	255,//PERIPHERAL_ID_TIMER3,			//2
	0,//PERIPHERAL_ID_SDIO_RX,			//3
	1,//PERIPHERAL_ID_SDIO_TX,			//4
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
	4,//PERIPHERAL_ID_AUDIO_DAC0_TX,		//20
	5,//PERIPHERAL_ID_AUDIO_DAC1_TX,		//21
	255,//PERIPHERAL_ID_I2S0_RX,			//22
	255,//PERIPHERAL_ID_I2S0_TX,			//23
	255,//PERIPHERAL_ID_I2S1_RX,			//24
	255,//PERIPHERAL_ID_I2S1_TX,			//25
	255,//PERIPHERAL_ID_PPWM,				//26
	255,//PERIPHERAL_ID_ADC,     			//27
	255,//PERIPHERAL_ID_SOFTWARE,			//28
};

uint8_t read_buf[4096];
uint8_t write_buf[4096];


void usb_host_example(void)
{
	DBG("usb host example\n");
	OTG_HostControlInit();
	OTG_HostEnumDevice();
	if(UDiskInit() == 1)
	{
		DBG("枚举MASSSTORAGE接口OK\n");
		UDiskReadBlock(0,write_buf,1);
		UDiskWriteBlock(0,write_buf,1);
		UDiskReadBlock(0,read_buf,1);
		if(memcmp(write_buf,read_buf,512) == 0)
		{
			DBG("读写U盘数据OK\n");
		}
		else
		{
			DBG("读写U盘数据错误\n");
		}
	}
	else
	{
		DBG("UDisk Init err\n");
	}
}

//void CardPortInit(uint8_t SdioPort)
//{
//	switch(SdioPort)
//	{
//		case SDIO_A15_A16_A17:
//			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA15);
//			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA16);
//			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA17);
//			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA15);
//			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA16);
//			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA17);
//			GPIO_PortAModeSet(GPIOA15, SDIO_CMD_A15_MODE);//CMD
//			GPIO_PortAModeSet(GPIOA16, SDIO_CLK_A16_MODE);//CLK
//			GPIO_PortAModeSet(GPIOA17, SDIO_DAT_A17_MODE);//DAT
//			break;
//
//		case SDIO_A20_A21_A22:
//			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA20);
//			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA21);
//			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA22);
//			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA20);
//			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA21);
//			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA22);
//			GPIO_PortAModeSet(GPIOA20, SDIO_CMD_A20_MODE);//CMD
//			GPIO_PortAModeSet(GPIOA21, SDIO_CLK_A21_MODE);//CLK
//			GPIO_PortAModeSet(GPIOA22, SDIO_DAT_A22_MODE);//DAT
//			break;
//
//		default:
//			break;
//	}
//}

void usb_device_example(uint8_t mode)
{
	DMA_ChannelAllocTableSet(DmaChannelMap);
	if((mode == READER)||(mode == READER_ADUIO)||(mode == READER_HID)||(mode == READER_AUDIO_HID))
	{
		CardPortInit(SDIO_A15_A16_A17);

		if(SDCard_Init() != NONE_ERR)
		{
			DBG("sd card error\n");
			while(1);
		}
	}
	if((mode == AUDIO)||(mode == READER_ADUIO)||(mode == AUDIO_HID)||(mode == READER_AUDIO_HID))
	{
		OTG_DeviceAudioInit();
		UsbAudioMicDacInit();
	}
	//OTG_DeviceModeSel(mode);
	OTG_DeviceModeSel(mode,0x0000,0x1234);
	OTG_DeviceInit();
	NVIC_EnableIRQ(Usb_IRQn);

	while(1)
	{
		OTG_DeviceRequestProcess();
		OTG_DeviceStorProcess();
	}
}

extern void USB_DelayLine(void);
int main(void)
{
	Chip_Init(1);
	WDG_Disable();

	Clock_Module1Enable(ALL_MODULE1_CLK_SWITCH);
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	Clock_Module3Enable(ALL_MODULE3_CLK_SWITCH);

	Clock_Config(1, 24000000);
	Clock_PllLock(288000);
	Clock_APllLock(240000);

	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(APLL_CLK_MODE);
	Clock_Timer3ClkSelect(SYSTEM_CLK_MODE);

	Clock_USBClkDivSet(4);
	Clock_USBClkSelect(APLL_CLK_MODE);

	Remap_InitTcm(FLASH_ADDR, TCM_SIZE);
	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	//GPIO_PortAModeSet(GPIOA9, 1);//Rx, A9:uart1_rxd_0
	//GPIO_PortAModeSet(GPIOA10, 3);//Tx,A10:uart1_txd_0
	GPIO_PortAModeSet(GPIOA24, 1);	//Rx, A24:uart1_rxd_0
	GPIO_PortAModeSet(GPIOA25, 6);	//Tx, A25:uart1_txd_0
	DbgUartInit(1, 256000, 8, 0, 1);

	GIE_ENABLE();
	SysTickInit();

	DBG("\n");
    DBG("/-----------------------------------------------------\\\n");
    DBG("|                    USB Example                      |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");

 	Timer_Config(TIMER2,1000,0);
 	Timer_Start(TIMER2);
 	NVIC_EnableIRQ(Timer2_IRQn);

	//usb_host_example();

	usb_device_example(READER_ADUIO);
 	//usb_device_example(READER);

	while(1);
}

bool IsUsbAudioMode =  TRUE; //USB声卡/mic数据时钟启用传输（有没有被暂停）

const uint8_t SilentData[192] =
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

void UsbAudioTimer1msProcess(void)
{
	uint32_t FramLen = 192;
	static uint32_t FramCount = 0;
	static  bool IsUsbAduioInitFlag = FALSE;

	if(IsUsbAudioMode == FALSE)
	{
		return;
	}
	if(UsbAudioMic.InitOk == 0)
	{
		IsUsbAduioInitFlag = FALSE;
		return;
	}
	if(UsbAudioSpeaker.InitOk == 0)
	{
		IsUsbAduioInitFlag = FALSE;
		return;
	}

	FramCount++;
	if(AudioDAC_SampleRateGet(DAC0) == 44100)
	{
		FramLen = 176;
		if((FramCount%10) == 0)
		{
			FramCount = 0;
			FramLen = 180;
		}
	}
	////PC没有传送任何数据
	if(UsbAudioSpeaker.AltSet == 0)
	{
		//需要根据采样率填写数据
		//MCUCircular_PutData(&SpeakerCircularBuf,(void*)SilentData,FramLen);
		//uint16_t AudioDAC_DataSet(DAC_MODULE DACModule, void* Buf, uint16_t Len)
		AudioDAC_DataSet(DAC0, (void*)SilentData, FramLen/4);
	}

	//adc
	////PC没有传送任何数据
	if(UsbAudioMic.AltSet == 0)
	{
		//需要更具采样率丢弃数据
	}
	if(!IsUsbAduioInitFlag)
	{
		IsUsbAduioInitFlag = TRUE;
	}
}

//__attribute__((section(".driver.isr"))) void BT_Interrupt(void)
//{
//}
//
//__attribute__((section(".driver.isr"))) void BLE_Interrupt(void)
//{
//}
//
//__attribute__((section(".driver.isr"))) void IRInterrupt(void)
//{
//}
