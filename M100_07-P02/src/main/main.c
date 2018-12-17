//  maintainer: Halley
#include "type.h"
#include "app_config.h"
#include "os.h"
#include "msgq.h"
#include "app_msg.h"
#include "uart.h"
#include "key.h"
#include "clk.h"
#include "spi_flash.h"
#include "cache.h"
#include "gpio.h"
#include "dac.h"
#include "audio_adc.h"
#include "wakeup.h"
#include "timer.h"
#include "sys_app.h"
#include "power_management.h"
#include "rtc.h"
#include "adc.h"
#include "ir.h"
#include "host_hcd.h"
#include "host_stor.h"
#include "watchdog.h"
#include "mixer.h"
#include "breakpoint.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "eq.h"
#include "lcd_seg.h"
#include "eq_params.h"
#include "sound_remind.h"
#include "sw_uart.h"
#include "debug.h"
#include "bt_stack_api.h"
#include "audio_decoder.h"
#include "get_bootup_info.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "micro_adjust.h"
#include "fat_file.h"
#include "task_decoder.h"
#include "presearch.h"
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#endif
#ifdef FUNC_SLAVE_UART_EN
#include "slave_uart_com.h"
#endif
#ifdef FUNC_WIFI_EN
#include "wifi_control.h"
#include "wifi_uart_com.h"
#endif
#include "player_control.h"
#include "STA33X.h"

int32_t BtTaskHandle = 0;
int32_t MainTaskHandle = 0;
xTaskHandle AudioProcessHandle;

extern SwEqContext * p_gSwEq;
#ifdef FLASH_ERASE_IN_TCM
SPI_FLASH_INFO gFlashInfo;
#endif

#ifdef FUNC_SPI_UPDATE_EN
extern void BootUpgradeChk(void);
#endif
extern void AudioProcessTaskEntrance(void);

//extern uint8_t UpgradeFileFound;

extern void DetectMassTestCondition(void);
extern bool GetMassTestFlag(void);
extern bool GetHfTransferState(void);
extern bool FlshBTInfoAreaInit(void);
extern void GpioPowerKeyScanInit(void);


__attribute__((section(".driver.isr"))) void Timer1Interrupt(void)
{
	Timer1IntClr();
#if (defined(FUNC_USB_EN) || defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN))
	OTGLinkCheck();
#endif
	SystemPowerOffDetect();
#ifdef FUNC_WIFI_EN
	Mcu_TimerPollingRcvUartData();	
#endif
}

////注意boot版本，如果是boot5.0版本
//GPIOA0做为普通GPIO使用时请请main最开头代码使能
int32_t main(void)
{
	extern void GuiTaskEntrance(void);
//	extern void DecoderTaskEntrance(void);
	extern void BTEntrance(void);
	extern void ShellCommand(void);
	
	//boot5.0芯片，GPIO做为普通GPIO使用时，请打开下面这行代码
	GpioA0SetMode(A0_FOR_GPIO);
	
#if defined(USE_POWERKEY_SOFT_PUSH_BUTTON) || defined(FUNC_GPIO_POWERKEY_EN)
	LED_WHITE_MODE_OFF();
#else
	LED_WHITE_MODE_ON();
#endif
	
#ifdef FUNC_AMP_MUTE_EN
	AmpMuteControl(TRUE);
#endif
	
#ifdef FUNC_SLAVE_BLUETOOTH_EN
	SlaveBtPowerOff();
#endif
	
#ifdef FUNC_5VIN_TRIM_LDO3V3   	
	SarAdcTrimLdo3V3();   //attention! only used in Power = 5V
#endif
		
	ClkModuleEn(ALL_MODULE_CLK_SWITCH);	
	ClkModuleGateEn(ALL_MODULE_CLK_GATE_SWITCH);        //open all clk gating
	CacheInit();
	
//	SysGotoPowerDown();

	SysGetWakeUpFlag(); //get wake up flag, DO NOT remove this!!

#ifdef FUNC_WIFI_POWER_KEEP_ON
	WiFiControlGpioInit();		
#endif
#ifdef USE_POWERKEY_SLIDE_SWITCH
  #ifdef FUNC_GPIO_SLIDE_SWITCH_EN
	GpioPowerKeyScanInit();
	#endif
	SysPowerKeyInit(POWERKEY_MODE_SLIDE_SWITCH, 200);//200ms
#endif
#ifdef USE_POWERKEY_SOFT_PUSH_BUTTON
	SysPowerKeyInit(POWERKEY_MODE_PUSH_BUTTON, 2000); //2s
#endif

	SpiFlashInfoInit();		//Flash RD/WR/ER/LOCK initialization
#ifdef FLASH_ERASE_IN_TCM
	SpiFlashGetInfo(&gFlashInfo);
#endif
 
	ClkPorRcToDpll(0); 		//clock src is 32768hz OSC
	
	//GD flash，选择SYS CLK或DPLL80Mhz，可以开启HPM，其他型号建议关闭HPM
	SpiFlashClkSet(FLASHCLK_SYSCLK_SEL, TRUE);
	
#ifdef FUNC_RADIO_DIV_CLK_EN
	ClkDpllClkGatingEn(1);
#endif
	SarAdcLdoinVolInit();    //do NOT remove this
	LcdCtrlRegInit();        //do NOT remove this
	
#ifdef FUNC_WIFI_POWER_KEEP_ON
	WiFiPowerOn();
#endif
	/*
	 * Now, uart has been initialized automatically by OS.
	 * if you want to customize debug uart setting(defaut 115200-8-N-1),
	 * you can invoke the function at any place you want(for example here).
	 */
#ifdef FUNC_DEBUG_EN
 #ifdef FUNC_SWUART_DBG_EN
  OsSetDebugFlag(1);
  SwUartTxInit(SWUART_GPIO_PORT_A, 0, 115200);
	EnableSwUartAsFuart(TRUE);
 #else
	OsSetDebugFlag(1);
	GpioFuartRxIoConfig(DEBUG_RX_PORT);
	GpioFuartTxIoConfig(DEBUG_TX_PORT);
	OSDBGUartInit(115200, 8, 0, 1);
 #endif
#else
	OsSetDebugFlag(0);
#endif


#if (!defined(FUNC_USB_AUDIO_EN) && !defined(FUNC_USB_READER_EN) && !defined(FUNC_USB_AUDIO_READER_EN))
#undef PC_PORT_NUM
#define PC_PORT_NUM    0
#endif

#ifdef FUNC_SPI_UPDATE_EN
	BootUpgradeChk();	//boot upgrade check upgrade for the last time
#endif

	Osc32kExtCapCalibrate();//32KHz external oscillator calibration
#ifdef FUNC_BREAKPOINT_EN
	BP_LoadInfo();// 检查、装载（如果第一次上电）基本配置信息，DO NOT remove this code!!!
#endif

	OSStartKernel();                //start initialize os kernel

#ifdef MASS_PRODUCTION_TEST_FUNC
	DetectMassTestCondition();
#endif

	OSStartSchedule();              //Start the scheduler.

	OSTaskPrioSet(NULL, 4);

	FlashUnlock();
	//FlashLock(FLASH_LOCK_RANGE_HALF);		// if need, Lock range setting see function description
	
	SysVarInit();                  //全局变量初始化，注意在BP_LoadInfo之后调用

#if (defined(FUNC_ADC_KEY_EN) || defined(FUNC_IR_KEY_EN) || defined(FUNC_CODING_KEY_EN))
	KeyInit();
#endif
	//GpioSdIoConfig(SD_PORT_NUM);    // set sd port
#ifdef FUNC_CARD_EN
	IsCardLink();
#endif
#ifdef FUNC_USB_EN
	Usb1SetDetectMode((UDISK_PORT_NUM == 1), (PC_PORT_NUM == 1)); // usb1 port host/device mode set
	Usb2SetDetectMode((UDISK_PORT_NUM == 2), (PC_PORT_NUM == 2)); // usb2 port host/device mode set
	UsbSetCurrentPort(UDISK_PORT_NUM); // set usb host port
#endif
	InitDeviceDetect();                 // 上电时，设备状态扫描消抖

//#ifdef FUNC_RTC_EN
//	RtcInit(AlarmWorkMem, sizeof(AlarmWorkMem));
//	NVIC_EnableIRQ(RTC_IRQn);           // 打开闹钟提醒中断
//#endif

//	SystemOn();
//如果电能控制功能打开，此处将启动第1次低电压检测和充电设备接入检测等
#ifdef FUNC_POWER_MONITOR_EN
	PowerMonitorInit();
#endif
	//DacNoUseCapacityToPlay();

	AudioAnaInit();	//模拟通道初始化
	DacVolumeSet(DAC_DIGITAL_VOL, DAC_DIGITAL_VOL);
	AdcVolumeSet(ADC_DIGITAL_VOL, ADC_DIGITAL_VOL);
	MixerInit((void*)PCM_FIFO_ADDR, PCM_FIFO_LEN);
    AudioOutputInit();//输出通道初始化
    
#ifdef FUNC_MIXER_SRC_EN
	MixerSrcEnable(TRUE);
#else
	MixerSrcEnable(FALSE);
#endif
	MixerSetFadeSpeed(MIXER_SOURCE_DEC, DEC_FADEIN_TIME, 10);

#if defined(FUNC_AUDIO_EFFECT_EQ_EN)
  gSys.Eq = (EQ_STYLE_SUM - 1);     //如果SDK音效打开，默认将EQ配置为最后一个自定义EQ
#else
  gSys.Eq = 2;                      //否则，将EQ配置为Flat
#endif
	EqStyleInit(p_gSwEq);
	EqStyleSelect(p_gSwEq, 44100, gSys.Eq);
//#ifdef FUNC_TREB_BASS_EN
//	TrebBassSet(gSys.TrebVal, gSys.BassVal);
//#endif
#ifdef FUNC_SOUND_REMIND
	SoundRemindInit();
#endif
#ifdef FUNC_SOFT_ADJUST_EN
    SoftAdjustInit(2);
#endif
	SetDriverTerminateFuc(GetQuickResponseFlag);
	SetFsTerminateFuc(GetQuickResponseFlag);
	
	APP_DBG("****************************************************************\n");
	APP_DBG("System Clock     :%d MHz(%d)\n", ClkGetCurrentSysClkFreq() / 1000000, ClkGetCurrentSysClkFreq());
	APP_DBG("Flash Clock      :%d MHz(%d)\n", ClkGetFshcClkFreq() / 1000000, ClkGetFshcClkFreq());
	APP_DBG("BOOT Version     :%d.%d.%d%c\n", GetBootVersion(), GetPatchVersion() / 10, GetPatchVersion() % 10,GetBootVersionEx());
	APP_DBG("SDK  Version     :%d.%d.%d\n", (GetSdkVer() >> 8) & 0xFF, (GetSdkVer() >> 16) & 0xFF, GetSdkVer() >> 24);
	APP_DBG("Free Memory      :%d(%d KB)\n", OSMMMIoCtl(MMM_IOCTL_GETSZ_INALL, 0), OSMMMIoCtl(MMM_IOCTL_GETSZ_INALL, 0) / 1024);
	APP_DBG("Code Size        :%d(%d KB)\n", GetCodeSize(), GetCodeSize() / 1024);
	APP_DBG("Code Encrypted   :%s\n", GetCodeEncryptedInfo());
	APP_DBG("Wakeup Source    :%s(0x%08X)\n", GetWakeupSrcName(gWakeUpFlag), gWakeUpFlag);
	APP_DBG("BT Lib Ver       :%s\n", GetLibVersionBt());
	APP_DBG("AudioDecoder Ver :%s\n", GetLibVersionAudioDecoder());
	APP_DBG("Driver Ver       :%s\n", GetLibVersionDriver());
	APP_DBG("FreeRTOS Ver     :%s\n", GetLibVersionFreertos());
	APP_DBG("FS Ver           :%s\n", GetLibVersionFs());
	APP_DBG("****************************************************************\n");

	FlshBTInfoAreaInit();	//BT data aera init. do NOT move this
	
#ifdef	CFG_SHELL_DEBUG
	OSTaskCreate(ShellCommand, "SHELL", 720, NULL, 3, NULL);
#endif	//CFG_SHELL_DEBUG
	OSTaskCreate(DecoderTaskEntrance, "Decoder", 2048, NULL, 3, NULL);
	OSTaskCreate(GuiTaskEntrance, "MainTask", 1280, NULL, 2, &MainTaskHandle);
	OSTaskCreate(AudioProcessTaskEntrance, "AudioProcessTask", 1024, NULL, 3, &AudioProcessHandle);
#ifdef FUNC_BT_EN
	OSTaskCreate(BTEntrance, "BT", 1920, NULL, 3, &BtTaskHandle);
#endif
	
	//init timer0/1 interrupt
	NVIC_EnableIRQ(TMR1_IRQn);
	Timer1Set(1000);
	
#ifdef FUNC_DISP_EN
#if defined(FUNC_SINGLE_LED_EN)
	gDisplayMode = DISP_DEV_SLED;
#elif defined(FUNC_SEG_LCD_EN)
	gDisplayMode = DISP_DEV_LCD58;
#else
	gDisplayMode = DISP_DEV_LED1888;
#endif
	DispInit(FALSE);
#endif

	DBG("Start Detect External Device(Keypad, U disk, SD card, FM,...)\n");
	OSQueueMsgIOCtl(MSGQ_IOCTL_DEL_PENDMSG, 0);
#ifndef FUNC_WATCHDOG_EN
	WdgDis();				// disable watch dog
#endif

//UART从机功能初始化
#ifdef FUNC_SLAVE_UART_EN
  Slave_UartInit();
#endif	
	while(1)
	{
		//feed watch dog ever 10 ms
#ifdef FUNC_WATCHDOG_EN
		WdgFeed();
#endif
#ifdef FUNC_POWER_MONITOR_EN
		//执行电池电压检测、充电设备接入检测及其附属的显示和系统关闭处理等
		PowerMonitor();
#endif
#ifdef FUNC_DISP_EN
		Display();
#endif
#ifdef FUNC_SPI_UPDATE_EN
		if(UpgradeFileFound)
		{
			//upgrade file found,try to upgrade it
			BootUpgradeChk();
		}
#endif
		MsgRecv(10);    
#if (defined(FUNC_ADC_KEY_EN) || defined(FUNC_IR_KEY_EN) || defined(FUNC_CODING_KEY_EN))
		KeyScan();
#endif
#ifdef MASS_PRODUCTION_TEST_FUNC
		if(GetMassTestFlag())
		{
			continue;
		}
#endif
		DeviceDetect(); // 设备检测接口
		
#if defined(FUNC_AMP_MUTE_EN) && defined(AMP_SILENCE_MUTE_EN)
    if(!GetSilenceMuteFlag()
#ifdef FUNC_SOUND_REMIND
			 || IsSoundRemindPlaying()
#endif
#ifdef FUNC_BT_HF_EN
		   || (GetHfTransferState() == TRUE)	
#endif
		  )
	  {
		  AmpMuteControl(0);
	  #ifdef FUNC_SLEEP_EN
		  gSys.SleepTime = 0;
			gSys.SleepStartPowerOff = FALSE;
	  #endif
	  }
	  else
	  {
		  AmpMuteControl(1);
	  #ifdef FUNC_SLEEP_EN
		  if(gSys.SleepTime == FALSE)
		  {
			  gSys.SleepTime = SLEEP_POWEROFF_TMR;
			  gSys.SleepTimeCnt = 0;
		  }
	  #endif
	  }
#endif

//注意：原定时关机功能是在定时器1中处理，因定时器中断处理过多代码会出现
//蓝牙协议栈阻塞，所以将定时关机改到主任务中处理。
#ifdef FUNC_SLEEP_EN
	  if(gSys.SleepTime != 0)
	  {
		  if(gSys.SleepTimeCnt > 6000*gSys.SleepTime)
		  {
			   if(gSys.NextModuleID != MODULE_ID_POWEROFF)	
			   //MSG_COMMON_CLOSE only need send once
			   {
				   //gSys.NextModuleID = MODULE_ID_POWEROFF;
				   gSys.SleepStartPowerOff = TRUE;
					 gSys.SleepTime = 0;
					 gSys.SleepTimeCnt = 0;
					 //gSys.NextModuleID = MODULE_ID_IDLE;
				   //MsgSend(MSG_COMMON_CLOSE);
			   }
		  }
		  else
		  {
			   gSys.SleepTimeCnt++;
		  }
	  }
#endif

#ifdef FUNC_SLAVE_UART_EN
		Slave_CmdProcess();
#endif		
	}
}


