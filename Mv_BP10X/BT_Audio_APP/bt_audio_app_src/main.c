#include <stdlib.h>
#include <nds32_intrinsic.h>
#include "uarts.h"
#include "uarts_interface.h"
#include "backup.h"
#include "backup_interface.h"
#include "type.h"
#include "debug.h"
#include "timeout.h"
#include "clk.h"
#include "dma.h"
#include "timer.h"
#include "adc.h"
#include "i2s.h"
#include "watchdog.h"
#include "reset.h"
#include "rtc.h"
#include "spi_flash.h"
#include "gpio.h"
#include "chip_info.h"
#include "irqn.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "remap.h"
#include "rtos_api.h"
#include "main_task.h"
#include "dac.h" //for volscale
#include "otg_detect.h"
#include "sw_uart.h"
#include "remind_sound_service.h"
#ifdef CFG_APP_BT_MODE_EN
#include "bt_stack_api.h"
#endif
#include "file.h"
#include "flash_boot.h"
#include "sadc_interface.h"
#include "app_config.h"
#include "hdmi_in_mode.h"
#include "spdif_mode.h"
#include "powercontroller.h"
//#include "OrioleReg.h"
#include "audio_decoder_api.h"

#ifdef CFG_FUNC_DISPLAY_EN
#include "display.h"
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_HFP_SUPPORT == ENABLE)
#include "bt_hf_mode.h"
#endif
#endif

#include "rtc_timer.h"
extern const unsigned char *GetLibVersionFatfsACC(void);
#ifdef CFG_FUNC_MAIN_DEEPSLEEP_EN
#include "delay.h"

#include "ir_key.h"
#include "deepsleep.h" 
#include "misc.h"
#include "efuse.h"

/**根据appconfig缺省配置:DMA 8个通道配置**/
/*1、SD卡录音需PERIPHERAL_ID_SDIO1 RX/TX*/
/*2、在线串口调音需PERIPHERAL_ID_UART1 RX/TX*/ 
/*3、线路输入需PERIPHERAL_ID_AUDIO_ADC0_RX*/
/*4、Mic开启需PERIPHERAL_ID_AUDIO_ADC1_RX，mode之间通道必须一致*/
/*5、Dac0开启需PERIPHERAL_ID_AUDIO_DAC0_TX mode之间通道必须一致*/
/*6、DacX需开启PERIPHERAL_ID_AUDIO_DAC1_TX mode之间通道必须一致*/

static uint8_t DmaChannelMap[29] = {
	255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
	255,//PERIPHERAL_ID_SPIS_TX,		//1
	255,//PERIPHERAL_ID_TIMER3,			//2
	4,//PERIPHERAL_ID_SDIO_RX,			//3
	5,//PERIPHERAL_ID_SDIO0_TX,			//4
	255,//PERIPHERAL_ID_UART0_RX,		//5
	255,//PERIPHERAL_ID_TIMER1,			//6
	255,//PERIPHERAL_ID_TIMER2,			//7
	255,//PERIPHERAL_ID_SDPIF_RX,		//8 SPDIF_RX /TX same chanell
	255,//PERIPHERAL_ID_SDPIF_TX,		//8 SPDIF_RX /TX same chanell
	255,//PERIPHERAL_ID_SPIM_RX,		//9
	255,//PERIPHERAL_ID_SPIM_TX,		//10
	255,//PERIPHERAL_ID_UART0_TX,		//11
	
#ifdef CFG_COMMUNICATION_BY_UART	
	7,//PERIPHERAL_ID_UART1_RX,			//12
	6,//PERIPHERAL_ID_UART1_TX,			//13
#else
	255,//PERIPHERAL_ID_UART1_RX,		//12
	255,//PERIPHERAL_ID_UART1_TX,		//13
#endif

	255,//PERIPHERAL_ID_TIMER4,			//14
	255,//PERIPHERAL_ID_TIMER5,			//15
	255,//PERIPHERAL_ID_TIMER6,			//16
	0,//PERIPHERAL_ID_AUDIO_ADC0_RX,	//17
	1,//PERIPHERAL_ID_AUDIO_ADC1_RX,	//18
	2,//PERIPHERAL_ID_AUDIO_DAC0_TX,	//19
	3,//PERIPHERAL_ID_AUDIO_DAC1_TX,	//20
	255,//PERIPHERAL_ID_I2S0_RX,		//21
#if	(defined(CFG_RES_AUDIO_I2SOUT_EN )&&(CFG_RES_I2S_PORT==0))
	7,//PERIPHERAL_ID_I2S0_TX,			//22
#else	
	255,//PERIPHERAL_ID_I2S0_TX,		//22
#endif	
	255,//PERIPHERAL_ID_I2S1_RX,		//23
#if	(defined(CFG_RES_AUDIO_I2SOUT_EN )&&(CFG_RES_I2S_PORT==1))
	7,	//PERIPHERAL_ID_I2S1_TX,		//24
#else
	255,//PERIPHERAL_ID_I2S1_TX,		//24
#endif
	255,//PERIPHERAL_ID_PPWM,			//25
	255,//PERIPHERAL_ID_ADC,     		//26
	255,//PERIPHERAL_ID_SOFTWARE,		//27
};

#endif

extern void UsbAudioTimer1msProcess(void);
void EnableSwUartAsUART(uint8_t EnableFlag); //retarget.c
void report_up_grate(void);
uint8_t Reset_FlagGet_Flash_Boot(void);

void ResetFlagGet(uint8_t Flag)
{
	DBG("RstFlag = %x\n", Flag);

	if(Flag & 0x01)
	{
		DBG("power on reset\n");
	}
	if(Flag & 0x02)
	{
		DBG("pin reset\n");
	}
	if(Flag & 0x04)
	{
		DBG("watchdog reset\n");
	}
	if(Flag & 0x08)
	{
		DBG("LVD reset\n");
	}
	if(Flag & 0x10)
	{
		DBG("cpu debug reset\n");
	}
	if(Flag & 0x20)
	{
		DBG("system reset\n");
	}	
	if(Flag & 0x40)
	{
		DBG("cpu core reset\n");
	}
	DBG("\n");
}

//__attribute__((section(".driver.isr")))
void Timer2Interrupt(void)
{
	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
	OTG_PortLinkCheck();

    #ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
	extern uint32_t  Silence_Power_Off_Time;
	if(Silence_Power_Off_Time)
		Silence_Power_Off_Time--;
	#endif

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	UsbAudioTimer1msProcess(); //1ms中断监控
#endif

#ifdef	CFG_FUNC_POWERKEY_EN
#if	((POWERKEY_MODE == POWERKEY_MODE_SLIDE_SWITCH_LPD) || (POWERKEY_MODE == POWERKEY_MODE_SLIDE_SWITCH_HPD))
	{
		static uint32_t CntTimer = 500;
		if(SystemPowerKeyDetect())
		{
			CntTimer--;
			if(CntTimer == 0)
			{
				SystemPowerDown();
			}
		}
		else
		{
			CntTimer = 500;
		}
	}
#endif
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_HFP_SUPPORT == ENABLE)
		BtHf_Timer1msCheck();
#endif
#endif
}

#ifdef CFG_FUNC_LED_REFRESH
__attribute__((section(".tcm_section")))
void Timer6Interrupt(void)
{
	Timer_InterruptFlagClear(TIMER6, UPDATE_INTERRUPT_SRC);

	//示例代码，需要添加段“.tcm_section”
	//关键字    __attribute__((section(".tcm_section")))
	//客户需要将自己的实现的API代码添加关键字
	//GPIO_RegOneBitSet(GPIO_A_TGL, GPIO_INDEX2);
	LedFlushDisp();
}
#endif

void SystemClockInit(void)
{
	//clock配置
	Clock_Config(1, 24000000);
	Clock_PllLock(288000);
	Clock_APllLock(240000);
	Clock_USBClkDivSet(4);// bkd add for u disk 60M 2019.4.17
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_USBClkSelect(APLL_CLK_MODE);
	Clock_UARTClkSelect(APLL_CLK_MODE);
	Clock_Timer3ClkSelect(RC_CLK_MODE);//for cec rc clk

	Clock_Module1Enable(ALL_MODULE1_CLK_SWITCH);
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	Clock_Module3Enable(ALL_MODULE3_CLK_SWITCH);
}

void LogUartConfig(bool InitBandRate)
{
#ifdef CFG_FUNC_DEBUG_EN
#ifdef CFG_USE_SW_UART
	SwUartTxInit(SW_UART_IO_PORT, SW_UART_IO_PORT_PIN_INDEX, CFG_UART_BANDRATE);
	if(InitBandRate)
	{
		EnableSwUartAsUART(1);
	}
#else
	switch (CFG_UART_TX_PORT)
	{
	case 0:
		GPIO_PortAModeSet(GPIOA5, 1);//Rx, A5:uart0_rxd_1
		GPIO_PortAModeSet(GPIOA6, 5);//Tx, A6:uart0_rxd_1
		if(InitBandRate)
		{
			DbgUartInit(0, CFG_UART_BANDRATE, 8, 0, 1);
		}
		break;
	case 1:
		GPIO_PortAModeSet(GPIOA9, 1);//Rx, A9:uart1_rxd_0
		GPIO_PortAModeSet(GPIOA10, 3);//Tx,A10:uart1_txd_0
		if(InitBandRate)
		{
			DbgUartInit(1, CFG_UART_BANDRATE, 8, 0, 1);
		}
		break;
	case 2:
		GPIO_PortAModeSet(GPIOA24, 1);	//Rx, A24:uart1_rxd_0
		GPIO_PortAModeSet(GPIOA25, 6);	//Tx, A25:uart1_txd_0
		if(InitBandRate)
		{
			DbgUartInit(1, CFG_UART_BANDRATE, 8, 0, 1);
		}
		break;
	default:
		break;
	}
#endif
#endif

}

#if	defined(CFG_FUNC_DEEPSLEEP_EN) || defined(CFG_FUNC_MAIN_DEEPSLEEP_EN)
inline void SleepMain(void)
{
	Clock_UARTClkSelect(RC_CLK_MODE);//先切换log clk。避免后续慢速处理
	LogUartConfig(TRUE); //scan不打印时 可屏蔽
	SysTickDeInit();
//	Efuse_ReadDataDisable(); //////////关闭EFUSE////////
	SpiFlashInit(80000000, MODE_1BIT, 0, 1);//rc时钟 不支持flash 4bit，系统恢复时重配。
	Clock_DeepSleepSysClkSelect(RC_CLK_MODE, FSHC_RC_CLK_MODE, 1);
	Clock_PllClose();
	Clock_AUPllClose();//AUPLL会增加约980uA的功耗
#if !defined(CFG_RES_RTC_EN)
	Clock_HOSCDisable();//若有RTC应用并且RTC所用时钟是HOSC，则不关闭HOSC，即24M晶振
#endif
//	Clock_LOSCDisable(); //若有RTC应用并且RTC所用时钟是LOSC，则不关闭LOSC，即32K晶振

	SysTickInit();
}

void WakeupMain(void)
{
	Chip_Init(1);
	SysTickDeInit();
	WDG_Enable(WDG_STEP_4S);
	SystemClockInit();
	SysTickInit();
	Clock_DeepSleepSysClkSelect(PLL_CLK_MODE, FSHC_PLL_CLK_MODE, 0);
	SpiFlashInit(80000000, MODE_4BIT, 0, 1);
	LogUartConfig(TRUE);//调整时钟后，重配串口前不要打印。
//	Clock_Pll5ClkDivSet(8);// bkd mark sleep
	//B0B1为SW调式端口，在调试阶段若系统进入了低功耗模式时关闭了GPIO复用模式，请在此处开启
	//GPIO_PortBModeSet(GPIOB0, 0x3); //B0 sw_clk(i) 
	//GPIO_PortBModeSet(GPIOB1, 0x4); //B1 sw_d(io)
	DBG("Main:wakeup\n");
}
#endif


extern const RTC_DATE_TIME ResetRtcVal;

int main(void)
{
	uint16_t RstFlag = 0;

	Chip_Init(1);

	WDG_Enable(WDG_STEP_4S);
	//WDG_Disable();
	
//#if FLASH_BOOT_EN
//	RstFlag = Reset_FlagGet_Flash_Boot();
//#else
	RstFlag = Reset_FlagGet();
	Reset_FlagClear();
//#endif

	//如果需要使用NVM内存时，需要调用该API，第一次系统上电需要清除对NVM内存清零操作
	BACKUP_NVMInit();
	
	SystemClockInit();

	LogUartConfig(TRUE);

#if FLASH_BOOT_EN
	Remap_DisableTcm();
	Remap_InitTcm(FLASH_ADDR+0x10000, TCM_SIZE);
#else
	Remap_InitTcm(FLASH_ADDR, TCM_SIZE);
#endif

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);
	Clock_RcFreqGet(TRUE);//不可屏蔽
	//考虑到大容量的8M flash，写之前需要Unlock，SDK默认不做加锁保护
	//用户为了增加flash 安全性，请根据自己flash大小和实际情况酌情做flash加锁保护机制
	//SpiFlashIOCtrl(IOCTL_FLASH_PROTECT, FLASH_LOCK_RANGE_HALF);//加锁保护code区域

	prvInitialiseHeap();
	osSemaphoreMutexCreate();//硬件串口OS启用了软件锁，必须在创建锁之后输出log，否则死机，锁要初始化堆栈后创建。软件模拟串口不影响。
#ifdef CFG_RES_RTC_EN
    RTC_ClockSrcSel(OSC_12M);//此函数的参数选择必须和上面系统初始化选择的晶振（“Clock_Config()”）保持一致
	RTC_IntDisable();//默认关闭RTC中断
	RTC_IntFlagClear();
	RTC_WakeupDisable();
#ifdef CFG_FUNC_ALARM_EN
//	RTC_DateTimerSet((RTC_DATE_TIME*)&ResetRtcVal);//设置错误尚未处理
#endif

#endif

#ifdef	CFG_FUNC_POWERKEY_EN
	Backup_Clock(BACKUP_CLK_32K_RC32);//开发板基于32K晶体，默认使用片内RC32K，RTC功能需要高精度RTC，请选择晶体32K时钟
	while(!BACKUP_IsOscClkToggle());	//wait backup clk ready.
	SystemPowerKeyInit(POWERKEY_MODE, POWERKEY_CNT);
	PowerKeyModeGet();
#endif

#ifdef CFG_SOFT_POWER_KEY_EN
	SoftPowerInit();
	WaitSoftKey();
#endif
	NVIC_EnableIRQ(SWI_IRQn);
	GIE_ENABLE();	//开启总中断

#ifdef CFG_FUNC_LED_REFRESH
	//默认优先级为0，旨在提高刷新速率，特别是断点记忆等写flash操作有影响刷屏，必须严格遵守所有timer6中断调用都是TCM代码，含调用的driver库代码
	//已确认GPIO_RegOneBitSet、GPIO_RegOneBitClear在TCM区，其他api请先确认。
	NVIC_SetPriority(Timer6_IRQn, 0);
 	Timer_Config(TIMER6,1000,0);
 	Timer_Start(TIMER6);
 	NVIC_EnableIRQ(Timer6_IRQn);

 	//此行代码仅仅用于延时，配合Timer中断处理函数，客户一定要做修改调整
 	//GPIO_RegOneBitSet(GPIO_A_OE, GPIO_INDEX2);//only test，user must modify
#endif

#ifdef CFG_FUNC_DISPLAY_EN
 	DispInit(0);
#endif
	DBG("\n");
	DBG("****************************************************************\n");
	DBG("|                    MVsilicon B1 SDK                      |\n");
	DBG("|            Mountain View Silicon Technology Co.,Ltd.         |\n");
	DBG("****************************************************************\n");
	DBG("sys clk =%ld\n",Clock_SysClockFreqGet());
#if 0
{  
	uint64_t tem;  
	uint32_t Chip_ID_High;  
	uint32_t Chip_ID_Low;  
	tem = Chip_IDGet();  
	Chip_ID_High = (tem>> 32) & 0xffffffff;  
	Chip_ID_Low= (tem) & 0xffffffff;  
	DBG("Chip_ID == %llx\n",tem);  
	DBG("Chip_ID_High == %x\n",Chip_ID_High);  
	DBG("Chip_ID_Low == %x\n",Chip_ID_Low); 
}
#endif
	//dpll mclk
	//GPIO_PortAModeSet(GPIOA1, 8);

//#if FLASH_BOOT_EN
//    report_up_grate();
//#endif
    ResetFlagGet(RstFlag);

	DBG("Audio Decoder Version: %s\n", (unsigned char *)audio_decoder_get_lib_version());
	
    DBG("Driver Version: %s %x\n", GetLibVersionDriver(),Chip_Version());//
#ifdef CFG_FUNC_LRC_EN
    DBG("Lrc Version: %s\n", GetLibVersionLrc()); //bkd 
#endif
#ifdef CFG_APP_BT_MODE_EN
    DBG("BtLib Version: %s\n", (unsigned char *)GetLibVersionBt());
#endif
#ifdef CFG_RES_FLASHFS_EN
	DBG("FlashFSLib Version: %s\n", (unsigned char *)GetLibVersionFlashFS());
#endif
	DBG("Fatfs presearch acc Lib Version: %s\n", (unsigned char *)GetLibVersionFatfsACC());
#ifdef CFG_FUNC_ALARM_EN
	DBG("RTC Version: %s\n", GetLibVersionRTC());//bkd 
#endif
	DBG("\n");

	MainAppTaskStart();
	vTaskStartScheduler();

	while(1);

}

