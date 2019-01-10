
#ifndef __BES_ANKER_3163_BOARD_IO_DEF_H__
#define __BES_ANKER_3163_BOARD_IO_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "bes_hal_board.h"
#include "hal_gpadc.h"

#if HW_BOARD_SEL == HW_BOARD_ANKER_BT3163
/************************Anker_BT3162_board pin define *********************************/

#define CFG_HW_GPIOKEY_NUM (2)
#define LINEIN_SEQUENCE_SEL         LINEIN_SEQUENCE_APPLE
#define CHARGING_FULL_PIN_FULL_LVL       (0) // charging full pin state: low is full; else high is full

#define BOARD_HW_I2C_MODULE
#define BOARD_HW_SPI_MODULE
#define BOARD_HW_ADC_KEY_MODULE
#define BOARD_HW_LINEIN_VOICE_DETCT_MODULE
#define BOARD_HW_GPIO_I2C_MODULE
#define BOARD_HW_MIC_CTRL_MODULE

#ifdef BOARD_HW_MIC_CTRL_MODULE
#define MICPHONE_DEF_OPEN_WHILE_BOOTUP       (1)
#endif

/*** I2C Bus define ***/
#define BOARD_IOMUX_I2C_SCL                 HAL_IOMUX_PIN_P2_6        
#define BOARD_IOMUX_I2C_SDA                 HAL_IOMUX_PIN_P2_7

#ifdef BOARD_HW_GPIO_I2C_MODULE
#define BOARD_IOMUX_GPIO_I2C_SCL            HAL_IOMUX_PIN_P2_4
#define BOARD_IOMUX_GPIO_I2C_SDA            HAL_IOMUX_PIN_P2_5
#define BOARD_GPIO_GPIO_I2C_SCL             HAL_GPIO_PIN_P2_4
#define BOARD_GPIO_GPIO_I2C_SDA             HAL_GPIO_PIN_P2_5
#endif

#ifdef VOICE_INPUT_MODULE_CX20921
/*** Conexant CX20921 define ***/
#define BOARD_IOMUX_CX20921_WAKEUP          HAL_IOMUX_PIN_P4_2        
#define BOARD_IOMUX_CX20921_RST             HAL_IOMUX_PIN_P1_7
#define BOARD_IOMUX_CX20921_PWR             HAL_IOMUX_PIN_P1_6
#define BOARD_GPIO_CX20921_WAKEUP           HAL_GPIO_PIN_P4_2
#define BOARD_GPIO_CX20921_RST              HAL_GPIO_PIN_P1_7
#define BOARD_GPIO_CX20921_PWR              HAL_GPIO_PIN_P1_6
#endif

/*** KEY ***/
#define BOARD_IOMUX_KEY_BT_PARE             HAL_IOMUX_PIN_P1_3   
#define BOARD_IOMUX_KEY_BT_VOL_UP           HAL_IOMUX_PIN_P2_2 


/*** PA CTRL ***/
#define BOARD_IOMUX_PA_PWR                  HAL_IOMUX_PIN_P1_0        
#define BOARD_IOMUX_PA_EN                   HAL_IOMUX_PIN_P1_1
#define BOARD_IOMUX_PA_MUT                  HAL_IOMUX_PIN_P1_2
#define BOARD_GPIO_PA_PWR                   HAL_GPIO_PIN_P1_0
#define BOARD_GPIO_PA_EN                    HAL_GPIO_PIN_P1_1
#define BOARD_GPIO_PA_MUT                   HAL_GPIO_PIN_P1_2

/*** SYS POWER LOCK ***/
#define BOARD_IOMUX_SYS_PWR_LOCK            HAL_IOMUX_PIN_P0_0 
#define BOARD_GPIO_SYS_PWR_LOCK             HAL_GPIO_PIN_P0_0

/*** CHARGING CTRL ***/
#define BOARD_IOMUX_CHARGE_EN               HAL_IOMUX_PIN_P0_1 
#define BOARD_IOMUX_CHARGE_STATE            HAL_IOMUX_PIN_P0_2
#define BOARD_GPIO_CHARGE_EN                HAL_GPIO_PIN_P0_1
#define BOARD_GPIO_CHARGE_STATE             HAL_GPIO_PIN_P0_2

/*** LIGHT CTRL CHIP ONOFF CTRL ***/
#define BOARD_IOMUX_LIGHT_CHIP_CTRL         HAL_IOMUX_PIN_P1_5 
#define BOARD_GPIO_LIGHT_CHIP_CTRL          HAL_GPIO_PIN_P1_5

/*** USB 5V OUT CTRL ***/
#define BOARD_IOMUX_USB_5V_OUT_EN           HAL_IOMUX_PIN_P1_4 
#define BOARD_GPIO_USB_5V_OUT_EN            HAL_GPIO_PIN_P1_4

#define BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC  0
#if BOARD_USB5V_OUT_OVERFLOW_DETE_FUNC
#define BOARD_IOMUX_USB_5V_OUT_STATE        HAL_IOMUX_PIN_P4_1
#define BOARD_GPIO_USB_5V_OUT_STATE         HAL_GPIO_PIN_P4_1
#endif

/*** LINE IN VOICE INPUT DETECT ***/
#ifdef BOARD_HW_LINEIN_VOICE_DETCT_MODULE
#define BOARD_IOMUX_LINEIN_VOICE_DECT       HAL_IOMUX_PIN_P3_3 
#define BOARD_GPIO_LINEIN_VOICE_DECT        HAL_GPIO_PIN_P3_3
#endif

/*** LINE IN DETECT ***/
#define BOARD_IOMUX_LINEIN_DECT             HAL_IOMUX_PIN_P0_3 
#define BOARD_GPIO_LINEIN_DECT              HAL_GPIO_PIN_P0_3


/*** LED CTRL ***/
#define BOARD_IOMUX_BT_WHITE_LED             HAL_IOMUX_PIN_P4_3 
#define BOARD_GPIO_BT_WHITE_LED              HAL_GPIO_PIN_P4_3
#define BOARD_IOMUX_BT_BLUE_LED              HAL_IOMUX_PIN_P3_7 
#define BOARD_GPIO_BT_BLUE_LED               HAL_GPIO_PIN_P3_7

#define BOARD_IOMUX_ALEXA_LED                HAL_IOMUX_PIN_P3_4 
#define BOARD_GPIO_ALEXA_LED                 HAL_GPIO_PIN_P3_4

#define BOARD_IOMUX_ENV_LED                  HAL_IOMUX_PIN_P2_0 
#define BOARD_GPIO_ENV_LED                   HAL_GPIO_PIN_P2_0

#define BOARD_IOMUX_BASS_LED                 HAL_IOMUX_PIN_P3_5 
#define BOARD_GPIO_BASS_LED                  HAL_GPIO_PIN_P3_5

#ifdef BOARD_HW_MIC_CTRL_MODULE
#define BOARD_IOMUX_MIC_CTRL                 HAL_IOMUX_PIN_P2_1 
#define BOARD_GPIO_MIC_CTRL                  HAL_GPIO_PIN_P2_1
#endif

#ifdef EXTRAL_CODEC_AK7755_ENABLE
#define BOARD_IOMUX_AK7755_PWR               HAL_IOMUX_PIN_P3_2 
#define BOARD_GPIO_AK7755_PWR                HAL_GPIO_PIN_P3_2
#define BOARD_IOMUX_AK7755_RST               HAL_IOMUX_PIN_P2_3 
#define BOARD_GPIO_AK7755_RST                HAL_GPIO_PIN_P2_3
#endif

#define BOARD_IOMUX_SPI_CLK                  HAL_IOMUX_PIN_P0_5 
#define BOARD_IOMUX_SPI_CS                   HAL_IOMUX_PIN_P0_6
#define BOARD_IOMUX_SPI_OUT                  HAL_IOMUX_PIN_P0_4 
#define BOARD_IOMUX_SPI_IN                   HAL_IOMUX_PIN_P0_7

/**** adc define ****/
/*** battery voltage <--> VBATT_SNESE  bes default define ****/
/*** hardware version <---> HW_ADC0 ***/
/*** SW adc channel <---> HW adc pins 
 * SW ADC1 <---> VBATT_SNESE
 * SW ADC2 <---> HW adc0 pin
 * SW ADC3 <---> HW adc1 pin
*/
#define BOARD_HW_VERSION_DET_CH                HAL_GPADC_CHAN_BATTERY  // vbatsense
#define BOARD_HW_BATTERY_VOLT_CH               HAL_GPADC_CHAN_3  // hw adc 1 --> sw adc 3
#define BOARD_HW_ADC_KEY_CH                    HAL_GPADC_CHAN_2  // hw adc 0 --> sw adc 2

#ifdef BOARD_HW_ADC_KEY_MODULE
#define ADC_KEY_MAX_NUM                     (5)
#define BOARD_IOMUX_ADCKEY_INTR             HAL_IOMUX_PIN_P3_6   
#define BOARD_GPIO_ADCKEY_INTR              HAL_GPIO_PIN_P3_6   
#endif



typedef enum LED_3163_BOARD_TYPE_T_{
    LED_BT_WHITE,
    LED_BT_BLUE,
    LED_BT_PLAY_STATE,
    LED_ENV_STATE,
    LED_BASS_STATE,
    LED_MAX_TYPE,
}LED_3163_BOARD_TYPE;
void bes_anker_3163_led_ctrl(LED_3163_BOARD_TYPE ledType, uint8_t onoff);




#endif






#ifdef __cplusplus
}
#endif
#endif