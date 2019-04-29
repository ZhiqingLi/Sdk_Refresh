/******************************************************************************
 * MODULE NAME:    hw_leds.h
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    Functions to access LEDs
 * MAINTAINER:     Tom Kerwick
 * DATE:           10 Jan 2011
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/
#ifndef _PARTHUS_HW_LEDS_
#define _PARTHUS_HW_LEDS_

#include "sys_types.h"
#include "bk3000_reg.h"
#include "sys_hal_config.h"

typedef enum {
    HWled_RED = 0,
    HWled_GREEN,
    HWled_AMBER,
} HW_LED_STATUS;

__INLINE__ void HWled_On (HW_LED_STATUS s) {
    switch(s) {
    case HWled_RED:
        BK3000_GPIO_11_CONFIG = 0;
        break;
    case HWled_GREEN:
        BK3000_GPIO_8_CONFIG  = 0;
        break;
    case HWled_AMBER:
        BK3000_GPIO_8_CONFIG  = 0;
        BK3000_GPIO_11_CONFIG = 0;
        break;
    default:
        break;
    }
}

__INLINE__ void HWled_Off (HW_LED_STATUS s) {
    switch(s) {
    case HWled_RED:
        BK3000_GPIO_11_CONFIG = 1<<sft_GPIO_OUTPUT;
        break;
    case HWled_GREEN:
        BK3000_GPIO_8_CONFIG  = 1<<sft_GPIO_OUTPUT;
        break;
    case HWled_AMBER:
        BK3000_GPIO_8_CONFIG  = 1<<sft_GPIO_OUTPUT;
        BK3000_GPIO_11_CONFIG = 1<<sft_GPIO_OUTPUT;
        break;
    default:
        break;
    }
}

__INLINE__ void HWled_Toggle (HW_LED_STATUS s) {
    switch(s) {
    case HWled_RED:
        BK3000_GPIO_11_CONFIG ^= 1<<sft_GPIO_OUTPUT;
        break;
    case HWled_GREEN:
        BK3000_GPIO_8_CONFIG  ^= 1<<sft_GPIO_OUTPUT;
        break;
    case HWled_AMBER:
        BK3000_GPIO_8_CONFIG  ^= 1<<sft_GPIO_OUTPUT;
        BK3000_GPIO_11_CONFIG ^= 1<<sft_GPIO_OUTPUT;
        break;
    default:
        break;
    }
}

/* __INLINE__ void HWleds_Initialise (void) { */
/*     HWled_Off(HWled_AMBER); */
/*     HWled_On(HWled_GREEN); */
/*     /\* os_delay_ms(200); *\/ */
/*     HWled_Off(HWled_GREEN); */
/* } */

#endif
