#include "plat_addr_map.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_iomuxip.h"
#include "hal_gpio.h"
#include "hal_wdt.h"
#include "hal_sleep.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_cache.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "analog.h"

#include "bt_drv_interface.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"

// for init
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_pwl.h"
#include "app_overlay.h"
#include "app_battery.h"
#include "app_utils.h"

// for audio
#include "resources.h"
#include "audioflinger.h"
#include "app_audio.h"
#include "app_utils.h"
#include "app_bt_stream.h"
#include "app_media_player.h"

// for progress
#include "hal_uart.h"
#include "tool_msg.h"

#include "cmsis_nvic.h"

#define  system_shutdown_wdt_config(seconds) \
                        do{ \
                            hal_wdt_stop(HAL_WDT_ID_0); \
                            hal_wdt_set_irq_callback(HAL_WDT_ID_0, NULL); \
                            hal_wdt_set_timeout(HAL_WDT_ID_0, seconds); \
                            hal_wdt_start(HAL_WDT_ID_0); \
                            hal_sleep_set_lowpower_hook(HAL_SLEEP_HOOK_USER_0, NULL); \
                        }while(0)

#define REG_SET_FIELD(reg, mask, shift, v)\
                        do{ \
                            *(volatile unsigned int *)(reg) &= ~(mask<<shift); \
                            *(volatile unsigned int *)(reg) |= (v<<shift); \
                        }while(0)

static osThreadId main_thread_tid = NULL;

int system_shutdown(void)
{
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x4);
    return 0;
}

int system_reset(void)
{
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x8);
    return 0;
}

int tgt_hardware_setup(void)
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_pinmux_pwl, sizeof(cfg_hw_pinmux_pwl)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    return 0;
}

#define WAIT_TRACE_TIMEOUT                      MS_TO_TICKS(200)

typedef void (*FLASH_ENTRY)(void);

#define EXEC_CODE_BASE (FLASH_BASE+200*0x400)

static const struct boot_struct_t * const boot_struct =
    (struct boot_struct_t *)EXEC_CODE_BASE;

extern "C" void BootSelect(void)
{
	FLASH_ENTRY entry;

    // Disable all IRQs
    NVIC_DisableAllIRQs();
    // Ensure in thumb state
	entry = (FLASH_ENTRY)FLASH_TO_FLASHX(&boot_struct->hdr + 1);
    entry = (FLASH_ENTRY)((uint32_t)entry | 1);
    entry();
}

static void wait_trace_finished(void)
{
    uint32_t time;
    int idle_cnt = 0;

    time = hal_sys_timer_get();

    while (idle_cnt < 2 && hal_sys_timer_get() - time < WAIT_TRACE_TIMEOUT) {
        hal_sys_timer_delay(10);
        idle_cnt = hal_trace_busy() ? 0 : (idle_cnt + 1);
    }
}

int ota_init(void)
{
    int nRet = 0;
    uint32_t nLan;

    TRACE("ota_init");

#ifdef __WATCHER_DOG_RESET__
    app_wdt_open(60);
#endif
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_52M);
    app_os_init();
    app_battery_open();
    if (app_key_open(false)){
        nRet = -1;
        goto exit;
    }
exit:
    return nRet;
}

int ota_deinit(void)
{
	return 0;
}
int main(int argc, char *argv[])
{
    uint8_t sys_case = 0;
    int ret = 0;
    uint32_t bootmode = hal_sw_bootmode_get();

    tgt_hardware_setup();

    main_thread_tid = osThreadGetId();

    hal_sys_timer_open();
    hwtimer_init();

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();

#ifdef DEBUG
#if (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#endif

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
#endif

    hal_iomux_ispi_access_init();

    pmu_open();

    analog_open();

    hal_cmu_simu_init();

    ret = ota_init();

    if (!ret){
        while(1)
        {
            osEvent evt;
            osSignalClear (main_thread_tid, 0x0f);
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
    ota_deinit();
    TRACE("byebye~~~ %d\n", sys_case);
    if ((sys_case == 1)||(sys_case == 0)){
        TRACE("shutdown\n");
        pmu_shutdown();
    }else if (sys_case == 2){
        TRACE("reset\n");
        hal_cmu_sys_reboot();
    }
    return 0;
}

