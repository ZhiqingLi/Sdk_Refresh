#include "stdio.h"
#include "cmsis_os.h"
#include "list.h"
#include "string.h"

#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"

#include "audioflinger.h"
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_pwl.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "app_battery.h"
#include "app_utils.h"
#include "app_status_ind.h"
#ifdef __FACTORY_MODE_SUPPORT__
#include "app_factory.h"
#include "app_factory_bt.h"
#endif
#include "bt_drv_interface.h"
#include "besbt.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"

extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
}
#include "btalloc.h"
#include "btapp.h"
#include "app_bt.h"
#include "pmu.h"
#ifdef MEDIA_PLAYER_SUPPORT
#include "resources.h"
#include "app_media_player.h"
#endif
#include "app_bt_media_manager.h"
#include "hal_sleep.h"
#if defined(__SMARTVOICE__)
#include "app_sv_handle.h"
#endif
#if defined(__AMA_VOICE__)
#include "app_ama_handle.h"
#include "communication_svr.h"
#endif
#if defined(__DMA_VOICE__)
#include "app_dma_handle.h"
#include "app_dma_bt.h"
#include "app_dma_info.h" // add by cai.zhong 20190105 for get Customized information
#endif
#include "app_ai_voice.h"

#if defined(BTUSB_AUDIO_MODE)
extern "C"	bool app_usbaudio_mode_on(void) ;
#endif
#ifdef __BT_ANC__
#include "app_anc.h"
#endif

#if OTA_ENABLED
#include "ota_handler.h"
#endif

#ifdef __THIRDPARTY
#include "app_thirdparty.h"

#ifdef CHIP_BEST2000
// TODO: To be removed
#include "pmu.h"

int app_efuse_get_handler(int page, unsigned short *efuse)
{
    if(page == 0xE){
        *efuse = app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_GET_PAGE_E_EFUSE);
        return 0;
    }

    if(page == 0xF){
        *efuse = app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_GET_PAGE_F_EFUSE);
        return 0;
    }

    return 1;
}
#endif
#endif

#define APP_BATTERY_LEVEL_LOWPOWERTHRESHOLD (1)

#ifdef MEDIA_PLAYER_RBCODEC
extern int rb_ctl_init();
extern bool rb_ctl_is_init_done(void);
extern void app_rbplay_audio_reset_pause_status(void);
#endif

#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
extern bool app_pwr_key_monitor_get_val(void);
static bool anc_single_mode_on = false;
extern "C" bool anc_single_mode_is_on(void)
{
	return anc_single_mode_on;
}
#endif

#ifdef __ANC_STICK_SWITCH_USE_GPIO__
extern bool app_anc_switch_get_val(void);
#endif
uint8_t  app_poweroff_flag = 0;

#ifndef APP_TEST_MODE
static uint8_t app_status_indication_init(void)
{
    struct APP_PWL_CFG_T cfg;
    memset(&cfg, 0, sizeof(struct APP_PWL_CFG_T));
    app_pwl_open();
    app_pwl_setup(APP_PWL_ID_0, &cfg);
    app_pwl_setup(APP_PWL_ID_1, &cfg);
    return 0;
}
#endif

#if defined(__EARPHONE__) && defined(__AUTOPOWEROFF__)

void PairingTransferToConnectable(void);

typedef void (*APP_10_SECOND_TIMER_CB_T)(void);

void app_pair_timerout(void);
void app_poweroff_timerout(void);
void CloseEarphone(void);

typedef struct
{
    uint8_t timer_id;
    uint8_t timer_en;
    uint8_t timer_count;
    uint8_t timer_period;
    APP_10_SECOND_TIMER_CB_T cb;
}APP_10_SECOND_TIMER_STRUCT;



APP_10_SECOND_TIMER_STRUCT app_pair_timer =
{
    .timer_id =APP_PAIR_TIMER_ID,
    .timer_en = 0,
    .timer_count = 0,
    .timer_period = 18,
    .cb =    PairingTransferToConnectable
};

APP_10_SECOND_TIMER_STRUCT app_poweroff_timer=
{
    .timer_id =APP_POWEROFF_TIMER_ID,
    .timer_en = 0,
    .timer_count = 0,
    .timer_period = 30,
    .cb =    CloseEarphone
};

APP_10_SECOND_TIMER_STRUCT  *app_10_second_array[2] =
{
    &app_pair_timer,
    &app_poweroff_timer,
};



void app_stop_10_second_timer(uint8_t timer_id)
{
    APP_10_SECOND_TIMER_STRUCT *timer = app_10_second_array[timer_id];
    timer->timer_en = 0;
    timer->timer_count = 0;
}

void app_start_10_second_timer(uint8_t timer_id)
{
    APP_10_SECOND_TIMER_STRUCT *timer = app_10_second_array[timer_id];

    timer->timer_en = 1;
    timer->timer_count = 0;
}

void app_10_second_timerout_handle(uint8_t timer_id)
{
    APP_10_SECOND_TIMER_STRUCT *timer = app_10_second_array[timer_id];

    timer->timer_en = 0;
    timer->cb();
}




void app_10_second_timer_check(void)
{
    uint8_t i;
    for(i=0;i<sizeof(app_10_second_array)/sizeof(app_10_second_array[0]);i++)
    {
        if(app_10_second_array[i]->timer_en)
        {
            app_10_second_array[i]->timer_count++;
            if(app_10_second_array[i]->timer_count >= app_10_second_array[i]->timer_period)
            {
                app_10_second_timerout_handle(i);
            }
        }

    }
}
#endif

#if (HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_BATTERY_REPORT) || (HF_SDK_FEATURES & HF_FEATURE_HF_INDICATORS)
extern void app_hfp_set_battery_level(uint8_t level);
#endif

int app_status_battery_report(uint8_t level)
{
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    if( anc_single_mode_on )//anc power on,anc only mode
        return 0;
#endif
#if defined(__EARPHONE__)
    app_bt_state_checker();
    app_10_second_timer_check();
#endif
    if (level<=APP_BATTERY_LEVEL_LOWPOWERTHRESHOLD){
        //add something
    }
#if (HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_BATTERY_REPORT) || (HF_SDK_FEATURES & HF_FEATURE_HF_INDICATORS)
    app_hfp_set_battery_level(level);
#else
    TRACE("[%s] Can not enable HF_CUSTOM_FEATURE_BATTERY_REPORT",__func__);
#endif
    OS_NotifyEvm();
    return 0;
}

#ifdef MEDIA_PLAYER_SUPPORT

void app_status_set_num(const char* p)
{
    media_Set_IncomingNumber(p);
}



extern "C" int app_voice_report(APP_STATUS_INDICATION_T status,uint8_t device_id)
{
#if defined(BTUSB_AUDIO_MODE)
    if(app_usbaudio_mode_on()) return 0;
#endif
    TRACE("%s %d",__func__, status);
    AUD_ID_ENUM id = MAX_RECORD_NUM;
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    if(anc_single_mode_on)
        return 0;
#endif
    if (app_poweroff_flag == 1){
        switch (status) {
            case APP_STATUS_INDICATION_POWEROFF:
                id = AUD_ID_POWER_OFF;
                break;    
            default:
                return 0;                
                break;    
        }
    }else{
        switch (status) {
            case APP_STATUS_INDICATION_POWERON:
                id = AUD_ID_POWER_ON;
                break;
            case APP_STATUS_INDICATION_POWEROFF:
                id = AUD_ID_POWER_OFF;
                break;    
            case APP_STATUS_INDICATION_CONNECTED:
                id = AUD_ID_BT_CONNECTED;
                break;
            case APP_STATUS_INDICATION_DISCONNECTED:
                id = AUD_ID_BT_DIS_CONNECT;              
                break;    
            case APP_STATUS_INDICATION_CALLNUMBER:
                id = AUD_ID_BT_CALL_INCOMING_NUMBER;         
                break;  
            case APP_STATUS_INDICATION_CHARGENEED:
                id = AUD_ID_BT_CHARGE_PLEASE;   
                break;
            case APP_STATUS_INDICATION_FULLCHARGE:
                id = AUD_ID_BT_CHARGE_FINISH;
                break;  
            case APP_STATUS_INDICATION_PAIRSUCCEED:
                id = AUD_ID_BT_PAIRING_SUC;
                break;  
            case APP_STATUS_INDICATION_PAIRFAIL:
                id = AUD_ID_BT_PAIRING_FAIL;
                break;      
                
            case APP_STATUS_INDICATION_HANGUPCALL:
                id = AUD_ID_BT_CALL_HUNG_UP;
                break;  
                
            case APP_STATUS_INDICATION_REFUSECALL:
                id = AUD_ID_BT_CALL_REFUSE; 
                break;  
                
            case APP_STATUS_INDICATION_ANSWERCALL:
                id = AUD_ID_BT_CALL_ANSWER; 
                break;  
                
            case APP_STATUS_INDICATION_CLEARSUCCEED:
                id = AUD_ID_BT_CLEAR_SUCCESS;
                break;      
                
            case APP_STATUS_INDICATION_CLEARFAIL:
                id = AUD_ID_BT_CLEAR_FAIL;  
                break;
            case APP_STATUS_INDICATION_INCOMINGCALL:
                id = AUD_ID_BT_CALL_INCOMING_CALL;  
                break;
            case APP_STATUS_INDICATION_BOTHSCAN:            
                id = AUD_ID_BT_PAIR_ENABLE;
                break;
            case APP_STATUS_INDICATION_WARNING:
                id = AUD_ID_BT_WARNING;
                break; //add by cai.zhong 20181114 for voice bug
            case APP_STATUS_INDICATION_ALEXA_START:
                id = AUDIO_ID_BT_ALEXA_START;
                break;
            case APP_STATUS_INDICATION_ALEXA_STOP:
                id = AUDIO_ID_BT_ALEXA_STOP;
                break;
			//add begin by cai.zhong 20181114 for dueros voice
            case APP_STATUS_INDICATION_ILLEGAL_DEVICE:
                id = AUDIO_ID_BT_ILLEGAL_DEVICE;
                break;			
            case APP_STATUS_INDICATION_DUEROS_START:
                id = AUDIO_ID_BT_DUEROS_START;
                break;
			//add end by cai.zhong 20181114 for dueros voice
            default:
                break;
        }
    }
    
    media_PlayAudio(id, device_id);  

    return 0;
}
#endif


#define POWERON_PRESSMAXTIME_THRESHOLD_MS  (5000)

enum APP_POWERON_CASE_T {
    APP_POWERON_CASE_NORMAL = 0,
    APP_POWERON_CASE_DITHERING,
    APP_POWERON_CASE_REBOOT,
    APP_POWERON_CASE_ALARM,
    APP_POWERON_CASE_CALIB,
    APP_POWERON_CASE_BOTHSCAN,
    APP_POWERON_CASE_CHARGING,
    APP_POWERON_CASE_FACTORY,
    APP_POWERON_CASE_TEST,
    APP_POWERON_CASE_INVALID,

    APP_POWERON_CASE_NUM
};


static osThreadId apps_init_tid = NULL;

static enum APP_POWERON_CASE_T pwron_case = APP_POWERON_CASE_INVALID;

static void app_poweron_normal(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
    pwron_case = APP_POWERON_CASE_NORMAL;

    osSignalSet(apps_init_tid, 0x2);
}

static void app_poweron_scan(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
    pwron_case = APP_POWERON_CASE_BOTHSCAN;

    osSignalSet(apps_init_tid, 0x2);
}

#ifdef __ENGINEER_MODE_SUPPORT__
static void app_poweron_factorymode(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    app_factorymode_enter();
}
#endif

static void app_poweron_finished(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
    osSignalSet(apps_init_tid, 0x2);
}

void app_poweron_wait_finished(void)
{
    osSignalWait(0x2, osWaitForever);
}

#if  defined(__POWERKEY_CTRL_ONOFF_ONLY__)
void app_bt_key_shutdown(APP_KEY_STATUS *status, void *param);
const  APP_KEY_HANDLE  pwron_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},           "power on: shutdown"     , app_bt_key_shutdown, NULL},
};
#elif defined(__ENGINEER_MODE_SUPPORT__)
const  APP_KEY_HANDLE  pwron_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITUP},           "power on: normal"     , app_poweron_normal, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITLONGPRESS},    "power on: both scan"  , app_poweron_scan  , NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITLONGLONGPRESS},"power on: factory mode", app_poweron_factorymode  , NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITFINISHED},     "power on: finished"   , app_poweron_finished  , NULL},
};
#else
const  APP_KEY_HANDLE  pwron_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITUP},           "power on: normal"     , app_poweron_normal, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITLONGPRESS},    "power on: both scan"  , app_poweron_scan  , NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITFINISHED},     "power on: finished"   , app_poweron_finished  , NULL},
};
#endif

#ifndef APP_TEST_MODE
static void app_poweron_key_init(void)
{
    uint8_t i = 0;
    TRACE("%s",__func__);

    for (i=0; i<(sizeof(pwron_key_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&pwron_key_handle_cfg[i]);
    }
}

static uint8_t app_poweron_wait_case(void)
{
    uint32_t stime, etime;

#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
    pwron_case = APP_POWERON_CASE_NORMAL;
#else
    pwron_case = APP_POWERON_CASE_INVALID;

    stime = hal_sys_timer_get();
    osSignalWait(0x2, POWERON_PRESSMAXTIME_THRESHOLD_MS);
    etime = hal_sys_timer_get();
    TRACE("powon raw case:%d time:%d", pwron_case, TICKS_TO_MS(etime - stime));
#endif
    return pwron_case;

}
#endif

int system_shutdown(void);
int app_shutdown(void)
{
    system_shutdown();
    return 0;
}

int system_reset(void);
int app_reset(void)
{
    system_reset();
    return 0;
}

void app_bt_key_shutdown(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    app_reset();
#else
    app_shutdown();
#endif
}

void app_bt_key_enter_testmode(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s\n",__FUNCTION__);

    if(app_status_indication_get() == APP_STATUS_INDICATION_BOTHSCAN){
#ifdef __FACTORY_MODE_SUPPORT__
        app_factorymode_bt_signalingtest(status, param);
#endif
    }
}

void app_bt_key_enter_nosignal_mode(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s\n",__FUNCTION__);
    if(app_status_indication_get() == APP_STATUS_INDICATION_BOTHSCAN){
#ifdef __FACTORY_MODE_SUPPORT__
        app_factorymode_bt_nosignalingtest(status, param);
#endif
    }
}


void app_bt_singleclick(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
}

void app_bt_doubleclick(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
}

void app_power_off(APP_KEY_STATUS *status, void *param)
{
    TRACE("app_power_off\n");
}

extern "C" void OS_NotifyEvm(void);

#define PRESS_KEY_TO_ENTER_OTA_INTERVEL    (15000)          // press key 15s enter to ota
#define PRESS_KEY_TO_ENTER_OTA_REPEAT_CNT    ((PRESS_KEY_TO_ENTER_OTA_INTERVEL - 2000) / 500)
void app_otaMode_enter(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s",__func__);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
#ifdef __KMATE106__
    app_status_indication_set(APP_STATUS_INDICATION_OTA);
    app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
    osDelay(1200);
#endif
    hal_cmu_sys_reboot();
}

void app_dfu_key_handler(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s ",__func__);
	hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
	hal_sw_bootmode_set(HAL_SW_BOOTMODE_FORCE_USB_DLD);
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
	hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}	

void app_ota_key_handler(APP_KEY_STATUS *status, void *param)
{
    static uint32_t time = hal_sys_timer_get();
    static uint16_t cnt = 0;

    TRACE("%s %d,%d",__func__, status->code, status->event);

    if (TICKS_TO_MS(hal_sys_timer_get() - time) > 600) // 600 = (repeat key intervel)500 + (margin)100
        cnt = 0;
    else
        cnt++;

    if (cnt == PRESS_KEY_TO_ENTER_OTA_REPEAT_CNT) {
        app_otaMode_enter(NULL, NULL);
    }

    time = hal_sys_timer_get();
}



extern int app_ama_wake_up();

extern int app_factorymode_audioloop(bool on, enum APP_SYSFREQ_FREQ_T freq);
extern void hal_iomux_set_spi(void);
extern void hal_iomux_set_spilcd(void);
#if 0
#include "hal_spi.h"

void app_spi_test()
{
	hal_iomux_set_spilcd();

	struct HAL_SPI_CFG_T spi_cfg;
	uint32_t spi_data;
	uint32_t spi_cmd;

	spi_cfg.clk_delay_half = false;
	spi_cfg.clk_polarity = false;
	spi_cfg.slave = false;
	spi_cfg.dma_rx = false,
	spi_cfg.dma_tx = false,
	spi_cfg.rx_sep_line = false;
	spi_cfg.cs = 0;
	//spi_cfg.rate = 13000000;
	spi_cfg.rate = 130000;
	spi_cfg.rx_bits = 24;
	spi_cfg.tx_bits = 32;//28
	spi_cfg.rx_frame_bits = 32;

	hal_spilcd_open(&spi_cfg);

	spi_data = 0xa0010b02;
	hal_spilcd_send(&spi_data, 4);

}
#endif

#ifdef __DMA_VOICE__
extern "C" uint8_t is_sco_mode (void);
#endif

#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined (__DMA_VOICE__)
void ai_voice_wakeup(APP_KEY_STATUS *status, void *param)
{
#ifdef __SMARTVOICE__
    //app_smartvoice_switch_ble_stream_status();
	app_smartvoice_switch_spp_stream_status();
#endif

#ifdef __AMA_VOICE__
	app_ama_wake_up();
#endif

#ifdef __DMA_VOICE__
    if (is_sco_mode() == 0) {
        app_baidu_dma_wake_up();
    }
    else {
        TRACE("%s:in sco mode, ignore this actioin!", __func__);
    }
#endif

}

//for dsp trigger
int ai_voice_irq_wake_up()
{
	int ret=0;
#ifdef __SMARTVOICE__
	//ret=app_smartvoice_switch_ble_stream_status();
	ret=app_smartvoice_switch_spp_stream_status();
#endif

#ifdef __AMA_VOICE__
	ret=app_ama_wake_up();
#endif

#ifdef __DMA_VOICE__
	ret=app_baidu_dma_wake_up();
#endif
	return ret;
}
#endif



//extern "C" void app_dspg_init();

//extern "C" void mic_value_detect();
BtStatus LinkDisconnectDirectly(void);

void app_bt_key(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);
    switch(status->event)
    {
        case APP_KEY_EVENT_CLICK:
            TRACE("first blood!");
			//app_dspg_init();
			//return;
            break;
        case APP_KEY_EVENT_DOUBLECLICK:
            TRACE("double kill");
			//LinkDisconnectDirectly();
			//return;
			//mic_value_detect();
            break;
        case APP_KEY_EVENT_TRIPLECLICK:
            TRACE("triple kill");
            break;
        case APP_KEY_EVENT_ULTRACLICK:
            TRACE("ultra kill");
            break;
        case APP_KEY_EVENT_RAMPAGECLICK:
            TRACE("rampage kill!you are crazy!");
            break;
    }
#ifdef __FACTORY_MODE_SUPPORT__
    if (app_status_indication_get() == APP_STATUS_INDICATION_BOTHSCAN && (status->event == APP_KEY_EVENT_DOUBLECLICK)){
        app_factorymode_languageswitch_proc();
    }else
#endif
    {
#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
        if(!anc_single_mode_on)
#endif
        bt_key_send(status);
    }
}

#ifdef MEDIA_PLAYER_RBCODEC
extern bool app_rbcodec_check_hfp_active(void );
void app_switch_player_key(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);

    if(!rb_ctl_is_init_done()) {
        TRACE("rb ctl not init done");
        return ;
    }

    if( app_rbcodec_check_hfp_active() ) {
        app_bt_key(status,param);
        return;
    }

    app_rbplay_audio_reset_pause_status();

    if(app_rbplay_mode_switch()) {
        app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
        app_rbcodec_ctr_play_onoff(true);
    } else {
        app_rbcodec_ctr_play_onoff(false);
        app_voice_report(APP_STATUS_INDICATION_POWEROFF, 0);
    }
    return ;

}
#endif

void app_enter_paring_groupkey_keep_con(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);

    switch(status->event)
    {
        case APP_KEY_EVENT_CLICK:
        {
			//if((MEC(activeCons) == 2))
			//	return;
            app_voice_report(APP_STATUS_INDICATION_BOTHSCAN,0);
            app_bt_accessmode_set_req(BT_DEFAULT_ACCESS_MODE_PAIR);

            
        }
        break;
        
        default:

            break;

    }

}


#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
#if defined(__APP_KEY_FN_STYLE_A__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"bt play backward key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_LONGPRESS},"bt play forward key",app_bt_key, NULL},
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif

};
#else //#elif defined(__APP_KEY_FN_STYLE_B__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_REPEAT},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt play backward key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_REPEAT},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_UP},"bt play forward key",app_bt_key, NULL},
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif

};
#endif
#else
#if defined(__APP_KEY_FN_STYLE_A__)
#if defined(__EDIFIER_W285A__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
	{{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_CLICK},"bt function key",ai_voice_wakeup, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt play backward key",app_bt_key, NULL},
	{{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"bt play forward key",app_bt_key, NULL},
};
#elif defined(__BELON_EARPONE__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
	{{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},    
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_CLICK},"bt function key",ai_voice_wakeup, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt play backward key",app_bt_key, NULL},
	{{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"bt play forward key",app_bt_key, NULL},
};
#else // default project
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
#ifdef MEDIA_PLAYER_RBCODEC
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_switch_player_key, NULL},
#else
#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined (__DMA_VOICE__)
#ifdef HM_LIVE_UI
	{{APP_KEY_CODE_AMA,APP_KEY_EVENT_CLICK},"bt function key",ai_voice_wakeup, NULL},
#else
	{{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",ai_voice_wakeup, NULL},
#endif
#else
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},
#endif
#endif
#ifdef HM_LIVE_UI
	{{APP_KEY_CODE_FN4,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},
	{{APP_KEY_CODE_FN4,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
	{{APP_KEY_CODE_FN3,APP_KEY_EVENT_CLICK},"pair key",app_enter_paring_groupkey_keep_con, NULL},
#else
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"bt function key",app_bt_key, NULL},
#endif    
#if RAMPAGECLICK_TEST_MODE
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"bt function key",app_bt_key_enter_nosignal_mode, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"bt function key",app_bt_key_enter_testmode, NULL},
#endif
#ifdef __ANKER_UI__
	{{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"pair key",app_enter_paring_groupkey_keep_con, NULL},
	{{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"pair key",app_enter_paring_groupkey_keep_con, NULL},
#else
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt play backward key",app_bt_key, NULL},
#endif
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"bt mode src snk key",app_bt_key, NULL},
#endif
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"bt play forward key",app_bt_key, NULL},
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif
#ifdef __BT_ANC_KEY__
     {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt anc key",app_anc_key, NULL},
#endif
};
#endif // default project
#else //#elif defined(__APP_KEY_FN_STYLE_B__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"bt function key",app_bt_key_enter_nosignal_mode, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"bt function key",app_bt_key_enter_testmode, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_REPEAT},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt play backward key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_REPEAT},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt play forward key",app_bt_key, NULL},
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif
};
#endif
#endif

void app_key_init(void)
{
    uint8_t i = 0;
    TRACE("%s",__func__);
    
    app_key_handle_clear();
    for (i=0; i<(sizeof(app_key_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&app_key_handle_cfg[i]);
    }
}

void app_key_init_on_charging(void)
{
    uint8_t i = 0;
    const APP_KEY_HANDLE  key_cfg[] = {
        {{APP_KEY_CODE_PWR,APP_KEY_EVENT_REPEAT},"ota function key",app_ota_key_handler, NULL},
		{{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"bt function key",app_dfu_key_handler, NULL},
    };

    TRACE("%s",__func__);
    for (i=0; i<(sizeof(key_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&key_cfg[i]);
    }
}


#ifdef EXTERN_PA_ENABLE
void app_extern_pa_init()
{
    if (BT_EXTERN_PA_EN.pin != HAL_IOMUX_PIN_NUM){
		hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&BT_EXTERN_PA_EN, 1);
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)BT_EXTERN_PA_EN.pin, HAL_GPIO_DIR_OUT, 0);
	}
}
extern "C"
void app_extern_pa_onoff(bool onoff)
{
    if(onoff == false){
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)BT_EXTERN_PA_EN.pin);
    }else{
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)BT_EXTERN_PA_EN.pin);
    }
}
#endif


#ifdef __AUTOPOWEROFF__

void CloseEarphone(void)
{
    int activeCons;
    OS_LockStack();
    activeCons = MEC(activeCons);
    OS_UnlockStack();

#ifdef __BT_ANC__
    if(app_anc_work_status()) {
        app_poweroff_timer.timer_en = 1;
        app_poweroff_timer.timer_period = 30;
        return;
    }
#endif
    if(activeCons == 0)
    {
      TRACE("!!!CloseEarphone\n");
      app_shutdown();
    }
}
#endif /* __AUTOPOWEROFF__ */



void a2dp_suspend_music_force(void);

int app_deinit(int deinit_case)
{
    int nRet = 0;
    TRACE("%s case:%d",__func__, deinit_case);

#if defined(BTUSB_AUDIO_MODE)
    if(app_usbaudio_mode_on()) return 0;
#endif
    if (!deinit_case){
        app_poweroff_flag = 1;
#if defined(APP_LINEIN_A2DP_SOURCE)
        app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#endif
#if defined(APP_I2S_A2DP_SOURCE)
        app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_I2S_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#endif
        app_status_indication_filter_set(APP_STATUS_INDICATION_BOTHSCAN);
        app_audio_sendrequest((uint8_t)APP_BT_STREAM_NUM, (uint8_t)APP_BT_SETTING_CLOSEALL, 0);
        osDelay(500);
        LinkDisconnectDirectly();
        osDelay(500);
        app_status_indication_set(APP_STATUS_INDICATION_POWEROFF);
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_POWEROFF, 0);
#endif
        osDelay(5000);
        af_close();
#if FPGA==0
        nv_record_flash_flush();
#endif 
    }

    return nRet;
}

#ifdef APP_TEST_MODE
extern void app_test_init(void);
int app_init(void)
{
    int nRet = 0;
    //uint8_t pwron_case = APP_POWERON_CASE_INVALID;
    TRACE("%s",__func__);
    app_poweroff_flag = 0;

    apps_init_tid = osThreadGetId();
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_52M);
    list_init();
    af_open();
    app_os_init();
    app_pwl_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();
    if (app_key_open(true))
    {
        nRet = -1;
        goto exit;
    }

    app_test_init();
exit:
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
    return nRet;
}
#else
#define NVRAM_ENV_FACTORY_TESTER_STATUS_TEST_PASS (0xffffaa55)
int app_bt_connect2tester_init(void)
{
    BtDeviceRecord rec;
    BT_BD_ADDR tester_addr;
    uint8_t i;
    bool find_tester = false;
    struct nvrecord_env_t *nvrecord_env;
    btdevice_profile *btdevice_plf_p;
    nv_record_env_get(&nvrecord_env);

    if (nvrecord_env->factory_tester_status.status != NVRAM_ENV_FACTORY_TESTER_STATUS_DEFAULT)
        return 0;

    if (!nvrec_dev_get_dongleaddr(&tester_addr)){
        nv_record_open(section_usrdata_ddbrecord);
        for (i = 0; nv_record_enum_dev_records(i, &rec) == BT_STATUS_SUCCESS; i++) {
            if (!memcmp(rec.bdAddr.addr, tester_addr.addr, BD_ADDR_SIZE)){
                find_tester = true;
            }
        }
        if(i==0 && !find_tester){
            memset(&rec, 0, sizeof(BtDeviceRecord));
            memcpy(rec.bdAddr.addr, tester_addr.addr, BD_ADDR_SIZE);
            nv_record_add(section_usrdata_ddbrecord, &rec);
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(rec.bdAddr.addr);
            btdevice_plf_p->hfp_act = true;
            btdevice_plf_p->a2dp_act = true;
        }
        if (find_tester && i>2){
            nv_record_ddbrec_delete(&tester_addr);
            nvrecord_env->factory_tester_status.status = NVRAM_ENV_FACTORY_TESTER_STATUS_TEST_PASS;
            nv_record_env_set(nvrecord_env);
        }
    }

    return 0;
}

int app_nvrecord_rebuild(void)
{
    struct nvrecord_env_t *nvrecord_env;
    nv_record_env_get(&nvrecord_env);

    nv_record_sector_clear();
    nv_record_open(section_usrdata_ddbrecord);
    nvrecord_env->factory_tester_status.status = NVRAM_ENV_FACTORY_TESTER_STATUS_TEST_PASS;
    nv_record_env_set(nvrecord_env);
    nv_record_flash_flush();

    return 0;
}

#if defined(BTUSB_AUDIO_MODE)
#include "app_audio.h"
#include "usb_audio_frm_defs.h"
#include "usb_audio_app.h"

static bool app_usbaudio_mode = false;

extern "C" void btusbaudio_entry(void);
void app_usbaudio_entry(void)
{
    app_usbaudio_mode = true ;

    btusbaudio_entry();
}

bool app_usbaudio_mode_on(void)
{
	return app_usbaudio_mode;
}

void app_usb_key(APP_KEY_STATUS *status, void *param)
{
    TRACE("%s %d,%d",__func__, status->code, status->event);

}

const APP_KEY_HANDLE  app_usb_handle_cfg[] = {
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"USB HID FN1 UP key",app_usb_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"USB HID FN2 UP key",app_usb_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"USB HID PWR UP key",app_usb_key, NULL},
};

void app_usb_key_init(void)
{
    uint8_t i = 0;
    TRACE("%s",__func__);
    for (i=0; i<(sizeof(app_usb_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&app_usb_handle_cfg[i]);
    }
}

#endif


#ifdef __ANKER_UI__
#include "bes_hal_board.h"
extern "C" void cx20921_wake_up_event_register(void (* cb)(void));
#endif

#if OS_HAS_CPU_STAT
extern "C" void rtx_show_all_threads_usage(void);
#define CPU_USAGE_TIMER_TMO_VALUE (_CPU_STATISTICS_PEROID_/3)
static void cpu_usage_timer_handler(void const *param);
osTimerDef(cpu_usage_timer, cpu_usage_timer_handler);
static osTimerId cpu_usage_timer_id = NULL;
static void cpu_usage_timer_handler(void const *param)
{
    rtx_show_all_threads_usage();
}
#endif

extern "C" bool hal_pwrkey_startup_pressed(void);//change by cai.zhong 20181225 for check power key
int app_init(void)
{
    int nRet = 0;
    struct nvrecord_env_t *nvrecord_env;
    bool need_check_key = true;
    uint8_t pwron_case = APP_POWERON_CASE_INVALID;
    uint32_t bootUpMode = hal_sw_bootmode_get();
    TRACE("app_init");
    #ifdef __WATCHER_DOG_RESET__
    app_wdt_open(15);
#endif
#ifdef __BT_ANC__
    app_anc_ios_init();
#endif
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_52M);
    #if defined(MCU_HIGH_PERFORMANCE_MODE)
    TRACE("sys freq calc : %d", hal_sys_timer_calc_cpu_freq(0,0));
    #endif
    apps_init_tid = osThreadGetId();
    list_init();
    app_os_init();
    TRACE("%s: sw bootmode =0x%08x!---------------------------------",__func__,bootUpMode);
    app_status_indication_init();
#if OS_HAS_CPU_STAT
	cpu_usage_timer_id = osTimerCreate(osTimer(cpu_usage_timer), osTimerPeriodic, NULL);
	if (cpu_usage_timer_id != NULL) {
		osTimerStart(cpu_usage_timer_id, CPU_USAGE_TIMER_TMO_VALUE);
	}
#endif

#ifdef __ANKER_UI__	
	hal_board_init();
#endif

    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_REBOOT){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        pwron_case = APP_POWERON_CASE_REBOOT;
        need_check_key = false;
        TRACE("REBOOT!!!");
    }

    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_MODE){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_MODE);
        pwron_case = APP_POWERON_CASE_TEST;
        need_check_key = false;
        TRACE("TESTER!!!");
    }

#ifdef __ANKER_UI__
	TRACE("do not open battery");
	if(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_VOICE_INPUT_OTA)
    {
    #ifdef __CUSTOM_APP__
        //app_enter_voice_input_mode_by_uart(1);
    #endif
    }
    /* end: add by jason.wu for voice input module ota update*/

    nRet = app_battery_open();
    TRACE("%d",nRet);
    if (nRet < 0){        
        nRet = -1;
        TRACE("%s: app_battery_open failed",__func__);
        goto exit;
    }else if (nRet > 0 && 
        pwron_case != APP_POWERON_CASE_TEST){
        pwron_case = APP_POWERON_CASE_CHARGING;
        app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
        TRACE("CHARGING!!!");
        app_battery_start();
        need_check_key = false;
        nRet = 0;
    #if 0 // charging mode normal power on
        #if !defined(BTUSB_AUDIO_MODE)
        btdrv_start_bt();
        btdrv_hciopen();
        btdrv_sleep_config(1);
        btdrv_hcioff();
        #endif
		app_key_open(false);
        app_key_init_on_charging();
        // if battery is fake, need to del this exit to avoid system can't boot
        goto exit;
    #endif
    }else{
        nRet = 0;
    }
    if(bootUpMode & HAL_SW_BOOTMODE_ENTER_HIDE_BOOT)
    {
        TRACE("%s: Begin BES OTA Process",__func__);
        nRet = -1;
        goto exit;
    }
#else
    nRet = app_battery_open();
    TRACE("%d",nRet);
    if (nRet < 0){        
#if !defined(BYPASS_BATTERY_OPS)
        nRet = -1;
        goto exit;
#endif
    }else if (nRet > 0 && 
        pwron_case != APP_POWERON_CASE_TEST){
#if !defined(BYPASS_BATTERY_OPS)
        pwron_case = APP_POWERON_CASE_CHARGING;
        app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
        TRACE("CHARGING!!!");
        app_battery_start();
#if !defined(BTUSB_AUDIO_MODE)
        btdrv_start_bt();
        btdrv_hciopen();
        btdrv_sleep_config(1);
        btdrv_hcioff();
#endif
		app_key_open(false);
        app_key_init_on_charging();
        nRet = 0;
        goto exit;
#endif
    }else{
        nRet = 0;
    }
#if defined(BYPASS_BATTERY_OPS)
    nRet = 0;
#endif
#endif
    if (app_key_open(need_check_key)){
        TRACE("%s: app_key_open failed",__func__);
        nRet = -1;
        goto exit;
    }

    hal_sw_bootmode_set(HAL_SW_BOOTMODE_REBOOT);
#ifdef  EXTERN_PA_ENABLE 
	app_extern_pa_init();
#endif

    app_bt_init();
    af_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();

    nv_record_env_init();
    nvrec_dev_data_open();
//    app_bt_connect2tester_init();
    nv_record_env_get(&nvrecord_env);
	btdrv_start_bt();
	hal_sys_timer_delay(MS_TO_TICKS(200));
#ifdef __BT_ANC__	
	app_anc_open_module();
#endif
#ifdef __AMA_VOICE__
	app_ama_handle_init();
#endif
#ifdef __DMA_VOICE__
    // add begin by cai.zhong 20190105 for DMA requires the BT and BLE addr to be the same. 
    if (app_check_bt_ble_addr()){
        TRACE("%s: app_check_bt_ble_addr failed",__func__);
        af_close();
        app_key_close();
        nRet = -1;
        goto exit;
    }
    // add end by cai.zhong 20190105 for DMA requires the BT and BLE addr to be the same.
	app_dueros_dma_init();
#endif

#ifdef MEDIA_PLAYER_SUPPORT
    app_play_audio_set_lang(nvrecord_env->media_language.language);
#endif
    app_bt_stream_volume_ptr_update(NULL);
#ifdef __THIRDPARTY
#ifdef CHIP_BEST2000
    // TODO: To be removed
    pmu_override_efuse_get_handler(app_efuse_get_handler);
#endif
	app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_INIT);
#endif

    // init every time even if wdt timeout or reboot
    //if (pwron_case == APP_POWERON_CASE_REBOOT) {
    //    pwron_case = APP_POWERON_CASE_NORMAL;
    //}

    if (pwron_case == APP_POWERON_CASE_REBOOT){
        //btdrv_start_bt();
        BesbtInit();
#ifdef __SMARTVOICE__
        app_smartvoice_init();
#endif        
#if 0//def __DMA_VOICE__  //change by cai.zhong 20190102 for rfcomm init in the app_bt.cpp
        app_dma_rfcomm_server_init();
#endif
        osDelay(600);
#if OTA_ENABLED
		ota_handler_init();
#endif
        bt_drv_extra_config_after_init();
        TRACE("\n\n\nbt_stack_init_done:%d\n\n\n", pwron_case);
        app_bt_accessmode_set(BAM_NOT_ACCESSIBLE);
        app_bt_profile_connect_manager_opening_reconnect();
        app_key_init();
        app_battery_start();
#if defined(__EARPHONE__) && defined(__AUTOPOWEROFF__)
        app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
#endif

#if ((IAG_BLE_INCLUDE == XA_ENABLED) && (BLE_APP_DATAPATH_SERVER))
        BLE_custom_command_init();
#endif
#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_START);
		app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_BT_CONNECTABLE);
#endif
    }
#ifdef __ENGINEER_MODE_SUPPORT__
    else if(pwron_case == APP_POWERON_CASE_TEST){
		app_factorymode_set(true);
        app_status_indication_set(APP_STATUS_INDICATION_POWERON);
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
#endif
#ifdef __WATCHER_DOG_RESET__
        app_wdt_close();
#endif
        TRACE("!!!!!ENGINEER_MODE!!!!!\n");
        nRet = 0;
        //btdrv_start_bt();
        BesbtInit();
        osDelay(600);
#if OTA_ENABLED
        ota_handler_init();
#endif		
        bt_drv_extra_config_after_init();
        app_nvrecord_rebuild();
        app_factorymode_key_init();
        if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_SIGNALINGMODE){
            hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_MASK);
            app_factorymode_bt_signalingtest(NULL, NULL);
        }
        if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE){
            hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_MASK);
            app_factorymode_bt_nosignalingtest(NULL, NULL);
        }
    }
#endif
    else{	
		//change begin by cai.zhong 20181225 for check power key
		if (hal_pwrkey_startup_pressed()) {
            app_status_indication_set(APP_STATUS_INDICATION_POWERON);
            #ifdef MEDIA_PLAYER_SUPPORT
            app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
            #endif
		}else{
		    TRACE("%s: power key is DITHERING!!!",__func__);
            af_close();
            app_key_close();
            nRet = -1;
			goto exit;
        }
		//change end by cai.zhong 20181225 for check power key

        app_poweron_key_init(); 
        pwron_case = app_poweron_wait_case();
        {
            if (pwron_case != APP_POWERON_CASE_INVALID && pwron_case != APP_POWERON_CASE_DITHERING){
                TRACE("hello world i'm bes1000 hahaha case:%d\n", pwron_case);
                nRet = 0;
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
                app_status_indication_set(APP_STATUS_INDICATION_INITIAL);
#endif
                //btdrv_start_bt();
                BesbtInit();
#ifdef __SMARTVOICE__
                app_smartvoice_init();
#endif
#if 0// def __DMA_VOICE__ //change by cai.zhong 20190102 for rfcomm init in the app_bt.cpp
				app_dma_rfcomm_server_init();
#endif

                osDelay(600);
#if OTA_ENABLED
				ota_handler_init();
#endif
                bt_drv_extra_config_after_init();
                TRACE("\n\n\nbt_stack_init_done:%d\n\n\n", pwron_case);

#if ((IAG_BLE_INCLUDE == XA_ENABLED) && (BLE_APP_DATAPATH_SERVER))
                BLE_custom_command_init();
#endif
                switch (pwron_case) {
                    case APP_POWERON_CASE_CALIB:
                        break;
                    case APP_POWERON_CASE_BOTHSCAN:
                        app_status_indication_set(APP_STATUS_INDICATION_BOTHSCAN);
#ifdef MEDIA_PLAYER_SUPPORT
                        app_voice_report(APP_STATUS_INDICATION_BOTHSCAN,0);
#endif
#ifdef __EARPHONE__
                        app_bt_accessmode_set(BT_DEFAULT_ACCESS_MODE_PAIR);
#if defined(__AUTOPOWEROFF__)
                        app_start_10_second_timer(APP_PAIR_TIMER_ID);
#endif
#endif
#ifdef __THIRDPARTY
						app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_BT_DISCOVERABLE);
#endif
                        break;
                    case APP_POWERON_CASE_NORMAL:
#if defined( __EARPHONE__ ) && !defined(__EARPHONE_STAY_BOTH_SCAN__)
                        app_bt_accessmode_set(BAM_NOT_ACCESSIBLE);
#endif
                    case APP_POWERON_CASE_REBOOT:
                    case APP_POWERON_CASE_ALARM:
                    default:
                        app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
#if defined( __EARPHONE__) && defined(__BT_RECONNECT__)
                        app_bt_profile_connect_manager_opening_reconnect();
#endif
#ifdef __THIRDPARTY
                        app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_BT_CONNECTABLE);
#endif

                        break;
                }
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
                app_poweron_wait_finished();
#endif
                app_key_init();
                app_battery_start();
#if defined(__EARPHONE__) && defined(__AUTOPOWEROFF__)
                app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
#endif
#ifdef __THIRDPARTY
                app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO1,THIRDPARTY_START);
#endif

#ifdef MEDIA_PLAYER_RBCODEC
                rb_ctl_init();
#endif
            }else{
                af_close();
                app_key_close();
                nRet = -1;
            }
        }
    }
exit:
#ifdef KNOWLES_UART_DATA
	app_ai_voice_uart_audio_init();
#endif	
#ifdef __ANKER_UI__	
#ifndef __PC_CMD_UART__
	hal_board_late_init();
#endif
#endif
#ifdef __BT_ANC__
    app_anc_set_init_done();
#endif
#if defined(BTUSB_AUDIO_MODE)
    if(pwron_case == APP_POWERON_CASE_CHARGING) {
#ifdef __WATCHER_DOG_RESET__
        app_wdt_close();
#endif
        af_open();
        app_key_handle_clear();
        app_usb_key_init();

        app_usbaudio_entry();

    }
#endif
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_32K);
	

    return nRet;
}
#endif

