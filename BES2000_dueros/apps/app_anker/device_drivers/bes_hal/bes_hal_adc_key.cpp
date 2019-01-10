
#include <stdio.h>
#include "bttypes.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "string.h"


#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_gpadc.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "hal_trace.h"
#include "app_key.h"
#include "bes_hal_board.h"


extern "C" 
{
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "sys/mei.h"
#include "hal_key.h"
}
#include "hal_aud.h"
#include "btapp.h"

#include "bes_hal_adc_key.h"
#include "bes_hal_hdver.h"
#ifdef BOARD_HW_ADC_KEY_MODULE

typedef struct ADC_KEY_MEASURE_T_
{
    enum ADC_KEY_STATE_T keyStatus;
    enum APP_KEY_CODE_T keyCode;
    uint16_t avgVolt;
    uint16_t voltage[ADC_KEY_DEBOUNCE_NUM];
    uint16_t index;
    uint16_t pressMeasureCount;
}ADC_KEY_MEASURE_T;


static ADC_KEY_MEASURE_T gAdcKeyMeasure;

static APP_ADC_KEY_MAP *gAdcKeyMap = NULL;
static uint8_t gAdcKeyNum = 0;
static uint8_t gAdcReadFlag = 0;

static uint32_t hw_adckey_timer_request = 0;
static HWTIMER_ID hw_adckey_timerid = NULL;

static void hw_adckey_detect_handler(enum HAL_GPIO_PIN_T pin);
static void hw_adc_key_timer_handler(void *p);
static int32_t bes_hal_adc_key_start_read(void);
static int32_t bes_hal_adc_key_stop_read(void);


APP_ADC_KEY_MAP app_adc_key_handle_playing_cfg[ADC_KEY_MAX_NUM] = {
    {APP_KEY_CODE_FN4,0,50,0,"Adckey_Vole_Down",NULL, NULL},
    {APP_KEY_CODE_FN6,340,50,-50,"Adckey_Light_Ctrl",NULL, NULL},
    {APP_KEY_CODE_FN1,680,50,-50,"Adckey_BT_Play",NULL, NULL},
    {APP_KEY_CODE_FN5,950,50,-50,"Adckey_Bass_Ctrl",NULL, NULL},
    {APP_KEY_CODE_FN7,1260,50,-50,"Adckey_Mic_Mute",NULL, NULL}
};

static uint8_t bes_hal_adc_key_volt2index(uint16_t volt)
{
    uint8_t index = 0;

    if(hardware_version_get_ver() == BES_HW_VER_3163_DEMO)
    {
        if((volt < gAdcKeyMap[0].keyVolt + gAdcKeyMap[0].downDaltVolt) || 
            (volt > app_adc_key_handle_playing_cfg[gAdcKeyNum-1].keyVolt + app_adc_key_handle_playing_cfg[gAdcKeyNum-1].upDaltVolt))
            return gAdcKeyNum;
    }
    else
    {
        if((volt < gAdcKeyMap[0].keyVolt + gAdcKeyMap[0].downDaltVolt) || 
        (volt > gAdcKeyMap[gAdcKeyNum-1].keyVolt + gAdcKeyMap[gAdcKeyNum-1].upDaltVolt))
            return gAdcKeyNum;
    }
    

    for(index = 0; index < gAdcKeyNum; index++)
    {
        if((volt >= gAdcKeyMap[index].keyVolt + gAdcKeyMap[index].downDaltVolt) &&
            (volt < gAdcKeyMap[index].keyVolt + gAdcKeyMap[index].upDaltVolt))
        {
            break;
        }
    }

    if(index < gAdcKeyNum)
    {
        return index;
    }

    if(hardware_version_get_ver() == BES_HW_VER_3163_DEMO)
    {
        for(index = 0; index < gAdcKeyNum; index++)
        {
            if((volt >= app_adc_key_handle_playing_cfg[index].keyVolt + app_adc_key_handle_playing_cfg[index].downDaltVolt) &&
                (volt < app_adc_key_handle_playing_cfg[index].keyVolt + app_adc_key_handle_playing_cfg[index].upDaltVolt))
            {
                break;
            }
        }
    }
    return index;
    

}

APP_KEY_STATUS adc_key_status;

void set_adc_key_status(APP_KEY_STATUS key_status){
	//if(adc_key_status == key_status)return;
	
	adc_key_status = key_status;
	if(adc_key_status.event == APP_KEY_EVENT_DOWN){
		hal_key_debounce_timer_restart();
	}
	TRACE("%s : key_status event: %d,  code:%d",__func__,key_status.event,key_status.code);
}

APP_KEY_STATUS get_adc_key_status(void){
	return adc_key_status;
}

void bes_hal_adc_key_send_key(enum APP_KEY_CODE_T keyCode, enum ADC_KEY_STATE_T keyStatus)
{
    APP_KEY_STATUS key_status;
    //TRACE("%s: keyCode = %d, keyStatus = %d",__func__,keyCode,keyStatus);
    switch(keyStatus)
    {
        case ADC_KEY_IDLE: 
            key_status.event = APP_KEY_EVENT_UP;
        break;
        case ADC_KEY_PRESS: 
            key_status.event = APP_KEY_EVENT_DOWN;
        break;
        case ADC_KEY_LONGPRESS: 
            key_status.event = APP_KEY_EVENT_LONGPRESS;
        break;
        case ADC_KEY_LONGLONGPRESS: 
            key_status.event = APP_KEY_EVENT_LONGLONGPRESS;
        break;
        default:
            key_status.code = APP_KEY_CODE_NONE;
            key_status.event = APP_KEY_EVENT_NONE;
        break;
    }
	
    if(keyStatus < ADC_KEY_STATE_MAX)
    {
        key_status.code = keyCode;
        //bt_key_send(&key_status);

        if(keyStatus == ADC_KEY_PRESS)
        {
        	set_adc_key_status(key_status);
            TRACE("%s: KEY PRESS keyCode = %d",__func__,keyCode);
        }
        else if(keyStatus == ADC_KEY_IDLE)
        {
        	set_adc_key_status(key_status);
            TRACE("%s: KEY Release keyCode = %d",__func__,keyCode);
        }
    }
    
}


void bes_hal_adc_key_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    uint32_t meanVolt = 0;
    ADC_KEY_MEASURE_T* key_measure =  (ADC_KEY_MEASURE_T*)&gAdcKeyMeasure;

    //TRACE("%s %d irq:0x%04x",__func__, volt, irq_val);
    if (volt == HAL_GPADC_BAD_VALUE){
        TRACE("%s %d Err volt",__func__, volt);
        return;
    }

    if(!gAdcReadFlag)
    {
        TRACE("%s read not valid",__func__);
        return;
    }

    key_measure->voltage[key_measure->index++%ADC_KEY_DEBOUNCE_NUM] = volt;
    //TRACE("%s insert %d -- %d volt to array",__func__,key_measure->index, volt);
    if (key_measure->index >= ADC_KEY_DEBOUNCE_NUM)
    {
        enum APP_KEY_CODE_T keyCode = APP_KEY_CODE_NONE;
        uint8_t validVoltCount = 0, i = 0, keyIndex = ADC_KEY_MAX_NUM;
        bes_hal_adc_key_stop_read();
        for (i=0; i<ADC_KEY_DEBOUNCE_NUM; i++)
        {
            if(key_measure->voltage[i] != ADC_INVALID_VOLT)
            {
                validVoltCount++;
                meanVolt += key_measure->voltage[i];
            }
        }

        for (i=0; i<ADC_KEY_DEBOUNCE_NUM; i++)
        {
            key_measure->voltage[i] = ADC_INVALID_VOLT;
        }
        key_measure->index = 0;

        TRACE("%s: measure_index =%d, validVoltCount = %d",__func__, key_measure->index, validVoltCount);
        if(validVoltCount > 0)
            meanVolt /= validVoltCount;
        else
        {
            hwtimer_stop(hw_adckey_timerid);
            hwtimer_start(hw_adckey_timerid, MS_TO_TICKS(ADC_KEY_TIME_INTEVAL));
            return;
        }
        
        key_measure->avgVolt = (uint16_t)(meanVolt);
        keyIndex = bes_hal_adc_key_volt2index(key_measure->avgVolt);

        TRACE("%s: meanVolt =%d, keyIndex = %d",__func__, meanVolt, keyIndex);
        if(keyIndex < gAdcKeyNum)
            keyCode = (enum APP_KEY_CODE_T)(gAdcKeyMap[keyIndex].keyCode);
        else
            keyCode = APP_KEY_CODE_NONE;

        TRACE("%s: keyCode =%d, keyStatus = %d",__func__, keyCode, key_measure->keyStatus);
        if(keyCode != APP_KEY_CODE_NONE)
        {
            if(key_measure->keyStatus == ADC_KEY_IDLE)
            {
                // key press
                key_measure->keyCode = keyCode;
                key_measure->keyStatus = ADC_KEY_PRESS;
                key_measure->pressMeasureCount = 0;
                bes_hal_adc_key_send_key(key_measure->keyCode,key_measure->keyStatus);
            }
            else if(key_measure->keyStatus == ADC_KEY_PRESS)
            {
                key_measure->pressMeasureCount++;
                if(key_measure->pressMeasureCount >= ADC_KEY_LONG_PRESS_NUM)
                {
                    // key long press
                    key_measure->keyStatus = ADC_KEY_LONGPRESS;
                    bes_hal_adc_key_send_key(key_measure->keyCode,key_measure->keyStatus);
                }
            }
            else if(key_measure->keyStatus == ADC_KEY_LONGPRESS)
            {
                key_measure->pressMeasureCount++;
                if(key_measure->pressMeasureCount >= ADC_KEY_LONGLONG_PRESS_NUM)
                {
                    // key long long press
                    key_measure->keyStatus = ADC_KEY_LONGLONGPRESS;
                    bes_hal_adc_key_send_key(key_measure->keyCode,key_measure->keyStatus);
                }
            }
            else if(key_measure->keyStatus == ADC_KEY_LONGLONGPRESS)
            {
                key_measure->pressMeasureCount++;
            }

            hwtimer_stop(hw_adckey_timerid);
            hwtimer_start(hw_adckey_timerid, MS_TO_TICKS(ADC_KEY_TIME_INTEVAL));
        }
        else
        {
            if((key_measure->keyStatus == ADC_KEY_PRESS) || 
                (key_measure->keyStatus == ADC_KEY_LONGPRESS) || 
                (key_measure->keyStatus == ADC_KEY_LONGLONGPRESS))
            {
                // key release
                key_measure->keyStatus = ADC_KEY_IDLE;
                bes_hal_adc_key_send_key(key_measure->keyCode,key_measure->keyStatus);
                key_measure->keyCode = APP_KEY_CODE_NONE;
            }
            hwtimer_stop(hw_adckey_timerid);
        }
    }
    #if 0
    else
    {
        TRACE("%s start continus read %d types",__func__,key_measure->index, volt);
        bes_hal_adc_key_start_read();
    }
    #endif
}


static int32_t bes_hal_adc_key_start_read(void)
{
    ADC_KEY_MEASURE_T* key_measure =  (ADC_KEY_MEASURE_T*)&gAdcKeyMeasure;
    uint8_t i = 0;
    key_measure->index = 0;
    for(i = 0; i < ADC_KEY_DEBOUNCE_NUM; i++)
        key_measure->voltage[i] = ADC_INVALID_VOLT;

    gAdcReadFlag = 1;
    return hal_gpadc_open(BOARD_HW_ADC_KEY_CH, HAL_GPADC_ATP_1MS, bes_hal_adc_key_irqhandler);
}

static int32_t bes_hal_adc_key_stop_read(void)
{
    gAdcReadFlag = 0;
    return hal_gpadc_close(BOARD_HW_ADC_KEY_CH);
}

static void bes_hal_adc_key_clear_measure(void)
{
    ADC_KEY_MEASURE_T* key_measure =  (ADC_KEY_MEASURE_T*)&gAdcKeyMeasure;
    uint8_t i = 0;
    key_measure->keyStatus = ADC_KEY_IDLE;
    key_measure->avgVolt = 0;
    key_measure->index = 0;
    key_measure->keyCode = APP_KEY_CODE_NONE;
    key_measure->pressMeasureCount = 0;
    for(i = 0; i < ADC_KEY_DEBOUNCE_NUM; i++)
        key_measure->voltage[i] = ADC_INVALID_VOLT;
}


int bes_hal_adc_key_open(APP_ADC_KEY_MAP *pAdcKeyMap, uint8_t adcKeyNum)
{
    gAdcKeyMap = pAdcKeyMap;
    gAdcKeyNum = adcKeyNum;
    bes_hal_adc_key_clear_measure();
    gpio_intr_set_edge(BOARD_GPIO_ADCKEY_INTR,1,hw_adckey_detect_handler);
    if (hw_adckey_timerid == NULL)
        hw_adckey_timerid = hwtimer_alloc((HWTIMER_CALLBACK_T)hw_adc_key_timer_handler, &hw_adckey_timer_request);
    return 0;
}

static void hw_adckey_detect_handler(enum HAL_GPIO_PIN_T pin)
{
    // wakeup bes2000 bt mcu
    if(pin != BOARD_GPIO_ADCKEY_INTR)
    {
        TRACE("%s: err pin intr %d--------------",__func__,pin);
        return;
    }
    TRACE("%s: --------------",__func__);
    bes_hal_adc_key_clear_measure();
    hwtimer_stop(hw_adckey_timerid);
    hwtimer_start(hw_adckey_timerid, MS_TO_TICKS(ADC_KEY_TIME_FIRST_INTEVAL));
}

static void hw_adc_key_timer_handler(void *p)
{
    bes_hal_adc_key_start_read();
}

#endif