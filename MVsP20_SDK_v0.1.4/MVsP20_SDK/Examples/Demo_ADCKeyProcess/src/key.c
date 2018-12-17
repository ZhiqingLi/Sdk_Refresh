/**
 **************************************************************************************
 * @file    key.c
 * @brief   key process
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-11-22 11:30:00$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "adc.h"
#include "gpio.h"
#include "adc_key.h"
#include "timeout.h"
#include "debug.h"

uint8_t		gKeyValue = 0;

#define   GetKeyValue()              gKeyValue
#define   SetKeyValue(v)             gKeyValue = v

void KeyInit(void)
{
	AdcKeyInit();
}

void KeyScanProcess(void)
{
    AdcKeyMsg KeyMsg;

    KeyMsg = AdcKeyScan();

    if(KeyMsg.index != ADC_KEY_UNKNOW && KeyMsg.type != KEY_UNKOWN_TYPE)
    {
        if(KeyMsg.type == KEY_PRESSED || KeyMsg.type == KEY_LONG_PRESS_HOLD)
        {
            switch(KeyMsg.index)
            {
            case ADC_KEY_1:
                SetKeyValue('1');
                break;
            case ADC_KEY_2:
                SetKeyValue('2');
                break;
            case ADC_KEY_3:
                SetKeyValue('3');
                break;
            case ADC_KEY_4:
                SetKeyValue('4');
                break;
            case ADC_KEY_5:
                SetKeyValue('5');
                break;
            case ADC_KEY_6:
                SetKeyValue('6');
                break;
            case ADC_KEY_7:
                SetKeyValue('7');
                break;
            case ADC_KEY_8:
                SetKeyValue('8');
                break;
            case ADC_KEY_9:
                SetKeyValue('9');
                break;
            case ADC_KEY_10:
                SetKeyValue('A');
                break;

            default:
                break;
            }
        }
    }
}

void KeyProcess(void)
{
    //adc key scan
    KeyScanProcess();

    switch(gKeyValue)
    {
        case '1':
        	DBG("Key 2 Press\n");
            break;

        case '2':
        	DBG("Key 3 Press\n");
            break;

        case '3':
        	DBG("Key 4 Press\n");
            break;

        case '4':
        	DBG("Key 5 Press\n");
            break;

        case '5':
        	DBG("Key 6 Press\n");
            break;

        case '6':
        	DBG("Key 7 Press\n");
            break;

        case '7':
        	DBG("Key 8 Press\n");
            break;

        case '8':
        	DBG("Key 9 Press\n");
            break;

        case '9':
        	DBG("Key 10 Press\n");
            break;

        case 'A':
        	DBG("Key 11 Press\n");
            break;

        default:

            break;
    }
    SetKeyValue(0);
}
