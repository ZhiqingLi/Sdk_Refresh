
#ifndef __BES_ANKER_DEMO_BOARD_IO_DEF_H__
#define __BES_ANKER_DEMO_BOARD_IO_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "bes_hal_board.h"
#include "hal_gpadc.h"


#if HW_BOARD_SEL == HW_BOARD_ANKER_BT3163_DEMO
/************************Anker_BT3162_board pin define *********************************/

#define CFG_HW_GPIOKEY_NUM (6)
#define LINEIN_SEQUENCE_SEL         LINEIN_SEQUENCE_APPLE
#define HW_INT_CODEC_POWER_FROM_EXTRAL

#define CHARGING_FULL_PIN_FULL_LVL       (0) // charging full pin state: low is full; else high is full

#define BOARD_HW_I2C_MODULE
#define BOARD_CONEXANT_MODULE

/*** I2C Bus define ***/
#define BOARD_IOMUX_I2C_SCL                 HAL_IOMUX_PIN_P2_6        
#define BOARD_IOMUX_I2C_SDA                 HAL_IOMUX_PIN_P2_7

#ifdef VOICE_INPUT_MODULE_CX20921
/*** Conexant CX20921 define ***/
#define BOARD_IOMUX_CX20921_WAKEUP          HAL_IOMUX_PIN_P3_5        
#define BOARD_IOMUX_CX20921_RST             HAL_IOMUX_PIN_P0_0
#define BOARD_IOMUX_CX20921_PWR             HAL_IOMUX_PIN_P0_7
#define BOARD_GPIO_CX20921_WAKEUP           HAL_GPIO_PIN_P3_5
#define BOARD_GPIO_CX20921_RST              HAL_GPIO_PIN_P0_0
#define BOARD_GPIO_CX20921_PWR              HAL_GPIO_PIN_P0_7
#endif

/*** KEY ***/
#define BOARD_IOMUX_KEY_BT_PARE             HAL_IOMUX_PIN_P1_0        
#define BOARD_IOMUX_KEY_BT_OK               HAL_IOMUX_PIN_P1_1
#define BOARD_IOMUX_KEY_BT_VOL_UP           HAL_IOMUX_PIN_P1_2
#define BOARD_IOMUX_KEY_BT_VOL_DOWN         HAL_IOMUX_PIN_P1_3
#define BOARD_IOMUX_KEY_BASS_CTRL           HAL_IOMUX_PIN_P3_7
#define BOARD_IOMUX_KEY_LIGHT_CTRL          HAL_IOMUX_PIN_P4_3

/*** PA CTRL ***/
#define BOARD_IOMUX_PA_PWR                  HAL_IOMUX_PIN_P0_4        
#define BOARD_IOMUX_PA_EN                   HAL_IOMUX_PIN_P0_5
#define BOARD_IOMUX_PA_MUT                  HAL_IOMUX_PIN_P0_6
#define BOARD_GPIO_PA_PWR                   HAL_GPIO_PIN_P0_4
#define BOARD_GPIO_PA_EN                    HAL_GPIO_PIN_P0_5
#define BOARD_GPIO_PA_MUT                   HAL_GPIO_PIN_P0_6

/*** SYS POWER LOCK ***/
#define BOARD_IOMUX_SYS_PWR_LOCK            HAL_IOMUX_PIN_P0_2 
#define BOARD_GPIO_SYS_PWR_LOCK             HAL_GPIO_PIN_P0_2

/*** CHARGING CTRL ***/
#define BOARD_IOMUX_CHARGE_EN               HAL_IOMUX_PIN_P3_3 
#define BOARD_IOMUX_CHARGE_STATE            HAL_IOMUX_PIN_P2_5
#define BOARD_GPIO_CHARGE_EN                HAL_GPIO_PIN_P3_3
#define BOARD_GPIO_CHARGE_STATE             HAL_GPIO_PIN_P2_5

/*** CODEC POWER EXTRAL CTRL ***/
#define BOARD_IOMUX_CODEC_EXTRAL_PWR_CTRL   HAL_IOMUX_PIN_P2_2 
#define BOARD_GPIO_CODEC_EXTRAL_PWR_CTRL    HAL_GPIO_PIN_P2_2

/*** LIGHT CTRL CHIP ONOFF CTRL ***/
#define BOARD_IOMUX_LIGHT_CHIP_CTRL         HAL_IOMUX_PIN_P3_4 
#define BOARD_GPIO_LIGHT_CHIP_CTRL          HAL_GPIO_PIN_P3_4

/*** USB 5V OUT CTRL ***/
#define BOARD_IOMUX_USB_5V_OUT_EN           HAL_IOMUX_PIN_P0_1 
#define BOARD_GPIO_USB_5V_OUT_EN            HAL_GPIO_PIN_P0_1

#define BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC  0
#if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
#define BOARD_IOMUX_USB_5V_OUT_STATE        HAL_IOMUX_PIN_P2_3
#define BOARD_GPIO_USB_5V_OUT_STATE         HAL_GPIO_PIN_P2_3
#endif
/*** LINE IN DETECT ***/
#define BOARD_IOMUX_LINEIN_DECT             HAL_GPIO_PIN_P0_3 
#define BOARD_GPIO_LINEIN_DECT              HAL_GPIO_PIN_P0_3


/**** adc define ****/
/*** battery voltage <--> VBATT_SNESE  bes default define ****/
/*** hardware version <---> HW_ADC0 ***/
/*** SW adc channel <---> HW adc pins 
 * SW ADC1 <---> VBATT_SNESE
 * SW ADC2 <---> HW adc0 pin
 * SW ADC3 <---> HW adc1 pin
*/


#define BOARD_HW_VERSION_DET_CH                HAL_GPADC_CHAN_3         // vbatsense
#define BOARD_HW_BATTERY_VOLT_CH               HAL_GPADC_CHAN_BATTERY  // hw adc 1 --> sw adc 3
#define BOARD_HW_ADC_KEY_CH                    HAL_GPADC_CHAN_2         // hw adc 0 --> sw adc 2

#else
/************************BES_Develop_board pin define *********************************/



#endif






#ifdef __cplusplus
}
#endif
#endif