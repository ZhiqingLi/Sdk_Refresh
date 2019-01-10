
#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "string.h"
#include "bes_hal_board.h"
#include "bes_hal_uart.h"
#include "app_key.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "app_audio.h"
#include "app_bt_stream.h"
#include "app_bt_media_manager.h"

#include "is31fl3236a.h"
#include "bes_hal_flash.h"
#include "app_utils.h"
#include "hal_sysfreq.h"
#ifdef VOICE_INPUT_MODULE_CX20921
#include "conexant_mode.h"
#endif
#include "app_ai_voice.h"
#ifdef __AMA_VOICE__
//#include "ama_hal.h"
#endif
//#include "app_custom_ctrl.h"
#include "bes_hal_hdver.h"
#include "led_process.h"
#include "ambient_light_process.h"

#ifdef EXTRAL_CODEC_AK7755_ENABLE
#include "ak7755.h"
#endif

#define HAL_ANKER_BOARD_DEBUG       BES_HAL_UART_DEBUG
#define HAL_ANKER_BOARD_INFO        BES_HAL_UART_INFO
#define HAL_ANKER_BOARD_WARNING     BES_HAL_UART_WARNING
#define HAL_ANKER_BOARD_ERROR       BES_HAL_UART_ERROR
#define HAL_ANKER_BOARD_FATAL       BES_HAL_UART_FATEL

#if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
#define BOARD_GPIO_INTR_DEBOUNCE_NUM         2
#else
#define BOARD_GPIO_INTR_DEBOUNCE_NUM         1
#endif

#define BOARD_GPIO_DEBOUNCE_DELAY            MS_TO_TICKS(100)
#define BOARD_LINEIN_PLUG_DECT_DEBOUNCE      0x00000001

#if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
#define BOARD_USB_OUT_OVERFLOW_DEBOUNCE      0x00000002
#define BOARD_GPIO_INTR_DEBOUNCE_MASK        0x00000003
#else
#define BOARD_GPIO_INTR_DEBOUNCE_MASK        0x00000001
#endif

//static uint8_t bes_firmware_version[32] = {'4','.','8','.','0','.','1'};
static uint8_t bes_firmware_version[32] = {'0','.','1','.','5','\0'};


static uint32_t board_hw_intr_debounce_time[BOARD_GPIO_INTR_DEBOUNCE_NUM] = {0};
static uint32_t board_hw_timer_request = 0;
static HWTIMER_ID board_hw_timerid = NULL;
static int32_t board_hw_timer_set(uint32_t request, uint32_t delay);

void bes_firmware_version_get(uint8_t* pVerStr)
{
    memcpy((void*)pVerStr,(void*)bes_firmware_version, strlen((const char*)bes_firmware_version));
}

void gpio_intr_set_edge(enum HAL_GPIO_PIN_T pin, uint8_t down_edge, gpio_intr_handle intrHandle)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;
    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_polarity = (down_edge == 1) ? HAL_GPIO_IRQ_POLARITY_LOW_FALLING : HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    gpiocfg.irq_handler = intrHandle;
    hal_gpio_setup_irq(pin, &gpiocfg);
}

/******************************** I2C BUS Pin Define **************************************************/
void hal_iomux_set_i2c_bus(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c_bus[] = {
        {BOARD_IOMUX_I2C_SCL, HAL_IOMUX_FUNC_I2C_M_SCL, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {BOARD_IOMUX_I2C_SDA, HAL_IOMUX_FUNC_I2C_M_SDA, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };
    hal_iomux_init(pinmux_i2c_bus, ARRAY_SIZE(pinmux_i2c_bus));
}

/********************************CONEXANT CX20921 IO Define *********************************************/
#ifdef VOICE_INPUT_MODULE_CX20921
static void hal_iomux_set_conexant_cx20921_pin(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_cx20921_dev[] = {
        {BOARD_IOMUX_CX20921_WAKEUP, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLDOWN_ENALBE},  // wakeup irq pin : pin 3_5
        {BOARD_IOMUX_CX20921_RST, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}, // Reset pin : pin 0_0
        {BOARD_IOMUX_CX20921_PWR, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, // Reset pin : pin 0_7
    };
    hal_iomux_init(pinmux_cx20921_dev, ARRAY_SIZE(pinmux_cx20921_dev));
    hal_gpio_pin_set_dir(BOARD_GPIO_CX20921_RST, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(BOARD_GPIO_CX20921_PWR, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_CX20921_WAKEUP, HAL_GPIO_DIR_IN, 0);
}
#endif

/******************************** Key Pin Define ***********************************************************/
#if HW_BOARD_SEL == HW_BOARD_ANKER_BT3163_DEMO
/*
 * HAL_IOMUX_PIN_P1_0 : bt_pare_ctrl_key
 * HAL_IOMUX_PIN_P1_1 : bt_func_key play_key 
 * HAL_IOMUX_PIN_P1_2 : volume + key
 * HAL_IOMUX_PIN_P1_3 : volume - key
 * HAL_IOMUX_PIN_P3_7 : BassUp key
 * HAL_IOMUX_PIN_P4_3 : Light key
*/

/*
 * APP_KEY_CODE_FN2 : BT_PARE_KEY; 
 * APP_KEY_CODE_FN1 : BT_FUNC_KEY; 
 * APP_KEY_CODE_FN3 : short --> press Volue_Up_KEY, long press --> play backward key
 * APP_KEY_CODE_FN4 : short --> press Volue_Down_KEY, long press --> play forward key
 * APP_KEY_CODE_FN5 : short --> press Bass up key
 * APP_KEY_CODE_FN6 : short --> Light key
 */
//gpiokey define

const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM] = {
    {APP_KEY_CODE_FN1,{BOARD_IOMUX_KEY_BT_OK, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
    {APP_KEY_CODE_FN2,{BOARD_IOMUX_KEY_BT_PARE, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
    {APP_KEY_CODE_FN3,{BOARD_IOMUX_KEY_BT_VOL_UP, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
    {APP_KEY_CODE_FN4,{BOARD_IOMUX_KEY_BT_VOL_DOWN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
    {APP_KEY_CODE_FN5,{BOARD_IOMUX_KEY_BASS_CTRL, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
    {APP_KEY_CODE_FN6,{BOARD_IOMUX_KEY_LIGHT_CTRL, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
};
#elif HW_BOARD_SEL == HW_BOARD_ANKER_BT3163
 const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM] = {
    {APP_KEY_CODE_FN2,{BOARD_IOMUX_KEY_BT_PARE, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
    {APP_KEY_CODE_FN3,{BOARD_IOMUX_KEY_BT_VOL_UP, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
};
#else
const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM] = {
    {APP_KEY_CODE_FN1,{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
    {APP_KEY_CODE_FN2,{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
    {APP_KEY_CODE_FN3,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
    {APP_KEY_CODE_FN4,{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
    {APP_KEY_CODE_FN5,{HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}},
};
#endif

#ifdef BOARD_HW_ADC_KEY_MODULE
static void hal_iomux_set_adc_key_intr(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_adckey_intr[] = {
        {BOARD_IOMUX_ADCKEY_INTR, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //PA_PWR_ON

    };
    hal_iomux_init(pinmux_adckey_intr, ARRAY_SIZE(pinmux_adckey_intr));
    hal_gpio_pin_set_dir(BOARD_GPIO_ADCKEY_INTR, HAL_GPIO_DIR_IN, 0);


}
#endif

/****************************************** PA IO Define *******************************************/
/*
 * HAL_IOMUX_PIN_P0_4 : PA_PWR_ON
 * HAL_IOMUX_PIN_P0_5 : PA_EN
 * HAL_IOMUX_PIN_P0_6 : PA_MUTE
*/
static void hal_iomux_set_pa(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pa[] = {
        {BOARD_IOMUX_PA_PWR, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //PA_PWR_ON
        {BOARD_IOMUX_PA_EN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //PA_EN
        {BOARD_IOMUX_PA_MUT, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL} //PA_MUTE
    };
    hal_iomux_init(pinmux_pa, ARRAY_SIZE(pinmux_pa));
    hal_gpio_pin_set_dir(BOARD_GPIO_PA_PWR, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_PA_EN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_PA_MUT, HAL_GPIO_DIR_OUT, 0);
}

typedef enum  {
    CODEC_POWER_UP = 0,
    PA_POWER_UP,
    PA_ENABLE,
    PA_DISABLE_MUTE,
    CODEC_DISABLE_MUTE,
    VOICE_OPEN_STEP_MAX
}VOICE_OPEN_STEP_T;

typedef enum  {
    CODEC_ENABLE_MUTE = 0,
    PA_ENABLE_MUTE,
    PA_DISABLE,
    PA_POWER_DOWN,
    CODEC_POWER_DOWN,
    VOICE_CLOSE_STEP_MAX
}VOICE_CLOSE_STEP_T;

typedef void(*voice_operation_func)(void);
typedef struct{
    uint8_t steps;
    voice_operation_func operationFunc;
    uint16_t delayMs;
}hal_voice_process_t;

osMutexId codec_opeation_lock = NULL;
osMutexDef(codec_opeation_mutex);
static void codec_opeation_lock_init(void)
{
    if(codec_opeation_lock == NULL){
        codec_opeation_lock = osMutexCreate((osMutex(codec_opeation_mutex)));
    }
}

static void codec_opeation_lock_lock(void)
{
    if(codec_opeation_lock)
        osMutexWait(codec_opeation_lock, osWaitForever); //osWaitForever
}

static void codec_opeation_lock_unlock(void)
{
    if(codec_opeation_lock)
        osMutexRelease(codec_opeation_lock);
}

static void codec_power_up(void)
{
    TRACE("%s: ENTER",__func__);
    codec_opeation_lock_lock();
    ak7755_application_init();
    codec_opeation_lock_unlock();
    TRACE("%s: EXIT",__func__);
}

void codec_power_down(void)
{
    TRACE("%s: ENTER",__func__);
    codec_opeation_lock_lock();
    ak7755_power_off();
    codec_opeation_lock_unlock();
    TRACE("%s: EXIT",__func__);
}

void codec_mute_enable(void)
{
    TRACE("%s: ENTER",__func__);
    codec_opeation_lock_lock();
    ak7755_adc_mute();
    ak7755_dac_mute();
    codec_opeation_lock_unlock();
    TRACE("%s: EXIT",__func__);
}

void codec_mute_disable(void)
{
    TRACE("%s: ENTER",__func__);
    codec_opeation_lock_lock();
    ak7755_adc_normal();
    ak7755_dac_normal();
    codec_opeation_lock_unlock();
    TRACE("%s: EXIT",__func__);
}
void pa_mute_enable(void)
{
    hal_gpio_pin_clr(BOARD_GPIO_PA_MUT); 
    TRACE("%s",__func__);
}

void pa_mute_disable(void)
{
    hal_gpio_pin_set(BOARD_GPIO_PA_MUT);    
    TRACE("%s",__func__);
}

void pa_func_enable(void)
{
    hal_gpio_pin_set(BOARD_GPIO_PA_EN);
    TRACE("%s",__func__);
}

void pa_func_disable(void)
{
    hal_gpio_pin_clr(BOARD_GPIO_PA_EN);
    TRACE("%s",__func__);
}

void pa_power_up(void)
{
    hal_gpio_pin_set(BOARD_GPIO_PA_PWR);
    TRACE("%s",__func__);
}

void pa_power_down(void)
{
    hal_gpio_pin_clr(BOARD_GPIO_PA_PWR);
    TRACE("%s",__func__);
}

VOICE_OPEN_STEP_T gVoiceOpenCurtStep = VOICE_OPEN_STEP_MAX;
hal_voice_process_t gVoiceOpenProcess[VOICE_OPEN_STEP_MAX] ={
    {CODEC_POWER_UP,codec_power_up,10},
    {PA_POWER_UP,pa_power_up,100},
    {PA_ENABLE,pa_func_enable,10},
    {PA_DISABLE_MUTE,pa_mute_disable,10},
    {CODEC_DISABLE_MUTE,codec_mute_disable,0},
};

VOICE_CLOSE_STEP_T gVoiceCloseCurtStep = VOICE_CLOSE_STEP_MAX;
hal_voice_process_t gVoiceCloseProcess[VOICE_CLOSE_STEP_MAX] ={
    {CODEC_ENABLE_MUTE, codec_mute_enable, 5000},
    {PA_ENABLE_MUTE, codec_mute_enable, 100},
    {PA_DISABLE, codec_mute_enable, 100},
    {PA_POWER_DOWN, codec_mute_enable, 100},
    {CODEC_POWER_DOWN, codec_mute_enable, 0},
};

static void voice_on_timerhandler(void const *param);
osTimerDef (VOICE_ON_PROCESS, voice_on_timerhandler);
static osTimerId voice_on_timer = NULL;
static void voice_on_timer_start(uint16_t ms)
{
    if(voice_on_timer)
    {
        osTimerStop(voice_on_timer);
        osTimerStart(voice_on_timer, ms);
    }
   // TRACE("%s: START ON TIMER",__func__);
}

static void voice_on_timer_stop(void)
{
    if(voice_on_timer)
    {
        osTimerStop(voice_on_timer);
    }
    //TRACE("%s: STOP ON TIMER",__func__);
}

static void voice_on_timerhandler(void const *param)
{
    //TRACE("%s: enter gVoiceOpenCurtStep = %d",__func__,gVoiceOpenCurtStep);
    if(gVoiceOpenCurtStep < VOICE_OPEN_STEP_MAX)
    {
       // TRACE("%s: OPEN VOICE PROCESSING--------",__func__);
        gVoiceOpenProcess[gVoiceOpenCurtStep].operationFunc();
        if(gVoiceOpenProcess[gVoiceOpenCurtStep].delayMs > 0)
        {
            voice_on_timer_stop();
            voice_on_timer_start(gVoiceOpenProcess[gVoiceOpenCurtStep].delayMs);
        }
        gVoiceOpenCurtStep++;
    }
   // TRACE("%s: exit gVoiceOpenCurtStep = %d",__func__,gVoiceOpenCurtStep);
}

static void voice_on_timer_create(void)
{
    if (voice_on_timer == NULL)
        voice_on_timer = osTimerCreate (osTimer(VOICE_ON_PROCESS), osTimerOnce, NULL);
}

static void voice_on_process_reset(void)
{
    voice_on_timer_stop();
    gVoiceOpenCurtStep = VOICE_OPEN_STEP_MAX;
    //TRACE("%s: RESET VOICE ON----------",__func__);
}

static void voice_off_timerhandler(void const *param);
osTimerDef (VOICE_OFF_PROCESS, voice_off_timerhandler);
static osTimerId voice_off_timer = NULL;
static void voice_off_timer_start(uint16_t ms)
{
    if(voice_off_timer)
    {
        osTimerStop(voice_off_timer);
        osTimerStart(voice_off_timer, ms);
    }
    //TRACE("%s: START OFF TIMER",__func__);
}

static void voice_off_timer_stop(void)
{
    if(voice_off_timer)
    {
        osTimerStop(voice_off_timer);
    }
   //TRACE("%s: STOP VOICE OFF TIMER",__func__);
}

static void voice_off_timerhandler(void const *param)
{
   // TRACE("%s: enter gVoiceCloseCurtStep = %d",__func__,gVoiceCloseCurtStep);
    if(gVoiceCloseCurtStep < VOICE_CLOSE_STEP_MAX)
    {
        //TRACE("%s: CLOSE VOICE PROCESSING--------",__func__);
        gVoiceCloseProcess[gVoiceCloseCurtStep].operationFunc();
        if(gVoiceCloseProcess[gVoiceCloseCurtStep].delayMs > 0)
        {
            voice_off_timer_start(gVoiceCloseProcess[gVoiceCloseCurtStep].delayMs);
        }
        gVoiceCloseCurtStep++;
    }
    //TRACE("%s: exit gVoiceCloseCurtStep = %d",__func__,gVoiceCloseCurtStep);
}

static void voice_off_timer_create(void)
{
    if (voice_off_timer == NULL)
        voice_off_timer = osTimerCreate (osTimer(VOICE_OFF_PROCESS), osTimerOnce, NULL);
}

static void voice_off_process_reset(void)
{
    voice_off_timer_stop();
    gVoiceCloseCurtStep = VOICE_CLOSE_STEP_MAX;
    //TRACE("%s: RESET VOICE OFF----------",__func__);
}

void hal_pa_power_on(void)
{
//    hal_gpio_pin_set(BOARD_GPIO_PA_PWR);
    codec_opeation_lock_init();
    voice_on_timer_create();
    voice_off_timer_create();
}


void hal_pa_output_ctrl(uint8_t flag)
{
    //TRACE("%s: pa power onoff = %d--------------",__func__,flag);
    if(flag) // pa on
    {
        voice_off_process_reset();
        //TRACE("%s: gVoiceOpenCurtStep = %d--------------",__func__,gVoiceOpenCurtStep);
        if(gVoiceOpenCurtStep == VOICE_OPEN_STEP_MAX)
        {
           // TRACE("%s: Start VOICE OPEN PROCESS--------------",__func__);
            gVoiceOpenCurtStep = CODEC_POWER_UP;
            voice_on_timer_start(1);
        }
    }
    else
    {
        voice_on_process_reset();
       // TRACE("%s: gVoiceCloseCurtStep = %d--------------",__func__,gVoiceCloseCurtStep);
        if(gVoiceCloseCurtStep == VOICE_CLOSE_STEP_MAX)
        {
          //  TRACE("%s: Start VOICE CLOSE PROCESS--------------",__func__);
            gVoiceCloseCurtStep = CODEC_ENABLE_MUTE;
            voice_off_timer_start(1);
        }
    }
}

/************************************ Power Key ************************************/
static void hal_iomux_set_sys_power_lock(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_sys_power_lock[] = {
        {BOARD_IOMUX_SYS_PWR_LOCK, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}, 
    };

    hal_iomux_init(pinmux_sys_power_lock, ARRAY_SIZE(pinmux_sys_power_lock));
    hal_gpio_pin_set_dir(BOARD_GPIO_SYS_PWR_LOCK, HAL_GPIO_DIR_OUT, 1);
}

static void hal_sys_power_lock_ctrl(uint8_t flag)
{
    if(flag)
    { 
        hal_gpio_pin_set(BOARD_GPIO_SYS_PWR_LOCK); //sets RESET PIN to high.
    }
    else
    {
        hal_gpio_pin_clr(BOARD_GPIO_SYS_PWR_LOCK); //sets RESET PIN to low.
    }
}

/*************************************Power Charging **********************************/

/*
 * HAL_IOMUX_PIN_P3_3 : CHARGER_ENABLE: high enable charging, low disable charging
 * HAL_IOMUX_PIN_P2_5 : CHARGER_STATE: high, battery is full; low, battery is charging; VBUS must exist; 
*/
// app_battery_ext_charger_indicator_cfg is not used
const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE
};

static void hal_iomux_set_charger(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_charger[] = {
        {BOARD_IOMUX_CHARGE_EN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},  // charger enbale
        {BOARD_IOMUX_CHARGE_STATE, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE} // CHARGER_STATE: high, battery is full; low, battery is charging; VBUS must exist; 
    };
    hal_iomux_init(pinmux_charger, ARRAY_SIZE(pinmux_charger));
    hal_gpio_pin_set_dir(BOARD_GPIO_CHARGE_EN, HAL_GPIO_DIR_OUT, 0); 
    hal_gpio_pin_set_dir(BOARD_GPIO_CHARGE_STATE, HAL_GPIO_DIR_IN, 0); 
}

void hal_charger_ctrl(uint8_t flag)
{
    if(flag)
    {
        hal_gpio_pin_set(BOARD_GPIO_CHARGE_EN);
        TRACE("%s: TURN ON CHARGER ----------",__func__);
    }
    else
    {
        hal_gpio_pin_clr(BOARD_GPIO_CHARGE_EN);
        TRACE("%s: TURN OFF CHARGER ----------",__func__);
    }
}

/*************************************Codec extral power ctrl **********************************/
#ifdef HW_INT_CODEC_POWER_FROM_EXTRAL
static void hal_iomux_set_codec_extral_power(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_codex[] = {
        {BOARD_IOMUX_CODEC_EXTRAL_PWR_CTRL, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //Codec Power ctrl : 1 on, 0 off
    };
    hal_iomux_init(pinmux_codex, ARRAY_SIZE(pinmux_codex));
    hal_gpio_pin_set_dir(BOARD_GPIO_CODEC_EXTRAL_PWR_CTRL, HAL_GPIO_DIR_OUT, 0);
}

void hal_codec_extral_power_ctrl(uint8_t flag)
{
    TRACE("%s: codec extral power onoff = %d--------------",__func__,flag);
    if(flag)
    {
        hal_gpio_pin_set(BOARD_GPIO_CODEC_EXTRAL_PWR_CTRL);
    }
    else
    {
        hal_gpio_pin_clr(BOARD_GPIO_CODEC_EXTRAL_PWR_CTRL);
    }
}
#endif

/*************************************light ctrl **********************************/
static void hal_iomux_set_light_ctrl(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_codex[] = {
        {BOARD_IOMUX_LIGHT_CHIP_CTRL, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //Codec Power ctrl : 1 on, 0 off
    };
    hal_iomux_init(pinmux_codex, ARRAY_SIZE(pinmux_codex));
    hal_gpio_pin_set_dir(BOARD_GPIO_LIGHT_CHIP_CTRL, HAL_GPIO_DIR_OUT, 0);
}

void hal_light_ctrl_power_ctrl(uint8_t flag)
{
    if(flag)
    {
        hal_gpio_pin_set(BOARD_GPIO_LIGHT_CHIP_CTRL);
    }
    else
    {
        hal_gpio_pin_clr(BOARD_GPIO_LIGHT_CHIP_CTRL);
    }
}

/*************************************USB 5V Out ctrl **********************************/
void hal_usb_5v_out_ctrl(uint8_t enable)
{
    if(enable)
    {
        #if 0
        if(hal_gpio_pin_get_val(HAL_GPIO_PIN_P2_3) == 0)
        {
            hal_gpio_pin_set(HAL_IOMUX_PIN_P0_1);
            TRACE("%s: enable 5v out",__func__);
        }
        else
        {
            TRACE("%s: over flow pin is high, can't enable 5v out",__func__);
            hal_gpio_pin_clr(HAL_IOMUX_PIN_P0_1);
            TRACE("%s: disable 5v out",__func__);
        }
        #else
        hal_gpio_pin_set(BOARD_GPIO_USB_5V_OUT_EN);
        TRACE("%s: enable 5v out",__func__);
        #endif
    }
    else
    {
        hal_gpio_pin_clr(BOARD_GPIO_USB_5V_OUT_EN);
        TRACE("%s: disable 5v out",__func__);
    }
}

#if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
static void usbout_overflow_handler(enum HAL_GPIO_PIN_T pin)
{
    if(pin != BOARD_GPIO_USB_5V_OUT_STATE)
    {
        TRACE("%s: err pin intr %d--------------",__func__,pin);
        return;
    }   
    board_hw_timer_set(BOARD_USB_OUT_OVERFLOW_DEBOUNCE,BOARD_GPIO_DEBOUNCE_DELAY);
}


static void usbout_overflow_debounce_handle(void)
{
    uint8_t state = 0;
    // wakeup bes2000 bt mcu
    
    state = hal_gpio_pin_get_val(BOARD_GPIO_USB_5V_OUT_STATE);
    
    if(state != 0) //high, over flow, disable 5v out
    {
        TRACE("%s: USB_5V_OUT OVER FLOW!!!",__func__);
        hal_light_ctrl_power_ctrl(0);
    }
}
#endif

static void hal_iomux_set_usb5v_out_ctrl(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_usb_5v[] = {
        {BOARD_IOMUX_USB_5V_OUT_EN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //gpio_01: 5v out en: high-->enable, low-->disable
    #if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
        {BOARD_IOMUX_USB_5V_OUT_STATE, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, //gpio_23: over flow detect pin, high-->over flow, low-->normal
    #endif
    };
    hal_iomux_init(pinmux_usb_5v, ARRAY_SIZE(pinmux_usb_5v));
    hal_gpio_pin_set_dir(BOARD_GPIO_USB_5V_OUT_EN, HAL_GPIO_DIR_OUT, 1);
    #if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
    hal_gpio_pin_set_dir(BOARD_GPIO_USB_5V_OUT_STATE, HAL_GPIO_DIR_IN, 0);
    gpio_intr_set_edge(BOARD_IOMUX_USB_5V_OUT_STATE,0,usbout_overflow_handler);
    #endif
}

/*************************************LineIn Detect **********************************/
//gpio_03: linein input detect
static void linein_detect_handler(enum HAL_GPIO_PIN_T pin)
{
    // wakeup bes2000 bt mcu
    if(pin != BOARD_GPIO_LINEIN_DECT)
    {
        TRACE("%s: err pin intr %d--------------",__func__,pin);
        return;
    }
    TRACE("%s: --------------",__func__);
    board_hw_timer_set(BOARD_LINEIN_PLUG_DECT_DEBOUNCE,3*BOARD_GPIO_DEBOUNCE_DELAY);
}

 bool linein_is_plug_in(void){
	return (hal_gpio_pin_get_val(BOARD_GPIO_LINEIN_DECT)>0) ?true:false;
}
static void linein_detect_debounce_handler(void)
{
    uint8_t state = 0;
    state = hal_gpio_pin_get_val(BOARD_GPIO_LINEIN_DECT);
    TRACE("%s: state = %d--------------",__func__,state);
    #if LINEIN_SEQUENCE_SEL == LINEIN_SEQUENCE_APPLE
    if(state != 0) // low--> high, plug in
    #else
    if(state == 0) // high-->low, plug in
    #endif
    {
        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_LINEIN,0,0); // btapp.h , BT_DEVICE_ID_1
        gpio_intr_set_edge(BOARD_GPIO_LINEIN_DECT,1,linein_detect_handler);  // 
        TRACE("%s: APPLE_SEQ plug in",__func__);
    }
    else // high-->low, plug out
    {
        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_LINEIN,0,0); // btapp.h , BT_DEVICE_ID_1
        gpio_intr_set_edge(BOARD_GPIO_LINEIN_DECT,0,linein_detect_handler);  // 
        TRACE("%s: APPLE_SEQ plug out",__func__);
    }
}

static void hal_iomux_set_linein_detect(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_linein[] = {
        {BOARD_IOMUX_LINEIN_DECT, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
    };
    hal_iomux_init(pinmux_linein, ARRAY_SIZE(pinmux_linein));
    hal_gpio_pin_set_dir(BOARD_GPIO_LINEIN_DECT, HAL_GPIO_DIR_IN, 0);
#if LINEIN_SEQUENCE_SEL == LINEIN_SEQUENCE_APPLE
    gpio_intr_set_edge(BOARD_GPIO_LINEIN_DECT,0,linein_detect_handler);
#else
    gpio_intr_set_edge(BOARD_GPIO_LINEIN_DECT,1,linein_detect_handler);
#endif
}

/*************************** LINE IN VOICE INPUT DECT ***************************************/
#ifdef BOARD_HW_LINEIN_VOICE_DETCT_MODULE
#define BOARD_IOMUX_LINEIN_VOICE_DECT       HAL_IOMUX_PIN_P3_3 
#define BOARD_GPIO_LINEIN_VOICE_DECT        HAL_GPIO_PIN_P3_3

static void hal_iomux_set_linein_voice_detect(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_linein_voice[] = {
        {BOARD_IOMUX_LINEIN_VOICE_DECT, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE}, 
    };
    hal_iomux_init(pinmux_linein_voice, ARRAY_SIZE(pinmux_linein_voice));
    hal_gpio_pin_set_dir(BOARD_GPIO_LINEIN_VOICE_DECT, HAL_GPIO_DIR_IN, 0);
    gpio_intr_set_edge(BOARD_GPIO_LINEIN_VOICE_DECT,1,linein_detect_handler);

}
#endif

#ifdef EXTRAL_CODEC_AK7755_ENABLE

#include "reg_iomux_best2000.h"
#include "hal_iomux.h"

#ifdef SPI_VOLTAGE_VMEM
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

static struct IOMUX_T * const iomux = (struct IOMUX_T *)IOMUX_BASE;
static void hal_iomux_set_3163_spi(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi[] = {
        {BOARD_IOMUX_SPI_CLK, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_SPI_CS, HAL_IOMUX_FUNC_SPILCD_CS, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_SPI_IN, HAL_IOMUX_FUNC_SPILCD_DCN, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_SPI_OUT, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL}, 
    };
    hal_iomux_init(pinmux_spi, ARRAY_SIZE(pinmux_spi));
    iomux->REG_060 |= IOMUX_SPILCD_4WIRE;
}

void hal_iomux_set_3163_spi_normal(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi[] = {
        {BOARD_IOMUX_SPI_CLK, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_SPI_CS, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_SPI_IN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_SPI_OUT, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
    };
    hal_iomux_init(pinmux_spi, ARRAY_SIZE(pinmux_spi));
    hal_gpio_pin_set_dir(BOARD_IOMUX_SPI_CLK, HAL_GPIO_DIR_IN, 0);
    hal_gpio_pin_set_dir(BOARD_IOMUX_SPI_CS, HAL_GPIO_DIR_IN, 0);
    hal_gpio_pin_set_dir(BOARD_IOMUX_SPI_IN, HAL_GPIO_DIR_IN, 0);
    hal_gpio_pin_set_dir(BOARD_IOMUX_SPI_OUT, HAL_GPIO_DIR_IN, 0);
    TRACE("%s: Release SPI PINS------------------",__func__);
}


static void hal_iomux_set_3163_ak7755(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_ak7755[] = {
        {BOARD_IOMUX_AK7755_PWR, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_AK7755_RST, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 

    };
    hal_iomux_init(pinmux_ak7755, ARRAY_SIZE(pinmux_ak7755));
    hal_gpio_pin_set_dir(BOARD_GPIO_AK7755_PWR, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_AK7755_RST, HAL_GPIO_DIR_OUT, 0);
}

void ak7755_power_ctrl(uint8_t flag){

	if(flag)
        hal_gpio_pin_set(BOARD_GPIO_AK7755_PWR);	
	else
		hal_gpio_pin_clr(BOARD_GPIO_AK7755_PWR);
}

void ak7755_reset_ctrl(uint8_t flag){
	if(flag)
        hal_gpio_pin_set(BOARD_GPIO_AK7755_RST);	
	else
		hal_gpio_pin_clr(BOARD_GPIO_AK7755_RST);
}
#endif

/************************** LED CTRL *****************************************************************/
//#if HW_BOARD_SEL == HW_BOARD_ANKER_BT3163
#if 0

void led_pin_init(void){
	int i;
	for (i=0;i<2;i++){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_pinmux_pwl[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[i].pin, HAL_GPIO_DIR_OUT, 1);
    }
}
static bool power_red_led_state = false;
static bool power_white_led_state = false;

void power_red_led_set(bool state){
	if(state)
		hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_1].pin);
	else
		hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_1].pin);
	power_red_led_state = state;
}

void power_red_led_switch_state(void){

	if(power_red_led_state){
		power_red_led_set(false);
	}else{
		power_red_led_set(true);
	}
}

void power_white_led_set(bool state){
	if(state)
		hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_0].pin);
	else
		hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_0].pin);
	power_white_led_state = state;
}

void power_white_led_switch_state(void){
	uint8_t ret = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_1].pin);
	if(power_white_led_state){
		power_white_led_set(false);
	}else{
		power_white_led_set(true);
	}
}


static void hal_iomux_set_3163_led(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_led_ctrl[] = {
        {BOARD_IOMUX_BT_WHITE_LED, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_BT_BLUE_LED, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_ALEXA_LED, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_ENV_LED, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}, 
        {BOARD_IOMUX_BASS_LED, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}
    };
    hal_iomux_init(pinmux_led_ctrl, ARRAY_SIZE(pinmux_led_ctrl));
    hal_gpio_pin_set_dir(BOARD_GPIO_BT_WHITE_LED, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_BT_BLUE_LED, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_ALEXA_LED, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_ENV_LED, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(BOARD_GPIO_BASS_LED, HAL_GPIO_DIR_OUT, 0);
}

void switch_bt_blue_led_state(void){
	uint8_t ret = hal_gpio_pin_get_val(BOARD_GPIO_BT_BLUE_LED);
	if(ret)
		hal_gpio_pin_clr(BOARD_GPIO_BT_BLUE_LED);
	else
		hal_gpio_pin_set(BOARD_GPIO_BT_BLUE_LED);
}

void switch_led_bass_state(void){
	uint8_t ret = hal_gpio_pin_get_val(BOARD_GPIO_BASS_LED);
	if(ret)
		hal_gpio_pin_clr(BOARD_GPIO_BASS_LED);
	else
		hal_gpio_pin_set(BOARD_GPIO_BASS_LED);
}

void bes_anker_3163_led_ctrl(LED_3163_BOARD_TYPE ledType, uint8_t onoff)
{
    //TRACE("%s: ledType = %d, onoff = %d",__func__,ledType,onoff);
    switch(ledType)
    {
        case LED_BT_WHITE:
            if(onoff)
                hal_gpio_pin_set(BOARD_GPIO_BT_WHITE_LED);
            else
                hal_gpio_pin_clr(BOARD_GPIO_BT_WHITE_LED);
        break;
        case LED_BT_BLUE:
            if(onoff)
                hal_gpio_pin_set(BOARD_GPIO_BT_BLUE_LED);
            else
                hal_gpio_pin_clr(BOARD_GPIO_BT_BLUE_LED);
        break;
        case LED_BT_PLAY_STATE:
            if(onoff)
                hal_gpio_pin_set(BOARD_GPIO_ALEXA_LED);
            else
                hal_gpio_pin_clr(BOARD_GPIO_ALEXA_LED);
        break;
        case LED_ENV_STATE:
            if(onoff)
                hal_gpio_pin_set(BOARD_GPIO_ENV_LED);
            else
                hal_gpio_pin_clr(BOARD_GPIO_ENV_LED);
        break;
        case LED_BASS_STATE:
            if(onoff)
                hal_gpio_pin_set(BOARD_GPIO_BASS_LED);
            else
                hal_gpio_pin_clr(BOARD_GPIO_BASS_LED);
        break;
        default:
        break;
    }
}
#endif

/************************* MIC Ctrl ***************************/
#ifdef BOARD_HW_MIC_CTRL_MODULE
static void hal_iomux_set_mic_ctrl(void)
{
    #if MICPHONE_DEF_OPEN_WHILE_BOOTUP
    uint8_t flag = 0; // high means close mic, low means open mic;
    #else
    uint8_t flag = 1; // high means close mic, low means open mic;
    #endif
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_charger[] = {
        {BOARD_IOMUX_MIC_CTRL, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},  // charger enbale
    };
    hal_iomux_init(pinmux_charger, ARRAY_SIZE(pinmux_charger));
    hal_gpio_pin_set_dir(BOARD_GPIO_MIC_CTRL, HAL_GPIO_DIR_OUT, flag);  
}

void hal_mic_power_ctrl(uint8_t flag)
{
    if(flag)
    {
    	hal_gpio_pin_clr(BOARD_GPIO_MIC_CTRL);
    }
    else
    {
         hal_gpio_pin_set(BOARD_GPIO_MIC_CTRL);
    }
}

uint8_t hal_mic_power_state_get(void)
{
    uint8_t ret = 0;
    ret = hal_gpio_pin_get_val(BOARD_GPIO_MIC_CTRL) != 0 ? 1 : 0 ;
    return ret;
}
#endif
/************************* GPIO INTURRUPT HARDWARE TIMER ********************************************/

static int32_t board_hw_timer_find_minimun_dubounce(uint32_t *pIndex)
{
    uint32_t minDebounceTime = 0xffffffff;
    uint32_t index = 0, selIndex = BOARD_GPIO_INTR_DEBOUNCE_NUM;
    for(index = 0; index < BOARD_GPIO_INTR_DEBOUNCE_NUM; index++)
    {
        if((board_hw_timer_request & (0x01<<index)) && 
            ((board_hw_intr_debounce_time[index] < minDebounceTime) && (board_hw_intr_debounce_time[index] != 0) && (minDebounceTime != 0)))
        {
            selIndex = index;
            minDebounceTime = board_hw_intr_debounce_time[index];
        }
    }

    if(selIndex >= BOARD_GPIO_INTR_DEBOUNCE_NUM)
        return -1;
    
    *pIndex = selIndex;
    return 0;
}

static void board_hw_timer_clear_all_debounce_time(void)
{
    uint32_t index = 0;
    for(index = 0; index < BOARD_GPIO_INTR_DEBOUNCE_NUM; index++)
        board_hw_intr_debounce_time[index] = 0;
}

static void board_hw_timer_handler(void *p)
{
    TRACE(" %s , request %d ", __func__, board_hw_timer_request);
    uint32_t selIndex = BOARD_GPIO_INTR_DEBOUNCE_NUM;
    uint32_t minDelay = 0;
    int32_t nRet = 0;
    nRet = board_hw_timer_find_minimun_dubounce(&selIndex);
    if(nRet < 0)
    {
        TRACE("%s: find minimun dubounce failed 111111",__func__);
        goto debounce_err_exit;
    }

    switch(0x01<<selIndex)
    {
        #if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
        case BOARD_USB_OUT_OVERFLOW_DEBOUNCE:
        board_hw_timer_request = board_hw_timer_request & (~BOARD_USB_OUT_OVERFLOW_DEBOUNCE & BOARD_GPIO_INTR_DEBOUNCE_MASK);
        usbout_overflow_debounce_handle();
        break;
        #endif
        case BOARD_LINEIN_PLUG_DECT_DEBOUNCE:
        board_hw_timer_request = board_hw_timer_request & (~BOARD_LINEIN_PLUG_DECT_DEBOUNCE & BOARD_GPIO_INTR_DEBOUNCE_MASK);
        linein_detect_debounce_handler();
        break;
        default:
            TRACE("%s: find minimun dubounce failed 22222",__func__);
            goto debounce_err_exit;
    }

    if(board_hw_timer_request == 0)
    {
        board_hw_timer_clear_all_debounce_time();
        hwtimer_stop(board_hw_timerid);
    }
    else
    {   
        nRet = board_hw_timer_find_minimun_dubounce(&selIndex);
        if(nRet < 0)
        {
            TRACE("%s: find minimun dubounce failed 33333",__func__);
            goto debounce_err_exit;
        }
        minDelay = board_hw_intr_debounce_time[selIndex] - hal_sys_timer_get();
        hwtimer_stop(board_hw_timerid);
        hwtimer_start(board_hw_timerid, minDelay);
    }
    return;
debounce_err_exit:
    hwtimer_stop(board_hw_timerid);
}

static int32_t board_hw_timer_set(uint32_t request, uint32_t delay)
{
    uint32_t time = hal_sys_timer_get();
    uint32_t index = 0;

    for(index = 0; index < BOARD_GPIO_INTR_DEBOUNCE_NUM; index++)
    {
        if(request & (0x01<<index))
            break;
    }
    TRACE(" %s request %d , index = %d, ocur time = %d, delay %ds ", __func__, request, index, time, (TICKS_TO_MS(delay) / 1000));
    if(index >= BOARD_GPIO_INTR_DEBOUNCE_NUM)
    {
        TRACE("%s: parse request failed: request = 0x%08x, index = %d",__func__,request, index);
        return -1;
    }
    if (board_hw_timerid == NULL)
    {
        TRACE("%s: hw timer is not exist: request = 0x%08x, index = %d",__func__,request, index);
        return -2;
    }

    board_hw_intr_debounce_time[index] = time + delay;
    if(board_hw_timer_request == 0) // no other timer is request
    {
        board_hw_timer_request |= request;
        
        hwtimer_stop(board_hw_timerid);
        hwtimer_start(board_hw_timerid, delay);
    }
    else // there is other timer requests
    {
        uint32_t selIndex = BOARD_GPIO_INTR_DEBOUNCE_NUM;
        uint32_t minDelay = 0;
        int32_t nRet = 0;
        nRet = board_hw_timer_find_minimun_dubounce(&selIndex);
        if(nRet < 0)
        {
            TRACE("%s: find minimun dubounce failed 33333",__func__);
            hwtimer_stop(board_hw_timerid);
            return -1;
        }

        minDelay =  board_hw_intr_debounce_time[selIndex] - hal_sys_timer_get();
        hwtimer_stop(board_hw_timerid);
        hwtimer_start(board_hw_timerid, minDelay);
    }

    return 0;
}


void app_initboard_hardware_timer(void)
{
    if (board_hw_timerid == NULL)
        board_hw_timerid = hwtimer_alloc((HWTIMER_CALLBACK_T)board_hw_timer_handler, &board_hw_timer_request);
}


static void hal_board_iomux_init(void)
{
    hal_iomux_set_sys_power_lock();
    hal_iomux_set_pa();
    hal_iomux_set_charger();
#ifdef HW_INT_CODEC_POWER_FROM_EXTRAL
    hal_iomux_set_codec_extral_power();
#endif
//    hal_iomux_set_i2c_bus();
#ifdef VOICE_INPUT_MODULE_CX20921
    hal_iomux_set_conexant_cx20921_pin();
#endif
    hal_iomux_set_usb5v_out_ctrl();
    hal_iomux_set_linein_detect();
    hal_iomux_set_light_ctrl();

#ifdef BOARD_HW_ADC_KEY_MODULE
    hal_iomux_set_adc_key_intr();
#endif

#if HW_BOARD_SEL == HW_BOARD_ANKER_BT3163
    //hal_iomux_set_3163_led();
#endif
#ifdef BOARD_HW_MIC_CTRL_MODULE
    hal_iomux_set_mic_ctrl();
#endif

#ifdef EXTRAL_CODEC_AK7755_ENABLE
    hal_iomux_set_spilcd();
//    hal_iomux_set_3163_spi();
    hal_iomux_set_3163_ak7755();
#endif

#ifdef BOARD_HW_LINEIN_VOICE_DETCT_MODULE
    hal_iomux_set_linein_voice_detect();
#endif
}

int32_t hal_board_pre_init(void)
{
    hal_board_iomux_init();
    hal_sys_power_lock_ctrl(1);
    hardware_version_open();
    return 0;
}

int32_t hal_board_init(void)
{
    int32_t nRet = 0;
#ifdef __CUSTOM_APP__
	app_timer_init();
	led_process_init();
#endif
#ifdef HW_INT_CODEC_POWER_FROM_EXTRAL
    hal_codec_extral_power_ctrl(1);
#endif
#ifdef EXTRAL_CODEC_AK7755_ENABLE
	//app_eq_init();
#endif

#ifdef __CUSTOM_APP__
	ambient_light_process_thread_create();
#endif
    TRACE("%s: light ctrl init return %d",__func__,nRet);

    app_initboard_hardware_timer();
/*
#ifdef __AMA_VOICE__
    //ama_init();
#endif
#ifdef __CUSTOM_APP__
    //app_custom_ctrl_process_init();
#endif
*/
	hal_pa_power_on();
    return nRet;
}

int32_t hal_board_late_init(void)
{
#ifdef VOICE_INPUT_MODULE_CX20921
    conexant_mode_init();
#endif

    return 0;
}
