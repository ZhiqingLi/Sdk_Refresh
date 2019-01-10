#ifndef __BES_HAL_ADC_KEY_H__
#define __BES_HAL_ADC_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bes_hal_board.h"

#ifdef BOARD_HW_ADC_KEY_MODULE
#include "app_key.h"

enum ADC_KEY_STATE_T{
    ADC_KEY_IDLE = 0,
    ADC_KEY_PRESS,
    ADC_KEY_LONGPRESS,
    ADC_KEY_LONGLONGPRESS,
    ADC_KEY_STATE_MAX,
};

#define ADC_KEY_DEBOUNCE_NUM            (5)
#define ADC_INVALID_VOLT                (1600)

#define ADC_KEY_TIME_FIRST_INTEVAL    (20)
#define ADC_KEY_TIME_INTEVAL    (50)
#define ADC_KEY_LONG_PRESS_NUM  (5*5) // 5*200ms * 5 = 5s
#define ADC_KEY_LONGLONG_PRESS_NUM  (10*5) // 5*200ms * 10 = 10s

void set_adc_key_status(APP_KEY_STATUS key_status);

APP_KEY_STATUS get_adc_key_status(void);
int bes_hal_adc_key_open(APP_ADC_KEY_MAP *pAdcKeyMap, uint8_t adcKeyNum);
#endif
#ifdef __cplusplus
}
#endif
#endif