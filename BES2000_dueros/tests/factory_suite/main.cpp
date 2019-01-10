#include "cmsis_os.h"
#include "cmsis.h"
#include "list.h"
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_iomux.h"

#include "hal_gpio.h"
#include "hal_wdt.h"
#include "hal_sleep.h"
#include "hal_bootmode.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "analog.h"

#include "bt_drv_interface.h"
#include "bt_drv.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"

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


extern "C" void app_audio_manager_open(void);
extern void bt_drv_check_calib(void);
static void factory_suite_timehandler(void const *param);


osTimerId factory_suite_timer = NULL;
osTimerDef (factory_suite_TIMER, factory_suite_timehandler);
static struct message_t send_msg = { { PREFIX_CHAR, }, };
static unsigned char send_seq = 0;

static unsigned char factory_suite_msg_check_sum(unsigned char *buf, unsigned char len)
{
    int i;
    unsigned char sum = 0;

    for (i = 0; i < len; i++) {
        sum += buf[i];
    }

    return sum;
}

static int factory_suite_msg_uart_send(const unsigned char *buf, size_t len)
{
    uint32_t sent = 0;

    while (sent < len) {
        hal_uart_blocked_putc(HAL_UART_ID_0, buf[sent++]);
    }

    if (sent != len) {
        return 1;
    }

    return 0;
}

static int factory_suite_uart_init(void)
{
    struct HAL_UART_CFG_T uart_cfg;

    memset(&uart_cfg, 0, sizeof(struct HAL_UART_CFG_T));
    uart_cfg.parity = HAL_UART_PARITY_NONE,
    uart_cfg.stop = HAL_UART_STOP_BITS_1,
    uart_cfg.data = HAL_UART_DATA_BITS_8,
    uart_cfg.flow = HAL_UART_FLOW_CONTROL_NONE,//HAL_UART_FLOW_CONTROL_RTSCTS,
    uart_cfg.tx_level = HAL_UART_FIFO_LEVEL_1_2,
    uart_cfg.rx_level = HAL_UART_FIFO_LEVEL_1_4,
    uart_cfg.baud = 921600,
    uart_cfg.dma_rx = false,
    uart_cfg.dma_tx = false,
    uart_cfg.dma_rx_stop_on_err = false;
    hal_uart_close(HAL_UART_ID_0);
    hal_uart_open(HAL_UART_ID_0, &uart_cfg);

    return 0;
}

static int factory_suite_alive(void)
{
    int ret;

    send_msg.hdr.type = 0x88;
    send_msg.hdr.seq = send_seq++;
    send_msg.hdr.len = 2;
    send_msg.data[0] = 0xaa;
    send_msg.data[1] = 0x55;
    send_msg.data[2] = ~factory_suite_msg_check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = factory_suite_msg_uart_send((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));

    return ret;
}

static void factory_suite_timehandler(void const *param)
{
    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODUAL_OHTER;
    app_mailbox_put(&msg);
}

static int factory_suite_process(APP_MESSAGE_BODY *msg_body)
{
    factory_suite_alive();
    return 0;
}

#ifdef __FACTORY_SUITE_CONNECTOR__
extern "C" void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param);
#endif

static uint8_t factory_suite_indication_init(void)
{
    struct APP_PWL_CFG_T cfg0;
    struct APP_PWL_CFG_T cfg1;
    app_pwl_open();
    cfg0.part[0].level = 0;
    cfg0.part[0].time = (500);
    cfg0.part[1].level = 1;
    cfg0.part[1].time = (500);
    cfg0.parttotal = 2;
    cfg0.startlevel = 0;
    cfg0.periodic = true;

    cfg1.part[0].level = 1;
    cfg1.part[0].time = (500);
    cfg1.part[1].level = 0;
    cfg1.part[1].time = (500);
    cfg1.parttotal = 2;
    cfg1.startlevel = 1;
    cfg1.periodic = true;

    app_pwl_setup(APP_PWL_ID_0, &cfg0);
    app_pwl_start(APP_PWL_ID_0);
    app_pwl_setup(APP_PWL_ID_1, &cfg1);
    app_pwl_start(APP_PWL_ID_1);

    return 0;
}

int factory_suite_loop(void)
{
    int nRet = 0;
	struct nvrecord_env_t *nvrecord_env;
    TRACE("factory_suite");

#ifdef RANDOM_BTADDR_GEN
    dev_addr_name devinfo;
#endif

    factory_suite_uart_init();
#ifdef __WATCHER_DOG_RESET__
    app_wdt_open(60);
#endif
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_52M);
    list_init();
    app_os_init();
    app_battery_open();
    if (app_key_open(false)){
        nRet = -1;
        goto exit;
    }
    app_set_threadhandle(APP_MODUAL_OHTER, factory_suite_process);
    factory_suite_timer = osTimerCreate (osTimer(factory_suite_TIMER), osTimerPeriodic, NULL);
    osTimerStart(factory_suite_timer, 1000);

    af_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();

    factory_suite_indication_init();

    nv_record_env_init();
    nvrec_dev_data_open();
	nv_record_env_get(&nvrecord_env);
    app_play_audio_set_lang(nvrecord_env->media_language.language);

#ifdef RANDOM_BTADDR_GEN
    devinfo.btd_addr = bt_addr;
    devinfo.ble_addr = ble_addr;
    devinfo.localname = BT_LOCAL_NAME;
    nvrec_dev_localname_addr_init(&devinfo);
#endif
    app_voice_report(APP_STATUS_INDICATION_POWERON, 0);

    btdrv_testmode_start();
    osDelay(1000);

#ifdef __FACTORY_SUITE_CALIB__
    bt_drv_calib_open();
    bt_drv_check_calib();
#endif

#ifdef __FACTORY_SUITE_CONNECTOR__
    btdrv_hciopen();
    while(1){
        uint8_t i = 0;
        btdrv_hci_reset();
        osDelay(1000);
        btdrv_send_cmd(HCI_DBG_SET_BD_ADDR_CMD_OPCODE, sizeof(bt_addr), bt_addr);
        osDelay(100);
        btdrv_enable_autoconnector();
        for (i=0; i<200; i++){
            osDelay(1000);
        }
    }
#endif

    SAFE_PROGRAM_STOP();
exit:
    return nRet;
}


int tgt_hardware_setup(void)
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_pinmux_pwl, sizeof(cfg_hw_pinmux_pwl)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    return 0;
}

int system_shutdown(void)
{
    return 0;
}

int system_reset(void)
{
    return 0;
}


int main(int argc, char *argv[])
{
    int ret;
    uint32_t bootmode = hal_sw_bootmode_get();

    tgt_hardware_setup();

    hal_sys_timer_open();
    hwtimer_init();

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();


#if defined( DEBUG)
#if (DEBUG_PORT == 0)
    #error "HAL_TRACE_TRANSPORT_USB NOT SUPPORT"
#elif (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#elif (DEBUG_PORT == 2)
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

    ret = hal_analogif_open();
    if (ret)
        TRACE("hal_analogif_open failed \n");

    pmu_open();

    analog_open();

    hal_cmu_simu_init();

    factory_suite_loop();

    pmu_shutdown();

    return 0;
}

