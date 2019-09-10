#include "type.h"
#include "adc.h"
#include "gpio.h"
#include "adc_key.h"
#include "timeout.h"
#include "debug.h"

#define  ADC_KEY_PORT               ADC_CHANNEL_GPIOA20_A23
#define  ADC_KEY_SCAN_TIME			10
#define  ADC_KEY_JITTER_TIME		30
#define  ADC_KEY_CP_TIME		    1000
#define  ADC_KEY_CPH_TIME			200
#define	 ADC_KEY_COUNT				10
#define	 ADC_KEY_FULL_VAL			4096

typedef enum _ADC_KEY_STATE
{
    ADC_KEY_STATE_IDLE,
    ADC_KEY_STATE_JITTER,
    ADC_KEY_STATE_PRESS_DOWN,
    ADC_KEY_STATE_CP
}ADC_KEY_STATE;

TIMER AdcKeyWaitTimer;
TIMER AdcKeyScanTimer;

ADC_KEY_STATE AdcKeyState;

//Key process, image key value to key event.
static uint16_t AdcKeyGetIndex(uint8_t Channel)
{
	uint16_t Val;
	uint16_t KeyIndex;

	Val = ADC_SingleModeDataGet(Channel);
	//DBG("Val = %d\n", Val);
	if(Val >= (ADC_KEY_FULL_VAL - (ADC_KEY_FULL_VAL / ADC_KEY_COUNT / 2)))
	{
		return ADC_KEY_UNKNOW;
	}

	KeyIndex = (Val + (ADC_KEY_FULL_VAL / ADC_KEY_COUNT) / 2) * ADC_KEY_COUNT / ADC_KEY_FULL_VAL;
	//DBG("KeyIndex = %d\n", KeyIndex);
	return KeyIndex;
}

AdcKeyMsg AdcKeyScan(void)
{
	static uint8_t PreKeyIndex = ADC_KEY_UNKNOW;

	uint16_t KeyIndex;

    AdcKeyMsg Msg = {ADC_KEY_UNKNOW, KEY_UNKOWN_TYPE};

	if(!IsTimeOut(&AdcKeyScanTimer))
	{
		return Msg;
	}

	TimeOutSet(&AdcKeyScanTimer, ADC_KEY_SCAN_TIME);

	KeyIndex = AdcKeyGetIndex(ADC_KEY_PORT);

    Msg.index = KeyIndex;

	switch(AdcKeyState)
	{
	case ADC_KEY_STATE_IDLE:
		if(KeyIndex != ADC_KEY_UNKNOW)
		{
			PreKeyIndex = KeyIndex;
            TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_JITTER_TIME);
            AdcKeyState = ADC_KEY_STATE_JITTER;
		}
        break;

	case ADC_KEY_STATE_JITTER:
		if(PreKeyIndex != KeyIndex)
		{
			AdcKeyState = ADC_KEY_STATE_IDLE;
		}
		else if(IsTimeOut(&AdcKeyWaitTimer))
		{
			TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CP_TIME);
			AdcKeyState = ADC_KEY_STATE_PRESS_DOWN;

			Msg.type = KEY_PRESSED;
		}
		break;

	case ADC_KEY_STATE_PRESS_DOWN:
		if(PreKeyIndex != KeyIndex)
		{
			AdcKeyState = ADC_KEY_STATE_IDLE;

			Msg.type = KEY_RELEASED;
		}
		else if(IsTimeOut(&AdcKeyWaitTimer))
		{
			TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
			AdcKeyState = ADC_KEY_STATE_CP;

            Msg.type = KEY_LONG_PRESSED;
		}
		break;

	case ADC_KEY_STATE_CP:
		if(PreKeyIndex != KeyIndex)
		{
			AdcKeyState = ADC_KEY_STATE_IDLE;

            Msg.type = KEY_LONG_RELEASED;
		}
		else if(IsTimeOut(&AdcKeyWaitTimer))
		{
			TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);

            Msg.type = KEY_LONG_PRESS_HOLD;
		}
		break;

	default:
		AdcKeyState = ADC_KEY_STATE_IDLE;
		break;
	}

	return Msg;
}

void AdcKeyInit(void)
{
	AdcKeyState = ADC_KEY_STATE_IDLE;

	TimeOutSet(&AdcKeyScanTimer, 0);

	GPIO_RegOneBitSet(GPIO_A_ANA_EN, GPIO_INDEX23);//channel 4
}
