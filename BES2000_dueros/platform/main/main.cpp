#include "plat_addr_map.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_wdt.h"
#include "hal_sleep.h"
#include "hal_bootmode.h"
#include "hal_trace.h"
#include "cmsis.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "analog.h"
#include "apps.h"
#include "hal_gpio.h"
#include "tgt_hardware.h"
#ifdef __ANKER_UI__
#include "bes_hal_board.h"
#endif
#include "hal_norflash.h"
#ifdef RTOS
#include "cmsis_os.h"
#include "rt_Time.h"
#include "app_factory.h"
#endif


#define  system_shutdown_wdt_config(seconds) \
                        do{ \
                            hal_wdt_stop(HAL_WDT_ID_0); \
                            hal_wdt_set_irq_callback(HAL_WDT_ID_0, NULL); \
                            hal_wdt_set_timeout(HAL_WDT_ID_0, seconds); \
                            hal_wdt_start(HAL_WDT_ID_0); \
                            hal_sleep_set_lowpower_hook(HAL_SLEEP_HOOK_USER_0, NULL); \
                        }while(0)

static osThreadId main_thread_tid = NULL;

int system_shutdown(void)
{
    TRACE("%s: SHUTDOWN",__func__);
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x4);
    return 0;
}

int system_reset(void)
{
    TRACE("%s: RESET",__func__);
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x8);
    return 0;
}

int tgt_hardware_setup(void)
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_pinmux_pwl, sizeof(cfg_hw_pinmux_pwl)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    if (app_battery_ext_charger_indicator_cfg.pin != HAL_IOMUX_PIN_NUM){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_battery_ext_charger_indicator_cfg, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_indicator_cfg.pin, HAL_GPIO_DIR_OUT, 0);
    }
    return 0;
}

#if defined(CHIP_BEST1000) && defined(__SBC_FUNC_IN_ROM__)
SBC_ROM_STRUCT SBC_ROM_FUNC =
{
    .SBC_FrameLen = (__SBC_FrameLen )0x000085f5,
    .SBC_InitDecoder = (__SBC_InitDecoder )0x000086ad,
    .SbcSynthesisFilter4 = (__SbcSynthesisFilter4 )0x000086b9,
    .SbcSynthesisFilter8 = (__SbcSynthesisFilter8 )0x00008f0d,
    .SBC_DecodeFrames = (__SBC_DecodeFrames )0x0000a165,
    .SBC_DecodeFrames_Out_SBSamples = (__SBC_DecodeFrames_Out_SBSamples )0x0000a4e1

};
#endif

#ifdef FPGA
uint32_t a2dp_audio_more_data(uint8_t *buf, uint32_t len);
uint32_t a2dp_audio_init(void);
extern "C" void app_audio_manager_open(void);
extern "C" void app_bt_init(void);
extern "C" uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count);
void app_overlay_open(void);

extern "C" void BesbtInit(void);
extern "C" int app_os_init(void);
extern "C" uint32_t af_open(void);
extern "C" int list_init(void);
extern "C" void app_audio_open(void);
volatile uint32_t ddddd = 0;

#if defined(AAC_TEST)
#include "app_overlay.h"
int decode_aac_frame_test(unsigned char *pcm_buffer, unsigned int pcm_len);
#define AAC_TEST_PCM_BUFF_LEN (4096)
unsigned char aac_test_pcm_buff[AAC_TEST_PCM_BUFF_LEN];
#endif

#endif

#ifdef SYSTEM_USE_PSRAM
#include "hal_location.h"
extern "C" void system_psram_init(void);
PSRAM_DATA_LOC const int psram_test[] = {0x22,0x33,0x44,0x55,0xfe,0xef,0x78,0x79} ;
#endif


int main(int argc, char *argv[])
{
    uint8_t sys_case = 0;
    int ret = 0;
    uint32_t bootmode = 0;

    bootmode = hal_sw_bootmode_get();
    

    tgt_hardware_setup();
    
    main_thread_tid = osThreadGetId();

    hwtimer_init();

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();

#ifdef DEBUG
#if (DEBUG_PORT == 1)
#ifndef __PC_CMD_UART__
    hal_iomux_set_uart0();
#endif
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#endif

    TRACE("%s 1111111111",__func__);
#if (DEBUG_PORT == 2)
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_iomux_set_analog_i2c();
    }
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#endif
    hal_sleep_start_stats(10000, 10000);
#endif
    hal_iomux_ispi_access_init();
    TRACE("%s 22222222",__func__);
    pmu_open();
    TRACE("%s 3333333",__func__);
    analog_open();
    TRACE("%s 4444444",__func__);
// with OTA support 
#if defined(FLASH_REGION_BASE) && FLASH_REGION_BASE > FLASH_BASE
	// as OTA bin will load the factory data and user data from the bottom TWO sectors from the flash,
	// the FLASH_SIZE defined is the common.mk must be the actual chip flash size, otherwise the ota will load the 
	// wrong information
	uint32_t actualFlashSize = hal_norflash_get_flash_total_size();
	if (FLASH_SIZE != actualFlashSize)
	{
		TRACE("Wrong FLASH_SIZE defined in common.mk!");
		TRACE("FLASH_SIZE is defined as 0x%x while the actual chip flash size is 0x%x!", FLASH_SIZE, actualFlashSize);
		TRACE("Please change the FLASH_SIZE in common.mk to 0x%x to enable the OTA feature.", actualFlashSize);
		ASSERT(false, "");
	}
#endif
    TRACE("%s 5555555",__func__);
#ifdef SYSTEM_USE_PSRAM
	system_psram_init();
	TRACE("psram_test = 0x%x \n", psram_test);
	DUMP32(" 0x%x", psram_test, sizeof(psram_test)/4);
#endif
    TRACE("%s 66666666",__func__);
#ifdef __ANKER_UI__
    hal_board_pre_init();
#endif
#ifdef FPGA

    TRACE("\n[best of best of best...]\n");
    TRACE("\n[ps: w4 0x%x,2]", &ddddd);

    ddddd = 1;
    while (ddddd == 1);
    TRACE("bt start");

    list_init();

    app_os_init();
    app_bt_init();
    a2dp_audio_init();

    af_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();

    BesbtInit();

#if defined(AAC_TEST)
    app_overlay_select(APP_OVERLAY_A2DP_AAC);
    decode_aac_frame_test(aac_test_pcm_buff, AAC_TEST_PCM_BUFF_LEN);
#endif

    SAFE_PROGRAM_STOP();

#else // !FPGA

#ifdef __FACTORY_MODE_SUPPORT__
    if (bootmode & HAL_SW_BOOTMODE_FACTORY){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_FACTORY);
        ret = app_factorymode_init(bootmode);

    }else if(bootmode & HAL_SW_BOOTMODE_CALIB){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CALIB);
        app_factorymode_calib_only();
    }else
#endif
    {
        TRACE("%s 7777777",__func__);
        ret = app_init();
        TRACE("%s: app_init return %d", __func__,ret);
    }

    if (!ret){
        while(1)
        {
            osEvent evt;
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
            osSignalClear (main_thread_tid, 0x0f);
#endif
            //wait any signal
            evt = osSignalWait(0x0, osWaitForever);

            //get role from signal value
            if(evt.status == osEventSignal)
            {
                if(evt.value.signals & 0x04)
                {
                    sys_case = 1;
                    break;
                }
                else if(evt.value.signals & 0x08)
                {
                    sys_case = 2;
                    break;
                }
            }else{
                sys_case = 1;
                break;
            }
         }
    }
#ifdef __WATCHER_DOG_RESET__
    system_shutdown_wdt_config(10);
#endif
    app_deinit(ret);
    TRACE("byebye~~~ %d", sys_case);
    if ((sys_case == 1)||(sys_case == 0)){
        TRACE("shutdown");
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        pmu_shutdown();
    }else if (sys_case == 2){
        TRACE("reset");
        hal_cmu_sys_reboot();
    }

#endif // !FPGA

    return 0;
}

