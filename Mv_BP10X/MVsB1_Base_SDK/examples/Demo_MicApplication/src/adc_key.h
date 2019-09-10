#ifndef __ADC_KEY_H__
#define __ADC_KEY_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

typedef enum _AdcKeyIndex
{
    ADC_KEY_1 = 0,
    ADC_KEY_2 = 2,
    ADC_KEY_3 = 4,
    ADC_KEY_4 = 6,
    ADC_KEY_5 = 8,
    ADC_KEY_UNKNOW = 0xFF,
}AdcKeyIndex;

typedef enum _AdcKeyType
{
    KEY_PRESSED = 0,
    KEY_RELEASED,
    KEY_LONG_PRESSED,
    KEY_LONG_PRESS_HOLD,
    KEY_LONG_RELEASED,
    KEY_UNKOWN_TYPE = 0xFF
}AdcKeyType;

typedef struct _AdcKeyMsg
{
    uint16_t index;
    uint16_t type;
}AdcKeyMsg;

AdcKeyMsg AdcKeyScan(void);

void AdcKeyInit(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__ADC_KEY_H__

